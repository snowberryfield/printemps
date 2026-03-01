/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MODEL_COMPONENT_CONSTRAINT_MULTINOMIAL_H__
#define PRINTEMPS_MODEL_COMPONENT_CONSTRAINT_MULTINOMIAL_H__

#include <array>

namespace printemps::model_component {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Variable;

/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Constraint;

/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct ConstraintMultinomial {
    Constraint<T_Variable, T_Expression>* constraint_ptr;

    std::vector<Variable<T_Variable, T_Expression>*> variable_ptrs;
    std::vector<T_Expression>                        coefficients;

    T_Expression    constant_value;
    ConstraintSense sense;

    /*************************************************************************/
    ConstraintMultinomial(void) {
        this->initialize();
    }

    /*************************************************************************/
    ConstraintMultinomial(
        Constraint<T_Variable, T_Expression>* a_constraint_ptr) {
        this->setup(a_constraint_ptr);
    }

    /*************************************************************************/
    void initialize(void) {
        this->constraint_ptr = nullptr;
        this->variable_ptrs.clear();
        this->coefficients.clear();
        this->constant_value = 0;
        this->sense          = ConstraintSense::Equal;
    }

    /*************************************************************************/
    void setup(Constraint<T_Variable, T_Expression>* a_constraint_ptr) {
        this->constraint_ptr = a_constraint_ptr;

        const auto& EXPRESSION = this->constraint_ptr->expression();
        const auto  SENSITIVITIES_PAIR_VECTOR =
            EXPRESSION.sensitivities_pair_vector(true);

        const int SENSITIVITIES_SIZE = SENSITIVITIES_PAIR_VECTOR.size();
        for (auto i = 0; i < SENSITIVITIES_SIZE; i++) {
            this->variable_ptrs.push_back(SENSITIVITIES_PAIR_VECTOR[i].first);
            this->coefficients.push_back(SENSITIVITIES_PAIR_VECTOR[i].second);
        }

        this->constant_value = EXPRESSION.constant_value();
        this->sense          = this->constraint_ptr->sense();
    }
};

}  // namespace printemps::model_component
#endif
/*****************************************************************************/