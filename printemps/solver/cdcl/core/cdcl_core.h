/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_CDCL_CORE_CDCL_CORE_H__
#define PRINTEMPS_SOLVER_CDCL_CORE_CDCL_CORE_H__

#include "cdcl_core_constants.h"
#include "cdcl_core_termination_status.h"
#include "cdcl_core_state.h"
#include "cdcl_core_result.h"
#include "cdcl_core_state_manager.h"

#include "cdcl_iteration_event_type.h"

namespace printemps::solver::cdcl::core {
/*****************************************************************************/
class CDCLCore {
   private:
    /*************************************************************************/
    struct FirstUIPAnalysisResult {
        int              unique_implication_point_variable_index;
        std::vector<int> learned_positive_variable_indices;
        std::vector<int> learned_negative_variable_indices;
    };

    /*************************************************************************/
    struct ConflictAnalysisResult {
        int                   backjump_level;
        int                   unique_implication_point_variable_index;
        int                   unique_implication_point_forced_value;
        CardinalityConstraint learned_constraint;
    };

    /*************************************************************************/
    struct ConflictHandlingResult {
        bool                   is_continuable;
        CDCLIterationEventType event_type;
    };

    matrix_model::MatrixModel*           m_instance_ptr;
    std::optional<std::function<bool()>> m_check_interrupt;
    option::Option                       m_option;

    CDCLCoreStateManager m_state_manager;
    CDCLCoreResult       m_result;

    /*************************************************************************/
    inline void assign(const int a_VARIABLE_INDEX, const int a_VALUE,
                       const CDCLConstraintReference& a_REASON) {
        m_state_manager.mutable_model().assign(
            a_VARIABLE_INDEX, a_VALUE, m_state_manager.current_decision_level(),
            a_REASON);
        m_state_manager.push_trail(a_VARIABLE_INDEX);
        m_state_manager.on_variable_assigned(a_VALUE);
    }

    /*************************************************************************/
    inline void backjump(const int a_BACKJUMP_LEVEL) {
        const int BACKTRACK_INDEX =
            (a_BACKJUMP_LEVEL < m_state_manager.trail_level_offsets_size())
                ? m_state_manager.trail_level_offset_at(a_BACKJUMP_LEVEL)
                : static_cast<int>(m_state_manager.trail().size());

        const auto& trail      = m_state_manager.trail();
        const int   TRAIL_SIZE = static_cast<int>(trail.size());
        for (int i = TRAIL_SIZE - 1; i >= BACKTRACK_INDEX; i--) {
            const int VAR_INDEX = trail[i];
            m_state_manager.on_variable_unassigned(VAR_INDEX);
            m_state_manager.mutable_model().rollback(VAR_INDEX);
        }

        m_state_manager.resize_trail(BACKTRACK_INDEX);
        m_state_manager.resize_trail_level_offsets(a_BACKJUMP_LEVEL);
        m_state_manager.set_current_decision_level(a_BACKJUMP_LEVEL);

        m_state_manager.mutable_model().clear_propagation_queue();
    }

    /*************************************************************************/
    inline bool propagate(void) {
        auto assign_callback = [this](const int a_VARIABLE_INDEX,
                                      const int a_VALUE,
                                      const CDCLConstraintReference& a_REASON) {
            this->assign(a_VARIABLE_INDEX, a_VALUE, a_REASON);
            this->m_state_manager.on_propagation();
        };

        const CDCLConstraintReference CONFLICT_REFERENCE =
            m_state_manager.mutable_model().propagate(assign_callback);

        if (CONFLICT_REFERENCE.type != CDCLConstraintType::NONE) {
            m_state_manager.set_conflict_constraint_index(CONFLICT_REFERENCE);
            return false;
        }
        return true;
    }

    /*************************************************************************/
    inline FirstUIPAnalysisResult find_first_unique_implication_point(void) {
        std::vector<int> learned_positive_variable_indices;
        std::vector<int> learned_negative_variable_indices;

        const auto& TRAIL = m_state_manager.trail();
        const auto& MODEL = m_state_manager.model();
        const int   CURRENT_DECISION_LEVEL =
            m_state_manager.current_decision_level();

        int       path_count  = 0;
        const int CONFLICT_ID = m_state_manager.next_seen_conflict_id();

        CDCLConstraintReference current_constraint =
            m_state_manager.conflict_constraint_index();
        int forced_variable_index = CDCLCoreConstant::NO_VARIABLE;

        int trail_index = static_cast<int>(TRAIL.size()) - 1;
        int unique_implication_point_variable_index =
            CDCLCoreConstant::NO_VARIABLE;

        while (true) {
            const auto& POSITIVE_VARIABLE_INDICES =
                MODEL.positive_variable_indices(current_constraint);
            const auto& NEGATIVE_VARIABLE_INDICES =
                MODEL.negative_variable_indices(current_constraint);

            for (int variable_index : POSITIVE_VARIABLE_INDICES) {
                const auto& VARIABLE = MODEL.variable(variable_index);
                if (variable_index != forced_variable_index &&
                    VARIABLE.value == 0) {
                    if (!m_state_manager.is_seen(variable_index, CONFLICT_ID)) {
                        m_state_manager.mark_seen(variable_index, CONFLICT_ID);
                        if (VARIABLE.decision_level == CURRENT_DECISION_LEVEL) {
                            path_count++;
                        } else if (VARIABLE.decision_level > 0) {
                            learned_positive_variable_indices.push_back(
                                variable_index);
                        }
                    }
                    m_state_manager.bump_activity(variable_index);
                }
            }

            for (int variable_index : NEGATIVE_VARIABLE_INDICES) {
                const auto& VARIABLE = MODEL.variable(variable_index);
                if (variable_index != forced_variable_index &&
                    VARIABLE.value == 1) {
                    if (!m_state_manager.is_seen(variable_index, CONFLICT_ID)) {
                        m_state_manager.mark_seen(variable_index, CONFLICT_ID);
                        if (VARIABLE.decision_level == CURRENT_DECISION_LEVEL) {
                            path_count++;
                        } else if (VARIABLE.decision_level > 0) {
                            learned_negative_variable_indices.push_back(
                                variable_index);
                        }
                    }
                    m_state_manager.bump_activity(variable_index);
                }
            }

            if (path_count == 0) {
                break;
            }

            while (trail_index >= 0 &&
                   !m_state_manager.is_seen(TRAIL[trail_index], CONFLICT_ID)) {
                trail_index--;
            }

            if (trail_index < 0) {
                break;
            }

            const int PIVOT_VARIABLE_INDEX = TRAIL[trail_index];
            m_state_manager.mark_seen(PIVOT_VARIABLE_INDEX, 0);
            path_count--;

            if (path_count == 0) {
                unique_implication_point_variable_index = PIVOT_VARIABLE_INDEX;
                break;
            }

            current_constraint =
                MODEL.variable(PIVOT_VARIABLE_INDEX).reason_constraint;
            forced_variable_index = PIVOT_VARIABLE_INDEX;

            if (current_constraint.type == CDCLConstraintType::NONE) {
                unique_implication_point_variable_index = PIVOT_VARIABLE_INDEX;
                break;
            }

            trail_index--;
        }

        if (unique_implication_point_variable_index ==
            CDCLCoreConstant::NO_VARIABLE) {
            /**
             * Fallback: take the last variable on the trail.
             * This can only be reached when the loop above exhausts the trail
             * without finding a proper 1-UIP.  Guard against an empty trail
             * to avoid UB from TRAIL.back() on an empty vector.
             */
            if (!TRAIL.empty()) {
                unique_implication_point_variable_index = TRAIL.back();
            } else {
                // Trail is empty — no variable to use as UIP; return early.
                return {CDCLCoreConstant::NO_VARIABLE, {}, {}};
            }
        }

        const int UNIQUE_IMPLICATION_POINT_CURRENT_VALUE =
            MODEL.variable(unique_implication_point_variable_index).value;
        if (UNIQUE_IMPLICATION_POINT_CURRENT_VALUE == 0) {
            learned_positive_variable_indices.push_back(
                unique_implication_point_variable_index);
        } else {
            learned_negative_variable_indices.push_back(
                unique_implication_point_variable_index);
        }

        return {unique_implication_point_variable_index,
                std::move(learned_positive_variable_indices),
                std::move(learned_negative_variable_indices)};
    }

    /*************************************************************************/
    inline int compute_backjump_level(
        const std::vector<int>& a_LEARNED_POSITIVE_VARIABLE_INDICES,
        const std::vector<int>& a_LEARNED_NEGATIVE_VARIABLE_INDICES,
        const int a_UNIQUE_IMPLICATION_POINT_VARIABLE_INDEX) const {
        const auto& model = m_state_manager.model();

        int backjump_level = 0;

        for (const int variable_index : a_LEARNED_POSITIVE_VARIABLE_INDICES) {
            if (variable_index != a_UNIQUE_IMPLICATION_POINT_VARIABLE_INDEX) {
                backjump_level =
                    std::max(backjump_level,
                             model.variable(variable_index).decision_level);
            }
        }
        for (const int variable_index : a_LEARNED_NEGATIVE_VARIABLE_INDICES) {
            if (variable_index != a_UNIQUE_IMPLICATION_POINT_VARIABLE_INDEX) {
                backjump_level =
                    std::max(backjump_level,
                             model.variable(variable_index).decision_level);
            }
        }

        return backjump_level;
    }

    /*************************************************************************/
    inline int compute_literal_block_distance(
        const std::vector<int>& a_LEARNED_POSITIVE_VARIABLE_INDICES,
        const std::vector<int>& a_LEARNED_NEGATIVE_VARIABLE_INDICES) {
        const auto& model    = m_state_manager.model();
        const int   LEVEL_ID = m_state_manager.next_seen_level_id();

        int literal_block_distance = 0;

        for (const int variable_index : a_LEARNED_POSITIVE_VARIABLE_INDICES) {
            const int LEVEL = model.variable(variable_index).decision_level;
            if (!m_state_manager.is_level_seen(LEVEL, LEVEL_ID)) {
                m_state_manager.mark_level_seen(LEVEL, LEVEL_ID);
                literal_block_distance++;
            }
        }
        for (const int variable_index : a_LEARNED_NEGATIVE_VARIABLE_INDICES) {
            const int LEVEL = model.variable(variable_index).decision_level;
            if (!m_state_manager.is_level_seen(LEVEL, LEVEL_ID)) {
                m_state_manager.mark_level_seen(LEVEL, LEVEL_ID);
                literal_block_distance++;
            }
        }

        return literal_block_distance;
    }

    /*************************************************************************/
    inline ConflictAnalysisResult analyze_conflict(void) {
        const auto [UNIQUE_IMPLICATION_POINT_VARIABLE_INDEX,
                    LEARNED_POSITIVE_VARIABLE_INDICES,
                    LEARNED_NEGATIVE_VARIABLE_INDICES] =
            this->find_first_unique_implication_point();

        /**
         * If the trail was empty when find_first_UIP ran, it returns
         * NO_VARIABLE.  This is a degenerate state that signals a level-0
         * conflict (the problem is infeasible). Return a sentinel result with
         * backjump_level = -1 so that handle_conflict can detect it.
         */
        if (UNIQUE_IMPLICATION_POINT_VARIABLE_INDEX ==
            CDCLCoreConstant::NO_VARIABLE) {
            return {CDCLCoreConstant::NO_VARIABLE,
                    CDCLCoreConstant::NO_VARIABLE, CDCLCoreConstant::NO_VALUE,
                    CardinalityConstraint{}};
        }

        const int BACKJUMP_LEVEL = this->compute_backjump_level(
            LEARNED_POSITIVE_VARIABLE_INDICES,
            LEARNED_NEGATIVE_VARIABLE_INDICES,
            UNIQUE_IMPLICATION_POINT_VARIABLE_INDEX);

        const int LITERAL_BLOCK_DISTANCE = this->compute_literal_block_distance(
            LEARNED_POSITIVE_VARIABLE_INDICES,
            LEARNED_NEGATIVE_VARIABLE_INDICES);

        const int UNIQUE_IMPLICATION_POINT_CURRENT_VALUE =
            m_state_manager.model()
                .variable(UNIQUE_IMPLICATION_POINT_VARIABLE_INDEX)
                .value;
        const int UNIQUE_IMPLICATION_POINT_FORCED_VALUE =
            (UNIQUE_IMPLICATION_POINT_CURRENT_VALUE == 0) ? 1 : 0;

        CardinalityConstraint learned_constraint(
            CDCLCoreConstant::NO_CONSTRAINT, 1,
            std::move(LEARNED_POSITIVE_VARIABLE_INDICES),
            std::move(LEARNED_NEGATIVE_VARIABLE_INDICES));
        learned_constraint.literal_block_distance = LITERAL_BLOCK_DISTANCE;

        return {BACKJUMP_LEVEL, UNIQUE_IMPLICATION_POINT_VARIABLE_INDEX,
                UNIQUE_IMPLICATION_POINT_FORCED_VALUE,
                std::move(learned_constraint)};
    }

    /*************************************************************************/
    inline int find_branching_variable_index(void) {
        return m_state_manager.pop_max_unassigned_variable();
    }

    /*************************************************************************/
    inline bool branch(void) {
        const int BEST_VARIABLE_INDEX = this->find_branching_variable_index();
        if (BEST_VARIABLE_INDEX == CDCLCoreConstant::NO_VARIABLE) {
            return false;
        }

        m_state_manager.on_decision();
        m_state_manager.increment_decision_level();

        /**
         * NOTE: Reuse the previously saved value if available (Phase Saving).
         * Typically, all variables have their initial phases pre-assigned
         * by CDCLModel::setup_initial_phases(). NO_PHASE only occurs
         * in exceptional uninitialized cases, where 0 is used as fallback.
         */
        const auto& MODEL        = m_state_manager.model();
        const int   BRANCH_VALUE = (m_option.cdcl.is_enabled_phase_saving &&
                                  MODEL.variable(BEST_VARIABLE_INDEX).phase !=
                                      CDCLCoreConstant::NO_PHASE)
                                       ? MODEL.variable(BEST_VARIABLE_INDEX).phase
                                       : 0;

        this->assign(BEST_VARIABLE_INDEX, BRANCH_VALUE,
                     CDCLConstraintReference{CDCLConstraintType::NONE,
                                             CDCLCoreConstant::DECISION});

        return true;
    }

    /*************************************************************************/
    inline bool should_restart(void) const {
        if (!m_option.cdcl.is_enabled_restart) {
            return false;
        }

        switch (m_option.cdcl.restart_mode) {
            case option::cdcl_restart_mode::CDCLRestartMode::Geometric:
            case option::cdcl_restart_mode::CDCLRestartMode::Luby:
                return m_state_manager.number_of_conflicts() >=
                       m_state_manager.conflict_limit();

            case option::cdcl_restart_mode::CDCLRestartMode::Adaptive: {
                const auto& queue =
                    m_state_manager.literal_block_distance_queue();
                if (queue.size() < queue.max_size() || queue.max_size() == 0) {
                    return false;
                }
                if (m_state_manager.total_learned_count() <= 0) {
                    return false;
                }
                const double LOCAL_AVG = queue.average();
                const double GLOBAL_AVG =
                    m_state_manager.average_learned_literal_block_distance();
                return LOCAL_AVG >=
                       GLOBAL_AVG *
                           m_option.cdcl
                               .restart_adaptive_literal_block_distance_ratio_threshold;
            }
        }
        return false;
    }

    /*************************************************************************/
    inline bool should_reduce_learned_constraints(void) const {
        return m_option.cdcl.is_enabled_learned_constraint_reduction &&
               m_state_manager.model().number_of_learned_constraints() >
                   m_option.cdcl.learned_constraint_capacity;
    }

    /*************************************************************************/
    inline void restart(void) {
        m_state_manager.on_restart();
        this->backjump(0);
    }

    /*************************************************************************/
    inline ConflictHandlingResult handle_conflict(void) {
        if (m_state_manager.current_decision_level() == 0) {
            m_state_manager.set_termination_status(
                CDCLCoreTerminationStatus::INFEASIBLE);
            return {false, CDCLIterationEventType::None};
        }

        auto [BACKJUMP_LEVEL, UNIQUE_IMPLICATION_POINT_VARIABLE_INDEX,
              UNIQUE_IMPLICATION_POINT_FORCED_VALUE, LEARNED_CONSTRAINT] =
            this->analyze_conflict();

        /**
         * analyze_conflict returns BACKJUMP_LEVEL = NO_VARIABLE (= -1) when
         * the trail was empty (degenerate level-0 conflict detected inside
         * find_first_unique_implication_point).  Treat this as INFEASIBLE.
         */
        if (BACKJUMP_LEVEL == CDCLCoreConstant::NO_VARIABLE) {
            m_state_manager.set_termination_status(
                CDCLCoreTerminationStatus::INFEASIBLE);
            return {false, CDCLIterationEventType::None};
        }

        m_state_manager.on_learned_constraint_added(LEARNED_CONSTRAINT);

        m_state_manager.decay_activity_increment();
        m_state_manager.increment_number_of_conflicts();

        this->backjump(BACKJUMP_LEVEL);

        CDCLConstraintReference new_constraint_reference =
            m_state_manager.mutable_model().add_learned_constraint(
                std::move(LEARNED_CONSTRAINT));

        if (UNIQUE_IMPLICATION_POINT_VARIABLE_INDEX !=
                CDCLCoreConstant::NO_VARIABLE &&
            !m_state_manager.model()
                 .variable(UNIQUE_IMPLICATION_POINT_VARIABLE_INDEX)
                 .is_assigned()) {
            this->assign(UNIQUE_IMPLICATION_POINT_VARIABLE_INDEX,
                         UNIQUE_IMPLICATION_POINT_FORCED_VALUE,
                         new_constraint_reference);
        }

        if (this->should_restart()) {
            this->restart();
            if (this->should_reduce_learned_constraints()) {
                [[maybe_unused]] const int PURGED_COUNT =
                    m_state_manager.mutable_model().reduce_learned_constraints(
                        m_option.cdcl.learned_constraint_reduction_rate,
                        m_option.cdcl
                            .literal_block_distance_protection_threshold);
            }
            return {true, CDCLIterationEventType::Restart};
        }

        return {true, CDCLIterationEventType::Backjump};
    }

    /*************************************************************************/
    inline bool handle_decision(void) {
        const bool HAS_UNASSIGNED = this->branch();

        if (HAS_UNASSIGNED) {
            return true;
        }

        m_state_manager.set_termination_status(
            CDCLCoreTerminationStatus::FEASIBLE);

        return false;
    }

    /*************************************************************************/
    inline bool check_interrupt(void) const {
        return m_check_interrupt.has_value() && m_check_interrupt.value()();
    }

    /*************************************************************************/
    inline bool satisfy_interrupted_terminate_condition(void) {
        if (this->check_interrupt()) {
            m_state_manager.set_termination_status(
                CDCLCoreTerminationStatus::INTERRUPTION);
            return true;
        }
        return false;
    }

    /*************************************************************************/
    inline bool satisfy_time_over_terminate_condition(void) {
        if (m_state_manager.elapsed_time() > m_option.cdcl.time_max) {
            m_state_manager.set_termination_status(
                CDCLCoreTerminationStatus::TIME_OVER);
            return true;
        }
        if (m_option.general.time_max >= 0 &&
            m_state_manager.elapsed_time() + m_option.cdcl.time_offset >
                m_option.general.time_max) {
            m_state_manager.set_termination_status(
                CDCLCoreTerminationStatus::TIME_OVER);
            return true;
        }
        return false;
    }

    /*************************************************************************/
    inline bool satisfy_iteration_over_terminate_condition(void) {
        if (m_state_manager.iteration() >= m_option.cdcl.iteration_max) {
            m_state_manager.set_termination_status(
                CDCLCoreTerminationStatus::ITERATION_OVER);
            return true;
        }
        return false;
    }

    /*************************************************************************/
    inline bool satisfy_conflict_over_terminate_condition(void) {
        if (m_option.cdcl.conflict_max >= 0 &&
            m_state_manager.number_of_conflicts() >=
                m_option.cdcl.conflict_max) {
            m_state_manager.set_termination_status(
                CDCLCoreTerminationStatus::CONFLICT_OVER);
            return true;
        }
        return false;
    }

    /*************************************************************************/
    inline void print_table_header(const bool a_IS_ENABLED_PRINT) const {
        if (!a_IS_ENABLED_PRINT) {
            return;
        }

        utility::print(
            "---------+-------+---------------------------+-------+----------"
            "----------------",
            true);
        utility::print(
            "Iteration| Level | Assigned Variables (1s/%) | Confl.| "
            "Learned (Avg. Len/LBD) ",
            true);
        utility::print(
            "---------+-------+---------------------------+-------+----------"
            "----------------",
            true);
    }

    /*************************************************************************/
    inline void print_table_initial(const bool a_IS_ENABLED_PRINT) const {
        if (!a_IS_ENABLED_PRINT) {
            return;
        }

        this->print_table_body(CDCLIterationEventType::None,
                               a_IS_ENABLED_PRINT);
    }

    /*************************************************************************/
    inline void print_table_body(const CDCLIterationEventType a_EVENT_TYPE,
                                 const bool a_IS_ENABLED_PRINT) const {
        if (!a_IS_ENABLED_PRINT) {
            return;
        }

        const int ASSIGNED_VARIABLES =
            m_state_manager.number_of_assigned_variables();
        const int ASSIGNED_ONES = m_state_manager.number_of_assigned_ones();
        const int TOTAL_VARIABLES =
            m_state_manager.model().number_of_variables();
        const double ASSIGNED_PERCENTAGE =
            (TOTAL_VARIABLES > 0)
                ? (ASSIGNED_VARIABLES * 100.0 / TOTAL_VARIABLES)
                : 0.0;

        char suffix = ' ';
        if (a_EVENT_TYPE == CDCLIterationEventType::Restart) {
            suffix = 'r';
        } else if (a_EVENT_TYPE == CDCLIterationEventType::Backjump) {
            suffix = 'b';
        }

        std::string color_feasible_begin = "";
        std::string color_feasible_end   = "";

#ifdef _PRINTEMPS_STYLING
        if (TOTAL_VARIABLES > 0 && ASSIGNED_VARIABLES == TOTAL_VARIABLES) {
            color_feasible_begin = constant::CYAN;
            color_feasible_end   = constant::NO_COLOR;
        }
#endif

        std::string avg_len_str =
            (m_state_manager.total_learned_count() > 0)
                ? utility::to_string(m_state_manager.average_learned_length(),
                                     "%.2e")
                : "       -";
        std::string avg_lbd_str =
            (m_state_manager.total_learned_count() > 0)
                ? utility::to_string(
                      m_state_manager.average_learned_literal_block_distance(),
                      "%.2e")
                : "       -";

        std::printf(
            "%s%c| %s |%8d (%8d/%s%6.2f%%%s)| %s | %s (%s/%s)\n",
            utility::int_format(m_state_manager.iteration(), 8).c_str(),  //
            suffix,                                                       //
            utility::int_format(m_state_manager.current_decision_level())
                .c_str(),                                                   //
            ASSIGNED_VARIABLES,                                             //
            ASSIGNED_ONES,                                                  //
            color_feasible_begin.c_str(),                                   //
            ASSIGNED_PERCENTAGE,                                            //
            color_feasible_end.c_str(),                                     //
            utility::int_format(m_state_manager.number_of_conflicts())      //
                .c_str(),                                                   //
            utility::int_format(                                            //
                m_state_manager.model().number_of_learned_constraints())    //
                .c_str(),                                                   //
            avg_len_str.c_str(),                                            //
            avg_lbd_str.c_str());
    }

    /*************************************************************************/
    inline void print_table_footer(const bool a_IS_ENABLED_PRINT) const {
        if (!a_IS_ENABLED_PRINT) {
            return;
        }

        utility::print(
            "---------+-------+---------------------------+-------+----------"
            "----------------",
            true);
        utility::print_info(  //
            " -- r: Search was restarted from decision level 0.", true);
        utility::print_info(  //
            " -- b: Non-chronological backjump was performed by conflict "
            "analysis.",
            true);
        utility::print_single_line(true);
    }

    /*************************************************************************/
    inline void postprocess(void) {
        m_result =
            CDCLCoreResult(m_state_manager.state(), m_state_manager.model());
    }

   public:
    /*************************************************************************/
    CDCLCore(void) {
        this->initialize();
    }

    /*************************************************************************/
    CDCLCore(matrix_model::MatrixModel*                  a_instance_ptr,
             const std::optional<std::function<bool()>>& a_CHECK_INTERRUPT,
             const option::Option&                       a_OPTION) {
        this->setup(a_instance_ptr, a_CHECK_INTERRUPT, a_OPTION);
    }

    /*************************************************************************/
    inline void initialize(void) {
        m_instance_ptr    = nullptr;
        m_check_interrupt = std::nullopt;
        m_option.initialize();
        m_state_manager.initialize();
        m_result.initialize();
    }

    /*************************************************************************/
    inline void setup(
        matrix_model::MatrixModel*                  a_instance_ptr,
        const std::optional<std::function<bool()>>& a_CHECK_INTERRUPT,
        const option::Option&                       a_OPTION) {
        this->initialize();
        m_instance_ptr    = a_instance_ptr;
        m_check_interrupt = a_CHECK_INTERRUPT;
        m_option          = a_OPTION;
        m_state_manager.setup(m_instance_ptr, m_option);
    }

   public:
    /*************************************************************************/
    void run(void) {
        utility::TimeKeeper time_keeper;
        time_keeper.set_start_time();
        m_state_manager.reset_iteration();

        const bool IS_ENABLED_PRINT_TABLE =
            m_option.output.verbose >= option::verbose::Inner;

        this->print_table_header(IS_ENABLED_PRINT_TABLE);
        this->print_table_initial(IS_ENABLED_PRINT_TABLE);

        while (true) {
            m_state_manager.set_elapsed_time(time_keeper.clock());

            /**
             * Terminate the loop if interrupted
             */
            if (this->satisfy_interrupted_terminate_condition()) {
                break;
            }

            /**
             * Terminate the loop if the time is over.
             */
            if (this->satisfy_time_over_terminate_condition()) {
                break;
            }

            /**
             * Terminate the loop if the iteration is over.
             */
            if (this->satisfy_iteration_over_terminate_condition()) {
                break;
            }

            /**
             * Terminate the loop if the conflict limit is reached.
             */
            if (this->satisfy_conflict_over_terminate_condition()) {
                break;
            }

            CDCLIterationEventType event_type = CDCLIterationEventType::None;

            if (!this->propagate()) {
                const auto [IS_CONTINUABLE, CONFLICT_EVENT_TYPE] =
                    this->handle_conflict();
                if (!IS_CONTINUABLE) {
                    break;
                }
                event_type = CONFLICT_EVENT_TYPE;
            } else {
                if (!this->handle_decision()) {
                    break;
                }
            }

            m_state_manager.next_iteration();

            const bool IS_LOG_ITERATION =
                (m_option.cdcl.log_interval > 0 &&
                 m_state_manager.iteration() % m_option.cdcl.log_interval == 0);

            if (event_type == CDCLIterationEventType::Restart ||
                IS_LOG_ITERATION) {
                this->print_table_body(event_type, IS_ENABLED_PRINT_TABLE);
            }
        }

        this->print_table_body(CDCLIterationEventType::None,
                               IS_ENABLED_PRINT_TABLE);
        this->print_table_footer(IS_ENABLED_PRINT_TABLE);

        m_state_manager.set_elapsed_time(time_keeper.clock());
        this->postprocess();
    }

    /*************************************************************************/
    inline const CDCLCoreResult& result(void) const {
        return m_result;
    }
};

}  // namespace printemps::solver::cdcl::core

#endif
/*****************************************************************************/
// END
/*****************************************************************************/
