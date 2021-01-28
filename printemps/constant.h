/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_CONSTANT_H__
#define PRINTEMPS_CONSTANT_H__

#include <string>
#include <limits>

namespace printemps {
namespace constant {
static constexpr auto EPSILON      = 1E-5;
static constexpr auto INT_HALF_MIN = std::numeric_limits<int>::min() / 2 + 1;
static constexpr auto INT_HALF_MAX = (std::numeric_limits<int>::max() - 1) / 2;
static std::string    VERSION      = "v1.5.3";
}  // namespace constant
}  // namespace printemps

#endif

/*****************************************************************************/
// END
/*****************************************************************************/
