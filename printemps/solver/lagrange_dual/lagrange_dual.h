/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_LAGRANGE_DUAL_LAGRANGE_DUAL_H__
#define PRINTEMPS_SOLVER_LAGRANGE_DUAL_LAGRANGE_DUAL_H__

#include "lagrange_dual_option.h"
#include "lagrange_dual_print.h"
#include "lagrange_dual_result.h"

namespace printemps {
namespace solver {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class IncumbentHolder;

/*****************************************************************************/
struct Option;

namespace lagrange_dual {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
void bound_dual(
    model::Model<T_Variable, T_Expression>*       a_model_ptr,
    std::vector<multi_array::ValueProxy<double>>* a_dual_value_proxies) {
    for (auto&& proxy : a_model_ptr->constraint_proxies()) {
        for (auto&& constraint : proxy.flat_indexed_constraints()) {
            int proxy_index = constraint.proxy_index();
            int flat_index  = constraint.flat_index();

            auto& lagrange_multiplier =
                (*a_dual_value_proxies)[proxy_index].flat_indexed_values(
                    flat_index);

            switch (constraint.sense()) {
                case model::ConstraintSense::Less: {
                    lagrange_multiplier = std::max(lagrange_multiplier, 0.0);
                    break;
                }
                case model::ConstraintSense::Greater: {
                    lagrange_multiplier = std::min(lagrange_multiplier, 0.0);
                    break;
                }
                case model::ConstraintSense::Equal: {
                    /// nothing to do
                    break;
                }
                default: {
                    /// nothing to do
                }
            }
        }
    }
}

/*****************************************************************************/
template <class T_Variable, class T_Expression>
LagrangeDualResult<T_Variable, T_Expression> solve(
    model::Model<T_Variable, T_Expression>* a_model_ptr,     //
    const Option&                           a_OPTION,        //
    const std::vector<multi_array::ValueProxy<T_Variable>>&  //
        a_INITIAL_VARIABLE_VALUE_PROXIES,                    //
    const IncumbentHolder<T_Variable, T_Expression>&         //
        a_INCUMBENT_HOLDER) {
    /**
     * Define type aliases.
     */
    using Model_T           = model::Model<T_Variable, T_Expression>;
    using Result_T          = LagrangeDualResult<T_Variable, T_Expression>;
    using IncumbentHolder_T = IncumbentHolder<T_Variable, T_Expression>;

    /**
     * Start to measure computational time.
     */
    utility::TimeKeeper time_keeper;

    /**
     * Copy arguments as local variables.
     */
    Model_T* model_ptr = a_model_ptr;
    Option   option    = a_OPTION;

    IncumbentHolder_T incumbent_holder = a_INCUMBENT_HOLDER;

    /**
     * Reset the local augmented incumbent.
     */
    incumbent_holder.reset_local_augmented_incumbent();

    model_ptr->import_variable_values(a_INITIAL_VARIABLE_VALUE_PROXIES);
    model_ptr->update();

    /**
     * Initialize the solution and update the model.
     */
    solution::SolutionScore solution_score = model_ptr->evaluate({});

    int update_status =
        incumbent_holder.try_update_incumbent(model_ptr, solution_score);
    int total_update_status = 0;

    /**
     * Prepare the primal solution.
     */
    auto primal_incumbent = model_ptr->export_solution();

    /**
     * Prepare the dual solution as lagrange multipliers.
     */
    std::vector<multi_array::ValueProxy<double>> dual_value_proxies =
        model_ptr->generate_constraint_parameter_proxies(0.0);
    bound_dual(model_ptr, &dual_value_proxies);

    auto dual_value_proxies_incumbent = dual_value_proxies;

    /**
     * Prepare the lagrangian incumbent and its queue.
     */
    double lagrangian_incumbent = -HUGE_VALF;

    utility::FixedSizeQueue<double> queue(option.lagrange_dual.queue_size);

    /**
     * Prepare the step size for subgradient algorithm.
     */
    double step_size = 1.0 / model_ptr->number_of_variables();

    /**
     * Prepare historical solutions holder.
     */
    std::vector<solution::PlainSolution<T_Variable, T_Expression>>
        historical_feasible_solutions;

    /**
     * Prepare other local variables.
     */
    LagrangeDualTerminationStatus termination_status =
        LagrangeDualTerminationStatus::ITERATION_OVER;

    /**
     * Print the header of optimization progress table and print the
     * initial solution status.
     */
    utility::print_single_line(option.verbose >= Verbose::Full);
    utility::print_message("Lagrange dual starts.",
                           option.verbose >= Verbose::Full);
    print_table_header(option.verbose >= Verbose::Full);
    print_table_initial(model_ptr,         //
                        -HUGE_VALF,        //
                        step_size,         //
                        solution_score,    //
                        incumbent_holder,  //
                        option.verbose >= Verbose::Full);

    /**
     * Iterations start.
     */
    int iteration = 0;

    auto variable_ptrs   = model_ptr->variable_reference().variable_ptrs;
    auto constraint_ptrs = model_ptr->constraint_reference().constraint_ptrs;

    while (true) {
        /**
         *  Check the terminating condition.
         */
        double elapsed_time = time_keeper.clock();
        if (elapsed_time > option.lagrange_dual.time_max) {
            termination_status = LagrangeDualTerminationStatus::TIME_OVER;
            break;
        }
        if (elapsed_time + option.lagrange_dual.time_offset > option.time_max) {
            termination_status = LagrangeDualTerminationStatus::TIME_OVER;
            break;
        }
        if (iteration >= option.lagrange_dual.iteration_max) {
            termination_status = LagrangeDualTerminationStatus::ITERATION_OVER;
            break;
        }
        if (incumbent_holder.feasible_incumbent_objective() <=
            option.target_objective_value) {
            termination_status = LagrangeDualTerminationStatus::REACH_TARGET;
            break;
        }

        /**
         * Update the dual solution.
         */
        const int CONSTRAINTS_SIZE = constraint_ptrs.size();
#ifdef _OPENMP
#pragma omp parallel for if (option.is_enabled_parallel_evaluation) \
    schedule(static)
#endif
        for (auto i = 0; i < CONSTRAINTS_SIZE; i++) {
            double constraint_value = constraint_ptrs[i]->constraint_value();
            int    proxy_index      = constraint_ptrs[i]->proxy_index();
            int    flat_index       = constraint_ptrs[i]->flat_index();

            dual_value_proxies[proxy_index].flat_indexed_values(flat_index) +=
                step_size * constraint_value;
        }

        /**
         * Bound the values of dual solution.
         */
        bound_dual(model_ptr, &dual_value_proxies);

        /**
         * Update the primal optimal solution so that it minimizes lagrangian
         * for the updated dual solution.
         */
        const int VARIABLES_SIZE = variable_ptrs.size();
#ifdef _OPENMP
#pragma omp parallel for if (option.is_enabled_parallel_evaluation) \
    schedule(static)
#endif
        for (auto i = 0; i < VARIABLES_SIZE; i++) {
            if (variable_ptrs[i]->is_fixed()) {
                continue;
            }
            double coefficient = variable_ptrs[i]->objective_sensitivity();

            for (auto&& item : variable_ptrs[i]->constraint_sensitivities()) {
                const auto& constraint_ptr = item.first;
                double      sensitivity    = item.second;

                int proxy_index = constraint_ptr->proxy_index();
                int flat_index  = constraint_ptr->flat_index();

                coefficient +=
                    dual_value_proxies[proxy_index].flat_indexed_values(
                        flat_index) *
                    sensitivity * model_ptr->sign();
            }

            if (coefficient > 0) {
                if (model_ptr->is_minimization()) {
                    variable_ptrs[i]->set_value_if_mutable(
                        variable_ptrs[i]->lower_bound());
                } else {
                    variable_ptrs[i]->set_value_if_mutable(
                        variable_ptrs[i]->upper_bound());
                }

            } else {
                if (model_ptr->is_minimization()) {
                    variable_ptrs[i]->set_value_if_mutable(
                        variable_ptrs[i]->upper_bound());
                } else {
                    variable_ptrs[i]->set_value_if_mutable(
                        variable_ptrs[i]->lower_bound());
                }
            }
        }

        /**
         * Update the model.
         */
        model_ptr->update();
        solution_score = model_ptr->evaluate({});

        update_status =
            incumbent_holder.try_update_incumbent(model_ptr, solution_score);
        total_update_status = update_status || total_update_status;

        /**
         * Push the current solution to historical data.
         */
        if (solution_score.is_feasible) {
            historical_feasible_solutions.push_back(
                model_ptr->export_plain_solution());
        }

        /**
         * Compute the lagrangian value.
         */
        double lagrangian = model_ptr->compute_lagrangian(dual_value_proxies) *
                            model_ptr->sign();

        /**
         * Update the lagrangian incumbent.
         */
        if (lagrangian > lagrangian_incumbent) {
            lagrangian_incumbent         = lagrangian;
            auto primal_incumbent        = model_ptr->export_solution();
            dual_value_proxies_incumbent = dual_value_proxies;
        }

        /**
         * Update the lagrangian queue.
         */
        queue.push(lagrangian);
        double queue_average = queue.average();
        double queue_max     = queue.max();

        /**
         * Adjust the step size.
         */
        if (queue.size() > 0) {
            if (lagrangian > queue_average) {
                step_size *= option.lagrange_dual.step_size_extend_rate;
            }
            if (lagrangian < queue_max) {
                step_size *= option.lagrange_dual.step_size_reduce_rate;
            }
        }

        /**
         * Print the optimization progress.
         */
        if (iteration % std::max(option.lagrange_dual.log_interval, 1) == 0 ||
            update_status > 1) {
            print_table_body(model_ptr,         //
                             iteration,         //
                             lagrangian,        //
                             step_size,         //
                             solution_score,    //
                             update_status,     //
                             incumbent_holder,  //
                             option.verbose >= Verbose::Full);
        }

        /**
         * Terminate the loop if lagrangian converges.
         */
        if (queue.size() == option.lagrange_dual.queue_size &&
            fabs(lagrangian - queue_average) <
                std::max(1.0, fabs(queue_average)) *
                    option.lagrange_dual.tolerance) {
            termination_status = LagrangeDualTerminationStatus::CONVERGE;
            break;
        }

        iteration++;
    }

    /**
     * Print the footer of the optimization progress table.
     */
    print_table_footer(option.verbose >= Verbose::Full);

    /**
     * Prepare the result.
     */
    Result_T result;
    result.lagrangian                    = lagrangian_incumbent;
    result.primal_solution               = primal_incumbent;
    result.dual_value_proxies            = dual_value_proxies_incumbent;
    result.incumbent_holder              = incumbent_holder;
    result.total_update_status           = total_update_status;
    result.number_of_iterations          = iteration;
    result.termination_status            = termination_status;
    result.historical_feasible_solutions = historical_feasible_solutions;

    return result;
}
}  // namespace lagrange_dual
}  // namespace solver
}  // namespace printemps

#endif
/*****************************************************************************/
// END
/*****************************************************************************/