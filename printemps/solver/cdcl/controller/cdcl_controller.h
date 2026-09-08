/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
// CDCLController class definition
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_CDCL_CONTROLLER_CDCL_CONTROLLER_H_
#define PRINTEMPS_SOLVER_CDCL_CONTROLLER_CDCL_CONTROLLER_H_

#include "../../abstract_solver_controller.h"
#include "../core/cdcl_core.h"
#include "cdcl_controller_result.h"

namespace printemps::solver::cdcl::controller {

template <class T_Variable, class T_Expression>
class CDCLController
    : public AbstractSolverController<T_Variable, T_Expression> {
   private:
    CDCLControllerResult<T_Variable, T_Expression> m_result;

   public:
    /*************************************************************************/
    CDCLController(void) {
        this->initialize();
    }

    /*************************************************************************/
    CDCLController(
        model::Model<T_Variable, T_Expression>* a_model_ptr,         //
        GlobalState<T_Variable, T_Expression>*  a_global_state_ptr,  //
        const solution::SparseSolution<T_Variable, T_Expression>&
                                                    a_INITIAL_SOLUTION,  //
        const utility::TimeKeeper&                  a_TIME_KEEPER,       //
        const std::optional<std::function<bool()>>& a_CHECK_INTERRUPT,   //
        const std::function<void(
            solver::GlobalState<T_Variable, T_Expression>*)>& a_CALLBACK,  //
        const option::Option&                                 a_OPTION) {
        this->setup(a_model_ptr,         //
                    a_global_state_ptr,  //
                    a_INITIAL_SOLUTION,  //
                    a_TIME_KEEPER,       //
                    a_CHECK_INTERRUPT,   //
                    a_CALLBACK,          //
                    a_OPTION);
    }

    /*************************************************************************/
    inline void initialize(void) override {
        AbstractSolverController<T_Variable, T_Expression>::initialize();
        this->m_result.initialize();
    }

    /*************************************************************************/
    inline bool satisfy_solved_skip_condition(const bool a_IS_ENABLED_PRINT) {
        if (this->m_model_ptr->reference()
                .variable.mutable_variable_ptrs.size() == 0) {
            utility::print_warning(
                "CDCL was skipped because the problem has already been solved.",
                a_IS_ENABLED_PRINT);
            return true;
        }
        return false;
    }

    /*************************************************************************/
    inline bool satisfy_not_applicable_skip_condition(
        const bool a_IS_ENABLED_PRINT) {
        if (!this->m_model_ptr->is_all_binary_variables()) {
            utility::print_warning(
                "CDCL was skipped because it is not applicable (problem must "
                "be 0-1 integer programming).",
                a_IS_ENABLED_PRINT);
            return true;
        }

        if (this->m_model_ptr->is_monotone()) {
            utility::print_warning(
                "CDCL was skipped because a feasible solution may be found "
                "by naive search.",
                a_IS_ENABLED_PRINT);
            return true;
        }
        return false;
    }

    /*************************************************************************/
    inline bool satisfy_feasible_solution_found_skip_condition(
        const bool a_IS_ENABLED_PRINT) {
        if (this->m_global_state_ptr->incumbent_holder
                .is_found_feasible_solution()) {
            utility::print_message(
                "CDCL was skipped because a feasible solution has already been "
                "found.",
                a_IS_ENABLED_PRINT);
            return true;
        }
        return false;
    }

    /*************************************************************************/
    inline bool satisfy_interrupted_skip_condition(
        const bool a_IS_ENABLED_PRINT) {
        if (this->check_interrupt()) {
            utility::print_message("CDCL was skipped because of interruption.",
                                   a_IS_ENABLED_PRINT);
            return true;
        }
        return false;
    }

    /*************************************************************************/
    inline bool satisfy_time_over_skip_condition(
        const double a_TOTAL_ELAPSED_TIME, const bool a_IS_ENABLED_PRINT) {
        if (this->m_option.general.time_max >= 0 &&
            a_TOTAL_ELAPSED_TIME > this->m_option.general.time_max) {
            utility::print_message(
                "CDCL was skipped because of time-over (" +
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
                "CDCL was skipped because of feasible objective "
                "reaches the target limit.",
                a_IS_ENABLED_PRINT);
            return true;
        }
        return false;
    }

    /*************************************************************************/
    inline void run(void) {
        const double TOTAL_ELAPSED_TIME = this->m_time_keeper.clock();

        /**
         * Skip CDCL if already solved.
         */
        if (this->satisfy_solved_skip_condition(this->m_option.output.verbose >=
                                                option::verbose::Outer)) {
            this->m_result.initialize();
            return;
        }

        /**
         * Skip CDCL if the problem is not a 0-1 integer programming.
         */
        if (this->satisfy_not_applicable_skip_condition(
                this->m_option.output.verbose >= option::verbose::Outer)) {
            this->m_result.initialize();
            return;
        }

        /**
         * Skip CDCL if a feasible solution has already been found.
         */
        if (this->satisfy_feasible_solution_found_skip_condition(
                this->m_option.output.verbose >= option::verbose::Outer)) {
            this->m_result.initialize();
            return;
        }

        /**
         * Skip CDCL if interrupted.
         */
        if (this->satisfy_interrupted_skip_condition(
                this->m_option.output.verbose >= option::verbose::Outer)) {
            this->m_result.initialize();
            return;
        }

        /**
         * Skip CDCL if the time is over.
         */
        if (this->satisfy_time_over_skip_condition(
                TOTAL_ELAPSED_TIME,
                this->m_option.output.verbose >= option::verbose::Outer)) {
            this->m_result.initialize();
            return;
        }

        /**
         * Skip CDCL if the objective value of the feasible
         * incumbent reaches the target value.
         */
        if (this->satisfy_reach_target_skip_condition(
                this->m_option.output.verbose >= option::verbose::Outer)) {
            this->m_result.initialize();
            return;
        }

        /**
         * Prepare an option object for CDCL.
         */
        auto matrix_model =
            this->m_model_ptr->matrix_model_handler().export_matrix_model();
        option::Option option   = this->m_option;
        option.cdcl.time_offset = TOTAL_ELAPSED_TIME;

        /**
         * Run the CDCL.
         */
        core::CDCLCore cdcl_core(&matrix_model, this->m_check_interrupt,
                                 option);

        cdcl_core.run();
        auto cdcl_result = cdcl_core.result();

        /**
         * Update the incumbent and solution archives if a feasible solution
         * was found.
         */
        if (cdcl_result.termination_status ==
            core::CDCLCoreTerminationStatus::FEASIBLE) {
            this->m_model_ptr->matrix_model_handler().import_solution(
                cdcl_result.solution);
            this->m_model_ptr->updater()
                .update_dependent_variables_and_disabled_constraints();
            this->m_model_ptr->updater().update();

            const auto SCORE = this->m_model_ptr->evaluator().evaluate({});
            this->m_global_state_ptr->incumbent_holder.try_update_incumbent(
                this->m_model_ptr, SCORE);

            const auto SPARSE_SOLUTION =
                this->m_model_ptr->state_inspector().export_sparse_solution();

            if (this->m_option.output.is_enabled_store_feasible_solutions) {
                this->update_feasible_solution_archive({SPARSE_SOLUTION});
            }
            this->update_incumbent_solution_archive_and_search_tree(
                {SPARSE_SOLUTION});
        }

        /**
         * If CDCL proved the problem infeasible at level 0, propagate this
         * information to GlobalState so that subsequent solver phases can
         * be skipped.
         */
        if (cdcl_result.termination_status ==
            core::CDCLCoreTerminationStatus::INFEASIBLE) {
            this->m_global_state_ptr->termination_status =
                TerminationStatus::INFEASIBLE;
        }

        /**
         * Store the result.
         */
        this->m_result =
            CDCLControllerResult<T_Variable, T_Expression>(cdcl_result);

        /**
         * Print the search summary.
         */
        utility::print_message(
            "CDCL finished (Reason: " +
                core::CDCLCoreTerminationStatusInverseMap.at(
                    cdcl_result.termination_status) +
                ").",
            this->m_option.output.verbose >= option::verbose::Outer);

        this->m_time_keeper.clock();
        this->print_total_elapsed_time(  //
            this->m_option.output.verbose >= option::verbose::Outer);

        this->print_incumbent_summary(  //
            this->m_option.output.verbose >= option::verbose::Outer);

        this->print_dual_bound(  //
            this->m_option.output.verbose >= option::verbose::Outer);

        /**
         * Run the call-back function if specified.
         */
        this->m_callback(this->m_global_state_ptr);
    }

    /*************************************************************************/
    inline const CDCLControllerResult<T_Variable, T_Expression>& result(
        void) const {
        return this->m_result;
    }
};
}  // namespace printemps::solver::cdcl::controller
#endif
/*****************************************************************************/
// END
/*****************************************************************************/
