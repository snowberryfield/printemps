/*****************************************************************************/
// Copyright (c) 2020-2026 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_TABU_SEARCH_CONTROLLER_SEARCH_MODE_H__
#define PRINTEMPS_SOLVER_TABU_SEARCH_CONTROLLER_SEARCH_MODE_H__

namespace printemps::solver::tabu_search::controller {
/*****************************************************************************/
enum class SearchMode { Intensify, Diversify };

/*****************************************************************************/
static std::unordered_map<std::string, SearchMode> SearchModeMap = {
    {"Intensify", SearchMode::Intensify},  //
    {"Diversify", SearchMode::Diversify}   //
};

/*****************************************************************************/
static std::unordered_map<SearchMode, std::string> SearchModeInverseMap = {
    {SearchMode::Intensify, "Intensify"},  //
    {SearchMode::Diversify, "Diversify"}   //
};
}  // namespace printemps::solver::tabu_search::controller

#endif
/*****************************************************************************/
// END
/*****************************************************************************/