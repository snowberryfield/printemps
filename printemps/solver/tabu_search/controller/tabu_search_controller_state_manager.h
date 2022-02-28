/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_TABU_SEARCH_CONTROLLER_TABU_SEARCH_CONTROLLER_STATE_MANAGER_H__
#define PRINTEMPS_SOLVER_TABU_SEARCH_CONTROLLER_TABU_SEARCH_CONTROLLER_STATE_MANAGER_H__

#include "tabu_search_controller_state.h"

namespace printemps {
namespace solver {
namespace tabu_search {
namespace controller {
/*****************************************************************************/
struct TabuSearchControllerParameter;

/*****************************************************************************/
struct TabuSearchControllerStateManagerConstant {
    static constexpr int STAGNATION_THRESHOLD = 80;
};

/*****************************************************************************/
template <class T_Variable, class T_Expression>
class TabuSearchControllerStateManager {
   private:
    TabuSearchControllerState<T_Variable, T_Expression>  m_state;
    model::Model<T_Variable, T_Expression>*              m_model_ptr;
    solution::IncumbentHolder<T_Variable, T_Expression>* m_incumbent_holder_ptr;
    Memory<T_Variable, T_Expression>*                    m_memory_ptr;
    option::Option                                       m_master_option;

   public:
    /*************************************************************************/
    TabuSearchControllerStateManager(
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
    TabuSearchControllerStateManager(void) {
        this->initialize();
    }

    /*************************************************************************/
    inline void initialize(void) {
        m_state.initialize();
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

        m_state.current_primal_intensity  = m_memory_ptr->primal_intensity();
        m_state.previous_primal_intensity = 0.0;

        m_state.current_dual_intensity  = m_memory_ptr->dual_intensity();
        m_state.previous_dual_intensity = 0.0;
    }

    /*************************************************************************/
    inline constexpr void update(
        const tabu_search::core::TabuSearchCoreResult& a_RESULT) {
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
    }

    /*************************************************************************/
    inline constexpr void update(
        const TabuSearchControllerParameter& a_PARAMETER) {
        /**
         * Update the current solution as the initial solution in the next tabu
         * search loop.
         */
        this->update_current_solution(a_PARAMETER);

        /**
         * Update the various counts about penalty coefficients relaxation.
         */
        this->update_relaxation_status(a_PARAMETER);
    }

    /*************************************************************************/
    inline constexpr void update_last_tabu_search_result(
        const tabu_search::core::TabuSearchCoreResult& a_RESULT) {
        /**
         * Update the incumbent update flags.
         */
        m_state.tabu_search_result = a_RESULT;
        m_state.is_global_augmented_incumbent_updated =
            (m_state.tabu_search_result.total_update_status &
             solution::IncumbentHolderConstant::
                 STATUS_GLOBAL_AUGMENTED_INCUMBENT_UPDATE);

        m_state.is_feasible_incumbent_updated =
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
         * incumbent update is not less than the constant
         * STAGNATION_THRESHOLD.
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
    inline constexpr void update_current_solution(
        const TabuSearchControllerParameter& a_PARAMETER) {
        if (a_PARAMETER.employing_global_augmented_solution_flag) {
            m_state.current_solution =
                m_incumbent_holder_ptr->global_augmented_incumbent_solution();
            m_state.current_solution_score =
                m_incumbent_holder_ptr->global_augmented_incumbent_score();
            m_state
                .employing_global_augmented_solution_count_after_relaxation++;
        } else if (a_PARAMETER.employing_local_augmented_solution_flag) {
            m_state.current_solution =
                m_incumbent_holder_ptr->local_augmented_incumbent_solution();
            m_state.current_solution_score =
                m_incumbent_holder_ptr->local_augmented_incumbent_score();
            m_state.employing_local_augmented_solution_count_after_relaxation++;
        } else if (a_PARAMETER.employing_previous_solution_flag) {
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
    inline constexpr void update_relaxation_status(
        const TabuSearchControllerParameter& a_PARAMETER) {
        if (a_PARAMETER.is_enabled_penalty_coefficient_relaxing) {
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
}  // namespace controller
}  // namespace tabu_search
}  // namespace solver
}  // namespace printemps
#endif
/*****************************************************************************/
// END
/*****************************************************************************/