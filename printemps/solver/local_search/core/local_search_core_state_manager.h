/*****************************************************************************/
// Copyright (c) 2020-2024 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_LOCAL_SEARCH_CORE_LOCAL_SEARCH_CORE_STATE_MANAGER_H__
#define PRINTEMPS_SOLVER_LOCAL_SEARCH_CORE_LOCAL_SEARCH_CORE_STATE_MANAGER_H__

#include "local_search_core_state.h"

namespace printemps::solver::local_search::core {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class LocalSearchCoreStateManager {
   private:
    LocalSearchCoreState<T_Variable, T_Expression> m_state;
    model::Model<T_Variable, T_Expression>*        m_model_ptr;
    GlobalState<T_Variable, T_Expression>*         m_global_state_ptr;
    option::Option                                 m_option;

   public:
    /*************************************************************************/
    LocalSearchCoreStateManager(
        model::Model<T_Variable, T_Expression>* a_model_ptr,         //
        GlobalState<T_Variable, T_Expression>*  a_global_state_ptr,  //
        const option::Option&                   a_OPTION) {
        this->setup(a_model_ptr, a_global_state_ptr, a_OPTION);
    }

    /*************************************************************************/
    LocalSearchCoreStateManager(void) {
        this->initialize();
    }

    /*************************************************************************/
    inline void initialize(void) {
        m_state.initialize();
        m_model_ptr        = nullptr;
        m_global_state_ptr = nullptr;
        m_option.initialize();
    }

    /*************************************************************************/
    inline void setup(
        model::Model<T_Variable, T_Expression>* a_model_ptr,         //
        GlobalState<T_Variable, T_Expression>*  a_global_state_ptr,  //
        const option::Option&                   a_OPTION) {
        this->initialize();
        m_model_ptr        = a_model_ptr;
        m_global_state_ptr = a_global_state_ptr;
        m_option           = a_OPTION;

        /**
         * Evaluate the initial solution score.
         */
        m_state.current_solution_score  = m_model_ptr->evaluate({});
        m_state.previous_solution_score = m_state.current_solution_score;
        m_state.update_status =
            m_global_state_ptr->incumbent_holder.try_update_incumbent(
                m_model_ptr, m_state.current_solution_score);
        m_state.total_update_status =
            solution::IncumbentHolderConstant::STATUS_NOT_UPDATED;

        /**
         * NOTE: The values of the other members of m_state remain at their
         * default values.
         */
    }

    /*************************************************************************/
    inline void update(
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
    inline void update_solution_score(
        const solution::SolutionScore& a_SOLUTION_SCORE) {
        m_state.previous_solution_score = m_state.current_solution_score;
        m_state.current_solution_score  = a_SOLUTION_SCORE;
    }

    /*************************************************************************/
    inline void update_move(
        neighborhood::Move<T_Variable, T_Expression>* a_selected_move_ptr) {
        m_state.previous_move = m_state.current_move;
        m_state.current_move  = *a_selected_move_ptr;
    }

    /*************************************************************************/
    inline void update_update_status(void) {
        m_state.update_status =
            m_global_state_ptr->incumbent_holder.try_update_incumbent(
                m_model_ptr, m_state.current_solution_score);
        m_state.total_update_status =
            m_state.update_status | m_state.total_update_status;
    }

    /*************************************************************************/
    inline void set_number_of_moves(const int a_NUMBER_OF_MOVES) {
        m_state.number_of_moves = a_NUMBER_OF_MOVES;
    }

    /*************************************************************************/
    inline void update_number_of_performed_moves(
        const int a_NUMBER_OF_PERFORMED_MOVES) {
        m_state.number_of_performed_moves = a_NUMBER_OF_PERFORMED_MOVES;
    }

    /*************************************************************************/
    inline void update_is_found_improving_solution(
        const bool a_IS_FOUND_IMPROVING_SOLUTION) {
        m_state.is_found_improving_solution = a_IS_FOUND_IMPROVING_SOLUTION;
    }

    /*************************************************************************/
    inline void set_termination_status(
        const LocalSearchCoreTerminationStatus a_TERMINATION_STATUS) {
        m_state.termination_status = a_TERMINATION_STATUS;
    }

    /*************************************************************************/
    inline void set_elapsed_time(const double a_ELAPSED_TINE) {
        m_state.elapsed_time = a_ELAPSED_TINE;
    }

    /*************************************************************************/
    inline void reset_iteration(void) {
        m_state.iteration = 0;
    }

    /*************************************************************************/
    inline void next_iteration(void) {
        m_state.iteration++;
    }

    /*************************************************************************/
    inline LocalSearchCoreState<T_Variable, T_Expression>& state(void) {
        return m_state;
    }

    /*************************************************************************/
    inline const LocalSearchCoreState<T_Variable, T_Expression>& state(
        void) const {
        return m_state;
    }
};
}  // namespace printemps::solver::local_search::core
#endif
/*****************************************************************************/
// END
/*****************************************************************************/