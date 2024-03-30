/*****************************************************************************/
// Copyright (c) 2020-2024 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_LAGRANGE_DUAL_CONTROLLER_LAGRANGE_DUAL_CONTROLLER_RESULT_H__
#define PRINTEMPS_SOLVER_LAGRANGE_DUAL_CONTROLLER_LAGRANGE_DUAL_CONTROLLER_RESULT_H__

#include "../core/lagrange_dual_core_result.h"

namespace printemps::solver::lagrange_dual::controller {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct LagrangeDualControllerResult {
    core::LagrangeDualCoreResult<T_Variable, T_Expression> core;

    /*************************************************************************/
    LagrangeDualControllerResult(void) {
        this->initialize();
    }

    /*************************************************************************/
    LagrangeDualControllerResult(
        const core::LagrangeDualCoreResult<T_Variable, T_Expression>
            &a_CORE_RESULT)
        : core(a_CORE_RESULT) {
        /// nothing to do
    }

    /*************************************************************************/
    inline void initialize(void) {
        this->core.initialize();
    }
};
}  // namespace printemps::solver::lagrange_dual::controller

#endif
/*****************************************************************************/
// END
/*****************************************************************************/