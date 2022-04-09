/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
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

namespace printemps {
namespace solver {
namespace tabu_search {
namespace controller {
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
        model::Model<T_Variable, T_Expression>* a_model_ptr,  //
        const solution::DenseSolution<T_Variable, T_Expression>&
            a_INITIAL_SOLUTION,  //
        solution::IncumbentHolder<T_Variable, T_Expression>*
                                          a_incumbent_holder_ptr,
        Memory<T_Variable, T_Expression>* a_memory_ptr,  //
        solution::SolutionArchive<T_Variable, T_Expression>*
                                   a_solution_archive_ptr,  //
        const utility::TimeKeeper& a_TIME_KEEPER,           //
        const option::Option&      a_OPTION) {
        this->initialize();

        this->setup(a_model_ptr,             //
                    a_INITIAL_SOLUTION,      //
                    a_incumbent_holder_ptr,  //
                    a_memory_ptr,            //
                    a_solution_archive_ptr,  //
                    a_TIME_KEEPER,           //
                    a_OPTION);
    }

    /*************************************************************************/
    virtual ~TabuSearchController(void) {
        /// nothing to do
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
        m_state_manager.setup(this->m_model_ptr,             //
                              this->m_incumbent_holder_ptr,  //
                              this->m_memory_ptr,            //
                              this->m_option);

        auto& state = m_state_manager.state();

        m_mt19937.seed(this->m_option.seed);

        state.current_solution = this->m_initial_solution;

        if (this->m_option.is_enabled_write_trend) {
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
    inline bool satisfy_time_over_terminate_condition(
        const bool a_IS_ENABLED_PRINT) {
        const auto& STATE = m_state_manager.state();

        if (STATE.total_elapsed_time > this->m_option.time_max) {
            utility::print_message(
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
        if (STATE.iteration >= this->m_option.iteration_max) {
            utility::print_message(
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
        if (this->m_incumbent_holder_ptr->feasible_incumbent_objective() <=
            this->m_option.target_objective_value) {
            utility::print_message(
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
            utility::print_message(
                "Outer loop was terminated because an optimal solution was "
                "found.",
                a_IS_ENABLED_PRINT);
            return true;
        }
        return false;
    }

    /*************************************************************************/
    inline void print_trend(const bool a_IS_ENABLED_PRINT) {
        const auto& STATE = m_state_manager.state();

        /**
         * Print the summary.
         */
        this->print_outer_loop_iteration(  //
            STATE.iteration, a_IS_ENABLED_PRINT);

        this->print_total_elapsed_time(  //
            STATE.total_elapsed_time, a_IS_ENABLED_PRINT);

        this->print_incumbent_summary(a_IS_ENABLED_PRINT);

        /**
         * Print the optimization status of the previous tabu search loop.
         */
        this->print_update_status(  //
            STATE.tabu_search_result.total_update_status, a_IS_ENABLED_PRINT);

        /**
         * Print the search intensity.
         */
        this->print_intensity(  //
            STATE.current_primal_intensity, STATE.current_dual_intensity,
            a_IS_ENABLED_PRINT);

        /**
         * Print the search performance.
         */
        this->print_performance(  //
            STATE.tabu_search_result.performance, a_IS_ENABLED_PRINT);

        /**
         * Print the averaged inner iteration speed.
         */
        this->print_averaged_inner_iteration_speed(
            STATE.averaged_inner_iteration_speed, a_IS_ENABLED_PRINT);

        /**
         * Print the averaged move evaluation speed.
         */
        this->print_averaged_move_evaluation_speed(
            STATE.averaged_move_evaluation_speed, a_IS_ENABLED_PRINT);

        /**
         * Print the number of found feasible solutions.
         */
        this->print_number_of_feasible_solutions(
            this->m_solution_archive_ptr->solutions().size(),
            a_IS_ENABLED_PRINT);

        /**
         * Print the number of violative constraints.
         */
        this->print_violative_constraints(
            this->m_incumbent_holder_ptr->local_augmented_incumbent_solution(),
            a_IS_ENABLED_PRINT);

        /**
         * Print message if the penalty coefficients were changed.
         */
        if (STATE.penalty_coefficient_reset_flag) {
            this->print_penalty_coefficient_reset(a_IS_ENABLED_PRINT);
        } else if (STATE.is_enabled_penalty_coefficient_relaxing) {
            this->print_penalty_coefficient_relaxing(a_IS_ENABLED_PRINT);
        } else if (STATE.is_enabled_penalty_coefficient_tightening) {
            this->print_penalty_coefficient_tightening(a_IS_ENABLED_PRINT);
        }

        this->print_penalty_coefficient_relaxing_rate(  //
            STATE.penalty_coefficient_relaxing_rate, a_IS_ENABLED_PRINT);

        this->print_penalty_coefficient_tightening_rate(  //
            STATE.penalty_coefficient_tightening_rate, a_IS_ENABLED_PRINT);

        /**
         * Print the initial tabu tenure for the next loop.
         */
        this->print_initial_tabu_tenure(  //
            STATE.initial_tabu_tenure, a_IS_ENABLED_PRINT);

        /**
         * Print the improvability screening_mode
         */
        this->print_improvability_screening_mode(  //
            STATE.improvability_screening_mode, a_IS_ENABLED_PRINT);

        /**
         * Print the initial solution for the next loop.
         */
        if (STATE.employing_global_augmented_solution_flag) {
            this->print_employing_global_augmented_solution(a_IS_ENABLED_PRINT);
        } else if (STATE.employing_local_augmented_solution_flag) {
            this->print_employing_local_augmented_solution(a_IS_ENABLED_PRINT);
        } else if (STATE.employing_previous_solution_flag) {
            this->print_employing_previous_solution(a_IS_ENABLED_PRINT);
        }

        /**
         * Print the number of initial modification for the next
         * loop.
         */
        this->print_number_of_initial_modification(  //
            STATE.number_of_initial_modification, a_IS_ENABLED_PRINT);

        /**
         * Print the number of iterations for the next loop.
         */
        this->print_inner_iteration_max(  //
            STATE.iteration_max, a_IS_ENABLED_PRINT);

        /**
         * Print a message of the special neighborhood moves
         * activation/deactivation.
         */
        if (STATE.is_disabled_special_neighborhood_move) {
            this->print_special_neighborhood_move_disabled(a_IS_ENABLED_PRINT);
        }

        if (STATE.is_enabled_special_neighborhood_move) {
            this->print_special_neighborhood_move_enabled(a_IS_ENABLED_PRINT);
        }

        /**
         * Print the number of the stored chain moves.
         */
        this->print_number_of_stored_chain_moves(  //
            this->m_model_ptr->neighborhood().chain().moves().size(),
            a_IS_ENABLED_PRINT);
    }

    /*************************************************************************/
    inline void print_update_status(const bool a_UPDATE_STATUS,
                                    const bool a_IS_ENABLED_PRINT) {
        if (a_UPDATE_STATUS & solution::IncumbentHolderConstant::
                                  STATUS_FEASIBLE_INCUMBENT_UPDATE) {
            utility::print_message("Feasible incumbent objective was updated. ",
                                   a_IS_ENABLED_PRINT);

        } else if (a_UPDATE_STATUS &
                   solution::IncumbentHolderConstant::
                       STATUS_GLOBAL_AUGMENTED_INCUMBENT_UPDATE) {
            utility::print_message("Global incumbent objective was updated. ",
                                   a_IS_ENABLED_PRINT);
        } else {
            utility::print_message("Incumbent objective was not updated.",
                                   a_IS_ENABLED_PRINT);
        }
    }

    /*************************************************************************/
    inline void print_number_of_feasible_solutions(
        const int  a_NUMBER_OF_FEASIBLE_SOLUTIONS,
        const bool a_IS_ENABLED_PRINT) {
        if (this->m_option.is_enabled_store_feasible_solutions) {
            utility::print_message(
                "Number of feasible solutions found so far is " +
                    std::to_string(a_NUMBER_OF_FEASIBLE_SOLUTIONS) + ".",
                a_IS_ENABLED_PRINT);
        }
    }

    /*************************************************************************/
    inline void print_intensity(const double a_CURRENT_PRIMAL_INTENSITY,
                                const double a_CURRENT_DUAL_INTENSITY,
                                const bool   a_IS_ENABLED_PRINT) {
        utility::print_message(
            "Historical search intensity is " +
                std::to_string(a_CURRENT_PRIMAL_INTENSITY) + " (primal) / " +
                std::to_string(a_CURRENT_DUAL_INTENSITY) + " (dual).",
            a_IS_ENABLED_PRINT);
    }

    /*************************************************************************/
    inline void print_performance(const double a_SEARCH_PERFORMANCE,
                                  const bool   a_IS_ENABLED_PRINT) {
        utility::print_message("Search performance in the previous loop is " +
                                   std::to_string(a_SEARCH_PERFORMANCE) + ".",
                               a_IS_ENABLED_PRINT);
    }

    /*************************************************************************/
    inline void print_averaged_inner_iteration_speed(
        const double a_AVERAGED_INNER_ITERATION_SPEED,
        const bool   a_IS_ENABLED_PRINT) {
        utility::print_message(
            "Averaged inner iteration speed is " +
                std::to_string(a_AVERAGED_INNER_ITERATION_SPEED) +
                " iterations/sec.",
            a_IS_ENABLED_PRINT);
    }

    /*************************************************************************/
    inline void print_averaged_move_evaluation_speed(
        const double a_AVERAGED_MOVE_EVALUATION_SPEED,
        const bool   a_IS_ENABLED_PRINT) {
        utility::print_message(
            "Averaged move evaluation speed is " +
                std::to_string(a_AVERAGED_MOVE_EVALUATION_SPEED) +
                " moves/sec.",
            a_IS_ENABLED_PRINT);
    }

    /*************************************************************************/
    inline void print_penalty_coefficient_relaxing_rate(
        const double a_PENALTY_COEFFICIENT_RELAXING_RATE,
        const bool   a_IS_ENABLED_PRINT) {
        utility::print_message(  //
            "Penalty coefficients relaxing rate is " +
                std::to_string(a_PENALTY_COEFFICIENT_RELAXING_RATE) + ".",
            a_IS_ENABLED_PRINT);
    }

    /*************************************************************************/
    inline void print_penalty_coefficient_tightening_rate(
        const double a_PENALTY_COEFFICIENT_TIGHTENING_RATE,
        const bool   a_IS_ENABLED_PRINT) {
        utility::print_message(  //
            "Penalty coefficients tightening rate is " +
                std::to_string(a_PENALTY_COEFFICIENT_TIGHTENING_RATE) + ".",
            a_IS_ENABLED_PRINT);
    }

    /*************************************************************************/
    inline void print_penalty_coefficient_reset(const bool a_IS_ENABLED_PRINT) {
        utility::print_message(
            "Penalty coefficients were reset due to search stagnation.",
            a_IS_ENABLED_PRINT);
    }

    /*************************************************************************/
    inline void print_penalty_coefficient_relaxing(
        const bool a_IS_ENABLED_PRINT) {
        utility::print_message(                    //
            "Penalty coefficients were relaxed.",  //
            a_IS_ENABLED_PRINT);
    }

    /*************************************************************************/
    inline void print_penalty_coefficient_tightening(
        const bool a_IS_ENABLED_PRINT) {
        utility::print_message(                      //
            "Penalty coefficients were tightened.",  //
            a_IS_ENABLED_PRINT);
    }

    /*************************************************************************/
    inline void print_number_of_stored_chain_moves(
        const int  a_NUMBER_OF_STORED_CHAIN_MOVES,
        const bool a_IS_ENABLED_PRINT) {
        if (a_NUMBER_OF_STORED_CHAIN_MOVES > 0) {
            utility::print_message(
                "There are " + std::to_string(a_NUMBER_OF_STORED_CHAIN_MOVES) +
                    " stored chain moves.",
                a_IS_ENABLED_PRINT);
        }
    }

    /*************************************************************************/
    inline void print_special_neighborhood_move_enabled(
        const bool a_IS_ENABLED_PRINT) {
        if (this->m_model_ptr->neighborhood()
                .number_of_special_neighborhood_moves() > 0) {
            utility::print_message("Special neighborhood moves were enabled.",
                                   a_IS_ENABLED_PRINT);
        }
    }

    /*************************************************************************/
    inline void print_special_neighborhood_move_disabled(
        const bool a_IS_ENABLED_PRINT) {
        if (this->m_model_ptr->neighborhood()
                .number_of_special_neighborhood_moves() > 0) {
            utility::print_message("Special neighborhood moves were disabled.",
                                   a_IS_ENABLED_PRINT);
        }
    }

    /*************************************************************************/
    inline void print_violative_constraints(
        const solution::DenseSolution<T_Variable, T_Expression>& a_SOLUTION,
        const bool a_IS_ENABLED_PRINT) {
        if (!a_SOLUTION.is_feasible) {
            int number_of_violative_constraints = 0;
            /**
             * Due to the slow speed of standard output in the Windows DOS,
             * printing all violations will affect performance. To avoid
             * this problem, the maximum number of violations to be printed
             * is 20.
             */
            constexpr int MAX_NUMBER_OF_PRINT_ITEMS = 20;

            utility::print_message(
                "The current solution does not satisfy the following "
                "constraints:",
                a_IS_ENABLED_PRINT);

            for (const auto& proxy : a_SOLUTION.violation_value_proxies) {
                const auto& values      = proxy.flat_indexed_values();
                const auto& names       = proxy.flat_indexed_names();
                const int   VALUES_SIZE = values.size();

                for (auto i = 0; i < VALUES_SIZE; i++) {
                    if (values[i] > 0) {
                        number_of_violative_constraints++;
                        if (number_of_violative_constraints <=
                            MAX_NUMBER_OF_PRINT_ITEMS) {
                            utility::print_info(
                                " -- " + names[i] + " (violation: " +
                                    std::to_string(values[i]) + ")",
                                a_IS_ENABLED_PRINT);
                        }
                    }
                }
            }
            if (number_of_violative_constraints > MAX_NUMBER_OF_PRINT_ITEMS) {
                utility::print_info("and much more...", a_IS_ENABLED_PRINT);
            }

            utility::print_message(
                "There are " + std::to_string(number_of_violative_constraints) +
                    " violative constraints.",
                a_IS_ENABLED_PRINT);
        }
    }

    /*************************************************************************/
    inline void print_improvability_screening_mode(
        const option::improvability_screening_mode::ImprovabilityScreeningMode&
             a_IMPROVABILITY_SCREENING_MODE,
        bool a_IS_ENABLED_PRINT) {
        switch (a_IMPROVABILITY_SCREENING_MODE) {
            case option::improvability_screening_mode::Off: {
                utility::print_message(
                    "No improvability screening will be applied for the next "
                    "loop.",
                    a_IS_ENABLED_PRINT);
                break;
            }
            case option::improvability_screening_mode::Soft: {
                utility::print_message(
                    "Soft improvability screening will be applied in the "
                    "next loop.",
                    a_IS_ENABLED_PRINT);
                break;
            }
            case option::improvability_screening_mode::Aggressive: {
                utility::print_message(
                    "Aggressive improvability screening will be applied in "
                    "the next loop.",
                    a_IS_ENABLED_PRINT);
                break;
            }
            case option::improvability_screening_mode::Intensive: {
                utility::print_message(
                    "Intensive improvability screening will be applied in "
                    "the next loop.",
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
    inline void print_outer_loop_iteration(const int  a_ITERATION,
                                           const bool a_IS_ENABLED_PRINT) {
        utility::print_message(
            "Tabu search loop (" + std::to_string(a_ITERATION + 1) + "/" +
                std::to_string(this->m_option.iteration_max) + ") finished.",
            a_IS_ENABLED_PRINT);
    }

    /*************************************************************************/
    inline void print_initial_tabu_tenure(const int  a_INITIAL_TABU_TENURE,
                                          const bool a_IS_ENABLED_PRINT) {
        utility::print_message(  //
            "Tabu tenure for the next loop was set to " +
                std::to_string(a_INITIAL_TABU_TENURE) + ".",
            a_IS_ENABLED_PRINT);
    }

    /*************************************************************************/
    inline void print_number_of_initial_modification(
        const int  a_NUMBER_OF_INITIAL_MODIFICATION,
        const bool a_IS_ENABLED_PRINT) {
        if (a_NUMBER_OF_INITIAL_MODIFICATION > 0) {
            utility::print_message(
                "For the initial " +
                    std::to_string(a_NUMBER_OF_INITIAL_MODIFICATION) +
                    " iterations in the next loop, the solution will be "
                    "randomly updated to escape from the local optimum.",
                a_IS_ENABLED_PRINT);
        }
    }

    /*************************************************************************/
    inline void print_employing_global_augmented_solution(
        const bool a_IS_ENABLED_PRINT) {
        utility::print_message(
            "The next loop will start from the global incumbent solution.",
            a_IS_ENABLED_PRINT);
    }

    /*************************************************************************/
    inline void print_employing_local_augmented_solution(
        const bool a_IS_ENABLED_PRINT) {
        utility::print_message(
            "The next loop will start from the local incumbent solution found "
            "in the previous loop.",
            a_IS_ENABLED_PRINT);
    }

    /*************************************************************************/
    inline void print_employing_previous_solution(
        const bool a_IS_ENABLED_PRINT) {
        utility::print_message(
            "The next loop will start from the same initial solution of the "
            "previous loop.",
            a_IS_ENABLED_PRINT);
    }

    /*************************************************************************/
    inline void print_inner_iteration_max(const int  a_ITERATION_MAX,
                                          const bool a_IS_ENABLED_PRINT) {
        utility::print_message(
            "The maximum number of iterations for the next loop was "
            "set to " +
                std::to_string(a_ITERATION_MAX) + ".",
            a_IS_ENABLED_PRINT);
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
             * Terminate the loop if the time is over.
             */
            if (this->satisfy_time_over_terminate_condition(  //
                    this->m_option.verbose >= option::verbose::Outer)) {
                break;
            }

            /**
             * Terminate the loop if the iteration is over.
             */
            if (this->satisfy_iteration_over_terminate_condition(  //
                    this->m_option.verbose >= option::verbose::Outer)) {
                break;
            }

            /**
             * Terminate the loop if the objective value of the feasible
             * incumbent reaches the target value.
             */
            if (this->satisfy_reach_target_terminate_condition(  //
                    this->m_option.verbose >= option::verbose::Outer)) {
                break;
            }

            /**
             * Terminate the loop if the optimal solution is found.
             */
            if (this->satisfy_optimal_terminate_condition(  //
                    this->m_option.verbose >= option::verbose::Outer)) {
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
                this->m_model_ptr,                              //
                state.current_solution.variable_value_proxies,  //
                this->m_incumbent_holder_ptr,                   //
                this->m_memory_ptr,                             //
                option);

            tabu_search.run();

            /**
             * Update the elapsed time.
             */
            m_state_manager.set_total_elapsed_time(this->m_time_keeper.clock());

            /**
             * Update the state by tabu search result.
             */
            m_state_manager.update(tabu_search.result(), &m_mt19937);

            /**
             * Update variable bounds.
             */
            if (this->m_option.is_enabled_presolve &&
                state.is_feasible_incumbent_updated) {
                this->bound_variables(
                    this->m_incumbent_holder_ptr
                        ->feasible_incumbent_objective(),
                    this->m_option.verbose >= option::verbose::Outer);
            }

            /**
             * Update the feasible solutions archive.
             */
            if (this->m_option.is_enabled_store_feasible_solutions) {
                this->update_archive(tabu_search.feasible_solutions());
            }

            /**
             * Print trend.
             */
            this->print_trend(this->m_option.verbose >= option::verbose::Outer);

            /**
             * Logging.
             */
            if (this->m_option.is_enabled_write_trend) {
                m_logger.write_log();
            }

            /**
             * Run the call-back function if specified.
             */
            this->m_model_ptr->callback(&option, this->m_incumbent_holder_ptr);

            m_state_manager.next_iteration();
        }

        this->postprocess();
    }

    /*************************************************************************/
    inline constexpr const TabuSearchControllerResult<T_Variable, T_Expression>&
    result(void) const {
        return m_result;
    }
};
}  // namespace controller
}  // namespace tabu_search
}  // namespace solver
}  // namespace printemps
#endif
/*****************************************************************************/
// END
/*****************************************************************************/