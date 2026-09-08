/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_TERMINATION_STATUS_H__
#define PRINTEMPS_SOLVER_TERMINATION_STATUS_H__

#include <string>
#include <unordered_map>

namespace printemps::solver {
/*****************************************************************************/
enum class TerminationStatus {
    UNKNOWN,
    OPTIMAL,
    FEASIBLE,
    INFEASIBLE,
    TIME_OVER,
    ITERATION_OVER,
    INTERRUPTION
};

/*****************************************************************************/
static std::unordered_map<std::string, TerminationStatus>
    TerminationStatusMap = {  //
        {"UNKNOWN", TerminationStatus::UNKNOWN},
        {"OPTIMAL", TerminationStatus::OPTIMAL},
        {"FEASIBLE", TerminationStatus::FEASIBLE},
        {"INFEASIBLE", TerminationStatus::INFEASIBLE},
        {"TIME_OVER", TerminationStatus::TIME_OVER},
        {"ITERATION_OVER", TerminationStatus::ITERATION_OVER},
        {"INTERRUPTION", TerminationStatus::INTERRUPTION}};

/*****************************************************************************/
static std::unordered_map<TerminationStatus, std::string>
    TerminationStatusInverseMap = {  //
        {TerminationStatus::UNKNOWN, "UNKNOWN"},
        {TerminationStatus::OPTIMAL, "OPTIMAL"},
        {TerminationStatus::FEASIBLE, "FEASIBLE"},
        {TerminationStatus::INFEASIBLE, "INFEASIBLE"},
        {TerminationStatus::TIME_OVER, "TIME_OVER"},
        {TerminationStatus::ITERATION_OVER, "ITERATION_OVER"},
        {TerminationStatus::INTERRUPTION, "INTERRUPTION"}};

}  // namespace printemps::solver

#endif
/*****************************************************************************/
// END
/*****************************************************************************/
