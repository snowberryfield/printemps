/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MODEL_COMPONENT_VARIABLE_REFERENCE_H__
#define PRINTEMPS_MODEL_COMPONENT_VARIABLE_REFERENCE_H__

namespace printemps::model_component {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Variable;

/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct VariableReference {
    std::vector<Variable<T_Variable, T_Expression> *> variable_ptrs;
    std::vector<Variable<T_Variable, T_Expression> *> fixed_variable_ptrs;
    std::vector<Variable<T_Variable, T_Expression> *> mutable_variable_ptrs;
    std::vector<Variable<T_Variable, T_Expression> *> selection_variable_ptrs;
    std::vector<Variable<T_Variable, T_Expression> *> binary_variable_ptrs;
    std::vector<Variable<T_Variable, T_Expression> *> integer_variable_ptrs;
    std::vector<Variable<T_Variable, T_Expression> *>
        intermediate_variable_ptrs;

    /*************************************************************************/
    VariableReference(void) {
        this->initialize();
    }

    /*************************************************************************/
    void initialize(void) {
        this->variable_ptrs.clear();
        this->fixed_variable_ptrs.clear();
        this->mutable_variable_ptrs.clear();
        this->selection_variable_ptrs.clear();
        this->binary_variable_ptrs.clear();
        this->integer_variable_ptrs.clear();
        this->intermediate_variable_ptrs.clear();
    }
};
}  // namespace printemps::model_component
#endif
/*****************************************************************************/
// END
/*****************************************************************************/