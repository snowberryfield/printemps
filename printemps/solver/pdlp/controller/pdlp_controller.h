/*****************************************************************************/
// Copyright (c) 2020-2024 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_PDLP_CONTROLLER_PDLP_CONTROLLER__H__
#define PRINTEMPS_SOLVER_PDLP_CONTROLLER_PDLP_CONTROLLER__H__

#include "../../abstract_solver_controller.h"
#include "../core/pdlp_core.h"
#include "pdlp_controller_result.h"

namespace printemps::solver::pdlp::controller {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class PDLPController {
   private:
    model::Model<T_Variable, T_Expression>*            m_model_ptr;
    GlobalState<T_Variable, T_Expression>*             m_global_state_ptr;
    solution::SparseSolution<T_Variable, T_Expression> m_initial_solution;
    utility::TimeKeeper                                m_time_keeper;
    option::Option                                     m_option;

    PDLPControllerResult m_result;

    /*************************************************************************/
    inline void print_total_elapsed_time(const bool a_IS_ENABLED_PRINT) {
        utility::print_info(  //
            " -- Total elapsed time: " +
                utility::to_string(m_time_keeper.elapsed_time(), "%.3f") +
                "sec",
            a_IS_ENABLED_PRINT);
    }

    /*************************************************************************/
    inline void print_dual_bound(const bool a_IS_ENABLED_PRINT) {
        utility::print_info(
            " -- Dual Bound: " +
                utility::to_string(
                    m_global_state_ptr->incumbent_holder.dual_bound(), "%.5e"),
            a_IS_ENABLED_PRINT);
    }

   public:
    /*************************************************************************/
    PDLPController(void) {
        this->initialize();
    }

    /*************************************************************************/
    PDLPController(model::Model<T_Variable, T_Expression>* a_model_ptr,  //
                   const solution::SparseSolution<T_Variable, T_Expression>&
                                                          a_INITIAL_SOLUTION,  //
                   const utility::TimeKeeper&             a_TIME_KEEPER,  //
                   const option::Option&                  a_OPTION,       //
                   GlobalState<T_Variable, T_Expression>* a_global_state_ptr) {
        this->initialize();
        this->setup(a_model_ptr, a_INITIAL_SOLUTION, a_TIME_KEEPER, a_OPTION,
                    a_global_state_ptr);
    }

    /*************************************************************************/
    inline void initialize(void) {
        m_model_ptr        = nullptr;
        m_global_state_ptr = nullptr;
        m_initial_solution.initialize();
        m_time_keeper.initialize();
        m_option.initialize();
        m_result.initialize();
    }

    /*************************************************************************/
    inline void setup(                                               //
        model::Model<T_Variable, T_Expression>* a_model_ptr,         //
        GlobalState<T_Variable, T_Expression>*  a_global_state_ptr,  //
        const solution::SparseSolution<T_Variable, T_Expression>&
                                   a_INITIAL_SOLUTION,  //
        const utility::TimeKeeper& a_TIME_KEEPER,       //
        const option::Option&      a_OPTION) {
        m_model_ptr        = a_model_ptr;
        m_global_state_ptr = a_global_state_ptr;
        m_initial_solution = a_INITIAL_SOLUTION;
        m_time_keeper      = a_TIME_KEEPER;
        m_option           = a_OPTION;
    }

    /*************************************************************************/
    inline bool satisfy_not_linear_skip_condition(
        const bool a_IS_ENABLED_PRINT) {
        if (!this->m_model_ptr->is_linear()) {
            utility::print_warning(
                "PDLP was skipped because the problem is not linear.",
                a_IS_ENABLED_PRINT);
            return true;
        }
        return false;
    }

    /*************************************************************************/
    inline bool satisfy_solved_skip_condition(const bool a_IS_ENABLED_PRINT) {
        if (this->m_model_ptr->variable_reference()
                .mutable_variable_ptrs.size() == 0) {
            utility::print_warning(
                "PDLP was skipped because the problem has already been solved.",
                a_IS_ENABLED_PRINT);
            return true;
        }
        return false;
    }

    /*************************************************************************/
    inline bool satisfy_nonsense_skip_condition(
        const linear_programming::LinearProgramming& a_LP_INSTANCE,
        const bool                                   a_IS_ENABLED_PRINT) {
        if (a_LP_INSTANCE.number_of_columns <= 1 ||
            a_LP_INSTANCE.number_of_rows <= 1) {
            utility::print_warning(
                "PDLP was skipped because the problem does not make sense.",
                a_IS_ENABLED_PRINT);
            return true;
        }
        return false;
    }

    /*************************************************************************/
    inline bool satisfy_time_over_skip_condition(
        const double a_TOTAL_ELAPSED_TIME, const bool a_IS_ENABLED_PRINT) {
        if (a_TOTAL_ELAPSED_TIME > this->m_option.general.time_max) {
            utility::print_message(
                "PDLP was skipped because of time-over (" +
                    utility::to_string(a_TOTAL_ELAPSED_TIME, "%.3f") + "sec).",
                a_IS_ENABLED_PRINT);
            return true;
        }
        return false;
    }

    /*************************************************************************/
    inline void run(void) {
        const double TOTAL_ELAPSED_TIME = this->m_time_keeper.clock();
        /**
         * Skip PDLP if the problem is not linear.
         */
        if (this->satisfy_not_linear_skip_condition(  //
                this->m_option.output.verbose >= option::verbose::Outer)) {
            m_result.initialize();
            return;
        }

        /**
         * Skip PDLP if the problem has already been solved.
         */
        if (this->satisfy_solved_skip_condition(  //
                this->m_option.output.verbose >= option::verbose::Outer)) {
            m_result.initialize();
            return;
        }

        /**
         * Skip PDLP if the time is over.
         */
        if (this->satisfy_time_over_skip_condition(
                TOTAL_ELAPSED_TIME,
                this->m_option.output.verbose >= option::verbose::Outer)) {
            m_result.initialize();
            return;
        }

        /**
         * Prepare an option object for PDLP.
         */
        option::Option option   = this->m_option;
        option.pdlp.time_offset = TOTAL_ELAPSED_TIME;

        /**
         * Run the PDLP
         */
        m_model_ptr->import_solution(m_initial_solution);
        m_model_ptr->update();
        auto lp_instance = m_model_ptr->export_lp_instance();

        /**
         * Skip PDLP if the problem does not make sense.
         */
        if (this->satisfy_nonsense_skip_condition(
                lp_instance,
                this->m_option.output.verbose >= option::verbose::Outer)) {
            m_result.initialize();
            return;
        }

        lp_instance.scaling(m_option.pdlp.counts_of_ruiz_scaling,
                            m_option.pdlp.is_enabled_pock_chambolle_scaling);

        pdlp::core::PDLPCore pdlp_core(&lp_instance, option);
        pdlp_core.run();

        auto pdlp_result = pdlp_core.result();
        pdlp_result.scaling(
            lp_instance.primal_constraint_coefficients.row_scaler,
            lp_instance.primal_constraint_coefficients.column_scaler);

        /**
         * Store the result.
         */
        m_result = PDLPControllerResult(pdlp_result);

        /**
         * Update the dual bound.
         */
        if (m_result.core.dual.relative_violation_norm <
            m_option.pdlp.tolerance) {
            const auto DUAL_BOUND = m_result.core.dual.objective;
            if (m_model_ptr->is_minimization()) {
                if (DUAL_BOUND >
                    m_global_state_ptr->incumbent_holder.dual_bound()) {
                    m_global_state_ptr->incumbent_holder.update_dual_bound(
                        DUAL_BOUND);
                }
            } else {
                if (-DUAL_BOUND <
                    m_global_state_ptr->incumbent_holder.dual_bound()) {
                    m_global_state_ptr->incumbent_holder.update_dual_bound(
                        -DUAL_BOUND);
                }
            }
        }

        /**
         * Print the search summary.
         */
        utility::print_message(
            "PDLP finished (Reason: " +
                core::PDLPCoreTerminationStatusInverseMap.at(
                    pdlp_result.termination_status) +
                ").",
            this->m_option.output.verbose >= option::verbose::Outer);

        this->m_time_keeper.clock();
        this->print_total_elapsed_time(  //
            this->m_option.output.verbose >= option::verbose::Outer);

        this->print_dual_bound(  //
            this->m_option.output.verbose >= option::verbose::Outer);
    }

    /*************************************************************************/
    inline const PDLPControllerResult& result(void) const {
        return m_result;
    }
};
}  // namespace printemps::solver::pdlp::controller

#endif
/*****************************************************************************/
// END
/*****************************************************************************/