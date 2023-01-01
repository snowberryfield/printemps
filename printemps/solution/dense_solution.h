/*****************************************************************************/
// Copyright (c) 2020-2023 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLUTION_DENSE_SOLUTION_H__
#define PRINTEMPS_SOLUTION_DENSE_SOLUTION_H__

namespace printemps::solution {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct DenseSolution {
    std::vector<multi_array::ValueProxy<T_Variable>>   variable_value_proxies;
    std::vector<multi_array::ValueProxy<T_Expression>> expression_value_proxies;
    std::vector<multi_array::ValueProxy<T_Expression>> constraint_value_proxies;
    std::vector<multi_array::ValueProxy<T_Expression>> violation_value_proxies;

    T_Expression objective;
    T_Expression total_violation;
    bool         is_feasible;

    /*************************************************************************/
    DenseSolution(void) {
        this->initialize();
    }

    /*************************************************************************/
    void initialize(void) {
        this->variable_value_proxies.clear();
        this->expression_value_proxies.clear();
        this->constraint_value_proxies.clear();
        this->violation_value_proxies.clear();

        this->objective       = 0;
        this->total_violation = 0;
        this->is_feasible     = false;
    }
};
using IPDenseSolution = DenseSolution<int, double>;
}  // namespace printemps::solution
#endif
/*****************************************************************************/
// END
/*****************************************************************************/