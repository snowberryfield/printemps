/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MPS_MPS_OBJECTIVE_H__
#define PRINTEMPS_MPS_MPS_OBJECTIVE_H__

#include "mps_objective_sense.h"

namespace printemps {
namespace mps {
/*****************************************************************************/
struct MPSObjective {
    MPSObjectiveSense                       sense;
    std::string                             name;
    std::unordered_map<std::string, double> sensitivities;

    /*************************************************************************/
    MPSObjective(void) {
        this->initialize();
    }

    /*************************************************************************/
    void initialize(void) {
        this->sense = MPSObjectiveSense::Minimize;
        this->name  = "";
        this->sensitivities.clear();
    }
};
}  // namespace mps
}  // namespace printemps
#endif
/*****************************************************************************/
// END
/*****************************************************************************/