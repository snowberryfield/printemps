/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_LAGRANGE_DUAL_CORE_LAGRANGE_DUAL_CORE_H__
#define PRINTEMPS_SOLVER_LAGRANGE_DUAL_CORE_LAGRANGE_DUAL_CORE_H__

#include "lagrange_dual_core_print.h"
#include "lagrange_dual_core_termination_status.h"
#include "lagrange_dual_core_result.h"

namespace printemps {
namespace solver {
namespace lagrange_dual {
namespace core {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
void bound_dual(
    model::Model<T_Variable, T_Expression>*       a_model_ptr,
    std::vector<multi_array::ValueProxy<double>>* a_dual_value_proxies) {
    for (auto&& proxy : a_model_ptr->constraint_proxies()) {
        for (auto&& constraint : proxy.flat_indexed_constraints()) {
            const int PROXY_INDEX = constraint.proxy_index();
            const int FLAT_INDEX  = constraint.flat_index();

            auto& lagrange_multiplier =
                (*a_dual_value_proxies)[PROXY_INDEX].flat_indexed_values(
                    FLAT_INDEX);

            switch (constraint.sense()) {
                case model_component::ConstraintSense::Less: {
                    lagrange_multiplier = std::max(lagrange_multiplier, 0.0);
                    break;
                }
                case model_component::ConstraintSense::Greater: {
                    lagrange_multiplier = std::min(lagrange_multiplier, 0.0);
                    break;
                }
                case model_component::ConstraintSense::Equal: {
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
LagrangeDualCoreResult<T_Variable, T_Expression> solve(
    model::Model<T_Variable, T_Expression>* a_model_ptr,  //
    solution::IncumbentHolder<T_Variable, T_Expression>*
        a_incumbent_holder_ptr,  //
    std::vector<solution::SparseSolution<T_Variable, T_Expression>>*
                          a_feasible_solutions_ptr,          //
    const option::Option& a_OPTION,                          //
    const std::vector<multi_array::ValueProxy<T_Variable>>&  //
        a_INITIAL_VARIABLE_VALUE_PROXIES) {
    /**
     * Define type aliases.
     */
    using Model_T = model::Model<T_Variable, T_Expression>;
    using IncumbentHolder_T =
        solution::IncumbentHolder<T_Variable, T_Expression>;

    /**
     * Start to measure computational time.
     */
    utility::TimeKeeper time_keeper;
    time_keeper.set_start_time();

    /**
     * Copy arguments as local variables.
     */
    Model_T*           model_ptr            = a_model_ptr;
    option::Option     option               = a_OPTION;
    IncumbentHolder_T* incumbent_holder_ptr = a_incumbent_holder_ptr;

    /**
     * Reset the local augmented incumbent.
     */
    incumbent_holder_ptr->reset_local_augmented_incumbent();

    /**
     * Reset the feasible solutions storage.
     */
    a_feasible_solutions_ptr->clear();

    model_ptr->import_variable_values(a_INITIAL_VARIABLE_VALUE_PROXIES);
    model_ptr->update();

    /**
     * Initialize the solution and update the model.
     */
    solution::SolutionScore solution_score = model_ptr->evaluate({});

    int update_status =
        incumbent_holder_ptr->try_update_incumbent(model_ptr, solution_score);
    int total_update_status = 0;

    /**
     * Prepare the primal solution.
     */
    auto primal_incumbent = model_ptr->export_solution();

    /**
     * Prepare the dual solution as lagrange multipliers.
     */
    auto dual_value_proxies =
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
     * Prepare other local variables.
     */
    auto termination_status = LagrangeDualCoreTerminationStatus::ITERATION_OVER;

    /**
     * Print the header of optimization progress table and print the
     * initial solution status.
     */
    utility::print_single_line(option.verbose >= option::verbose::Full);
    utility::print_message("Lagrange dual starts.",
                           option.verbose >= option::verbose::Full);
    print_table_header(option.verbose >= option::verbose::Full);
    print_table_initial(model_ptr,             //
                        -HUGE_VALF,            //
                        step_size,             //
                        solution_score,        //
                        incumbent_holder_ptr,  //
                        option.verbose >= option::verbose::Full);

    /**
     * Iterations start.
     */
    int iteration = 0;

    auto& variable_ptrs   = model_ptr->variable_reference().variable_ptrs;
    auto& constraint_ptrs = model_ptr->constraint_reference().constraint_ptrs;

    while (true) {
        /**
         *  Check the terminating condition.
         */
        double elapsed_time = time_keeper.clock();
        if (elapsed_time > option.lagrange_dual.time_max) {
            termination_status = LagrangeDualCoreTerminationStatus::TIME_OVER;
            break;
        }

        if (elapsed_time + option.lagrange_dual.time_offset > option.time_max) {
            termination_status = LagrangeDualCoreTerminationStatus::TIME_OVER;
            break;
        }

        if (iteration >= option.lagrange_dual.iteration_max) {
            termination_status =
                LagrangeDualCoreTerminationStatus::ITERATION_OVER;
            break;
        }

        if (incumbent_holder_ptr->feasible_incumbent_objective() <=
            option.target_objective_value) {
            termination_status =
                LagrangeDualCoreTerminationStatus::REACH_TARGET;
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
            const double CONSTRAINT_VALUE =
                constraint_ptrs[i]->constraint_value();
            const int PROXY_INDEX = constraint_ptrs[i]->proxy_index();
            const int FLAT_INDEX  = constraint_ptrs[i]->flat_index();

            dual_value_proxies[PROXY_INDEX].flat_indexed_values(FLAT_INDEX) +=
                step_size * CONSTRAINT_VALUE;
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
                const auto&  constraint_ptr = item.first;
                const double SENSITIVITY    = item.second;

                const int PROXY_INDEX = constraint_ptr->proxy_index();
                const int FLAT_INDEX  = constraint_ptr->flat_index();

                coefficient +=
                    dual_value_proxies[PROXY_INDEX].flat_indexed_values(
                        FLAT_INDEX) *
                    SENSITIVITY * model_ptr->sign();
            }

            variable_ptrs[i]->set_lagrangian_coefficient(coefficient);

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

        update_status = incumbent_holder_ptr->try_update_incumbent(
            model_ptr, solution_score);
        total_update_status = update_status || total_update_status;

        /**
         * Store the current feasible solution.
         */
        if (solution_score.is_feasible) {
            a_feasible_solutions_ptr->push_back(
                model_ptr->export_plain_solution());
        }

        /**
         * Compute the lagrangian value.
         */
        const double LAGRANGIAN =
            model_ptr->compute_lagrangian(dual_value_proxies) *
            model_ptr->sign();

        /**
         * Update the lagrangian incumbent.
         */
        if (LAGRANGIAN > lagrangian_incumbent) {
            lagrangian_incumbent         = LAGRANGIAN;
            auto primal_incumbent        = model_ptr->export_solution();
            dual_value_proxies_incumbent = dual_value_proxies;
        }

        /**
         * Update the lagrangian queue.
         */
        queue.push(LAGRANGIAN);
        const double QUEUE_AVERAGE = queue.average();
        const double QUEUE_MAX     = queue.max();

        /**
         * Adjust the step size.
         */
        if (queue.size() > 0) {
            if (LAGRANGIAN > QUEUE_AVERAGE) {
                step_size *= option.lagrange_dual.step_size_extend_rate;
            }
            if (LAGRANGIAN < QUEUE_MAX) {
                step_size *= option.lagrange_dual.step_size_reduce_rate;
            }
        }

        /**
         * Print the optimization progress.
         */
        if (iteration % std::max(option.lagrange_dual.log_interval, 1) == 0 ||
            update_status > 1) {
            print_table_body(model_ptr,             //
                             iteration,             //
                             LAGRANGIAN,            //
                             step_size,             //
                             solution_score,        //
                             update_status,         //
                             incumbent_holder_ptr,  //
                             option.verbose >= option::verbose::Full);
        }

        /**
         * Terminate the loop if lagrangian converges.
         */
        if (queue.size() == option.lagrange_dual.queue_size &&
            fabs(LAGRANGIAN - QUEUE_AVERAGE) <
                std::max(1.0, fabs(QUEUE_AVERAGE)) *
                    option.lagrange_dual.tolerance) {
            termination_status = LagrangeDualCoreTerminationStatus::CONVERGE;
            break;
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
    LagrangeDualCoreResult<T_Variable, T_Expression> result(
        total_update_status,   //
        iteration,             //
        termination_status,    //
        lagrangian_incumbent,  //
        primal_incumbent,      //
        dual_value_proxies_incumbent);

    return result;
}
}  // namespace core
}  // namespace lagrange_dual
}  // namespace solver
}  // namespace printemps

#endif
/*****************************************************************************/
// END
/*****************************************************************************/