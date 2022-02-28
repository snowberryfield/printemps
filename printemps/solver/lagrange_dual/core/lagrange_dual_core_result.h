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
    solution::DenseSolution<T_Variable, T_Expression> primal_solution;
    std::vector<multi_array::ValueProxy<double>>      dual_value_proxies;

    /*************************************************************************/
    LagrangeDualCoreResult(void) {
        this->initialize();
    }

    /*************************************************************************/
    LagrangeDualCoreResult(
        const int a_TOTAL_UPDATE_STATUS, const int a_NUMBER_OF_ITERATIONS,
        const LagrangeDualCoreTerminationStatus a_TERMINATION_STATUS,
        const double                            a_LAGRANGIAN,
        const solution::DenseSolution<T_Variable, T_Expression>
            &a_PRIMAL_SOLUTION,
        const std::vector<multi_array::ValueProxy<double>>
            &a_DUAL_VALUE_PROXIES)
        : total_update_status(a_TOTAL_UPDATE_STATUS),
          number_of_iterations(a_NUMBER_OF_ITERATIONS),
          termination_status(a_TERMINATION_STATUS),
          lagrangian(a_LAGRANGIAN),
          primal_solution(a_PRIMAL_SOLUTION),
          dual_value_proxies(a_DUAL_VALUE_PROXIES) {
        /// nothing to do
    }

    /*************************************************************************/
    void initialize(void) {
        this->total_update_status  = 0;
        this->number_of_iterations = 0;
        this->termination_status =
            LagrangeDualCoreTerminationStatus::ITERATION_OVER;
        this->lagrangian = -HUGE_VALF;
        this->primal_solution.initialize();
        this->dual_value_proxies.clear();
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