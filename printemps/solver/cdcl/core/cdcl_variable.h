/*****************************************************************************/
// Copyright (c) 2020-2026 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_CDCL_CORE_CDCL_VARIABLE_H__
#define PRINTEMPS_SOLVER_CDCL_CORE_CDCL_VARIABLE_H__

#include "cdcl_constraint_reference.h"

namespace printemps::solver::cdcl::core {

/*****************************************************************************/
struct CDCLVariable {
    int                     index;
    int                     decision_level;
    CDCLConstraintReference reason_constraint;
    int8_t                  value;
    int8_t                  phase;
    int8_t                  pad;

    /*************************************************************************/
    CDCLVariable(void) noexcept {
        this->initialize();
    }

    /*************************************************************************/
    inline void initialize(void) noexcept {
        this->index                   = CDCLCoreConstant::NO_VARIABLE;
        this->decision_level          = CDCLCoreConstant::UNASSIGNED_LEVEL;
        this->reason_constraint.index = CDCLCoreConstant::NO_CONSTRAINT;
        this->reason_constraint.type  = CDCLConstraintType::NONE;
        this->value = static_cast<int8_t>(CDCLCoreConstant::UNASSIGNED);
        this->phase = static_cast<int8_t>(CDCLCoreConstant::NO_PHASE);
        this->pad   = 0;
    }

    /*************************************************************************/
    inline bool is_assigned(void) const noexcept {
        return this->value != CDCLCoreConstant::UNASSIGNED;
    }

    /*************************************************************************/
    inline void assign(const int a_VALUE, const int a_DECISION_LEVEL,
                       const CDCLConstraintReference& a_REASON) noexcept {
        this->value             = static_cast<int8_t>(a_VALUE);
        this->decision_level    = a_DECISION_LEVEL;
        this->phase             = static_cast<int8_t>(a_VALUE);
        this->reason_constraint = a_REASON;
    }

    /*************************************************************************/
    inline void remove(void) noexcept {
        this->value = static_cast<int8_t>(CDCLCoreConstant::UNASSIGNED);
        this->decision_level          = CDCLCoreConstant::UNASSIGNED_LEVEL;
        this->reason_constraint.index = CDCLCoreConstant::NO_CONSTRAINT;
        this->reason_constraint.type  = CDCLConstraintType::NONE;

        /**
         * NOTE: The phase is not reset to preserve the last assigned value for
         * phase saving heuristics.
         */
    }
};
static_assert(sizeof(CDCLVariable) == 16,
              "CDCLVariable must be exactly 16 bytes");
}  // namespace printemps::solver::cdcl::core
#endif
/*****************************************************************************/
// END
/*****************************************************************************/
