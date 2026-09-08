/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_CDCL_CORE_CDCL_CORE_TERMINATION_STATUS_H__
#define PRINTEMPS_SOLVER_CDCL_CORE_CDCL_CORE_TERMINATION_STATUS_H__

namespace printemps::solver::cdcl::core {
/*****************************************************************************/
enum class CDCLCoreTerminationStatus {
    INTERRUPTION,
    TIME_OVER,
    ITERATION_OVER,
    CONFLICT_OVER,
    FEASIBLE,
    INFEASIBLE,
    NOT_SOLVED
};

/*****************************************************************************/
const std::unordered_map<std::string, CDCLCoreTerminationStatus>
    CDCLCoreTerminationStatusMap = {
        {"INTERRUPTION", CDCLCoreTerminationStatus::INTERRUPTION},
        {"TIME_OVER", CDCLCoreTerminationStatus::TIME_OVER},
        {"ITERATION_OVER", CDCLCoreTerminationStatus::ITERATION_OVER},
        {"CONFLICT_OVER", CDCLCoreTerminationStatus::CONFLICT_OVER},
        {"FEASIBLE", CDCLCoreTerminationStatus::FEASIBLE},
        {"INFEASIBLE", CDCLCoreTerminationStatus::INFEASIBLE},
        {"NOT_SOLVED", CDCLCoreTerminationStatus::NOT_SOLVED},
};

/*****************************************************************************/
const std::unordered_map<CDCLCoreTerminationStatus, std::string>
    CDCLCoreTerminationStatusInverseMap = {
        {CDCLCoreTerminationStatus::INTERRUPTION, "INTERRUPTION"},
        {CDCLCoreTerminationStatus::TIME_OVER, "TIME_OVER"},
        {CDCLCoreTerminationStatus::ITERATION_OVER, "ITERATION_OVER"},
        {CDCLCoreTerminationStatus::CONFLICT_OVER, "CONFLICT_OVER"},
        {CDCLCoreTerminationStatus::FEASIBLE, "FEASIBLE"},
        {CDCLCoreTerminationStatus::INFEASIBLE, "INFEASIBLE"},
        {CDCLCoreTerminationStatus::NOT_SOLVED, "NOT_SOLVED"},
};
}  // namespace printemps::solver::cdcl::core

#endif
/*****************************************************************************/
// END
/*****************************************************************************/
