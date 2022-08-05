/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_TABU_SEARCH_CONTROLLER_TABU_SEARCH_CONTROLLER_STATE_MANAGER_H__
#define PRINTEMPS_SOLVER_TABU_SEARCH_CONTROLLER_TABU_SEARCH_CONTROLLER_STATE_MANAGER_H__

namespace printemps::solver::tabu_search::controller {
/*****************************************************************************/
struct TabuSearchControllerStateManagerConstant {
    static constexpr double RELATIVE_RANGE_THRESHOLD              = 1E-2;
    static constexpr double PENALTY_COEFFICIENT_RELAXING_RATE_MIN = 0.3;
    static constexpr double PENALTY_COEFFICIENT_RELAXING_RATE_MAX = 1.0 - 1E-4;
    static constexpr double PENALTY_COEFFICIENT_RELAXING_RATE_DECREASE_RATE =
        0.9;
    static constexpr double PENALTY_COEFFICIENT_RELAXING_RATE_STEP_SIZE = 1E-1;
    static constexpr int    ITERATION_AFTER_RELAXATION_MAX              = 30;
    static constexpr double GAP_TOLERANCE        = constant::EPSILON;
    static constexpr int    STAGNATION_THRESHOLD = 80;
};

/*****************************************************************************/
template <class T_Variable, class T_Expression>
class TabuSearchControllerStateManager {
   private:
    TabuSearchControllerState<T_Variable, T_Expression>  m_state;
    model::Model<T_Variable, T_Expression>*              m_model_ptr;
    solution::IncumbentHolder<T_Variable, T_Expression>* m_incumbent_holder_ptr;
    Memory<T_Variable, T_Expression>*                    m_memory_ptr;
    option::Option                                       m_option;

   public:
    /*************************************************************************/
    TabuSearchControllerStateManager(
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
    TabuSearchControllerStateManager(void) {
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
         * Initialize the primal and dual intensities.
         */
        m_state.current_primal_intensity  = m_memory_ptr->primal_intensity();
        m_state.previous_primal_intensity = 0.0;

        m_state.current_dual_intensity  = m_memory_ptr->dual_intensity();
        m_state.previous_dual_intensity = 0.0;

        /**
         * Initialize the option values.
         */
        m_state.initial_tabu_tenure = m_option.tabu_search.initial_tabu_tenure;
        m_state.pruning_rate_threshold =
            m_option.tabu_search.pruning_rate_threshold;
        m_state.number_of_initial_modification = 0;
        m_state.iteration_max = m_option.tabu_search.iteration_max;
        m_state.penalty_coefficient_relaxing_rate =
            m_option.penalty_coefficient_relaxing_rate;
        m_state.penalty_coefficient_tightening_rate =
            m_option.penalty_coefficient_tightening_rate;

        m_state.improvability_screening_mode =
            m_option.improvability_screening_mode;
        if (m_state.improvability_screening_mode ==
            option::improvability_screening_mode::Automatic) {
            m_state.improvability_screening_mode =
                option::improvability_screening_mode::Intensive;
        }
    }

    /*************************************************************************/
    inline option::Option create_option() const {
        option::Option option = m_option;

        option.improvability_screening_mode =
            m_state.improvability_screening_mode;
        option.tabu_search.iteration_max = m_state.iteration_max;
        option.tabu_search.time_offset   = m_state.total_elapsed_time;
        option.tabu_search.seed          = m_state.iteration;
        option.tabu_search.number_of_initial_modification =
            m_state.number_of_initial_modification;
        option.tabu_search.initial_tabu_tenure = m_state.initial_tabu_tenure;
        option.tabu_search.pruning_rate_threshold =
            m_state.pruning_rate_threshold;
        return option;
    }

    /*************************************************************************/
    inline constexpr void update(const tabu_search::core::TabuSearchCoreResult<
                                     T_Variable, T_Expression>& a_RESULT,
                                 std::mt19937*                  a_mt19937_ptr) {
        /**
         * Update the last tabu search update status.
         */
        this->update_last_tabu_search_result(a_RESULT);

        /**
         * Update the intensity.
         */
        this->update_intensity();

        /**
         * Keep the previous solution.
         */
        this->keep_previous_solution();

        /**
         * Update the status of infeasible stagnation.
         */
        this->update_is_infeasible_stagnation();

        /**
         * Update the status of improvement in the last tabu search.
         */
        this->update_is_improved();

        /**
         * Turn off the flags for parameter control.
         */
        this->turn_flags_off();

        /**
         * If the improvability_screening_mode was set to "Automatic",
         * determine the mode according to the search status so far.
         */
        if (m_option.improvability_screening_mode ==
            option::improvability_screening_mode::Automatic) {
            this->update_improvability_screening_mode();
        }

        /**
         * Determine the initial solution for the next loop and whether penalty
         * coefficients are to be relaxed or tightened.
         */
        this->update_initial_solution_and_penalty_coefficient_flags();

        /**
         * Additional processes for cases when the penalty coefficients are
         * relaxed.
         */
        if (m_state.is_enabled_penalty_coefficient_relaxing) {
            this->update_penalty_coefficient_relaxing_rate();
        }

        /**
         * Additional processes for cases when the penalty coefficients are
         * tightened: Reset penalty coefficients if stagnation is detected.
         */
        if (m_state.is_enabled_penalty_coefficient_tightening) {
            this->update_penalty_coefficient_reset_flag();
        }

        /**
         * Update the local penalty coefficients.
         */
        if (m_state.penalty_coefficient_reset_flag) {
            this->reset_local_penalty_coefficient();
        } else if (m_state.is_enabled_penalty_coefficient_tightening) {
            this->tighten_local_penalty_coefficient();
        } else if (m_state.is_enabled_penalty_coefficient_relaxing) {
            this->relax_local_penalty_coefficient();
        }

        /**
         * Update the initial tabu tenure for the next loop.
         */
        if (m_option.tabu_search.is_enabled_automatic_tabu_tenure_adjustment) {
            this->update_initial_tabu_tenure();
        }

        /**
         * Update the number of initial modification for the next loop.
         */
        if (m_option.tabu_search.is_enabled_initial_modification) {
            this->update_number_of_initial_modification(a_mt19937_ptr);
        }

        /**
         * Update the maximum number of iterations and pruning rate threshold
         * for the next loop.
         */
        if (m_option.tabu_search.is_enabled_automatic_iteration_adjustment) {
            this->update_iteration_max();
            this->update_pruning_rate_threshold();
        }

        /**
         * Update the neighborhood moves to be employed for the next loop.
         */
        if (m_state.is_global_augmented_incumbent_updated) {
            /**
             * Disable the special neighborhood moves if the incumbent was
             * updated.
             */
            this->disable_special_neighborhood_moves();

        } else {
            /**
             * Enable the special neighborhood moves if the incumbent was
             * not updated.
             */
            if (m_state.tabu_search_result.number_of_iterations ==
                m_option.tabu_search.iteration_max) {
                this->enable_special_neighborhood_moves();
            }
        }

        /**
         * Reset chain moves if the global augmented objective was updated.
         */
        if (m_state.is_global_augmented_incumbent_updated) {
            if (m_option.is_enabled_chain_move) {
                this->clear_chain_moves();
            }
        }

        /**
         * Sort and deduplicate registered chain moves.
         */
        if (m_model_ptr->neighborhood().chain().is_enabled() &&
            m_option.chain_move_capacity > 0) {
            this->sort_and_deduplicate_chain_moves();
        }

        /**
         * Reduce the registered chain moves.
         */
        if (static_cast<int>(
                this->m_model_ptr->neighborhood().chain().moves().size()) >
            m_option.chain_move_capacity) {
            this->reduce_chain_moves(a_mt19937_ptr);
        }

        /**
         * Update the current solution as the initial solution in the next tabu
         * search loop.
         */
        this->update_current_solution();

        /**
         * Update the various counts about penalty coefficients relaxation.
         */
        this->update_relaxation_status();
    }

    /*************************************************************************/
    inline constexpr void update_last_tabu_search_result(
        const tabu_search::core::TabuSearchCoreResult<T_Variable, T_Expression>&
            a_RESULT) {
        /**
         * Update the incumbent update flags.
         */
        m_state.tabu_search_result = a_RESULT;
        m_state.is_global_augmented_incumbent_updated =
            (m_state.tabu_search_result.total_update_status &
             solution::IncumbentHolderConstant::
                 STATUS_GLOBAL_AUGMENTED_INCUMBENT_UPDATE);

        m_state.previous_is_feasible_incumbent_updated =
            m_state.current_is_feasible_incumbent_updated;
        m_state.current_is_feasible_incumbent_updated =
            (m_state.tabu_search_result.total_update_status &
             solution::IncumbentHolderConstant::
                 STATUS_FEASIBLE_INCUMBENT_UPDATE);

        m_state.is_not_updated =
            (m_state.tabu_search_result.total_update_status ==
             solution::IncumbentHolderConstant::STATUS_NOT_UPDATED);

        /**
         * Update the iteration after global augmented incumbent update.
         */
        if (m_state.is_global_augmented_incumbent_updated) {
            m_state.iteration_after_global_augmented_incumbent_update = 0;
        } else {
            m_state.iteration_after_global_augmented_incumbent_update++;
        }

        /**
         * Update the consecutive iteration with no update.
         */
        if (m_state.is_not_updated) {
            m_state.iteration_after_no_update++;
        } else {
            m_state.iteration_after_no_update = 0;
        }

        /**
         * Update the total inner number of iterations and evaluated moves.
         */
        m_state.total_number_of_inner_iterations +=
            a_RESULT.number_of_iterations;
        m_state.total_number_of_evaluated_moves +=
            a_RESULT.number_of_evaluated_moves;

        m_state.averaged_inner_iteration_speed =
            m_state.total_number_of_inner_iterations /
            m_state.tabu_search_elapsed_time;

        m_state.averaged_move_evaluation_speed =
            m_state.total_number_of_evaluated_moves /
            m_state.tabu_search_elapsed_time;

        /**
         * Update the total update status.
         */
        m_state.total_update_status |= a_RESULT.total_update_status;
    }

    /*************************************************************************/
    inline constexpr void update_intensity(void) {
        m_state.previous_primal_intensity = m_state.current_primal_intensity;
        m_state.current_primal_intensity  = m_memory_ptr->primal_intensity();

        m_state.previous_dual_intensity = m_state.current_dual_intensity;
        m_state.current_dual_intensity  = m_memory_ptr->dual_intensity();
    }

    /*************************************************************************/
    inline constexpr void keep_previous_solution(void) {
        m_state.previous_solution       = m_state.current_solution;
        m_state.previous_solution_score = m_state.current_solution_score;
    }

    /*************************************************************************/
    inline constexpr void update_is_infeasible_stagnation(void) {
        /**
         * "Stagnation" refers to when the iteration after global augmented
         * incumbent update is not less than the constant STAGNATION_THRESHOLD.
         */
        m_state.is_infeasible_stagnation =
            !m_incumbent_holder_ptr->is_found_feasible_solution() &&
            m_state.iteration_after_global_augmented_incumbent_update >=
                TabuSearchControllerStateManagerConstant::STAGNATION_THRESHOLD;
    }

    /*************************************************************************/
    inline constexpr void update_is_improved(void) {
        /**
         * "Improved" refers to when any of the following conditions are
         * satisfied:
         * - Objective function value is improved from the previous one.
         * - Total penalty is decreased from the previous one.
         */
        m_state.is_improved =
            (m_incumbent_holder_ptr->local_augmented_incumbent_objective() <
             m_state.previous_solution_score.objective) ||
            (m_incumbent_holder_ptr->local_augmented_incumbent_score()
                 .global_penalty <
             m_state.previous_solution_score.global_penalty);
    }

    /*************************************************************************/
    inline constexpr void turn_flags_off(void) {
        m_state.employing_local_augmented_solution_flag   = false;
        m_state.employing_global_augmented_solution_flag  = false;
        m_state.employing_previous_solution_flag          = false;
        m_state.is_enabled_penalty_coefficient_tightening = false;
        m_state.is_enabled_penalty_coefficient_relaxing   = false;
        m_state.is_enabled_forcibly_initial_modification  = false;
        m_state.penalty_coefficient_reset_flag            = false;
        m_state.is_enabled_special_neighborhood_move      = false;
        m_state.is_disabled_special_neighborhood_move     = false;
    }

    /*************************************************************************/
    inline constexpr void update_improvability_screening_mode(void) {
        if (m_state.tabu_search_result.termination_status ==
            tabu_search::core::TabuSearchCoreTerminationStatus::NO_MOVE) {
            m_state.improvability_screening_mode =
                option::improvability_screening_mode::Soft;
            return;
        }

        if (m_state.is_global_augmented_incumbent_updated) {
            /**
             * If the incumbent solution was updated in the last loop, the
             * improvability screening mode is set to "Intensive" to search
             * better solutions by intensive search.
             */
            m_state.improvability_screening_mode =
                option::improvability_screening_mode::Intensive;
            return;
        }

        if (m_state.tabu_search_result.is_few_permissible_neighborhood) {
            /**
             * If the last loop encountered a situation where there is no
             * permissible solution, the improvability screening mode is set to
             * "Soft" for search diversity.
             */
            m_state.improvability_screening_mode =
                option::improvability_screening_mode::Soft;
            return;
        }

        if (!m_state.tabu_search_result.is_found_new_feasible_solution) {
            /**
             * If the last loop failed to find any feasible solution, the
             * improvability screening mode is set to "Aggressive" or
             * "Intensive" to prioritize the search for feasible solutions.
             */
            if (m_state.is_infeasible_stagnation) {
                if (m_state.relaxation_count % 2 == 0) {
                    m_state.improvability_screening_mode =
                        option::improvability_screening_mode::Intensive;
                } else {
                    m_state.improvability_screening_mode =
                        option::improvability_screening_mode::Aggressive;
                }
            } else {
                m_state.improvability_screening_mode =
                    option::improvability_screening_mode::Aggressive;
            }
            return;
        }
        /**
         * Otherwise, the improvability screening mode is set to "Soft" for
         * search diversity.
         */
        m_state.improvability_screening_mode =
            option::improvability_screening_mode::Soft;
    }

    /*************************************************************************/
    inline constexpr void update_initial_solution_and_penalty_coefficient_flags(
        void) {
        /**
         * Prepare variables for control initial solution, penalty coefficients,
         * initial modification, etc.
         */
        const auto& RESULT_LOCAL_AUGMENTED_INCUMBENT_SCORE =
            m_incumbent_holder_ptr->local_augmented_incumbent_score();

        /**
         * Determine the initial solution for the next loop and flags to tighten
         * or relax the penalty coefficients.
         */

        /**
         *  NOTE: The gap can takes both of positive and negative value.
         */
        const double GAP =
            m_incumbent_holder_ptr->global_augmented_incumbent_objective() -
            m_incumbent_holder_ptr->local_augmented_incumbent_objective();
        const double RELATIVE_RANGE =
            m_state.tabu_search_result.global_augmented_objective_range /
            std::max(1.0, fabs(m_incumbent_holder_ptr
                                   ->global_augmented_incumbent_objective()));

        if (m_state.is_global_augmented_incumbent_updated) {
            /**
             * If the global incumbent solution was updated in the last loop,
             * the global incumbent is employed as the initial solution for the
             * next loop. The penalty coefficients are to be relaxed.
             */
            m_state.employing_global_augmented_solution_flag = true;
            m_state.is_enabled_penalty_coefficient_relaxing  = true;
            return;
        }

        if (m_state.is_not_updated) {
            /**
             * If the last loop failed to find any local/global incumbent
             * solution, the global incumbent solution is employed as the
             * initial solution for the next loop with some initial
             * modifications. The penalty coefficients are to be relaxed after
             * two consecutive search failures.
             */
            m_state.employing_global_augmented_solution_flag = true;
            m_state.is_enabled_forcibly_initial_modification = true;

            if (RESULT_LOCAL_AUGMENTED_INCUMBENT_SCORE.is_feasible) {
                m_state.is_enabled_penalty_coefficient_relaxing = true;
            } else {
                if (m_state.iteration_after_no_update > 0) {
                    m_state.is_enabled_penalty_coefficient_relaxing = true;
                }
            }
            return;
        }

        /**
         * If a local incumbent solution was updated the last loop, the initial
         * solution for the next loop and flags to tighten or relax the penalty
         * coefficients will be determined by complex rules below.
         */
        if (GAP < TabuSearchControllerStateManagerConstant::GAP_TOLERANCE) {
            /**
             * The fact that the gap is negative implies that the obtained local
             * incumbent solution is worse than the global incumbent solution.
             * For this case, the initial solution for the next loop is reset by
             * the global incumbent solution with some initial modifications.
             * The penalty coefficients are to be relaxed or tightened according
             * to the feasibility of the local incumbent solution.
             */
            m_state.employing_global_augmented_solution_flag = true;
            m_state.is_enabled_forcibly_initial_modification = true;

            if (RESULT_LOCAL_AUGMENTED_INCUMBENT_SCORE.is_feasible) {
                m_state.is_enabled_penalty_coefficient_relaxing = true;
            } else {
                m_state.is_enabled_penalty_coefficient_tightening = true;
            }
            return;
        }

        if (RESULT_LOCAL_AUGMENTED_INCUMBENT_SCORE.is_feasible) {
            /**
             * If the gap is positive and the local incumbent solution is
             * feasible, the local incumbent solution is employed as the initial
             * solution for the next loop. The penalty coefficients are to be
             * relaxed.
             */
            m_state.employing_local_augmented_solution_flag = true;
            m_state.is_enabled_penalty_coefficient_relaxing = true;
            return;
        }

        if (RELATIVE_RANGE < TabuSearchControllerStateManagerConstant::
                                 RELATIVE_RANGE_THRESHOLD) {
            m_state.employing_global_augmented_solution_flag = true;
            m_state.is_enabled_forcibly_initial_modification = true;
            m_state.is_enabled_penalty_coefficient_relaxing  = true;
            return;
        }

        /**
         * For the case that the gap is positive and the local incumbent
         * solution is not feasible, the following rules will be applied:
         * (1) If no feasible solution has been found in the previous loops:
         * -> The obtained local incumbent solution is employed as the initial
         * solution for the next loop.
         * (2) If a feasible solution has been found in the previous loops:
         * (2.1) If the obtained local incumbent solution improves the objective
         * function value or global penalty than those of global incumbent
         * solution: -> The obtained local incumbent solution is employed as the
         * initial solution for the next loop.
         *  (2.2) Otherwise; if the obtained local incumbent solution does not
         * improve either objective function value or global penalty:
         * -> The previous initial solution is employed as the initial solution
         * for the next loop.
         *
         * For all cases, penalty coefficients are to be tightened.
         */
        if (m_incumbent_holder_ptr->is_found_feasible_solution()) {
            if (m_state.is_improved) {
                m_state.employing_local_augmented_solution_flag = true;
            } else {
                m_state.employing_previous_solution_flag = true;
            }
        } else {
            m_state.employing_local_augmented_solution_flag = true;
        }
        m_state.is_enabled_penalty_coefficient_tightening = true;
    }

    /*************************************************************************/
    inline constexpr void update_penalty_coefficient_relaxing_rate(void) {
        /**
         * Decrease penalty coefficient relaxing rate if lack of
         * diversification is detected. This applies only if no feasible
         * solution has been found.
         */
        if (m_state.is_infeasible_stagnation &&
            ((m_state.current_primal_intensity >
              m_state.current_primal_intensity_before_relaxation) &&
             (m_state.current_dual_intensity >
              m_state.current_dual_intensity_before_relaxation))) {
            m_state.penalty_coefficient_relaxing_rate = std::max(
                TabuSearchControllerStateManagerConstant::
                    PENALTY_COEFFICIENT_RELAXING_RATE_MIN,
                m_state.penalty_coefficient_relaxing_rate *
                    TabuSearchControllerStateManagerConstant::
                        PENALTY_COEFFICIENT_RELAXING_RATE_DECREASE_RATE);
            return;
        }

        /**
         * Revert penalty coefficient relaxing rate if the feasible incumbent
         * solution is updated.
         */
        if (m_state.current_is_feasible_incumbent_updated) {
            m_state.penalty_coefficient_relaxing_rate =
                m_option.penalty_coefficient_relaxing_rate;
            return;
        }

        /**
         * Increase penalty coefficient relaxing rate if previous solutions are
         * employed as initial solutions, which indicates overrelaxation.
         */
        if (m_state.employing_previous_solution_count_after_relaxation >
            std::max(
                m_state
                    .employing_local_augmented_solution_count_after_relaxation,
                m_state
                    .employing_global_augmented_solution_count_after_relaxation)) {
            m_state.penalty_coefficient_relaxing_rate =
                std::min(TabuSearchControllerStateManagerConstant::
                             PENALTY_COEFFICIENT_RELAXING_RATE_MAX,
                         sqrt(m_state.penalty_coefficient_relaxing_rate));
            return;
        }

        /**
         * Otherwise, draw back the penalty coefficient relaxing rate to the
         * original value.
         */
        m_state.penalty_coefficient_relaxing_rate +=
            TabuSearchControllerStateManagerConstant::
                PENALTY_COEFFICIENT_RELAXING_RATE_STEP_SIZE *
            (m_option.penalty_coefficient_relaxing_rate -
             m_state.penalty_coefficient_relaxing_rate);
    }

    /*************************************************************************/
    inline constexpr void update_penalty_coefficient_reset_flag(void) {
        if (m_state.is_infeasible_stagnation &&
            m_state.is_exceeded_initial_penalty_coefficient &&
            m_state.iteration_after_relaxation >
                TabuSearchControllerStateManagerConstant::
                    ITERATION_AFTER_RELAXATION_MAX) {
            m_state.penalty_coefficient_reset_flag           = true;
            m_state.employing_global_augmented_solution_flag = true;
            m_state.is_enabled_forcibly_initial_modification = true;
        }
    }

    /*************************************************************************/
    inline constexpr void reset_local_penalty_coefficient(void) {
        for (auto&& proxy : m_model_ptr->constraint_proxies()) {
            for (auto&& constraint : proxy.flat_indexed_constraints()) {
                constraint.reset_local_penalty_coefficient();
            }
        }
    }

    /*************************************************************************/
    inline constexpr void tighten_local_penalty_coefficient(void) {
        double total_violation         = 0.0;
        double total_squared_violation = 0.0;

        const auto& LOCAL_AUGMENTED_INCUMBENT_SOLUTION =
            m_incumbent_holder_ptr->local_augmented_incumbent_solution();
        const auto& CONSTRAINT_VALUE_PROXIES =
            LOCAL_AUGMENTED_INCUMBENT_SOLUTION.constraint_value_proxies;
        const auto& VIOLATION_VALUE_PROXIES =
            LOCAL_AUGMENTED_INCUMBENT_SOLUTION.violation_value_proxies;

        m_state.is_exceeded_initial_penalty_coefficient = false;

        for (auto&& proxy : this->m_model_ptr->constraint_proxies()) {
            for (auto&& constraint : proxy.flat_indexed_constraints()) {
                const double VIOLATION =
                    VIOLATION_VALUE_PROXIES[proxy.index()]
                                           [constraint.flat_index()];
                total_violation += VIOLATION;
                total_squared_violation += VIOLATION * VIOLATION;
            }
        }

        const double BALANCE = m_option.penalty_coefficient_updating_balance;
        const double GAP =
            m_incumbent_holder_ptr->global_augmented_incumbent_objective() -
            m_incumbent_holder_ptr->local_augmented_incumbent_objective();

        for (auto&& proxy : this->m_model_ptr->constraint_proxies()) {
            for (auto&& constraint : proxy.flat_indexed_constraints()) {
                const double CONSTRAINT_VALUE = CONSTRAINT_VALUE_PROXIES  //
                    [proxy.index()][constraint.flat_index()];
                const double VIOLATION_VALUE = VIOLATION_VALUE_PROXIES  //
                    [proxy.index()][constraint.flat_index()];

                const double DELTA_PENALTY_COEFFICIENT_CONSTANT =
                    std::max(0.0, GAP) / total_violation;
                const double DELTA_PENALTY_COEFFICIENT_PROPORTIONAL =
                    std::max(0.0, GAP) / total_squared_violation *
                    VIOLATION_VALUE;

                const double POSITIVE_PART = std::max(CONSTRAINT_VALUE, 0.0);
                const double NEGATIVE_PART = std::max(-CONSTRAINT_VALUE, 0.0);
                const double DELTA_PENALTY_COEFFICIENT =
                    (BALANCE * DELTA_PENALTY_COEFFICIENT_CONSTANT +
                     (1.0 - BALANCE) * DELTA_PENALTY_COEFFICIENT_PROPORTIONAL);

                if (constraint.is_less_or_equal() &&
                    POSITIVE_PART > constant::EPSILON) {
                    constraint.local_penalty_coefficient_less() +=
                        m_state.penalty_coefficient_tightening_rate *
                        DELTA_PENALTY_COEFFICIENT;
                } else if (constraint.is_greater_or_equal() &&
                           NEGATIVE_PART > constant::EPSILON) {
                    constraint.local_penalty_coefficient_greater() +=
                        m_state.penalty_coefficient_tightening_rate *
                        DELTA_PENALTY_COEFFICIENT;
                }
            }

            if (m_option.is_enabled_grouping_penalty_coefficient) {
                double max_local_penalty_coefficient = 0;
                for (auto&& constraint : proxy.flat_indexed_constraints()) {
                    max_local_penalty_coefficient =
                        std::max(max_local_penalty_coefficient,
                                 constraint.local_penalty_coefficient_less());
                    max_local_penalty_coefficient = std::max(
                        max_local_penalty_coefficient,
                        constraint.local_penalty_coefficient_greater());
                }
                for (auto&& constraint : proxy.flat_indexed_constraints()) {
                    constraint.local_penalty_coefficient_less() =
                        max_local_penalty_coefficient;
                    constraint.local_penalty_coefficient_greater() =
                        max_local_penalty_coefficient;
                }
            }

            /**
             * Penalty coefficients are bounded by the initial penalty
             * coefficient specified in option.
             */
            const double INITIAL_PENALTY_COEFFICIENT =
                this->m_option.initial_penalty_coefficient;
            for (auto&& constraint : proxy.flat_indexed_constraints()) {
                if (constraint.local_penalty_coefficient_less() >
                    INITIAL_PENALTY_COEFFICIENT) {
                    m_state.is_exceeded_initial_penalty_coefficient = true;
                    constraint.local_penalty_coefficient_less() =
                        INITIAL_PENALTY_COEFFICIENT;
                }

                if (constraint.local_penalty_coefficient_greater() >
                    INITIAL_PENALTY_COEFFICIENT) {
                    m_state.is_exceeded_initial_penalty_coefficient = true;
                    constraint.local_penalty_coefficient_greater() =
                        INITIAL_PENALTY_COEFFICIENT;
                }
            }
        }
    }

    /*************************************************************************/
    inline constexpr void relax_local_penalty_coefficient(void) {
        double corrected_penalty_coefficient_relaxing_rate =
            m_state.penalty_coefficient_relaxing_rate;

        const auto& RESULT = m_state.tabu_search_result;
        if (RESULT.objective_constraint_rate > constant::EPSILON) {
            if (m_incumbent_holder_ptr->local_augmented_incumbent_score()
                    .is_feasible) {
                constexpr double MARGIN = 1.0;
                corrected_penalty_coefficient_relaxing_rate =
                    std::min(m_state.penalty_coefficient_relaxing_rate,
                             RESULT.objective_constraint_rate * MARGIN);
            }
        }

        for (auto&& proxy : m_model_ptr->constraint_proxies()) {
            const auto& CONSTRAINT_VALUES =
                m_incumbent_holder_ptr->local_augmented_incumbent_solution()
                    .constraint_value_proxies[proxy.index()]
                    .flat_indexed_values();

            for (auto&& constraint : proxy.flat_indexed_constraints()) {
                const double CONSTRAINT_VALUE =
                    CONSTRAINT_VALUES[constraint.flat_index()];
                const double POSITIVE_PART = std::max(CONSTRAINT_VALUE, 0.0);
                const double NEGATIVE_PART = std::max(-CONSTRAINT_VALUE, 0.0);

                if (constraint.is_less_or_equal() &&
                    POSITIVE_PART < constant::EPSILON) {
                    constraint.local_penalty_coefficient_less() *=
                        corrected_penalty_coefficient_relaxing_rate;
                }
                if (constraint.is_greater_or_equal() &&
                    NEGATIVE_PART < constant::EPSILON) {
                    constraint.local_penalty_coefficient_greater() *=
                        corrected_penalty_coefficient_relaxing_rate;
                }
            }
        }
    }

    /*************************************************************************/
    inline constexpr void update_initial_tabu_tenure(void) {
        if (m_state.is_global_augmented_incumbent_updated) {
            m_state.initial_tabu_tenure =
                std::min(m_option.tabu_search.initial_tabu_tenure,
                         m_model_ptr->number_of_mutable_variables());
            return;
        }

        if (m_state.is_not_updated) {
            m_state.initial_tabu_tenure =
                std::max(m_state.initial_tabu_tenure - 1,
                         std::min(m_option.tabu_search.initial_tabu_tenure,
                                  m_model_ptr->number_of_mutable_variables()));
            return;
        }

        const auto LAST_TABU_TENURE = m_state.tabu_search_result.tabu_tenure;
        if (LAST_TABU_TENURE > m_state.initial_tabu_tenure) {
            m_state.initial_tabu_tenure =
                std::min(m_state.initial_tabu_tenure + 1,
                         this->m_model_ptr->number_of_mutable_variables());
            return;
        }

        if ((LAST_TABU_TENURE == m_state.initial_tabu_tenure) &&
            ((m_state.current_primal_intensity >
              m_state.previous_primal_intensity) ||
             (m_state.current_dual_intensity >
              m_state.previous_dual_intensity))) {
            return;
        }

        m_state.initial_tabu_tenure =
            std::max(m_state.initial_tabu_tenure - 1,
                     std::min(m_option.tabu_search.initial_tabu_tenure,
                              m_model_ptr->number_of_mutable_variables()));
    }

    /*************************************************************************/
    inline constexpr void update_number_of_initial_modification(
        std::mt19937* a_mt19937_ptr) {
        if (m_state.is_global_augmented_incumbent_updated) {
            m_state.number_of_initial_modification = 0;
            return;
        }

        if (!m_state.is_enabled_forcibly_initial_modification) {
            m_state.number_of_initial_modification = 0;
            return;
        }

        int number_of_initial_modification  //
            = static_cast<int>(std::floor(
                m_option.tabu_search.initial_modification_fixed_rate *
                m_state.initial_tabu_tenure));

        const int RANDOM_WIDTH = static_cast<int>(
            m_option.tabu_search.initial_modification_randomize_rate *
            number_of_initial_modification);

        if (RANDOM_WIDTH > 0) {
            number_of_initial_modification +=
                (*a_mt19937_ptr)() % (2 * RANDOM_WIDTH) - RANDOM_WIDTH;
        }

        number_of_initial_modification =
            std::max(1, number_of_initial_modification);

        m_state.number_of_initial_modification = number_of_initial_modification;
    }

    /*************************************************************************/
    inline constexpr void update_iteration_max(void) {
        if (m_state.tabu_search_result.number_of_iterations !=
            m_state.iteration_max) {
            return;
        }

        int iteration_max = 0;

        if (m_state.is_global_augmented_incumbent_updated) {
            iteration_max = static_cast<int>(
                ceil(m_state.tabu_search_result
                         .last_local_augmented_incumbent_update_iteration *
                     m_option.tabu_search.iteration_increase_rate));

        } else {
            iteration_max = static_cast<int>(
                ceil(m_state.iteration_max *
                     m_option.tabu_search.iteration_increase_rate));
        }

        m_state.iteration_max = std::max(
            m_option.tabu_search.initial_tabu_tenure,
            std::min(m_option.tabu_search.iteration_max, iteration_max));
    }

    /*************************************************************************/
    inline constexpr void update_pruning_rate_threshold(void) {
        m_state.pruning_rate_threshold =
            (m_state.iteration_max == m_option.tabu_search.iteration_max)
                ? m_option.tabu_search.pruning_rate_threshold
                : 1.0;
    }

    /*************************************************************************/
    inline constexpr void disable_special_neighborhood_moves(void) {
        auto& neighborhood = m_model_ptr->neighborhood();

        /// Exclusive OR
        if (m_option.is_enabled_exclusive_or_move) {
            neighborhood.exclusive_or().disable();
        }

        /// Exclusive NOR
        if (m_option.is_enabled_exclusive_nor_move) {
            neighborhood.exclusive_nor().disable();
        }

        /// Inverted Integers
        if (m_option.is_enabled_inverted_integers_move) {
            neighborhood.inverted_integers().disable();
        }

        /// Balanced Integers
        if (m_option.is_enabled_balanced_integers_move) {
            neighborhood.balanced_integers().disable();
        }

        /// Constant Sum Integers
        if (m_option.is_enabled_constant_sum_integers_move) {
            neighborhood.constant_sum_integers().disable();
        }

        /// Constant Difference Integers
        if (m_option.neighborhood
                .is_enabled_constant_difference_integers_move) {
            neighborhood.constant_difference_integers().disable();
        }

        /// Constant Ratio Integers
        if (m_option.is_enabled_constant_ratio_integers_move) {
            neighborhood.constant_ratio_integers().disable();
        }

        /// Aggregation
        if (m_option.is_enabled_aggregation_move) {
            neighborhood.aggregation().disable();
        }

        /// Precedence
        if (m_option.is_enabled_precedence_move) {
            neighborhood.precedence().disable();
        }

        /// Variable Bound
        if (m_option.is_enabled_variable_bound_move) {
            neighborhood.variable_bound().disable();
        }

        /// Trinomial Exclusive NOR
        if (m_option.is_enabled_trinomial_exclusive_nor_move) {
            neighborhood.trinomial_exclusive_nor().disable();
        }

        /// Soft Selection
        if (m_option.is_enabled_soft_selection_move) {
            neighborhood.soft_selection().disable();
        }

        /// Chain
        if (m_option.is_enabled_chain_move) {
            neighborhood.chain().disable();
        }

        /// Two Flip
        if (m_option.is_enabled_two_flip_move) {
            neighborhood.two_flip().disable();
        }
        m_state.is_disabled_special_neighborhood_move = true;
    }

    /*************************************************************************/
    inline constexpr void enable_special_neighborhood_moves(void) {
        auto& neighborhood = m_model_ptr->neighborhood();

        /// Exclusive OR
        if (m_option.is_enabled_exclusive_or_move &&
            neighborhood.exclusive_or().moves().size() > 0) {
            neighborhood.exclusive_or().enable();
        }

        /// Exclusive NOR
        if (m_option.is_enabled_exclusive_nor_move &&
            neighborhood.exclusive_nor().moves().size() > 0) {
            neighborhood.exclusive_nor().enable();
        }

        /// Inverted Integers
        if (m_option.is_enabled_inverted_integers_move &&
            neighborhood.inverted_integers().moves().size() > 0) {
            neighborhood.inverted_integers().enable();
        }

        /// Balanced Integers
        if (m_option.is_enabled_balanced_integers_move &&
            neighborhood.balanced_integers().moves().size() > 0) {
            neighborhood.balanced_integers().enable();
        }

        /// Constant Sum Integers
        if (m_option.is_enabled_constant_sum_integers_move &&
            neighborhood.constant_sum_integers().moves().size() > 0) {
            neighborhood.constant_sum_integers().enable();
        }

        /// Constant Difference Integers
        if (m_option.neighborhood
                .is_enabled_constant_difference_integers_move &&
            neighborhood.constant_difference_integers().moves().size() > 0) {
            neighborhood.constant_difference_integers().enable();
        }

        /// Constant Ratio Integers
        if (m_option.is_enabled_constant_ratio_integers_move &&
            neighborhood.constant_ratio_integers().moves().size() > 0) {
            neighborhood.constant_ratio_integers().enable();
        }

        /// Aggregation
        if (m_option.is_enabled_aggregation_move &&
            neighborhood.aggregation().moves().size() > 0) {
            neighborhood.aggregation().enable();
        }

        /// Precedence
        if (m_option.is_enabled_precedence_move &&
            neighborhood.precedence().moves().size() > 0) {
            neighborhood.precedence().enable();
        }

        /// Variable Bound
        if (m_option.is_enabled_variable_bound_move &&
            neighborhood.variable_bound().moves().size() > 0) {
            neighborhood.variable_bound().enable();
        }

        /// Trinomial Exclusive NOR
        if (m_option.is_enabled_trinomial_exclusive_nor_move &&
            neighborhood.trinomial_exclusive_nor().moves().size() > 0) {
            neighborhood.trinomial_exclusive_nor().enable();
        }

        /// Soft Selection
        if (m_option.is_enabled_soft_selection_move &&
            neighborhood.soft_selection().moves().size() > 0) {
            neighborhood.soft_selection().enable();
        }

        /// Chain
        if (m_option.is_enabled_chain_move &&
            neighborhood.chain().moves().size() > 0) {
            neighborhood.chain().enable();
        }

        /// Two Flip
        if (m_option.is_enabled_two_flip_move &&
            neighborhood.two_flip().moves().size() > 0) {
            neighborhood.two_flip().enable();
        }

        /**
         * Reset the availability of special neighborhood moves.
         */
        if (m_model_ptr->neighborhood()
                .is_enabled_special_neighborhood_move()) {
            m_model_ptr->neighborhood()
                .reset_special_neighborhood_moves_availability();
        }
        m_state.is_enabled_special_neighborhood_move = true;
    }

    /*************************************************************************/
    inline constexpr void clear_chain_moves(void) {
        m_model_ptr->neighborhood().chain().clear_moves();
    }

    /*************************************************************************/
    inline constexpr void sort_and_deduplicate_chain_moves(void) {
        m_model_ptr->neighborhood().chain().sort_moves();
        m_model_ptr->neighborhood().chain().deduplicate_moves();
    }

    /*************************************************************************/
    inline constexpr void reduce_chain_moves(std::mt19937* a_mt19937_ptr) {
        switch (m_option.chain_move_reduce_mode) {
            case option::chain_move_reduce_mode::OverlapRate: {
                m_model_ptr->neighborhood().chain().reduce_moves(
                    m_option.chain_move_capacity);
                break;
            }
            case option::chain_move_reduce_mode::Shuffle: {
                m_model_ptr->neighborhood().chain().shuffle_moves(
                    a_mt19937_ptr);
                m_model_ptr->neighborhood().chain().reduce_moves(
                    m_option.chain_move_capacity);
                break;
            }
            default: {
                throw std::logic_error(utility::format_error_location(
                    __FILE__, __LINE__, __func__,
                    "The specified Chain move reduce mode is "
                    "invalid."));
            }
        }
    }

    /*************************************************************************/
    inline constexpr void update_current_solution(void) {
        if (m_state.employing_global_augmented_solution_flag) {
            m_state.current_solution =
                m_incumbent_holder_ptr->global_augmented_incumbent_solution();
            m_state.current_solution_score =
                m_incumbent_holder_ptr->global_augmented_incumbent_score();
            m_state
                .employing_global_augmented_solution_count_after_relaxation++;
        } else if (m_state.employing_local_augmented_solution_flag) {
            m_state.current_solution =
                m_incumbent_holder_ptr->local_augmented_incumbent_solution();
            m_state.current_solution_score =
                m_incumbent_holder_ptr->local_augmented_incumbent_score();
            m_state.employing_local_augmented_solution_count_after_relaxation++;
        } else if (m_state.employing_previous_solution_flag) {
            m_state.current_solution       = m_state.previous_solution;
            m_state.current_solution_score = m_state.previous_solution_score;
            m_state.employing_previous_solution_count_after_relaxation++;
        } else {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "An error ocurred in determining the next initial solution."));
        }
    }

    /*************************************************************************/
    inline constexpr void update_relaxation_status(void) {
        if (m_state.is_enabled_penalty_coefficient_relaxing) {
            m_state.previous_primal_intensity_before_relaxation =
                m_state.current_primal_intensity_before_relaxation;
            m_state.current_primal_intensity_before_relaxation =
                m_state.current_primal_intensity;

            m_state.previous_dual_intensity_before_relaxation =
                m_state.current_dual_intensity_before_relaxation;
            m_state.current_dual_intensity_before_relaxation =
                m_state.current_dual_intensity;

            m_state.iteration_after_relaxation                         = 0;
            m_state.employing_previous_solution_count_after_relaxation = 0;
            m_state.employing_global_augmented_solution_count_after_relaxation =
                0;
            m_state.employing_local_augmented_solution_count_after_relaxation =
                0;
            m_state.relaxation_count++;
        } else {
            m_state.iteration_after_relaxation++;
        }
    }

    /*************************************************************************/
    inline constexpr void set_total_elapsed_time(
        const double a_TOTAL_ELAPSED_TIME) {
        m_state.total_elapsed_time = a_TOTAL_ELAPSED_TIME;
        m_state.tabu_search_elapsed_time =
            m_state.total_elapsed_time - m_state.tabu_search_start_time;
    }

    /*************************************************************************/
    inline constexpr void set_tabu_search_start_time(
        const double a_START_TIME) {
        m_state.tabu_search_start_time = a_START_TIME;
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
    inline constexpr TabuSearchControllerState<T_Variable, T_Expression>& state(
        void) {
        return m_state;
    }

    /*************************************************************************/
    inline constexpr const TabuSearchControllerState<T_Variable, T_Expression>&
    state(void) const {
        return m_state;
    }
};
}  // namespace printemps::solver::tabu_search::controller
#endif
/*****************************************************************************/
// END
/*****************************************************************************/