/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MODEL_HANDLER_VARIABLE_TYPE_REFERENCE_H__
#define PRINTEMPS_MODEL_HANDLER_VARIABLE_TYPE_REFERENCE_H__

namespace printemps::model_handler {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct VariableTypeReference {
    std::vector<model_component::Variable<T_Variable, T_Expression> *>
        selection_variable_ptrs;
    std::vector<model_component::Variable<T_Variable, T_Expression> *>
        binary_variable_ptrs;
    std::vector<model_component::Variable<T_Variable, T_Expression> *>
        integer_variable_ptrs;
    std::vector<model_component::Variable<T_Variable, T_Expression> *>
        dependent_binary_variable_ptrs;
    std::vector<model_component::Variable<T_Variable, T_Expression> *>
        dependent_integer_variable_ptrs;

    /*************************************************************************/
    VariableTypeReference(void) noexcept {
        this->initialize();
    }

    /*************************************************************************/
    void initialize(void) noexcept {
        this->selection_variable_ptrs.clear();
        this->binary_variable_ptrs.clear();
        this->integer_variable_ptrs.clear();
        this->dependent_binary_variable_ptrs.clear();
        this->dependent_integer_variable_ptrs.clear();
    }
};
}  // namespace printemps::model_handler
#endif
/*****************************************************************************/
// END
/*****************************************************************************/