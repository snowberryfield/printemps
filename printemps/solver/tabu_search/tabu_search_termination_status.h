/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_TABU_SEARCH_TERMINATION_STATUS_H__
#define PRINTEMPS_SOLVER_TABU_SEARCH_TERMINATION_STATUS_H__

namespace printemps {
namespace solver {
namespace tabu_search {
/*****************************************************************************/
enum class TabuSearchTerminationStatus {
    TIME_OVER,
    ITERATION_OVER,
    NO_MOVE,
    REACH_TARGET,
    EARLY_STOP,
    OPTIMAL
};
}  // namespace tabu_search
}  // namespace solver
}  // namespace printemps

#endif
/*****************************************************************************/
// END
/*****************************************************************************/