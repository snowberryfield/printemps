/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_OPB_OPB_CONSTRAINT_H__
#define PRINTEMPS_OPB_OPB_CONSTRAINT_H__

#include "opb_constraint_sense.h"
#include "opb_term.h"

namespace printemps::opb {
/*****************************************************************************/
struct OPBConstraint {
    int                  weight;
    OPBConstraintSense   sense;
    std::string          name;
    std::vector<OPBTerm> terms;
    int                  rhs;

    /*************************************************************************/
    OPBConstraint(void) {
        this->initialize();
    }

    /*************************************************************************/
    inline void initialize(void) {
        this->weight = std::numeric_limits<int>::max();
        this->sense  = OPBConstraintSense::Less;
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
}  // namespace printemps::opb
#endif
/*****************************************************************************/
// END
/*****************************************************************************/