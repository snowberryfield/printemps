/*****************************************************************************/
// Copyright (c) 2020-2023 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_NEIGHBORHOOD_TRINOMIAL_CONSTRAINT_H__
#define PRINTEMPS_NEIGHBORHOOD_TRINOMIAL_CONSTRAINT_H__

namespace printemps::neighborhood {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct TrinomialConstraint {
    model_component::Variable<T_Variable, T_Expression>* variable_ptr_first;
    model_component::Variable<T_Variable, T_Expression>* variable_ptr_second;
    model_component::Variable<T_Variable, T_Expression>* variable_ptr_third;

    T_Expression sensitivity_first;
    T_Expression sensitivity_second;
    T_Expression sensitivity_third;
    T_Expression constant_value;

    model_component::ConstraintSense sense;
    /*************************************************************************/
    TrinomialConstraint(void)
        : variable_ptr_first(nullptr),
          variable_ptr_second(nullptr),
          variable_ptr_third(nullptr),
          sensitivity_first(0),
          sensitivity_second(0),
          sensitivity_third(0),
          sense(model_component::ConstraintSense::Equal) {
        /// nothing to do
    }
};

}  // namespace printemps::neighborhood
#endif
/*****************************************************************************/
// END
/*****************************************************************************/