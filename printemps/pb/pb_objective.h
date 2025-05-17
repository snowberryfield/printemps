/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_PB_PB_OBJECTIVE_H__
#define PRINTEMPS_PB_PB_OBJECTIVE_H__

#include "pb_term.h"

namespace printemps::pb {
/*****************************************************************************/
struct PBObjective {
    bool                is_minimization;
    std::string         name;
    std::vector<PBTerm> terms;

    /*************************************************************************/
    PBObjective(void) {
        this->initialize();
    }

    /*************************************************************************/
    inline void initialize(void) {
        this->is_minimization = true;
        this->name            = "";
        this->terms.clear();
    }
};
}  // namespace printemps::pb
#endif
/*****************************************************************************/
// END
/*****************************************************************************/