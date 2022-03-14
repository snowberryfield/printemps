/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_TABU_SEARCH_CONTROLLER_TABU_SEARCH_CONTROLLER_RESULT_H__
#define PRINTEMPS_SOLVER_TABU_SEARCH_CONTROLLER_TABU_SEARCH_CONTROLLER_RESULT_H__

namespace printemps {
namespace solver {
namespace tabu_search {
namespace controller {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct TabuSearchControllerResult {
    TabuSearchControllerState<T_Variable, T_Expression> state;
    int                                                 update_status;

    /*************************************************************************/
    TabuSearchControllerResult(void) {
        this->initialize();
    }

    /*************************************************************************/
    TabuSearchControllerResult(
        const TabuSearchControllerState<T_Variable, T_Expression> &a_STATE,
        const int a_UPDATE_STATUS)
        : state(a_STATE), update_status(a_UPDATE_STATUS) {
        /// nothing to do
    }

    /*************************************************************************/
    inline void initialize(void) {
        this->state.initialize();
        this->update_status =
            solution::IncumbentHolderConstant::STATUS_NOT_UPDATED;
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