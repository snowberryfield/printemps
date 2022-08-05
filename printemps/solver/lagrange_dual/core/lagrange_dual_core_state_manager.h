/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_LAGRANGE_DUAL_CORE_LAGRANGE_DUAL_CORE_STATE_MANAGER_H__
#define PRINTEMPS_SOLVER_LAGRANGE_DUAL_CORE_LAGRANGE_DUAL_CORE_STATE_MANAGER_H__

#include "lagrange_dual_core_state.h"

namespace printemps::solver::lagrange_dual::core {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class LagrangeDualCoreStateManager {
   private:
    LagrangeDualCoreState<T_Variable, T_Expression>      m_state;
    model::Model<T_Variable, T_Expression>*              m_model_ptr;
    solution::IncumbentHolder<T_Variable, T_Expression>* m_incumbent_holder_ptr;
    Memory<T_Variable, T_Expression>*                    m_memory_ptr;
    option::Option                                       m_option;

   public:
    /*************************************************************************/
    LagrangeDualCoreStateManager(
        model::Model<T_Variable, T_Expression>* a_model_ptr,
        solution::IncumbentHolder<T_Variable, T_Expression>*
                                          a_incumbent_holder_ptr,  //
        Memory<T_Variable, T_Expression>* a_memory_ptr,            //
        const option::Option&             a_OPTION) {
        this->setup(a_model_ptr,             //
                    a_incumbent_holder_ptr,  //
                    a_memory_ptr,            //
                    a_OPTION);
    }

    /*************************************************************************/
    LagrangeDualCoreStateManager(void) {
        this->initialize();
    }

    /*************************************************************************/
    inline void initialize(void) {
        m_state.initialize();
        m_model_ptr            = nullptr;
        m_incumbent_holder_ptr = nullptr;
        m_memory_ptr           = nullptr;
        m_option.initialize();
    }

    /*************************************************************************/
    inline void setup(model::Model<T_Variable, T_Expression>* a_model_ptr,
                      solution::IncumbentHolder<T_Variable, T_Expression>*
                                                        a_incumbent_holder_ptr,  //
                      Memory<T_Variable, T_Expression>* a_memory_ptr,  //
                      const option::Option&             a_OPTION) {
        this->initialize();
        m_model_ptr            = a_model_ptr;
        m_incumbent_holder_ptr = a_incumbent_holder_ptr;
        m_memory_ptr           = a_memory_ptr;
        m_option               = a_OPTION;

        /**
         * Evaluate the initial solution score.
         */
        m_state.current_solution_score  = m_model_ptr->evaluate({});
        m_state.previous_solution_score = m_state.current_solution_score;
        m_state.update_status = m_incumbent_holder_ptr->try_update_incumbent(
            m_model_ptr, m_state.current_solution_score);
        m_state.total_update_status =
            solution::IncumbentHolderConstant::STATUS_NOT_UPDATED;

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
        m_state.primal           = m_model_ptr->export_solution();
        m_state.primal_incumbent = m_state.primal;

        /**
         * Initialize the dual solution as lagrange multipliers.
         */
        m_state.dual = m_model_ptr->generate_constraint_parameter_proxies(0.0);
        this->bound_dual();
        m_state.dual_incumbent = m_state.dual;

        /**
         * Initialize the step size for subgradient algorithm.
         */
        m_state.step_size = 1.0 / m_model_ptr->number_of_variables();

        /**
         * Set up the queue for historical lagrangians.
         */
        m_state.queue.setup(m_option.lagrange_dual.queue_size);

        /**
         * NOTE: The values of the other members of m_state remain at their
         * default values.
         */
    }

    /*************************************************************************/
    inline constexpr void update(
        const solution::SolutionScore& a_SOLUTION_SCORE) {
        /**
         * Update the current solution score with keeping the previous one.
         */
        this->update_solution_score(a_SOLUTION_SCORE);

        /**
         * Update the update status.
         */
        this->update_update_status();

        /**
         * Update the lagrangian value.
         */
        this->update_lagrangian();

        /**
         * Update the step size for subgradient algorithm.
         */
        this->update_step_size();
    }

    /*************************************************************************/
    inline constexpr void update_solution_score(
        const solution::SolutionScore& a_SOLUTION_SCORE) {
        m_state.previous_solution_score = m_state.current_solution_score;
        m_state.current_solution_score  = a_SOLUTION_SCORE;
    }

    /*************************************************************************/
    inline constexpr void update_move(
        neighborhood::Move<T_Variable, T_Expression>* a_selected_move_ptr) {
        m_state.previous_move = m_state.current_move;
        m_state.current_move  = *a_selected_move_ptr;
    }

    /*************************************************************************/
    inline constexpr void update_update_status(void) {
        m_state.update_status = m_incumbent_holder_ptr->try_update_incumbent(
            m_model_ptr, m_state.current_solution_score);
        m_state.total_update_status =
            m_state.update_status | m_state.total_update_status;
    }

    /*************************************************************************/
    inline constexpr void update_lagrangian(void) {
        m_state.lagrangian =
            m_model_ptr->compute_lagrangian(m_state.dual) * m_model_ptr->sign();

        /**
         * Update the lagrangian incumbent.
         */
        if (m_state.lagrangian > m_state.lagrangian_incumbent) {
            m_state.lagrangian_incumbent = m_state.lagrangian;
            m_state.primal_incumbent     = m_model_ptr->export_solution();
            m_state.dual_incumbent       = m_state.dual;
        }

        /**
         * Update the lagrangian queue.
         */
        m_state.queue.push(m_state.lagrangian);
    }

    /*************************************************************************/
    inline constexpr void update_step_size(void) {
        if (m_state.queue.size() > 0) {
            if (m_state.lagrangian > m_state.queue.average()) {
                m_state.step_size *=
                    m_option.lagrange_dual.step_size_extend_rate;
            }
            if (m_state.lagrangian < m_state.queue.max()) {
                m_state.step_size *=
                    m_option.lagrange_dual.step_size_reduce_rate;
            }
        }
    }

    /*************************************************************************/
    inline void update_dual(void) {
        /// Cannot be constexpr for clang.
        auto& constraint_ptrs =
            m_model_ptr->constraint_reference().constraint_ptrs;
        const int CONSTRAINTS_SIZE = constraint_ptrs.size();

#ifdef _OPENMP
#pragma omp parallel for if (m_option.is_enabled_parallel_evaluation) \
    schedule(static)
#endif
        for (auto i = 0; i < CONSTRAINTS_SIZE; i++) {
            const double CONSTRAINT_VALUE =
                constraint_ptrs[i]->constraint_value();
            const int PROXY_INDEX = constraint_ptrs[i]->proxy_index();
            const int FLAT_INDEX  = constraint_ptrs[i]->flat_index();

            m_state.dual[PROXY_INDEX].flat_indexed_values(FLAT_INDEX) +=
                m_state.step_size * CONSTRAINT_VALUE;
        }

        /**
         * Bound the values of dual solution.
         */
        this->bound_dual();
    }

    /*************************************************************************/
    inline void bound_dual(void) {
        for (auto&& proxy : this->m_model_ptr->constraint_proxies()) {
            for (auto&& constraint : proxy.flat_indexed_constraints()) {
                const int PROXY_INDEX = constraint.proxy_index();
                const int FLAT_INDEX  = constraint.flat_index();

                auto& lagrange_multiplier =
                    m_state.dual[PROXY_INDEX].flat_indexed_values(FLAT_INDEX);

                switch (constraint.sense()) {
                    case model_component::ConstraintSense::Equal: {
                        /// nothing to do
                        break;
                    }
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
                    default: {
                        /// nothing to do
                    }
                }
            }
        }
    }

    /*************************************************************************/
    inline constexpr void set_termination_status(
        const LagrangeDualCoreTerminationStatus a_TERMINATION_STATUS) {
        m_state.termination_status = a_TERMINATION_STATUS;
    }
    /*************************************************************************/
    inline constexpr void set_elapsed_time(const double a_ELAPSED_TINE) {
        m_state.elapsed_time = a_ELAPSED_TINE;
    }

    /*************************************************************************/
    inline constexpr void reset_iteration(void) {
        m_state.iteration = 0;
    }

    /*************************************************************************/
    inline constexpr void next_iteration(void) {
        m_state.iteration++;
    }

    /*************************************************************************/
    inline constexpr LagrangeDualCoreState<T_Variable, T_Expression>& state(
        void) {
        return m_state;
    }

    /*************************************************************************/
    inline constexpr const LagrangeDualCoreState<T_Variable, T_Expression>&
    state(void) const {
        return m_state;
    }
};
}  // namespace printemps::solver::lagrange_dual::core
#endif
/*****************************************************************************/
// END
/*****************************************************************************/