/*****************************************************************************/
// Copyright (c) 2020-2023 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_PDLP_CORE_PDLP_CORE_STATE_MANAGER_H__
#define PRINTEMPS_SOLVER_PDLP_CORE_PDLP_CORE_STATE_MANAGER_H__

#include "pdlp_core_state.h"

namespace printemps::solver::pdlp::core {
/*****************************************************************************/
class PDLPCoreStateManager {
   private:
    PDLPCoreState                          m_state;
    linear_programming::LinearProgramming* m_instance_ptr;
    option::Option                         m_option;

   public:
    /*************************************************************************/
    PDLPCoreStateManager(linear_programming::LinearProgramming* a_instance_ptr,
                         const option::Option&                  a_OPTION) {
        this->setup(a_instance_ptr, a_OPTION);
    }

    /*************************************************************************/
    PDLPCoreStateManager(void) {
        this->initialize();
    }

    /*************************************************************************/
    inline void initialize(void) {
        m_state.initialize();
        m_instance_ptr = nullptr;
        m_option.initialize();
    }

    /*************************************************************************/
    inline void setup(linear_programming::LinearProgramming* a_instance_ptr,
                      const option::Option&                  a_OPTION) {
        this->initialize();
        m_instance_ptr = a_instance_ptr;
        m_option       = a_OPTION;

        const int NUMBER_OF_ROWS    = m_instance_ptr->number_of_rows;
        const int NUMBER_OF_COLUMNS = m_instance_ptr->number_of_columns;

        m_state.primal = PrimalDualState(NUMBER_OF_COLUMNS);
        m_state.dual   = PrimalDualState(NUMBER_OF_ROWS);

        m_state.primal.solution = m_instance_ptr->primal_initial_solution;
        m_state.dual.solution   = m_instance_ptr->dual_initial_solution;

        m_state.primal.objective_coefficients_norm =
            m_instance_ptr->primal_objective_coefficients.norm();
        m_state.dual.objective_coefficients_norm =
            m_instance_ptr->dual_objective_coefficients.norm();

        m_state.primal.objective_lower_bound = 0.0;
        m_state.primal.objective_upper_bound = 0.0;
        for (auto i = 0; i < NUMBER_OF_COLUMNS; i++) {
            const auto COEFFICIENT =
                m_instance_ptr->primal_objective_coefficients[i];
            if (COEFFICIENT > 0) {
                m_state.primal.objective_upper_bound +=
                    COEFFICIENT * m_instance_ptr->primal_upper_bounds[i];
                m_state.primal.objective_lower_bound +=
                    COEFFICIENT * m_instance_ptr->primal_lower_bounds[i];
            } else {
                m_state.primal.objective_upper_bound +=
                    COEFFICIENT * m_instance_ptr->primal_lower_bounds[i];
                m_state.primal.objective_lower_bound +=
                    COEFFICIENT * m_instance_ptr->primal_upper_bounds[i];
            }
        }

        m_state.primal.solution_average =
            m_instance_ptr->primal_initial_solution;
        m_state.dual.solution_average = m_instance_ptr->dual_initial_solution;

        m_state.current_outer_loop_normalized_gap =
            std::numeric_limits<double>::max();
        m_state.previous_outer_loop_normalized_gap =
            std::numeric_limits<double>::max();

        m_state.current_inner_loop_normalized_gap =
            std::numeric_limits<double>::max();
        m_state.previous_inner_loop_normalized_gap =
            std::numeric_limits<double>::max();

        m_state.step_size_cumulative_sum = 0.0;

        this->setup_initial_step_size();
        this->setup_initial_primal_weight();
    }

    /*************************************************************************/
    inline void setup_initial_step_size(void) {
        m_state.step_size_current =
            1.0 / m_instance_ptr->primal_constraint_coefficients.norm_infty;
        m_state.step_size_previous = m_state.step_size_current;
    }

    /*************************************************************************/
    inline void setup_initial_primal_weight(void) {
        const double PRIMAL_NORM =
            m_instance_ptr->primal_objective_coefficients.norm();
        const double DUAL_NORM =
            m_instance_ptr->dual_objective_coefficients.norm();

        if (PRIMAL_NORM > constant::EPSILON && DUAL_NORM > constant::EPSILON) {
            m_state.primal_weight =
                m_instance_ptr->primal_objective_coefficients.norm() /
                m_instance_ptr->dual_objective_coefficients.norm();
        } else {
            m_state.primal_weight = 1.0;
        }
    }

    /*************************************************************************/
    inline void setup_new_inner_loop(void) {
        if (m_state.outer_iteration == 0) {
            m_state.primal.solution_average.reset();
            m_state.dual.solution_average.reset();
            m_state.step_size_cumulative_sum = 0.0;
        } else {
            m_state.primal.solution_average  = m_state.primal.solution;
            m_state.dual.solution_average    = m_state.dual.solution;
            m_state.step_size_cumulative_sum = 1.0;
        }

        this->next_outer_iteration();
        this->reset_inner_iteration();

        m_state.primal.solution_baseline = m_state.primal.solution;
        m_state.dual.solution_baseline   = m_state.dual.solution;

        m_state.previous_outer_loop_normalized_gap =
            m_state.current_outer_loop_normalized_gap;
        m_state.current_outer_loop_normalized_gap =
            m_state.current_inner_loop_normalized_gap;

        m_state.current_inner_loop_normalized_gap =
            std::numeric_limits<double>::max();
        m_state.previous_inner_loop_normalized_gap =
            std::numeric_limits<double>::max();
    }

    /*************************************************************************/
    inline double compute_weighted_norm(
        const utility::sparse::Vector& a_PRIMAL,
        const utility::sparse::Vector& a_DUAL) const {
        const double PRIMAL_NORM = a_PRIMAL.norm();
        const double DUAL_NORM   = a_DUAL.norm();

        return sqrt(m_state.primal_weight * PRIMAL_NORM * PRIMAL_NORM +
                    1.0 / m_state.primal_weight * DUAL_NORM * DUAL_NORM);
    }

    /*************************************************************************/
    inline double compute_weighted_norm(
        const utility::sparse::Vector& a_PRIMAL_FIRST,
        const utility::sparse::Vector& a_PRIMAL_SECOND,
        const utility::sparse::Vector& a_DUAL_FIRST,
        const utility::sparse::Vector& a_DUAL_SECOND) const {
        const double PRIMAL_DISTANCE =
            utility::sparse::distance(a_PRIMAL_FIRST, a_PRIMAL_SECOND);
        const double DUAL_DISTANCE =
            utility::sparse::distance(a_DUAL_FIRST, a_DUAL_SECOND);

        return sqrt(m_state.primal_weight * PRIMAL_DISTANCE * PRIMAL_DISTANCE +
                    1.0 / m_state.primal_weight * DUAL_DISTANCE *
                        DUAL_DISTANCE);
    }

    /*************************************************************************/
    inline double compute_normalized_gap(
        const utility::sparse::Vector& a_PRIMAL_CENTER,
        const utility::sparse::Vector& a_DUAL_CENTER, const double a_RADIUS) {
        const auto& INSTANCE = *m_instance_ptr;

        utility::sparse::linear_combination(
            &m_state.primal.lagrangian_coefficients,  //
            -1.0, m_instance_ptr->dual_constraint_coefficients,
            a_DUAL_CENTER,                               //
            1.0, INSTANCE.primal_objective_coefficients  //
        );

        utility::sparse::linear_combination(
            &m_state.dual.lagrangian_coefficients,  //
            -1.0, m_instance_ptr->primal_constraint_coefficients,
            a_PRIMAL_CENTER,                           //
            1.0, INSTANCE.dual_objective_coefficients  //
        );

        for (auto i = 0; i < INSTANCE.number_of_columns; i++) {
            m_state.primal.direction[i] = 0.0;

            if (a_PRIMAL_CENTER[i] >= INSTANCE.primal_upper_bounds[i] &&
                m_state.primal.lagrangian_coefficients[i] <= 0) {
                continue;
            }

            if (a_PRIMAL_CENTER[i] <= INSTANCE.primal_lower_bounds[i] &&
                m_state.primal.lagrangian_coefficients[i] >= 0) {
                continue;
            }

            m_state.primal.direction[i] =
                -m_state.primal.lagrangian_coefficients[i] *
                m_state.primal_weight;
        }

        for (auto i = 0; i < INSTANCE.number_of_rows; i++) {
            m_state.dual.direction[i] = 0.0;

            if (a_DUAL_CENTER[i] >= INSTANCE.dual_upper_bounds[i] &&
                m_state.dual.lagrangian_coefficients[i] <= 0) {
                continue;
            }

            if (a_DUAL_CENTER[i] <= INSTANCE.dual_upper_bounds[i] &&
                m_state.dual.lagrangian_coefficients[i] >= 0) {
                continue;
            }

            m_state.dual.direction[i] =
                m_state.dual.lagrangian_coefficients[i] / m_state.primal_weight;
        }

        const auto DIRECTION_NORM = this->compute_weighted_norm(
            m_state.primal.direction, m_state.dual.direction);

        if (DIRECTION_NORM < constant::EPSILON_10) {
            return 0.0;
        }

        m_state.primal.direction /= DIRECTION_NORM * a_RADIUS;
        m_state.dual.direction /= DIRECTION_NORM * a_RADIUS;

        utility::sparse::sum(&m_state.primal.solution_trial, a_PRIMAL_CENTER,
                             m_state.primal.direction);
        utility::sparse::sum(&m_state.dual.solution_trial, a_DUAL_CENTER,
                             m_state.dual.direction);

        const auto NORMALIZED_GAP =
            -m_instance_ptr->primal_objective_coefficients.dot(
                m_state.primal.direction) +
            m_instance_ptr->dual_objective_coefficients.dot(
                m_state.dual.direction) -
            utility::sparse::dot_dot(
                &m_state.primal.buffer, a_PRIMAL_CENTER,
                m_instance_ptr->dual_constraint_coefficients,
                m_state.dual.solution_trial) +
            utility::sparse::dot_dot(  //
                &m_state.dual.buffer, a_DUAL_CENTER,
                m_instance_ptr->primal_constraint_coefficients,
                m_state.primal.solution_trial);

        return NORMALIZED_GAP / a_RADIUS;
    }

    /*************************************************************************/
    inline void update_solution(void) {
        double      step_size_trial = m_state.step_size_current;
        const auto& INSTANCE        = *m_instance_ptr;

        INSTANCE.dual_constraint_coefficients.dot(  //
            &m_state.primal.lhs, m_state.dual.solution);
        while (true) {
            m_state.number_of_solution_update_attempt++;

            /// Primal
            const double PRIMAL_STEP_SIZE =
                step_size_trial / m_state.primal_weight;

            utility::sparse::linear_combination(                            //
                &m_state.primal.solution_trial,                             //
                1.0, m_state.primal.solution,                               //
                -PRIMAL_STEP_SIZE, INSTANCE.primal_objective_coefficients,  //
                PRIMAL_STEP_SIZE, m_state.primal.lhs                        //
            );

            m_state.primal.solution_trial.clamp(INSTANCE.primal_lower_bounds,
                                                INSTANCE.primal_upper_bounds);

            /// Dual
            const double DUAL_STEP_SIZE =
                step_size_trial * m_state.primal_weight;

            utility::sparse::linear_combination(     //
                &m_state.primal.buffer,              //
                2.0, m_state.primal.solution_trial,  //
                -1.0, m_state.primal.solution        //
            );

            INSTANCE.primal_constraint_coefficients.dot(&m_state.dual.lhs,
                                                        m_state.primal.buffer);

            utility::sparse::linear_combination(                       //
                &m_state.dual.solution_trial,                          //
                1.0, m_state.dual.solution,                            //
                DUAL_STEP_SIZE, INSTANCE.dual_objective_coefficients,  //
                -DUAL_STEP_SIZE, m_state.dual.lhs                      //
            );

            m_state.dual.solution_trial.clamp(INSTANCE.dual_lower_bounds,
                                              INSTANCE.dual_upper_bounds);

            utility::sparse::subtract(  //
                &m_state.primal.move, m_state.primal.solution_trial,
                m_state.primal.solution);

            utility::sparse::subtract(  //
                &m_state.dual.move, m_state.dual.solution_trial,
                m_state.dual.solution);

            const double MOVE = this->compute_weighted_norm(  //
                m_state.primal.move, m_state.dual.move);

            INSTANCE.primal_constraint_coefficients.dot(  //
                &m_state.dual.buffer, m_state.primal.move);

            const double INTERACTION =
                fabs(m_state.dual.move.dot(m_state.dual.buffer));

            const double STEP_SIZE_LIMIT =
                INTERACTION > constant::EPSILON
                    ? 0.5 * MOVE * MOVE / INTERACTION
                    : std::numeric_limits<double>::max();

            const double STEP_SIZE_CANDIDATE = std::min(
                (1.0 - std::pow(m_state.number_of_solution_update_attempt + 1.0,
                                m_option.pdlp.step_size_reduce_exponent)) *
                    STEP_SIZE_LIMIT,
                (1.0 + std::pow(m_state.number_of_solution_update_attempt + 1.0,
                                m_option.pdlp.step_size_extend_exponent)) *
                    step_size_trial);

            if (step_size_trial < STEP_SIZE_LIMIT) {
                m_state.primal.solution    = m_state.primal.solution_trial;
                m_state.dual.solution      = m_state.dual.solution_trial;
                m_state.step_size_previous = step_size_trial;
                m_state.step_size_current  = STEP_SIZE_CANDIDATE;
                return;
            }
            step_size_trial = STEP_SIZE_CANDIDATE;
        }
    }

    /*************************************************************************/
    inline void update_averaged_solution(void) {
        m_state.step_size_cumulative_sum += m_state.step_size_previous;

        const double LEARNING_RATE =
            m_state.step_size_previous / m_state.step_size_cumulative_sum;

        m_state.primal.solution_average.learn(m_state.primal.solution,
                                              LEARNING_RATE);
        m_state.dual.solution_average.learn(m_state.dual.solution,
                                            LEARNING_RATE);
    }

    /*************************************************************************/
    inline void update_restart_information(void) {
        m_state.is_enabled_restart = false;
        if (m_state.inner_iteration <= 1) {
            return;
        }

        if (m_state.total_iteration % m_option.pdlp.restart_check_interval !=
            0) {
            return;
        }

        const double RADIUS_AVERAGE = this->compute_weighted_norm(
            m_state.primal.solution_average, m_state.primal.solution_baseline,
            m_state.dual.solution_average, m_state.dual.solution_baseline);

        const double RADIUS_CURRENT = this->compute_weighted_norm(
            m_state.primal.solution, m_state.primal.solution_baseline,
            m_state.dual.solution, m_state.dual.solution_baseline);

        const double NORMALIZED_GAP_AVERAGE = this->compute_normalized_gap(
            m_state.primal.solution_average, m_state.dual.solution_average,
            RADIUS_AVERAGE);

        const double NORMALIZED_GAP_CURRENT = this->compute_normalized_gap(
            m_state.primal.solution, m_state.dual.solution, RADIUS_CURRENT);

        m_state.previous_inner_loop_normalized_gap =
            m_state.current_inner_loop_normalized_gap;

        if (NORMALIZED_GAP_CURRENT < NORMALIZED_GAP_AVERAGE) {
            m_state.primal.solution_restart_candidate_ptr =
                &m_state.primal.solution;
            m_state.dual.solution_restart_candidate_ptr =
                &m_state.dual.solution;
            m_state.current_inner_loop_normalized_gap = NORMALIZED_GAP_CURRENT;
            m_state.restart_mode                      = RestartMode::Current;
        } else {
            m_state.primal.solution_restart_candidate_ptr =
                &m_state.primal.solution_average;
            m_state.dual.solution_restart_candidate_ptr =
                &m_state.dual.solution_average;
            m_state.current_inner_loop_normalized_gap = NORMALIZED_GAP_AVERAGE;
            m_state.restart_mode                      = RestartMode::Average;
        }

        if (m_state.outer_iteration > 0 &&
            (m_state.current_inner_loop_normalized_gap <
             m_option.pdlp.restart_threshold_sufficient *
                 m_state.previous_outer_loop_normalized_gap)) {
            m_state.is_enabled_restart = true;
            return;
        }

        if (m_state.outer_iteration > 0 &&
            (m_state.current_inner_loop_normalized_gap <
             m_option.pdlp.restart_threshold_necessary *
                 m_state.previous_outer_loop_normalized_gap) &&
            (m_state.current_inner_loop_normalized_gap >
             m_state.previous_inner_loop_normalized_gap)) {
            m_state.is_enabled_restart = true;
            return;
        }

        if (m_state.inner_iteration >
            m_option.pdlp.restart_threshold_artificial *
                m_state.total_iteration) {
            m_state.is_enabled_restart = true;
            return;
        }
    }

    /*************************************************************************/
    inline void skip_restart(void) {
        m_state.is_enabled_restart = false;
    }

    /*************************************************************************/
    inline void update_restart_solution(void) {
        m_state.primal.solution =
            *(m_state.primal.solution_restart_candidate_ptr);
        m_state.dual.solution = *(m_state.dual.solution_restart_candidate_ptr);
    }

    /*************************************************************************/
    inline void update_primal_weight(void) {
        const double PRIMAL_DISTANCE = utility::sparse::distance(
            m_state.primal.solution_average, m_state.primal.solution_baseline);
        const double DUAL_DISTANCE = utility::sparse::distance(
            m_state.dual.solution_average, m_state.dual.solution_baseline);

        if (PRIMAL_DISTANCE > constant::EPSILON &&
            DUAL_DISTANCE > constant::EPSILON &&
            PRIMAL_DISTANCE < 1.0 / constant::EPSILON &&
            DUAL_DISTANCE < 1.0 / constant::EPSILON) {
            m_state.primal_weight =
                exp(0.5 * log((DUAL_DISTANCE / PRIMAL_DISTANCE)) +
                    0.5 * log(m_state.primal_weight));
        }
    }

    /*************************************************************************/
    inline void update_convergence_information(void) {
        this->update_objective();
        this->update_violation();
        this->update_gap();
    }

    /*************************************************************************/
    inline void update_objective(void) {
        utility::sparse::linear_combination(  //
            &m_state.primal.reduced_cost_coefficients, -1.0,
            m_instance_ptr->dual_constraint_coefficients,
            m_state.dual.solution_average, 1.0,
            m_instance_ptr->primal_objective_coefficients);

        for (auto i = 0; i < m_instance_ptr->number_of_columns; i++) {
            if (m_instance_ptr->is_primal_upper_unbounded[i] == 1) {
                m_state.primal.reduced_cost_coefficients[i] =
                    std::max(m_state.primal.reduced_cost_coefficients[i], 0.0);
            }

            if (m_instance_ptr->is_primal_lower_unbounded[i] == 1) {
                m_state.primal.reduced_cost_coefficients[i] =
                    std::min(m_state.primal.reduced_cost_coefficients[i], 0.0);
            }
        }

        double reduced_cost = 0.0;

        for (auto i = 0; i < m_instance_ptr->number_of_columns; i++) {
            if (m_state.primal.reduced_cost_coefficients[i] > 0) {
                reduced_cost += m_state.primal.reduced_cost_coefficients[i] *
                                m_instance_ptr->primal_lower_bounds[i];

            } else {
                reduced_cost += m_state.primal.reduced_cost_coefficients[i] *
                                m_instance_ptr->primal_upper_bounds[i];
            }
        }

        m_state.primal.objective =
            m_state.primal.solution_average.dot(
                m_instance_ptr->primal_objective_coefficients) +
            m_instance_ptr->objective_offset;

        m_state.dual.objective =
            m_state.dual.solution_average.dot(
                m_instance_ptr->dual_objective_coefficients) +
            reduced_cost + m_instance_ptr->objective_offset;
    }

    /*************************************************************************/
    inline void update_violation(void) {
        utility::sparse::linear_combination(
            &m_state.dual.buffer, -1.0,
            m_instance_ptr->primal_constraint_coefficients,
            m_state.primal.solution_average, 1.0,
            m_instance_ptr->dual_objective_coefficients);

        {
            int start = m_instance_ptr->less_constraint_index_range.first;
            int end   = m_instance_ptr->less_constraint_index_range.second;
            for (auto i = start; i < end; i++) {
                /// original less constraints were converted into greater ones.
                m_state.dual.buffer[i] = std::max(m_state.dual.buffer[i], 0.0);
            }
        }

        {
            int start = m_instance_ptr->greater_constraint_index_range.first;
            int end   = m_instance_ptr->greater_constraint_index_range.second;
            for (auto i = start; i < end; i++) {
                m_state.dual.buffer[i] = std::max(m_state.dual.buffer[i], 0.0);
            }
        }

        m_state.primal.absolute_violation_norm = m_state.dual.buffer.norm();
        m_state.primal.relative_violation_norm =
            m_state.primal.absolute_violation_norm /
            (1.0 + m_state.dual.objective_coefficients_norm);

        utility::sparse::linear_combination(
            &m_state.primal.buffer, -1.0,
            m_instance_ptr->dual_constraint_coefficients,
            m_state.dual.solution_average, 1.0,
            m_instance_ptr->primal_objective_coefficients, -1.0,
            m_state.primal.reduced_cost_coefficients);

        m_state.dual.absolute_violation_norm = m_state.primal.buffer.norm();
        m_state.dual.relative_violation_norm =
            m_state.dual.absolute_violation_norm /
            (1.0 + m_state.primal.objective_coefficients_norm);
    }

    /*************************************************************************/
    inline void update_gap(void) {
        m_state.absolute_gap =
            fabs(m_state.primal.objective - m_state.dual.objective);
        m_state.relative_gap =
            m_state.absolute_gap / (1.0 + fabs(m_state.dual.objective) +
                                    fabs(m_state.primal.objective));
    }

    /*************************************************************************/
    inline void set_termination_status(
        const PDLPCoreTerminationStatus a_TERMINATION_STATUS) {
        m_state.termination_status = a_TERMINATION_STATUS;
    }

    /*************************************************************************/
    inline void set_elapsed_time(const double a_ELAPSED_TINE) {
        m_state.elapsed_time = a_ELAPSED_TINE;
    }

    /*************************************************************************/
    inline void reset_inner_iteration(void) {
        m_state.inner_iteration = 0;
    }

    /*************************************************************************/
    inline void reset_iteration(void) {
        m_state.inner_iteration = 0;
        m_state.outer_iteration = 0;
        m_state.total_iteration = 0;
    }

    /*************************************************************************/
    inline void next_inner_iteration(void) {
        m_state.inner_iteration++;
    }

    /*************************************************************************/
    inline void next_outer_iteration(void) {
        m_state.outer_iteration++;
    }

    /*************************************************************************/
    inline void next_total_iteration(void) {
        m_state.total_iteration++;
    }

    /*************************************************************************/
    inline PDLPCoreState& state(void) {
        return m_state;
    }

    /*************************************************************************/
    inline const PDLPCoreState& state(void) const {
        return m_state;
    }
};  // namespace printemps::solver::pdlp::core
}  // namespace printemps::solver::pdlp::core
#endif
/*****************************************************************************/
// END
/*****************************************************************************/