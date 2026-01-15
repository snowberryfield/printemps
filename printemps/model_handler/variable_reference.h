/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MODEL_HANDLER_VARIABLE_REFERENCE_H__
#define PRINTEMPS_MODEL_COMPONENT_VARIABLE_REFERENCE_H__

namespace printemps::model_handler {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct VariableReference {
    std::vector<model_component::Variable<T_Variable, T_Expression> *>
        variable_ptrs;
    std::vector<model_component::Variable<T_Variable, T_Expression> *>
        fixed_variable_ptrs;
    std::vector<model_component::Variable<T_Variable, T_Expression> *>
        mutable_variable_ptrs;
    std::vector<model_component::Variable<T_Variable, T_Expression> *>
        mutable_independent_variable_ptrs;
    std::vector<model_component::Variable<T_Variable, T_Expression> *>
        mutable_dependent_variable_ptrs;

    /*************************************************************************/
    VariableReference(void) noexcept {
        this->initialize();
    }

    /*************************************************************************/
    void initialize(void) noexcept {
        this->variable_ptrs.clear();
        this->fixed_variable_ptrs.clear();
        this->mutable_variable_ptrs.clear();
        this->mutable_independent_variable_ptrs.clear();
        this->mutable_dependent_variable_ptrs.clear();
    }
};
}  // namespace printemps::model_handler
#endif
/*****************************************************************************/
// END
/*****************************************************************************/