/*****************************************************************************/
// Copyright (c) 2020-2026 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MODEL_COMPONENT_FLAT_SENSITIVITY_H__
#define PRINTEMPS_MODEL_COMPONENT_FLAT_SENSITIVITY_H__

namespace printemps::model_component {
/*****************************************************************************/
template <class T_Expression>
struct FlatSensitivity {
    int          constraint_index;
    T_Expression coefficient;

    FlatSensitivity(void) noexcept
        : constraint_index(-1), coefficient(static_cast<T_Expression>(0)) {
    }

    FlatSensitivity(const int          a_CONSTRAINT_INDEX,
                    const T_Expression a_COEFFICIENT) noexcept
        : constraint_index(a_CONSTRAINT_INDEX), coefficient(a_COEFFICIENT) {
    }
};
}  // namespace printemps::model_component
#endif
/*****************************************************************************/
// END
/*****************************************************************************/
