/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_TABU_SEARCH_CORE_TERMINATION_STATUS_H__
#define PRINTEMPS_SOLVER_TABU_SEARCH_CORE_TERMINATION_STATUS_H__

namespace printemps::solver::tabu_search::core {
/*****************************************************************************/
enum class TabuSearchCoreTerminationStatus {
    TIME_OVER,
    ITERATION_OVER,
    NO_MOVE,
    REACH_TARGET,
    EARLY_STOP,
    OPTIMAL,
    PENALTY_COEFFICIENT_TOO_LARGE
};
}  // namespace printemps::solver::tabu_search::core
#endif
/*****************************************************************************/
// END
/*****************************************************************************/