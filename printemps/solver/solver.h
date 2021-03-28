/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_H__
#define PRINTEMPS_SOLVER_H__

#include "../model/model.h"
#include "../utility/utility.h"

#include "incumbent_holder.h"
#include "option.h"
#include "status.h"
#include "solution_archive.h"
#include "result.h"
#include "tabu_search/tabu_search.h"
#include "local_search/local_search.h"
#include "lagrange_dual/lagrange_dual.h"

namespace printemps {
namespace solver {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
Result<T_Variable, T_Expression> solve(
    model::Model<T_Variable, T_Expression>* a_model) {
    Option option;
    return solve(a_model, option);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression>
Result<T_Variable, T_Expression> solve(
    model::Model<T_Variable, T_Expression>* a_model,  //
    const Option&                           a_OPTION) {
    /**
     * Start to measure computational time and get the starting time.
     */
    utility::TimeKeeper time_keeper;
    std::string         start_date_time = utility::date_time();

    /**
     * Print the program name, the project url, and the starting time.
     */
    utility::print_single_line(a_OPTION.verbose >= Verbose::Outer);
    utility::print(
        "PRINTEMPS " + constant::VERSION + " (" + constant::PROJECT_URL + ")",
        a_OPTION.verbose >= Verbose::Outer);
    utility::print("running from " + start_date_time,
                   a_OPTION.verbose >= Verbose::Outer);

    /**
     * The model can be solved only once.
     */
    if (a_model->is_solved()) {
        throw std::logic_error(utility::format_error_location(
            __FILE__, __LINE__, __func__,
            "This model has already been solved."));
    } else {
        a_model->set_is_solved(true);
    }

    /**
     * Define type aliases.
     */
    using Model_T           = model::Model<T_Variable, T_Expression>;
    using Solution_T        = model::Solution<T_Variable, T_Expression>;
    using IncumbentHolder_T = IncumbentHolder<T_Variable, T_Expression>;

    /**
     * Copy arguments as local variables.
     */
    Model_T* model         = a_model;
    Option   master_option = a_OPTION;

    /**
     * Set default target objective value if it is not defined by the user.
     * For minimization problems, the default target value is set as -1E100.
     * For maximization problems, the default target value is set as 1E100.
     * If there is no objective function definition, the default target value is
     * set as 0 and the algorithm will be terminated if a feasible solution is
     * found.
     */
    auto target_objective_changed_rate =
        master_option.target_objective_value /
            OptionConstant::DEFAULT_TARGET_OBJECTIVE -
        1.0;

    if (fabs(target_objective_changed_rate) > constant::EPSILON) {
        master_option.target_objective_value *= model->sign();
    }

    if (fabs(target_objective_changed_rate) < constant::EPSILON) {
        if (!model->is_defined_objective()) {
            master_option.target_objective_value = 0.0;
        }
    }

    /**
     * Print the option value.
     */
    if (master_option.verbose >= Verbose::Outer) {
        master_option.print();
    }

    /**
     * Setup the model. This includes the following processes.
     * - setup_variable_related_constraints()
     * - setup_unique_name()
     * - setup_is_linear()
     * - setup_variable_sensitivity()
     * - presolve()
     * - categorize_variables()
     * - categorize_constraints()
     * - extract_selections()
     * - setup_neighborhood()
     * - verify_and_correct_selection_variables_initial_values()
     * - verify_and_correct_binary_variables_initial_values()
     * - verify_and_correct_integer_variables_initial_values()
     * - setup_fixed_sensitivities()
     */
    model->setup(master_option.is_enabled_presolve,
                 master_option.is_enabled_initial_value_correction,
                 master_option.is_enabled_aggregation_move,
                 master_option.is_enabled_precedence_move,
                 master_option.is_enabled_variable_bound_move,
                 master_option.is_enabled_user_defined_move,
                 master_option.is_enabled_chain_move,
                 master_option.selection_mode,
                 master_option.verbose >= Verbose::Outer);

    /**
     * Print the problem size.
     */
    if (master_option.verbose >= Verbose::Outer) {
        model->print_number_of_variables();
        model->print_number_of_constraints();
    }

    /**
     * Print the problem size.
     */
    if (master_option.verbose >= Verbose::Outer) {
        model->print_number_of_variables();
        model->print_number_of_constraints();
    }

    /**
     * Disable the Chain move for the problem with no monic
     * constraints (set partitioning, set packing, set covering, cardinality,
     * and invariant knapsack).
     */
    if (master_option.is_enabled_chain_move &&
        !model->has_monic_constraints()) {
        master_option.is_enabled_chain_move = false;
        utility::print_warning(
            "Chain move was disabled because the problem does not include any "
            "monic constraints (set partitioning/packing/covering, "
            "cardinality, and invariant knapsack).",
            master_option.verbose >= Verbose::Warning);
    }

    /**
     * Enables the default neighborhood moves. Special neighborhood moves will
     * be enabled when optimization stagnates.
     */
    if (master_option.is_enabled_binary_move) {
        model->neighborhood().binary().enable();
    }

    if (master_option.is_enabled_integer_move) {
        model->neighborhood().integer().enable();
    }

    if (master_option.is_enabled_user_defined_move) {
        model->neighborhood().user_defined().enable();
    }

    if (master_option.selection_mode != model::SelectionMode::None) {
        model->neighborhood().selection().enable();
    }

    /**
     * Check whether there exist special neighborhood moves or not.
     */
    bool has_special_neighborhood_moves =
        (model->neighborhood().aggregation().moves().size() +
         model->neighborhood().precedence().moves().size() +
         model->neighborhood().variable_bound().moves().size()) > 0;

    if (master_option.is_enabled_chain_move) {
        has_special_neighborhood_moves = true;
    }

    /**
     * Prepare a random generator.
     */
    std::mt19937 get_rand_mt(master_option.seed);

    /**
     * Set local and global penalty coefficient for each constraint.
     */
    for (auto&& proxy : model->constraint_proxies()) {
        for (auto&& constraint : proxy.flat_indexed_constraints()) {
            constraint.local_penalty_coefficient() =
                master_option.initial_penalty_coefficient;
            constraint.global_penalty_coefficient() =
                master_option.initial_penalty_coefficient;
        }
    }

    /**
     * Create memory which stores updating count for each decision variable.
     */
    Memory memory(model);

    /**
     * Prepare feasible solutions archive.
     */
    SolutionArchive<T_Variable, T_Expression>  //
        solution_archive(master_option.historical_data_capacity,
                         model->is_minimization());

    /**
     * Compute the values of expressions, constraints, and the objective
     * function according to the initial solution.
     */
    model->update();

    /**
     * Prepare local variables to be updated in the following process.
     */
    Solution_T        current_solution = model->export_solution();
    Solution_T        previous_solution;
    IncumbentHolder_T incumbent_holder;

    model::SolutionScore current_solution_score = model->evaluate({});
    model::SolutionScore previous_solution_score;

    [[maybe_unused]] int update_status = incumbent_holder.try_update_incumbent(
        current_solution, current_solution_score);

    int number_of_lagrange_dual_iterations = 0;
    int number_of_local_search_iterations  = 0;
    int number_of_tabu_search_iterations   = 0;
    int number_of_tabu_search_loops        = 0;

    bool is_terminated = false;

    /**
     * Start optimization.
     */
    utility::print_single_line(  //
        master_option.verbose >= Verbose::Outer);

    utility::print_message(  //
        "Optimization starts.", master_option.verbose >= Verbose::Outer);

    /**
     * Solve Lagrange dual to obtain a better initial solution ß(Optional).
     */
    if (master_option.is_enabled_lagrange_dual && !is_terminated) {
        utility::print_single_line(master_option.verbose == Verbose::Outer);
        if (!model->is_linear()) {
            utility::print_warning(
                "Solving lagrange dual was skipped because the problem is "
                "nonlinear.",
                master_option.verbose >= Verbose::Warning);
        } else if (model->number_of_selection_variables() > 0) {
            utility::print_warning(
                "Solving lagrange dual was skipped because it does not "
                "applicable to selection variables.",
                master_option.verbose >= Verbose::Warning);
        } else {
            double elapsed_time = time_keeper.clock();

            /**
             *  Check the terminating condition.
             */
            if (elapsed_time > master_option.time_max) {
                is_terminated = true;
                utility::print_message(
                    "Outer loop was terminated because of time-over (" +
                        utility::to_string(elapsed_time, "%.3f") + "sec).",
                    master_option.verbose >= Verbose::Outer);
            } else if (incumbent_holder.feasible_incumbent_objective() <=
                       master_option.target_objective_value) {
                is_terminated = true;
                utility::print_message(
                    "Outer loop was terminated because of feasible objective "
                    "reaches the target limit.",
                    master_option.verbose >= Verbose::Outer);
            }

            if (!is_terminated) {
                /**
                 *  Prepare an option object for local search.
                 */
                Option option                    = master_option;
                option.lagrange_dual.time_offset = elapsed_time;

                /**
                 * Prepare the initial variable values.
                 */
                std::vector<model::ValueProxy<T_Variable>>
                    initial_variable_value_proxies =
                        current_solution.variable_value_proxies;

                /**
                 * Run the lagrange dual search.
                 */
                auto result =
                    lagrange_dual::solve(model,                           //
                                         option,                          //
                                         initial_variable_value_proxies,  //
                                         incumbent_holder);               //

                /**
                 * Update the current solution.
                 */
                previous_solution       = current_solution;
                previous_solution_score = current_solution_score;

                current_solution = result.incumbent_holder
                                       .global_augmented_incumbent_solution();
                current_solution_score =
                    result.incumbent_holder.global_augmented_incumbent_score();

                /**
                 * Update the historical data.
                 */
                if (master_option.is_enabled_collect_historical_data &&
                    result.historical_feasible_solutions.size() > 0) {
                    solution_archive.push(result.historical_feasible_solutions);
                }

                /**
                 * Update the global augmented incumbent solution if it was
                 * improved by the Lagrange dual search.
                 */
                update_status = incumbent_holder.try_update_incumbent(
                    result.incumbent_holder
                        .global_augmented_incumbent_solution(),
                    result.incumbent_holder.global_augmented_incumbent_score());

                /**
                 * Update the feasible incumbent solution if it was improved by
                 * the Lagrange dual search
                 */
                if (result.incumbent_holder.is_found_feasible_solution()) {
                    update_status = incumbent_holder.try_update_incumbent(
                        result.incumbent_holder.feasible_incumbent_solution(),
                        result.incumbent_holder.feasible_incumbent_score());
                }

                /**
                 * Preserve the number of iterations for solving the Lagrange
                 * dual problem.
                 */
                number_of_lagrange_dual_iterations =
                    result.number_of_iterations;

                /**
                 * Measure the elapsed time to solve the Lagrange dual problem.
                 */
                elapsed_time = time_keeper.clock();

                /**
                 * Print the search summary.
                 */
                utility::print_message("Solving Lagrange dual finished. ",
                                       master_option.verbose >= Verbose::Outer);

                utility::print_info(  //
                    " -- Total elapsed time: " +
                        utility::to_string(elapsed_time, "%.3f") + "sec",
                    master_option.verbose >= Verbose::Outer);

                utility::print_info(
                    " -- Global augmented incumbent objective: " +
                        utility::to_string(
                            incumbent_holder
                                    .global_augmented_incumbent_objective() *
                                model->sign(),
                            "%.3f"),
                    master_option.verbose >= Verbose::Outer);

                utility::print_info(
                    " -- Feasible incumbent objective: " +
                        utility::to_string(
                            incumbent_holder.feasible_incumbent_objective() *
                                model->sign(),
                            "%.3f"),
                    master_option.verbose >= Verbose::Outer);
            }
        }
    }

    /**
     * Run a local search to improve the initial solution (optional).
     */
    if (master_option.is_enabled_local_search && !is_terminated) {
        double elapsed_time = time_keeper.clock();
        utility::print_single_line(master_option.verbose == Verbose::Outer);
        /**
         *  Check the terminating condition.
         */
        if (elapsed_time > master_option.time_max) {
            is_terminated = true;
            utility::print_message(
                "Outer loop was terminated because of time-over (" +
                    utility::to_string(elapsed_time, "%.3f") + "sec).",
                master_option.verbose >= Verbose::Outer);
        } else if (incumbent_holder.feasible_incumbent_objective() <=
                   master_option.target_objective_value) {
            is_terminated = true;
            utility::print_message(
                "Outer loop was terminated because of feasible objective "
                "reaches the target limit.",
                master_option.verbose >= Verbose::Outer);
        }

        if (!is_terminated) {
            /**
             *  Prepare an option object for local search.
             */
            Option option                   = master_option;
            option.local_search.time_offset = elapsed_time;

            /**
             * Prepare the initial variable values.
             */
            std::vector<model::ValueProxy<T_Variable>>
                initial_variable_value_proxies =
                    current_solution.variable_value_proxies;

            /**
             * Run the local search.
             */
            auto result =
                local_search::solve(model,                           //
                                    option,                          //
                                    initial_variable_value_proxies,  //
                                    incumbent_holder,                //
                                    memory);                         //

            /**
             * Update the current solution.
             */
            previous_solution       = current_solution;
            previous_solution_score = current_solution_score;

            current_solution =
                result.incumbent_holder.global_augmented_incumbent_solution();
            current_solution_score =
                result.incumbent_holder.global_augmented_incumbent_score();

            /**
             * Update the historical data.
             */
            if (master_option.is_enabled_collect_historical_data &&
                result.historical_feasible_solutions.size() > 0) {
                solution_archive.push(result.historical_feasible_solutions);
            }

            /**
             * Update the global augmented incumbent solution if it was improved
             * by the local search.
             */
            update_status = incumbent_holder.try_update_incumbent(
                result.incumbent_holder.global_augmented_incumbent_solution(),
                result.incumbent_holder.global_augmented_incumbent_score());

            /**
             * Update the feasible incumbent solution if it was improved by the
             * local search.
             */
            if (result.incumbent_holder.is_found_feasible_solution()) {
                update_status = incumbent_holder.try_update_incumbent(
                    result.incumbent_holder.feasible_incumbent_solution(),
                    result.incumbent_holder.feasible_incumbent_score());
            }

            /**
             * Update the memory.
             */
            memory = result.memory;

            /**
             * Preserve the number of iterations for the local search.
             */
            number_of_local_search_iterations = result.number_of_iterations;

            /**
             * Measure the elapsed time for the local search.
             */
            elapsed_time = time_keeper.clock();

            /**
             * Print the search summary.
             */
            utility::print_message("Local search finished.",
                                   master_option.verbose >= Verbose::Outer);

            utility::print_info(  //
                " -- Total elapsed time: " +
                    utility::to_string(elapsed_time, "%.3f") + "sec",
                master_option.verbose >= Verbose::Outer);

            utility::print_info(
                " -- Global augmented incumbent objective: " +
                    utility::to_string(
                        incumbent_holder
                                .global_augmented_incumbent_objective() *
                            model->sign(),
                        "%.3f"),
                master_option.verbose >= Verbose::Outer);

            utility::print_info(
                " -- Feasible incumbent objective: " +
                    utility::to_string(
                        incumbent_holder.feasible_incumbent_objective() *
                            model->sign(),
                        "%.3f"),
                master_option.verbose >= Verbose::Outer);
        }
    }

    /**
     * Run tabu searches to find better solutions.
     */
    int iteration                                  = 0;
    int no_global_augmented_incumbent_update_count = 0;
    int no_update_count                            = 0;
    tabu_search::TabuSearchTerminationStatus termination_status =
        tabu_search::TabuSearchTerminationStatus::ITERATION_OVER;

    bool penalty_coefficient_reset_flag = false;

    int    inital_tabu_tenure = master_option.tabu_search.initial_tabu_tenure;
    double pruning_rate_threshold =
        master_option.tabu_search.pruning_rate_threshold;
    int number_of_initial_modification = 0;
    int iteration_max = master_option.tabu_search.iteration_max;

    ImprovabilityScreeningMode improvability_screening_mode =
        master_option.improvability_screening_mode;
    if (improvability_screening_mode == ImprovabilityScreeningMode::Automatic) {
        improvability_screening_mode = ImprovabilityScreeningMode::Intensive;
    }

    while (!is_terminated) {
        utility::print_single_line(master_option.verbose == Verbose::Outer);
        /**
         *  Check the terminating condition.
         */
        double elapsed_time = time_keeper.clock();
        if (elapsed_time > master_option.time_max) {
            utility::print_message(
                "Outer loop was terminated because of time-over (" +
                    utility::to_string(elapsed_time, "%.3f") + "sec).",
                master_option.verbose >= Verbose::Outer);
            is_terminated = true;
        } else if (iteration >= master_option.iteration_max) {
            utility::print_message(
                "Outer loop was terminated because of iteration limit (" +
                    utility::to_string(iteration, "%d") + " iterations).",
                master_option.verbose >= Verbose::Outer);
            is_terminated = true;
        } else if (incumbent_holder.feasible_incumbent_objective() <=
                   master_option.target_objective_value) {
            utility::print_message(
                "Outer loop was terminated because of feasible objective "
                "reaches the target limit (" +
                    utility::to_string(iteration, "%d") + " iterations).",
                master_option.verbose >= Verbose::Outer);
            is_terminated = true;
        } else if (iteration > 0 &&
                   termination_status ==
                       tabu_search::TabuSearchTerminationStatus::OPTIMAL) {
            utility::print_message(
                "Outer loop was terminated because an optimal solution was "
                "found.",
                master_option.verbose >= Verbose::Outer);
            is_terminated = true;
        }

        if (is_terminated) {
            break;
        }

        /**
         *  Prepare an option object for tabu search.
         */
        Option option = master_option;

        option.improvability_screening_mode = improvability_screening_mode;

        if (option.tabu_search.is_enabled_automatic_iteration_adjustment) {
            option.tabu_search.iteration_max = iteration_max;
        }

        option.tabu_search.time_offset = elapsed_time;
        option.tabu_search.seed += iteration;
        option.tabu_search.number_of_initial_modification =
            number_of_initial_modification;
        option.tabu_search.initial_tabu_tenure    = inital_tabu_tenure;
        option.tabu_search.pruning_rate_threshold = pruning_rate_threshold;

        /**
         * Prepare the initial variable values.
         */
        std::vector<model::ValueProxy<T_Variable>>
            initial_variable_value_proxies =
                current_solution.variable_value_proxies;

        /**
         * Run the tabu search.
         */
        auto result = tabu_search::solve(model,                           //
                                         option,                          //
                                         initial_variable_value_proxies,  //
                                         incumbent_holder,                //
                                         memory);

        /**
         * Update the global augmented incumbent solution if it was improved
         * by the tabu search.
         */
        update_status = incumbent_holder.try_update_incumbent(
            result.incumbent_holder.global_augmented_incumbent_solution(),
            result.incumbent_holder.global_augmented_incumbent_score());

        /**
         * Update the feasible incumbent solution if it was improved by the tabu
         * search.
         */
        if (result.incumbent_holder.is_found_feasible_solution()) {
            update_status = incumbent_holder.try_update_incumbent(
                result.incumbent_holder.feasible_incumbent_solution(),
                result.incumbent_holder.feasible_incumbent_score());
        }

        /**
         * Update the memory.
         */
        memory = result.memory;

        /**
         * Update the termination status.
         */
        termination_status = result.termination_status;

        /**
         * Reset the penalty coefficients if the incumbent solution was not
         * updated for specified count of loops.
         */
        if ((update_status && (IncumbentHolderConstant::
                                   STATUS_GLOBAL_AUGMENTED_INCUMBENT_UPDATE))) {
            /**
             * Reset the count of loops with no-update if the incumbent solution
             * was updated.
             */
            no_global_augmented_incumbent_update_count = 0;
            penalty_coefficient_reset_flag             = false;
        } else {
            no_global_augmented_incumbent_update_count++;

            if (no_global_augmented_incumbent_update_count ==
                master_option.penalty_coefficient_reset_count_threshold) {
                /**
                 * Activate the flag to reset the penalty coefficients if the
                 * count reaches the specified value, and then also reset the
                 * count.
                 */
                penalty_coefficient_reset_flag             = true;
                no_global_augmented_incumbent_update_count = 0;
            } else {
                penalty_coefficient_reset_flag = false;
            }
        }

        /**
         * Update the current solution which is employed as the initial solution
         * of the next loop. Whether the local penalty coefficients should be
         * relaxed or tightened, is also determined.
         */
        previous_solution       = current_solution;
        previous_solution_score = current_solution_score;

        auto result_local_augmented_incumbent_solution =
            result.incumbent_holder.local_augmented_incumbent_solution();
        auto result_global_augmented_incumbent_solution =
            result.incumbent_holder.global_augmented_incumbent_solution();

        auto result_local_augmented_incumbent_objective =
            result.incumbent_holder.local_augmented_incumbent_objective();
        auto result_global_augmented_incumbent_objective =
            result.incumbent_holder.global_augmented_incumbent_objective();

        auto result_local_augmented_incumbent_score =
            result.incumbent_holder.local_augmented_incumbent_score();
        auto result_global_augmented_incumbent_score =
            result.incumbent_holder.global_augmented_incumbent_score();

        bool employing_local_augmented_solution_flag  = false;
        bool employing_global_augmented_solution_flag = false;
        bool employing_previous_solution_flag         = false;

        bool penalty_coefficient_tightening_flag      = false;
        bool penalty_coefficient_relaxing_flag        = false;
        bool is_enabled_forcibly_initial_modification = false;

        /**
         * If the improvability_screening_mode was set to "Automatic", determine
         * the mode according to the search status so far.
         */
        if (master_option.improvability_screening_mode ==
            ImprovabilityScreeningMode::Automatic) {
            if (result.termination_status ==
                tabu_search::TabuSearchTerminationStatus::NO_MOVE) {
                improvability_screening_mode = ImprovabilityScreeningMode::Soft;
            } else if (result.total_update_status &
                       IncumbentHolderConstant::
                           STATUS_GLOBAL_AUGMENTED_INCUMBENT_UPDATE) {
                /**
                 * If the incumbent solution was updated in the last loop, the
                 * improvability screening mode is set to "Intensive" to search
                 * better solutions by intensive search.
                 */
                improvability_screening_mode =
                    ImprovabilityScreeningMode::Intensive;
            } else if (result.is_few_permissible_neighborhood) {
                /**
                 * If the last loop encountered a situation where there is no
                 * permissible solution, the improvability screening mode is set
                 * to "Soft" for search diversity.
                 */
                improvability_screening_mode = ImprovabilityScreeningMode::Soft;
            } else if (!result.is_found_new_feasible_solution) {
                /**
                 * If the last loop failed to find any feasible solution, the
                 * improvability screening mode is set to "Aggressive" to
                 * prioritize the search for feasible solutions.
                 */
                improvability_screening_mode =
                    ImprovabilityScreeningMode::Aggressive;
            } else {
                /**
                 * Otherwise, the improvability screening mode is set to "Soft"
                 * for search diversity.
                 */
                improvability_screening_mode = ImprovabilityScreeningMode::Soft;
            }
        }

        /**
         * Determine the initial solution for the next loop and flags to tighten
         * or relax the penalty coefficients.
         */

        /**
         *  NOTE: The gap can takes both of positive and negative value.
         */
        double gap = result_global_augmented_incumbent_objective -
                     result_local_augmented_incumbent_objective;

        if (result.total_update_status &
            IncumbentHolderConstant::STATUS_GLOBAL_AUGMENTED_INCUMBENT_UPDATE) {
            /**
             * If the global incumbent solution was updated in the last loop,
             * the global incumbent is employed as the initial solution for the
             * next loop. The penalty coefficients are to be relaxed.
             */
            employing_global_augmented_solution_flag = true;
            penalty_coefficient_relaxing_flag        = true;

            /**
             * The variable no_update_count is a count that the
             * result.total_update_status consecutively has taken the value of
             * IncumbentHolderConstant::NO_UPDATE.
             */
            no_update_count = 0;
        } else {
            if (result.total_update_status ==
                IncumbentHolderConstant::STATUS_NO_UPDATED) {
                /**
                 * If the last loop failed to find any local/global incumbent
                 * solution, the global incumbent solution is employed as
                 * the initial solution for the next loop with some initial
                 * modifications. The penalty coefficients are to be relaxed
                 * after two consecutive search failures.
                 */
                employing_global_augmented_solution_flag = true;
                is_enabled_forcibly_initial_modification = true;

                if (result_local_augmented_incumbent_score.is_feasible) {
                    penalty_coefficient_relaxing_flag = true;
                    no_update_count                   = 0;
                } else {
                    if (no_update_count < 1) {
                        no_update_count++;
                    } else {
                        penalty_coefficient_relaxing_flag = true;
                        no_update_count                   = 0;
                    }
                }
            } else {
                /**
                 * If a local incumbent solution was updated the last loop, the
                 * initial solution for the next loop and flags to tighten or
                 * relax the penalty coefficients will be determined by
                 * complexed rules below.
                 */
                no_update_count = 0;

                if (gap < constant::EPSILON) {
                    /**
                     * The fact that the gap is negative implies that the
                     * obtained local incumbent solution is worse than the
                     * global incumbent solution. For this case, the initial
                     * solution for the next loop is reset by the global
                     * incumbent solution with some initial modifications. The
                     * penalty coefficients are to be relaxed or tightened
                     * according to the feasibility of the local incumbent
                     * solution.
                     */
                    employing_global_augmented_solution_flag = true;
                    is_enabled_forcibly_initial_modification = true;

                    if (result_local_augmented_incumbent_score.is_feasible) {
                        penalty_coefficient_relaxing_flag = true;
                    } else {
                        penalty_coefficient_tightening_flag = true;
                    }
                } else {
                    /**
                     * If the gap is positive and the local incumbent solution
                     * is feasible, the local incumbent solution is employed as
                     * the initial solution for the next loop. The penalty
                     * coefficients are to be relaxed.
                     */
                    if (result_local_augmented_incumbent_score.is_feasible) {
                        employing_local_augmented_solution_flag = true;
                        penalty_coefficient_relaxing_flag       = true;

                    } else {
                        /**
                         * For the case that the gap is positive and the local
                         * incumbent solution is not feasible, the following
                         * rules will be applied:
                         * (1) If no feasible solution has been found in the
                         * previous loops:
                         * -> The obtained local incumbent solution is
                         * employed as the initial solution for the next loop.
                         * (2) If a feasible solution has been found in the
                         * previous loops:
                         * (2.1) If the obtained local incumbent solution
                         * improves the objective function value or global
                         * penalty than those of global incumbent solution:
                         * -> The obtained local incumbent solution is
                         * employed as the initial solution for the next loop.
                         * (2.2) Otherwise; if the obtained local incumbent
                         * solution does not improve either objective function
                         * value or global penalty:
                         * -> The previous initial solution is employed as the
                         * initial solution for the next loop.
                         *
                         * For all cases, penalty coefficients are to be
                         * tightened.
                         */
                        if (incumbent_holder.is_found_feasible_solution()) {
                            if ((result_local_augmented_incumbent_score
                                     .objective <
                                 previous_solution_score.objective) ||
                                (result_local_augmented_incumbent_score
                                     .global_penalty <
                                 previous_solution_score.global_penalty)) {
                                employing_local_augmented_solution_flag = true;

                            } else {
                                employing_previous_solution_flag = true;
                            }
                        } else {
                            employing_local_augmented_solution_flag = true;
                        }
                        penalty_coefficient_tightening_flag = true;
                    }
                }
            }
        }

        /**
         * Set the current solution and its score according to the result of the
         * above process.
         */
        if (employing_global_augmented_solution_flag) {
            current_solution       = result_global_augmented_incumbent_solution;
            current_solution_score = result_global_augmented_incumbent_score;
        } else if (employing_local_augmented_solution_flag) {
            current_solution       = result_local_augmented_incumbent_solution;
            current_solution_score = result_local_augmented_incumbent_score;
        } else if (employing_previous_solution_flag) {
            current_solution       = previous_solution;
            current_solution_score = previous_solution_score;
        } else {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "An error ocurred in determining the next initial solution."));
        }

        /**
         * This flag will be used to control the initial modification.
         */
        bool is_changed = current_solution.variable_value_proxies !=
                          previous_solution.variable_value_proxies;

        /**
         * Update the historical data.
         */
        if (master_option.is_enabled_collect_historical_data &&
            result.historical_feasible_solutions.size() > 0) {
            solution_archive.push(result.historical_feasible_solutions);
        }

        /**
         * Update the local penalty coefficients.
         */
        if (penalty_coefficient_reset_flag) {
            /**
             * If penalty_coefficient_reset_flag is true, the penalty
             * coefficients will be reset.
             */
            for (auto&& proxy : model->constraint_proxies()) {
                for (auto&& constraint : proxy.flat_indexed_constraints()) {
                    constraint.reset_local_penalty_coefficient();
                }
            }
        } else if (penalty_coefficient_tightening_flag) {
            /**
             * Tighten the local penalty coefficients.
             */
            double total_violation         = 0.0;
            double total_squared_violation = 0.0;
            for (const auto& proxy : result_local_augmented_incumbent_solution
                                         .violation_value_proxies) {
                for (const auto& element : proxy.flat_indexed_values()) {
                    total_violation += element;
                    total_squared_violation += element * element;
                }
            }

            double balance = master_option.penalty_coefficient_updating_balance;
            double gap     = result_global_augmented_incumbent_objective -
                         result_local_augmented_incumbent_objective;

            for (auto&& proxy : model->constraint_proxies()) {
                const auto& violation_values =
                    result_local_augmented_incumbent_solution
                        .violation_value_proxies[proxy.index()]
                        .flat_indexed_values();

                for (auto&& constraint : proxy.flat_indexed_constraints()) {
                    double delta_penalty_constant =
                        std::max(0.0, gap) / total_violation;
                    double delta_penalty_proportional =
                        std::max(0.0, gap) / total_squared_violation *
                        violation_values[constraint.flat_index()];

                    constraint.local_penalty_coefficient() +=
                        master_option.penalty_coefficient_tightening_rate *
                        (balance * delta_penalty_constant +
                         (1.0 - balance) * delta_penalty_proportional);
                }

                if (master_option.is_enabled_grouping_penalty_coefficient) {
                    double max_local_penalty_coefficient = 0;
                    for (auto&& constraint : proxy.flat_indexed_constraints()) {
                        max_local_penalty_coefficient =
                            std::max(max_local_penalty_coefficient,
                                     constraint.local_penalty_coefficient());
                    }
                    for (auto&& constraint : proxy.flat_indexed_constraints()) {
                        constraint.local_penalty_coefficient() =
                            max_local_penalty_coefficient;
                    }
                }

                /**
                 * Penalty coefficients are bounded by the initial penalty
                 * coefficient specified in option.
                 */
                for (auto&& constraint : proxy.flat_indexed_constraints()) {
                    constraint.local_penalty_coefficient() =
                        std::min(constraint.local_penalty_coefficient(),
                                 master_option.initial_penalty_coefficient);
                }
            }
        } else if (penalty_coefficient_relaxing_flag) {
            /**
             * Relax the local penalty coefficients of which
             * corresponding constraints are satisfied.
             */
            double penalty_coefficient_relaxing_rate =
                master_option.penalty_coefficient_relaxing_rate;

            if (result.objective_constraint_rate > constant::EPSILON) {
                if (result_local_augmented_incumbent_score.is_feasible) {
                    const double MARGIN = 1.0;
                    penalty_coefficient_relaxing_rate =
                        std::min(penalty_coefficient_relaxing_rate,
                                 result.objective_constraint_rate * MARGIN);
                }
            }

            for (auto&& proxy : model->constraint_proxies()) {
                const auto& violation_values =
                    current_solution.violation_value_proxies[proxy.index()]
                        .flat_indexed_values();

                for (auto&& constraint : proxy.flat_indexed_constraints()) {
                    if (violation_values[constraint.flat_index()] <
                        constant::EPSILON) {
                        constraint.local_penalty_coefficient() *=
                            penalty_coefficient_relaxing_rate;
                    }
                }
            }
        }

        /**
         * Update the initial tabu tenure for the next loop.
         */
        if (master_option.tabu_search
                .is_enabled_automatic_tabu_tenure_adjustment) {
            if (result.total_update_status &
                IncumbentHolderConstant::
                    STATUS_GLOBAL_AUGMENTED_INCUMBENT_UPDATE) {
                inital_tabu_tenure =
                    std::min(master_option.tabu_search.initial_tabu_tenure,
                             model->number_of_mutable_variables());
            } else if (result.total_update_status ==
                       IncumbentHolderConstant::STATUS_NO_UPDATED) {
                inital_tabu_tenure = std::max(
                    option.tabu_search.initial_tabu_tenure - 1,
                    std::min(master_option.tabu_search.initial_tabu_tenure,
                             model->number_of_mutable_variables()));
            } else if (result.tabu_tenure >
                       option.tabu_search.initial_tabu_tenure) {
                inital_tabu_tenure =
                    std::min(option.tabu_search.initial_tabu_tenure + 1,
                             model->number_of_mutable_variables());

            } else {
                inital_tabu_tenure = std::max(
                    option.tabu_search.initial_tabu_tenure - 1,
                    std::min(master_option.tabu_search.initial_tabu_tenure,
                             model->number_of_mutable_variables()));
            }
        } else {
            inital_tabu_tenure = master_option.tabu_search.initial_tabu_tenure;
        }

        /**
         * Update the number of initial modification for the next loop.
         */
        if (result.total_update_status &
            IncumbentHolderConstant::STATUS_FEASIBLE_INCUMBENT_UPDATE) {
            number_of_initial_modification = 0;
        } else if (result.total_update_status &
                   IncumbentHolderConstant::
                       STATUS_GLOBAL_AUGMENTED_INCUMBENT_UPDATE) {
            number_of_initial_modification = 0;
        } else {
            if (master_option.tabu_search.is_enabled_initial_modification &&
                (!is_changed || is_enabled_forcibly_initial_modification)) {
                int nominal_number_of_initial_modification  //
                    = static_cast<int>(
                        std::floor(master_option.tabu_search
                                       .initial_modification_fixed_rate *
                                   inital_tabu_tenure));

                int initial_modification_random_width = static_cast<int>(
                    option.tabu_search.initial_modification_randomize_rate *
                    nominal_number_of_initial_modification);

                int number_of_initial_modification_temp =
                    nominal_number_of_initial_modification;
                if (initial_modification_random_width > 0) {
                    number_of_initial_modification_temp +=
                        get_rand_mt() %
                            (2 * initial_modification_random_width) -
                        initial_modification_random_width;
                }

                number_of_initial_modification =
                    std::max(1, number_of_initial_modification_temp);
            } else {
                number_of_initial_modification = 0;
            }
        }

        /**
         * Update the maximum number of iterations and pruning rate threshold
         * for the next loop.
         */
        if (master_option.tabu_search
                .is_enabled_automatic_iteration_adjustment &&
            result.number_of_iterations == option.tabu_search.iteration_max) {
            int iteration_max_temp = 0;
            if (result.total_update_status &
                IncumbentHolderConstant::
                    STATUS_GLOBAL_AUGMENTED_INCUMBENT_UPDATE) {
                iteration_max_temp  //
                    = static_cast<int>(ceil(
                        result.last_local_augmented_incumbent_update_iteration *
                        master_option.tabu_search.iteration_increase_rate));

            } else {
                iteration_max_temp  //
                    = static_cast<int>(ceil(
                        option.tabu_search.iteration_max *
                        master_option.tabu_search.iteration_increase_rate));
            }
            iteration_max =
                std::max(master_option.tabu_search.initial_tabu_tenure,
                         std::min(master_option.tabu_search.iteration_max,
                                  iteration_max_temp));

            if (iteration_max == master_option.tabu_search.iteration_max) {
                pruning_rate_threshold =
                    master_option.tabu_search.pruning_rate_threshold;
            } else {
                pruning_rate_threshold = 1.0;
            }
        }

        /**
         * Reset chain moves if the global augmented objective was updated.
         */
        if (result.total_update_status &
            IncumbentHolderConstant::STATUS_GLOBAL_AUGMENTED_INCUMBENT_UPDATE) {
            if (option.is_enabled_chain_move) {
                model->neighborhood().chain().clear_moves();
            }
        }

        /**
         * Update the neighborhood moves to be employed for the next loop.
         */
        bool is_activated_special_neighborhood_move   = false;
        bool is_deactivated_special_neighborhood_move = false;

        if (result.total_update_status &
            IncumbentHolderConstant::STATUS_GLOBAL_AUGMENTED_INCUMBENT_UPDATE) {
            /**
             * Disable the special neighborhood moves if the incumbent was
             * updated.
             */
            /// Aggregation
            if (master_option.is_enabled_aggregation_move) {
                if (model->neighborhood().aggregation().is_enabled()) {
                    model->neighborhood().aggregation().disable();
                    is_deactivated_special_neighborhood_move = true;
                }
            }

            /// Precedence
            if (master_option.is_enabled_precedence_move) {
                if (model->neighborhood().precedence().is_enabled()) {
                    model->neighborhood().precedence().disable();
                    is_deactivated_special_neighborhood_move = true;
                }
            }

            /// Variable Bound
            if (master_option.is_enabled_variable_bound_move) {
                if (model->neighborhood().variable_bound().is_enabled()) {
                    model->neighborhood().variable_bound().disable();
                    is_deactivated_special_neighborhood_move = true;
                }
            }

            /// Chain
            if (master_option.is_enabled_chain_move) {
                if (model->neighborhood().chain().is_enabled()) {
                    model->neighborhood().chain().disable();
                    is_deactivated_special_neighborhood_move = true;
                }
            }

        } else {
            /**
             * Enable the special neighborhood moves if the incumbent was
             * not updated.
             */
            if (result.number_of_iterations ==
                master_option.tabu_search.iteration_max) {
                /// Aggregation
                if (master_option.is_enabled_aggregation_move) {
                    if (!model->neighborhood().aggregation().is_enabled()) {
                        model->neighborhood().aggregation().enable();
                        is_activated_special_neighborhood_move = true;
                    }
                }

                /// Precedence
                if (master_option.is_enabled_precedence_move) {
                    if (!model->neighborhood().precedence().is_enabled()) {
                        model->neighborhood().precedence().enable();
                        is_activated_special_neighborhood_move = true;
                    }
                }

                /// Variable Bound
                if (master_option.is_enabled_variable_bound_move) {
                    if (!model->neighborhood().variable_bound().is_enabled()) {
                        model->neighborhood().variable_bound().enable();
                        is_activated_special_neighborhood_move = true;
                    }
                }

                /// Chain
                if (master_option.is_enabled_chain_move) {
                    if (!model->neighborhood().chain().is_enabled()) {
                        model->neighborhood().chain().enable();
                        is_activated_special_neighborhood_move = true;
                    }
                }
            }
        }

        /**
         * Sort and deduplicate registered chain moves.
         */
        if (model->neighborhood().chain().is_enabled() &&
            master_option.chain_move_capacity > 0) {
            model->neighborhood().chain().sort_moves();
            model->neighborhood().chain().deduplicate_moves();
        }

        /**
         * Reduce the registered chain moves.
         */
        if (static_cast<int>(model->neighborhood().chain().moves().size()) >
            master_option.chain_move_capacity) {
            switch (master_option.chain_move_reduce_mode) {
                case ChainMoveReduceMode::OverlapRate: {
                    model->neighborhood().chain().reduce_moves(
                        master_option.chain_move_capacity);
                    break;
                }
                case ChainMoveReduceMode::Shuffle: {
                    model->neighborhood().chain().shuffle_moves(&get_rand_mt);
                    model->neighborhood().chain().reduce_moves(
                        master_option.chain_move_capacity);
                    break;
                }
                default: {
                    throw std::logic_error(utility::format_error_location(
                        __FILE__, __LINE__, __func__,
                        "The specified Chain move reduce mode is invalid."));
                }
            }
        }

        /**
         * Reset the availability of special neighborhood moves.
         */
        if (model->neighborhood().is_enabled_special_neighborhood_move()) {
            model->neighborhood()
                .reset_special_neighborhood_moves_availability();
        }

        /**
         * Preserve the number of iterations of the previous loop.
         */
        number_of_tabu_search_iterations += result.number_of_iterations;
        number_of_tabu_search_loops++;

        /**
         * Update the elapsed time.
         */
        elapsed_time = time_keeper.clock();

        /**
         * Print the summary.
         */
        utility::print_message(
            "Tabu search loop (" + std::to_string(iteration + 1) + "/" +
                std::to_string(master_option.iteration_max) + ") finished.",
            master_option.verbose >= Verbose::Outer);

        utility::print_info(  //
            " -- Total elapsed time: " +
                utility::to_string(elapsed_time, "%.3f") + "sec",
            master_option.verbose >= Verbose::Outer);

        utility::print_info(
            " -- Global augmented incumbent objective: " +
                utility::to_string(
                    incumbent_holder.global_augmented_incumbent_objective() *
                        model->sign(),
                    "%.3f"),
            master_option.verbose >= Verbose::Outer);
        utility::print_info(
            " -- Feasible incumbent objective: " +
                utility::to_string(
                    incumbent_holder.feasible_incumbent_objective() *
                        model->sign(),
                    "%.3f"),
            master_option.verbose >= Verbose::Outer);

        /**
         * Print the optimization status of the previous tabu search loop.
         */
        if (result.total_update_status &
            IncumbentHolderConstant::STATUS_FEASIBLE_INCUMBENT_UPDATE) {
            utility::print_message("Feasible incumbent objective was updated. ",
                                   master_option.verbose >= Verbose::Outer);

        } else if (result.total_update_status &
                   IncumbentHolderConstant::
                       STATUS_GLOBAL_AUGMENTED_INCUMBENT_UPDATE) {
            utility::print_message("Global incumbent objective was updated. ",
                                   master_option.verbose >= Verbose::Outer);
        } else {
            utility::print_message("Incumbent objective was not updated.",
                                   master_option.verbose >= Verbose::Outer);
        }

        /**
         * Print the number of violative constraints.
         */
        if (!current_solution.is_feasible) {
            int number_of_violative_constraints = 0;
            /**
             * Due to the slow speed of standard output in the Windows DOS,
             * printing all violations will affect performance. To avoid
             * this problem, the maximum number of violations to be printed
             * is 20.
             */
            const int MAX_NUMBER_OF_PRINT_ITEMS = 20;

            utility::print_message(
                "The current solution does not satisfy the following "
                "constraints:",
                master_option.verbose >= Verbose::Outer);

            for (const auto& proxy : current_solution.violation_value_proxies) {
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
                                master_option.verbose >= Verbose::Outer);
                        }
                    }
                }
            }
            if (number_of_violative_constraints > MAX_NUMBER_OF_PRINT_ITEMS) {
                utility::print_info("and much more...",
                                    master_option.verbose >= Verbose::Outer);
            }

            utility::print_message(
                "There are " + std::to_string(number_of_violative_constraints) +
                    " violative constraints.",
                master_option.verbose >= Verbose::Outer);
        }

        /**
         * Print message if the penalty coefficients were changed.
         */
        if (penalty_coefficient_reset_flag) {
            utility::print_message(
                "The penalty coefficients were reset due to search "
                "stagnation.",
                master_option.verbose >= Verbose::Outer);
        } else if (penalty_coefficient_relaxing_flag) {
            utility::print_message(  //
                "The penalty coefficients were relaxed.",
                master_option.verbose >= Verbose::Outer);
        } else if (penalty_coefficient_tightening_flag) {
            utility::print_message(  //
                "The penalty coefficients were tightened.",
                master_option.verbose >= Verbose::Outer);
        }

        /**
         * Print the initial tabu tenure for the next loop.
         */
        utility::print_message("The tabu tenure for the next loop was set to " +
                                   std::to_string(inital_tabu_tenure) + ".",
                               master_option.verbose >= Verbose::Outer);

        /**
         * Print the improvability screening_mode
         */
        switch (improvability_screening_mode) {
            case ImprovabilityScreeningMode::Off: {
                utility::print_message(
                    "No improvability screening will be applied for the next "
                    "loop.",
                    master_option.verbose >= Verbose::Outer);
                break;
            }
            case ImprovabilityScreeningMode::Soft: {
                utility::print_message(
                    "The soft improvability screening will be applied in the "
                    "next loop.",
                    master_option.verbose >= Verbose::Outer);
                break;
            }
            case ImprovabilityScreeningMode::Aggressive: {
                utility::print_message(
                    "The aggressive improvability screening will be applied in "
                    "the next loop.",
                    master_option.verbose >= Verbose::Outer);
                break;
            }
            case ImprovabilityScreeningMode::Intensive: {
                utility::print_message(
                    "The intensive improvability screening will be applied in "
                    "the next loop.",
                    master_option.verbose >= Verbose::Outer);
                break;
            }
            default: {
                throw std::logic_error(utility::format_error_location(
                    __FILE__, __LINE__, __func__,
                    "The specified improvability screening mode is invalid."));
            }
        }

        /**
         * Print the initial solution for the next loop.
         */
        if (employing_global_augmented_solution_flag) {
            utility::print_message(
                "The next loop will be start from the global incumbent "
                "solution.",
                master_option.verbose >= Verbose::Outer);
        } else if (employing_local_augmented_solution_flag) {
            utility::print_message(
                "The next loop will be start from the local incumbent solution "
                "found in the previous loop.",
                master_option.verbose >= Verbose::Outer);
        } else if (employing_previous_solution_flag) {
            utility::print_message(
                "The next loop will be start from the same initial solution of "
                "the previous loop.",
                master_option.verbose >= Verbose::Outer);
        }

        /**
         * Print the number of initial modification for the next
         * loop.
         */
        if (number_of_initial_modification > 0) {
            utility::print_message(
                "For the initial " +
                    std::to_string(number_of_initial_modification) +
                    " iterations in the next loop, the solution will be "
                    "randomly updated to escape from the local optimum.",
                master_option.verbose >= Verbose::Outer);
        }

        /**
         * Print the number of iterations for the next loop.
         */
        if (master_option.tabu_search
                .is_enabled_automatic_iteration_adjustment) {
            utility::print_message(
                "The maximum number of iterations for the next loop was set "
                "to " +
                    std::to_string(iteration_max) + ".",
                master_option.verbose >= Verbose::Outer);
        }

        /**
         * Print a message of the special neighborhood moves
         * activation/deactivation.
         */
        if (is_deactivated_special_neighborhood_move &&
            has_special_neighborhood_moves) {
            utility::print_message("Special neighborhood moves were disabled.",
                                   master_option.verbose >= Verbose::Outer);
        }

        if (is_activated_special_neighborhood_move &&
            has_special_neighborhood_moves) {
            utility::print_message("Special neighborhood moves were enabled.",
                                   master_option.verbose >= Verbose::Outer);
        }

        /**
         * Print the number of the stored chain moves.
         */
        if (model->neighborhood().chain().is_enabled()) {
            utility::print_message(
                "There are " +
                    std::to_string(
                        model->neighborhood().chain().moves().size()) +
                    " stored chain moves.",
                master_option.verbose >= Verbose::Outer);
        }

        model->callback();
        iteration++;
    }

    /**
     * Get the finish time.
     */
    std::string finish_date_time = utility::date_time();

    /**
     * If a feasible solution is found in optimization, the incumbent
     * solution is defined by the solution with the best objective function
     * value among the feasible solutions. If no feasible solution is found,
     * the incumbent solution is substituted by solution with the best
     * augmented solution which has smallest sum of the objective function
     * value and the penalty value.
     */
    auto incumbent =
        incumbent_holder.is_found_feasible_solution()
            ? incumbent_holder.feasible_incumbent_solution()
            : incumbent_holder.global_augmented_incumbent_solution();

    /**
     * All value of the expressions and the constraints are updated forcibly
     * to take into account the cases they are disabled.
     */
    model->import_variable_values(incumbent.variable_value_proxies);
    model->update();
    incumbent = model->export_solution();

    auto named_solution = model->convert_to_named_solution(incumbent);

    /**
     * Export the final penalty coefficient values.
     */
    std::unordered_map<std::string, model::ValueProxy<double>>
        named_penalty_coefficients;

    std::vector<model::ValueProxy<double>> local_penalty_coefficient_proxies =
        model->export_local_penalty_coefficient_proxies();

    const int   CONSTRAINT_PROXIES_SIZE = model->constraint_proxies().size();
    const auto& constraint_names        = model->constraint_names();
    for (auto i = 0; i < CONSTRAINT_PROXIES_SIZE; i++) {
        named_penalty_coefficients[constraint_names[i]] =
            local_penalty_coefficient_proxies[i];
    }

    /**
     * Export the final variable update counts.
     */
    std::unordered_map<std::string, model::ValueProxy<int>> named_update_counts;
    const int   VARIABLE_PROXIES_SIZE = model->variable_proxies().size();
    const auto& variable_names        = model->variable_names();

    const auto& update_counts = memory.update_counts();
    for (auto i = 0; i < VARIABLE_PROXIES_SIZE; i++) {
        named_update_counts[variable_names[i]] = update_counts[i];
    }

    /**
     * Prepare the result object to return.
     */
    Result<T_Variable, T_Expression> result;
    result.solution                          = named_solution;
    result.status.penalty_coefficients       = named_penalty_coefficients;
    result.status.update_counts              = named_update_counts;
    result.status.is_found_feasible_solution = named_solution.is_feasible();
    result.status.start_date_time            = start_date_time;
    result.status.finish_date_time           = finish_date_time;
    result.status.elapsed_time               = time_keeper.elapsed_time();
    result.status.number_of_lagrange_dual_iterations =
        number_of_lagrange_dual_iterations;
    result.status.number_of_local_search_iterations =
        number_of_local_search_iterations;
    result.status.number_of_tabu_search_iterations =
        number_of_tabu_search_iterations;
    result.status.number_of_tabu_search_loops = number_of_tabu_search_loops;
    result.solution_archive                   = solution_archive;

    return result;
}
}  // namespace solver
}  // namespace printemps

#endif
/*****************************************************************************/
// END
/*****************************************************************************/