/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_LOCAL_SEARCH_CORE_LOCAL_SEARCH_CORE_RESULT_H__
#define PRINTEMPS_SOLVER_LOCAL_SEARCH_CORE_LOCAL_SEARCH_CORE_RESULT_H__

namespace printemps::solver::local_search::core {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct LocalSearchCoreResult {
    int total_update_status;
    int number_of_iterations;

    LocalSearchCoreTerminationStatus termination_status;

    /*************************************************************************/
    LocalSearchCoreResult(void) {
        this->initialize();
    }

    /*************************************************************************/
    LocalSearchCoreResult(
        const LocalSearchCoreState<T_Variable, T_Expression> &a_STATE)
        : total_update_status(a_STATE.total_update_status),
          number_of_iterations(a_STATE.iteration),
          termination_status(a_STATE.termination_status) {
        /// nothing to do
    }

    /*************************************************************************/
    void initialize(void) {
        this->total_update_status  = 0;
        this->number_of_iterations = 0;
        this->termination_status =
            LocalSearchCoreTerminationStatus::ITERATION_OVER;
    }
};
}  // namespace printemps::solver::local_search::core

#endif
/*****************************************************************************/
// END
/*****************************************************************************/