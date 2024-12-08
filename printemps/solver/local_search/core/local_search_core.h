/*****************************************************************************/
// Copyright (c) 2020-2024 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_LOCAL_SEARCH_CORE_LOCAL_SEARCH_CORE_H__
#define PRINTEMPS_SOLVER_LOCAL_SEARCH_CORE_LOCAL_SEARCH_CORE_H__

#include "../../integer_step_size_adjuster.h"
#include "local_search_core_termination_status.h"
#include "local_search_core_state.h"
#include "local_search_core_state_manager.h"
#include "local_search_core_result.h"

namespace printemps::solver::local_search::core {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class LocalSearchCore {
   private:
    model::Model<T_Variable, T_Expression>*           m_model_ptr;
    GlobalState<T_Variable, T_Expression>*            m_global_state_ptr;
    solution::DenseSolution<T_Variable, T_Expression> m_initial_solution;
    option::Option                                    m_option;

    std::vector<solution::SparseSolution<T_Variable, T_Expression>>
        m_feasible_solutions;
    std::vector<solution::SparseSolution<T_Variable, T_Expression>>
        m_incumbent_solutions;

    LocalSearchCoreStateManager<T_Variable, T_Expression> m_state_manager;
    LocalSearchCoreResult<T_Variable, T_Expression>       m_result;
    std::mt19937                                          m_get_rand_mt;

    /*************************************************************************/
    inline void preprocess(void) {
        /**
         * Reset the local augmented incumbent.
         */
        m_global_state_ptr->incumbent_holder.reset_local_augmented_incumbent();

        /**
         * Reset the last update iterations.
         */
        m_global_state_ptr->memory.reset_local_last_update_iterations();

        /**
         * Reset the feasible solutions storage.
         */
        m_feasible_solutions.clear();

        /**
         * Reset the incumbent solutions storage.
         */
        m_incumbent_solutions.clear();

        /**
         * Prepare a random generator, which is used for shuffling moves.
         */
        m_get_rand_mt.seed(m_option.local_search.seed);

        /**
         * Initialize the solution and update the model.
         */
        m_model_ptr->import_solution(m_initial_solution);
        m_model_ptr->update();

        /**
         * Reset the variable improvability.
         */
        m_model_ptr->reset_variable_objective_improvabilities();
        m_model_ptr->reset_variable_feasibility_improvabilities();

        m_state_manager.setup(m_model_ptr, m_global_state_ptr, m_option);
    }

    /*************************************************************************/
    inline void postprocess(void) {
        /**
         * Prepare the result.
         */
        m_result = LocalSearchCoreResult<T_Variable, T_Expression>(
            m_state_manager.state(), m_option);
    }

    /*************************************************************************/
    inline bool satisfy_time_over_terminate_condition(void) {
        const auto& STATE = m_state_manager.state();

        if (STATE.elapsed_time > m_option.local_search.time_max) {
            m_state_manager.set_termination_status(
                LocalSearchCoreTerminationStatus::TIME_OVER);
            return true;
        }

        if (STATE.elapsed_time + m_option.local_search.time_offset >
            m_option.general.time_max) {
            m_state_manager.set_termination_status(
                LocalSearchCoreTerminationStatus::TIME_OVER);
            return true;
        }
        return false;
    }

    /*************************************************************************/
    inline bool satisfy_iteration_over_terminate_condition(void) {
        const auto& STATE = m_state_manager.state();

        if (STATE.iteration >= m_option.local_search.iteration_max) {
            m_state_manager.set_termination_status(
                LocalSearchCoreTerminationStatus::ITERATION_OVER);
            return true;
        }
        return false;
    }

    /*************************************************************************/
    inline bool satisfy_reach_target_terminate_condition(void) {
        if (m_global_state_ptr->incumbent_holder
                .feasible_incumbent_objective() <=
            m_option.general.target_objective_value) {
            m_state_manager.set_termination_status(
                LocalSearchCoreTerminationStatus::REACH_TARGET);
            return true;
        }
        return false;
    }

    /*************************************************************************/
    inline bool satisfy_optimal_or_no_move_terminate_condition(void) {
        const auto& STATE = m_state_manager.state();

        if (STATE.number_of_moves > 0) {
            return false;
        }

        if (m_model_ptr->is_feasible()) {
            /**
             * NOTE: If the current solution is feasible and there is no
             * improvable solution, the solution should be an optimum. It can
             * happen for decomp2 instance in MIPLIB 2017.
             */
            m_state_manager.set_termination_status(
                LocalSearchCoreTerminationStatus::OPTIMAL);
            for (const auto& variable_ptr :
                 m_model_ptr->variable_reference().variable_ptrs) {
                if (variable_ptr->is_objective_improvable()) {
                    m_state_manager.set_termination_status(
                        LocalSearchCoreTerminationStatus::NO_MOVE);
                    break;
                }
            }
            return true;
        } else {
            m_state_manager.set_termination_status(
                LocalSearchCoreTerminationStatus::NO_MOVE);
            return true;
        }

        return false;
    }

    /*************************************************************************/
    inline bool satisfy_local_optimal_terminate_condition(
        const bool a_IS_FOUND_IMPROVING_SOLUTION) {
        if (!a_IS_FOUND_IMPROVING_SOLUTION) {
            m_state_manager.set_termination_status(
                LocalSearchCoreTerminationStatus::LOCAL_OPTIMAL);
            return true;
        }
        return false;
    }

    /*************************************************************************/
    inline void update_moves() {
        const auto& STATE = m_state_manager.state();

        bool accept_all                    = true;
        bool accept_objective_improvable   = true;
        bool accept_feasibility_improvable = true;

        /**
         * NOTE: Checking whether the model is linear or not in this process can
         * be skipped because local search will be applied only for linear
         * model.
         */
        if (STATE.iteration == 0) {
            m_model_ptr->update_variable_objective_improvabilities();
        } else {
            m_model_ptr->update_variable_objective_improvabilities(
                STATE.current_move.related_variable_ptrs_vector());
        }

        if (m_model_ptr->is_feasible()) {
            accept_all                    = false;
            accept_objective_improvable   = true;
            accept_feasibility_improvable = false;
        } else {
            m_model_ptr->reset_variable_feasibility_improvabilities();
            m_model_ptr->update_variable_feasibility_improvabilities();

            accept_all                    = false;
            accept_objective_improvable   = false;
            accept_feasibility_improvable = true;
        }

        m_model_ptr->neighborhood().update_moves(
            accept_all,                                                //
            accept_objective_improvable,                               //
            accept_feasibility_improvable,                             //
            m_option.parallel.is_enabled_move_update_parallelization,  //
            m_option.parallel.number_of_threads_move_update);

        m_state_manager.set_number_of_moves(
            m_model_ptr->neighborhood().move_ptrs().size());
    }

    /*************************************************************************/
    inline void update_memory(
        const neighborhood::Move<T_Variable, T_Expression>* a_move_ptr) {
        const auto& STATE = m_state_manager.state();
        m_global_state_ptr->memory.update(*a_move_ptr, STATE.iteration);
    }

    /*************************************************************************/
    inline void print_table_header(const bool a_IS_ENABLED_PRINT) {
        if (!a_IS_ENABLED_PRINT) {
            return;
        }

        utility::print(
            "---------+------------------------+----------------------+--------"
            "--------------",
            true);
        utility::print(
            "Iteration| Number of Neighborhoods|   Current Solution   |"
            "  Incumbent Solution ",
            true);
        utility::print(
            "         |      All     Performed |   Objective (Viol.)  | "
            "  Objective (Viol.)  ",
            true);
        utility::print(
            "---------+------------------------+----------------------+--------"
            "--------------",
            true);
    }

    /*************************************************************************/
    inline void print_table_initial(const bool a_IS_ENABLED_PRINT) {
        if (!a_IS_ENABLED_PRINT) {
            return;
        }

        const auto& STATE = m_state_manager.state();
        const auto  SIGN  = m_model_ptr->sign();

        const auto& CURRENT_SOLUTION_SCORE = STATE.current_solution_score;
        const auto& INCUMBENT_SOLUTION_SCORE =
            m_global_state_ptr->incumbent_holder
                .global_augmented_incumbent_score();

        std::string color_current_feasible_begin   = "";
        std::string color_current_feasible_end     = "";
        std::string color_incumbent_feasible_begin = "";
        std::string color_incumbent_feasible_end   = "";

#ifdef _PRINTEMPS_STYLING
        if (CURRENT_SOLUTION_SCORE.is_feasible) {
            color_current_feasible_begin = constant::CYAN;
            color_current_feasible_end   = constant::NO_COLOR;
        }

        if (INCUMBENT_SOLUTION_SCORE.is_feasible) {
            color_incumbent_feasible_begin = constant::CYAN;
            color_incumbent_feasible_end   = constant::NO_COLOR;
        }
#endif

        std::printf(
            " INITIAL |          -           - | %9.2e %s(%8.2e)%s | %9.2e "
            "%s(%8.2e)%s\n",
            CURRENT_SOLUTION_SCORE.objective * SIGN,
            color_current_feasible_begin.c_str(),
            CURRENT_SOLUTION_SCORE.is_feasible
                ? 0.0
                : CURRENT_SOLUTION_SCORE.total_violation,  //
            color_current_feasible_end.c_str(),
            INCUMBENT_SOLUTION_SCORE.objective * SIGN,
            color_incumbent_feasible_begin.c_str(),
            std::max(INCUMBENT_SOLUTION_SCORE.total_violation, 0.0) * SIGN,
            color_incumbent_feasible_end.c_str());
    }

    /*************************************************************************/
    inline void print_table_body(const bool a_IS_ENABLED_PRINT) {
        if (!a_IS_ENABLED_PRINT) {
            return;
        }

        const auto& STATE = m_state_manager.state();
        const auto  SIGN  = m_model_ptr->sign();

        const auto& CURRENT_SOLUTION_SCORE = STATE.current_solution_score;
        const auto& INCUMBENT_SOLUTION_SCORE =
            m_global_state_ptr->incumbent_holder
                .global_augmented_incumbent_score();

        char mark_current                    = ' ';
        char mark_global_augmented_incumbent = ' ';

        if (STATE.update_status &  //
            solution::IncumbentHolderConstant::
                STATUS_FEASIBLE_INCUMBENT_UPDATE) {
            mark_current                    = '*';
            mark_global_augmented_incumbent = '*';
        } else if (STATE.update_status &  //
                   solution::IncumbentHolderConstant::
                       STATUS_GLOBAL_AUGMENTED_INCUMBENT_UPDATE) {
            mark_current                    = '#';
            mark_global_augmented_incumbent = '#';
        }

        std::string color_current_feasible_begin   = "";
        std::string color_current_feasible_end     = "";
        std::string color_incumbent_feasible_begin = "";
        std::string color_incumbent_feasible_end   = "";
        std::string color_incumbent_update_begin   = "";
        std::string color_incumbent_update_end     = "";

#ifdef _PRINTEMPS_STYLING
        if (CURRENT_SOLUTION_SCORE.is_feasible) {
            color_current_feasible_begin = constant::CYAN;
            color_current_feasible_end   = constant::NO_COLOR;
        }

        if (INCUMBENT_SOLUTION_SCORE.is_feasible) {
            color_incumbent_feasible_begin = constant::CYAN;
            color_incumbent_feasible_end   = constant::NO_COLOR;
        }

        if (STATE.update_status &  //
            solution::IncumbentHolderConstant::
                STATUS_GLOBAL_AUGMENTED_INCUMBENT_UPDATE) {
            color_current_feasible_begin = constant::YELLOW;
            color_current_feasible_end   = constant::NO_COLOR;

            color_incumbent_feasible_begin = constant::YELLOW;
            color_incumbent_feasible_end   = constant::NO_COLOR;

            color_incumbent_update_begin = constant::YELLOW;
            color_incumbent_update_end   = constant::NO_COLOR;
        }
#endif

        std::printf(  //
            "%8d | %10d  %10d |%s%c%9.2e%s %s(%8.2e)%s |%s%c%9.2e%s "
            "%s(%8.2e)%s\n",
            STATE.iteration,                  //
            STATE.number_of_moves,            //
            STATE.number_of_performed_moves,  //
            color_incumbent_update_begin.c_str(),
            mark_current,  //
            CURRENT_SOLUTION_SCORE.objective * SIGN,
            color_incumbent_update_end.c_str(),
            color_current_feasible_begin.c_str(),
            CURRENT_SOLUTION_SCORE.is_feasible
                ? 0.0
                : CURRENT_SOLUTION_SCORE.total_violation,
            color_current_feasible_end.c_str(),  //
            color_incumbent_update_begin.c_str(),
            mark_global_augmented_incumbent,
            INCUMBENT_SOLUTION_SCORE.objective * SIGN,
            color_incumbent_update_end.c_str(),
            color_incumbent_feasible_begin.c_str(),
            std::max(INCUMBENT_SOLUTION_SCORE.total_violation, 0.0) * SIGN,
            color_incumbent_feasible_end.c_str());
    }

    /*************************************************************************/
    inline void print_table_footer(const bool a_IS_ENABLED_PRINT) {
        if (!a_IS_ENABLED_PRINT) {
            return;
        }

        utility::print(
            "---------+------------------------+----------------------+--------"
            "--------------",
            true);
        utility::print_info(  //
            " -- *: Feasible incumbent solution was updated.", true);
        utility::print_info(  //
            " -- #: Global incumbent solution was updated.", true);
        utility::print_single_line(true);
    }

   public:
    /*************************************************************************/
    LocalSearchCore(void) {
        this->initialize();
    }

    /*************************************************************************/
    LocalSearchCore(
        model::Model<T_Variable, T_Expression>* a_model_ptr,         //
        GlobalState<T_Variable, T_Expression>*  a_global_state_ptr,  //
        const solution::SparseSolution<T_Variable, T_Expression>&    //
                              a_INITIAL_SOLUTION,                    //
        const option::Option& a_OPTION) {
        this->initialize();
        this->setup(a_model_ptr,         //
                    a_global_state_ptr,  //
                    a_INITIAL_SOLUTION,  //
                    a_OPTION);
    }

    /*************************************************************************/
    inline void initialize(void) {
        m_model_ptr        = nullptr;
        m_global_state_ptr = nullptr;
        m_initial_solution.initialize();
        m_option.initialize();

        m_feasible_solutions.clear();
        m_incumbent_solutions.clear();

        m_state_manager.initialize();
        m_result.initialize();
        m_get_rand_mt.seed(0);
    }

    /*************************************************************************/
    inline void setup(                                               //
        model::Model<T_Variable, T_Expression>* a_model_ptr,         //
        GlobalState<T_Variable, T_Expression>*  a_global_state_ptr,  //
        const solution::SparseSolution<T_Variable, T_Expression>&
                              a_INITIAL_SOLUTION,  //
        const option::Option& a_OPTION) {
        m_model_ptr        = a_model_ptr;
        m_global_state_ptr = a_global_state_ptr;
        m_model_ptr->import_solution(a_INITIAL_SOLUTION);
        m_initial_solution = m_model_ptr->export_dense_solution();
        m_option           = a_OPTION;

        m_feasible_solutions.clear();
        m_incumbent_solutions.clear();
    }

    /*************************************************************************/
    inline void run(void) {
        /**
         * Start to measure computational time.
         */
        utility::TimeKeeper time_keeper;
        time_keeper.set_start_time();

        const auto& STATE = m_state_manager.state();

        /**
         * Preprocess.
         */
        this->preprocess();

        /**
         * Prepare an step size adjuster for integer moves.
         */
        IntegerStepSizeAdjuster integer_step_size_adjuster(m_model_ptr,
                                                           m_option);
        std::vector<solution::SolutionScore> trial_solution_scores;
        std::vector<int>                     move_indices;
        std::unordered_set<
            model_component::Constraint<T_Variable, T_Expression>*>
                                                     constraint_ptrs;
        neighborhood::Move<T_Variable, T_Expression> move;
        int number_of_performed_moves = 0;

        /**
         * Print the header of optimization progress table and print the initial
         * solution status.
         */
        utility::print_single_line(m_option.output.verbose >=
                                   option::verbose::Outer);
        utility::print_message(
            "Local search starts.",
            m_option.output.verbose >= option::verbose::Outer);

        this->print_table_header(m_option.output.verbose >=
                                 option::verbose::Inner);
        this->print_table_initial(m_option.output.verbose >=
                                  option::verbose::Inner);

        /**
         * Iterations start.
         */
        m_state_manager.reset_iteration();
        while (true) {
            m_state_manager.set_elapsed_time(time_keeper.clock());

            /**
             * Terminate the loop if the time is over.
             */
            if (this->satisfy_time_over_terminate_condition()) {
                break;
            }

            /**
             * Terminate the loop if the iteration is over.
             */
            if (this->satisfy_iteration_over_terminate_condition()) {
                break;
            }

            /**
             * Terminate the loop if the objective value of the feasible
             * incumbent reaches the target value.
             */
            if (this->satisfy_reach_target_terminate_condition()) {
                break;
            }

            /**
             * Update the moves.
             */
            this->update_moves();

            /**
             *  Check the terminating condition (2).
             */
            if (this->satisfy_optimal_or_no_move_terminate_condition()) {
                break;
            }

            /**
             * Reserve elements for vectors by the number of the moves.
             */
            const auto& TRIAL_MOVE_PTRS =
                m_model_ptr->neighborhood().move_ptrs();
            trial_solution_scores.resize(STATE.number_of_moves);

            const auto NUMBER_OF_MOVES        = STATE.number_of_moves;
            const auto CURRENT_SOLUTION_SCORE = STATE.current_solution_score;
#ifdef _OPENMP
#pragma omp parallel for if (m_option.parallel                                \
                                 .is_enabled_move_evaluation_parallelization) \
    schedule(static)                                                          \
        num_threads(m_option.parallel.number_of_threads_move_evaluation)
#endif
            for (auto i = 0; i < NUMBER_OF_MOVES; i++) {
                /**
                 * The neighborhood solutions will be evaluated in parallel by
                 * fast or ordinary(slow) evaluation methods.
                 */
                if (TRIAL_MOVE_PTRS[i]->is_univariable_move) {
                    m_model_ptr->evaluate_single(&trial_solution_scores[i],  //
                                                 *TRIAL_MOVE_PTRS[i],        //
                                                 CURRENT_SOLUTION_SCORE);
                } else if (TRIAL_MOVE_PTRS[i]->is_selection_move) {
                    m_model_ptr->evaluate_selection(  //
                        &trial_solution_scores[i],    //
                        *TRIAL_MOVE_PTRS[i],          //
                        CURRENT_SOLUTION_SCORE);

                } else {
                    m_model_ptr->evaluate_multi(    //
                        &trial_solution_scores[i],  //
                        *TRIAL_MOVE_PTRS[i],        //
                        CURRENT_SOLUTION_SCORE);
                }
            }

            move_indices = utility::sequence(STATE.number_of_moves);
            std::stable_sort(  //
                move_indices.begin(), move_indices.end(),
                [&TRIAL_MOVE_PTRS](const auto& a_FIRST, const auto& a_SECOND) {
                    return TRIAL_MOVE_PTRS[a_FIRST]
                               ->related_constraint_ptrs.size() <
                           TRIAL_MOVE_PTRS[a_SECOND]
                               ->related_constraint_ptrs.size();
                });

            std::stable_sort(  //
                move_indices.begin(), move_indices.end(),
                [&trial_solution_scores](const auto a_FIRST,
                                         const auto a_SECOND) {
                    return trial_solution_scores[a_FIRST]
                               .global_augmented_objective <
                           trial_solution_scores[a_SECOND]
                               .global_augmented_objective;
                });

            move.alterations.reserve(m_model_ptr->number_of_variables());
            constraint_ptrs.clear();
            number_of_performed_moves = 0;
            move.initialize();
            for (auto i = 0; i < NUMBER_OF_MOVES; i++) {
                auto& score    = trial_solution_scores[move_indices[i]];
                auto& move_ptr = TRIAL_MOVE_PTRS[move_indices[i]];

                if (score.is_feasible) {
                    if (!score.is_objective_improvable) {
                        break;
                    }
                } else {
                    if (!(score.total_violation <
                          CURRENT_SOLUTION_SCORE.total_violation)) {
                        break;
                    }
                }

                bool has_intersection = false;

                for (const auto& constraint_ptr :
                     move_ptr->related_constraint_ptrs) {
                    if (!constraint_ptr->is_enabled()) {
                        continue;
                    }
                    if (constraint_ptrs.find(constraint_ptr) !=
                        constraint_ptrs.end()) {
                        has_intersection = true;
                        break;
                    }
                }

                if (has_intersection) {
                    continue;
                }

                move.alterations.insert(move.alterations.end(),
                                        move_ptr->alterations.begin(),
                                        move_ptr->alterations.end());

                move.related_constraint_ptrs.insert(
                    move.related_constraint_ptrs.end(),
                    move_ptr->related_constraint_ptrs.begin(),
                    move_ptr->related_constraint_ptrs.end());

                constraint_ptrs.insert(
                    move_ptr->related_constraint_ptrs.begin(),
                    move_ptr->related_constraint_ptrs.end());

                if (m_option.neighborhood
                        .is_enabled_integer_step_size_adjuster &&
                    move_ptr->sense == neighborhood::MoveSense::Integer) {
                    integer_step_size_adjuster.adjust(&move,
                                                      CURRENT_SOLUTION_SCORE);
                }

                number_of_performed_moves++;
            }

            bool is_found_improving_solution = move.alterations.size() > 0;

            /**
             * Terminate the loop if there is no improving solution in the
             * checked neighborhood.
             */
            if (this->satisfy_local_optimal_terminate_condition(
                    is_found_improving_solution)) {
                break;
            }

            solution::SolutionScore solution_score;
            m_model_ptr->evaluate_multi(  //
                &solution_score,          //
                move,                     //
                CURRENT_SOLUTION_SCORE);

            /**
             * Update the model by the selected move.
             */
            m_model_ptr->update(move);

            for (auto&& alteration : move.alterations) {
                if (alteration.first->sense() ==
                        model_component::VariableSense::Selection &&
                    alteration.second == 1) {
                    alteration.first->select();
                }
            }

            /**
             * Update the memory.
             */
            this->update_memory(&move);

            /**
             * Update the state.
             */
            m_state_manager.update(&move,                        //
                                   number_of_performed_moves,    //
                                   is_found_improving_solution,  //
                                   solution_score);

            /**
             * Store the current feasible solution.
             */
            if (m_option.output.is_enabled_store_feasible_solutions &&
                STATE.current_solution_score.is_feasible) {
                m_model_ptr
                    ->update_dependent_variables_and_disabled_constraints();
                m_feasible_solutions.push_back(
                    m_model_ptr->export_sparse_solution());
            }

            /**
             * Print the optimization progress.
             */
            if ((STATE.iteration %
                 std::max(m_option.local_search.log_interval, 1)) == 0 ||
                STATE.update_status > 1) {
                this->print_table_body(m_option.output.verbose >=
                                       option::verbose::Inner);
            }

            m_state_manager.next_iteration();
        }

        /**
         * Print the footer of the optimization progress table.
         */
        this->print_table_footer(m_option.output.verbose >=
                                 option::verbose::Inner);

        /**
         * Store the incumbent solution.
         */
        if (STATE.total_update_status &  //
            solution::IncumbentHolderConstant::
                STATUS_GLOBAL_AUGMENTED_INCUMBENT_UPDATE) {
            m_incumbent_solutions.push_back(
                m_global_state_ptr->incumbent_holder
                    .global_augmented_incumbent_solution()
                    .to_sparse());
        }

        /**
         * Postprocess.
         */
        m_state_manager.set_elapsed_time(time_keeper.clock());
        this->postprocess();
    }

    /*************************************************************************/
    inline const std::vector<
        solution::SparseSolution<T_Variable, T_Expression>>&
    feasible_solutions(void) const {
        return m_feasible_solutions;
    }

    /*************************************************************************/
    inline const std::vector<
        solution::SparseSolution<T_Variable, T_Expression>>&
    incumbent_solutions(void) const {
        return m_incumbent_solutions;
    }

    /*************************************************************************/
    inline const LocalSearchCoreResult<T_Variable, T_Expression>& result(
        void) const {
        return m_result;
    }
};
}  // namespace printemps::solver::local_search::core

#endif
/*****************************************************************************/
// END
/*****************************************************************************/