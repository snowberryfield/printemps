/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_OPTION_CDCL_RESTART_MODE_H__
#define PRINTEMPS_OPTION_CDCL_RESTART_MODE_H__

#include <string>
#include <unordered_map>

namespace printemps::option::cdcl_restart_mode {
/*****************************************************************************/
enum CDCLRestartMode : int {  //
    Geometric,
    Luby,
    Adaptive
};

/*****************************************************************************/
static std::unordered_map<std::string, CDCLRestartMode> CDCLRestartModeMap = {
    {"Geometric", CDCLRestartMode::Geometric},  //
    {"Luby", CDCLRestartMode::Luby},            //
    {"Adaptive", CDCLRestartMode::Adaptive}};

/*****************************************************************************/
static std::unordered_map<CDCLRestartMode, std::string>
    CDCLRestartModeInverseMap = {{CDCLRestartMode::Geometric, "Geometric"},  //
                                 {CDCLRestartMode::Luby, "Luby"},            //
                                 {CDCLRestartMode::Adaptive, "Adaptive"}};

}  // namespace printemps::option::cdcl_restart_mode

#endif
/*****************************************************************************/
// END
/*****************************************************************************/
