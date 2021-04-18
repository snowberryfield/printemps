/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MODEL_SELECTION_MODE_H__
#define PRINTEMPS_MODEL_SELECTION_MODE_H__

namespace printemps {
namespace model {
/*****************************************************************************/
enum SelectionMode : int {  //
    None,
    Defined,
    Smaller,
    Larger,
    Independent
};
}  // namespace model
}  // namespace printemps
#endif
/*****************************************************************************/
// END
/*****************************************************************************/