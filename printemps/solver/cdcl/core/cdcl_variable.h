/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_CDCL_CORE_CDCL_VARIABLE_H__
#define PRINTEMPS_SOLVER_CDCL_CORE_CDCL_VARIABLE_H__

#include "cdcl_constraint_reference.h"

namespace printemps::solver::cdcl::core {

/*****************************************************************************/
struct CDCLVariable {
    int index;
    int value;
    int decision_level;
    int phase;

    CDCLConstraintReference reason_constraint;

    std::vector<int> related_cardinality_positive_constraint_indices;
    std::vector<int> related_cardinality_negative_constraint_indices;
    std::vector<int> related_learned_positive_constraint_indices;
    std::vector<int> related_learned_negative_constraint_indices;

    /*************************************************************************/
    CDCLVariable(void) {
        this->initialize();
    }

    /*************************************************************************/
    inline void initialize(void) {
        this->index          = CDCLCoreConstant::NO_VARIABLE;
        this->value          = CDCLCoreConstant::UNASSIGNED;
        this->decision_level = CDCLCoreConstant::UNASSIGNED_LEVEL;
        this->phase          = CDCLCoreConstant::NO_PHASE;

        this->reason_constraint.type  = CDCLConstraintType::NONE;
        this->reason_constraint.index = CDCLCoreConstant::NO_CONSTRAINT;

        this->related_cardinality_positive_constraint_indices.clear();
        this->related_cardinality_negative_constraint_indices.clear();
        this->related_learned_positive_constraint_indices.clear();
        this->related_learned_negative_constraint_indices.clear();
    }

    /*************************************************************************/
    inline bool is_assigned(void) const noexcept {
        return this->value != CDCLCoreConstant::UNASSIGNED;
    }

    /*************************************************************************/
    inline void assign(const int a_VALUE, const int a_DECISION_LEVEL,
                       const CDCLConstraintReference& a_REASON) noexcept {
        this->value             = a_VALUE;
        this->decision_level    = a_DECISION_LEVEL;
        this->phase             = a_VALUE;
        this->reason_constraint = a_REASON;
    }

    /*************************************************************************/
    inline void remove(void) noexcept {
        this->value                   = CDCLCoreConstant::UNASSIGNED;
        this->decision_level          = CDCLCoreConstant::UNASSIGNED_LEVEL;
        this->reason_constraint.type  = CDCLConstraintType::NONE;
        this->reason_constraint.index = CDCLCoreConstant::NO_CONSTRAINT;

        /**
         * NOTE: The phase is not reset to preserve the last assigned value for
         * phase saving heuristics.
         */
    }
};
}  // namespace printemps::solver::cdcl::core
#endif
/*****************************************************************************/
// END
/*****************************************************************************/
