/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_TABU_SEARCH_CORE_TABU_SEARCH_CORE_STATE_MANAGER_H__
#define PRINTEMPS_SOLVER_TABU_SEARCH_CORE_TABU_SEARCH_CORE_STATE_MANAGER_H__

#include "tabu_search_core_state.h"

namespace printemps::solver::tabu_search::core {

/*****************************************************************************/
template <class T_Variable, class T_Expression>
class TabuSearchCoreStateManager {
   private:
    TabuSearchCoreState<T_Variable, T_Expression>        m_state;
    model::Model<T_Variable, T_Expression>*              m_model_ptr;
    solution::IncumbentHolder<T_Variable, T_Expression>* m_incumbent_holder_ptr;
    Memory<T_Variable, T_Expression>*                    m_memory_ptr;
    option::Option                                       m_option;

   public:
    /*************************************************************************/
    TabuSearchCoreStateManager(
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
    TabuSearchCoreStateManager(void) {
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
         * Initialize the last incumbent update iterations.
         */
        m_state.last_local_augmented_incumbent_update_iteration  = -1;
        m_state.last_global_augmented_incumbent_update_iteration = -1;
        m_state.last_feasible_incumbent_update_iteration         = -1;

        /**
         * Initialize the range of raw objective.
         */
        m_state.objective_range.update(
            m_state.current_solution_score.objective);

        /**
         * Initialize the range of local augmented objective.
         */
        m_state.local_augmented_objective_range.update(
            m_state.current_solution_score.local_augmented_objective);

        /**
         * Initialize the range of global augmented objective.
         */
        m_state.global_augmented_objective_range.update(
            m_state.current_solution_score.global_augmented_objective);

        /**
         * Initialize the range of local penalty.
         */
        m_state.local_penalty_range.update(HUGE_VALF);
        if (!m_state.current_solution_score.is_feasible) {
            m_state.local_penalty_range.update(
                m_state.current_solution_score.local_penalty);
        }

        /**
         * Initialize the primal and dual intensities.
         */
        m_state.current_primal_intensity  = m_memory_ptr->primal_intensity();
        m_state.previous_primal_intensity = m_state.current_primal_intensity;

        m_state.current_dual_intensity  = m_memory_ptr->dual_intensity();
        m_state.previous_dual_intensity = m_state.current_dual_intensity;

        /**
         * Initialize the tabu tenure.
         */
        m_state.original_tabu_tenure =
            std::min(m_option.tabu_search.initial_tabu_tenure,
                     m_model_ptr->number_of_mutable_variables());
        m_state.tabu_tenure = m_state.original_tabu_tenure;

        /**
         * NOTE: The values of the other members of m_state remain at their
         * default values.
         */
    }

    /*************************************************************************/
    inline constexpr void update(
        neighborhood::Move<T_Variable, T_Expression>* a_selected_move_ptr,
        const int a_SELECTED_INDEX, const bool a_IS_ASPIRATED,
        const std::vector<TabuSearchCoreMoveScore>& a_TRIAL_MOVE_SCORES,
        const std::vector<solution::SolutionScore>& a_TRIAL_SOLUTION_SCORES) {
        /**
         * Update the current move with keeping the previous one.
         */
        this->update_move(a_selected_move_ptr);

        /**
         * Update the current solution score with keeping the previous one.
         */
        this->update_solution_score(a_TRIAL_SOLUTION_SCORES[a_SELECTED_INDEX]);

        /**
         * Update the update status.
         */
        this->update_update_status();

        /**
         * Update the aspiraton flag.
         */
        this->update_is_aspirated(a_IS_ASPIRATED);

        /**
         * Update the range of raw objective.
         */
        this->update_objective_range();

        /**
         * Update the range of local augmented objective.
         */
        this->update_local_augmented_objective_range();

        /**
         * Update the range of global augmented objective.
         */
        this->update_global_augmented_objective_range();

        /**
         * Update the range of local penalty.
         */
        this->update_local_penalty_range();

        /**
         * Update the number of effective updates.
         */
        this->update_number_of_effective_updates();

        /**
         * Update the number of evaluated moves.
         */
        this->update_number_of_evaluated_moves();

        /**
         * Update whether new feasible solution was found.
         */
        this->update_is_found_new_feasible_solution();

        /**
         * Update last incumbent update iterations.
         */
        this->update_last_local_augmented_incumbent_update_iteration();
        this->update_last_global_augmented_incumbent_update_iteration();
        this->update_last_feasible_incumbent_update_iteration();

        /**
         * For pruning, count updating of the local augmented incumbent
         * without global augmented incumbent improvement.
         */
        this->update_local_augmented_incumbent_update_count();

        /**
         * Update the number of neighborhoods.
         */
        this->update_number_of_neighborhoods(a_TRIAL_MOVE_SCORES,
                                             a_TRIAL_SOLUTION_SCORES);

        /**
         * Update the tabu tenure and related states.
         */
        this->update_tabu_tenure();
    }

    /*************************************************************************/
    inline constexpr void update_move(
        neighborhood::Move<T_Variable, T_Expression>* a_selected_move_ptr) {
        m_state.previous_move = m_state.current_move;
        m_state.current_move  = *a_selected_move_ptr;
    }

    /*************************************************************************/
    inline constexpr void update_solution_score(
        const solution::SolutionScore& a_SOLUTION_SCORE) {
        m_state.previous_solution_score = m_state.current_solution_score;
        m_state.current_solution_score  = a_SOLUTION_SCORE;
    }

    /*************************************************************************/
    inline constexpr void update_update_status(void) {
        m_state.update_status = m_incumbent_holder_ptr->try_update_incumbent(
            m_model_ptr, m_state.current_solution_score);
        m_state.total_update_status =
            m_state.update_status | m_state.total_update_status;
    }

    /*************************************************************************/
    inline constexpr void update_is_aspirated(const bool a_IS_ASPIRATED) {
        m_state.is_aspirated = a_IS_ASPIRATED;
    }

    /*************************************************************************/
    inline constexpr void update_objective_range(void) {
        m_state.objective_range.update(
            m_state.current_solution_score.objective);
    }

    /*************************************************************************/
    inline constexpr void update_local_augmented_objective_range(void) {
        m_state.local_augmented_objective_range.update(
            m_state.current_solution_score.local_augmented_objective);
    }

    /*************************************************************************/
    inline constexpr void update_global_augmented_objective_range(void) {
        m_state.global_augmented_objective_range.update(
            m_state.current_solution_score.global_augmented_objective);
    }

    /*************************************************************************/
    inline constexpr void update_local_penalty_range(void) {
        if (!m_state.current_solution_score.is_feasible) {
            m_state.local_penalty_range.update(
                m_state.current_solution_score.local_penalty);
        }
    }

    /*************************************************************************/
    inline constexpr void update_number_of_effective_updates() {
        if (m_state.update_status &
            solution::IncumbentHolderConstant::
                STATUS_GLOBAL_AUGMENTED_INCUMBENT_UPDATE) {
            m_state.number_of_effective_updates++;
        } else if (m_state.update_status &
                   solution::IncumbentHolderConstant::
                       STATUS_LOCAL_AUGMENTED_INCUMBENT_UPDATE) {
            if (m_state.current_solution_score.objective <
                m_incumbent_holder_ptr->global_augmented_incumbent_score()
                    .objective) {
                m_state.number_of_effective_updates++;
            }
        }
    }

    /*************************************************************************/
    inline constexpr void update_number_of_evaluated_moves() {
        m_state.number_of_evaluated_moves += m_state.number_of_moves;
    }

    /*************************************************************************/
    inline constexpr void update_is_found_new_feasible_solution(void) {
        if (m_state.current_solution_score.is_feasible) {
            m_state.is_found_new_feasible_solution = true;
        }
    }

    /*************************************************************************/
    inline constexpr void
    update_last_local_augmented_incumbent_update_iteration(void) {
        if (m_state.update_status &
            solution::IncumbentHolderConstant::
                STATUS_LOCAL_AUGMENTED_INCUMBENT_UPDATE) {
            m_state.last_local_augmented_incumbent_update_iteration =
                m_state.iteration;
        }
    }

    /*************************************************************************/
    inline constexpr void
    update_last_global_augmented_incumbent_update_iteration(void) {
        if (m_state.update_status &
            solution::IncumbentHolderConstant::
                STATUS_GLOBAL_AUGMENTED_INCUMBENT_UPDATE) {
            m_state.last_global_augmented_incumbent_update_iteration =
                m_state.iteration;
        }
    }

    /*************************************************************************/
    inline constexpr void update_last_feasible_incumbent_update_iteration(
        void) {
        if (m_state.update_status & solution::IncumbentHolderConstant::
                                        STATUS_FEASIBLE_INCUMBENT_UPDATE) {
            m_state.last_feasible_incumbent_update_iteration =
                m_state.iteration;
        }
    }

    /*************************************************************************/
    inline constexpr void update_local_augmented_incumbent_update_count(void) {
        if (m_state.update_status ==
            solution::IncumbentHolderConstant::
                STATUS_LOCAL_AUGMENTED_INCUMBENT_UPDATE) {
            m_state.local_augmented_incumbent_update_count++;
            return;
        }

        if (m_state.update_status &
            solution::IncumbentHolderConstant::
                STATUS_GLOBAL_AUGMENTED_INCUMBENT_UPDATE) {
            m_state.local_augmented_incumbent_update_count = 0;
            return;
        }
    }

    /*************************************************************************/
    inline constexpr void update_number_of_neighborhoods(
        const std::vector<TabuSearchCoreMoveScore>& a_TRIAL_MOVE_SCORES,
        const std::vector<solution::SolutionScore>& a_TRIAL_SOLUTION_SCORES) {
        m_state.number_of_all_neighborhoods = m_state.number_of_moves;
        if (m_state.iteration %
                    std::max(m_option.tabu_search.log_interval, 1) ==
                0 ||
            m_state.update_status > 0) {
            m_state.number_of_feasible_neighborhoods    = 0;
            m_state.number_of_permissible_neighborhoods = 0;
            m_state.number_of_improvable_neighborhoods  = 0;

            for (const auto& score : a_TRIAL_SOLUTION_SCORES) {
                if (score.is_feasible) {
                    m_state.number_of_feasible_neighborhoods++;
                }
                if (score.is_objective_improvable ||
                    score.is_feasibility_improvable) {
                    m_state.number_of_improvable_neighborhoods++;
                }
            }

            for (const auto& score : a_TRIAL_MOVE_SCORES) {
                if (score.is_permissible) {
                    m_state.number_of_permissible_neighborhoods++;
                }
            }

            if (m_state.number_of_permissible_neighborhoods == 0) {
                m_state.is_few_permissible_neighborhood = true;
            }
        } else {
            bool is_few_permissible_neighborhood_temp = true;
            for (const auto& score : a_TRIAL_MOVE_SCORES) {
                if (score.is_permissible) {
                    is_few_permissible_neighborhood_temp = false;
                    break;
                }
            }
            if (is_few_permissible_neighborhood_temp) {
                m_state.is_few_permissible_neighborhood = true;
            }
        }
    }

    /*************************************************************************/
    inline constexpr void update_tabu_tenure(void) {
        if ((m_state.update_status &
             solution::IncumbentHolderConstant::
                 STATUS_GLOBAL_AUGMENTED_INCUMBENT_UPDATE) &&
            m_state.tabu_tenure > m_state.original_tabu_tenure) {
            /**
             * The tabu tenure will be reverted to the original value if it
             * has been increased and the global incumbent is updated.
             */
            m_state.tabu_tenure = m_state.original_tabu_tenure;
            m_state.last_tabu_tenure_updated_iteration = m_state.iteration;
            m_state.intensity_decrease_count           = 0;
            m_state.intensity_increase_count           = 0;
            utility::print_debug(
                "Tabu tenure reverted: " + std::to_string(m_state.tabu_tenure) +
                    ".",
                m_option.output.verbose >= option::verbose::Debug);
        } else if ((m_state.iteration -
                    m_state.last_tabu_tenure_updated_iteration) %
                       (m_state.tabu_tenure + 1) ==
                   0) {
            /**
             * The intensity of searching will be computed with the interval
             * of tabu_tenure+1. The tabu tenure will be increased if the
             * intensity has grown up, and decreased if the intensity has
             * been reduced.
             */
            m_state.previous_primal_intensity =
                m_state.current_primal_intensity;
            m_state.current_primal_intensity = m_memory_ptr->primal_intensity();

            if (m_state.current_primal_intensity >
                m_state.previous_primal_intensity) {
                m_state.intensity_increase_count++;
                m_state.intensity_decrease_count = 0;

                if (m_state.intensity_increase_count >
                    m_option.tabu_search.intensity_increase_count_threshold) {
                    m_state.intensity_increase_count = 0;
                    m_state.tabu_tenure =
                        std::min(m_state.tabu_tenure + 1,
                                 m_model_ptr->number_of_mutable_variables());
                    m_state.last_tabu_tenure_updated_iteration =
                        m_state.iteration;
                    utility::print_debug(
                        "Tabu tenure increased: " +
                            std::to_string(m_state.tabu_tenure) + ".",
                        m_option.output.verbose >= option::verbose::Debug);
                }
            } else {
                m_state.intensity_decrease_count++;
                m_state.intensity_increase_count = 0;

                if (m_state.intensity_decrease_count >
                    m_option.tabu_search.intensity_decrease_count_threshold) {
                    m_state.intensity_decrease_count = 0;
                    m_state.tabu_tenure =
                        std::max(m_state.tabu_tenure - 1,
                                 std::max(1, m_state.original_tabu_tenure / 2));
                    m_state.last_tabu_tenure_updated_iteration =
                        m_state.iteration;

                    utility::print_debug(
                        "Tabu tenure decreased: " +
                            std::to_string(m_state.tabu_tenure) + ".",
                        m_option.output.verbose >= option::verbose::Debug);
                }
            }
        }
    }

    /*************************************************************************/
    inline constexpr void set_number_of_moves(const int a_NUMBER_OF_MOVES) {
        m_state.number_of_moves = a_NUMBER_OF_MOVES;
    }

    /*************************************************************************/
    inline constexpr void set_termination_status(
        const TabuSearchCoreTerminationStatus a_TERMINATION_STATUS) {
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
    inline constexpr TabuSearchCoreState<T_Variable, T_Expression>& state(
        void) {
        return m_state;
    }

    /*************************************************************************/
    inline constexpr const TabuSearchCoreState<T_Variable, T_Expression>& state(
        void) const {
        return m_state;
    }
};
}  // namespace printemps::solver::tabu_search::core
#endif
/*****************************************************************************/
// END
/*****************************************************************************/