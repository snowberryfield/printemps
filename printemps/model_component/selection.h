/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MODEL_COMPONENT_SELECTION_H__
#define PRINTEMPS_MODEL_COMPONENT_SELECTION_H__

namespace printemps {
namespace model_component {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Variable;

/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Constraint;

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
    Selection(Constraint<T_Variable, T_Expression> *a_constraint_ptr) {
        this->setup(a_constraint_ptr);
    }

    /*************************************************************************/
    void initialize(void) {
        this->variable_ptrs.clear();
        this->selected_variable_ptr = nullptr;
        this->constraint_ptr        = nullptr;
        this->related_constraint_ptrs.clear();
    }
    /*************************************************************************/
    void setup(Constraint<T_Variable, T_Expression> *a_constraint_ptr) {
        this->initialize();
        this->constraint_ptr = a_constraint_ptr;

        for (const auto &sensitivity :
             constraint_ptr->expression().sensitivities()) {
            this->variable_ptrs.push_back(sensitivity.first);
            auto &constraint_ptrs =
                sensitivity.first->related_constraint_ptrs();
            this->related_constraint_ptrs.insert(constraint_ptrs.begin(),
                                                 constraint_ptrs.end());
        }
    }
};  // namespace model_component
}  // namespace model_component
}  // namespace printemps
#endif
/*****************************************************************************/
// END
/*****************************************************************************/