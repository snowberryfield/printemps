/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_LOCAL_SEARCH_CONTROLLER_LOCAL_SEARCH_CONTROLLER_H__
#define PRINTEMPS_SOLVER_LOCAL_SEARCH_CONTROLLER_LOCAL_SEARCH_CONTROLLER_H__

#include "../../abstract_solver_controller.h"
#include "../core/local_search_core.h"
#include "local_search_controller_result.h"

namespace printemps::solver::local_search::controller {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class LocalSearchController
    : public AbstractSolverController<T_Variable, T_Expression> {
   private:
    LocalSearchControllerResult<T_Variable, T_Expression> m_result;

   public:
    /*************************************************************************/
    LocalSearchController(void) {
        this->initialize();
    }

    /*************************************************************************/
    LocalSearchController(
        model::Model<T_Variable, T_Expression>*      a_model_ptr,         //
        const GlobalState<T_Variable, T_Expression>* a_global_state_ptr,  //
        const solution::SparseSolution<T_Variable, T_Expression>&
                                                    a_INITIAL_SOLUTION,  //
        const utility::TimeKeeper&                  a_TIME_KEEPER,       //
        const std::optional<std::function<bool()>>& a_CHECK_INTERRUPT,   //
        const std::function<void(
            solver::GlobalState<T_Variable, T_Expression>*)>& a_CALLBACK,  //
        const option::Option&                                 a_OPTION) {
        this->initialize();
        this->setup(a_model_ptr,         //
                    a_global_state_ptr,  //
                    a_INITIAL_SOLUTION,  //
                    a_TIME_KEEPER,       //
                    a_CHECK_INTERRUPT,   //
                    a_CALLBACK,          //
                    a_OPTION);
    }

    /*************************************************************************/
    inline void initialize(void) {
        AbstractSolverController<T_Variable, T_Expression>::initialize();
        m_result.initialize();
    }

    /*************************************************************************/
    inline bool satisfy_interrupted_skip_condition(
        const bool a_IS_ENABLED_PRINT) {
        if (this->check_interrupt()) {
            utility::print_message(  //
                "Local search was skipped because of interruption.",
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
                "Local search was skipped because of time-over (" +
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
                "Local search was skipped because of feasible objective "
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
         * Skip local search if interrupted.
         */
        if (this->satisfy_interrupted_skip_condition(
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
         * Skip local search if the objective value of the feasible
         * incumbent reaches the target value.
         */
        if (this->satisfy_reach_target_skip_condition(  //
                this->m_option.output.verbose >= option::verbose::Outer)) {
            m_result.initialize();
            return;
        }

        /**
         * Prepare an option object for local search.
         */
        option::Option option           = this->m_option;
        option.local_search.time_offset = TOTAL_ELAPSED_TIME;

        /**
         * Run the local search.
         */
        core::LocalSearchCore<T_Variable, T_Expression> local_search(
            this->m_model_ptr,         //
            this->m_global_state_ptr,  //
            this->m_initial_solution,  //
            option);

        local_search.run();

        auto local_search_result = local_search.result();

        /**
         * Update the feasible solutions archive.
         */
        if (this->m_option.output.is_enabled_store_feasible_solutions) {
            this->update_feasible_solution_archive(
                local_search.feasible_solutions());
        }

        /**
         * Update the incumbent solutions archive.
         */
        this->update_incumbent_solution_archive_and_search_tree(
            local_search.incumbent_solutions());

        /**
         * Store the result.
         */
        m_result = LocalSearchControllerResult<T_Variable, T_Expression>(
            local_search_result);

        /**
         * Print the search summary.
         */
        utility::print_message(
            "Local search finished (Reason: " +
                core::LocalSearchCoreTerminationStatusInverseMap.at(
                    local_search_result.termination_status) +
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
    inline const LocalSearchControllerResult<T_Variable, T_Expression>& result(
        void) const {
        return m_result;
    }
};
}  // namespace printemps::solver::local_search::controller

#endif
/*****************************************************************************/
// END
/*****************************************************************************/