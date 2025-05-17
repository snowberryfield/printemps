/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MPS_MPS_OBJECTIVE_H__
#define PRINTEMPS_MPS_MPS_OBJECTIVE_H__

namespace printemps::mps {
/*****************************************************************************/
struct MPSObjective {
    bool                                    is_minimization;
    std::string                             name;
    std::unordered_map<std::string, double> sensitivities;

    /*************************************************************************/
    MPSObjective(void) {
        this->initialize();
    }

    /*************************************************************************/
    inline void initialize(void) {
        this->is_minimization = true;
        this->name            = "";
        this->sensitivities.clear();
    }
};
}  // namespace printemps::mps
#endif
/*****************************************************************************/
// END
/*****************************************************************************/