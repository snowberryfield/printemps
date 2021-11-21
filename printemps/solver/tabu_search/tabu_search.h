/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_TABU_SEARCH_TABU_SEARCH_H__
#define PRINTEMPS_SOLVER_TABU_SEARCH_TABU_SEARCH_H__

#include "../memory.h"
#include "tabu_search_move_score.h"
#include "tabu_search_print.h"
#include "tabu_search_termination_status.h"
#include "tabu_search_result.h"

namespace printemps {
namespace solver {
namespace tabu_search {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
TabuSearchResult<T_Variable, T_Expression> solve(
    model::Model<T_Variable, T_Expression>* a_model_ptr,        //
    const option::Option&                   a_OPTION,           //
    const std::vector<multi_array::ValueProxy<T_Variable>>&     //
        a_INITIAL_VARIABLE_VALUE_PROXIES,                       //
    const solution::IncumbentHolder<T_Variable, T_Expression>&  //
                 a_INCUMBENT_HOLDER,                            //
    const Memory a_MEMORY) {
    /**
     * Define type aliases.
     */
    using Model_T  = model::Model<T_Variable, T_Expression>;
    using Result_T = TabuSearchResult<T_Variable, T_Expression>;
    using IncumbentHolder_T =
        solution::IncumbentHolder<T_Variable, T_Expression>;
    using Move_T    = neighborhood::Move<T_Variable, T_Expression>;
    using MoveScore = TabuSearchMoveScore;

    /**
     * Start to measure computational time.
     */
    utility::TimeKeeper time_keeper;
    time_keeper.set_start_time();

    /**
     * Copy arguments as local variables.
     */
    Model_T*       model_ptr = a_model_ptr;
    option::Option option    = a_OPTION;
    Memory         memory    = a_MEMORY;

    IncumbentHolder_T incumbent_holder = a_INCUMBENT_HOLDER;

    /**
     * Reset the local augmented incumbent.
     */
    incumbent_holder.reset_local_augmented_incumbent();

    /**
     * Prepare a random generator, which is used for shuffling moves.
     */
    std::mt19937 get_rand_mt(option.tabu_search.seed);

    /**
     * Initialize the solution and update the model.
     */
    model_ptr->import_variable_values(a_INITIAL_VARIABLE_VALUE_PROXIES);
    model_ptr->update();

    solution::SolutionScore current_solution_score  = model_ptr->evaluate({});
    solution::SolutionScore previous_solution_score = current_solution_score;

    int update_status = incumbent_holder.try_update_incumbent(
        model_ptr, current_solution_score);
    int total_update_status =
        solution::IncumbentHolderConstant::STATUS_NO_UPDATED;

    /**
     * Reset the last update iterations.
     */
    memory.reset_last_update_iterations();

    /**
     * Set up the tabu tenure and related parameters.
     */
    int original_tabu_tenure =
        std::min(option.tabu_search.initial_tabu_tenure,
                 model_ptr->number_of_mutable_variables());
    int tabu_tenure = original_tabu_tenure;

    double intensity_previous       = 0.0;
    double intensity_current        = 0.0;
    int    intensity_increase_count = 0;
    int    intensity_decrease_count = 0;

    int last_tabu_tenure_updated_iteration = 0;

    /**
     * Prepare feasible solutions holder.
     */
    std::vector<solution::SparseSolution<T_Variable, T_Expression>>
        feasible_solutions;

    /**
     * Reset the variable improvability.
     */
    model_ptr->reset_variable_objective_improvabilities();
    model_ptr->reset_variable_feasibility_improvabilities();

    /**
     * Prepare other local variables.
     */
    int number_of_all_neighborhoods         = 0;
    int number_of_feasible_neighborhoods    = 0;
    int number_of_permissible_neighborhoods = 0;
    int number_of_improvable_neighborhoods  = 0;

    std::vector<solution::SolutionScore> trial_solution_scores;
    std::vector<MoveScore>               trial_move_scores;
    std::vector<double>                  total_scores;
    std::vector<double>                  infeasible_local_penalties;

    int last_local_augmented_incumbent_update_iteration  = -1;
    int last_global_augmented_incumbent_update_iteration = -1;
    int last_feasible_incumbent_update_iteration         = -1;

    int local_augmented_incumbent_update_count = 0;

    auto termination_status = TabuSearchTerminationStatus::ITERATION_OVER;

    neighborhood::Move<T_Variable, T_Expression> previous_move;
    neighborhood::Move<T_Variable, T_Expression> current_move;

    bool is_few_permissible_neighborhood = false;
    bool is_found_new_feasible_solution  = false;

    double min_objective     = current_solution_score.objective;
    double max_objective     = current_solution_score.objective;
    double min_local_penalty = HUGE_VALF;
    if (!current_solution_score.is_feasible) {
        min_local_penalty = current_solution_score.local_penalty;
    }

    /**
     * Print the header of optimization progress table and print the initial
     * solution status.
     */
    utility::print_single_line(option.verbose >= option::verbose::Full);
    utility::print_message("Tabu Search starts.",
                           option.verbose >= option::verbose::Full);

    print_table_header(option.verbose >= option::verbose::Full);
    print_table_initial(model_ptr,               //
                        current_solution_score,  //
                        incumbent_holder,        //
                        option.verbose >= option::verbose::Full);

    /**
     * Iterations start.
     */
    int iteration = 0;
    while (true) {
        /**
         *  Check the terminating condition.
         */
        double elapsed_time = time_keeper.clock();
        if (elapsed_time > option.tabu_search.time_max) {
            termination_status = TabuSearchTerminationStatus::TIME_OVER;
            break;
        }

        if (elapsed_time + option.tabu_search.time_offset > option.time_max) {
            termination_status = TabuSearchTerminationStatus::TIME_OVER;
            break;
        }

        if (iteration >= option.tabu_search.iteration_max) {
            termination_status = TabuSearchTerminationStatus::ITERATION_OVER;
            break;
        }

        if (incumbent_holder.feasible_incumbent_objective() <=
            option.target_objective_value) {
            termination_status = TabuSearchTerminationStatus::REACH_TARGET;
            break;
        }

        if (local_augmented_incumbent_update_count >
            option.tabu_search.pruning_rate_threshold *
                option.tabu_search.iteration_max) {
            termination_status = TabuSearchTerminationStatus::EARLY_STOP;
            break;
        }

        /**
         * Update the moves.
         */
        bool is_enabled_improvability_screening =
            (option.improvability_screening_mode !=
             option::improvability_screening_mode::Off);

        bool accept_all                    = true;
        bool accept_objective_improvable   = true;
        bool accept_feasibility_improvable = true;

        if (model_ptr->is_linear() && is_enabled_improvability_screening) {
            /**
             * If the option improvability_screening_mode is not None,
             * only improvable moves will be generated.
             */
            auto changed_variable_ptrs = utility::to_vector(
                neighborhood::related_variable_ptrs(current_move));

            if (iteration == 0) {
                model_ptr->update_variable_objective_improvabilities();
            } else {
                model_ptr->update_variable_objective_improvabilities(
                    changed_variable_ptrs);
            }

            switch (option.improvability_screening_mode) {
                case option::improvability_screening_mode::Soft: {
                    if (model_ptr->is_feasible()) {
                        accept_all                    = false;
                        accept_objective_improvable   = true;
                        accept_feasibility_improvable = false;
                    } else {
                        model_ptr->reset_variable_feasibility_improvabilities();
                        model_ptr
                            ->update_variable_feasibility_improvabilities();

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
                        model_ptr
                            ->update_variable_feasibility_improvabilities();

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
                        auto changed_constraint_ptrs = utility::to_vector(
                            current_move.related_constraint_ptrs);

                        if (iteration == 0) {
                            model_ptr
                                ->reset_variable_feasibility_improvabilities();
                            model_ptr
                                ->update_variable_feasibility_improvabilities();
                        } else {
                            model_ptr
                                ->reset_variable_feasibility_improvabilities(
                                    changed_constraint_ptrs);
                            model_ptr
                                ->update_variable_feasibility_improvabilities(
                                    changed_constraint_ptrs);
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
        }
        model_ptr->neighborhood().update_moves(
            accept_all,                     //
            accept_objective_improvable,    //
            accept_feasibility_improvable,  //
            option.is_enabled_parallel_neighborhood_update);

        if (option.tabu_search.is_enabled_shuffle) {
            model_ptr->neighborhood().shuffle_moves(&get_rand_mt);
        }

        const auto& trial_move_ptrs = model_ptr->neighborhood().move_ptrs();
        int         number_of_moves = trial_move_ptrs.size();

        if (option.tabu_search.is_enabled_move_curtail) {
            number_of_moves = static_cast<int>(
                floor(option.tabu_search.move_preserve_rate * number_of_moves));
        }

        /**
         * If the number of the moves is zero, the tabu search iterations will
         * be terminated.
         */
        if (number_of_moves == 0) {
            if (model_ptr->is_linear() && model_ptr->is_feasible()) {
                /**
                 * If the current solution is feasible and there is no
                 * improvable solution, the solution should be an optimum.
                 * It can happen for decomp2 instance in MIPLIB 2017.
                 */
                termination_status = TabuSearchTerminationStatus::OPTIMAL;
                for (const auto& variable_ptr :
                     model_ptr->variable_reference().variable_ptrs) {
                    if (variable_ptr->is_objective_improvable()) {
                        termination_status =
                            TabuSearchTerminationStatus::NO_MOVE;
                        break;
                    }
                }
                break;
            } else {
                termination_status = TabuSearchTerminationStatus::NO_MOVE;
                break;
            }
        }

        /**
         * Reserve elements for vectors by the number of the moves. This step is
         * required for each iteration because the number of the moves can be
         * changed.
         */
        trial_solution_scores.resize(number_of_moves);
        trial_move_scores.resize(number_of_moves);
        total_scores.resize(number_of_moves);

#ifdef _OPENMP
#pragma omp parallel for if (option.is_enabled_parallel_evaluation) \
    schedule(static)
#endif
        for (auto i = 0; i < number_of_moves; i++) {
            /**
             * The neighborhood solutions will be evaluated in parallel by fast
             * or ordinary(slow) evaluation methods.
             */
#ifndef _MPS_SOLVER
            if (model_ptr->is_enabled_fast_evaluation()) {
#endif
                model_ptr->evaluate(&trial_solution_scores[i],  //
                                    *trial_move_ptrs[i],        //
                                    current_solution_score);
#ifndef _MPS_SOLVER
            } else {
                model_ptr->evaluate(&trial_solution_scores[i],  //
                                    *trial_move_ptrs[i]);
            }
#endif
            evaluate_move(&trial_move_scores[i],  //
                          *trial_move_ptrs[i],    //
                          iteration,              //
                          memory,                 //
                          option,                 //
                          tabu_tenure);

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
            if (trial_move_ptrs[i]->is_special_neighborhood_move &&
                !(trial_solution_scores[i].is_objective_improvable ||
                  trial_solution_scores[i].is_feasibility_improvable)) {
                total_scores[i] += constant::LARGE_VALUE_100;
            }
        }

        /**
         * Select moves for the next solution.
         */
        int argmin_global_augmented_objective = std::distance(
            trial_solution_scores.begin(),
            min_element(trial_solution_scores.begin(),
                        trial_solution_scores.end(),
                        [](const auto& a_FIRST, const auto& a_SECOND) {
                            return a_FIRST.global_augmented_objective <
                                   a_SECOND.global_augmented_objective;
                        }));

        int  argmin_total_score = utility::argmin(total_scores);
        int  selected_index     = 0;
        bool is_aspirated       = false;

        if (iteration < option.tabu_search.number_of_initial_modification) {
            /**
             * For diversification, the move for next solution will be randomly
             * selected for initial several iteration.
             */
            selected_index = get_rand_mt() % number_of_moves;
        } else {
            /**
             * The move for next solution will be determined by evaluations of
             * solutions and moves after the inital modifications.
             */
            selected_index = argmin_total_score;

            /**
             * A move which improves the augmented incumbent solution can be
             * accepted (optional).
             */
            if (option.tabu_search.ignore_tabu_if_global_incumbent) {
                if (trial_solution_scores[argmin_global_augmented_objective]
                            .global_augmented_objective +
                        constant::EPSILON <
                    incumbent_holder.global_augmented_incumbent_objective()) {
                    selected_index = argmin_global_augmented_objective;
                    if (!trial_move_scores[selected_index].is_permissible) {
                        is_aspirated = true;
                    }
                }
            }
        }
        /**
         * Backup the previous solution score and move.
         */
        previous_solution_score = current_solution_score;
        previous_move           = current_move;

        /**
         * Update the model by the selected move.
         */
        Move_T* move_ptr = trial_move_ptrs[selected_index];
        model_ptr->update(*move_ptr);

        /**
         * Update the current solution score and move.
         */
        current_solution_score = trial_solution_scores[selected_index];
        current_move           = *move_ptr;

        min_objective =
            std::min(min_objective, current_solution_score.objective);
        max_objective =
            std::min(max_objective, current_solution_score.objective);
        if (!current_solution_score.is_feasible) {
            min_local_penalty = std::min(min_local_penalty,
                                         current_solution_score.local_penalty);
        }

        /**
         * Update the status.
         */
        update_status = incumbent_holder.try_update_incumbent(
            model_ptr, current_solution_score);
        total_update_status = update_status | total_update_status;

        if (current_solution_score.is_feasible) {
            is_found_new_feasible_solution = true;
        }

        /**
         * Store the current feasible solution.
         */
        if (option.is_enabled_store_feasible_solutions &&
            current_solution_score.is_feasible) {
            feasible_solutions.push_back(model_ptr->export_plain_solution());
        }

        /**
         * Update the memory.
         */
        int random_width = static_cast<int>(
            option.tabu_search.tabu_tenure_randomize_rate * tabu_tenure);
        memory.update(*move_ptr,     //
                      iteration,     //
                      random_width,  //
                      &get_rand_mt);

        /**
         * To avoid cycling, each special neighborhood can be used only once in
         * one tabu search loop.
         */
        if (move_ptr->is_special_neighborhood_move) {
            move_ptr->is_available = false;
        }

        /**
         * Calculate various statistics for logging.
         */
        if (update_status & solution::IncumbentHolderConstant::
                                STATUS_LOCAL_AUGMENTED_INCUMBENT_UPDATE) {
            last_local_augmented_incumbent_update_iteration = iteration;
        }

        if (update_status & solution::IncumbentHolderConstant::
                                STATUS_GLOBAL_AUGMENTED_INCUMBENT_UPDATE) {
            last_global_augmented_incumbent_update_iteration = iteration;
        }

        if (update_status & solution::IncumbentHolderConstant::
                                STATUS_FEASIBLE_INCUMBENT_UPDATE) {
            last_feasible_incumbent_update_iteration = iteration;
        }

        /**
         * For pruning, count updating of the local augmented incumbent without
         * global augmented incumbent improvement.
         */
        if (update_status == solution::IncumbentHolderConstant::
                                 STATUS_LOCAL_AUGMENTED_INCUMBENT_UPDATE) {
            local_augmented_incumbent_update_count++;
        } else if (update_status &
                   solution::IncumbentHolderConstant::
                       STATUS_GLOBAL_AUGMENTED_INCUMBENT_UPDATE) {
            local_augmented_incumbent_update_count = 0;
        }

        /**
         * Calculate the number of moves for each type.
         */
        number_of_all_neighborhoods = number_of_moves;
        if (iteration % std::max(option.tabu_search.log_interval, 1) == 0 ||
            update_status > 0) {
            number_of_feasible_neighborhoods    = 0;
            number_of_permissible_neighborhoods = 0;
            number_of_improvable_neighborhoods  = 0;

            for (const auto& score : trial_solution_scores) {
                if (score.is_feasible) {
                    number_of_feasible_neighborhoods++;
                }
                if (score.is_objective_improvable ||
                    score.is_feasibility_improvable) {
                    number_of_improvable_neighborhoods++;
                }
            }

            for (const auto& score : trial_move_scores) {
                if (score.is_permissible) {
                    number_of_permissible_neighborhoods++;
                }
            }

            if (number_of_permissible_neighborhoods == 0) {
                is_few_permissible_neighborhood = true;
            }
        } else {
            bool is_few_permissible_neighborhood_temp = true;
            for (const auto& score : trial_move_scores) {
                if (score.is_permissible) {
                    is_few_permissible_neighborhood_temp = false;
                    break;
                }
            }
            if (is_few_permissible_neighborhood_temp) {
                is_few_permissible_neighborhood = true;
            }
        }

        /**
         * Register a chain move.
         */
        if (iteration > 0 && option.is_enabled_chain_move) {
            if ((previous_move.sense == neighborhood::MoveSense::Binary &&
                 current_move.sense == neighborhood::MoveSense::Binary &&
                 previous_move.alterations.front().second !=
                     current_move.alterations.front().second) ||
                (previous_move.sense == neighborhood::MoveSense::Chain &&
                 current_move.sense == neighborhood::MoveSense::Chain)) {
                Move_T chain_move;
                if (previous_move.alterations.front().first <
                    current_move.alterations.front().first)
                    chain_move = previous_move + current_move;
                else {
                    chain_move = current_move + previous_move;
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

        if (option.tabu_search.is_enabled_automatic_tabu_tenure_adjustment) {
            if ((update_status &
                 solution::IncumbentHolderConstant::
                     STATUS_GLOBAL_AUGMENTED_INCUMBENT_UPDATE) &&
                tabu_tenure > original_tabu_tenure) {
                /**
                 * The tabu tenure will be reverted to the original value if it
                 * has been increased and the global incumbent is updated.
                 */
                tabu_tenure                        = original_tabu_tenure;
                last_tabu_tenure_updated_iteration = iteration;
                intensity_decrease_count           = 0;
                intensity_increase_count           = 0;
                utility::print_debug("Tabu tenure reverted: " +
                                         std::to_string(tabu_tenure) + ".",
                                     option.verbose >= option::verbose::Debug);
            } else if ((iteration - last_tabu_tenure_updated_iteration) %
                           (tabu_tenure + 1) ==
                       0) {
                /**
                 * The intensity of searching will be computed with the interval
                 * of tabu_tenure+1. The tabu tenure will be increased if the
                 * intensity has grown up, and decreased if the intensity has
                 * been reduced.
                 */
                intensity_previous = intensity_current;
                intensity_current  = memory.intensity();

                if (intensity_current > intensity_previous) {
                    intensity_increase_count++;
                    intensity_decrease_count = 0;

                    if (intensity_increase_count >
                        option.tabu_search.intensity_increase_count_threshold) {
                        intensity_increase_count = 0;
                        tabu_tenure =
                            std::min(tabu_tenure + 1,
                                     model_ptr->number_of_mutable_variables());
                        last_tabu_tenure_updated_iteration = iteration;
                        utility::print_debug(
                            "Tabu tenure increased: " +
                                std::to_string(tabu_tenure) + ".",
                            option.verbose >= option::verbose::Debug);
                    }
                } else {
                    intensity_decrease_count++;
                    intensity_increase_count = 0;

                    if (intensity_decrease_count >
                        option.tabu_search.intensity_decrease_count_threshold) {
                        intensity_decrease_count = 0;
                        tabu_tenure =
                            std::max(tabu_tenure - 1,
                                     std::max(1, original_tabu_tenure / 2));
                        last_tabu_tenure_updated_iteration = iteration;

                        utility::print_debug(
                            "Tabu tenure decreased: " +
                                std::to_string(tabu_tenure) + ".",
                            option.verbose >= option::verbose::Debug);
                    }
                }
            }
        }

        /**
         * Print the optimization progress.
         */
        if (iteration % std::max(option.tabu_search.log_interval, 1) == 0 ||
            update_status > 0) {
            print_table_body(model_ptr,                                  //
                             iteration,                                  //
                             current_move.is_special_neighborhood_move,  //
                             number_of_all_neighborhoods,                //
                             number_of_feasible_neighborhoods,           //
                             number_of_permissible_neighborhoods,        //
                             number_of_improvable_neighborhoods,         //
                             current_solution_score,                     //
                             update_status,                              //
                             incumbent_holder,                           //
                             is_aspirated,                               //
                             option.verbose >= option::verbose::Full);
        }

        if (option.tabu_search.is_enabled_automatic_break) {
            /**
             * If the local penalty us sufficiently larger than objective
             * sensitivity, the current loop will be terminated and the
             * local penalty coefficients will be adjusted.
             */
            constexpr int    ITERATION_MIN = 10;
            constexpr double MARGIN        = 100.0;

            if (iteration > ITERATION_MIN &&
                current_solution_score.is_feasible) {
                infeasible_local_penalties.clear();
                for (const auto& score : trial_solution_scores) {
                    if (!score.is_feasible) {
                        infeasible_local_penalties.push_back(
                            score.local_penalty);
                    }
                }
                if (infeasible_local_penalties.size() > 0) {
                    auto argminmax_objective_sensitivity_score_ptr =
                        std::minmax_element(
                            trial_solution_scores.begin(),
                            trial_solution_scores.end(),
                            [](const auto& a_FIRST, const auto& a_SECOND) {
                                return a_FIRST.objective_improvement <
                                       a_SECOND.objective_improvement;
                            });

                    double max_objective_sensitivity =
                        std::max(argminmax_objective_sensitivity_score_ptr
                                     .second->objective_improvement,
                                 -argminmax_objective_sensitivity_score_ptr
                                      .first->objective_improvement);

                    if (max_objective_sensitivity * MARGIN <
                        utility::min(infeasible_local_penalties)) {
                        termination_status =
                            TabuSearchTerminationStatus::EARLY_STOP;
                        break;
                    }
                }
            }
        }

        iteration++;
    }

    /**
     * Print the footer of the optimization progress table.
     */
    print_table_footer(option.verbose >= option::verbose::Full);

    /**
     * Prepare the result.
     */
    Result_T result;
    result.incumbent_holder     = incumbent_holder;
    result.memory               = memory;
    result.total_update_status  = total_update_status;
    result.tabu_tenure          = tabu_tenure;
    result.number_of_iterations = iteration;

    result.last_local_augmented_incumbent_update_iteration =
        last_local_augmented_incumbent_update_iteration;
    result.last_global_augmented_incumbent_update_iteration =
        last_global_augmented_incumbent_update_iteration;
    result.last_feasible_incumbent_update_iteration =
        last_feasible_incumbent_update_iteration;

    result.is_few_permissible_neighborhood = is_few_permissible_neighborhood;
    result.is_found_new_feasible_solution  = is_found_new_feasible_solution;

    auto abs_max_objective = std::max(fabs(max_objective), fabs(min_objective));

    result.objective_constraint_rate =
        std::max(1.0, std::max(abs_max_objective,  //
                               max_objective - min_objective)) /
        std::max(1.0, min_local_penalty);

    result.termination_status = termination_status;
    result.feasible_solutions = feasible_solutions;

    return result;
}
}  // namespace tabu_search
}  // namespace solver
}  // namespace printemps

#endif
/*****************************************************************************/
// END
/*****************************************************************************/