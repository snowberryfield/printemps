/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MODEL_SOLUTION_H__
#define PRINTEMPS_MODEL_SOLUTION_H__

#include <vector>

namespace printemps {
namespace model {
/*****************************************************************************/
template <class T_Value>
class ValueProxy;

/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Model;

/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct Solution {
    std::vector<ValueProxy<T_Variable>>   variable_value_proxies;
    std::vector<ValueProxy<T_Expression>> expression_value_proxies;
    std::vector<ValueProxy<T_Expression>> constraint_value_proxies;
    std::vector<ValueProxy<T_Expression>> violation_value_proxies;

    T_Expression objective;
    T_Expression total_violation;
    bool         is_feasible;

    /*************************************************************************/
    Solution(void) : objective(0), total_violation(0), is_feasible(false) {
        this->initialize();
    }

    /*************************************************************************/
    virtual ~Solution(void) {
        /// nothing to do
    }

    /*************************************************************************/
    inline void initialize(void) {
        this->variable_value_proxies.clear();
        this->expression_value_proxies.clear();
        this->constraint_value_proxies.clear();
        this->violation_value_proxies.clear();

        this->objective       = 0;
        this->total_violation = 0;
        this->is_feasible     = false;
    }
};
using IPSolution = Solution<int, double>;
}  // namespace model
}  // namespace printemps
#endif
/*****************************************************************************/
// END
/*****************************************************************************/