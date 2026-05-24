/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_TABU_SEARCH_CONTROLLER_PENALTY_COEFFICIENT_UPDATE_MODE_H__
#define PRINTEMPS_SOLVER_TABU_SEARCH_CONTROLLER_PENALTY_COEFFICIENT_UPDATE_MODE_H__

namespace printemps::solver::tabu_search::controller {
/*****************************************************************************/
enum class PenaltyCoefficientUpdateMode { Keep, Relax, Tighten, Reset };

/*****************************************************************************/
static std::unordered_map<std::string, PenaltyCoefficientUpdateMode>
    PenaltyCoefficientUpdateModeMap = {
        {"Keep", PenaltyCoefficientUpdateMode::Keep},        //
        {"Relax", PenaltyCoefficientUpdateMode::Relax},      //
        {"Tighten", PenaltyCoefficientUpdateMode::Tighten},  //
        {"Reset", PenaltyCoefficientUpdateMode::Reset}       //
};

/*****************************************************************************/
static std::unordered_map<PenaltyCoefficientUpdateMode, std::string>
    PenaltyCoefficientUpdateModeInverseMap = {
        {PenaltyCoefficientUpdateMode::Keep, "Keep"},        //
        {PenaltyCoefficientUpdateMode::Relax, "Relax"},      //
        {PenaltyCoefficientUpdateMode::Tighten, "Tighten"},  //
        {PenaltyCoefficientUpdateMode::Reset, "Reset"}       //
};
}  // namespace printemps::solver::tabu_search::controller

#endif
/*****************************************************************************/
// END
/*****************************************************************************/
