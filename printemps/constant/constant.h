/*****************************************************************************/
// Copyright (c) 2020-2024 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_CONSTANT_H__
#define PRINTEMPS_CONSTANT_H__

namespace printemps::constant {
static constexpr auto EPSILON      = 1E-5;
static constexpr auto EPSILON_10   = 1E-10;
static constexpr auto INT_HALF_MIN = std::numeric_limits<int>::min() / 2 + 1;
static constexpr auto INT_HALF_MAX = (std::numeric_limits<int>::max() - 1) / 2;
static constexpr auto LARGE_VALUE_50  = 1E50;
static constexpr auto LARGE_VALUE_100 = 1E100;
static const std::string VERSION      = "v2.2.1";
static const std::string PROJECT_URL =
    "https://snowberryfield.github.io/printemps/";

static const std::string NO_COLOR = "\e[0m";
static const std::string RED      = "\e[0;31m";
static const std::string GREEN    = "\e[0;32m";
static const std::string YELLOW   = "\e[0;33m";
static const std::string BlUE     = "\e[0;34m";
static const std::string PURPLE   = "\e[0;35m";
static const std::string CYAN     = "\e[0;36m";
static const std::string WHITE    = "\e[0;36m";

}  // namespace printemps::constant

#endif

/*****************************************************************************/
// END
/*****************************************************************************/
