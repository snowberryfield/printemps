/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef CPPMH_MODEL_PLAIN_SOLUTION_H__
#define CPPMH_MODEL_PLAIN_SOLUTION_H__

#include <vector>

namespace cppmh {
namespace model {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Model;

/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct PlainSolution {
    friend class Model<T_Variable, T_Expression>;

    T_Expression            objective;
    bool                    is_feasible;
    std::vector<T_Variable> variable_values;

    /*************************************************************************/
    PlainSolution(void) : objective(0), is_feasible(false) {
        this->initialize();
    }

    /*************************************************************************/
    virtual ~PlainSolution(void) {
        /// nothing to do
    }

    /*************************************************************************/
    inline void initialize(void) {
        this->variable_values.clear();

        this->objective   = 0;
        this->is_feasible = false;
    }
};
using IPPlainSolution = PlainSolution<int, double>;
}  // namespace model
}  // namespace cppmh
#endif
/*****************************************************************************/
// END
/*****************************************************************************/