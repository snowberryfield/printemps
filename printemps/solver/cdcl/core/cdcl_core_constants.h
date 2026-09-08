/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_CDCL_CORE_CDCL_CORE_CONSTANTS_H__
#define PRINTEMPS_SOLVER_CDCL_CORE_CDCL_CORE_CONSTANTS_H__

namespace printemps::solver::cdcl::core {
/*****************************************************************************/
struct CDCLCoreConstant {
    static constexpr int UNASSIGNED       = -1;
    static constexpr int UNASSIGNED_LEVEL = -1;
    static constexpr int DECISION         = -1;
    static constexpr int NO_VARIABLE      = -1;
    static constexpr int NO_CONSTRAINT    = -1;
    static constexpr int NO_CONFLICT      = -1;
    static constexpr int NO_PHASE         = -1;
    static constexpr int NO_VALUE         = -1;
};
}  // namespace printemps::solver::cdcl::core
#endif
/*****************************************************************************/
// END
/*****************************************************************************/
