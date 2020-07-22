/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef CPPMH_SOLVER_LAGRANGE_DUAL_LAGRANGE_DUAL_RESULT_H__
#define CPPMH_SOLVER_LAGRANGE_DUAL_LAGRANGE_DUAL_RESULT_H__

namespace cppmh {
namespace solver {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class IncumbentHolder;

/*****************************************************************************/
namespace lagrange_dual {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct LagrangeDualResult {
    double                                    lagrangian;
    model::Solution<T_Variable, T_Expression> primal_solution;
    std::vector<model::ValueProxy<double>>    dual_value_proxies;
    IncumbentHolder<T_Variable, T_Expression> incumbent_holder;
    int                                       total_update_status;
    int                                       number_of_iterations;

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
    }
};
}  // namespace lagrange_dual
}  // namespace solver
}  // namespace cppmh

#endif
/*****************************************************************************/
// END
/*****************************************************************************/