/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef CPPMH_SOLVER_TABU_SEARCH_TABU_SEARCH_H__
#define CPPMH_SOLVER_TABU_SEARCH_TABU_SEARCH_H__

#ifdef _OPENMP
#include <omp.h>
#endif

#include "../memory.h"
#include "tabu_search_move_score.h"
#include "tabu_search_option.h"
#include "tabu_search_print.h"
#include "tabu_search_result.h"

namespace cppmh {
namespace solver {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class IncumbentHolder;

/*****************************************************************************/
struct Option;

namespace tabu_search {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
TabuSearchResult<T_Variable, T_Expression> solve(
    model::Model<T_Variable, T_Expression>* a_model,   //
    const Option&                           a_OPTION,  //
    const std::vector<model::ValueProxy<double>>&      //
        a_LOCAL_PENALTY_COEFFICIENT_PROXIES,           //
    const std::vector<model::ValueProxy<double>>&      //
        a_GLOBAL_PENALTY_COEFFICIENT_PROXIES,          //
    const std::vector<model::ValueProxy<T_Variable>>&  //
        a_INITIAL_VARIABLE_VALUE_PROXIES,              //
    const IncumbentHolder<T_Variable, T_Expression>&   //
        a_INCUMBENT_HOLDER) {
    /**
     * Define type aliases.
     */
    using Model_T           = model::Model<T_Variable, T_Expression>;
    using Result_T          = TabuSearchResult<T_Variable, T_Expression>;
    using IncumbentHolder_T = IncumbentHolder<T_Variable, T_Expression>;
    using Move_T            = model::Move<T_Variable, T_Expression>;
    using MoveScore         = TabuSearchMoveScore;

    /**
     * Start to measure computational time.
     */
    utility::TimeKeeper time_keeper;

    /**
     * Copy arguments as local variables.
     */
    Model_T* model  = a_model;
    Option   option = a_OPTION;

    std::vector<model::ValueProxy<double>> local_penalty_coefficient_proxies =
        a_LOCAL_PENALTY_COEFFICIENT_PROXIES;
    std::vector<model::ValueProxy<double>> global_penalty_coefficient_proxies =
        a_GLOBAL_PENALTY_COEFFICIENT_PROXIES;

    IncumbentHolder_T incumbent_holder = a_INCUMBENT_HOLDER;

    /**
     * Reset the local augmented incumbent.
     */
    incumbent_holder.reset_local_augmented_incumbent();

    /**
     * Prepare a random generator, which is used for shuffling moves.
     */
    std::mt19937 get_rand_mt(option.tabu_search.seed);

    model->import_variable_values(a_INITIAL_VARIABLE_VALUE_PROXIES);
    model->update();

    model::SolutionScore solution_score =
        model->evaluate({},                                 //
                        local_penalty_coefficient_proxies,  //
                        global_penalty_coefficient_proxies);

    int update_status =
        incumbent_holder.try_update_incumbent(model, solution_score);

    int total_update_status = IncumbentHolderConstant::STATUS_NO_UPDATED;

    /**
     * Create memory which records final updated iteration and updated count for
     * each decision variable.
     */
    Memory memory(model);

    utility::print_single_line(option.verbose >= Verbose::Full);
    utility::print_message("Tabu Search starts.",
                           option.verbose >= Verbose::Full);
    print_table_header(option.verbose >= Verbose::Full);
    print_table_initial(model, solution_score, incumbent_holder,
                        option.verbose >= Verbose::Full);

    std::vector<model::SolutionScore> trial_solution_scores;
    std::vector<MoveScore>            trial_move_scores;

    std::vector<double> objective_improvements;
    std::vector<double> local_penalties;

    std::vector<double> local_augmented_objectives;
    std::vector<double> global_augmented_objectives;
    std::vector<double> total_scores;

    /**
     * Set up the tabu tenure and related parameters.
     */
    int original_tabu_tenure = std::min(option.tabu_search.initial_tabu_tenure,
                                        model->number_of_variables());
    int tabu_tenure          = original_tabu_tenure;

    bool has_constraint(local_penalty_coefficient_proxies.size() > 0);

    double bias_previous       = 0.0;
    double bias_current        = 0.0;
    int    bias_increase_count = 0;
    int    bias_decrease_count = 0;

    int last_tabu_tenure_updated_iteration = 0;

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
            break;
        }

        if (elapsed_time + option.tabu_search.time_offset > option.time_max) {
            break;
        }

        if (iteration >= option.tabu_search.iteration_max) {
            break;
        }

        if (incumbent_holder.feasible_incumbent_objective() <=
            option.target_objective_value) {
            break;
        }

        model->neighborhood().update_moves();
        if (option.tabu_search.is_enabled_shuffle) {
            model->neighborhood().shuffle_moves(&get_rand_mt);
        }

        auto& trial_move_ptrs = model->neighborhood().move_ptrs();
        int   number_of_moves = trial_move_ptrs.size();

        if (option.tabu_search.is_enabled_move_curtail) {
            number_of_moves = static_cast<int>(
                floor(option.tabu_search.move_preserve_rate * number_of_moves));
        }

        if (number_of_moves == 0) {
            break;
        }

        trial_solution_scores.resize(number_of_moves);
        trial_move_scores.resize(number_of_moves);

        objective_improvements.resize(number_of_moves);
        local_penalties.resize(number_of_moves);

        local_augmented_objectives.resize(number_of_moves);
        global_augmented_objectives.resize(number_of_moves);
        total_scores.resize(number_of_moves);

        bool is_aspirated = false;

#ifdef _OPENMP
#pragma omp parallel for if (option.is_enabled_parallel_evaluation) \
    schedule(static)
#endif
        for (auto i_move = 0; i_move < number_of_moves; i_move++) {
            if (model->is_enabled_fast_evaluation()) {
                trial_solution_scores[i_move] =
                    model->evaluate(*trial_move_ptrs[i_move], solution_score,
                                    local_penalty_coefficient_proxies,
                                    global_penalty_coefficient_proxies);

            } else {
                trial_solution_scores[i_move] = model->evaluate(
                    *trial_move_ptrs[i_move], local_penalty_coefficient_proxies,
                    global_penalty_coefficient_proxies);
            }

            trial_move_scores[i_move] =
                evaluate_move(*trial_move_ptrs[i_move], iteration, memory,
                              option, tabu_tenure);

            objective_improvements[i_move] =
                trial_solution_scores[i_move].objective_improvement;

            local_penalties[i_move] =
                trial_solution_scores[i_move].local_penalty;

            local_augmented_objectives[i_move] =
                trial_solution_scores[i_move].local_augmented_objective;
            global_augmented_objectives[i_move] =
                trial_solution_scores[i_move].global_augmented_objective;

            total_scores[i_move] =
                trial_solution_scores[i_move].local_augmented_objective +
                trial_move_scores[i_move].frequency_penalty;

            if (option.tabu_search.is_enabled_improvability_screening &&
                has_constraint) {
                if (solution_score.is_feasible &&
                    !trial_solution_scores[i_move].is_objective_improvable) {
                    total_scores[i_move] = HUGE_VALF;
                }
                if (!solution_score.is_feasible &&
                    !trial_solution_scores[i_move].is_constraint_improvable) {
                    total_scores[i_move] = HUGE_VALF;
                }
            }

            if (!trial_move_scores[i_move].is_permissible) {
                total_scores[i_move] = HUGE_VALF;
            }
        }
        int argmin_global_augmented_objective =
            utility::argmin(global_augmented_objectives);

        int argmin_total_score = utility::argmin(total_scores);
        int selected_index     = 0;

        if (iteration < option.tabu_search.number_of_initial_modification) {
            /**
             * For diversification, the move for next solution is randomly
             * selected for initial several iteration.
             */
            selected_index = rand() % number_of_moves;
        } else {
            /**
             * The move for next solution is determined by evaluations of
             * solutions and moves after the inital modification has finished.
             */

            selected_index = argmin_total_score;

            /**
             * A move which improves incumbent solution can be accepted.
             * (optional)
             */
            if (option.tabu_search.ignore_tabu_if_augmented_incumbent) {
                if (!trial_move_scores[argmin_global_augmented_objective]
                         .is_permissible &&
                    trial_solution_scores[argmin_global_augmented_objective]
                            .global_augmented_objective <
                        incumbent_holder
                            .global_augmented_incumbent_objective()) {
                    selected_index = argmin_global_augmented_objective;
                    is_aspirated   = true;
                }
            }

            if (option.tabu_search.ignore_tabu_if_feasible_incumbent) {
                if (trial_solution_scores[argmin_global_augmented_objective]
                        .is_feasible) {
                    if (!trial_move_scores[argmin_global_augmented_objective]
                             .is_permissible &&
                        trial_solution_scores[argmin_global_augmented_objective]
                                .global_augmented_objective <
                            incumbent_holder.feasible_incumbent_objective()) {
                        selected_index = argmin_global_augmented_objective;
                        is_aspirated   = true;
                    }
                }
            }
        }

        int number_of_all_neighborhoods         = number_of_moves;
        int number_of_feasible_neighborhoods    = 0;
        int number_of_infeasible_neighborhood   = 0;
        int number_of_permissible_neighborhoods = 0;
        int number_of_improvable_neighborhoods  = 0;

        for (const auto& score : trial_solution_scores) {
            if (score.is_feasible) {
                number_of_feasible_neighborhoods++;
            }
            if (score.is_objective_improvable ||
                score.is_constraint_improvable) {
                number_of_improvable_neighborhoods++;
            }
        }

        for (const auto& score : trial_move_scores) {
            if (score.is_permissible) {
                number_of_permissible_neighborhoods++;
            }
        }
        number_of_infeasible_neighborhood =
            number_of_all_neighborhoods - number_of_feasible_neighborhoods;

        Move_T* move_ptr = trial_move_ptrs[selected_index];
        model->update(*move_ptr);
        solution_score = trial_solution_scores[selected_index];

        update_status =
            incumbent_holder.try_update_incumbent(model, solution_score);

        total_update_status = update_status | total_update_status;

        memory.update(*move_ptr, iteration);

        if (iteration % std::max(option.tabu_search.log_interval, 1) == 0 ||
            update_status > 1) {
            print_table_body(model,                                //
                             iteration,                            //
                             number_of_all_neighborhoods,          //
                             number_of_feasible_neighborhoods,     //
                             number_of_permissible_neighborhoods,  //
                             number_of_improvable_neighborhoods,   //
                             solution_score,                       //
                             update_status,                        //
                             incumbent_holder,                     //
                             option.verbose >= Verbose::Full);
        }

        if (option.tabu_search.is_enabled_automatic_break) {
            /**
             * If the local penalty us sufficiently larger than objective
             * sensitivity, the current loop is terminated and the
             * local penalty coefficients will be adjusted.
             */
            constexpr int    ITERATION_MIN = 10;
            constexpr double MARGIN        = 100.0;

            if (iteration > ITERATION_MIN      //
                && solution_score.is_feasible  //
                && number_of_infeasible_neighborhood > 0) {
                std::vector<double> infeasible_local_penalties(
                    number_of_infeasible_neighborhood);
                int count = 0;
                for (auto i = 0; i < number_of_all_neighborhoods; i++) {
                    if (!trial_solution_scores[i].is_feasible) {
                        infeasible_local_penalties[count++] =
                            local_penalties[i];
                    }
                }

                double max_objective_sensitivity =
                    utility::max_abs(objective_improvements);
                double min_penalty = utility::min(infeasible_local_penalties);
                if (max_objective_sensitivity * MARGIN < min_penalty) {
                    break;
                }
            }
        }

        if (option.tabu_search.is_enabled_automatic_tabu_tenure_adjustment) {
            constexpr int BIAS_INCRESE_COUNT_THRESHOLD  = 5;
            constexpr int BIAS_DECREASE_COUNT_THRESHOLD = 10;

            if (is_aspirated || number_of_permissible_neighborhoods == 0) {
                /**
                 * The tabu tenure is decreased if
                 * - The incumbent solution is found in the tabu solutions,
                 * or
                 * - There is no permissible solutions.
                 */
                tabu_tenure = std::max(tabu_tenure - 1, original_tabu_tenure);
                last_tabu_tenure_updated_iteration = iteration;
                bias_decrease_count                = 0;
                bias_increase_count                = 0;
                utility::print_info("Tabu tenure decreased: " +
                                        std::to_string(tabu_tenure) + ".",
                                    option.verbose >= Verbose::Debug);
            } else if (((iteration - last_tabu_tenure_updated_iteration) %
                        tabu_tenure) == 0) {
                /**
                 * The bias of searching is computed with the interval of
                 * tabu_tenure. The tabu tenure is increased if the bias has
                 * grown up, and decreased if the bias is reduced.
                 */
                bias_previous = bias_current;
                bias_current  = memory.bias();

                if (bias_current > bias_previous) {
                    bias_increase_count++;
                    bias_decrease_count = 0;

                    if (bias_increase_count > BIAS_INCRESE_COUNT_THRESHOLD) {
                        bias_increase_count = 0;
                        tabu_tenure         = std::min(tabu_tenure + 1,
                                               model->number_of_variables());
                        last_tabu_tenure_updated_iteration = iteration;
                        utility::print_info("Tabu tenure increased: " +
                                                std::to_string(tabu_tenure) +
                                                ".",
                                            option.verbose >= Verbose::Debug);
                    }
                } else {
                    bias_decrease_count++;
                    bias_increase_count = 0;

                    if (bias_decrease_count > BIAS_DECREASE_COUNT_THRESHOLD) {
                        bias_decrease_count = 0;
                        tabu_tenure =
                            std::max(tabu_tenure - 1, original_tabu_tenure);
                        last_tabu_tenure_updated_iteration = iteration;

                        utility::print_info("Tabu tenure decreased: " +
                                                std::to_string(tabu_tenure) +
                                                ".",
                                            option.verbose >= Verbose::Debug);
                    }
                }
            }
        }
        iteration++;
    }

    print_table_footer(option.verbose >= Verbose::Full);

    Result_T result;
    result.incumbent_holder     = incumbent_holder;
    result.memory               = memory;
    result.total_update_status  = total_update_status;
    result.tabu_tenure          = tabu_tenure;
    result.number_of_iterations = iteration;
    return result;
}
}  // namespace tabu_search
}  // namespace solver
}  // namespace cppmh

#endif
/*****************************************************************************/
// END
/*****************************************************************************/