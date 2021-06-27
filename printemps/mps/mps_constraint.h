/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MPS_MPS_CONSTRAINT_H__
#define PRINTEMPS_MPS_MPS_CONSTRAINT_H__

#include "mps_constraint_sense.h"

namespace printemps {
namespace mps {
/*****************************************************************************/
struct MPSConstraint {
    MPSConstraintSense                      sense;
    std::string                             name;
    std::unordered_map<std::string, double> sensitivities;
    double                                  rhs;

    /*************************************************************************/
    MPSConstraint(void) {
        this->initialize();
    }

    /*************************************************************************/
    void initialize(void) {
        this->sense = MPSConstraintSense::Less;
        this->name  = "";
        this->sensitivities.clear();
        this->rhs = 0.0;
    }
};
}  // namespace mps
}  // namespace printemps
#endif
/*****************************************************************************/
// END
/*****************************************************************************/