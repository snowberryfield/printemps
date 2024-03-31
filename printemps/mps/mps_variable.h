/*****************************************************************************/
// Copyright (c) 2020-2024 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MPS_MPS_VARIABLE_H__
#define PRINTEMPS_MPS_MPS_VARIABLE_H__

#include "mps_variable_sense.h"

namespace printemps::mps {
/*****************************************************************************/
struct MPSVariable {
    MPSVariableSense sense;
    std::string      name;
    int              integer_lower_bound;
    int              integer_upper_bound;
    int              integer_fixed_value;
    double           continuous_lower_bound;
    double           continuous_upper_bound;
    double           continuous_fixed_value;
    bool             is_bounded;
    bool             is_bound_defined;
    bool             is_fixed;

    /*************************************************************************/
    MPSVariable(void) {
        this->initialize();
    }

    /*************************************************************************/
    void initialize(void) {
        this->sense                  = MPSVariableSense::Continuous;
        this->name                   = "";
        this->integer_lower_bound    = 0;
        this->integer_upper_bound    = constant::INT_HALF_MAX;
        this->integer_fixed_value    = 0;
        this->continuous_lower_bound = 0.0;
        this->continuous_upper_bound = HUGE_VAL;
        this->continuous_fixed_value = 0.0;
        this->is_bound_defined       = false;
        this->is_fixed               = false;
    }
};
}  // namespace printemps::mps
#endif
/*****************************************************************************/
// END
/*****************************************************************************/