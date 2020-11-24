/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef printemps_CONSTANT_H__
#define printemps_CONSTANT_H__

#include <limits>

namespace printemps {
namespace constant {
static constexpr auto EPSILON = 1E-5;
static constexpr auto INT_MIN = std::numeric_limits<int>::min() / 2 + 1;
static constexpr auto INT_MAX = (std::numeric_limits<int>::max() - 1) / 2;
}  // namespace constant
}  // namespace printemps

#endif

/*****************************************************************************/
// END
/*****************************************************************************/
