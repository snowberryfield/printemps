/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_NEIGHBORHOOD_BINOMIAL_CONSTRAINT_H__
#define PRINTEMPS_NEIGHBORHOOD_BINOMIAL_CONSTRAINT_H__

namespace printemps {
namespace neighborhood {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct BinomialConstraint {
    model_component::Variable<T_Variable, T_Expression>* variable_ptr_first;
    model_component::Variable<T_Variable, T_Expression>* variable_ptr_second;

    T_Expression sensitivity_first;
    T_Expression sensitivity_second;
    T_Expression constant_value;

    model_component::ConstraintSense sense;
    /*************************************************************************/
    BinomialConstraint(void)
        : variable_ptr_first(nullptr),
          variable_ptr_second(nullptr),
          sensitivity_first(0),
          sensitivity_second(0),
          sense(model_component::ConstraintSense::Equal) {
        /// nothing to do
    }
};

}  // namespace neighborhood
}  // namespace printemps
#endif
/*****************************************************************************/
// END
/*****************************************************************************/