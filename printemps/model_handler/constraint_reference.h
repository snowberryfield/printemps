/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MODEL_HANDLER_CONSTRAINT_REFERENCE_H__
#define PRINTEMPS_MODEL_HANDLER_CONSTRAINT_REFERENCE_H__

namespace printemps::model_handler {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct ConstraintReference {
    std::vector<model_component::Constraint<T_Variable, T_Expression> *>
        constraint_ptrs;
    std::vector<model_component::Constraint<T_Variable, T_Expression> *>
        enabled_constraint_ptrs;
    std::vector<model_component::Constraint<T_Variable, T_Expression> *>
        disabled_constraint_ptrs;

    std::vector<model_component::Constraint<T_Variable, T_Expression> *>
        less_ptrs;
    std::vector<model_component::Constraint<T_Variable, T_Expression> *>
        equal_ptrs;
    std::vector<model_component::Constraint<T_Variable, T_Expression> *>
        greater_ptrs;

    /*************************************************************************/
    ConstraintReference(void) noexcept {
        this->initialize();
    }

    /*************************************************************************/
    void initialize(void) noexcept {
        this->constraint_ptrs.clear();
        this->enabled_constraint_ptrs.clear();
        this->disabled_constraint_ptrs.clear();
        this->less_ptrs.clear();
        this->equal_ptrs.clear();
        this->greater_ptrs.clear();
    }
};
}  // namespace printemps::model_handler
#endif
/*****************************************************************************/
// END
/*****************************************************************************/