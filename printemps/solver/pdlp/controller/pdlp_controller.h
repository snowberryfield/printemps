/*****************************************************************************/
// Copyright (c) 2020-2023 Yuji KOGUMA
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
    model::Model<T_Variable, T_Expression>*           m_model_ptr;
    solution::DenseSolution<T_Variable, T_Expression> m_initial_solution;
    utility::TimeKeeper                               m_time_keeper;
    option::Option                                    m_option;
    PDLPControllerResult                              m_result;

    /*************************************************************************/
    inline void print_total_elapsed_time(const double a_TOTAL_ELAPSED_TIME,
                                         const bool   a_IS_ENABLED_PRINT) {
        utility::print_info(  //
            " -- Total elapsed time: " +
                utility::to_string(a_TOTAL_ELAPSED_TIME, "%.3f") + "sec",
            a_IS_ENABLED_PRINT);
    }

   public:
    /*************************************************************************/
    PDLPController(void) {
        this->initialize();
    }

    /*************************************************************************/
    PDLPController(model::Model<T_Variable, T_Expression>* a_model_ptr,  //
                   const solution::DenseSolution<T_Variable, T_Expression>&
                                              a_INITIAL_SOLUTION,  //
                   const utility::TimeKeeper& a_TIME_KEEPER,       //
                   const option::Option&      a_OPTION) {
        this->initialize();
        this->setup(a_model_ptr, a_INITIAL_SOLUTION, a_TIME_KEEPER, a_OPTION);
    }

    /*************************************************************************/
    virtual ~PDLPController(void) {
        /// nothing to do
    }

    /*************************************************************************/
    inline void initialize(void) {
        m_model_ptr = nullptr;
        m_initial_solution.initialize();
        m_time_keeper.initialize();
        m_option.initialize();
        m_result.initialize();
    }

    /*************************************************************************/
    inline void setup(                                        //
        model::Model<T_Variable, T_Expression>* a_model_ptr,  //
        const solution::DenseSolution<T_Variable, T_Expression>&
                                   a_INITIAL_SOLUTION,  //
        const utility::TimeKeeper& a_TIME_KEEPER,       //
        const option::Option&      a_OPTION) {
        m_model_ptr        = a_model_ptr;
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
         * Skip local search if the problem is not linear.
         */
        if (this->satisfy_not_linear_skip_condition(  //
                this->m_option.output.verbose >= option::verbose::Outer)) {
            m_result.initialize();
            return;
        }

        /**
         * Skip local search if the time is over.
         */
        if (this->satisfy_time_over_skip_condition(
                TOTAL_ELAPSED_TIME,
                this->m_option.output.verbose >= option::verbose::Outer)) {
            m_result.initialize();
            return;
        }

        /**
         * Prepare an option object for local search.
         */
        option::Option option   = this->m_option;
        option.pdlp.time_offset = TOTAL_ELAPSED_TIME;

        /**
         * Run the PDLP
         */
        m_model_ptr->import_variable_values(
            m_initial_solution.variable_value_proxies);
        m_model_ptr->update();
        auto lp_instance = m_model_ptr->export_lp_instance();
        lp_instance.scaling(m_option.pdlp.counts_of_ruiz_scaling,
                            m_option.pdlp.is_enabled_pock_chambolle_scaling);

        pdlp::core::PDLPCore pdlp_core(&lp_instance, option);
        pdlp_core.run();

        auto pdlp_result = pdlp_core.result();

        /**
         * Store the result.
         */
        m_result = PDLPControllerResult(pdlp_result);

        /**
         * Print the search summary.
         */
        utility::print_message(
            "PDLP finished (Reason: " +
                core::PDLPCoreTerminationStatusInverseMap.at(
                    pdlp_result.termination_status) +
                ").",
            this->m_option.output.verbose >= option::verbose::Outer);

        this->print_total_elapsed_time(  //
            this->m_time_keeper.clock(),
            this->m_option.output.verbose >= option::verbose::Outer);
    }

    /*************************************************************************/
    inline constexpr const PDLPControllerResult result(void) const {
        return m_result;
    }
};
}  // namespace printemps::solver::pdlp::controller

#endif
/*****************************************************************************/
// END
/*****************************************************************************/