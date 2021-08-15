/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_OPTION_SELECTION_MODE_H__
#define PRINTEMPS_OPTION_SELECTION_MODE_H__

namespace printemps {
namespace option {
namespace selection_mode {
/*****************************************************************************/
enum SelectionMode : int {  //
    None,
    Defined,
    Smaller,
    Larger,
    Independent
};
}  // namespace selection_mode
}  // namespace option
}  // namespace printemps
#endif
/*****************************************************************************/
// END
/*****************************************************************************/