/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_PB_TOP_COST_H__
#define PRINTEMPS_PB_TOP_COST_H__

namespace printemps::pb {
/*****************************************************************************/
struct PBTopCost {
    bool is_defined;
    int  value;

    /*************************************************************************/
    PBTopCost(void) {
        this->initialize();
    }

    /*************************************************************************/
    inline void initialize(void) {
        this->is_defined = false;
        this->value      = std::numeric_limits<int>::max();
    }
};
}  // namespace printemps::pb
#endif
/*****************************************************************************/
// END
/*****************************************************************************/