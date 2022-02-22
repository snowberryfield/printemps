/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_LOCAL_SEARCH_CORE_LOCAL_SEARCH_CONTROLLER_RESULT_H__
#define PRINTEMPS_SOLVER_LOCAL_SEARCH_CORE_LOCAL_SEARCH_CONTROLLER_RESULT_H__

namespace printemps {
namespace solver {
namespace local_search {
namespace controller {
/*****************************************************************************/
struct LocalSearchControllerResult {
    int number_of_iterations;
    int update_status;

    /*************************************************************************/
    LocalSearchControllerResult(void) {
        this->initialize();
    }

    /*************************************************************************/
    LocalSearchControllerResult(const int a_NUMBER_OF_ITERATIONS,
                                const int a_UPDATE_STATUS)
        : number_of_iterations(a_NUMBER_OF_ITERATIONS),
          update_status(a_UPDATE_STATUS) {
        /// nothing to do
    }

    /*************************************************************************/
    inline void initialize(void) {
        this->number_of_iterations = 0;
        this->update_status =
            solution::IncumbentHolderConstant::STATUS_NOT_UPDATED;
    }
};
}  // namespace controller
}  // namespace local_search
}  // namespace solver
}  // namespace printemps

#endif
/*****************************************************************************/
// END
/*****************************************************************************/