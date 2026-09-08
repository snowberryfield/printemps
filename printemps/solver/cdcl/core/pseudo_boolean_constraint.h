/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_CDCL_CORE_PSEUDO_BOOLEAN_CONSTRAINT_H__
#define PRINTEMPS_SOLVER_CDCL_CORE_PSEUDO_BOOLEAN_CONSTRAINT_H__

namespace printemps::solver::cdcl::core {
/*****************************************************************************/
struct PseudoBooleanConstraint {
    int     index;
    int64_t rhs;
    int64_t current_lhs_sum;
    int64_t unassigned_max_contribution;

    int literal_block_distance;

    std::vector<std::pair<int, int64_t>> positive_terms;
    std::vector<std::pair<int, int64_t>> negative_terms;

    std::vector<int> positive_variable_indices;
    std::vector<int> negative_variable_indices;

    /*************************************************************************/
    PseudoBooleanConstraint(void) {
        this->initialize();
    }

    /*************************************************************************/
    PseudoBooleanConstraint(
        const int a_INDEX, const int64_t a_RHS,
        const int64_t a_UNASSIGNED_MAX_CONTRIBUTION,
        const std::vector<std::pair<int, int64_t>>& a_POSITIVE_TERMS,
        const std::vector<std::pair<int, int64_t>>& a_NEGATIVE_TERMS) {
        this->index                       = a_INDEX;
        this->rhs                         = a_RHS;
        this->current_lhs_sum             = 0;
        this->unassigned_max_contribution = a_UNASSIGNED_MAX_CONTRIBUTION;
        this->literal_block_distance      = 0;
        this->positive_terms              = a_POSITIVE_TERMS;
        this->negative_terms              = a_NEGATIVE_TERMS;

        this->positive_variable_indices.reserve(this->positive_terms.size());
        for (const auto& [idx, coeff] : this->positive_terms) {
            this->positive_variable_indices.push_back(idx);
        }

        this->negative_variable_indices.reserve(this->negative_terms.size());
        for (const auto& [idx, coeff] : this->negative_terms) {
            this->negative_variable_indices.push_back(idx);
        }
    }

    /*************************************************************************/
    inline void initialize(void) {
        this->index                       = CDCLCoreConstant::NO_CONSTRAINT;
        this->rhs                         = 0;
        this->current_lhs_sum             = 0;
        this->unassigned_max_contribution = 0;
        this->literal_block_distance      = 0;
        this->positive_terms.clear();
        this->negative_terms.clear();
        this->positive_variable_indices.clear();
        this->negative_variable_indices.clear();
    }

    /*************************************************************************/
    inline int64_t max_possible_lhs_sum(void) const noexcept {
        return this->current_lhs_sum + this->unassigned_max_contribution;
    }

    /*************************************************************************/
    inline bool is_satisfied(void) const noexcept {
        return this->current_lhs_sum >= this->rhs;
    }

    /*************************************************************************/
    inline void on_variable_assigned(const bool    a_CONTRIBUTES_TO_LHS,
                                     const int64_t a_COEFFICIENT) noexcept {
        this->unassigned_max_contribution -= a_COEFFICIENT;
        if (a_CONTRIBUTES_TO_LHS) {
            this->current_lhs_sum += a_COEFFICIENT;
        }
    }

    /*************************************************************************/
    inline void on_variable_removed(const bool    a_CONTRIBUTED_TO_LHS,
                                    const int64_t a_COEFFICIENT) noexcept {
        this->unassigned_max_contribution += a_COEFFICIENT;
        if (a_CONTRIBUTED_TO_LHS) {
            this->current_lhs_sum -= a_COEFFICIENT;
        }
    }
};

}  // namespace printemps::solver::cdcl::core

#endif
/*****************************************************************************/
// END
/*****************************************************************************/
