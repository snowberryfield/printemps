/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef printemps_SOLVER_LAGRANGE_DUAL_LAGRANGE_DUAL_RESULT_H__
#define printemps_SOLVER_LAGRANGE_DUAL_LAGRANGE_DUAL_RESULT_H__

namespace printemps {
namespace solver {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class IncumbentHolder;

/*****************************************************************************/
namespace lagrange_dual {
enum class LagrangeDualTerminationStatus {
    TIME_OVER,
    ITERATION_OVER,
    CONVERGE,
    REACH_TARGET
};

/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct LagrangeDualResult {
    double                                    lagrangian;
    model::Solution<T_Variable, T_Expression> primal_solution;
    std::vector<model::ValueProxy<double>>    dual_value_proxies;
    IncumbentHolder<T_Variable, T_Expression> incumbent_holder;
    int                                       total_update_status;
    int                                       number_of_iterations;
    LagrangeDualTerminationStatus             termination_status;

    std::vector<model::PlainSolution<T_Variable, T_Expression>>
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
    inline void constexpr initialize(void) {
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