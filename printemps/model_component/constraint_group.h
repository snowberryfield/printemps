/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MODEL_COMPONENT_CONSTRAINT_GROUP_H__
#define PRINTEMPS_MODEL_COMPONENT_CONSTRAINT_GROUP_H__

namespace printemps::model_component {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct ConstraintGroup {
    std::vector<model_component::Variable<T_Variable, T_Expression>*>
        variable_ptrs;
    std::vector<model_component::Constraint<T_Variable, T_Expression>*>
        constraint_ptrs;
    std::vector<std::vector<
        std::pair<model_component::Variable<T_Variable, T_Expression>*, int>>>
        solutions;

    std::vector<std::pair<model_component::Variable<T_Variable, T_Expression>*,
                          model_component::Variable<T_Variable, T_Expression>*>>
        equal_variable_pairs;

    std::vector<std::pair<model_component::Variable<T_Variable, T_Expression>*,
                          model_component::Variable<T_Variable, T_Expression>*>>
        not_equal_variable_pairs;
};
}  // namespace printemps::model_component
#endif
/*****************************************************************************/
// END
/*****************************************************************************/
