/*****************************************************************************/
// Copyright (c) 2020-2023 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_PDLP_CONTROLLER_PDLP_CONTROLLER_RESULT_H__
#define PRINTEMPS_SOLVER_PDLP_CONTROLLER_PDLP_CONTROLLER_RESULT_H__

namespace printemps::solver::pdlp::controller {
/*****************************************************************************/
struct PDLPControllerResult {
    int number_of_iterations;

    /*************************************************************************/
    PDLPControllerResult(void) {
        this->initialize();
    }

    /*************************************************************************/
    PDLPControllerResult(const core::PDLPCoreResult &a_RESULT)
        : number_of_iterations(a_RESULT.number_of_iterations) {
        /// nothing to do
    }

    /*************************************************************************/
    inline void initialize(void) {
        this->number_of_iterations = 0;
    }
};
}  // namespace printemps::solver::pdlp::controller

#endif
/*****************************************************************************/
// END
/*****************************************************************************/