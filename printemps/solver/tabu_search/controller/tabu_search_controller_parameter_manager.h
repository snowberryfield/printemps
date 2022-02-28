/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_TABU_SEARCH_CONTROLLER_TABU_SEARCH_CONTROLLER_PARAMETER_MANAGER_H__
#define PRINTEMPS_SOLVER_TABU_SEARCH_CONTROLLER_TABU_SEARCH_CONTROLLER_PARAMETER_MANAGER_H__

namespace printemps {
namespace solver {
namespace tabu_search {
namespace controller {
/*****************************************************************************/
struct TabuSearchControllerParameterManagerConstant {
    static constexpr double RELATIVE_RANGE_THRESHOLD              = 1E-2;
    static constexpr double PENALTY_COEFFICIENT_RELAXING_RATE_MIN = 0.3;
    static constexpr double PENALTY_COEFFICIENT_RELAXING_RATE_MAX = 1.0 - 1E-4;
    static constexpr double PENALTY_COEFFICIENT_RELAXING_RATE_DECREASE_RATE =
        0.9;
    static constexpr double PENALTY_COEFFICIENT_RELAXING_RATE_STEP_SIZE = 1E-1;
    static constexpr int    ITERATION_AFTER_RELAXATION_MAX              = 30;
    static constexpr double GAP_TOLERANCE = constant::EPSILON;
};

/*****************************************************************************/
template <class T_Variable, class T_Expression>
class TabuSearchControllerParameterManager {
   private:
    TabuSearchControllerParameter                        m_parameter;
    model::Model<T_Variable, T_Expression>*              m_model_ptr;
    solution::IncumbentHolder<T_Variable, T_Expression>* m_incumbent_holder_ptr;
    Memory<T_Variable, T_Expression>*                    m_memory_ptr;
    option::Option                                       m_master_option;

   public:
    /*************************************************************************/
    TabuSearchControllerParameterManager(
        model::Model<T_Variable, T_Expression>* a_model_ptr,
        solution::IncumbentHolder<T_Variable, T_Expression>*
                                          a_incumbent_holder_ptr,  //
        Memory<T_Variable, T_Expression>* a_memory_ptr,            //
        const option::Option&             a_MASTER_OPTION) {
        this->setup(a_model_ptr,             //
                    a_incumbent_holder_ptr,  //
                    a_memory_ptr,            //
                    a_MASTER_OPTION);
    }

    /*************************************************************************/
    TabuSearchControllerParameterManager(void) {
        this->initialize();
    }

    /*************************************************************************/
    inline void initialize(void) {
        m_parameter.initialize();
        m_model_ptr            = nullptr;
        m_incumbent_holder_ptr = nullptr;
        m_memory_ptr           = nullptr;
        m_master_option.initialize();
    }

    /*************************************************************************/
    inline void setup(model::Model<T_Variable, T_Expression>* a_model_ptr,
                      solution::IncumbentHolder<T_Variable, T_Expression>*
                                                        a_incumbent_holder_ptr,  //
                      Memory<T_Variable, T_Expression>* a_memory_ptr,  //
                      const option::Option&             a_MASTER_OPTION) {
        this->initialize();
        m_model_ptr            = a_model_ptr;
        m_incumbent_holder_ptr = a_incumbent_holder_ptr;
        m_memory_ptr           = a_memory_ptr;
        m_master_option        = a_MASTER_OPTION;
        this->reset();
    }

    /*************************************************************************/
    inline constexpr void reset(void) {
        m_parameter.initial_tabu_tenure =
            m_master_option.tabu_search.initial_tabu_tenure;
        m_parameter.pruning_rate_threshold =
            m_master_option.tabu_search.pruning_rate_threshold;
        m_parameter.number_of_initial_modification = 0;
        m_parameter.iteration_max = m_master_option.tabu_search.iteration_max;
        m_parameter.penalty_coefficient_relaxing_rate =
            m_master_option.penalty_coefficient_relaxing_rate;
        m_parameter.penalty_coefficient_tightening_rate =
            m_master_option.penalty_coefficient_tightening_rate;

        m_parameter.improvability_screening_mode =
            m_master_option.improvability_screening_mode;
        if (m_parameter.improvability_screening_mode ==
            option::improvability_screening_mode::Automatic) {
            m_parameter.improvability_screening_mode =
                option::improvability_screening_mode::Intensive;
        }
    }

    /*************************************************************************/
    inline option::Option create_option(const int    a_SEED,
                                        const double a_ELAPSED_TIME) const {
        option::Option option = m_master_option;

        option.improvability_screening_mode =
            m_parameter.improvability_screening_mode;
        option.tabu_search.iteration_max = m_parameter.iteration_max;
        option.tabu_search.time_offset   = a_ELAPSED_TIME;
        option.tabu_search.seed          = a_SEED;
        option.tabu_search.number_of_initial_modification =
            m_parameter.number_of_initial_modification;
        option.tabu_search.initial_tabu_tenure =
            m_parameter.initial_tabu_tenure;
        option.tabu_search.pruning_rate_threshold =
            m_parameter.pruning_rate_threshold;
        return option;
    }

    /*************************************************************************/
    inline constexpr void update(
        const TabuSearchControllerState<T_Variable, T_Expression>& a_STATE,
        std::mt19937* a_mt19937_ptr) {
        this->turn_flags_off();
        /**
         * If the improvability_screening_mode was set to "Automatic",
         * determine the mode according to the search status so far.
         */
        if (m_master_option.improvability_screening_mode ==
            option::improvability_screening_mode::Automatic) {
            this->update_improvability_screening_mode(a_STATE);
        }

        /**
         * Determine the initial solution for the next loop and whether penalty
         * coefficients are to be relaxed or tightened.
         */
        this->update_initial_solution_and_penalty_coefficient_flags(a_STATE);

        /**
         * Additional processes for cases when the penalty coefficients are
         * relaxed.
         */
        if (m_parameter.is_enabled_penalty_coefficient_relaxing) {
            this->update_penalty_coefficient_relaxing_rate(a_STATE);
        }

        /**
         * Additional processes for cases when the penalty coefficients are
         * tightened: Reset penalty coefficients if stagnation is detected.
         */
        if (m_parameter.is_enabled_penalty_coefficient_tightening) {
            this->update_penalty_coefficient_reset_flag(a_STATE);
        }

        /**
         * Update the local penalty coefficients.
         */
        if (m_parameter.penalty_coefficient_reset_flag) {
            this->reset_local_penalty_coefficient();
        } else if (m_parameter.is_enabled_penalty_coefficient_tightening) {
            this->tighten_local_penalty_coefficient();
        } else if (m_parameter.is_enabled_penalty_coefficient_relaxing) {
            this->relax_local_penalty_coefficient(a_STATE);
        }

        /**
         * Update the initial tabu tenure for the next loop.
         */
        if (m_master_option.tabu_search
                .is_enabled_automatic_tabu_tenure_adjustment) {
            this->update_initial_tabu_tenure(a_STATE);
        }

        /**
         * Update the number of initial modification for the next loop.
         */
        if (m_master_option.tabu_search.is_enabled_initial_modification) {
            this->update_number_of_initial_modification(a_STATE, a_mt19937_ptr);
        }

        /**
         * Update the maximum number of iterations and pruning rate threshold
         * for the next loop.
         */
        if (m_master_option.tabu_search
                .is_enabled_automatic_iteration_adjustment) {
            this->update_iteration_max(a_STATE);
            this->update_pruning_rate_threshold();
        }

        /**
         * Update the neighborhood moves to be employed for the next loop.
         */
        if (a_STATE.is_global_augmented_incumbent_updated) {
            /**
             * Disable the special neighborhood moves if the incumbent was
             * updated.
             */
            this->disable_special_neighborhod_moves();

        } else {
            /**
             * Enable the special neighborhood moves if the incumbent was
             * not updated.
             */
            if (a_STATE.tabu_search_result.number_of_iterations ==
                m_master_option.tabu_search.iteration_max) {
                this->enable_special_neighborhod_moves();
            }
        }

        /**
         * Reset chain moves if the global augmented objective was updated.
         */
        if (a_STATE.is_global_augmented_incumbent_updated) {
            if (m_master_option.is_enabled_chain_move) {
                this->clear_chain_moves();
            }
        }

        /**
         * Sort and deduplicate registered chain moves.
         */
        if (m_model_ptr->neighborhood().chain().is_enabled() &&
            m_master_option.chain_move_capacity > 0) {
            this->sort_and_deduplicate_chain_moves();
        }

        /**
         * Reduce the registered chain moves.
         */
        if (static_cast<int>(
                this->m_model_ptr->neighborhood().chain().moves().size()) >
            m_master_option.chain_move_capacity) {
            this->reduce_chain_moves(a_mt19937_ptr);
        }
    }

    /*************************************************************************/
    inline constexpr void turn_flags_off(void) {
        m_parameter.employing_local_augmented_solution_flag   = false;
        m_parameter.employing_global_augmented_solution_flag  = false;
        m_parameter.employing_previous_solution_flag          = false;
        m_parameter.is_enabled_penalty_coefficient_tightening = false;
        m_parameter.is_enabled_penalty_coefficient_relaxing   = false;
        m_parameter.is_enabled_forcibly_initial_modification  = false;
        m_parameter.penalty_coefficient_reset_flag            = false;
        m_parameter.is_enabled_special_neighborhood_move      = false;
        m_parameter.is_disabled_special_neighborhood_move     = false;
    }

    /*************************************************************************/
    inline constexpr void update_improvability_screening_mode(
        const TabuSearchControllerState<T_Variable, T_Expression>& a_STATE) {
        if (a_STATE.tabu_search_result.termination_status ==
            tabu_search::core::TabuSearchCoreTerminationStatus::NO_MOVE) {
            m_parameter.improvability_screening_mode =
                option::improvability_screening_mode::Soft;
            return;
        }

        if (a_STATE.is_global_augmented_incumbent_updated) {
            /**
             * If the incumbent solution was updated in the last loop, the
             * improvability screening mode is set to "Intensive" to search
             * better solutions by intensive search.
             */
            m_parameter.improvability_screening_mode =
                option::improvability_screening_mode::Intensive;
            return;
        }

        if (a_STATE.tabu_search_result.is_few_permissible_neighborhood) {
            /**
             * If the last loop encountered a situation where there is no
             * permissible solution, the improvability screening mode is set to
             * "Soft" for search diversity.
             */
            m_parameter.improvability_screening_mode =
                option::improvability_screening_mode::Soft;
            return;
        }

        if (!a_STATE.tabu_search_result.is_found_new_feasible_solution) {
            /**
             * If the last loop failed to find any feasible solution, the
             * improvability screening mode is set to "Aggressive" or
             * "Intensive" to prioritize the search for feasible solutions.
             */
            if (a_STATE.is_infeasible_stagnation) {
                if (a_STATE.relaxation_count % 2 == 0) {
                    m_parameter.improvability_screening_mode =
                        option::improvability_screening_mode::Intensive;
                } else {
                    m_parameter.improvability_screening_mode =
                        option::improvability_screening_mode::Aggressive;
                }
            } else {
                m_parameter.improvability_screening_mode =
                    option::improvability_screening_mode::Aggressive;
            }
            return;
        }
        /**
         * Otherwise, the improvability screening mode is set to "Soft" for
         * search diversity.
         */
        m_parameter.improvability_screening_mode =
            option::improvability_screening_mode::Soft;
    }

    /*************************************************************************/
    inline constexpr void update_initial_solution_and_penalty_coefficient_flags(
        const TabuSearchControllerState<T_Variable, T_Expression>& a_STATE) {
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
            a_STATE.tabu_search_result.global_augmented_objective_range /
            std::max(1.0, fabs(m_incumbent_holder_ptr
                                   ->global_augmented_incumbent_objective()));

        if (a_STATE.is_global_augmented_incumbent_updated) {
            /**
             * If the global incumbent solution was updated in the last loop,
             * the global incumbent is employed as the initial solution for the
             * next loop. The penalty coefficients are to be relaxed.
             */
            m_parameter.employing_global_augmented_solution_flag = true;
            m_parameter.is_enabled_penalty_coefficient_relaxing  = true;
            return;
        }

        if (a_STATE.is_not_updated) {
            /**
             * If the last loop failed to find any local/global incumbent
             * solution, the global incumbent solution is employed as the
             * initial solution for the next loop with some initial
             * modifications. The penalty coefficients are to be relaxed after
             * two consecutive search failures.
             */
            m_parameter.employing_global_augmented_solution_flag = true;
            m_parameter.is_enabled_forcibly_initial_modification = true;

            if (RESULT_LOCAL_AUGMENTED_INCUMBENT_SCORE.is_feasible) {
                m_parameter.is_enabled_penalty_coefficient_relaxing = true;
            } else {
                if (a_STATE.iteration_after_no_update > 0) {
                    m_parameter.is_enabled_penalty_coefficient_relaxing = true;
                }
            }
            return;
        }

        /**
         * If a local incumbent solution was updated the last loop, the initial
         * solution for the next loop and flags to tighten or relax the penalty
         * coefficients will be determined by complexed rules below.
         */
        if (GAP < TabuSearchControllerParameterManagerConstant::GAP_TOLERANCE) {
            /**
             * The fact that the gap is negative implies that the obtained local
             * incumbent solution is worse than the global incumbent solution.
             * For this case, the initial solution for the next loop is reset by
             * the global incumbent solution with some initial modifications.
             * The penalty coefficients are to be relaxed or tightened according
             * to the feasibility of the local incumbent solution.
             */
            m_parameter.employing_global_augmented_solution_flag = true;
            m_parameter.is_enabled_forcibly_initial_modification = true;

            if (RESULT_LOCAL_AUGMENTED_INCUMBENT_SCORE.is_feasible) {
                m_parameter.is_enabled_penalty_coefficient_relaxing = true;
            } else {
                m_parameter.is_enabled_penalty_coefficient_tightening = true;
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
            m_parameter.employing_local_augmented_solution_flag = true;
            m_parameter.is_enabled_penalty_coefficient_relaxing = true;
            return;
        }

        if (RELATIVE_RANGE < TabuSearchControllerParameterManagerConstant::
                                 RELATIVE_RANGE_THRESHOLD) {
            m_parameter.employing_global_augmented_solution_flag = true;
            m_parameter.is_enabled_forcibly_initial_modification = true;
            m_parameter.is_enabled_penalty_coefficient_relaxing  = true;
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
            if (a_STATE.is_improved) {
                m_parameter.employing_local_augmented_solution_flag = true;
            } else {
                m_parameter.employing_previous_solution_flag = true;
            }
        } else {
            m_parameter.employing_local_augmented_solution_flag = true;
        }
        m_parameter.is_enabled_penalty_coefficient_tightening = true;
    }

    /*************************************************************************/
    inline constexpr void update_penalty_coefficient_relaxing_rate(
        const TabuSearchControllerState<T_Variable, T_Expression>& a_STATE) {
        /**
         * Descrease penalty coefficient relaxing rate if lack of
         * diversification is detected. This applies only if no feasible
         * solution has been found.
         */
        if (a_STATE.is_infeasible_stagnation &&
            ((a_STATE.current_primal_intensity >
              a_STATE.current_primal_intensity_before_relaxation) &&
             (a_STATE.current_dual_intensity >
              a_STATE.current_dual_intensity_before_relaxation))) {
            m_parameter.penalty_coefficient_relaxing_rate = std::max(
                TabuSearchControllerParameterManagerConstant::
                    PENALTY_COEFFICIENT_RELAXING_RATE_MIN,
                m_parameter.penalty_coefficient_relaxing_rate *
                    TabuSearchControllerParameterManagerConstant::
                        PENALTY_COEFFICIENT_RELAXING_RATE_DECREASE_RATE);
            return;
        }

        /**
         * Revert penalty coefficient relaxing rate if the feasible incumbent
         * solution is updated.
         */
        if (a_STATE.is_feasible_incumbent_updated) {
            m_parameter.penalty_coefficient_relaxing_rate =
                m_master_option.penalty_coefficient_relaxing_rate;
            return;
        }

        /**
         * Increase penalty coefficient relaxing rate if previous solutions are
         * employed as initial solutions, which indicates overrelaxation.
         */
        if (a_STATE.employing_previous_solution_count_after_relaxation >
            std::max(
                a_STATE
                    .employing_local_augmented_solution_count_after_relaxation,
                a_STATE
                    .employing_global_augmented_solution_count_after_relaxation)) {
            m_parameter.penalty_coefficient_relaxing_rate =
                std::min(TabuSearchControllerParameterManagerConstant::
                             PENALTY_COEFFICIENT_RELAXING_RATE_MAX,
                         sqrt(m_parameter.penalty_coefficient_relaxing_rate));
            return;
        }

        /**
         * Otherwise, draw back the penalty coefficient relaxing rate to the
         * original value.
         */
        m_parameter.penalty_coefficient_relaxing_rate +=
            TabuSearchControllerParameterManagerConstant::
                PENALTY_COEFFICIENT_RELAXING_RATE_STEP_SIZE *
            (m_master_option.penalty_coefficient_relaxing_rate -
             m_parameter.penalty_coefficient_relaxing_rate);
    }

    /*************************************************************************/
    inline constexpr void update_penalty_coefficient_reset_flag(
        const TabuSearchControllerState<T_Variable, T_Expression>& a_STATE) {
        if (a_STATE.is_infeasible_stagnation &&
            a_STATE.iteration_after_relaxation >
                TabuSearchControllerParameterManagerConstant::
                    ITERATION_AFTER_RELAXATION_MAX) {
            m_parameter.penalty_coefficient_reset_flag           = true;
            m_parameter.employing_global_augmented_solution_flag = true;
            m_parameter.is_enabled_forcibly_initial_modification = true;
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

        for (auto&& proxy : this->m_model_ptr->constraint_proxies()) {
            for (auto&& constraint : proxy.flat_indexed_constraints()) {
                const double VIOLATION =
                    VIOLATION_VALUE_PROXIES[proxy.index()]
                                           [constraint.flat_index()];
                total_violation += VIOLATION;
                total_squared_violation += VIOLATION * VIOLATION;
            }
        }

        const double BALANCE =
            m_master_option.penalty_coefficient_updating_balance;
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
                        m_parameter.penalty_coefficient_tightening_rate *
                        DELTA_PENALTY_COEFFICIENT;
                } else if (constraint.is_greater_or_equal() &&
                           NEGATIVE_PART > constant::EPSILON) {
                    constraint.local_penalty_coefficient_greater() +=
                        m_parameter.penalty_coefficient_tightening_rate *
                        DELTA_PENALTY_COEFFICIENT;
                }
            }

            if (m_master_option.is_enabled_grouping_penalty_coefficient) {
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
            for (auto&& constraint : proxy.flat_indexed_constraints()) {
                constraint.local_penalty_coefficient_less() =
                    std::min(constraint.local_penalty_coefficient_less(),
                             this->m_master_option.initial_penalty_coefficient);
                constraint.local_penalty_coefficient_greater() =
                    std::min(constraint.local_penalty_coefficient_greater(),
                             this->m_master_option.initial_penalty_coefficient);
            }
        }
    }

    /*************************************************************************/
    inline constexpr void relax_local_penalty_coefficient(
        const TabuSearchControllerState<T_Variable, T_Expression>& a_STATE) {
        double corrected_penalty_coefficient_relaxing_rate =
            m_parameter.penalty_coefficient_relaxing_rate;

        const auto& RESULT = a_STATE.tabu_search_result;
        if (RESULT.objective_constraint_rate > constant::EPSILON) {
            if (m_incumbent_holder_ptr->local_augmented_incumbent_score()
                    .is_feasible) {
                constexpr double MARGIN = 1.0;
                corrected_penalty_coefficient_relaxing_rate =
                    std::min(m_parameter.penalty_coefficient_relaxing_rate,
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
    inline constexpr void update_initial_tabu_tenure(
        const TabuSearchControllerState<T_Variable, T_Expression>& a_STATE) {
        if (a_STATE.is_global_augmented_incumbent_updated) {
            m_parameter.initial_tabu_tenure =
                std::min(m_master_option.tabu_search.initial_tabu_tenure,
                         m_model_ptr->number_of_mutable_variables());
            return;
        }

        if (a_STATE.is_not_updated) {
            m_parameter.initial_tabu_tenure = std::max(
                m_parameter.initial_tabu_tenure - 1,
                std::min(m_master_option.tabu_search.initial_tabu_tenure,
                         m_model_ptr->number_of_mutable_variables()));
            return;
        }

        const auto LAST_TABU_TENURE = a_STATE.tabu_search_result.tabu_tenure;
        if (LAST_TABU_TENURE > m_parameter.initial_tabu_tenure) {
            m_parameter.initial_tabu_tenure =
                std::min(m_parameter.initial_tabu_tenure + 1,
                         this->m_model_ptr->number_of_mutable_variables());
            return;
        }

        if ((LAST_TABU_TENURE == m_parameter.initial_tabu_tenure) &&
            ((a_STATE.current_primal_intensity >
              a_STATE.previous_primal_intensity) ||
             (a_STATE.current_dual_intensity >
              a_STATE.previous_dual_intensity))) {
            return;
        }

        m_parameter.initial_tabu_tenure =
            std::max(m_parameter.initial_tabu_tenure - 1,
                     std::min(m_master_option.tabu_search.initial_tabu_tenure,
                              m_model_ptr->number_of_mutable_variables()));
    }

    /*************************************************************************/
    inline constexpr void update_number_of_initial_modification(
        const TabuSearchControllerState<T_Variable, T_Expression>& a_STATE,
        std::mt19937* a_mt19937_ptr) {
        if (a_STATE.is_global_augmented_incumbent_updated) {
            m_parameter.number_of_initial_modification = 0;
            return;
        }

        if (!m_parameter.is_enabled_forcibly_initial_modification) {
            m_parameter.number_of_initial_modification = 0;
            return;
        }

        int number_of_initial_modification  //
            = static_cast<int>(std::floor(
                m_master_option.tabu_search.initial_modification_fixed_rate *
                m_parameter.initial_tabu_tenure));

        const int RANDOM_WIDTH = static_cast<int>(
            m_master_option.tabu_search.initial_modification_randomize_rate *
            number_of_initial_modification);

        if (RANDOM_WIDTH > 0) {
            number_of_initial_modification +=
                (*a_mt19937_ptr)() % (2 * RANDOM_WIDTH) - RANDOM_WIDTH;
        }

        number_of_initial_modification =
            std::max(1, number_of_initial_modification);

        m_parameter.number_of_initial_modification =
            number_of_initial_modification;
    }

    /*************************************************************************/
    inline constexpr void update_iteration_max(
        const TabuSearchControllerState<T_Variable, T_Expression>& a_STATE) {
        if (a_STATE.tabu_search_result.number_of_iterations !=
            m_parameter.iteration_max) {
            return;
        }

        int iteration_max = 0;

        if (a_STATE.is_global_augmented_incumbent_updated) {
            iteration_max = static_cast<int>(
                ceil(a_STATE.tabu_search_result
                         .last_local_augmented_incumbent_update_iteration *
                     m_master_option.tabu_search.iteration_increase_rate));

        } else {
            iteration_max = static_cast<int>(
                ceil(m_parameter.iteration_max *
                     m_master_option.tabu_search.iteration_increase_rate));
        }

        m_parameter.iteration_max = std::max(
            m_master_option.tabu_search.initial_tabu_tenure,
            std::min(m_master_option.tabu_search.iteration_max, iteration_max));
    }

    /*************************************************************************/
    inline constexpr void update_pruning_rate_threshold(void) {
        m_parameter.pruning_rate_threshold =
            (m_parameter.iteration_max ==
             m_master_option.tabu_search.iteration_max)
                ? m_master_option.tabu_search.pruning_rate_threshold
                : 1.0;
    }

    /*************************************************************************/
    inline constexpr void disable_special_neighborhod_moves(void) {
        /// Aggregation
        if (m_master_option.is_enabled_aggregation_move) {
            m_model_ptr->neighborhood().aggregation().disable();
        }

        /// Precedence
        if (m_master_option.is_enabled_precedence_move) {
            m_model_ptr->neighborhood().precedence().disable();
        }

        /// Variable Bound
        if (m_master_option.is_enabled_variable_bound_move) {
            m_model_ptr->neighborhood().variable_bound().disable();
        }

        /// Soft Selection
        if (m_master_option.is_enabled_soft_selection_move) {
            m_model_ptr->neighborhood().soft_selection().disable();
        }

        /// Chain
        if (m_master_option.is_enabled_chain_move) {
            m_model_ptr->neighborhood().chain().disable();
        }

        /// Two Flip
        if (m_master_option.is_enabled_two_flip_move &&
            m_model_ptr->flippable_variable_ptr_pairs().size() > 0) {
            m_model_ptr->neighborhood().two_flip().disable();
        }
        m_parameter.is_disabled_special_neighborhood_move = true;
    }

    /*************************************************************************/
    inline constexpr void enable_special_neighborhod_moves(void) {
        /// Aggregation
        if (m_master_option.is_enabled_aggregation_move) {
            m_model_ptr->neighborhood().aggregation().enable();
        }

        /// Precedence
        if (m_master_option.is_enabled_precedence_move) {
            m_model_ptr->neighborhood().precedence().enable();
        }

        /// Variable Bound
        if (m_master_option.is_enabled_variable_bound_move) {
            m_model_ptr->neighborhood().variable_bound().enable();
        }

        /// Soft Selection
        if (m_master_option.is_enabled_soft_selection_move) {
            m_model_ptr->neighborhood().soft_selection().enable();
        }

        /// Chain
        if (m_master_option.is_enabled_chain_move) {
            m_model_ptr->neighborhood().chain().enable();
        }

        /// Two Flip
        if (m_master_option.is_enabled_two_flip_move &&
            m_model_ptr->flippable_variable_ptr_pairs().size() > 0) {
            m_model_ptr->neighborhood().two_flip().enable();
        }

        /**
         * Reset the availability of special neighborhood moves.
         */
        if (m_model_ptr->neighborhood()
                .is_enabled_special_neighborhood_move()) {
            m_model_ptr->neighborhood()
                .reset_special_neighborhood_moves_availability();
        }
        m_parameter.is_enabled_special_neighborhood_move = true;
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
        switch (m_master_option.chain_move_reduce_mode) {
            case option::chain_move_reduce_mode::OverlapRate: {
                m_model_ptr->neighborhood().chain().reduce_moves(
                    m_master_option.chain_move_capacity);
                break;
            }
            case option::chain_move_reduce_mode::Shuffle: {
                m_model_ptr->neighborhood().chain().shuffle_moves(
                    a_mt19937_ptr);
                m_model_ptr->neighborhood().chain().reduce_moves(
                    m_master_option.chain_move_capacity);
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
    inline constexpr TabuSearchControllerParameter& parameter(void) {
        return m_parameter;
    }

    /*************************************************************************/
    inline constexpr const TabuSearchControllerParameter& parameter(
        void) const {
        return m_parameter;
    }

};  // namespace tabu_search
}  // namespace controller
}  // namespace tabu_search
}  // namespace solver
}  // namespace printemps

#endif
/*****************************************************************************/
// END
/*****************************************************************************/