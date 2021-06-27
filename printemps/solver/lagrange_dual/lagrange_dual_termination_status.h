/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_LAGRANGE_DUAL_LAGRANGE_DUAL_TERMINATION_STATUS_H__
#define PRINTEMPS_SOLVER_LAGRANGE_DUAL_LAGRANGE_DUAL_TERMINATION_STATUS_H__

namespace printemps {
namespace solver {
namespace lagrange_dual {
/*****************************************************************************/
enum class LagrangeDualTerminationStatus {
    TIME_OVER,
    ITERATION_OVER,
    CONVERGE,
    REACH_TARGET
};
}  // namespace lagrange_dual
}  // namespace solver
}  // namespace printemps

#endif
/*****************************************************************************/
// END
/*****************************************************************************/