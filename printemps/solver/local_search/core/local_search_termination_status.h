/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_LOCAL_SEARCH_CORE_LOCAL_SEARCH_TERMINATION_STATUS_H__
#define PRINTEMPS_SOLVER_LOCAL_SEARCH_CORE_LOCAL_SEARCH_TERMINATION_STATUS_H__

namespace printemps {
namespace solver {
namespace local_search {
namespace core {
/*****************************************************************************/
enum class LocalSearchTerminationStatus {
    TIME_OVER,
    ITERATION_OVER,
    NO_MOVE,
    REACH_TARGET,
    LOCAL_OPTIMAL
};
}  // namespace core
}  // namespace local_search
}  // namespace solver
}  // namespace printemps

#endif
/*****************************************************************************/
// END
/*****************************************************************************/