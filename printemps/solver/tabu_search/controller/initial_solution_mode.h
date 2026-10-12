/*****************************************************************************/
// Copyright (c) 2020-2026 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_TABU_SEARCH_CONTROLLER_INITIAL_SOLUTION_MODE_H__
#define PRINTEMPS_SOLVER_TABU_SEARCH_CONTROLLER_INITIAL_SOLUTION_MODE_H__

namespace printemps::solver::tabu_search::controller {
/*****************************************************************************/
enum class InitialSolutionMode { Global, Local, Pending, Previous };

/*****************************************************************************/
static std::unordered_map<std::string, InitialSolutionMode>
    InitialSolutionModeMap = {
        {"Global", InitialSolutionMode::Global},     //
        {"Local", InitialSolutionMode::Local},       //
        {"Pending", InitialSolutionMode::Pending},   //
        {"Previous", InitialSolutionMode::Previous}  //
};

/*****************************************************************************/
static std::unordered_map<InitialSolutionMode, std::string>
    InitialSolutionModeInverseMap = {
        {InitialSolutionMode::Global, "Global"},     //
        {InitialSolutionMode::Local, "Local"},       //
        {InitialSolutionMode::Pending, "Pending"},   //
        {InitialSolutionMode::Previous, "Previous"}  //
};

}  // namespace printemps::solver::tabu_search::controller
#endif
/*****************************************************************************/
// END
/*****************************************************************************/
