/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MODEL_COMPONENT_VARIABLE_TYPE_REFERENCE_H__
#define PRINTEMPS_MODEL_COMPONENT_VARIABLE_TYPE_REFERENCE_H__

namespace printemps::model_component {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Variable;

/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct VariableTypeReference {
    std::vector<Variable<T_Variable, T_Expression> *> selection_variable_ptrs;
    std::vector<Variable<T_Variable, T_Expression> *> binary_variable_ptrs;
    std::vector<Variable<T_Variable, T_Expression> *> integer_variable_ptrs;
    std::vector<Variable<T_Variable, T_Expression> *>
        dependent_binary_variable_ptrs;
    std::vector<Variable<T_Variable, T_Expression> *>
        dependent_integer_variable_ptrs;

    /*************************************************************************/
    VariableTypeReference(void) {
        this->initialize();
    }

    /*************************************************************************/
    void initialize(void) {
        this->selection_variable_ptrs.clear();
        this->binary_variable_ptrs.clear();
        this->integer_variable_ptrs.clear();
        this->dependent_binary_variable_ptrs.clear();
        this->dependent_integer_variable_ptrs.clear();
    }
};
}  // namespace printemps::model_component
#endif
/*****************************************************************************/
// END
/*****************************************************************************/