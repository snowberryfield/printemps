/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef CPPMH_SOLVER_H__
#define CPPMH_SOLVER_H__

#include <iostream>
#include <cstdlib>
#include <random>

#include "../model/model.h"
#include "../utility/utility.h"

#include "incumbent_holder.h"
#include "option.h"
#include "tabu_search/tabu_search.h"
#include "local_search/local_search.h"

namespace cppmh {
namespace solver {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
model::NamedSolution<T_Variable, T_Expression> solve(
    model::Model<T_Variable, T_Expression>* a_model) {
    Option option;
    return solve(a_model, option);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression>
model::NamedSolution<T_Variable, T_Expression> solve(
    model::Model<T_Variable, T_Expression>* a_model,  //
    const Option&                           a_OPTION) {
    /**
     * Define type aliases.
     */
    using Model_T          = model::Model<T_Variable, T_Expression>;
    using Solution_T       = model::Solution<T_Variable, T_Expression>;
    using IncumbetHolder_T = IncumbentHolder<T_Variable, T_Expression>;

    /**
     * Start to measure computational time.
     */
    utility::TimeKeeper time_keeper;

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

    model->reset_variable_sense();

    model->verify_problem();
    model->setup_default_neighborhood(
        master_option.is_enabled_parallel_neighborhood_update);

    model->setup_fixed_sensitivities();

    /**
     * If the user-defined_neighborhood is set, default neighborhood should
     * be disabled to avoid possible inconsistencies.
     */
    if (model->neighborhood().is_enabled_user_defined_move()) {
        model->neighborhood().disable_default_move();
    }

    model->setup_has_fixed_variables();

    model->verify_and_correct_selection_variables_initial_values(
        master_option.is_enabled_initial_value_correction,
        master_option.verbose >= Verbose::Warning);

    model->verify_and_correct_binary_variables_initial_values(
        master_option.is_enabled_initial_value_correction,
        master_option.verbose >= Verbose::Warning);

    model->verify_and_correct_integer_variables_initial_values(
        master_option.is_enabled_initial_value_correction,
        master_option.verbose >= Verbose::Warning);

    /**
     * Create local and global penalty coefficient for each constraint.
     */
    std::vector<model::ValueProxy<double>> penalty_coefficient_proxies =
        model->generate_constraint_parameter_proxies(
            master_option.initial_penalty_coefficient);

    auto global_penalty_coefficient_proxies = penalty_coefficient_proxies;
    auto local_penalty_coefficient_proxies  = penalty_coefficient_proxies;

    /**
     * Compute expressions, constraints, and objective according to initial
     * values.
     */
    model->update();

    Solution_T       current_solution = model->export_solution();
    IncumbetHolder_T incumbent_holder;

    model::SolutionScore current_solution_score =
        model->evaluate({},                                 //
                        local_penalty_coefficient_proxies,  //
                        global_penalty_coefficient_proxies);

    [[maybe_unused]] int update_status = incumbent_holder.try_update_incumbent(
        current_solution, current_solution_score);

    /**
     * Run a local search to improve the initial solution (optional).
     */
    if (master_option.is_enabled_local_search) {
        double elapsed_time = time_keeper.clock();

        /**
         *  Check the terminating condition.
         */
        if (elapsed_time > master_option.time_max) {
            utility::print_message(
                "Outer loop was terminated because of time-over (" +
                    utility::to_string(elapsed_time, "%.3f") + "sec).",
                master_option.verbose >= Verbose::Outer);
        }

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
            local_search::solve(model,                               //
                                option,                              //
                                local_penalty_coefficient_proxies,   //
                                global_penalty_coefficient_proxies,  //
                                initial_variable_value_proxies,      //
                                incumbent_holder);                   //

        /**
         * Update the current solution.
         */
        current_solution =
            result.incumbent_holder.global_augmented_incumbent_solution();

        /**
         * Update the global augmented incumbent solution if it was improved by
         * the local search.
         */
        update_status = incumbent_holder.try_update_incumbent(
            result.incumbent_holder.global_augmented_incumbent_solution(),
            result.incumbent_holder.global_augmented_incumbent_score());

        /**
         * Update the feasible incumbent solution if it was improved by the
         * local search
         */
        if (result.incumbent_holder.found_feasible_solution()) {
            update_status = incumbent_holder.try_update_incumbent(
                result.incumbent_holder.feasible_incumbent_solution(),
                result.incumbent_holder.feasible_incumbent_score());
        }

        elapsed_time = time_keeper.clock();
        utility::print_message(
            "Local search was finished. Total elapsed time: " +
                utility::to_string(elapsed_time, "%.3f") + "sec",
            master_option.verbose >= Verbose::Outer);

        utility::print_info(
            " - Global augmented incumbent objective: " +
                utility::to_string(
                    incumbent_holder.global_augmented_incumbent_objective() *
                        model->sign(),
                    "%.3f"),
            master_option.verbose >= Verbose::Outer);

        utility::print_info(
            " - Feasible incumbent objective: " +
                utility::to_string(
                    incumbent_holder.feasible_incumbent_objective() *
                        model->sign(),
                    "%.3f"),
            master_option.verbose >= Verbose::Outer);
    }

    /**
     * Run tabu searches to find better solutions.
     */
    int iteration                = 0;
    int last_total_update_status = IncumbentHolderConstant::STATUS_NO_UPDATED;
    int last_tabu_tenure         = 0;
    while (true) {
        /**
         *  Check the terminating condition.
         */
        double elapsed_time = time_keeper.clock();
        if (elapsed_time > master_option.time_max) {
            utility::print_message(
                "Outer loop was terminated because of time-over (" +
                    utility::to_string(elapsed_time, "%.3f") + "sec).",
                master_option.verbose >= Verbose::Outer);
            break;
        }

        if (iteration >= master_option.iteration_max) {
            utility::print_message(
                "Outer loop was terminated because of iteration limit (" +
                    utility::to_string(iteration, "%d") + " iterations).",
                master_option.verbose >= Verbose::Outer);
            break;
        }

        if (incumbent_holder.feasible_incumbent_objective() <=
            master_option.target_objective_value) {
            utility::print_message(
                "Outer loop was terminated because of feasible objective "
                "reaches the target limit (" +
                    utility::to_string(iteration, "%d") + " iterations).",
                master_option.verbose >= Verbose::Outer);
            break;
        }

        /**
         *  Prepare an option object for local search.
         */
        Option option                  = master_option;
        option.tabu_search.time_offset = elapsed_time;
        option.tabu_search.seed += iteration;
        if (last_total_update_status &
            IncumbentHolderConstant::STATUS_GLOBAL_AUGMENTED_INCUMBENT_UPDATE) {
            option.tabu_search.number_of_initial_modification =
                last_tabu_tenure;
        }

        /**
         * Prepare the initial variable values.
         */
        std::vector<model::ValueProxy<T_Variable>>
            initial_variable_value_proxies =
                current_solution.variable_value_proxies;

        /**
         * Run the tabu search.
         */
        auto result = tabu_search::solve(model,  //
                                         option,
                                         local_penalty_coefficient_proxies,   //
                                         global_penalty_coefficient_proxies,  //
                                         initial_variable_value_proxies,      //
                                         incumbent_holder);

        last_tabu_tenure         = result.tabu_tenure;
        last_total_update_status = result.total_update_status;

        /**
         * Update the current solution.
         */
        current_solution =
            result.incumbent_holder.global_augmented_incumbent_solution();

        /**
         * Update the global augmented incumbent solution if it was improved by
         * the tabu search.
         */
        update_status = incumbent_holder.try_update_incumbent(
            result.incumbent_holder.global_augmented_incumbent_solution(),
            result.incumbent_holder.global_augmented_incumbent_score());

        /**
         * Update the feasible incumbent solution if it was improved by the
         * tabu search
         */
        if (result.incumbent_holder.found_feasible_solution()) {
            update_status = incumbent_holder.try_update_incumbent(
                result.incumbent_holder.feasible_incumbent_solution(),
                result.incumbent_holder.feasible_incumbent_score());
        }

        Solution_T incumbent_inner =
            result.incumbent_holder.local_augmented_incumbent_solution();
        /**
         * Update the local penalty coefficients.
         */
        if (incumbent_inner.is_feasible) {
            /**
             * Relax the local penalty coefficients if the local augmented
             * incumbent solution obtained in the last tabu search is feasible.
             */
            for (auto& proxy : local_penalty_coefficient_proxies) {
                for (auto&& element : proxy.flat_indexed_values()) {
                    element *= master_option.penalty_coefficient_relaxing_ratio;
                }
            }
        } else {
            /**
             * Tighten the local penalty coefficients if the local augmented
             * incumbent solution obtained in the last tabu search is
             * infeasible.
             */
            for (auto& proxy : local_penalty_coefficient_proxies) {
                int id = proxy.id();

                auto violation_values =
                    incumbent_inner.violation_value_proxies[id]
                        .flat_indexed_values();

                if (master_option.is_enabled_grouping_penalty_coefficient) {
                    /**
                     * Apply same penalty coefficient for constraints belonging
                     * to same group. (optional)
                     */
                    auto max_violation = utility::max(violation_values);
                    for (auto&& element : proxy.flat_indexed_values()) {
                        element +=
                            master_option.penalty_coefficient_tightening_ratio *
                            max_violation;
                    }

                } else {
                    /**
                     * Apply independent penalty coefficient for each
                     * constraint. (optional)
                     */
                    int flat_index = 0;
                    for (auto&& element : proxy.flat_indexed_values()) {
                        element +=
                            master_option.penalty_coefficient_tightening_ratio *
                            violation_values[flat_index++];
                    }
                }

                /**
                 * Penalty coefficients are bounded by the initial penalty
                 * coefficient specified in option.
                 */
                for (auto&& element : proxy.flat_indexed_values()) {
                    element = std::min(
                        element, master_option.initial_penalty_coefficient);
                }
            }
        }
        elapsed_time = time_keeper.clock();
        utility::print_message(
            "Tabu search loop (" + std::to_string(iteration + 1) + "/" +
                std::to_string(master_option.iteration_max) +
                ") was finished. Total elapsed time: " +
                utility::to_string(elapsed_time, "%.3f") + "sec",
            master_option.verbose >= Verbose::Outer);
        utility::print_info(
            " - Global augmented incumbent objective: " +
                utility::to_string(
                    incumbent_holder.global_augmented_incumbent_objective() *
                        model->sign(),
                    "%.3f"),
            master_option.verbose >= Verbose::Outer);
        utility::print_info(
            " - Feasible incumbent objective: " +
                utility::to_string(
                    incumbent_holder.feasible_incumbent_objective() *
                        model->sign(),
                    "%.3f"),
            master_option.verbose >= Verbose::Outer);

        if (last_total_update_status &
            IncumbentHolderConstant::STATUS_FEASIBLE_INCUMBENT_UPDATE) {
            utility::print_message(
                "Feasible incumbent objective has beed updated. ",
                master_option.verbose >= Verbose::Outer);
        } else if (last_total_update_status &
                   IncumbentHolderConstant::
                       STATUS_GLOBAL_AUGMENTED_INCUMBENT_UPDATE) {
            utility::print_message(
                "Global incumbent objective has beed updated. ",
                master_option.verbose >= Verbose::Outer);
        } else {
            utility::print_message(
                "Incumbent objective has not beed updated. For the initial " +
                    std::to_string(last_tabu_tenure) +
                    " iterations in the next loop, the solution will be "
                    "randomly updated to escape from the local minimum.",
                master_option.verbose >= Verbose::Outer);
        }

        model->callback();
        iteration++;
    }

    /**
     * If a feasible solution is found in optimization, the incumbent solution
     * is defined by the solution with the best objective function value among
     * the feasible solutions. If no feasible solution is found, the incumbent
     * solution is substituted by solution with the best augmented solution
     * which has smallest sum of the objective function value and the penalty
     * value.
     * */

    auto incumbent =
        incumbent_holder.found_feasible_solution()
            ? incumbent_holder.feasible_incumbent_solution()
            : incumbent_holder.global_augmented_incumbent_solution();

    /**
     * All value of the expressions and the constraints are
     * updated forcibly to take into account the cases they are
     * disabled.
     */
    model->import_variable_values(incumbent.variable_value_proxies);
    model->update();
    incumbent = model->export_solution();

    auto named_solution = model->convert_to_named_solution(incumbent);
    return named_solution;
}
}  // namespace solver
}  // namespace cppmh

#endif
/*****************************************************************************/
// END
/*****************************************************************************/