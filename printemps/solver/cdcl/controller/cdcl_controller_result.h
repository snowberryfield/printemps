/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
// CDCLControllerResult struct definition
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_CDCL_CONTROLLER_CDCL_CONTROLLER_RESULT_H__
#define PRINTEMPS_SOLVER_CDCL_CONTROLLER_CDCL_CONTROLLER_RESULT_H__

#include "../core/cdcl_core_result.h"

namespace printemps::solver::cdcl::controller {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct CDCLControllerResult {
    core::CDCLCoreResult core;

    /*************************************************************************/
    CDCLControllerResult(void) {
        this->initialize();
    }

    /*************************************************************************/
    CDCLControllerResult(
        const core::CDCLCoreResult &a_CORE_RESULT)
        : core(a_CORE_RESULT) {
        /// nothing to do
    }

    /*************************************************************************/
    inline void initialize(void) {
        this->core.initialize();
    }
};
}  // namespace printemps::solver::cdcl::controller

#endif
/*****************************************************************************/
// END
/*****************************************************************************/
