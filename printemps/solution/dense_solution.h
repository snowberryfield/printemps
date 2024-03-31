/*****************************************************************************/
// Copyright (c) 2020-2024 Yuji KOGUMA
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
    double       global_augmented_objective;
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

        this->objective                  = 0;
        this->total_violation            = 0;
        this->global_augmented_objective = 0.0;
        this->is_feasible                = false;
    }

    /*************************************************************************/
    inline int distance(
        const DenseSolution<T_Variable, T_Expression> &a_SOLUTION) const {
        int       distance     = 0;
        const int PROXIES_SIZE = this->variable_value_proxies.size();

        for (auto i = 0; i < PROXIES_SIZE; i++) {
            const auto &FIRST  = this->variable_value_proxies[i];
            const auto &SECOND = a_SOLUTION.variable_value_proxies[i];
            const int   NUMBER_OF_ELEMENTS = FIRST.number_of_elements();
            for (auto j = 0; j < NUMBER_OF_ELEMENTS; j++) {
                if (FIRST.flat_indexed_values(j) !=
                    SECOND.flat_indexed_values(j)) {
                    distance++;
                }
            }
        }
        return distance;
    }

    /*************************************************************************/
    inline SparseSolution<T_Variable, T_Expression> to_sparse(void) const {
        solution::SparseSolution<T_Variable, T_Expression> sparse_solution;

        /// Decision variables
        for (const auto &proxy : this->variable_value_proxies) {
            const auto &NAMES              = proxy.flat_indexed_names();
            const auto &VALUES             = proxy.flat_indexed_values();
            const int   NUMBER_OF_ELEMENTS = NAMES.size();
            for (auto i = 0; i < NUMBER_OF_ELEMENTS; i++) {
                if (VALUES[i] != 0) {
                    sparse_solution.variables[NAMES[i]] = VALUES[i];
                }
            }
        }

        sparse_solution.objective       = this->objective;
        sparse_solution.total_violation = this->total_violation;
        sparse_solution.global_augmented_objective =
            this->global_augmented_objective;
        sparse_solution.is_feasible = this->is_feasible;

        return sparse_solution;
    }
};
using IPDenseSolution = DenseSolution<int, double>;
}  // namespace printemps::solution
#endif
/*****************************************************************************/
// END
/*****************************************************************************/