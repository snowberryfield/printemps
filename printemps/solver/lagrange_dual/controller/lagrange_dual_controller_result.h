/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_LAGRANGE_DUAL_CONTROLLER_LAGRANGE_DUAL_CONTROLLER_RESULT_H__
#define PRINTEMPS_SOLVER_LAGRANGE_DUAL_CONTROLLER_LAGRANGE_DUAL_CONTROLLER_RESULT_H__

namespace printemps::solver::lagrange_dual::controller {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct LagrangeDualControllerResult {
    int update_status;
    int number_of_iterations;

    solution::DenseSolution<T_Variable, T_Expression> primal;
    std::vector<multi_array::ValueProxy<double>>      dual;

    /*************************************************************************/
    LagrangeDualControllerResult(void) {
        this->initialize();
    }

    /*************************************************************************/
    LagrangeDualControllerResult(
        const core::LagrangeDualCoreResult<T_Variable, T_Expression> &a_RESULT)
        : update_status(a_RESULT.total_update_status),
          number_of_iterations(a_RESULT.number_of_iterations),
          primal(a_RESULT.primal),
          dual(a_RESULT.dual) {
        /// nothing to do
    }

    /*************************************************************************/
    inline void initialize(void) {
        this->update_status =
            solution::IncumbentHolderConstant::STATUS_NOT_UPDATED;
        this->number_of_iterations = 0;
    }
};
}  // namespace printemps::solver::lagrange_dual::controller

#endif
/*****************************************************************************/
// END
/*****************************************************************************/