/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MODEL_COMPONENT_CONSTRAINT_SENSE_H__
#define PRINTEMPS_MODEL_COMPONENT_CONSTRAINT_SENSE_H__

namespace printemps {
namespace model_component {
/*****************************************************************************/
enum class ConstraintSense {
    Less,    // <= target
    Equal,   // = target
    Greater  // >= target
};

}  // namespace model_component
}  // namespace printemps
#endif
/*****************************************************************************/
// END
/*****************************************************************************/