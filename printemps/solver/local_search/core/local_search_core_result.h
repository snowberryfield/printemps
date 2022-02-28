/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_LOCAL_SEARCH_CORE_LOCAL_SEARCH_CORE_RESULT_H__
#define PRINTEMPS_SOLVER_LOCAL_SEARCH_CORE_LOCAL_SEARCH_CORE_RESULT_H__

namespace printemps {
namespace solver {
namespace local_search {
namespace core {
/*****************************************************************************/
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
        const int a_TOTAL_UPDATE_STATUS, const int a_NUMBER_OF_ITERATIONS,
        const LocalSearchCoreTerminationStatus a_TERMINATION_STATUS)
        : total_update_status(a_TOTAL_UPDATE_STATUS),
          number_of_iterations(a_NUMBER_OF_ITERATIONS),
          termination_status(a_TERMINATION_STATUS) {
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
}  // namespace core
}  // namespace local_search
}  // namespace solver
}  // namespace printemps

#endif
/*****************************************************************************/
// END
/*****************************************************************************/