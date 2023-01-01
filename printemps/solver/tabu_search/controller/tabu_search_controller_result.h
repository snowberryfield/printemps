/*****************************************************************************/
// Copyright (c) 2020-2023 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_TABU_SEARCH_CONTROLLER_TABU_SEARCH_CONTROLLER_RESULT_H__
#define PRINTEMPS_SOLVER_TABU_SEARCH_CONTROLLER_TABU_SEARCH_CONTROLLER_RESULT_H__

namespace printemps::solver::tabu_search::controller {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct TabuSearchControllerResult {
    int update_status;

    TabuSearchControllerState<T_Variable, T_Expression> state;

    /*************************************************************************/
    TabuSearchControllerResult(void) {
        this->initialize();
    }

    /*************************************************************************/
    TabuSearchControllerResult(
        const TabuSearchControllerState<T_Variable, T_Expression> &a_STATE)
        : update_status(a_STATE.total_update_status), state(a_STATE) {
        /// nothing to do
    }

    /*************************************************************************/
    inline void initialize(void) {
        this->update_status =
            solution::IncumbentHolderConstant::STATUS_NOT_UPDATED;
        this->state.initialize();
    }
};
}  // namespace printemps::solver::tabu_search::controller

#endif
/*****************************************************************************/
// END
/*****************************************************************************/