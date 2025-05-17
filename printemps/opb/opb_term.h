/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_OPB_OPB_TERM_H__
#define PRINTEMPS_OPB_OPB_TERM_H__

namespace printemps::opb {
/*****************************************************************************/
struct OPBTerm {
    int                      coefficient;
    std::vector<std::string> variable_names;

    /*************************************************************************/
    OPBTerm(void) {
        this->initialize();
    }

    /*************************************************************************/
    inline void initialize(void) {
        this->coefficient = 0;
        this->variable_names.clear();
    }

    /*************************************************************************/
    inline std::string concated_variable_name(void) const {
        if (this->variable_names.size() == 1) {
            return this->variable_names.front();
        }

        std::string concated_variable_name = "";
        for (const auto& variable_name : this->variable_names) {
            concated_variable_name += variable_name;
        }
        return concated_variable_name;
    }
};
}  // namespace printemps::opb
#endif
/*****************************************************************************/
// END
/*****************************************************************************/