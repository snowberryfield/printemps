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
struct TabuSearchControllerResult {
    int number_of_iterations;
    int number_of_loops;
    int update_status;

    /*************************************************************************/
    TabuSearchControllerResult(void) {
        this->initialize();
    }

    /*************************************************************************/
    TabuSearchControllerResult(const int a_NUMBER_OF_ITERATIONS,
                               const int a_NUMBER_OF_LOOPS,
                               const int a_UPDATE_STATUS)
        : number_of_iterations(a_NUMBER_OF_ITERATIONS),
          number_of_loops(a_NUMBER_OF_LOOPS),
          update_status(a_UPDATE_STATUS) {
        /// nothing to do
    }

    /*************************************************************************/
    inline void initialize(void) {
        this->number_of_iterations = 0;
        this->number_of_loops      = 0;
        this->update_status =
            solution::IncumbentHolderConstant::STATUS_NO_UPDATED;
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