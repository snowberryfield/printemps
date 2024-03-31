/*****************************************************************************/
// Copyright (c) 2020-2024 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_OPTION_IMPROVABILITY_SCREENING_MODE_H__
#define PRINTEMPS_OPTION_IMPROVABILITY_SCREENING_MODE_H__

namespace printemps::option::improvability_screening_mode {
/*****************************************************************************/
enum ImprovabilityScreeningMode : int {  //
    Off,
    Soft,
    Aggressive,
    Intensive,
    Automatic
};

/*****************************************************************************/
static std::unordered_map<std::string, ImprovabilityScreeningMode>
    ImprovabilityScreeningModeMap = {  //
        {"Off", ImprovabilityScreeningMode::Off},
        {"Soft", ImprovabilityScreeningMode::Soft},
        {"Aggressive", ImprovabilityScreeningMode::Aggressive},
        {"Intensive", ImprovabilityScreeningMode::Intensive},
        {"Automatic", ImprovabilityScreeningMode::Automatic}};

/*****************************************************************************/
static std::unordered_map<ImprovabilityScreeningMode, std::string>
    ImprovabilityScreeningModeInverseMap = {  //
        {ImprovabilityScreeningMode::Off, "Off"},
        {ImprovabilityScreeningMode::Soft, "Soft"},
        {ImprovabilityScreeningMode::Aggressive, "Aggressive"},
        {ImprovabilityScreeningMode::Intensive, "Intensive"},
        {ImprovabilityScreeningMode::Automatic, "Automatic"}};

}  // namespace printemps::option::improvability_screening_mode

#endif
/*****************************************************************************/
// END
/*****************************************************************************/