/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_LAGRANGE_DUAL_CORE_LAGRANGE_DUAL_CORE_H__
#define PRINTEMPS_SOLVER_LAGRANGE_DUAL_CORE_LAGRANGE_DUAL_CORE_H__

#include "../../abstract_solver_core.h"
#include "lagrange_dual_core_termination_status.h"
#include "lagrange_dual_core_state.h"
#include "lagrange_dual_core_result.h"

namespace printemps {
namespace solver {
namespace lagrange_dual {
namespace core {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class LagrangeDualCore : public AbstractSolverCore<T_Variable, T_Expression> {
   private:
    LagrangeDualCoreState<T_Variable, T_Expression>  m_state;
    LagrangeDualCoreResult<T_Variable, T_Expression> m_result;
    std::mt19937                                     m_get_rand_mt;

    /*************************************************************************/
    inline void preprocess(void) {
        /**
         * Reset the local augmented incumbent.
         */
        this->m_incumbent_holder_ptr->reset_local_augmented_incumbent();

        /**
         * Reset the feasible solutions storage.
         */
        this->m_feasible_solutions.clear();

        /**
         * Initialize the solution and update the model.
         */
        this->m_model_ptr->import_variable_values(
            this->m_initial_variable_value_proxies);
        this->m_model_ptr->update();

        m_state.solution_score = this->m_model_ptr->evaluate({});
        m_state.update_status =
            this->m_incumbent_holder_ptr->try_update_incumbent(
                this->m_model_ptr, m_state.solution_score);

        /**
         * Initialize the update status.
         */
        m_state.total_update_status =
            solution::IncumbentHolderConstant::STATUS_NOT_UPDATED;
        m_state.termination_status =
            LagrangeDualCoreTerminationStatus::ITERATION_OVER;

        /**
         * Initialize the lagrangian.
         */
        m_state.lagrangian           = -HUGE_VALF;
        m_state.lagrangian_incumbent = -HUGE_VALF;

        /**
         * Initialize the primal solution.
         */
        m_state.primal           = this->m_model_ptr->export_solution();
        m_state.primal_incumbent = m_state.primal;

        /**
         * Initialize the dual solution as lagrange multipliers.
         */
        m_state.dual =
            this->m_model_ptr->generate_constraint_parameter_proxies(0.0);
        bound_dual(&m_state.dual);
        m_state.dual_incumbent = m_state.dual;

        /**
         * Initialize the step size for subgradient algorithm.
         */
        m_state.step_size = 1.0 / this->m_model_ptr->number_of_variables();
    }

    /*************************************************************************/
    inline void postprocess(void) {
        /**
         * Prepare the result.
         */
        m_result = LagrangeDualCoreResult<T_Variable, T_Expression>(m_state);
    }

    /*************************************************************************/
    inline bool satisfy_terminate_condition(const double a_ELAPSED_TIME) {
        if (a_ELAPSED_TIME > this->m_option.lagrange_dual.time_max) {
            m_state.termination_status =
                LagrangeDualCoreTerminationStatus::TIME_OVER;
            return true;
        }

        if (a_ELAPSED_TIME + this->m_option.lagrange_dual.time_offset >
            this->m_option.time_max) {
            m_state.termination_status =
                LagrangeDualCoreTerminationStatus::TIME_OVER;
            return true;
        }

        if (m_state.iteration >= this->m_option.lagrange_dual.iteration_max) {
            m_state.termination_status =
                LagrangeDualCoreTerminationStatus::ITERATION_OVER;
            return true;
        }

        if (this->m_incumbent_holder_ptr->feasible_incumbent_objective() <=
            this->m_option.target_objective_value) {
            m_state.termination_status =
                LagrangeDualCoreTerminationStatus::REACH_TARGET;
            return true;
        }
        return false;
    }

    /*************************************************************************/
    inline void bound_dual(
        std::vector<multi_array::ValueProxy<double>>* a_dual) {
        for (auto&& proxy : this->m_model_ptr->constraint_proxies()) {
            for (auto&& constraint : proxy.flat_indexed_constraints()) {
                const int PROXY_INDEX = constraint.proxy_index();
                const int FLAT_INDEX  = constraint.flat_index();

                auto& lagrange_multiplier =
                    (*a_dual)[PROXY_INDEX].flat_indexed_values(FLAT_INDEX);

                switch (constraint.sense()) {
                    case model_component::ConstraintSense::Less: {
                        lagrange_multiplier =
                            std::max(lagrange_multiplier, 0.0);
                        break;
                    }
                    case model_component::ConstraintSense::Greater: {
                        lagrange_multiplier =
                            std::min(lagrange_multiplier, 0.0);
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

    /*************************************************************************/
    inline void print_table_header(const bool a_IS_ENABLED_PRINT) {
        if (!a_IS_ENABLED_PRINT) {
            return;
        }

        utility::print(
            "---------+------------+-----------+----------------------+--------"
            "--------------",
            true);
        utility::print(
            "Iteration| Lagrangian | Step Size |   Current Solution   |"
            "  Incumbent Solution ",
            true);
        utility::print(
            "         |            |           |   Aug.Obj.(Penalty)  | "
            "  Aug.Obj.  Feas.Obj ",
            true);
        utility::print(
            "---------+------------+-----------+----------------------+--------"
            "--------------",
            true);
    }

    /*************************************************************************/
    inline void print_table_initial(const bool a_IS_ENABLED_PRINT) {
        if (!a_IS_ENABLED_PRINT) {
            return;
        }

        std::printf(  //
            " INITIAL |  %9.2e | %9.2e | %9.2e(%9.2e) | %9.2e  %9.2e\n",
            m_state.lagrangian * this->m_model_ptr->sign(), m_state.step_size,
            m_state.solution_score.local_augmented_objective *
                this->m_model_ptr->sign(),
            m_state.solution_score.is_feasible
                ? 0.0
                : m_state.solution_score.local_penalty,  //
            this->m_incumbent_holder_ptr
                    ->global_augmented_incumbent_objective() *
                this->m_model_ptr->sign(),
            this->m_incumbent_holder_ptr->feasible_incumbent_objective() *
                this->m_model_ptr->sign());
    }

    /*************************************************************************/
    inline void print_table_body(const bool a_IS_ENABLED_PRINT) {
        if (!a_IS_ENABLED_PRINT) {
            return;
        }

        char mark_current                    = ' ';
        char mark_global_augmented_incumbent = ' ';
        char mark_feasible_incumbent         = ' ';

        if (m_state.update_status &
            solution::IncumbentHolderConstant::
                STATUS_LOCAL_AUGMENTED_INCUMBENT_UPDATE) {
            mark_current = '!';
        }

        if (m_state.update_status &
            solution::IncumbentHolderConstant::
                STATUS_GLOBAL_AUGMENTED_INCUMBENT_UPDATE) {
            mark_current                    = '#';
            mark_global_augmented_incumbent = '#';
        }

        if (m_state.update_status &  //
            solution::IncumbentHolderConstant::
                STATUS_FEASIBLE_INCUMBENT_UPDATE) {
            mark_current                    = '*';
            mark_global_augmented_incumbent = '*';
            mark_feasible_incumbent         = '*';
        }

        std::printf(  //
            "%8d |  %9.2e | %9.2e |%c%9.2e(%9.2e) |%c%9.2e %c%9.2e\n",
            m_state.iteration,                               //
            m_state.lagrangian * this->m_model_ptr->sign(),  //
            m_state.step_size,                               //
            mark_current,                                    //
            m_state.solution_score.local_augmented_objective *
                this->m_model_ptr->sign(),  //
            m_state.solution_score.is_feasible
                ? 0.0
                : m_state.solution_score.local_penalty,  //
            mark_global_augmented_incumbent,             //
            this->m_incumbent_holder_ptr
                    ->global_augmented_incumbent_objective() *
                this->m_model_ptr->sign(),  //
            mark_feasible_incumbent,        //
            this->m_incumbent_holder_ptr->feasible_incumbent_objective() *
                this->m_model_ptr->sign());
    }

    /*************************************************************************/
    inline void print_table_footer(const bool a_IS_ENABLED_PRINT) {
        if (!a_IS_ENABLED_PRINT) {
            return;
        }
        utility::print(
            "---------+------------+-----------+----------------------+--------"
            "--------------",
            true);
    }

   public:
    /*************************************************************************/
    LagrangeDualCore(void) {
        this->initialize();
    }

    /*************************************************************************/
    LagrangeDualCore(model::Model<T_Variable, T_Expression>* a_model_ptr,     //
                     const std::vector<multi_array::ValueProxy<T_Variable>>&  //
                         a_INITIAL_VARIABLE_VALUE_PROXIES,                    //
                     solution::IncumbentHolder<T_Variable, T_Expression>*     //
                                                       a_incumbent_holder_ptr,  //
                     Memory<T_Variable, T_Expression>* a_memory_ptr,  //
                     const option::Option&             a_OPTION) {
        this->initialize();
        this->setup(a_model_ptr,                       //
                    a_INITIAL_VARIABLE_VALUE_PROXIES,  //
                    a_incumbent_holder_ptr,            //
                    a_memory_ptr,                      //
                    a_OPTION);
    }

    /*************************************************************************/
    virtual ~LagrangeDualCore(void) {
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
        auto                  model_ptr  = this->m_model_ptr;
        [[maybe_unused]] auto memory_ptr = this->m_memory_ptr;
        auto  incumbent_holder_ptr       = this->m_incumbent_holder_ptr;
        auto  option                     = this->m_option;
        auto& feasible_solutions         = this->m_feasible_solutions;
        auto& state                      = m_state;

        /**
         * Preprocess.
         */
        this->preprocess();

        utility::FixedSizeQueue<double> queue(option.lagrange_dual.queue_size);

        /**
         * Print the header of optimization progress table and print the
         * initial solution status.
         */
        utility::print_single_line(option.verbose >= option::verbose::Full);
        utility::print_message("Lagrange dual starts.",
                               option.verbose >= option::verbose::Full);
        print_table_header(option.verbose >= option::verbose::Full);
        print_table_initial(option.verbose >= option::verbose::Full);

        auto& variable_ptrs = model_ptr->variable_reference().variable_ptrs;
        auto& constraint_ptrs =
            model_ptr->constraint_reference().constraint_ptrs;

        const int VARIABLES_SIZE   = variable_ptrs.size();
        const int CONSTRAINTS_SIZE = constraint_ptrs.size();

        /**
         * Iterations start.
         */
        state.iteration = 0;

        while (true) {
            /**
             *  Check the terminating condition.
             */
            const double ELAPSED_TIME = time_keeper.clock();
            if (this->satisfy_terminate_condition(ELAPSED_TIME)) {
                break;
            }

            /**
             * Update the dual solution.
             */
#ifdef _OPENMP
#pragma omp parallel for if (option.is_enabled_parallel_evaluation) \
    schedule(static)
#endif
            for (auto i = 0; i < CONSTRAINTS_SIZE; i++) {
                const double CONSTRAINT_VALUE =
                    constraint_ptrs[i]->constraint_value();
                const int PROXY_INDEX = constraint_ptrs[i]->proxy_index();
                const int FLAT_INDEX  = constraint_ptrs[i]->flat_index();

                state.dual[PROXY_INDEX].flat_indexed_values(FLAT_INDEX) +=
                    state.step_size * CONSTRAINT_VALUE;
            }

            /**
             * Bound the values of dual solution.
             */
            this->bound_dual(&state.dual);

            /**
             * Update the primal optimal solution so that it minimizes
             * lagrangian for the updated dual solution.
             */
#ifdef _OPENMP
#pragma omp parallel for if (option.is_enabled_parallel_evaluation) \
    schedule(static)
#endif
            for (auto i = 0; i < VARIABLES_SIZE; i++) {
                if (variable_ptrs[i]->is_fixed()) {
                    continue;
                }
                double coefficient = variable_ptrs[i]->objective_sensitivity();

                for (auto&& item :
                     variable_ptrs[i]->constraint_sensitivities()) {
                    const auto&  constraint_ptr = item.first;
                    const double SENSITIVITY    = item.second;

                    const int PROXY_INDEX = constraint_ptr->proxy_index();
                    const int FLAT_INDEX  = constraint_ptr->flat_index();

                    coefficient += state.dual[PROXY_INDEX].flat_indexed_values(
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
            state.solution_score = model_ptr->evaluate({});

            state.update_status = incumbent_holder_ptr->try_update_incumbent(
                model_ptr, state.solution_score);
            state.total_update_status |= state.update_status;

            /**
             * Store the current feasible solution.
             */
            if (state.solution_score.is_feasible) {
                feasible_solutions.push_back(
                    model_ptr->export_plain_solution());
            }

            /**
             * Compute the lagrangian value.
             */
            state.lagrangian =
                model_ptr->compute_lagrangian(state.dual) * model_ptr->sign();

            /**
             * Update the lagrangian incumbent.
             */
            if (state.lagrangian > state.lagrangian_incumbent) {
                state.lagrangian_incumbent = state.lagrangian;
                state.primal_incumbent     = model_ptr->export_solution();
                state.dual_incumbent       = state.dual;
            }

            /**
             * Update the lagrangian queue.
             */
            queue.push(state.lagrangian);
            const double QUEUE_AVERAGE = queue.average();
            const double QUEUE_MAX     = queue.max();

            /**
             * Adjust the step size.
             */
            if (queue.size() > 0) {
                if (state.lagrangian > QUEUE_AVERAGE) {
                    state.step_size *=
                        option.lagrange_dual.step_size_extend_rate;
                }
                if (state.lagrangian < QUEUE_MAX) {
                    state.step_size *=
                        option.lagrange_dual.step_size_reduce_rate;
                }
            }

            /**
             * Print the optimization progress.
             */
            if (state.iteration %
                        std::max(option.lagrange_dual.log_interval, 1) ==
                    0 ||
                state.update_status > 1) {
                print_table_body(option.verbose >= option::verbose::Full);
            }

            /**
             * Terminate the loop if lagrangian converges.
             */
            if (queue.size() == option.lagrange_dual.queue_size &&
                fabs(state.lagrangian - QUEUE_AVERAGE) <
                    std::max(1.0, fabs(QUEUE_AVERAGE)) *
                        option.lagrange_dual.tolerance) {
                state.termination_status =
                    LagrangeDualCoreTerminationStatus::CONVERGE;
                break;
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
    inline constexpr const LagrangeDualCoreResult<T_Variable, T_Expression>&
    result(void) const {
        return m_result;
    }
};

}  // namespace core
}  // namespace lagrange_dual
}  // namespace solver
}  // namespace printemps

#endif
/*****************************************************************************/
// END
/*****************************************************************************/