/*****************************************************************************/
// Copyright (c) 2020-2024 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLUTION_SPARSE_SOLUTION_H__
#define PRINTEMPS_SOLUTION_SPARSE_SOLUTION_H__

namespace printemps::solution {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct SparseSolution {
    friend class model::Model<T_Variable, T_Expression>;

    T_Expression objective;
    T_Expression total_violation;
    double       global_augmented_objective;
    bool         is_feasible;

    std::unordered_map<std::string, T_Variable> variables;

    /*************************************************************************/
    SparseSolution(void) {
        this->initialize();
    }

    /*************************************************************************/
    inline void initialize(void) {
        this->objective                  = 0;
        this->total_violation            = 0;
        this->global_augmented_objective = 0.0;
        this->is_feasible                = false;
        this->variables.clear();
    }

    /*************************************************************************/
    inline int distance(
        const SparseSolution<T_Variable, T_Expression> &a_SOLUTION) const {
        return utility::distance_l0(this->variables, a_SOLUTION.variables);
    }
};
using IPSparseSolution = SparseSolution<int, double>;
}  // namespace printemps::solution
#endif
/*****************************************************************************/
// END
/*****************************************************************************/