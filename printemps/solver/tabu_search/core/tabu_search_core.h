/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_TABU_SEARCH_CORE_TABU_SEARCH_CORE_H__
#define PRINTEMPS_SOLVER_TABU_SEARCH_CORE_TABU_SEARCH_CORE_H__

#include "../../memory.h"
#include "tabu_search_core_move_score.h"
#include "tabu_search_core_move_evaluator.h"
#include "tabu_search_core_print.h"
#include "tabu_search_core_termination_status.h"
#include "tabu_search_core_state.h"
#include "tabu_search_core_result.h"

namespace printemps {
namespace solver {
namespace tabu_search {
namespace core {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class TabuSearchCore : public AbstractSolverCore<T_Variable, T_Expression> {
   private:
    TabuSearchCoreState<T_Variable, T_Expression>  m_state;
    TabuSearchCoreResult<T_Variable, T_Expression> m_result;
    std::mt19937                                   m_get_rand_mt;

    /*************************************************************************/
    inline void preprocess(void) {
        /**
         * Copy arguments as local variables.
         */
        auto  model_ptr            = this->m_model_ptr;
        auto  memory_ptr           = this->m_memory_ptr;
        auto  incumbent_holder_ptr = this->m_incumbent_holder_ptr;
        auto& state                = m_state;
        auto& option               = this->m_option;

        /**
         * Reset the local augmented incumbent.
         */
        incumbent_holder_ptr->reset_local_augmented_incumbent();

        /**
         * Reset the feasible solutions storage.
         */
        this->m_feasible_solutions.clear();

        /**
         * Prepare a random generator, which is used for shuffling moves.
         */
        m_get_rand_mt.seed(option.tabu_search.seed);

        /**
         * Initialize the solution and update the model.
         */
        model_ptr->import_variable_values(
            this->m_initial_variable_value_proxies);
        model_ptr->update();

        state.current_solution_score  = model_ptr->evaluate({});
        state.previous_solution_score = state.current_solution_score;

        state.update_status = incumbent_holder_ptr->try_update_incumbent(
            model_ptr, state.current_solution_score);
        state.total_update_status =
            solution::IncumbentHolderConstant::STATUS_NOT_UPDATED;

        /**
         * Reset the last update iterations.
         */
        memory_ptr->reset_last_update_iterations();

        // state.current_solution_score.initialize();
        // state.previous_solution_score.initialize();

        state.current_move.initialize();
        state.previous_move.initialize();

        /**
         * Set up the tabu tenure and related parameters.
         */
        state.original_tabu_tenure =
            std::min(option.tabu_search.initial_tabu_tenure,
                     model_ptr->number_of_mutable_variables());
        state.tabu_tenure = state.original_tabu_tenure;

        state.previous_primal_intensity = 0.0;
        state.current_primal_intensity  = 0.0;
        state.intensity_increase_count  = 0;
        state.intensity_decrease_count  = 0;

        state.last_tabu_tenure_updated_iteration = 0;

        /**
         * Reset the variable improvability.
         */
        model_ptr->reset_variable_objective_improvabilities();
        model_ptr->reset_variable_feasibility_improvabilities();

        /**
         * Prepare other local variables.
         */
        state.number_of_all_neighborhoods         = 0;
        state.number_of_feasible_neighborhoods    = 0;
        state.number_of_permissible_neighborhoods = 0;
        state.number_of_improvable_neighborhoods  = 0;

        state.last_local_augmented_incumbent_update_iteration  = -1;
        state.last_global_augmented_incumbent_update_iteration = -1;
        state.last_feasible_incumbent_update_iteration         = -1;

        state.local_augmented_incumbent_update_count = 0;

        state.termination_status =
            TabuSearchCoreTerminationStatus::ITERATION_OVER;

        state.is_few_permissible_neighborhood = false;
        state.is_found_new_feasible_solution  = false;
        state.is_aspirated                    = false;

        state.number_of_evaluated_moves = 0;

        state.min_objective = state.current_solution_score.objective;
        state.max_objective = state.current_solution_score.objective;

        state.min_local_augmented_objective =
            state.current_solution_score.local_augmented_objective;
        state.max_local_augmented_objective =
            state.current_solution_score.local_augmented_objective;

        state.min_global_augmented_objective =
            state.current_solution_score.global_augmented_objective;
        state.max_global_augmented_objective =
            state.current_solution_score.global_augmented_objective;
        state.min_local_penalty = HUGE_VALF;

        state.oscillation = 0.0;

        if (!m_state.current_solution_score.is_feasible) {
            m_state.min_local_penalty =
                m_state.current_solution_score.local_penalty;
        }
    }

    /*************************************************************************/
    inline void postprocess(void) {
        /**
         * Prepare the result.
         */
        m_result = TabuSearchCoreResult<T_Variable, T_Expression>(m_state);
    }

    /*************************************************************************/
    inline bool satisfy_time_over_terminate_condition(
        const double a_ELAPSED_TIME) {
        auto& state  = m_state;
        auto& option = this->m_option;

        if (a_ELAPSED_TIME > option.tabu_search.time_max) {
            state.termination_status =
                TabuSearchCoreTerminationStatus::TIME_OVER;
            return true;
        }

        if (a_ELAPSED_TIME + option.tabu_search.time_offset > option.time_max) {
            state.termination_status =
                TabuSearchCoreTerminationStatus::TIME_OVER;
            return true;
        }
        return false;
    }

    /*************************************************************************/
    inline bool satisfy_iteration_over_terminate_condition(void) {
        auto& state  = m_state;
        auto& option = this->m_option;
        if (state.iteration >= option.tabu_search.iteration_max) {
            state.termination_status =
                TabuSearchCoreTerminationStatus::ITERATION_OVER;
            return true;
        }
        return false;
    }

    /*************************************************************************/
    inline bool satisfy_reach_target_terminate_condition(void) {
        auto  incumbent_holder_ptr = this->m_incumbent_holder_ptr;
        auto& state                = m_state;
        auto& option               = this->m_option;
        if (incumbent_holder_ptr->feasible_incumbent_objective() <=
            option.target_objective_value) {
            state.termination_status =
                TabuSearchCoreTerminationStatus::REACH_TARGET;
            return true;
        }
        return false;
    }

    /*************************************************************************/
    inline bool satisfy_pruning_terminate_condition(void) {
        auto& state  = m_state;
        auto& option = this->m_option;
        if (state.local_augmented_incumbent_update_count >
            option.tabu_search.pruning_rate_threshold *
                option.tabu_search.iteration_max) {
            state.termination_status =
                TabuSearchCoreTerminationStatus::EARLY_STOP;
            return true;
        }
        return false;
    }

    /*************************************************************************/
    inline bool satisfy_optimal_or_no_move_terminate_condition(void) {
        /**
         * If the number of the moves is zero, the tabu search iterations
         * will be terminated.
         */
        auto  model_ptr = this->m_model_ptr;
        auto& state     = m_state;

        if (state.number_of_moves > 0) {
            return false;
        }

        if (model_ptr->is_linear() && model_ptr->is_feasible()) {
            /**
             * If the current solution is feasible and there is no
             * improvable solution, the solution should be an optimum.
             * It can happen for decomp2 instance in MIPLIB 2017.
             */
            state.termination_status = TabuSearchCoreTerminationStatus::OPTIMAL;
            for (const auto& variable_ptr :
                 model_ptr->variable_reference().variable_ptrs) {
                if (variable_ptr->is_objective_improvable()) {
                    state.termination_status =
                        TabuSearchCoreTerminationStatus::NO_MOVE;
                    break;
                }
            }
            return true;
        } else {
            state.termination_status = TabuSearchCoreTerminationStatus::NO_MOVE;
            return true;
        }

        return false;
    }

    /*************************************************************************/
    inline bool satisfy_penalty_coefficient_too_large_terminate_condition(
        const std::vector<solution::SolutionScore>& a_TRIAL_SOLUTION_SCORES) {
        /**
         * If the local penalty us sufficiently larger than objective
         * sensitivity, the current loop will be terminated and the local
         * penalty coefficients will be adjusted.
         */
        auto& state = m_state;

        constexpr int    ITERATION_MIN = 10;
        constexpr double MARGIN        = 100.0;

        if (state.iteration <= ITERATION_MIN) {
            return false;
        }

        if (!state.current_solution_score.is_feasible) {
            return false;
        }

        double min_infeasible_local_penalty  = HUGE_VALF;
        bool   has_infeasible_trial_solution = false;

        for (const auto& score : a_TRIAL_SOLUTION_SCORES) {
            if (!score.is_feasible) {
                min_infeasible_local_penalty =
                    std::min(min_infeasible_local_penalty, score.local_penalty);
                has_infeasible_trial_solution = true;
            }
        }

        if (!has_infeasible_trial_solution) {
            return false;
        }

        const auto SCORE_PTR_PAIR = std::minmax_element(
            a_TRIAL_SOLUTION_SCORES.begin(), a_TRIAL_SOLUTION_SCORES.end(),
            [](const auto& a_FIRST, const auto& a_SECOND) {
                return a_FIRST.objective_improvement <
                       a_SECOND.objective_improvement;
            });

        double max_objective_sensitivity =
            std::max(SCORE_PTR_PAIR.second->objective_improvement,
                     -SCORE_PTR_PAIR.first->objective_improvement);

        if (max_objective_sensitivity * MARGIN < min_infeasible_local_penalty) {
            state.termination_status =
                TabuSearchCoreTerminationStatus::EARLY_STOP;
            return true;
        }

        return false;
    }

    /*************************************************************************/
    inline void update_moves(void) {
        auto  model_ptr = this->m_model_ptr;
        auto& option    = this->m_option;
        auto& state     = this->m_state;

        bool accept_all                    = true;
        bool accept_objective_improvable   = true;
        bool accept_feasibility_improvable = true;

        if (!model_ptr->is_linear() ||
            option.improvability_screening_mode ==
                option::improvability_screening_mode::Off) {
            model_ptr->neighborhood().update_moves(
                accept_all,                     //
                accept_objective_improvable,    //
                accept_feasibility_improvable,  //
                option.is_enabled_parallel_neighborhood_update);

            m_state.number_of_moves =
                model_ptr->neighborhood().move_ptrs().size();
            return;
        }

        /**
         * If the option improvability_screening_mode is not Off, only
         * improvable moves will be generated.
         */
        const auto CHANGED_VARIABLE_PTRS = utility::to_vector(
            neighborhood::related_variable_ptrs(state.current_move));

        if (state.iteration == 0) {
            model_ptr->update_variable_objective_improvabilities();
        } else {
            model_ptr->update_variable_objective_improvabilities(
                CHANGED_VARIABLE_PTRS);
        }

        switch (option.improvability_screening_mode) {
            case option::improvability_screening_mode::Soft: {
                if (model_ptr->is_feasible()) {
                    accept_all                    = false;
                    accept_objective_improvable   = true;
                    accept_feasibility_improvable = false;
                } else {
                    model_ptr->reset_variable_feasibility_improvabilities();
                    model_ptr->update_variable_feasibility_improvabilities(
                        model_ptr->violative_constraint_ptrs());

                    accept_all                    = false;
                    accept_objective_improvable   = true;
                    accept_feasibility_improvable = true;
                }
                break;
            }
            case option::improvability_screening_mode::Aggressive: {
                if (model_ptr->is_feasible()) {
                    accept_all                    = false;
                    accept_objective_improvable   = true;
                    accept_feasibility_improvable = false;
                } else {
                    model_ptr->reset_variable_feasibility_improvabilities();
                    model_ptr->update_variable_feasibility_improvabilities(
                        model_ptr->violative_constraint_ptrs());

                    accept_all                    = false;
                    accept_objective_improvable   = false;
                    accept_feasibility_improvable = true;
                }
                break;
            }
            case option::improvability_screening_mode::Intensive: {
                if (model_ptr->is_feasible()) {
                    accept_all                    = false;
                    accept_objective_improvable   = true;
                    accept_feasibility_improvable = false;
                } else {
                    if (state.iteration == 0) {
                        model_ptr->reset_variable_feasibility_improvabilities();
                        model_ptr
                            ->update_variable_feasibility_improvabilities();
                    } else {
                        const auto CHANGED_CONSTRAINT_PTRS = utility::to_vector(
                            state.current_move.related_constraint_ptrs);
                        model_ptr->reset_variable_feasibility_improvabilities(
                            CHANGED_CONSTRAINT_PTRS);
                        model_ptr->update_variable_feasibility_improvabilities(
                            CHANGED_CONSTRAINT_PTRS);
                    }

                    accept_all                    = false;
                    accept_objective_improvable   = false;
                    accept_feasibility_improvable = true;
                }
                break;
            }
            default: {
                throw std::logic_error(utility::format_error_location(
                    __FILE__, __LINE__, __func__,
                    "The specified improvability screening mode is "
                    "invalid."));
            }
        }

        model_ptr->neighborhood().update_moves(
            accept_all,                     //
            accept_objective_improvable,    //
            accept_feasibility_improvable,  //
            option.is_enabled_parallel_neighborhood_update);

        m_state.number_of_moves = model_ptr->neighborhood().move_ptrs().size();
    }

    /*************************************************************************/
    inline void curtail_moves(void) {
        m_state.number_of_moves = static_cast<int>(
            floor(this->m_option.tabu_search.move_preserve_rate *
                  m_state.number_of_moves));
    }

    /*************************************************************************/
    inline void select_move(
        const std::vector<double>&                  a_TOTAL_SCORES,
        const std::vector<TabuSearchCoreMoveScore>& a_TRIAL_MOVE_SCORES,
        const std::vector<solution::SolutionScore>& a_TRIAL_SOLUTION_SCORES) {
        auto  incumbent_holder_ptr = this->m_incumbent_holder_ptr;
        auto& option               = this->m_option;
        auto& state                = this->m_state;

        if (state.iteration <
            option.tabu_search.number_of_initial_modification) {
            /**
             * For diversification, the move for next solution will be
             * randomly selected for initial several iteration.
             */
            state.selected_index = m_get_rand_mt() % state.number_of_moves;
            state.is_aspirated   = false;
            return;
        }

        const int ARGMIN_TOTAL_SCORE = utility::argmin(a_TOTAL_SCORES);

        /**
         * The move for next solution will be determined by evaluations
         * of solutions and moves after the inital modifications.
         */
        state.selected_index = ARGMIN_TOTAL_SCORE;
        state.is_aspirated   = false;

        /**
         * A move which improves the augmented incumbent solution can be
         * accepted (optional).
         */
        if (!option.tabu_search.ignore_tabu_if_global_incumbent) {
            return;
        }

        const int ARGMIN_GLOBAL_AUGMENTED_OBJECTIVE = std::distance(
            a_TRIAL_SOLUTION_SCORES.begin(),
            min_element(a_TRIAL_SOLUTION_SCORES.begin(),
                        a_TRIAL_SOLUTION_SCORES.end(),
                        [](const auto& a_FIRST, const auto& a_SECOND) {
                            return a_FIRST.global_augmented_objective <
                                   a_SECOND.global_augmented_objective;
                        }));

        if (a_TRIAL_SOLUTION_SCORES[ARGMIN_GLOBAL_AUGMENTED_OBJECTIVE]
                    .global_augmented_objective +
                constant::EPSILON <
            incumbent_holder_ptr->global_augmented_incumbent_objective()) {
            state.selected_index = ARGMIN_GLOBAL_AUGMENTED_OBJECTIVE;
            if (!a_TRIAL_MOVE_SCORES[state.selected_index].is_permissible) {
                state.is_aspirated = true;
            }
        }
    }

    /*************************************************************************/
    inline void update_state(
        neighborhood::Move<T_Variable, T_Expression>* a_selected_move_ptr,
        const std::vector<TabuSearchCoreMoveScore>&   a_TRIAL_MOVE_SCORES,
        const std::vector<solution::SolutionScore>&   a_TRIAL_SOLUTION_SCORES) {
        auto  model_ptr            = this->m_model_ptr;
        auto  incumbent_holder_ptr = this->m_incumbent_holder_ptr;
        auto& option               = this->m_option;
        auto& state                = this->m_state;

        state.previous_solution_score = state.current_solution_score;
        state.previous_move           = state.current_move;

        state.current_solution_score =
            a_TRIAL_SOLUTION_SCORES[state.selected_index];
        state.current_move = *a_selected_move_ptr;

        state.min_objective = std::min(state.min_objective,
                                       state.current_solution_score.objective);
        state.max_objective = std::max(state.max_objective,
                                       state.current_solution_score.objective);

        state.min_global_augmented_objective =
            std::min(state.min_global_augmented_objective,
                     state.current_solution_score.global_augmented_objective);
        state.max_global_augmented_objective =
            std::max(state.max_global_augmented_objective,
                     state.current_solution_score.global_augmented_objective);

        state.min_local_augmented_objective =
            std::min(state.min_local_augmented_objective,
                     state.current_solution_score.local_augmented_objective);
        state.max_local_augmented_objective =
            std::max(state.max_local_augmented_objective,
                     state.current_solution_score.local_augmented_objective);

        state.oscillation +=
            fabs(state.current_solution_score.local_augmented_objective -
                 -state.previous_solution_score.local_augmented_objective);

        state.number_of_evaluated_moves += state.number_of_moves;

        if (!state.current_solution_score.is_feasible) {
            state.min_local_penalty =
                std::min(state.min_local_penalty,
                         state.current_solution_score.local_penalty);
        }

        state.update_status = incumbent_holder_ptr->try_update_incumbent(
            model_ptr, state.current_solution_score);
        state.total_update_status =
            state.update_status | state.total_update_status;

        if (state.current_solution_score.is_feasible) {
            state.is_found_new_feasible_solution = true;
        }

        /**
         * Calculate various statistics for logging.
         */
        if (state.update_status & solution::IncumbentHolderConstant::
                                      STATUS_LOCAL_AUGMENTED_INCUMBENT_UPDATE) {
            state.last_local_augmented_incumbent_update_iteration =
                state.iteration;
        }

        if (state.update_status &
            solution::IncumbentHolderConstant::
                STATUS_GLOBAL_AUGMENTED_INCUMBENT_UPDATE) {
            state.last_global_augmented_incumbent_update_iteration =
                state.iteration;
        }

        if (state.update_status & solution::IncumbentHolderConstant::
                                      STATUS_FEASIBLE_INCUMBENT_UPDATE) {
            state.last_feasible_incumbent_update_iteration = state.iteration;
        }

        /**
         * For pruning, count updating of the local augmented incumbent
         * without global augmented incumbent improvement.
         */
        if (state.update_status ==
            solution::IncumbentHolderConstant::
                STATUS_LOCAL_AUGMENTED_INCUMBENT_UPDATE) {
            state.local_augmented_incumbent_update_count++;
        } else if (state.update_status &
                   solution::IncumbentHolderConstant::
                       STATUS_GLOBAL_AUGMENTED_INCUMBENT_UPDATE) {
            state.local_augmented_incumbent_update_count = 0;
        }

        /**
         * Calculate the number of moves for each type.
         */
        state.number_of_all_neighborhoods = state.number_of_moves;
        if (state.iteration % std::max(option.tabu_search.log_interval, 1) ==
                0 ||
            state.update_status > 0) {
            state.number_of_feasible_neighborhoods    = 0;
            state.number_of_permissible_neighborhoods = 0;
            state.number_of_improvable_neighborhoods  = 0;

            for (const auto& score : a_TRIAL_SOLUTION_SCORES) {
                if (score.is_feasible) {
                    state.number_of_feasible_neighborhoods++;
                }
                if (score.is_objective_improvable ||
                    score.is_feasibility_improvable) {
                    state.number_of_improvable_neighborhoods++;
                }
            }

            for (const auto& score : a_TRIAL_MOVE_SCORES) {
                if (score.is_permissible) {
                    state.number_of_permissible_neighborhoods++;
                }
            }

            if (state.number_of_permissible_neighborhoods == 0) {
                state.is_few_permissible_neighborhood = true;
            }
        } else {
            bool is_few_permissible_neighborhood_temp = true;
            for (const auto& score : a_TRIAL_MOVE_SCORES) {
                if (score.is_permissible) {
                    is_few_permissible_neighborhood_temp = false;
                    break;
                }
            }
            if (is_few_permissible_neighborhood_temp) {
                state.is_few_permissible_neighborhood = true;
            }
        }
    }

    /*************************************************************************/
    inline void update_memory(void) {
        auto  memory_ptr = this->m_memory_ptr;
        auto& option     = this->m_option;
        auto& state      = this->m_state;

        const int RANDOM_WIDTH = static_cast<int>(
            option.tabu_search.tabu_tenure_randomize_rate * state.tabu_tenure);
        memory_ptr->update(state.current_move,  //
                           state.iteration,     //
                           RANDOM_WIDTH,        //
                           &m_get_rand_mt);
    }

    /*************************************************************************/
    inline void update_chain_moves(void) {
        auto  model_ptr = this->m_model_ptr;
        auto& option    = this->m_option;
        auto& state     = this->m_state;

        if ((state.previous_move.sense == neighborhood::MoveSense::Binary &&
             state.current_move.sense == neighborhood::MoveSense::Binary &&
             state.previous_move.alterations.front().second !=
                 state.current_move.alterations.front().second) ||
            (state.previous_move.sense == neighborhood::MoveSense::Chain &&
             state.current_move.sense == neighborhood::MoveSense::Chain) ||
            (state.previous_move.sense == neighborhood::MoveSense::TwoFlip &&
             state.current_move.sense == neighborhood::MoveSense::TwoFlip)) {
            neighborhood::Move<T_Variable, T_Expression> chain_move;
            if (state.previous_move.alterations.front().first <
                state.current_move.alterations.front().first)
                chain_move = state.previous_move + state.current_move;
            else {
                chain_move = state.current_move + state.previous_move;
            }

            if (chain_move.overlap_rate >
                    option.chain_move_overlap_rate_threshold &&
                !neighborhood::has_duplicate_variable(chain_move)) {
                auto back_chain_move = chain_move;
                for (auto&& alteration : back_chain_move.alterations) {
                    alteration.second = 1 - alteration.second;
                }
                model_ptr->neighborhood().chain().register_move(chain_move);
                model_ptr->neighborhood().chain().register_move(
                    back_chain_move);
            }
        }
    }

    /*************************************************************************/
    inline void update_tabu_tenure(void) {
        auto  model_ptr  = this->m_model_ptr;
        auto  memory_ptr = this->m_memory_ptr;
        auto& option     = this->m_option;
        auto& state      = this->m_state;

        if ((state.update_status &
             solution::IncumbentHolderConstant::
                 STATUS_GLOBAL_AUGMENTED_INCUMBENT_UPDATE) &&
            state.tabu_tenure > state.original_tabu_tenure) {
            /**
             * The tabu tenure will be reverted to the original value if it has
             * been increased and the global incumbent is updated.
             */
            state.tabu_tenure = state.original_tabu_tenure;
            state.last_tabu_tenure_updated_iteration = state.iteration;
            state.intensity_decrease_count           = 0;
            state.intensity_increase_count           = 0;
            utility::print_debug("Tabu tenure reverted: " +
                                     std::to_string(state.tabu_tenure) + ".",
                                 option.verbose >= option::verbose::Debug);
        } else if ((state.iteration -
                    state.last_tabu_tenure_updated_iteration) %
                       (state.tabu_tenure + 1) ==
                   0) {
            /**
             * The intensity of searching will be computed with the interval of
             * tabu_tenure+1. The tabu tenure will be increased if the intensity
             * has grown up, and decreased if the intensity has been reduced.
             */
            state.previous_primal_intensity = state.current_primal_intensity;
            state.current_primal_intensity  = memory_ptr->primal_intensity();

            if (state.current_primal_intensity >
                state.previous_primal_intensity) {
                state.intensity_increase_count++;
                state.intensity_decrease_count = 0;

                if (state.intensity_increase_count >
                    option.tabu_search.intensity_increase_count_threshold) {
                    state.intensity_increase_count = 0;
                    state.tabu_tenure =
                        std::min(state.tabu_tenure + 1,
                                 model_ptr->number_of_mutable_variables());
                    state.last_tabu_tenure_updated_iteration = state.iteration;
                    utility::print_debug(
                        "Tabu tenure increased: " +
                            std::to_string(state.tabu_tenure) + ".",
                        option.verbose >= option::verbose::Debug);
                }
            } else {
                state.intensity_decrease_count++;
                state.intensity_increase_count = 0;

                if (state.intensity_decrease_count >
                    option.tabu_search.intensity_decrease_count_threshold) {
                    state.intensity_decrease_count = 0;
                    state.tabu_tenure =
                        std::max(state.tabu_tenure - 1,
                                 std::max(1, state.original_tabu_tenure / 2));
                    state.last_tabu_tenure_updated_iteration = state.iteration;

                    utility::print_debug(
                        "Tabu tenure decreased: " +
                            std::to_string(state.tabu_tenure) + ".",
                        option.verbose >= option::verbose::Debug);
                }
            }
        }
    }

    /*****************************************************************************/
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
            "         |  All Feas. Perm. Impr. |   Aug.Obj.(Penalty)  | "
            "  Aug.Obj.  Feas.Obj ",
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

        auto  model_ptr            = this->m_model_ptr;
        auto  incumbent_holder_ptr = this->m_incumbent_holder_ptr;
        auto& state                = m_state;

        const auto SIGN = model_ptr->sign();

        std::printf(
            " INITIAL |          -           - | %9.2e(%9.2e) | %9.2e  %9.2e\n",
            state.current_solution_score.local_augmented_objective * SIGN,
            state.current_solution_score.is_feasible
                ? 0.0
                : state.current_solution_score.local_penalty,  //
            incumbent_holder_ptr->global_augmented_incumbent_objective() * SIGN,
            incumbent_holder_ptr->feasible_incumbent_objective() * SIGN);
    }

    /*************************************************************************/
    inline void print_table_body(const bool a_IS_ENABLED_PRINT) {
        if (!a_IS_ENABLED_PRINT) {
            return;
        }

        auto  model_ptr            = this->m_model_ptr;
        auto  incumbent_holder_ptr = this->m_incumbent_holder_ptr;
        auto& state                = m_state;

        const auto SIGN = model_ptr->sign();

        char mark_special_neighborhood_move  = ' ';
        char mark_current                    = ' ';
        char mark_global_augmented_incumbent = ' ';
        char mark_feasible_incumbent         = ' ';

        if (state.is_special_neighborhood_move) {
            mark_special_neighborhood_move = 's';
        }

        if (state.update_status &  //
            solution::IncumbentHolderConstant::
                STATUS_LOCAL_AUGMENTED_INCUMBENT_UPDATE) {
            mark_current = '!';
        }

        if (state.update_status &  //
            solution::IncumbentHolderConstant::
                STATUS_GLOBAL_AUGMENTED_INCUMBENT_UPDATE) {
            mark_current                    = '#';
            mark_global_augmented_incumbent = '#';
            if (state.is_aspirated) {
                mark_current                    = '@';
                mark_global_augmented_incumbent = '@';
            }
        }

        if (state.update_status &  //
            solution::IncumbentHolderConstant::
                STATUS_FEASIBLE_INCUMBENT_UPDATE) {
            mark_current                    = '*';
            mark_global_augmented_incumbent = '*';
            mark_feasible_incumbent         = '*';
            if (state.is_aspirated) {
                mark_current                    = '@';
                mark_global_augmented_incumbent = '@';
                mark_feasible_incumbent         = '@';
            }
        }

        auto int_format = [](const int a_VALUE) {
            if (a_VALUE >= 100000) {
                return utility::to_string(a_VALUE / 1000, "%4dk");
            } else {
                return utility::to_string(a_VALUE, "%5d");
            }
        };

        std::printf(  //
            "%8d%c|%s %s %s %s |%c%9.2e(%9.2e) |%c%9.2e %c%9.2e\n",
            state.iteration,
            mark_special_neighborhood_move,                                 //
            int_format(state.number_of_all_neighborhoods).c_str(),          //
            int_format(state.number_of_feasible_neighborhoods).c_str(),     //
            int_format(state.number_of_permissible_neighborhoods).c_str(),  //
            int_format(state.number_of_improvable_neighborhoods).c_str(),   //
            mark_current,                                                   //
            state.current_solution_score.local_augmented_objective * SIGN,  //
            state.current_solution_score.is_feasible
                ? 0.0
                : state.current_solution_score.local_penalty,  //
            mark_global_augmented_incumbent,                   //
            incumbent_holder_ptr->global_augmented_incumbent_objective() * SIGN,
            mark_feasible_incumbent,  //
            incumbent_holder_ptr->feasible_incumbent_objective() * SIGN);
    }
    /*************************************************************************/
    inline void print_table_footer(const bool a_IS_ENABLED_PRINT) {
        if (!a_IS_ENABLED_PRINT) {
            return;
        }

        utility::print(
            "---------+------------------------+----------------------+--------"
            "--------------");
    }

   public:
    /*************************************************************************/
    TabuSearchCore(void) {
        this->initialize();
    }

    /*************************************************************************/
    TabuSearchCore(model::Model<T_Variable, T_Expression>* a_model_ptr,       //
                   const std::vector<multi_array::ValueProxy<T_Variable>>&    //
                       a_INITIAL_VARIABLE_VALUE_PROXIES,                      //
                   solution::IncumbentHolder<T_Variable, T_Expression>*       //
                                                     a_incumbent_holder_ptr,  //
                   Memory<T_Variable, T_Expression>* a_memory_ptr,            //
                   const option::Option&             a_OPTION) {
        this->initialize();
        this->setup(a_model_ptr,                       //
                    a_INITIAL_VARIABLE_VALUE_PROXIES,  //
                    a_incumbent_holder_ptr,            //
                    a_memory_ptr,                      //
                    a_OPTION);
    }

    /*************************************************************************/
    virtual ~TabuSearchCore(void) {
        /// nothing to do
    }

    /*************************************************************************/
    inline void initialize(void) {
        AbstractSolverCore<T_Variable, T_Expression>::initialize();
        m_state.initialize();
        m_result.initialize();
        m_get_rand_mt.seed(0);
    }

    /*************************************************************************/
    inline void run(void) {
        /**
         * Start to measure computational time.
         */
        utility::TimeKeeper time_keeper;
        time_keeper.set_start_time();

        /**
         * Copy arguments as local variables.
         */
        auto  model_ptr          = this->m_model_ptr;
        auto  memory_ptr         = this->m_memory_ptr;
        auto& option             = this->m_option;
        auto& feasible_solutions = this->m_feasible_solutions;
        auto& state              = m_state;

        /**
         * Preprocess.
         */
        this->preprocess();

        /**
         * Prepare the move evaluator.
         */
        TabuSearchCoreMoveEvaluator<T_Variable, T_Expression> move_evaluator(
            model_ptr, memory_ptr, option);

        std::vector<solution::SolutionScore> trial_solution_scores;
        std::vector<TabuSearchCoreMoveScore> trial_move_scores;
        std::vector<double>                  total_scores;

        /**
         * Print the header of optimization progress table and print the initial
         * solution status.
         */
        utility::print_single_line(option.verbose >= option::verbose::Full);
        utility::print_message("Tabu Search starts.",
                               option.verbose >= option::verbose::Full);

        print_table_header(option.verbose >= option::verbose::Full);
        print_table_initial(option.verbose >= option::verbose::Full);

        /**
         * Iterations start.
         */
        state.iteration = 0;
        while (true) {
            /**
             *  Check the terminating condition (1).
             */
            const double ELAPSED_TIME = time_keeper.clock();
            if (this->satisfy_time_over_terminate_condition(ELAPSED_TIME)) {
                break;
            }

            if (this->satisfy_iteration_over_terminate_condition()) {
                break;
            }

            if (this->satisfy_reach_target_terminate_condition()) {
                break;
            }

            if (this->satisfy_pruning_terminate_condition()) {
                break;
            }

            /**
             * Update the moves.
             */
            this->update_moves();

            /**
             * Shuffle the moves.
             */
            if (option.tabu_search.is_enabled_shuffle) {
                model_ptr->neighborhood().shuffle_moves(&m_get_rand_mt);
            }

            /**
             * Curtail moves (optional).
             */
            if (option.tabu_search.is_enabled_move_curtail) {
                this->curtail_moves();
            }

            /**
             *  Check the terminating condition (2).
             */
            if (this->satisfy_optimal_or_no_move_terminate_condition()) {
                break;
            }

            /**
             * Reserve elements for vectors by the number of the moves.
             */
            const auto& TRIAL_MOVE_PTRS = model_ptr->neighborhood().move_ptrs();
            trial_solution_scores.resize(state.number_of_moves);
            trial_move_scores.resize(state.number_of_moves);
            total_scores.resize(state.number_of_moves);

            const auto NUMBER_OF_MOVES        = state.number_of_moves;
            const auto CURRENT_SOLUTION_SCORE = state.current_solution_score;
            const auto ITERATION              = m_state.iteration;
            const auto TABU_TENURE            = m_state.tabu_tenure;
#ifdef _OPENMP
#pragma omp parallel for if (option.is_enabled_parallel_evaluation) \
    schedule(static)
#endif
            for (auto i = 0; i < NUMBER_OF_MOVES; i++) {
                /**
                 * The neighborhood solutions will be evaluated in parallel by
                 * fast or ordinary(slow) evaluation methods.
                 */
#ifndef _MPS_SOLVER
                if (model_ptr->is_enabled_fast_evaluation()) {
#endif
                    if (TRIAL_MOVE_PTRS[i]->is_univariable_move) {
                        model_ptr->evaluate_single(     //
                            &trial_solution_scores[i],  //
                            *TRIAL_MOVE_PTRS[i],        //
                            CURRENT_SOLUTION_SCORE);
                    } else {
                        model_ptr->evaluate_multi(      //
                            &trial_solution_scores[i],  //
                            *TRIAL_MOVE_PTRS[i],        //
                            CURRENT_SOLUTION_SCORE);
                    }

#ifndef _MPS_SOLVER
                } else {
                    model_ptr->evaluate(&trial_solution_scores[i],  //
                                        *TRIAL_MOVE_PTRS[i]);
                }
#endif
                move_evaluator.evaluate(&trial_move_scores[i],  //
                                        *TRIAL_MOVE_PTRS[i],    //
                                        ITERATION,              //
                                        TABU_TENURE);

                total_scores[i] =
                    trial_solution_scores[i].local_augmented_objective +
                    trial_move_scores[i].frequency_penalty +
                    trial_move_scores[i].lagrangian_penalty;

                /**
                 * If the move is "tabu", it will be set lower priorities in
                 * selecting a move for the next solution.
                 */
                if (!trial_move_scores[i].is_permissible) {
                    total_scores[i] += constant::LARGE_VALUE_50;
                }

                /**
                 * If the move is special neighborhood moves, it must improves
                 * objective or feasibility.
                 */
                if (TRIAL_MOVE_PTRS[i]->is_special_neighborhood_move &&
                    !(trial_solution_scores[i].is_objective_improvable ||
                      trial_solution_scores[i].is_feasibility_improvable)) {
                    total_scores[i] += constant::LARGE_VALUE_100;
                }
            }

            /**
             * Select moves for the next solution.
             */
            this->select_move(total_scores, trial_move_scores,
                              trial_solution_scores);

            /**
             * Update the model by the selected move.
             */
            auto move_ptr = TRIAL_MOVE_PTRS[state.selected_index];
            model_ptr->update(*move_ptr);

            /**
             * Update the state.
             */
            this->update_state(move_ptr, trial_move_scores,
                               trial_solution_scores);

            /**
             * Store the current feasible solution.
             */
            if (option.is_enabled_store_feasible_solutions &&
                state.current_solution_score.is_feasible) {
                feasible_solutions.push_back(
                    model_ptr->export_plain_solution());
            }

            /**
             * Update the memory.
             */
            this->update_memory();

            /**
             * To avoid cycling, each special neighborhood can be used only once
             * in one tabu search loop.
             */
            if (move_ptr->is_special_neighborhood_move) {
                move_ptr->is_available = false;
            }

            /**
             * Update the stored chain moves.
             */
            if (state.iteration > 0 && option.is_enabled_chain_move) {
                this->update_chain_moves();
            }

            /**
             * Update the tabu tenure.
             */
            if (option.tabu_search
                    .is_enabled_automatic_tabu_tenure_adjustment) {
                this->update_tabu_tenure();
            }

            /**
             * Print the optimization progress.
             */
            if ((state.iteration %
                 std::max(option.tabu_search.log_interval, 1)) == 0 ||
                state.update_status > 0) {
                print_table_body(option.verbose >= option::verbose::Full);
            }

            /**
             *  Check the terminating condition (3).
             */
            if (option.tabu_search.is_enabled_automatic_break) {
                if (this->satisfy_penalty_coefficient_too_large_terminate_condition(
                        trial_solution_scores)) {
                    break;
                }
            }

            state.iteration++;
        }

        /**
         * Print the footer of the optimization progress table.
         */
        print_table_footer(option.verbose >= option::verbose::Full);

        /**
         * Postprocess.
         */
        this->postprocess();
    }

    /*************************************************************************/
    inline constexpr const TabuSearchCoreResult<T_Variable, T_Expression>&
    result(void) const {
        return m_result;
    }
};
}  // namespace core
}  // namespace tabu_search
}  // namespace solver
}  // namespace printemps

#endif
/*****************************************************************************/
// END
/*****************************************************************************/