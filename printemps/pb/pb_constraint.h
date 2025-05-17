/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_PB_PB_CONSTRAINT_H__
#define PRINTEMPS_PB_PB_CONSTRAINT_H__

#include "pb_constraint_sense.h"
#include "pb_term.h"

namespace printemps::pb {
/*****************************************************************************/
struct PBConstraint {
    int                 weight;
    PBConstraintSense   sense;
    std::string         name;
    std::vector<PBTerm> terms;
    int                 rhs;

    /*************************************************************************/
    PBConstraint(void) {
        this->initialize();
    }

    /*************************************************************************/
    inline void initialize(void) {
        this->weight = std::numeric_limits<int>::max();
        this->sense  = PBConstraintSense::Less;
        this->name   = "";
        this->terms.clear();
        this->rhs = 0.0;
    }

    /*************************************************************************/
    inline bool is_all_coefficient_negative(void) const {
        for (const auto &term : this->terms) {
            if (term.coefficient > 0) {
                return false;
            }
        }
        return true;
    }
};
}  // namespace printemps::pb
#endif
/*****************************************************************************/
// END
/*****************************************************************************/