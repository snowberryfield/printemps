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
    inline std::vector<option::Option> generate_portfolio_options(
        const option::Option& a_BASE_OPTION,
        const int             a_NUMBER_OF_THREADS) const {
        std::vector<option::Option> portfolio(a_NUMBER_OF_THREADS,
                                              a_BASE_OPTION);
        for (int t = 0; t < a_NUMBER_OF_THREADS; ++t) {
            portfolio[t].output.verbose = option::verbose::Off;
            portfolio[t].cdcl.seed      = a_BASE_OPTION.cdcl.seed + t;

            if (t == 0) {
                // Thread 0: Baseline thread using base options directly
                continue;
            }

            // Worker threads t >= 1: Diversify randomness parameters
            portfolio[t].cdcl.random_variable_selection_rate =
                (a_BASE_OPTION.cdcl.random_variable_selection_rate > 0.0)
                    ? a_BASE_OPTION.cdcl.random_variable_selection_rate
                    : (0.01 + 0.005 * (t % 4));

            portfolio[t].cdcl.initial_activity_noise_ratio =
                (a_BASE_OPTION.cdcl.initial_activity_noise_ratio > 0.0)
                    ? a_BASE_OPTION.cdcl.initial_activity_noise_ratio
                    : (0.01 * ((t % 5) + 1));

            portfolio[t].cdcl.initial_phase_perturbation_rate =
                (a_BASE_OPTION.cdcl.initial_phase_perturbation_rate > 0.0)
                    ? a_BASE_OPTION.cdcl.initial_phase_perturbation_rate
                    : (0.05 * (t % 5));
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

        const int NUMBER_OF_THREADS = [&]() -> int {
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

        core::CDCLCoreResult              cdcl_result;
        int                               winning_thread_id = -1;
        std::vector<core::CDCLCoreResult> thread_results;

        if (NUMBER_OF_THREADS <= 1) {
            core::CDCLCore cdcl_core(&matrix_model, this->m_check_interrupt,
                                     base_option);
            cdcl_core.run();
            cdcl_result       = cdcl_core.result();
            thread_results    = {cdcl_result};
            winning_thread_id = 0;
        } else {
#ifdef _OPENMP
            if (this->m_option.output.verbose >= option::verbose::Outer) {
                utility::print_message("Running CDCL with OpenMP portfolio (" +
                                           std::to_string(NUMBER_OF_THREADS) +
                                           " threads).",
                                       true);
            }

            const auto portfolio_options = this->generate_portfolio_options(
                base_option, NUMBER_OF_THREADS);

            std::atomic<bool> is_interrupted_by_solution{false};
            std::mutex        result_mutex;
            bool              has_solution = false;
            thread_results.resize(NUMBER_OF_THREADS);

#pragma omp parallel num_threads(NUMBER_OF_THREADS)
            {
                const int THREAD_ID = omp_get_thread_num();

                auto thread_check_interrupt = [&]() -> bool {
                    if (is_interrupted_by_solution.load(
                            std::memory_order_relaxed)) {
                        return true;
                    }
                    return this->check_interrupt();
                };

                core::CDCLCore cdcl_core(&matrix_model, thread_check_interrupt,
                                         portfolio_options[THREAD_ID]);
                cdcl_core.run();

                auto result               = cdcl_core.result();
                thread_results[THREAD_ID] = result;

                if (result.termination_status ==
                        core::CDCLCoreTerminationStatus::FEASIBLE ||
                    result.termination_status ==
                        core::CDCLCoreTerminationStatus::INFEASIBLE) {
                    is_interrupted_by_solution.store(true,
                                                     std::memory_order_relaxed);
                    std::lock_guard<std::mutex> lock(result_mutex);
                    if (!has_solution ||
                        result.termination_status ==
                            core::CDCLCoreTerminationStatus::FEASIBLE) {
                        cdcl_result       = result;
                        has_solution      = true;
                        winning_thread_id = THREAD_ID;
                    }
                }
            }

            if (!has_solution) {
                cdcl_result       = thread_results[0];
                winning_thread_id = 0;
                if (this->m_option.cdcl.is_enabled_evaluate_full_solutions) {
                    for (int i = 1; i < NUMBER_OF_THREADS; ++i) {
                        if (thread_results[i].best_total_violation <
                            cdcl_result.best_total_violation) {
                            cdcl_result       = thread_results[i];
                            winning_thread_id = i;
                        }
                    }
                } else {
                    for (int i = 1; i < NUMBER_OF_THREADS; ++i) {
                        if (thread_results[i].number_of_conflicts >
                            cdcl_result.number_of_conflicts) {
                            cdcl_result       = thread_results[i];
                            winning_thread_id = i;
                        }
                    }
                }
            }
#else
            core::CDCLCore cdcl_core(&matrix_model, this->m_check_interrupt,
                                     base_option);
            cdcl_core.run();
            cdcl_result       = cdcl_core.result();
            thread_results    = {cdcl_result};
            winning_thread_id = 0;
#endif
        }

        /**
         * Update the incumbent and solution archives:
         * - If a feasible solution was found, import and evaluate it.
         * - If evaluate_full_solutions is enabled, import the best full
         *   solution (smallest constraint violation).
         */
        const bool HAS_FEASIBLE_SOLUTION =
            (cdcl_result.termination_status ==
             core::CDCLCoreTerminationStatus::FEASIBLE);

        const auto& candidate_solution =
            HAS_FEASIBLE_SOLUTION
                ? cdcl_result.solution
                : (this->m_option.cdcl.is_enabled_evaluate_full_solutions
                       ? cdcl_result.best_solution
                       : std::vector<int>{});

        if (!candidate_solution.empty()) {
            this->m_model_ptr->matrix_model_handler().import_solution(
                candidate_solution);

            for (auto&& selection : this->m_model_ptr->selections()) {
                for (auto* var_ptr : selection.variable_ptrs) {
                    if (var_ptr->value() == 1) {
                        var_ptr->select();
                        break;
                    }
                }
            }

            this->m_model_ptr->updater()
                .update_dependent_variables_and_disabled_constraints();
            this->m_model_ptr->updater().update();

            const auto SCORE = this->m_model_ptr->evaluator().evaluate({});
            this->m_global_state_ptr->incumbent_holder.try_update_incumbent(
                this->m_model_ptr, SCORE);

            const auto SPARSE_SOLUTION =
                this->m_model_ptr->state_exporter().export_sparse_solution();

            if (SCORE.is_feasible) {
                if (this->m_option.output.is_enabled_store_feasible_solutions) {
                    this->update_feasible_solution_archive({SPARSE_SOLUTION});
                }
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

        if (NUMBER_OF_THREADS > 1) {
            const bool IS_ENABLED_PRINT =
                this->m_option.output.verbose >= option::verbose::Outer;

            utility::print(
                "---------+-------+-------------------+---------+---------+----"
                "----+-------------",
                IS_ENABLED_PRINT);
            utility::print(
                "  Thread |  Seed | Status            |  Confl. |  Decis. |  "
                "Rest. |  Best Viol. ",
                IS_ENABLED_PRINT);
            utility::print(
                "---------+-------+-------------------+---------+---------+----"
                "----+-------------",
                IS_ENABLED_PRINT);

            for (int t = 0; t < NUMBER_OF_THREADS; ++t) {
                const auto& res = thread_results[t];
                std::string status_str =
                    core::CDCLCoreTerminationStatusInverseMap.at(
                        res.termination_status);

                std::string viol_str = "-";
                if (res.termination_status ==
                    core::CDCLCoreTerminationStatus::FEASIBLE) {
                    viol_str = "0.00000e+00";
                } else if (this->m_option.cdcl
                               .is_enabled_evaluate_full_solutions &&
                           !std::isinf(res.best_total_violation)) {
                    viol_str =
                        utility::to_string(res.best_total_violation, "%.5e");
                }

                char row[128];
                std::snprintf(row, sizeof(row),
                              "%2s %5d | %5d | %-17s | %7d | %7d | %6d | %11s ",
                              (t == winning_thread_id ? " *" : "  "),  //
                              t,                                       //
                              base_option.cdcl.seed + t,               //
                              status_str.c_str(),                      //
                              res.number_of_conflicts,                 //
                              res.number_of_decisions,                 //
                              res.number_of_restarts,                  //
                              viol_str.c_str());
                utility::print(std::string(row), IS_ENABLED_PRINT);
            }

            utility::print(
                "---------+-------+-------------------+---------+---------+----"
                "----+-------------",
                IS_ENABLED_PRINT);
        }

        if (NUMBER_OF_THREADS > 1 && winning_thread_id >= 0) {
            if (cdcl_result.termination_status ==
                    core::CDCLCoreTerminationStatus::FEASIBLE ||
                cdcl_result.termination_status ==
                    core::CDCLCoreTerminationStatus::INFEASIBLE) {
                utility::print_info(
                    " -- Solved by thread " +
                        std::to_string(winning_thread_id) + " (" +
                        std::to_string(cdcl_result.number_of_conflicts) +
                        " conflicts, " +
                        std::to_string(cdcl_result.number_of_decisions) +
                        " decisions).",
                    this->m_option.output.verbose >= option::verbose::Outer);
            } else if (this->m_option.cdcl.is_enabled_evaluate_full_solutions &&
                       !std::isinf(cdcl_result.best_total_violation)) {
                utility::print_info(
                    " -- Best full solution from thread " +
                        std::to_string(winning_thread_id) + " (violation: " +
                        utility::to_string(cdcl_result.best_total_violation,
                                           "%.5e") +
                        ", " + std::to_string(cdcl_result.number_of_conflicts) +
                        " conflicts).",
                    this->m_option.output.verbose >= option::verbose::Outer);
            }
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
