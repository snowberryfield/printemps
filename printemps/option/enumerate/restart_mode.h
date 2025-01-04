/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_OPTION_RESTART_MODE_H__
#define PRINTEMPS_OPTION_RESTART_MODE_H__

namespace printemps::option::restart_mode {
/*****************************************************************************/
enum RestartMode : int {  //
    Simple,
    Smart
};

/*****************************************************************************/
static std::unordered_map<std::string, RestartMode> RestartModeMap = {
    {"Simple", RestartMode::Simple},  //
    {"Smart", RestartMode::Smart}};

/*****************************************************************************/
static std::unordered_map<RestartMode, std::string> RestartModeInverseMap = {
    {RestartMode::Simple, "Simple"},  //
    {RestartMode::Smart, "Smart"}};

}  // namespace printemps::option::restart_mode

#endif
/*****************************************************************************/
// END
/*****************************************************************************/