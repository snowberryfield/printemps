/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef printemps_MODEL_SUMMARY_H__
#define printemps_MODEL_SUMMARY_H__

#include <string>

namespace printemps {
namespace model {
/*****************************************************************************/
struct ModelSummary {
    std::string name;
    int         number_of_variables;
    int         number_of_constraints;

    /*************************************************************************/
    ModelSummary(void) {
        this->initialize();
    }

    /*************************************************************************/
    virtual ~ModelSummary(void) {
        /// nothing to do
    }

    /*************************************************************************/
    inline void initialize(void) {
        this->name                  = "";
        this->number_of_variables   = 0;
        this->number_of_constraints = 0;
    }
};
}  // namespace model
}  // namespace printemps
#endif
/*****************************************************************************/
// END
/*****************************************************************************/