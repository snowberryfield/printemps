/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MODEL_COMPONENT_CONSTRAINT_COMPACT_H__
#define PRINTEMPS_MODEL_COMPONENT_CONSTRAINT_COMPACT_H__

namespace printemps::model_component {
/*****************************************************************************/
enum ConstraintCompactFlags : uint8_t {
    CONSTRAINT_COMPACT_ENABLED              = 1u << 0,
    CONSTRAINT_COMPACT_EVALUATION_IGNORABLE = 1u << 1,
    CONSTRAINT_COMPACT_LESS_OR_EQUAL        = 1u << 2,
    CONSTRAINT_COMPACT_GREATER_OR_EQUAL     = 1u << 3
};

/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct ConstraintCompact {
    T_Expression constraint_value;
    double       local_penalty_coefficient_less;
    double       local_penalty_coefficient_greater;
    std::uint8_t flags;
    /// Bit flags
    /// 0x01 : is_enabled
    /// 0x02 : is_evaluation_ignorable
    /// 0x04 : is_less_or_equal
    /// 0x08 : is_greater_or_equal

    /*************************************************************************/
    ConstraintCompact(void) {
        this->initialize();
    }

    /*************************************************************************/
    void initialize(void) {
        this->constraint_value                  = static_cast<T_Expression>(0);
        this->local_penalty_coefficient_less    = 0;
        this->local_penalty_coefficient_greater = 0;
        this->flags                             = 0;
    }
};
}  // namespace printemps::model_component
#endif
/*****************************************************************************/
// END
/*****************************************************************************/