/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MODEL_SELECTION_H__
#define PRINTEMPS_MODEL_SELECTION_H__

#include <vector>

namespace printemps {
namespace model {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Variable;

/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct Selection {
    std::vector<Variable<T_Variable, T_Expression> *> variable_ptrs;
    Variable<T_Variable, T_Expression> *              selected_variable_ptr;
    Constraint<T_Variable, T_Expression> *            constraint_ptr;

    std::unordered_set<Constraint<T_Variable, T_Expression> *>
        related_constraint_ptrs;

    /*************************************************************************/
    Selection(void) {
        this->initialize();
    }

    /*************************************************************************/
    virtual ~Selection(void) {
        /// nothing to do
    }
    /*************************************************************************/
    inline void initialize(void) {
        variable_ptrs.clear();
        selected_variable_ptr = nullptr;
        constraint_ptr        = nullptr;
        related_constraint_ptrs.clear();
    }
};
}  // namespace model
}  // namespace printemps
#endif
/*****************************************************************************/
// END
/*****************************************************************************/