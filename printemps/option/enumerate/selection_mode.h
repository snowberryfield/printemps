/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_OPTION_SELECTION_MODE_H__
#define PRINTEMPS_OPTION_SELECTION_MODE_H__

namespace printemps::option::selection_mode {
/*****************************************************************************/
enum SelectionMode : int {  //
    Off,
    Defined,
    Smaller,
    Larger,
    Independent,
    UserDefined
};

/*****************************************************************************/
static std::unordered_map<std::string, SelectionMode> SelectionModeMap = {  //
    {"Off", SelectionMode::Off},                                            //
    {"Defined", SelectionMode::Defined},                                    //
    {"Smaller", SelectionMode::Smaller},                                    //
    {"Larger", SelectionMode::Larger},                                      //
    {"Independent", SelectionMode::Independent},                            //
    {"UserDefined", SelectionMode::UserDefined}};

/*****************************************************************************/
static std::unordered_map<SelectionMode, std::string> SelectionModeInverseMap =
    {{SelectionMode::Off, "Off"},
     {SelectionMode::Defined, "Defined"},
     {SelectionMode::Smaller, "Smaller"},
     {SelectionMode::Larger, "Larger"},
     {SelectionMode::Independent, "Independent"},
     {SelectionMode::UserDefined, "UserDefined"}};

}  // namespace printemps::option::selection_mode
#endif
/*****************************************************************************/
// END
/*****************************************************************************/