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
}  // namespace printemps::solver::lagrange_dual::core

#endif
/*****************************************************************************/
// END
/*****************************************************************************/