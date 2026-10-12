/*****************************************************************************/
// Copyright (c) 2020-2026 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_CDCL_CORE_CDCL_CONSTRAINT_REFERENCE_H__
#define PRINTEMPS_SOLVER_CDCL_CORE_CDCL_CONSTRAINT_REFERENCE_H__

#include "cdcl_core_constants.h"
#include "cdcl_constraint_type.h"

namespace printemps::solver::cdcl::core {

/*****************************************************************************/
#pragma pack(push, 1)
struct CDCLConstraintReference {
    int                index;
    CDCLConstraintType type;

    /*************************************************************************/
    constexpr CDCLConstraintReference(void) noexcept
        : index(CDCLCoreConstant::NO_CONSTRAINT),
          type(CDCLConstraintType::NONE) {
    }

    /*************************************************************************/
    constexpr CDCLConstraintReference(const CDCLConstraintType a_TYPE,
                                      const int                a_INDEX) noexcept
        : index(a_INDEX), type(a_TYPE) {
    }

    /*************************************************************************/
    inline bool operator==(
        const CDCLConstraintReference& a_OTHER) const noexcept {
        return this->index == a_OTHER.index && this->type == a_OTHER.type;
    }

    /*************************************************************************/
    inline bool operator!=(
        const CDCLConstraintReference& a_OTHER) const noexcept {
        return !(*this == a_OTHER);
    }
};
#pragma pack(pop)
static_assert(sizeof(CDCLConstraintReference) == 5,
              "CDCLConstraintReference must be exactly 5 bytes");

}  // namespace printemps::solver::cdcl::core

#endif
/*****************************************************************************/
// END
/*****************************************************************************/
