/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_OPB_OPB_OBJECTIVE_H__
#define PRINTEMPS_OPB_OPB_OBJECTIVE_H__

#include "opb_term.h"

namespace printemps::opb {
/*****************************************************************************/
struct OPBObjective {
    bool                 is_minimization;
    std::string          name;
    std::vector<OPBTerm> terms;

    /*************************************************************************/
    OPBObjective(void) {
        this->initialize();
    }

    /*************************************************************************/
    inline void initialize(void) {
        this->is_minimization = true;
        this->name            = "";
        this->terms.clear();
    }
};
}  // namespace printemps::opb
#endif
/*****************************************************************************/
// END
/*****************************************************************************/