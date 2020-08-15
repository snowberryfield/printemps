/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef CPPMH_MODEL_VARIABLE_REFERENCE_H__
#define CPPMH_MODEL_VARIABLE_REFERENCE_H__

#include <vector>

namespace cppmh {
namespace model {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Variable;

/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct VariableReference {
    std::vector<Variable<T_Variable, T_Expression> *> variable_ptrs;
    std::vector<Variable<T_Variable, T_Expression> *> fixed_variable_ptrs;
    std::vector<Variable<T_Variable, T_Expression> *> selection_variable_ptrs;
    std::vector<Variable<T_Variable, T_Expression> *> binary_variable_ptrs;
    std::vector<Variable<T_Variable, T_Expression> *> integer_variable_ptrs;

    /*************************************************************************/
    VariableReference(void) {
        this->initialize();
    }

    /*************************************************************************/
    virtual ~VariableReference(void) {
        /// nothing to do
    }

    /*************************************************************************/
    inline void initialize(void) {
        this->variable_ptrs.clear();
        this->fixed_variable_ptrs.clear();
        this->selection_variable_ptrs.clear();
        this->binary_variable_ptrs.clear();
        this->integer_variable_ptrs.clear();
    }
};
}  // namespace model
}  // namespace cppmh
#endif
/*****************************************************************************/
// END
/*****************************************************************************/