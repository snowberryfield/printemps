/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_TABU_SEARCH_CONTROLLER_TABU_SEARCH_CONTROLLER_H__
#define PRINTEMPS_SOLVER_TABU_SEARCH_CONTROLLER_TABU_SEARCH_CONTROLLER_H__

#include "../../abstract_solver_controller.h"
#include "../core/tabu_search_core.h"

#include "tabu_search_controller_state.h"
#include "tabu_search_controller_state_manager.h"
#include "tabu_search_controller_logger.h"
#include "tabu_search_controller_result.h"

namespace printemps::solver::tabu_search::controller {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class TabuSearchController
    : public AbstractSolverController<T_Variable, T_Expression> {
   private:
    TabuSearchControllerStateManager<T_Variable, T_Expression> m_state_manager;

    TabuSearchControllerLogger<T_Variable, T_Expression> m_logger;
    TabuSearchControllerResult<T_Variable, T_Expression> m_result;
    std::mt19937                                         m_mt19937;

   public:
    /*************************************************************************/
    TabuSearchController(void) {
        this->initialize();
    }

    /*************************************************************************/
    TabuSearchController(
        model::Model<T_Variable, T_Expression>* a_model_ptr,         //
        GlobalState<T_Variable, T_Expression>*  a_global_state_ptr,  //
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
        m_state_manager.initialize();
        m_logger.initialize();
    }

    /*************************************************************************/
    inline void preprocess(void) {
        m_state_manager.setup(this->m_model_ptr,         //
                              this->m_global_state_ptr,  //
                              this->m_option);

        auto& state = m_state_manager.state();

        m_mt19937.seed(this->m_option.general.seed);

        state.current_solution = this->m_initial_solution;

        if (this->m_option.output.is_enabled_write_trend) {
            m_logger.setup("trend.txt", this, &state);
            m_logger.write_instance_info();
            m_logger.write_header();
        }
    }

    /*************************************************************************/
    inline void postprocess(void) {
        m_result = TabuSearchControllerResult<T_Variable, T_Expression>(
            m_state_manager.state());
    }

    /*************************************************************************/
    inline bool satisfy_interrupted_terminate_condition(
        const bool a_IS_ENABLED_PRINT) {
        if (this->check_interrupt()) {
            utility::print_message(  //
                "Outer loop was terminated because of interruption.",
                a_IS_ENABLED_PRINT);
            return true;
        }
        return false;
    }

    /*************************************************************************/
    inline bool satisfy_time_over_terminate_condition(
        const bool a_IS_ENABLED_PRINT) {
        const auto& STATE = m_state_manager.state();

        if (STATE.total_elapsed_time > this->m_option.general.time_max) {
            utility::print_message(  //
                "Outer loop was terminated because of time-over (" +
                    utility::to_string(STATE.total_elapsed_time, "%.3f") +
                    "sec).",
                a_IS_ENABLED_PRINT);
            return true;
        }
        return false;
    }

    /*************************************************************************/
    inline bool satisfy_iteration_over_terminate_condition(
        const bool a_IS_ENABLED_PRINT) {
        const auto& STATE = m_state_manager.state();
        if (STATE.iteration >= this->m_option.general.iteration_max) {
            utility::print_message(  //
                "Outer loop was terminated because of iteration limit (" +
                    utility::to_string(STATE.iteration, "%d") + " iterations).",
                a_IS_ENABLED_PRINT);
            return true;
        }
        return false;
    }

    /*************************************************************************/
    inline bool satisfy_reach_target_terminate_condition(
        const bool a_IS_ENABLED_PRINT) {
        const auto& STATE = m_state_manager.state();
        if (this->m_global_state_ptr->incumbent_holder
                .feasible_incumbent_objective() <=
            this->m_option.general.target_objective_value) {
            utility::print_message(  //
                "Outer loop was terminated because of feasible objective "
                "reaches the target limit (" +
                    utility::to_string(STATE.iteration, "%d") + " iterations).",
                a_IS_ENABLED_PRINT);
            return true;
        }
        return false;
    }

    /*************************************************************************/
    inline bool satisfy_optimal_terminate_condition(
        const bool a_IS_ENABLED_PRINT) {
        const auto& STATE = m_state_manager.state();
        if (STATE.iteration > 0 &&
            STATE.tabu_search_result.termination_status ==
                tabu_search::core::TabuSearchCoreTerminationStatus::OPTIMAL) {
            utility::print_message(  //
                "Outer loop was terminated because an optimal solution was "
                "found.",
                a_IS_ENABLED_PRINT);
            return true;
        }
        return false;
    }

    /*************************************************************************/
    inline void print_basic_summary(const bool a_IS_ENABLED_PRINT) {
        this->print_outer_loop_iteration(a_IS_ENABLED_PRINT);

        /**
         * Print the incumbent summary.
         */
        utility::print_dot_line(a_IS_ENABLED_PRINT);
        utility::print(  //
            "# Incumbent Summary", a_IS_ENABLED_PRINT);

        this->print_update_status(a_IS_ENABLED_PRINT);
        this->print_total_elapsed_time(a_IS_ENABLED_PRINT);
        this->print_incumbent_summary(a_IS_ENABLED_PRINT);
        this->print_dual_bound(a_IS_ENABLED_PRINT);
    }

    /*************************************************************************/
    inline void print_optional_summary(const bool a_IS_ENABLED_PRINT) {
        if (this->m_option.output.is_enabled_print_search_behavior_summary) {
            /**
             * Print the search behavior summary.
             */
            utility::print_dot_line(a_IS_ENABLED_PRINT);
            utility::print(  //
                "# Search Behavior Summary", a_IS_ENABLED_PRINT);
            this->print_intensity(a_IS_ENABLED_PRINT);
            this->print_performance(a_IS_ENABLED_PRINT);
            this->print_distance(a_IS_ENABLED_PRINT);
            this->print_speed(a_IS_ENABLED_PRINT);
            this->print_number_of_feasible_solutions(a_IS_ENABLED_PRINT);
        }

        if (this->m_option.output.is_enabled_print_tree_summary) {
            /**
             * Print the search tree summary (frontier solutions).
             */

            utility::print_dot_line(a_IS_ENABLED_PRINT);
            utility::print(  //
                "# Search Tree Summary (Frontier Solutions)",
                a_IS_ENABLED_PRINT);
            this->print_frontier_solutions(a_IS_ENABLED_PRINT);

            /**
             * Print the search tree summary (locally optimal solutions).
             */
            utility::print_dot_line(a_IS_ENABLED_PRINT);
            utility::print(  //
                "# Search Tree Summary (Locally Optimal Solutions)",
                a_IS_ENABLED_PRINT);
            this->print_locally_optimal_solutions(a_IS_ENABLED_PRINT);
        }

        if (this->m_option.output
                .is_enabled_print_thread_count_optimizer_summary) {
            /**
             * Print the state of the thread count optimizer summary.
             */
#ifdef _OPENMP
            if (this->m_option.parallel
                    .is_enabled_move_update_parallelization &&
                this->m_option.parallel
                    .is_enabled_move_evaluation_parallelization &&
                this->m_option.parallel.is_enabled_thread_count_optimization) {
                utility::print_dot_line(a_IS_ENABLED_PRINT);
                utility::print(  //
                    "# Thread Count Optimization Summary", a_IS_ENABLED_PRINT);
                this->print_thread_count_optimizer(a_IS_ENABLED_PRINT);
            }
#endif
        }

        if (this->m_option.output.is_enabled_print_variable_update_summary) {
            /**
             * Print the variable update summary.
             */
            utility::print_dot_line(a_IS_ENABLED_PRINT);
            utility::print(  //
                "# Variable Update Summary", a_IS_ENABLED_PRINT);
            this->print_variable_update_frequency(a_IS_ENABLED_PRINT);
        }

        if (this->m_option.output
                .is_enabled_print_constraint_violation_summary) {
            /**
             * Print the constraint violation summary.
             */
            utility::print_dot_line(a_IS_ENABLED_PRINT);
            utility::print(  //
                "# Constraint Violation Summary", a_IS_ENABLED_PRINT);
            this->print_constraint_violation_frequency(a_IS_ENABLED_PRINT);
        }

        if (this->m_option.output
                .is_enabled_print_violation_and_penalty_summary) {
            /**
             * Print the violation and penalty summary.
             */
            utility::print_dot_line(a_IS_ENABLED_PRINT);
            utility::print(  //
                "# Violation and Penalty Summary", a_IS_ENABLED_PRINT);
            this->print_violative_constraints(a_IS_ENABLED_PRINT);
            this->print_penalty_coefficient(a_IS_ENABLED_PRINT);
        }

        if (this->m_option.output.is_enabled_print_tabu_search_parameter) {
            /**
             * Print the tabu search parameters for the Next loop.
             */
            utility::print_dot_line(a_IS_ENABLED_PRINT);
            utility::print(  //
                "# Tabu Search Parameters for the Next loop",
                a_IS_ENABLED_PRINT);

            this->print_initial_tabu_tenure(a_IS_ENABLED_PRINT);
            this->print_improvability_screening_mode(a_IS_ENABLED_PRINT);
            this->print_initial_solution(a_IS_ENABLED_PRINT);
            this->print_number_of_initial_modification(a_IS_ENABLED_PRINT);
            this->print_inner_iteration_max(a_IS_ENABLED_PRINT);
            this->print_is_enabled_special_neighborhood_move(
                a_IS_ENABLED_PRINT);
            this->print_number_of_stored_chain_moves(a_IS_ENABLED_PRINT);
        }
    }

    /*************************************************************************/
    inline void print_outer_loop_iteration(
        const bool a_IS_ENABLED_PRINT) const {
        const auto& STATE = m_state_manager.state();

        utility::print_message(  //
            "Tabu search loop finished (" +
                std::to_string(STATE.iteration + 1) + "/" +
                std::to_string(this->m_option.general.iteration_max) +
                ", Reason: " +
                core::TabuSearchCoreTerminationStatusInverseMap.at(
                    STATE.tabu_search_result.termination_status) +
                ").",
            a_IS_ENABLED_PRINT);
    }

    /*************************************************************************/
    inline void print_update_status(const bool a_IS_ENABLED_PRINT) const {
        const auto& STATE        = m_state_manager.state();
        const auto UPDATE_STATUS = STATE.tabu_search_result.total_update_status;

        if (UPDATE_STATUS & solution::IncumbentHolderConstant::
                                STATUS_FEASIBLE_INCUMBENT_UPDATE) {
            utility::print_info(  //
                " -- Status: Feasible incumbent objective was updated. ",
                a_IS_ENABLED_PRINT);

        } else if (UPDATE_STATUS &
                   solution::IncumbentHolderConstant::
                       STATUS_GLOBAL_AUGMENTED_INCUMBENT_UPDATE) {
            utility::print_info(  //
                " -- Status: Global incumbent objective was updated. ",
                a_IS_ENABLED_PRINT);
        } else {
            utility::print_info(  //
                " -- Status: Incumbent objective was not updated.",
                a_IS_ENABLED_PRINT);
        }
    }

    /*************************************************************************/
    inline void print_intensity(const bool a_IS_ENABLED_PRINT) const {
        const auto& STATE = m_state_manager.state();
        utility::print_info(  //
            " -- Primal search intensity: " +
                utility::to_string(STATE.current_primal_intensity, "%.3e"),
            a_IS_ENABLED_PRINT);

        utility::print_info(  //
            " -- Dual search intensity: " +
                utility::to_string(STATE.current_dual_intensity, "%.3e"),
            a_IS_ENABLED_PRINT);
    }

    /*************************************************************************/
    inline void print_performance(const bool a_IS_ENABLED_PRINT) const {
        const auto& STATE = m_state_manager.state();
        utility::print_info(  //
            " -- Search performance in the previous loop: " +
                utility::to_string(STATE.tabu_search_result.performance,
                                   "%.5e"),
            a_IS_ENABLED_PRINT);
    }

    /*************************************************************************/
    inline void print_distance(const bool a_IS_ENABLED_PRINT) const {
        const auto& STATE = m_state_manager.state();
        utility::print_info(  //
            " -- L0-Distance from the previous initial solution: " +
                utility::to_string(STATE.distance_from_current_solution, "%d"),
            a_IS_ENABLED_PRINT);
        utility::print_info(  //
            " -- L0-Distance from the global incumbent solution: " +
                utility::to_string(STATE.distance_from_global_solution, "%d"),
            a_IS_ENABLED_PRINT);
    }

    /*************************************************************************/
    inline void print_speed(const bool a_IS_ENABLED_PRINT) const {
        const auto& STATE = m_state_manager.state();
        utility::print_info(  //
            " -- Averaged inner iteration speed: " +
                utility::to_string(STATE.averaged_inner_iteration_speed,
                                   "%.5e") +
                " iterations/sec",
            a_IS_ENABLED_PRINT);
        utility::print_info(  //
            " -- Averaged move evaluation speed: " +
                utility::to_string(STATE.averaged_move_evaluation_speed,
                                   "%.5e") +
                " moves/sec",
            a_IS_ENABLED_PRINT);
    }

    /*************************************************************************/
    inline void print_number_of_feasible_solutions(
        const bool a_IS_ENABLED_PRINT) const {
        if (this->m_option.output.is_enabled_store_feasible_solutions) {
            utility::print_info(  //
                " -- Number of feasible solutions found: " +
                    std::to_string(
                        this->m_global_state_ptr->feasible_solution_archive
                            .solutions()
                            .size()),
                a_IS_ENABLED_PRINT);
        }
    }

    /*************************************************************************/
    inline void print_frontier_solutions(const bool a_IS_ENABLED_PRINT) const {
        const auto& FRONTIER_SOLUTIONS =
            this->m_global_state_ptr->search_tree.frontier_solutions();
        if (FRONTIER_SOLUTIONS.size() == 0) {
            utility::print_message(  //
                "There are no frontier solution nodes in the search tree.",
                a_IS_ENABLED_PRINT);
            return;
        }

        utility::print_message(  //
            "There are " + std::to_string(FRONTIER_SOLUTIONS.size()) +
                " frontier solution nodes in the search tree:",
            a_IS_ENABLED_PRINT);
        for (const auto& solution : FRONTIER_SOLUTIONS) {
            utility::print_info(  //
                " -- Obj.: " +
                    utility::to_string(solution.first.objective, "%.3e,") +
                    " Viol: " +
                    utility::to_string(solution.first.total_violation,
                                       "%.3e,") +
                    " L0-dist.: " + utility::to_string(solution.second, "%d"),
                a_IS_ENABLED_PRINT);
        }
    }

    /*************************************************************************/
    inline void print_locally_optimal_solutions(
        const bool a_IS_ENABLED_PRINT) const {
        const auto& LOCALLY_OPTIMAL_SOLUTIONS =
            this->m_global_state_ptr->search_tree.locally_optimal_solutions();

        if (LOCALLY_OPTIMAL_SOLUTIONS.size() == 0) {
            utility::print_message(  //
                "There are no locally optimal solutions nodes in the search "
                "tree.",
                a_IS_ENABLED_PRINT);
            return;
        }

        utility::print_message(  //
            "There are " + std::to_string(LOCALLY_OPTIMAL_SOLUTIONS.size()) +
                " locally optimal solution nodes in the search tree:",
            a_IS_ENABLED_PRINT);
        for (const auto& solution : LOCALLY_OPTIMAL_SOLUTIONS) {
            utility::print_info(  //
                " -- Obj.: " +
                    utility::to_string(solution.first.objective, "%.3e,") +
                    " Viol: " +
                    utility::to_string(solution.first.total_violation,
                                       "%.3e,") +
                    " L0-dist.: " + utility::to_string(solution.second, "%d"),
                a_IS_ENABLED_PRINT);
        }
    }

    /*************************************************************************/
    inline void print_thread_count_optimizer(
        const bool a_IS_ENABLED_PRINT) const {
        const auto& STATE      = m_state_manager.state();
        const auto& CONTROLLER = STATE.thread_count_optimizer;

        utility::print_message(  //
            "The state of the thread count optimization (U: Update moves, E: "
            "Evaluate moves, '*': Selected pattern for the next loop):",
            a_IS_ENABLED_PRINT);

        for (const auto& action : CONTROLLER.actions()) {
            char flag =
                CONTROLLER.best_action().body == action.body ? '*' : ' ';
            utility::print_info(
                utility::to_string(flag, " -- %c(U,E)=") +
                    utility::to_string(action.body.first, "(%03d,") +
                    utility::to_string(action.body.second, "%03d), ") +
                    "Mean/Conf.: " + utility::to_string(action.mean, "%.1e/") +
                    utility::to_string(action.confidence, "%.1e, ") +
                    utility::to_string(action.number_of_samples, "N: %d") +
                    utility::to_string(
                        action.number_of_samples /
                            static_cast<double>(
                                CONTROLLER.total_number_of_samples()),
                        "(%.3f)"),
                a_IS_ENABLED_PRINT);
        }
        utility::print_info(  //
            " -- Averaged number of threads for move update: " +
                utility::to_string(  //
                    STATE.averaged_number_of_threads_move_update, "%.3e"),
            a_IS_ENABLED_PRINT);
        utility::print_info(  //
            " -- Averaged number of threads for move evaluation: " +
                utility::to_string(
                    STATE.averaged_number_of_threads_move_evaluation, "%.3e"),
            a_IS_ENABLED_PRINT);
    }

    /*************************************************************************/
    inline void print_variable_update_frequency(
        const bool a_IS_ENABLED_PRINT) const {
        auto mutable_variable_ptrs =
            this->m_model_ptr->variable_reference().mutable_variable_ptrs;
        if (mutable_variable_ptrs.size() == 0) {
            return;
        }

        constexpr int MAX_NUMBER_OF_PRINT_ITEMS = 5;
        const double  TOTAL_UPDATE_COUNT        = std::max(
            1.0, static_cast<double>(
                     this->m_global_state_ptr->memory.total_update_count()));

        const int MUTABLE_VARIABLES_SIZE = mutable_variable_ptrs.size();

        std::stable_sort(
            mutable_variable_ptrs.begin(), mutable_variable_ptrs.end(),
            [](const auto& a_FIRST, const auto& a_SECOND) {
                return a_FIRST->update_count() < a_SECOND->update_count();
            });

        int count = 0;
        for (auto i = 0; i < MUTABLE_VARIABLES_SIZE; i++) {
            double update_rate =
                mutable_variable_ptrs[i]->update_count() / TOTAL_UPDATE_COUNT;
            update_rate = std::max(0.0, std::min(1.0, update_rate));
            double density =
                mutable_variable_ptrs[i]->related_constraint_ptrs().size() /
                static_cast<double>(
                    std::max(1, this->m_model_ptr->number_of_constraints()));

            if (count > MAX_NUMBER_OF_PRINT_ITEMS) {
                break;
            }

            if (count == 0) {
                utility::print_message(  //
                    "Low-frequent updated variables:", a_IS_ENABLED_PRINT);
            }

            utility::print_info(
                " -- " + mutable_variable_ptrs[i]->name()  //
                    + " (" + mutable_variable_ptrs[i]->sense_label() +
                    ", Freq.: " + utility::to_string(update_rate, "%.3e") +
                    ", Dens.: " + utility::to_string(density, "%.3e") + ")",
                a_IS_ENABLED_PRINT);

            count++;
        }

        count = 0;
        for (auto i = MUTABLE_VARIABLES_SIZE - 1; i >= 0; i--) {
            double update_rate =
                mutable_variable_ptrs[i]->update_count() / TOTAL_UPDATE_COUNT;
            update_rate = std::max(0.0, std::min(1.0, update_rate));
            double density =
                mutable_variable_ptrs[i]->related_constraint_ptrs().size() /
                static_cast<double>(
                    std::max(1, this->m_model_ptr->number_of_constraints()));

            if (update_rate < constant::EPSILON_10) {
                break;
            }

            if (count > MAX_NUMBER_OF_PRINT_ITEMS) {
                break;
            }

            if (count == 0) {
                utility::print_message(  //
                    "High-frequent updated variables:", a_IS_ENABLED_PRINT);
            }

            utility::print_info(
                " -- " + mutable_variable_ptrs[i]->name()  //
                    + " (" + mutable_variable_ptrs[i]->sense_label() +
                    ", Freq.: " + utility::to_string(update_rate, "%.3e") +
                    ", Dens.: " + utility::to_string(density, "%.3e") + ")",
                a_IS_ENABLED_PRINT);

            count++;
        }
    }

    /*************************************************************************/
    inline void print_constraint_violation_frequency(
        const bool a_IS_ENABLED_PRINT) const {
        auto enabled_constraint_ptrs =
            this->m_model_ptr->constraint_reference().enabled_constraint_ptrs;
        if (enabled_constraint_ptrs.size() == 0) {
            return;
        }

        constexpr int MAX_NUMBER_OF_PRINT_ITEMS = 5;
        const double  TOTAL_UPDATE_COUNT        = std::max(
            1.0, static_cast<double>(
                     this->m_global_state_ptr->memory.total_update_count()));

        const int ENABLED_CONSTRAINTS_SIZE = enabled_constraint_ptrs.size();

        std::stable_sort(
            enabled_constraint_ptrs.begin(), enabled_constraint_ptrs.end(),
            [](const auto& a_FIRST, const auto& a_SECOND) {
                return a_FIRST->violation_count() > a_SECOND->violation_count();
            });

        int count = 0;
        for (auto i = 0; i < ENABLED_CONSTRAINTS_SIZE; i++) {
            double satisfaction_rate =
                1.0 - enabled_constraint_ptrs[i]->violation_count() /
                          TOTAL_UPDATE_COUNT;
            satisfaction_rate = std::max(0.0, std::min(1.0, satisfaction_rate));

            double density = enabled_constraint_ptrs[i]
                                 ->expression()
                                 .sensitivities()
                                 .size() /
                             static_cast<double>(std::max(
                                 1, this->m_model_ptr->number_of_variables()));

            if (std::fabs(1.0 - satisfaction_rate) < constant::EPSILON_10) {
                break;
            }

            if (count > MAX_NUMBER_OF_PRINT_ITEMS) {
                break;
            }

            if (count == 0) {
                utility::print_message(  //
                    "Low-frequent satisfied constraints:", a_IS_ENABLED_PRINT);
            }

            utility::print_info(
                " -- " + enabled_constraint_ptrs[i]->name()  //
                    + " (" + enabled_constraint_ptrs[i]->type() + ", Freq.: " +
                    utility::to_string(satisfaction_rate, "%.3e") +
                    ", Dens.: " + utility::to_string(density, "%.3e") + ")",
                a_IS_ENABLED_PRINT);

            count++;
        }

        count = 0;
        for (auto i = ENABLED_CONSTRAINTS_SIZE - 1; i >= 0; i--) {
            double satisfaction_rate =
                1.0 - enabled_constraint_ptrs[i]->violation_count() /
                          TOTAL_UPDATE_COUNT;
            satisfaction_rate = std::max(0.0, std::min(1.0, satisfaction_rate));

            double density = enabled_constraint_ptrs[i]
                                 ->expression()
                                 .sensitivities()
                                 .size() /
                             static_cast<double>(std::max(
                                 1, this->m_model_ptr->number_of_variables()));

            if (satisfaction_rate < constant::EPSILON_10) {
                break;
            }

            if (count > MAX_NUMBER_OF_PRINT_ITEMS) {
                break;
            }

            if (count == 0) {
                utility::print_message(  //
                    "High-frequent satisfied constraints:", a_IS_ENABLED_PRINT);
            }

            utility::print_info(
                " -- " + enabled_constraint_ptrs[i]->name()  //
                    + " (" + enabled_constraint_ptrs[i]->type() + ", Freq.: " +
                    utility::to_string(satisfaction_rate, "%.3e") +
                    ", Dens.: " + utility::to_string(density, "%.3e") + ")",
                a_IS_ENABLED_PRINT);

            count++;
        }
    }

    /*************************************************************************/
    inline void print_violative_constraints(
        const bool a_IS_ENABLED_PRINT) const {
        const auto& SOLUTION = this->m_global_state_ptr->incumbent_holder
                                   .local_augmented_incumbent_solution();

        if (!SOLUTION.is_feasible) {
            int number_of_violative_constraints = 0;
            /**
             * Due to the slow speed of standard output in the Windows DOS,
             * printing all violations will affect performance. To avoid
             * this problem, the maximum number of violations to be printed
             * is 10.
             */
            constexpr int MAX_NUMBER_OF_PRINT_ITEMS = 10;

            utility::print_message(  //
                "The current solution does not satisfy the following "
                "constraints:",
                a_IS_ENABLED_PRINT);

            const auto& VIOLATION_VALUE_PROXIES =
                SOLUTION.violation_value_proxies;

            for (auto&& proxy : this->m_model_ptr->constraint_proxies()) {
                for (auto&& constraint : proxy.flat_indexed_constraints()) {
                    const double VIOLATION =
                        VIOLATION_VALUE_PROXIES[proxy.index()]
                                               [constraint.flat_index()];

                    if (VIOLATION > 0) {
                        number_of_violative_constraints++;
                        if (number_of_violative_constraints <=
                            MAX_NUMBER_OF_PRINT_ITEMS) {
                            utility::print_info(
                                " -- " + constraint.name()  //
                                    + " (" + constraint.type() + ", Viol.: " +
                                    std::to_string(VIOLATION) + ")",
                                a_IS_ENABLED_PRINT);
                        }
                    }
                }
            }

            if (number_of_violative_constraints > MAX_NUMBER_OF_PRINT_ITEMS) {
                utility::print_info("and much more...", a_IS_ENABLED_PRINT);
            }

            utility::print_message(  //
                "There are " + std::to_string(number_of_violative_constraints) +
                    " violative constraints.",
                a_IS_ENABLED_PRINT);
        }
    }

    /*************************************************************************/
    inline void print_penalty_coefficient(const bool a_IS_ENABLED_PRINT) const {
        const auto& STATE = m_state_manager.state();

        if (STATE.penalty_coefficient_reset_flag) {
            utility::print_message(                                           //
                "Penalty coefficients were reset due to search stagnation.",  //
                a_IS_ENABLED_PRINT);
        } else if (STATE.is_enabled_penalty_coefficient_relaxing) {
            utility::print_message(                    //
                "Penalty coefficients were relaxed.",  //
                a_IS_ENABLED_PRINT);
        } else if (STATE.is_enabled_penalty_coefficient_tightening) {
            utility::print_message(                      //
                "Penalty coefficients were tightened.",  //
                a_IS_ENABLED_PRINT);
        }

        utility::print_info(  //
            " -- Penalty coefficients relaxing rate: " +
                std::to_string(STATE.penalty_coefficient_relaxing_rate),
            a_IS_ENABLED_PRINT);

        utility::print_info(  //
            " -- Penalty coefficients tightening rate: " +
                std::to_string(STATE.penalty_coefficient_tightening_rate),
            a_IS_ENABLED_PRINT);

        utility::print_info(  //
            " -- Current Penalty coefficients (Min / Max) : " +
                utility::to_string(STATE.local_penalty_coefficient_range.min(),
                                   "%.3e") +
                " / " +
                utility::to_string(STATE.local_penalty_coefficient_range.max(),
                                   "%.3e"),
            a_IS_ENABLED_PRINT);
    }

    /*************************************************************************/
    inline void print_initial_tabu_tenure(const bool a_IS_ENABLED_PRINT) const {
        const auto& STATE = m_state_manager.state();

        utility::print_info(  //
            " -- Tabu tenure: " + std::to_string(STATE.initial_tabu_tenure),
            a_IS_ENABLED_PRINT);
    }

    /*************************************************************************/
    inline void print_improvability_screening_mode(
        bool a_IS_ENABLED_PRINT) const {
        const auto& STATE = m_state_manager.state();

        switch (STATE.improvability_screening_mode) {
            case option::improvability_screening_mode::Off: {
                utility::print_info(  //
                    " -- Improvability screening mode: off",
                    a_IS_ENABLED_PRINT);
                break;
            }
            case option::improvability_screening_mode::Soft: {
                utility::print_info(  //
                    " -- Improvability screening mode: soft",
                    a_IS_ENABLED_PRINT);
                break;
            }
            case option::improvability_screening_mode::Aggressive: {
                utility::print_info(  //
                    " -- Improvability screening mode: aggressive",
                    a_IS_ENABLED_PRINT);
                break;
            }
            case option::improvability_screening_mode::Intensive: {
                utility::print_info(  //
                    " -- Improvability screening mode: intensive",
                    a_IS_ENABLED_PRINT);
                break;
            }
            default: {
                throw std::logic_error(utility::format_error_location(
                    __FILE__, __LINE__, __func__,
                    "The specified improvability screening mode is invalid."));
            }
        }
    }

    /*************************************************************************/
    inline void print_initial_solution(const bool a_IS_ENABLED_PRINT) const {
        const auto& STATE = m_state_manager.state();

        if (STATE.employing_global_solution_flag) {
            utility::print_info(  //
                " -- Initial solution: global incumbent", a_IS_ENABLED_PRINT);
        } else if (STATE.employing_local_solution_flag) {
            utility::print_info(  //
                " -- Initial solution: local incumbent", a_IS_ENABLED_PRINT);
        } else if (STATE.employing_previous_solution_flag) {
            utility::print_info(  //
                " -- Initial solution: previous initial", a_IS_ENABLED_PRINT);
        }
    }

    /*************************************************************************/
    inline void print_number_of_initial_modification(
        const bool a_IS_ENABLED_PRINT) const {
        const auto& STATE = m_state_manager.state();

        utility::print_info(  //
            " -- Initial modifications: " +
                std::to_string(STATE.number_of_initial_modification),
            a_IS_ENABLED_PRINT);
    }

    /*************************************************************************/
    inline void print_inner_iteration_max(const bool a_IS_ENABLED_PRINT) const {
        const auto& STATE = m_state_manager.state();

        utility::print_info(  //
            " -- Maximum number of iterations: " +
                std::to_string(STATE.iteration_max),
            a_IS_ENABLED_PRINT);
    }

    /*************************************************************************/
    inline void print_is_enabled_special_neighborhood_move(
        const bool a_IS_ENABLED_PRINT) const {
        const auto& STATE = m_state_manager.state();

        if (STATE.is_enabled_special_neighborhood_move) {
            utility::print_info(" -- Special neighborhood moves: enabled",
                                a_IS_ENABLED_PRINT);
        } else {
            utility::print_info(" -- Special neighborhood moves: disabled",
                                a_IS_ENABLED_PRINT);
        }
    }

    /*************************************************************************/
    inline void print_number_of_stored_chain_moves(
        const bool a_IS_ENABLED_PRINT) const {
        const auto NUMBER_OF_STORED_CHAIN_MOVES =
            this->m_model_ptr->neighborhood().chain().moves().size();

        if (NUMBER_OF_STORED_CHAIN_MOVES > 0) {
            utility::print_info(  //
                " -- Number of stored chain moves: " +
                    std::to_string(NUMBER_OF_STORED_CHAIN_MOVES),
                a_IS_ENABLED_PRINT);
        }
    }

    /*************************************************************************/
    inline void run(void) {
        this->preprocess();

        auto& state = m_state_manager.state();

        m_state_manager.set_tabu_search_start_time(this->m_time_keeper.clock());
        m_state_manager.reset_iteration();

        while (true) {
            m_state_manager.set_total_elapsed_time(this->m_time_keeper.clock());

            /**
             * Terminate the loop if interrupted
             */
            if (this->satisfy_interrupted_terminate_condition(
                    this->m_option.output.verbose >= option::verbose::Outer)) {
                break;
            }

            /**
             * Terminate the loop if the time is over.
             */
            if (this->satisfy_time_over_terminate_condition(  //
                    this->m_option.output.verbose >= option::verbose::Outer)) {
                break;
            }

            /**
             * Terminate the loop if the iteration is over.
             */
            if (this->satisfy_iteration_over_terminate_condition(  //
                    this->m_option.output.verbose >= option::verbose::Outer)) {
                break;
            }

            /**
             * Terminate the loop if the objective value of the feasible
             * incumbent reaches the target value.
             */
            if (this->satisfy_reach_target_terminate_condition(  //
                    this->m_option.output.verbose >= option::verbose::Outer)) {
                break;
            }

            /**
             * Terminate the loop if the optimal solution is found.
             */
            if (this->satisfy_optimal_terminate_condition(  //
                    this->m_option.output.verbose >= option::verbose::Outer)) {
                break;
            }

            /**
             * Prepare an option object for tabu search.
             */
            auto option = m_state_manager.create_option();

            /**
             * Run the tabu search.
             */
            core::TabuSearchCore<T_Variable, T_Expression> tabu_search(
                this->m_model_ptr,         //
                this->m_global_state_ptr,  //
                state.current_solution,    //
                option);

            tabu_search.run();

            /**
             * Update variable bounds.
             */
            if (this->m_option.preprocess.is_enabled_presolve &&
                this->m_option.preprocess.is_enabled_online_bounding &&
                state.current_is_feasible_incumbent_updated &&
                !state.previous_is_feasible_incumbent_updated) {
                this->update_variable_bounds(
                    this->m_global_state_ptr->incumbent_holder
                        .feasible_incumbent_objective(),
                    true,
                    this->m_option.output.verbose >= option::verbose::Outer);
            }

            /**
             * Update the feasible solutions archive.
             */
            if (this->m_option.output.is_enabled_store_feasible_solutions) {
                this->update_feasible_solution_archive(
                    tabu_search.feasible_solutions());
            }

            /**
             * Update the incumbent solutions archive.
             */
            this->update_incumbent_solution_archive_and_search_tree(
                tabu_search.incumbent_solutions());

            /**
             * Update the elapsed time.
             */
            m_state_manager.set_total_elapsed_time(this->m_time_keeper.clock());

            /**
             * Update the state by tabu search result.
             */
            m_state_manager.update(tabu_search.result(), &m_mt19937);

            /**
             * Print basic summary.
             */
            this->print_basic_summary(this->m_option.output.verbose >=
                                      option::verbose::Outer);

            /**
             * Print optional summary.
             */
            this->print_optional_summary(this->m_option.output.verbose >=
                                         option::verbose::Full);

            /**
             * Logging.
             */
            if (this->m_option.output.is_enabled_write_trend) {
                m_logger.write_log();
            }

            /**
             * Run the call-back function if specified.
             */
            this->m_callback(this->m_global_state_ptr);

            m_state_manager.next_iteration();
        }

        this->postprocess();
    }

    /*************************************************************************/
    inline const TabuSearchControllerResult<T_Variable, T_Expression>& result(
        void) const {
        return m_result;
    }
};
}  // namespace printemps::solver::tabu_search::controller
#endif
/*****************************************************************************/
// END
/*****************************************************************************/