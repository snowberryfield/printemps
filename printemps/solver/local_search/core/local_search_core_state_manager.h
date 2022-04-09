/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_LOCAL_SEARCH_CORE_LOCAL_SEARCH_CORE_STATE_MANAGER_H__
#define PRINTEMPS_SOLVER_LOCAL_SEARCH_CORE_LOCAL_SEARCH_CORE_STATE_MANAGER_H__

#include "local_search_core_state.h"

namespace printemps {
namespace solver {
namespace local_search {
namespace core {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class LocalSearchCoreStateManager {
   private:
    LocalSearchCoreState<T_Variable, T_Expression>       m_state;
    model::Model<T_Variable, T_Expression>*              m_model_ptr;
    solution::IncumbentHolder<T_Variable, T_Expression>* m_incumbent_holder_ptr;
    Memory<T_Variable, T_Expression>*                    m_memory_ptr;
    option::Option                                       m_option;

   public:
    /*************************************************************************/
    LocalSearchCoreStateManager(
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
    LocalSearchCoreStateManager(void) {
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
         * NOTE: The values of the other members of m_state remain at their
         * default values.
         */
    }

    /*************************************************************************/
    inline constexpr void update(
        neighborhood::Move<T_Variable, T_Expression>* a_selected_move_ptr,
        const int                      a_NUMBER_OF_PERFORMED_MOVES,
        const bool                     a_IS_FOUND_IMPROVING_SOLUTION,
        const solution::SolutionScore& a_SOLUTION_SCORE) {
        /**
         * Update the current move with keeping the previous one.
         */
        this->update_move(a_selected_move_ptr);

        /**
         * Update the current solution score with keeping the previous one.
         */
        this->update_solution_score(a_SOLUTION_SCORE);

        /**
         * Update the update status.
         */
        this->update_update_status();

        /**
         * Update the number of performed moves.
         */
        this->update_number_of_performed_moves(a_NUMBER_OF_PERFORMED_MOVES);

        /**
         * Update the improving solution found flag.
         */
        this->update_is_found_improving_solution(a_IS_FOUND_IMPROVING_SOLUTION);
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
    inline constexpr void set_number_of_moves(const int a_NUMBER_OF_MOVES) {
        m_state.number_of_moves = a_NUMBER_OF_MOVES;
    }

    /*************************************************************************/
    inline constexpr void update_number_of_performed_moves(
        const int a_NUMBER_OF_PERFORMED_MOVES) {
        m_state.number_of_performed_moves = a_NUMBER_OF_PERFORMED_MOVES;
    }

    /*************************************************************************/
    inline constexpr void update_is_found_improving_solution(
        const bool a_IS_FOUND_IMPROVING_SOLUTION) {
        m_state.is_found_improving_solution = a_IS_FOUND_IMPROVING_SOLUTION;
    }

    /*************************************************************************/
    inline constexpr void set_termination_status(
        const LocalSearchCoreTerminationStatus a_TERMINATION_STATUS) {
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
    inline constexpr LocalSearchCoreState<T_Variable, T_Expression>& state(
        void) {
        return m_state;
    }

    /*************************************************************************/
    inline constexpr const LocalSearchCoreState<T_Variable, T_Expression>&
    state(void) const {
        return m_state;
    }
};
}  // namespace core
}  // namespace local_search
}  // namespace solver
}  // namespace printemps
#endif
/*****************************************************************************/
// END
/*****************************************************************************/