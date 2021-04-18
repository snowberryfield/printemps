/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLUTION_PLAIN_SOLUTION_H__
#define PRINTEMPS_SOLUTION_PLAIN_SOLUTION_H__

namespace printemps {
namespace model {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Model;
}  // namespace model
}  // namespace printemps

namespace printemps {
namespace solution {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct PlainSolution {
    friend class model::Model<T_Variable, T_Expression>;

    T_Expression                                objective;
    T_Expression                                total_violation;
    bool                                        is_feasible;
    std::unordered_map<std::string, T_Variable> variables;

    /*************************************************************************/
    PlainSolution(void) : objective(0), total_violation(0), is_feasible(false) {
        this->variables.clear();
    }
};
using IPPlainSolution = PlainSolution<int, double>;
}  // namespace solution
}  // namespace printemps
#endif
/*****************************************************************************/
// END
/*****************************************************************************/