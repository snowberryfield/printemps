/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MODEL_COMPONENT_CONSTRAINT_SENSE_H__
#define PRINTEMPS_MODEL_COMPONENT_CONSTRAINT_SENSE_H__

namespace printemps::model_component {
/*****************************************************************************/
enum class ConstraintSense {
    Less,    // <= target
    Equal,   // = target
    Greater  // >= target
};

}  // namespace printemps::model_component
#endif
/*****************************************************************************/
// END
/*****************************************************************************/