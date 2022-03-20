/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_LAGRANGE_DUAL_CORE_LAGRANGE_DUAL_CORE_RESULT_H__
#define PRINTEMPS_SOLVER_LAGRANGE_DUAL_CORE_LAGRANGE_DUAL_CORE_RESULT_H__

namespace printemps {
namespace solver {
namespace lagrange_dual {
namespace core {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct LagrangeDualCoreResult {
    int                                               total_update_status;
    int                                               number_of_iterations;
    LagrangeDualCoreTerminationStatus                 termination_status;
    double                                            lagrangian;
    solution::DenseSolution<T_Variable, T_Expression> primal;
    std::vector<multi_array::ValueProxy<double>>      dual;

    /*************************************************************************/
    LagrangeDualCoreResult(void) {
        this->initialize();
    }

    /*************************************************************************/
    LagrangeDualCoreResult(
        const LagrangeDualCoreState<T_Variable, T_Expression> &a_STATE)
        : total_update_status(a_STATE.total_update_status),
          number_of_iterations(a_STATE.iteration),
          termination_status(a_STATE.termination_status),
          lagrangian(a_STATE.lagrangian),
          primal(a_STATE.primal_incumbent),
          dual(a_STATE.dual_incumbent) {
        /// nothing to do
    }

    /*************************************************************************/
    void initialize(void) {
        this->total_update_status  = 0;
        this->number_of_iterations = 0;
        this->termination_status =
            LagrangeDualCoreTerminationStatus::ITERATION_OVER;
        this->lagrangian = -HUGE_VALF;
        this->primal.initialize();
        this->dual.clear();
    }
};
}  // namespace core
}  // namespace lagrange_dual
}  // namespace solver
}  // namespace printemps

#endif
/*****************************************************************************/
// END
/*****************************************************************************/