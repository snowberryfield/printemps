/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MODEL_COMPONENT_EXPRESSION_STRUCTURE_H__
#define PRINTEMPS_MODEL_COMPONENT_EXPRESSION_STRUCTURE_H__

namespace printemps::model_component {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Variable;

/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct ExpressionStructure {
    int          number_of_variables;
    T_Expression constant_value;

    bool is_integer;
    bool has_only_binary_coefficient;
    bool has_only_binary_or_selection_variable;
    bool has_only_integer_variables;
    bool has_only_plus_or_minus_one_coefficient;
    bool has_bin_packing_variable;

    double max_abs_coefficient;

    std::vector<Variable<T_Variable, T_Expression> *>
        plus_one_coefficient_variable_ptrs;
    std::vector<Variable<T_Variable, T_Expression> *>
        minus_one_coefficient_variable_ptrs;

    std::vector<Variable<T_Variable, T_Expression> *>
        plus_one_coefficient_integer_variable_ptrs;
    std::vector<Variable<T_Variable, T_Expression> *>
        minus_one_coefficient_integer_variable_ptrs;

    std::vector<Variable<T_Variable, T_Expression> *>
        positive_coefficient_variable_ptrs;
    std::vector<Variable<T_Variable, T_Expression> *>
        negative_coefficient_variable_ptrs;

    std::vector<Variable<T_Variable, T_Expression> *> variable_ptrs;
    std::vector<T_Expression>                         coefficients;

    /**************************************************************************/
    ExpressionStructure(void) {
        this->initialize();
    }

    /**************************************************************************/
    inline void initialize(void) noexcept {
        this->number_of_variables = 0;
        this->constant_value      = 0;

        this->is_integer                             = false;
        this->has_only_binary_coefficient            = false;
        this->has_only_binary_or_selection_variable  = false;
        this->has_only_integer_variables             = false;
        this->has_only_plus_or_minus_one_coefficient = false;
        this->has_bin_packing_variable               = false;
        this->max_abs_coefficient                    = 0.0;

        this->plus_one_coefficient_variable_ptrs.clear();
        this->minus_one_coefficient_variable_ptrs.clear();

        this->plus_one_coefficient_integer_variable_ptrs.clear();
        this->minus_one_coefficient_integer_variable_ptrs.clear();

        this->positive_coefficient_variable_ptrs.clear();
        this->negative_coefficient_variable_ptrs.clear();

        this->variable_ptrs.clear();
        this->coefficients.clear();
    }
};
}  // namespace printemps::model_component
#endif
/*****************************************************************************/
// END
/*****************************************************************************/