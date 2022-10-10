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
    int    total_update_status;
    int    number_of_iterations;
    double elapsed_time;

    LocalSearchCoreTerminationStatus termination_status;

    option::Option option;

    /*************************************************************************/
    LocalSearchCoreResult(void) {
        this->initialize();
    }

    /*************************************************************************/
    LocalSearchCoreResult(
        const LocalSearchCoreState<T_Variable, T_Expression> &a_STATE,
        const option::Option &                                a_OPTION)
        : total_update_status(a_STATE.total_update_status),
          number_of_iterations(a_STATE.iteration),
          elapsed_time(a_STATE.elapsed_time),
          termination_status(a_STATE.termination_status),
          option(a_OPTION) {
        /// nothing to do
    }

    /*************************************************************************/
    void initialize(void) {
        this->total_update_status  = 0;
        this->number_of_iterations = 0;
        this->elapsed_time         = 0.0;
        this->termination_status =
            LocalSearchCoreTerminationStatus::ITERATION_OVER;
        this->option.initialize();
    }
};
}  // namespace printemps::solver::local_search::core

#endif
/*****************************************************************************/
// END
/*****************************************************************************/