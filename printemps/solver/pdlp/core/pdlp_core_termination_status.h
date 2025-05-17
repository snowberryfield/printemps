/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_PDLP_CORE_PDLP_TERMINATION_STATUS_H__
#define PRINTEMPS_SOLVER_PDLP_CORE_PDLP_TERMINATION_STATUS_H__

namespace printemps::solver::pdlp::core {
/*****************************************************************************/
enum class PDLPCoreTerminationStatus {
    INTERRUPTION,
    TIME_OVER,
    ITERATION_OVER,
    OPTIMAL,
    INFEASIBLE
};

/*****************************************************************************/
static std::unordered_map<std::string, PDLPCoreTerminationStatus>
    PDLPCoreTerminationStatusMap = {  //
        {"INTERRUPTION", PDLPCoreTerminationStatus::INTERRUPTION},
        {"TIME_OVER", PDLPCoreTerminationStatus::TIME_OVER},
        {"ITERATION_OVER", PDLPCoreTerminationStatus::ITERATION_OVER},
        {"OPTIMAL", PDLPCoreTerminationStatus::OPTIMAL},
        {"INFEASIBLE", PDLPCoreTerminationStatus::INFEASIBLE}};

/*****************************************************************************/
static std::unordered_map<PDLPCoreTerminationStatus, std::string>
    PDLPCoreTerminationStatusInverseMap = {  //
        {PDLPCoreTerminationStatus::INTERRUPTION, "INTERRUPTION"},
        {PDLPCoreTerminationStatus::TIME_OVER, "TIME_OVER"},
        {PDLPCoreTerminationStatus::ITERATION_OVER, "ITERATION_OVER"},
        {PDLPCoreTerminationStatus::OPTIMAL, "OPTIMAL"},
        {PDLPCoreTerminationStatus::INFEASIBLE, "INFEASIBLE"}};

}  // namespace printemps::solver::pdlp::core

#endif
/*****************************************************************************/
// END
/*****************************************************************************/