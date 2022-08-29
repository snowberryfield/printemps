/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_LOCAL_SEARCH_CORE_LOCAL_SEARCH_CORE_TERMINATION_STATUS_H__
#define PRINTEMPS_SOLVER_LOCAL_SEARCH_CORE_LOCAL_SEARCH_CORE_TERMINATION_STATUS_H__

namespace printemps::solver::local_search::core {
/*****************************************************************************/
enum class LocalSearchCoreTerminationStatus {
    TIME_OVER,
    ITERATION_OVER,
    NO_MOVE,
    REACH_TARGET,
    OPTIMAL,
    LOCAL_OPTIMAL
};

/*****************************************************************************/
static std::unordered_map<std::string, LocalSearchCoreTerminationStatus>
    LocalSearchCoreTerminationStatusMap = {  //
        {"TIME_OVER", LocalSearchCoreTerminationStatus::TIME_OVER},
        {"ITERATION_OVER", LocalSearchCoreTerminationStatus::ITERATION_OVER},
        {"NO_MOVE", LocalSearchCoreTerminationStatus::NO_MOVE},
        {"REACH_TARGET", LocalSearchCoreTerminationStatus::REACH_TARGET},
        {"OPTIMAL", LocalSearchCoreTerminationStatus::OPTIMAL},
        {"LOCAL_OPTIMAL", LocalSearchCoreTerminationStatus::LOCAL_OPTIMAL}};

/*****************************************************************************/
static std::unordered_map<LocalSearchCoreTerminationStatus, std::string>
    LocalSearchCoreTerminationStatusInverseMap = {  //
        {LocalSearchCoreTerminationStatus::TIME_OVER, "TIME_OVER"},
        {LocalSearchCoreTerminationStatus::ITERATION_OVER, "ITERATION_OVER"},
        {LocalSearchCoreTerminationStatus::NO_MOVE, "NO_MOVE"},
        {LocalSearchCoreTerminationStatus::REACH_TARGET, "REACH_TARGET"},
        {LocalSearchCoreTerminationStatus::OPTIMAL, "OPTIMAL"},
        {LocalSearchCoreTerminationStatus::LOCAL_OPTIMAL, "LOCAL_OPTIMAL"}};

}  // namespace printemps::solver::local_search::core

#endif
/*****************************************************************************/
// END
/*****************************************************************************/