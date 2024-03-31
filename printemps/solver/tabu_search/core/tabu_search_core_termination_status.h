/*****************************************************************************/
// Copyright (c) 2020-2024 Yuji KOGUMA
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

/*****************************************************************************/
static std::unordered_map<std::string, TabuSearchCoreTerminationStatus>
    TabuSearchCoreTerminationStatusMap = {  //
        {"TIME_OVER", TabuSearchCoreTerminationStatus::TIME_OVER},
        {"ITERATION_OVER", TabuSearchCoreTerminationStatus::ITERATION_OVER},
        {"NO_MOVE", TabuSearchCoreTerminationStatus::NO_MOVE},
        {"REACH_TARGET", TabuSearchCoreTerminationStatus::REACH_TARGET},
        {"EARLY_STOP", TabuSearchCoreTerminationStatus::EARLY_STOP},
        {"OPTIMAL", TabuSearchCoreTerminationStatus::OPTIMAL},
        {"PENALTY_COEFFICIENT_TOO_LARGE",
         TabuSearchCoreTerminationStatus::PENALTY_COEFFICIENT_TOO_LARGE}};

/*****************************************************************************/
static std::unordered_map<TabuSearchCoreTerminationStatus, std::string>
    TabuSearchCoreTerminationStatusInverseMap = {  //
        {TabuSearchCoreTerminationStatus::TIME_OVER, "TIME_OVER"},
        {TabuSearchCoreTerminationStatus::ITERATION_OVER, "ITERATION_OVER"},
        {TabuSearchCoreTerminationStatus::NO_MOVE, "NO_MOVE"},
        {TabuSearchCoreTerminationStatus::REACH_TARGET, "REACH_TARGET"},
        {TabuSearchCoreTerminationStatus::EARLY_STOP, "EARLY_STOP"},
        {TabuSearchCoreTerminationStatus::OPTIMAL, "OPTIMAL"},
        {TabuSearchCoreTerminationStatus::PENALTY_COEFFICIENT_TOO_LARGE,
         "PENALTY_COEFFICIENT_TOO_LARGE"}};

}  // namespace printemps::solver::tabu_search::core
#endif
/*****************************************************************************/
// END
/*****************************************************************************/