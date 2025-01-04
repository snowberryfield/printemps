/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_OPTION_TABU_MODE_H__
#define PRINTEMPS_OPTION_TABU_MODE_H__

namespace printemps::option::tabu_mode {
/*****************************************************************************/
enum TabuMode : int {
    All,
    Any,
};

/*****************************************************************************/
static std::unordered_map<std::string, TabuMode> TabuModeMap = {
    {"All", TabuMode::All},  //
    {"Any", TabuMode::Any}   //
};

/*****************************************************************************/
static std::unordered_map<TabuMode, std::string> TabuModeInverseMap = {
    {TabuMode::All, "All"},  //
    {TabuMode::Any, "Any"}};

}  // namespace printemps::option::tabu_mode

#endif
/*****************************************************************************/
// END
/*****************************************************************************/