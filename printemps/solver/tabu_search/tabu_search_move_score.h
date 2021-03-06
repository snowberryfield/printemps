/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_TABU_SEARCH_TABU_SEARCH_MOVE_SCORE_H__
#define PRINTEMPS_SOLVER_TABU_SEARCH_TABU_SEARCH_MOVE_SCORE_H__

namespace printemps {
namespace solver {
namespace tabu_search {
/*****************************************************************************/
struct TabuSearchMoveScore {
    bool   is_permissible;
    double frequency_penalty;
};

/*****************************************************************************/
template <class T_Variable, class T_Expression>
constexpr bool compute_permissibility(
    const neighborhood::Move<T_Variable, T_Expression> &a_MOVE,       //
    const Memory &                                      a_MEMORY,     //
    const int                                           a_ITERATION,  //
    const option::Option &                              a_OPTION,     //
    const int                                           a_TABU_TENURE) {
    const auto &last_update_iterations = a_MEMORY.last_update_iterations();

    if (a_OPTION.tabu_search.tabu_mode == option::tabu_mode::All &&
        a_MOVE.sense != neighborhood::MoveSense::Selection) {
        /**
         * "All" tabu mode
         * The move is regarded as "tabu" if all of the variable to
         * be altered are included in the tabu list.
         */
        for (const auto &alteration : a_MOVE.alterations) {
            const int last_update_iteration =
                last_update_iterations[alteration.first->proxy_index()]
                                      [alteration.first->flat_index()];
            if (a_ITERATION - last_update_iteration >= a_TABU_TENURE) {
                return true;
            }
        }
        return false;
    } else {
        /**
         * "Any" tabu mode
         * The move is regarded as "tabu" if it includes any alteration
         * about a variable in the tabu list.
         */
        for (const auto &alteration : a_MOVE.alterations) {
            const int last_update_iteration =
                last_update_iterations[alteration.first->proxy_index()]
                                      [alteration.first->flat_index()];
            if (a_ITERATION - last_update_iteration < a_TABU_TENURE) {
                return false;
            }
        }
        return true;
    }
    /**
     * If the process is normal, this return statement is not reached.
     */
    return true;
}

/*****************************************************************************/
template <class T_Variable, class T_Expression>
constexpr double compute_frequency_penalty(
    const neighborhood::Move<T_Variable, T_Expression> &a_MOVE,       //
    const int                                           a_ITERATION,  //
    const Memory &                                      a_MEMORY,     //
    const option::Option &                              a_OPTION) noexcept {
    const auto &update_counts = a_MEMORY.update_counts();

    if (a_ITERATION == 0) {
        return 0.0;
    }

    int move_update_count = 0;
    for (const auto &alteration : a_MOVE.alterations) {
        move_update_count += update_counts[alteration.first->proxy_index()]
                                          [alteration.first->flat_index()];
    }
    return move_update_count * a_MEMORY.total_update_count_reciprocal() *
           a_OPTION.tabu_search.frequency_penalty_coefficient;
}

/*****************************************************************************/
template <class T_Variable, class T_Expression>
constexpr void evaluate_move(
    TabuSearchMoveScore *                               a_score_ptr,  //
    const neighborhood::Move<T_Variable, T_Expression> &a_MOVE,       //
    const int                                           a_ITERATION,  //
    const Memory &                                      a_MEMORY,     //
    const option::Option &                              a_OPTION,     //
    const int a_TABU_TENURE) noexcept {
    /**
     * Check if the move is permissible or not.
     */
    a_score_ptr->is_permissible                //
        = compute_permissibility(a_MOVE,       //
                                 a_MEMORY,     //
                                 a_ITERATION,  //
                                 a_OPTION,     //
                                 a_TABU_TENURE);

    /**
     * Compute the frequency pnalty of the move.
     */
    a_score_ptr->frequency_penalty                //
        = compute_frequency_penalty(a_MOVE,       //
                                    a_ITERATION,  //
                                    a_MEMORY,     //
                                    a_OPTION);
}
}  // namespace tabu_search
}  // namespace solver
}  // namespace printemps

#endif
/*****************************************************************************/
// END
/*****************************************************************************/