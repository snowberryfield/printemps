/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_TABU_SEARCH_CORE_TABU_SEARCH_MOVE_SCORE_H__
#define PRINTEMPS_SOLVER_TABU_SEARCH_CORE_TABU_SEARCH_MOVE_SCORE_H__

namespace printemps {
namespace solver {
namespace tabu_search {
namespace core {
/*****************************************************************************/
struct TabuSearchMoveScore {
    bool   is_permissible;
    double frequency_penalty;
    double lagrangian_penalty;
};
}  // namespace core
}  // namespace tabu_search
}  // namespace solver
}  // namespace printemps

#endif
/*****************************************************************************/
// END
/*****************************************************************************/