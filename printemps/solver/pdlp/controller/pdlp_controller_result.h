/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_PDLP_CONTROLLER_PDLP_CONTROLLER_RESULT_H__
#define PRINTEMPS_SOLVER_PDLP_CONTROLLER_PDLP_CONTROLLER_RESULT_H__
#include "../core/pdlp_core_result.h"

namespace printemps::solver::pdlp::controller {
/*****************************************************************************/
struct PDLPControllerResult {
    core::PDLPCoreResult core;

    /*************************************************************************/
    PDLPControllerResult(void) {
        this->initialize();
    }

    /*************************************************************************/
    PDLPControllerResult(const core::PDLPCoreResult &a_CORE_RESULT)
        : core(a_CORE_RESULT) {
        /// nothing to do
    }

    /*************************************************************************/
    inline void initialize(void) {
        this->core.initialize();
    }
};
}  // namespace printemps::solver::pdlp::controller

#endif
/*****************************************************************************/
// END
/*****************************************************************************/