/*****************************************************************************/
// Copyright (c) 2020-2026 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
// CDCLController class definition
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_CDCL_CONTROLLER_CDCL_CONTROLLER_H__
#define PRINTEMPS_SOLVER_CDCL_CONTROLLER_CDCL_CONTROLLER_H__

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
        m_result.initialize();
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
                "CDCL was skipped because the objective value of the "
                "feasible incumbent reached the target value.",
                a_IS_ENABLED_PRINT);
            return true;
        }
        return false;
    }

    /*************************************************************************/
    inline double compute_portfolio_feedback_rate(
        const int    a_THREAD_INDEX,
        const double a_MAX_FEEDBACK_RATE = 0.90) const {
        if (a_THREAD_INDEX <= 0) {
            return 0.0;
        }
        if (a_THREAD_INDEX == 1) {
            return a_MAX_FEEDBACK_RATE;
        }
        // Van der Corput sequence (base 2) inverted for high-value
        // prioritization
        uint64_t thread_number      = static_cast<uint64_t>(a_THREAD_INDEX - 1);
        double   quasi_random_value = 0.0;
        double   base_weight        = 0.5;
        while (thread_number > 0) {
            if (thread_number & 1) {
                quasi_random_value += base_weight;
            }
            base_weight *= 0.5;
            thread_number >>= 1;
        }
        return a_MAX_FEEDBACK_RATE * (1.0 - quasi_random_value);
    }

    /*************************************************************************/
    inline std::vector<option::Option> generate_portfolio_options(
        const option::Option& a_BASE_OPTION,
        const int             a_NUMBER_OF_THREADS) const {
        std::vector<option::Option> portfolio(a_NUMBER_OF_THREADS,
                                              a_BASE_OPTION);

        for (int i = 0; i < a_NUMBER_OF_THREADS; i++) {
            portfolio[i].output.verbose = (a_NUMBER_OF_THREADS == 1)
                                              ? a_BASE_OPTION.output.verbose
                                              : option::verbose::Off;
            portfolio[i].cdcl.seed      = a_BASE_OPTION.cdcl.seed + i;

            // Disable random perturbations (differentiate portfolio purely by
            // feedback rate)
            portfolio[i].cdcl.random_variable_selection_rate  = 0.0;
            portfolio[i].cdcl.initial_activity_noise_ratio    = 0.0;
            portfolio[i].cdcl.initial_phase_perturbation_rate = 0.0;

            if (a_NUMBER_OF_THREADS == 1) {
                // Use baseline value for single-thread execution
                portfolio[i].cdcl.variable_activity_feedback_rate =
                    a_BASE_OPTION.cdcl.variable_activity_feedback_rate;
            } else {
                portfolio[i].cdcl.variable_activity_feedback_rate =
                    this->compute_portfolio_feedback_rate(i);
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
            m_result.initialize();
            return;
        }

        /**
         * Skip CDCL if the problem is not a 0-1 integer programming.
         */
        if (this->satisfy_not_applicable_skip_condition(
                this->m_option.output.verbose >= option::verbose::Outer)) {
            m_result.initialize();
            return;
        }

        /**
         * Skip CDCL if a feasible solution has already been found.
         */
        if (this->satisfy_feasible_solution_found_skip_condition(
                this->m_option.output.verbose >= option::verbose::Outer)) {
            m_result.initialize();
            return;
        }

        /**
         * Skip CDCL if interrupted.
         */
        if (this->satisfy_interrupted_skip_condition(
                this->m_option.output.verbose >= option::verbose::Outer)) {
            m_result.initialize();
            return;
        }

        /**
         * Skip CDCL if the time is over.
         */
        if (this->satisfy_time_over_skip_condition(
                TOTAL_ELAPSED_TIME,
                this->m_option.output.verbose >= option::verbose::Outer)) {
            m_result.initialize();
            return;
        }

        /**
         * Skip CDCL if the objective value of the feasible
         * incumbent reaches the target value.
         */
        if (this->satisfy_reach_target_skip_condition(
                this->m_option.output.verbose >= option::verbose::Outer)) {
            m_result.initialize();
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
            const int MAX_THREADS = omp_get_max_threads();
            if (this->m_option.parallel.number_of_threads_cdcl > 0) {
                return std::min(this->m_option.parallel.number_of_threads_cdcl,
                                MAX_THREADS);
            }
            return MAX_THREADS;
#else
            return 1;
#endif
        }();

        const auto portfolio_options =
            this->generate_portfolio_options(base_option, NUMBER_OF_THREADS);

        core::CDCLCoreResult              cdcl_result;
        int                               winning_thread_id = -1;
        std::vector<core::CDCLCoreResult> thread_results;

        if (NUMBER_OF_THREADS <= 1) {
            core::CDCLCore cdcl_core(&matrix_model, this->m_check_interrupt,
                                     portfolio_options[0]);
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
                    for (int i = 1; i < NUMBER_OF_THREADS; i++) {
                        if (thread_results[i].best_total_violation <
                            cdcl_result.best_total_violation) {
                            cdcl_result       = thread_results[i];
                            winning_thread_id = i;
                        }
                    }
                } else {
                    for (int i = 1; i < NUMBER_OF_THREADS; i++) {
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
                for (auto* variable_ptr : selection.variable_ptrs) {
                    if (variable_ptr->value() == 1) {
                        variable_ptr->select();
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
        m_result = CDCLControllerResult<T_Variable, T_Expression>(cdcl_result);

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
                "------+-------+--------------+--------+--------+-------+-----"
                "--------+----------",
                IS_ENABLED_PRINT);
            utility::print(
                "Thread|  Act. |    Status    |Conflict|Decision|Restart|  "
                "Intensity  |Best Viol.",
                IS_ENABLED_PRINT);
            utility::print(
                "      |Feedbk.|              |        |        |       | "
                "Var./Constr.|          ",
                IS_ENABLED_PRINT);
            utility::print(
                "------+-------+--------------+--------+--------+-------+-----"
                "--------+----------",
                IS_ENABLED_PRINT);

            for (int t = 0; t < NUMBER_OF_THREADS; t++) {
                const auto& thread_result = thread_results[t];
                std::string status_string =
                    core::CDCLCoreTerminationStatusInverseMap.at(
                        thread_result.termination_status);

                std::string violation_string = "-";
                if (thread_result.termination_status ==
                    core::CDCLCoreTerminationStatus::FEASIBLE) {
                    violation_string = "0.00e+00";
                } else if (this->m_option.cdcl
                               .is_enabled_evaluate_full_solutions &&
                           !std::isinf(thread_result.best_total_violation)) {
                    violation_string = utility::to_string(
                        thread_result.best_total_violation, "%.2e");
                }

                char intensity_buffer[32];
                if (thread_result.number_of_decisions > 0 &&
                    thread_result.number_of_conflicts > 0) {
                    std::snprintf(intensity_buffer, sizeof(intensity_buffer),
                                  " %5.3f/%-5.3f ",
                                  thread_result.decision_intensity,
                                  thread_result.conflict_intensity);
                } else if (thread_result.number_of_decisions > 0) {
                    std::snprintf(intensity_buffer, sizeof(intensity_buffer),
                                  " %5.3f/  -   ",
                                  thread_result.decision_intensity);
                } else {
                    std::snprintf(intensity_buffer, sizeof(intensity_buffer),
                                  "   -  /  -   ");
                }

                char row[160];
                std::snprintf(row, sizeof(row),
                              "%2s%4d| %5.3f | %-12s |%7d |%7d |%6d |%s| %8s ",
                              (t == winning_thread_id ? "* " : "  "),  //
                              t,                                       //
                              portfolio_options[t]
                                  .cdcl.variable_activity_feedback_rate,  //
                              status_string.c_str(),                      //
                              thread_result.number_of_conflicts,          //
                              thread_result.number_of_decisions,          //
                              thread_result.number_of_restarts,           //
                              intensity_buffer,                           //
                              violation_string.c_str());
                utility::print(std::string(row), IS_ENABLED_PRINT);
            }

            utility::print(
                "------+-------+--------------+--------+--------+-------+-----"
                "--------+----------",
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
        return m_result;
    }
};
}  // namespace printemps::solver::cdcl::controller
#endif
/*****************************************************************************/
// END
/*****************************************************************************/
