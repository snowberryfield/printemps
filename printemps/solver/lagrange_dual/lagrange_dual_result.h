/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_LAGRANGE_DUAL_LAGRANGE_DUAL_RESULT_H__
#define PRINTEMPS_SOLVER_LAGRANGE_DUAL_LAGRANGE_DUAL_RESULT_H__

namespace printemps {
namespace solver {
namespace lagrange_dual {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct LagrangeDualResult {
    double                                              lagrangian;
    solution::Solution<T_Variable, T_Expression>        primal_solution;
    std::vector<multi_array::ValueProxy<double>>        dual_value_proxies;
    solution::IncumbentHolder<T_Variable, T_Expression> incumbent_holder;
    int                                                 total_update_status;
    int                                                 number_of_iterations;
    LagrangeDualTerminationStatus                       termination_status;

    std::vector<solution::PlainSolution<T_Variable, T_Expression>>
        historical_feasible_solutions;

    /*************************************************************************/
    LagrangeDualResult(void) {
        this->initialize();
    }

    /*************************************************************************/
    virtual ~LagrangeDualResult(void) {
        /// nothing to do
    }

    /*************************************************************************/
    void initialize(void) {
        this->lagrangian = -HUGE_VALF;
        this->primal_solution.initialize();
        this->dual_value_proxies.clear();
        this->incumbent_holder.initialize();
        this->total_update_status  = 0;
        this->number_of_iterations = 0;
        this->termination_status =
            LagrangeDualTerminationStatus::ITERATION_OVER;
        this->historical_feasible_solutions.clear();
    }
};
}  // namespace lagrange_dual
}  // namespace solver
}  // namespace printemps

#endif
/*****************************************************************************/
// END
/*****************************************************************************/