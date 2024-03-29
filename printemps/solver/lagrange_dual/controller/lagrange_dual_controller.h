/*****************************************************************************/
// Copyright (c) 2020-2023 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_LAGRANGE_DUAL_CONTROLLER_LAGRANGE_DUAL_CONTROLLER_H__
#define PRINTEMPS_SOLVER_LAGRANGE_DUAL_CONTROLLER_LAGRANGE_DUAL_CONTROLLER_H__

#include "../../abstract_solver_controller.h"
#include "../core/lagrange_dual_core.h"
#include "lagrange_dual_controller_result.h"

namespace printemps::solver::lagrange_dual::controller {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class LagrangeDualController
    : public AbstractSolverController<T_Variable, T_Expression> {
   private:
    LagrangeDualControllerResult<T_Variable, T_Expression> m_result;

   public:
    /*************************************************************************/
    LagrangeDualController(void) {
        this->initialize();
    }

    /*************************************************************************/
    LagrangeDualController(
        model::Model<T_Variable, T_Expression>* a_model_ptr,         //
        GlobalState<T_Variable, T_Expression>*  a_global_state_ptr,  //
        const solution::SparseSolution<T_Variable, T_Expression>&
                                   a_INITIAL_SOLUTION,  //
        const utility::TimeKeeper& a_TIME_KEEPER,       //
        const option::Option&      a_OPTION) {
        this->initialize();
        this->setup(a_model_ptr,         //
                    a_global_state_ptr,  //
                    a_INITIAL_SOLUTION,  //
                    a_TIME_KEEPER,       //
                    a_OPTION);
    }

    /*************************************************************************/
    inline void initialize(void) {
        AbstractSolverController<T_Variable, T_Expression>::initialize();
        m_result.initialize();
    }

    /*************************************************************************/
    inline bool satisfy_not_linear_skip_condition(
        const bool a_IS_ENABLED_PRINT) {
        if (!this->m_model_ptr->is_linear()) {
            utility::print_warning(
                "Solving Lagrange dual was skipped because the problem is not "
                "linear.",
                a_IS_ENABLED_PRINT);
            return true;
        }
        return false;
    }

    /*************************************************************************/
    inline bool satisfy_not_applicable_skip_condition(
        const bool a_IS_ENABLED_PRINT) {
        if (this->m_model_ptr->number_of_selection_variables() > 0 ||
            this->m_model_ptr->number_of_dependent_binary_variables() > 0 ||
            this->m_model_ptr->number_of_dependent_integer_variables() > 0) {
            utility::print_warning(
                "Solving lagrange dual was skipped because it is not "
                "applicable to problems which include selection variables or "
                "dependent variables.",
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
                "Solving Lagrange dual was skipped because of time-over (" +
                    utility::to_string(a_TOTAL_ELAPSED_TIME, "%.3f") + "sec).",
                a_IS_ENABLED_PRINT);
            return true;
        }
        return false;
    }

    /*************************************************************************/
    inline bool satisfy_reach_target_skip_condition(
        const bool a_IS_ENABLED_PRINT) {
        if (this->m_global_state_ptr->incumbent_holder
                .feasible_incumbent_objective() <=
            this->m_option.general.target_objective_value) {
            utility::print_message(
                "Solving Lagrange dual was skipped because of feasible "
                "objective reaches the target limit.",
                a_IS_ENABLED_PRINT);
            return true;
        }
        return false;
    }

    /*************************************************************************/
    inline void run(void) {
        const double TOTAL_ELAPSED_TIME = this->m_time_keeper.clock();
        /**
         * Skip Lagrange dual if the problem is not linear.
         */
        if (this->satisfy_not_linear_skip_condition(  //
                this->m_option.output.verbose >= option::verbose::Outer)) {
            m_result.initialize();
            return;
        }

        /**
         * Skip Lagrange dual if it is not applicable.
         */
        if (this->satisfy_not_applicable_skip_condition(  //
                this->m_option.output.verbose >= option::verbose::Outer)) {
            m_result.initialize();
            return;
        }

        /**
         * Skip Lagrange dual if the time is over.
         */
        if (this->satisfy_time_over_skip_condition(
                TOTAL_ELAPSED_TIME,
                this->m_option.output.verbose >= option::verbose::Outer)) {
            m_result.initialize();
            return;
        }

        /**
         * Skip Lagrange dual if the objective value of the feasible
         * incumbent reaches the target value.
         */
        if (this->satisfy_reach_target_skip_condition(  //
                this->m_option.output.verbose >= option::verbose::Outer)) {
            m_result.initialize();
            return;
        }

        /**
         * Prepare an option object for lagrange dual search.
         */
        option::Option option            = this->m_option;
        option.lagrange_dual.time_offset = TOTAL_ELAPSED_TIME;

        /**
         * Run the lagrange dual search.
         */
        core::LagrangeDualCore<T_Variable, T_Expression> lagrange_dual(
            this->m_model_ptr,         //
            this->m_global_state_ptr,  //
            this->m_initial_solution,  //
            option);

        lagrange_dual.run();

        auto lagrange_dual_result = lagrange_dual.result();

        /**
         * Update the feasible solutions archive.
         */
        if (this->m_option.output.is_enabled_store_feasible_solutions) {
            this->update_feasible_solution_archive(
                lagrange_dual.feasible_solutions());
        }

        /**
         * Update the incumbent solutions archive.
         */
        this->update_incumbent_solution_archive_and_search_tree(
            lagrange_dual.incumbent_solutions());

        /**
         * Store the result.
         */
        m_result = LagrangeDualControllerResult<T_Variable, T_Expression>(
            lagrange_dual_result);

        /**
         * Update the dual bound.
         */
        const auto DUAL_BOUND = m_result.core.lagrangian;
        if (this->m_model_ptr->is_minimization()) {
            if (DUAL_BOUND >
                this->m_global_state_ptr->incumbent_holder.dual_bound()) {
                this->m_global_state_ptr->incumbent_holder.update_dual_bound(
                    DUAL_BOUND);
            }
        } else {
            if (-DUAL_BOUND <
                this->m_global_state_ptr->incumbent_holder.dual_bound()) {
                this->m_global_state_ptr->incumbent_holder.update_dual_bound(
                    -DUAL_BOUND);
            }
        }

        /**
         * Print the search summary.
         */
        utility::print_message(
            "Solving Lagrange dual finished (Reason: " +
                core::LagrangeDualCoreTerminationStatusInverseMap.at(
                    lagrange_dual_result.termination_status) +
                ").",
            this->m_option.output.verbose >= option::verbose::Outer);

        this->m_time_keeper.clock();
        this->print_total_elapsed_time(  //
            this->m_option.output.verbose >= option::verbose::Outer);

        this->print_incumbent_summary(  //
            this->m_option.output.verbose >= option::verbose::Outer);

        this->print_dual_bound(  //
            this->m_option.output.verbose >= option::verbose::Outer);
    }

    /*************************************************************************/
    inline const LagrangeDualControllerResult<T_Variable, T_Expression>& result(
        void) const {
        return m_result;
    }
};
}  // namespace printemps::solver::lagrange_dual::controller

#endif
/*****************************************************************************/
// END
/*****************************************************************************/