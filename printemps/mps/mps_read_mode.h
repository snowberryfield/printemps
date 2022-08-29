/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MPS_MPS_READ_MODE_H__
#define PRINTEMPS_MPS_MPS_READ_MODE_H__

namespace printemps::mps {
/*****************************************************************************/
enum class MPSReadMode {
    Initial,
    Name,
    Rows,
    Columns,
    Rhs,
    Ranges,
    Bounds,
    Endata
};
}  // namespace printemps::mps
#endif
/*****************************************************************************/
// END
/*****************************************************************************/