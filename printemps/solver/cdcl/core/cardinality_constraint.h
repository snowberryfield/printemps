/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_CDCL_CORE_CARDINALITY_CONSTRAINT_H__
#define PRINTEMPS_SOLVER_CDCL_CORE_CARDINALITY_CONSTRAINT_H__

namespace printemps::solver::cdcl::core {
/*****************************************************************************/
struct CardinalityConstraint {
    int index;
    int rhs;
    int current_lhs_sum;
    int unassigned_count;
    int literal_block_distance;

    std::vector<int> positive_variable_indices;
    std::vector<int> negative_variable_indices;

    /*************************************************************************/
    CardinalityConstraint(void) {
        this->initialize();
    }

    /*************************************************************************/
    CardinalityConstraint(const int a_INDEX, const int a_RHS,
                          const std::vector<int>& a_POSITIVE_VARIABLE_INDICES,
                          const std::vector<int>& a_NEGATIVE_VARIABLE_INDICES) {
        this->setup(a_INDEX, a_RHS, a_POSITIVE_VARIABLE_INDICES,
                    a_NEGATIVE_VARIABLE_INDICES);
    }

    /*************************************************************************/
    inline void initialize(void) {
        this->index                  = CDCLCoreConstant::NO_CONSTRAINT;
        this->rhs                    = 0;
        this->current_lhs_sum        = 0;
        this->unassigned_count       = 0;
        this->literal_block_distance = 0;

        this->positive_variable_indices.clear();
        this->negative_variable_indices.clear();
    }

    /*************************************************************************/
    inline void setup(const int a_INDEX, const int a_RHS,
                      const std::vector<int>& a_POSITIVE_VARIABLE_INDICES,
                      const std::vector<int>& a_NEGATIVE_VARIABLE_INDICES) {
        this->initialize();
        this->index = a_INDEX;
        this->rhs   = a_RHS;

        this->positive_variable_indices = a_POSITIVE_VARIABLE_INDICES;
        this->negative_variable_indices = a_NEGATIVE_VARIABLE_INDICES;
        this->unassigned_count =
            static_cast<int>(a_POSITIVE_VARIABLE_INDICES.size() +
                             a_NEGATIVE_VARIABLE_INDICES.size());
    }

    /*************************************************************************/
    inline int max_possible_lhs_sum(void) const noexcept {
        return this->current_lhs_sum + this->unassigned_count;
    }

    /*************************************************************************/
    inline bool is_satisfied(void) const noexcept {
        return this->current_lhs_sum >= this->rhs;
    }

    /*************************************************************************/
    inline void on_variable_assigned(const bool a_CONTRIBUTES_TO_LHS) noexcept {
        this->unassigned_count--;
        this->current_lhs_sum += a_CONTRIBUTES_TO_LHS;
    }

    /*************************************************************************/
    inline void on_variable_removed(const bool a_CONTRIBUTED_TO_LHS) noexcept {
        this->unassigned_count++;
        this->current_lhs_sum -= a_CONTRIBUTED_TO_LHS;
    }
};
}  // namespace printemps::solver::cdcl::core

#endif
/*****************************************************************************/
// END
/*****************************************************************************/