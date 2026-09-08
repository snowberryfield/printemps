/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_CDCL_CORE_CDCL_CONSTRAINT_REFERENCE_H__
#define PRINTEMPS_SOLVER_CDCL_CORE_CDCL_CONSTRAINT_REFERENCE_H__

#include "cdcl_constraint_type.h"

namespace printemps::solver::cdcl::core {

/*****************************************************************************/
struct CDCLConstraintReference {
    CDCLConstraintType type;
    int                index;

    /*************************************************************************/
    inline bool operator==(const CDCLConstraintReference& a_OTHER) const {
        return this->type == a_OTHER.type && this->index == a_OTHER.index;
    }

    /*************************************************************************/
    inline bool operator!=(const CDCLConstraintReference& a_OTHER) const {
        return !(*this == a_OTHER);
    }
};

}  // namespace printemps::solver::cdcl::core

#endif
/*****************************************************************************/
// END
/*****************************************************************************/
