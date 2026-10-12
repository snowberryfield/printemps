/*****************************************************************************/
// Copyright (c) 2020-2026 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_CDCL_CORE_CDCL_CORE_STATE_MANAGER_H__
#define PRINTEMPS_SOLVER_CDCL_CORE_CDCL_CORE_STATE_MANAGER_H__

#include "cdcl_core_constants.h"
#include "cdcl_core_termination_status.h"
#include "cdcl_core_state.h"
#include "cdcl_model.h"
#include "cdcl_variable_heap.h"

namespace printemps::solver::cdcl::core {
/*****************************************************************************/
class CDCLCoreStateManager {
   private:
    CDCLCoreState    m_state;
    CDCLModel        m_model;
    option::Option   m_option;
    CDCLVariableHeap m_heap;

   public:
    /*************************************************************************/
    CDCLCoreStateManager(void) {
        this->initialize();
    }

    /*************************************************************************/
    CDCLCoreStateManager(matrix_model::MatrixModel* a_instance_ptr,
                         const option::Option&      a_OPTION) {
        this->setup(a_instance_ptr, a_OPTION);
    }

    /*************************************************************************/
    inline void initialize(void) {
        m_state.initialize();
        m_model.initialize();
        m_option.initialize();
        m_heap.initialize();
    }

    /*************************************************************************/
    static inline int compute_luby_value(const int a_RESTART_COUNT) noexcept {
        return utility::luby_value(a_RESTART_COUNT);
    }

    /*************************************************************************/
    inline void setup(matrix_model::MatrixModel* a_instance_ptr,
                      const option::Option&      a_OPTION) {
        this->initialize();
        m_option = a_OPTION;

        m_model.setup(a_instance_ptr);
        m_state.activities = m_model.initial_activities();

        const int NUMBER_OF_VARIABLES = m_model.number_of_variables();

        if (m_option.cdcl.initial_activity_noise_ratio > 0.0 ||
            m_option.cdcl.initial_phase_perturbation_rate > 0.0) {
            std::mt19937 random_engine(
                static_cast<unsigned int>(m_option.cdcl.seed));
            std::uniform_real_distribution<double> distribution_real(0.0, 1.0);

            if (m_option.cdcl.initial_activity_noise_ratio > 0.0) {
                for (int i = 0; i < NUMBER_OF_VARIABLES; i++) {
                    m_state.activities[i] +=
                        m_state.activities[i] *
                            m_option.cdcl.initial_activity_noise_ratio *
                            distribution_real(random_engine) +
                        1e-6 * distribution_real(random_engine);
                }
            }

            if (m_option.cdcl.initial_phase_perturbation_rate > 0.0) {
                for (int i = 0; i < NUMBER_OF_VARIABLES; i++) {
                    if (distribution_real(random_engine) <
                        m_option.cdcl.initial_phase_perturbation_rate) {
                        auto& variable = m_model.mutable_variable(i);
                        if (variable.phase != CDCLCoreConstant::NO_PHASE) {
                            variable.phase = 1 - variable.phase;
                        } else {
                            variable.phase =
                                distribution_real(random_engine) < 0.5 ? 1 : 0;
                        }
                    }
                }
            }
        }

        if (m_option.cdcl.restart_mode ==
            option::cdcl_restart_mode::CDCLRestartMode::Luby) {
            m_state.restart_luby_count = 1;
            m_state.conflict_limit     = this->compute_luby_value(1) *
                                     m_option.cdcl.initial_conflict_limit;
        } else if (m_option.cdcl.restart_mode ==
                   option::cdcl_restart_mode::CDCLRestartMode::Geometric) {
            m_state.conflict_limit = m_option.cdcl.initial_conflict_limit;
        }
        m_state.literal_block_distance_queue.setup(
            m_option.cdcl.restart_adaptive_window);

        m_state.seen_marker.assign(NUMBER_OF_VARIABLES, 0);
        m_state.seen_conflict_id = 0;
        m_state.seen_level_marker.assign(NUMBER_OF_VARIABLES + 1, 0);
        m_state.seen_level_id = 0;

        /**
         * Build the indexed max-heap: all variables start unassigned.
         */
        m_heap.setup(NUMBER_OF_VARIABLES, m_state.activities);
    }

    /*************************************************************************/
    inline const CDCLCoreState& state(void) const noexcept {
        return m_state;
    }

    /*************************************************************************/
    inline const CDCLModel& model(void) const noexcept {
        return m_model;
    }

    /*************************************************************************/
    inline CDCLModel& mutable_model(void) noexcept {
        return m_model;
    }

    /*************************************************************************/
    inline const option::Option& option(void) const noexcept {
        return m_option;
    }

    /*************************************************************************/
    inline int current_decision_level(void) const noexcept {
        return m_state.current_decision_level;
    }

    /*************************************************************************/
    inline void set_current_decision_level(const int a_LEVEL) noexcept {
        m_state.current_decision_level = a_LEVEL;
    }

    /*************************************************************************/
    inline void increment_decision_level(void) {
        m_state.trail_level_offsets.push_back(m_state.trail.size());
        m_state.current_decision_level++;
    }

    /*************************************************************************/
    inline void push_trail(const int a_VARIABLE_INDEX) {
        m_state.trail.push_back(a_VARIABLE_INDEX);
    }

    /*************************************************************************/
    inline void resize_trail(const int a_SIZE) {
        m_state.trail.resize(a_SIZE);
    }

    /*************************************************************************/
    inline void resize_trail_level_offsets(const int a_SIZE) {
        m_state.trail_level_offsets.resize(a_SIZE);
    }

    /*************************************************************************/
    inline int trail_level_offset_at(const int a_LEVEL) const {
        return m_state.trail_level_offsets[a_LEVEL];
    }

    /*************************************************************************/
    inline int trail_level_offsets_size(void) const noexcept {
        return static_cast<int>(m_state.trail_level_offsets.size());
    }

    /*************************************************************************/
    inline const std::vector<int>& trail(void) const noexcept {
        return m_state.trail;
    }

    /*************************************************************************/
    inline int propagation_queue_head(void) const noexcept {
        return m_state.propagation_queue_head;
    }

    /*************************************************************************/
    inline int& mutable_propagation_queue_head(void) noexcept {
        return m_state.propagation_queue_head;
    }

    /*************************************************************************/
    inline void set_propagation_queue_head(
        const int a_PROPAGATION_QUEUE_HEAD) noexcept {
        m_state.propagation_queue_head = a_PROPAGATION_QUEUE_HEAD;
    }

    /*************************************************************************/
    inline CDCLConstraintReference conflict_constraint_index(
        void) const noexcept {
        return m_state.conflict_constraint_index;
    }

    /*************************************************************************/
    inline void set_conflict_constraint_index(
        const CDCLConstraintReference& a_REFERENCE) noexcept {
        m_state.conflict_constraint_index = a_REFERENCE;
    }

    /*************************************************************************/
    inline CDCLConstraintReference last_conflict_constraint(
        void) const noexcept {
        return m_state.last_conflict_constraint;
    }

    /*************************************************************************/
    inline void set_last_conflict_constraint(
        const CDCLConstraintReference& a_REFERENCE) noexcept {
        m_state.last_conflict_constraint = a_REFERENCE;
    }

    /*************************************************************************/
    inline int repeated_conflict_constraints_count(void) const noexcept {
        return m_state.repeated_conflict_constraints_count;
    }

    /*************************************************************************/
    inline void increment_repeated_conflict_constraints_count(void) noexcept {
        m_state.repeated_conflict_constraints_count++;
    }

    /*************************************************************************/
    inline double activity_of(const int a_VARIABLE_INDEX) const {
        return m_state.activities[a_VARIABLE_INDEX];
    }

    /*************************************************************************/
    inline const CDCLVariableHeap& heap(void) const noexcept {
        return m_heap;
    }

    /*************************************************************************/
    inline CDCLVariableHeap& mutable_heap(void) noexcept {
        return m_heap;
    }

    /*************************************************************************/
    inline void rescale_activities(void) {
        for (auto& activity : m_state.activities) {
            activity *= 1e-20;
        }
        m_state.variable_activity_increment *= 1e-20;
    }

    /*************************************************************************/
    inline void bump_activity(const int    a_VARIABLE_INDEX,
                              const double a_INCREMENT) {
        m_state.activities[a_VARIABLE_INDEX] += a_INCREMENT;
        if (m_state.activities[a_VARIABLE_INDEX] > 1e20) {
            this->rescale_activities();
        }
        m_heap.update_after_bump(a_VARIABLE_INDEX, m_state.activities);
    }

    /*************************************************************************/
    inline void bump_activity(const int a_VARIABLE_INDEX) {
        this->bump_activity(a_VARIABLE_INDEX,
                            m_state.variable_activity_increment);
    }

    /*************************************************************************/
    inline int pop_max_unassigned_variable(void) {
        while (!m_heap.empty()) {
            const int VARIABLE_INDEX = m_heap.pop(m_state.activities);
            if (!m_model.variable(VARIABLE_INDEX).is_assigned()) {
                return VARIABLE_INDEX;
            }
        }
        return CDCLCoreConstant::NO_VARIABLE;
    }

    /*************************************************************************/
    template <class T_Engine>
    inline int pop_random_unassigned_variable(T_Engine& a_engine) {
        constexpr int MAX_ATTEMPTS = 10;
        int           attempts     = 0;
        while (!m_heap.empty() && attempts < MAX_ATTEMPTS) {
            std::uniform_int_distribution<int> distribution(0,
                                                            m_heap.size() - 1);
            const int POSITION       = distribution(a_engine);
            const int VARIABLE_INDEX = m_heap.heap_array()[POSITION];
            if (!m_model.variable(VARIABLE_INDEX).is_assigned()) {
                m_heap.remove(VARIABLE_INDEX, m_state.activities);
                return VARIABLE_INDEX;
            } else {
                // Lazily purge already-assigned variable
                m_heap.remove(VARIABLE_INDEX, m_state.activities);
            }
            attempts++;
        }
        return CDCLCoreConstant::NO_VARIABLE;
    }

    /*************************************************************************/
    inline void on_variable_assigned(const int a_VALUE) noexcept {
        if (a_VALUE == 1) {
            m_state.number_of_assigned_ones++;
        } else if (a_VALUE == 0) {
            m_state.number_of_assigned_zeros++;
        }
    }

    /*************************************************************************/
    inline void on_variable_unassigned(const int a_VARIABLE_INDEX) noexcept {
        const int VALUE = m_model.variable(a_VARIABLE_INDEX).value;
        if (VALUE == 1) {
            m_state.number_of_assigned_ones--;
        } else if (VALUE == 0) {
            m_state.number_of_assigned_zeros--;
        }
        // Re-insert into heap so this variable can be branched on again.
        m_heap.push(a_VARIABLE_INDEX, m_state.activities);
    }

    /*************************************************************************/
    inline void on_learned_constraint_added(
        const CardinalityConstraint& a_CONSTRAINT) noexcept {
        m_state.last_literal_block_distance =
            a_CONSTRAINT.literal_block_distance;
        m_state.last_learned_length =
            static_cast<int>(a_CONSTRAINT.positive_variable_indices.size() +
                             a_CONSTRAINT.negative_variable_indices.size());
        m_state.total_learned_literal_block_distance +=
            m_state.last_literal_block_distance;
        m_state.total_learned_length += m_state.last_learned_length;
        m_state.total_learned_count++;

        if (m_option.cdcl.restart_mode ==
            option::cdcl_restart_mode::CDCLRestartMode::Adaptive) {
            m_state.literal_block_distance_queue.push(
                a_CONSTRAINT.literal_block_distance);
        }
    }

    /*************************************************************************/
    inline void on_restart(void) noexcept {
        m_state.number_of_restarts++;
        if (m_option.cdcl.restart_mode ==
            option::cdcl_restart_mode::CDCLRestartMode::Luby) {
            m_state.restart_luby_count++;
            m_state.conflict_limit +=
                this->compute_luby_value(m_state.restart_luby_count) *
                m_option.cdcl.initial_conflict_limit;
        } else if (m_option.cdcl.restart_mode ==
                   option::cdcl_restart_mode::CDCLRestartMode::Geometric) {
            m_state.conflict_limit = static_cast<int>(
                m_state.conflict_limit *
                m_option.cdcl.restart_exponential_increase_rate);
        } else if (m_option.cdcl.restart_mode ==
                   option::cdcl_restart_mode::CDCLRestartMode::Adaptive) {
            m_state.literal_block_distance_queue.clear();
        }
    }

    /*************************************************************************/
    inline void on_decision(void) noexcept {
        m_state.number_of_decisions++;
    }

    /*************************************************************************/
    inline void on_propagation(const int a_COUNT = 1) noexcept {
        m_state.number_of_propagations += a_COUNT;
    }

    /*************************************************************************/
    inline int next_seen_conflict_id(void) {
        m_state.seen_conflict_id++;
        if (m_state.seen_conflict_id <= 0) {
            m_state.seen_marker.assign(m_model.number_of_variables(), 0);
            m_state.seen_conflict_id = 1;
        }
        return m_state.seen_conflict_id;
    }

    /*************************************************************************/
    inline bool is_seen(const int a_VARIABLE_INDEX,
                        const int a_CONFLICT_ID) const noexcept {
        return m_state.seen_marker[a_VARIABLE_INDEX] == a_CONFLICT_ID;
    }

    /*************************************************************************/
    inline void mark_seen(const int a_VARIABLE_INDEX,
                          const int a_CONFLICT_ID) noexcept {
        m_state.seen_marker[a_VARIABLE_INDEX] = a_CONFLICT_ID;
    }

    /*************************************************************************/
    inline int next_seen_level_id(void) {
        m_state.seen_level_id++;
        if (m_state.seen_level_id <= 0) {
            m_state.seen_level_marker.assign(m_model.number_of_variables() + 1,
                                             0);
            m_state.seen_level_id = 1;
        }
        return m_state.seen_level_id;
    }

    /*************************************************************************/
    inline bool is_level_seen(const int a_LEVEL,
                              const int a_LEVEL_ID) const noexcept {
        if (a_LEVEL < 0 ||
            a_LEVEL >= static_cast<int>(m_state.seen_level_marker.size())) {
            return false;
        }
        return m_state.seen_level_marker[a_LEVEL] == a_LEVEL_ID;
    }

    /*************************************************************************/
    inline void mark_level_seen(const int a_LEVEL,
                                const int a_LEVEL_ID) noexcept {
        if (a_LEVEL < 0 ||
            a_LEVEL >= static_cast<int>(m_state.seen_level_marker.size())) {
            return;
        }
        m_state.seen_level_marker[a_LEVEL] = a_LEVEL_ID;
    }

    /*************************************************************************/
    inline void decay_activity_increment(void) {
        const double DECAY_RATE = m_option.cdcl.variable_activity_decay_rate;
        if (DECAY_RATE <= 0.0 || DECAY_RATE >= 1.0) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The variable activity decay rate must be in the range (0, "
                "1)."));
        }
        m_state.variable_activity_increment *= (1.0 / DECAY_RATE);
        if (m_state.variable_activity_increment > 1e20) {
            this->rescale_activities();
        }
    }

    /*************************************************************************/
    inline double variable_activity_increment(void) const noexcept {
        return m_state.variable_activity_increment;
    }

    /*************************************************************************/
    inline int number_of_conflicts(void) const noexcept {
        return m_state.number_of_conflicts;
    }

    /*************************************************************************/
    inline void increment_number_of_conflicts(void) noexcept {
        m_state.number_of_conflicts++;
    }

    /*************************************************************************/
    inline int conflict_limit(void) const noexcept {
        return m_state.conflict_limit;
    }

    /*************************************************************************/
    inline void set_conflict_limit(const int a_LIMIT) noexcept {
        m_state.conflict_limit = a_LIMIT;
    }

    /*************************************************************************/
    inline double elapsed_time(void) const noexcept {
        return m_state.elapsed_time;
    }

    /*************************************************************************/
    inline void set_elapsed_time(const double a_ELAPSED_TIME) noexcept {
        m_state.elapsed_time = a_ELAPSED_TIME;
    }

    /*************************************************************************/
    inline int iteration(void) const noexcept {
        return m_state.iteration;
    }

    /*************************************************************************/
    inline void reset_iteration(void) noexcept {
        m_state.iteration = 0;
    }

    /*************************************************************************/
    inline void next_iteration(void) noexcept {
        m_state.iteration++;
    }

    /*************************************************************************/
    inline void set_termination_status(
        const CDCLCoreTerminationStatus a_STATUS) noexcept {
        m_state.termination_status = a_STATUS;
    }

    /*************************************************************************/
    inline CDCLCoreTerminationStatus termination_status(void) const noexcept {
        return m_state.termination_status;
    }

    /*************************************************************************/
    inline int last_literal_block_distance(void) const noexcept {
        return m_state.last_literal_block_distance;
    }

    /*************************************************************************/
    inline int last_learned_length(void) const noexcept {
        return m_state.last_learned_length;
    }

    /*************************************************************************/
    inline int number_of_assigned_zeros(void) const noexcept {
        return m_state.number_of_assigned_zeros;
    }

    /*************************************************************************/
    inline int number_of_assigned_ones(void) const noexcept {
        return m_state.number_of_assigned_ones;
    }

    /*************************************************************************/
    inline int number_of_assigned_variables(void) const noexcept {
        return static_cast<int>(m_state.trail.size());
    }

    /*************************************************************************/
    inline int64_t total_learned_literal_block_distance(void) const noexcept {
        return m_state.total_learned_literal_block_distance;
    }

    /*************************************************************************/
    inline int64_t total_learned_length(void) const noexcept {
        return m_state.total_learned_length;
    }

    /*************************************************************************/
    inline int total_learned_count(void) const noexcept {
        return m_state.total_learned_count;
    }

    /*************************************************************************/
    inline double average_learned_literal_block_distance(void) const noexcept {
        return (m_state.total_learned_count > 0)
                   ? (static_cast<double>(
                          m_state.total_learned_literal_block_distance) /
                      m_state.total_learned_count)
                   : 0.0;
    }

    /*************************************************************************/
    inline double average_learned_length(void) const noexcept {
        return (m_state.total_learned_count > 0)
                   ? (static_cast<double>(m_state.total_learned_length) /
                      m_state.total_learned_count)
                   : 0.0;
    }

    /*************************************************************************/
    inline int number_of_restarts(void) const noexcept {
        return m_state.number_of_restarts;
    }

    /*************************************************************************/
    inline int number_of_decisions(void) const noexcept {
        return m_state.number_of_decisions;
    }

    /*************************************************************************/
    inline int number_of_propagations(void) const noexcept {
        return m_state.number_of_propagations;
    }

    /*************************************************************************/
    inline int restart_luby_count(void) const noexcept {
        return m_state.restart_luby_count;
    }

    /*************************************************************************/
    inline const utility::FixedSizeQueue<int>& literal_block_distance_queue(
        void) const noexcept {
        return m_state.literal_block_distance_queue;
    }

    /*************************************************************************/
    inline void check_and_update_best_solution(void) {
        auto         solution  = m_model.extract_current_solution();
        const double VIOLATION = m_model.compute_total_violation(solution);
        if (VIOLATION < m_state.best_total_violation) {
            m_state.best_total_violation = VIOLATION;
            m_state.best_solution        = std::move(solution);
        }
    }

    /*************************************************************************/
    inline void update_feasible_solution(void) {
        const int NUMBER_OF_VARIABLES = m_model.number_of_variables();
        m_state.solution.resize(NUMBER_OF_VARIABLES);
        for (int i = 0; i < NUMBER_OF_VARIABLES; i++) {
            m_state.solution[i] = m_model.variable(i).value;
        }
        m_state.best_solution        = m_state.solution;
        m_state.best_total_violation = 0.0;
    }

    /*************************************************************************/
    inline const std::vector<int>& solution(void) const noexcept {
        return m_state.solution;
    }

    /*************************************************************************/
    inline const std::vector<int>& best_solution(void) const noexcept {
        return m_state.best_solution;
    }

    /*************************************************************************/
    inline double best_total_violation(void) const noexcept {
        return m_state.best_total_violation;
    }

    /*************************************************************************/
    inline int max_trail_size(void) const noexcept {
        return m_state.max_trail_size;
    }

    /*************************************************************************/
    inline void set_max_trail_size(const int a_MAX_TRAIL_SIZE) noexcept {
        m_state.max_trail_size = a_MAX_TRAIL_SIZE;
    }
};
}  // namespace printemps::solver::cdcl::core

#endif
/*****************************************************************************/
// END
/*****************************************************************************/
