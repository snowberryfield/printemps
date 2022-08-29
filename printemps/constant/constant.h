/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
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
static std::string    VERSION         = "v2.0.0";
static std::string PROJECT_URL = "https://snowberryfield.github.io/printemps/";

static std::string NO_COLOR = "\e[0m";
static std::string RED      = "\e[0;31m";
static std::string GREEN    = "\e[0;32m";
static std::string YELLOW   = "\e[0;33m";
static std::string BlUE     = "\e[0;34m";
static std::string PURPLE   = "\e[0;35m";
static std::string CYAN     = "\e[0;36m";
static std::string WHITE    = "\e[0;36m";

}  // namespace printemps::constant

#endif

/*****************************************************************************/
// END
/*****************************************************************************/
