/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MODEL_CONSTRAINT_SENSE_H__
#define PRINTEMPS_MODEL_CONSTRAINT_SENSE_H__

namespace printemps {
namespace model {
/*****************************************************************************/
enum class ConstraintSense {
    Lower,  // <= target
    Equal,  // = target
    Upper   // >= target
};
}  // namespace model
}  // namespace printemps
#endif
/*****************************************************************************/
// END
/*****************************************************************************/