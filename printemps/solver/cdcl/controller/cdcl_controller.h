/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
// CDCLController class definition
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_CDCL_CONTROLLER_CDCL_CONTROLLER_H_
#define PRINTEMPS_SOLVER_CDCL_CONTROLLER_CDCL_CONTROLLER_H_

#include <atomic>
#include <mutex>
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
    inline std::vector<option::Option> generate_portfolio_options(
        const option::Option& a_BASE_OPTION, const int a_NUM_THREADS) const {
        std::vector<option::Option> portfolio(a_NUM_THREADS, a_BASE_OPTION);
        for (int t = 0; t < a_NUM_THREADS; ++t) {
            portfolio[t].output.verbose = option::verbose::Off;
            portfolio[t].cdcl.seed      = a_BASE_OPTION.cdcl.seed + t;
            switch (t) {
                case 0:
                    // Thread 0: Balanced standard config (Clause minimization ON, default decay, default restart)
                    portfolio[t].cdcl.is_enabled_clause_minimization =
                        a_BASE_OPTION.cdcl.is_enabled_clause_minimization;
                    portfolio[t].cdcl.variable_activity_decay_rate =
                        a_BASE_OPTION.cdcl.variable_activity_decay_rate;
                    portfolio[t].cdcl.initial_conflict_limit =
                        a_BASE_OPTION.cdcl.initial_conflict_limit;
                    break;
                case 1:
                    // Thread 1: High-throughput raw config (Clause minimization OFF)
                    portfolio[t].cdcl.is_enabled_clause_minimization = false;
                    portfolio[t].cdcl.variable_activity_decay_rate =
                        a_BASE_OPTION.cdcl.variable_activity_decay_rate;
                    portfolio[t].cdcl.initial_conflict_limit =
                        a_BASE_OPTION.cdcl.initial_conflict_limit;
                    break;
                case 2:
                    // Thread 2: Aggressive restarts (half interval) with clause minimization ON
                    portfolio[t].cdcl.is_enabled_clause_minimization = true;
                    portfolio[t].cdcl.variable_activity_decay_rate   = 0.90;
                    portfolio[t].cdcl.initial_conflict_limit =
                        std::max(10, a_BASE_OPTION.cdcl.initial_conflict_limit / 2);
                    break;
                case 3:
                    // Thread 3: Long-window exploration (double interval) with clause minimization OFF
                    portfolio[t].cdcl.is_enabled_clause_minimization = false;
                    portfolio[t].cdcl.variable_activity_decay_rate   = 0.85;
                    portfolio[t].cdcl.initial_conflict_limit =
                        a_BASE_OPTION.cdcl.initial_conflict_limit * 2;
                    break;
                default:
                    // Thread t >= 4: Alternating clause minimization, varied decay & restarts
                    portfolio[t].cdcl.is_enabled_clause_minimization = (t % 2 == 0);
                    portfolio[t].cdcl.variable_activity_decay_rate =
                        0.75 + 0.05 * (t % 5);
                    portfolio[t].cdcl.initial_conflict_limit =
                        a_BASE_OPTION.cdcl.initial_conflict_limit * ((t % 3) + 1);
                    break;
            }
        }
        return portfolio;
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
        option::Option base_option   = this->m_option;
        base_option.cdcl.time_offset = TOTAL_ELAPSED_TIME;

        const int NUM_THREADS = [&]() -> int {
#ifdef _OPENMP
            if (!this->m_option.parallel.is_enabled_cdcl_parallelization) {
                return 1;
            }
            if (this->m_option.parallel.number_of_threads_cdcl > 0) {
                return this->m_option.parallel.number_of_threads_cdcl;
            }
            return omp_get_max_threads();
#else
            return 1;
#endif
        }();

        core::CDCLCoreResult cdcl_result;
        int                  winning_thread_id = -1;

        if (NUM_THREADS <= 1) {
            core::CDCLCore cdcl_core(&matrix_model, this->m_check_interrupt,
                                     base_option);
            cdcl_core.run();
            cdcl_result = cdcl_core.result();
        } else {
#ifdef _OPENMP
            if (this->m_option.output.verbose >= option::verbose::Outer) {
                utility::print_message(
                    "Running CDCL with OpenMP portfolio (" +
                        std::to_string(NUM_THREADS) + " threads).",
                    true);
            }

            const auto portfolio_options =
                this->generate_portfolio_options(base_option, NUM_THREADS);

            std::atomic<bool> is_interrupted_by_solution{false};
            std::mutex        result_mutex;
            bool              has_solution = false;
            std::vector<core::CDCLCoreResult> thread_results(NUM_THREADS);

            #pragma omp parallel num_threads(NUM_THREADS)
            {
                const int THREAD_ID = omp_get_thread_num();

                auto thread_check_interrupt = [&]() -> bool {
                    if (is_interrupted_by_solution.load(
                            std::memory_order_relaxed)) {
                        return true;
                    }
                    return this->check_interrupt();
                };

                core::CDCLCore cdcl_core(
                    &matrix_model, thread_check_interrupt,
                    portfolio_options[THREAD_ID]);
                cdcl_core.run();

                auto res = cdcl_core.result();
                thread_results[THREAD_ID] = res;

                if (res.termination_status ==
                        core::CDCLCoreTerminationStatus::FEASIBLE ||
                    res.termination_status ==
                        core::CDCLCoreTerminationStatus::INFEASIBLE) {
                    is_interrupted_by_solution.store(true,
                                                     std::memory_order_relaxed);
                    std::lock_guard<std::mutex> lock(result_mutex);
                    if (!has_solution || res.termination_status ==
                                             core::CDCLCoreTerminationStatus::
                                                 FEASIBLE) {
                        cdcl_result       = res;
                        has_solution      = true;
                        winning_thread_id = THREAD_ID;
                    }
                }
            }

            if (!has_solution) {
                cdcl_result = thread_results[0];
                for (int t = 1; t < NUM_THREADS; ++t) {
                    if (thread_results[t].number_of_conflicts >
                        cdcl_result.number_of_conflicts) {
                        cdcl_result = thread_results[t];
                    }
                }
            }
#else
            core::CDCLCore cdcl_core(&matrix_model, this->m_check_interrupt,
                                     base_option);
            cdcl_core.run();
            cdcl_result = cdcl_core.result();
#endif
        }

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

        if (NUM_THREADS > 1 && winning_thread_id >= 0) {
            utility::print_info(
                " -- Solved by thread " + std::to_string(winning_thread_id) +
                    " (" + std::to_string(cdcl_result.number_of_conflicts) +
                    " conflicts, " +
                    std::to_string(cdcl_result.number_of_decisions) +
                    " decisions).",
                this->m_option.output.verbose >= option::verbose::Outer);
        }

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
