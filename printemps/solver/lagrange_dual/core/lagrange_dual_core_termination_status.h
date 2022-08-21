/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_LAGRANGE_DUAL_CORE_LAGRANGE_DUAL_CORE_TERMINATION_STATUS_H__
#define PRINTEMPS_SOLVER_LAGRANGE_DUAL_CORE_LAGRANGE_DUAL_CORE_TERMINATION_STATUS_H__

namespace printemps::solver::lagrange_dual::core {
/*****************************************************************************/
enum class LagrangeDualCoreTerminationStatus {
    TIME_OVER,
    ITERATION_OVER,
    CONVERGE,
    REACH_TARGET
};

/*****************************************************************************/
static std::unordered_map<std::string, LagrangeDualCoreTerminationStatus>
    LagrangeDualCoreTerminationStatusMap = {  //
        {"TIME_OVER", LagrangeDualCoreTerminationStatus::TIME_OVER},
        {"ITERATION_OVER", LagrangeDualCoreTerminationStatus::ITERATION_OVER},
        {"CONVERGE", LagrangeDualCoreTerminationStatus::CONVERGE},
        {"REACH_TARGET", LagrangeDualCoreTerminationStatus::REACH_TARGET}};

/*****************************************************************************/
static std::unordered_map<LagrangeDualCoreTerminationStatus, std::string>
    LagrangeDualCoreTerminationStatusInverseMap = {  //
        {LagrangeDualCoreTerminationStatus::TIME_OVER, "TIME_OVER"},
        {LagrangeDualCoreTerminationStatus::ITERATION_OVER, "ITERATION_OVER"},
        {LagrangeDualCoreTerminationStatus::CONVERGE, "CONVERGE"},
        {LagrangeDualCoreTerminationStatus::REACH_TARGET, "REACH_TARGET"}};

}  // namespace printemps::solver::lagrange_dual::core

#endif
/*****************************************************************************/
// END
/*****************************************************************************/