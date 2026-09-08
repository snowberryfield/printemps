/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_CDCL_CORE_CDCL_ITERATION_EVENT_TYPE_H__
#define PRINTEMPS_SOLVER_CDCL_CORE_CDCL_ITERATION_EVENT_TYPE_H__

namespace printemps::solver::cdcl::core {
/*****************************************************************************/
enum class CDCLIterationEventType {
    None,
    Backjump,
    Restart,
};

/*****************************************************************************/
static const std::unordered_map<std::string, CDCLIterationEventType>
    CDCLIterationEventTypeMap = {
        {"None", CDCLIterationEventType::None},
        {"Backjump", CDCLIterationEventType::Backjump},
        {"Restart", CDCLIterationEventType::Restart},
};

/*****************************************************************************/
static const std::unordered_map<CDCLIterationEventType, std::string>
    CDCLIterationEventTypeInverseMap = {
        {CDCLIterationEventType::None, "None"},
        {CDCLIterationEventType::Backjump, "Backjump"},
        {CDCLIterationEventType::Restart, "Restart"},
};

}  // namespace printemps::solver::cdcl::core

#endif
/*****************************************************************************/
// END
/*****************************************************************************/
