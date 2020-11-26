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
inline constexpr bool compute_permissibility(
    const model::Move<T_Variable, T_Expression> &a_MOVE,       //
    const Memory &                               a_MEMORY,     //
    const int                                    a_ITERATION,  //
    const Option &                               a_OPTION,     //
    const int                                    a_TABU_TENURE) {
    bool        is_permissible         = true;
    const auto &last_update_iterations = a_MEMORY.last_update_iterations();

    switch (a_OPTION.tabu_search.tabu_mode) {
        case TabuMode::All: {
            /**
             * "All" tabu mode
             * The move is regarded as "tabu" if all of the variable to
             * be altered are included in the tabu list.
             */
            is_permissible = false;
            for (const auto &alteration : a_MOVE.alterations) {
                if (a_ITERATION - last_update_iterations[alteration.first->id()]
                                                        [alteration.first
                                                             ->flat_index()] >=
                    a_TABU_TENURE) {
                    is_permissible = true;
                    break;
                }
            }
            break;
        }
        case TabuMode::Any: {
            /**
             * "Any" tabu mode
             * The move is regarded as "tabu" if it includes any alteration
             * about a variable in the tabu list.
             */
            is_permissible = true;
            for (const auto &alteration : a_MOVE.alterations) {
                if (a_ITERATION -
                        last_update_iterations[alteration.first->id()]
                                              [alteration.first->flat_index()] <
                    a_TABU_TENURE) {
                    is_permissible = false;
                    break;
                }
            }
            break;
        }

        default: {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The specified tabu mode is invalid."));
        }
    }
    return is_permissible;
}

/*****************************************************************************/
template <class T_Variable, class T_Expression>
inline constexpr double compute_frequency_penalty(
    const model::Move<T_Variable, T_Expression> &a_MOVE,
    const Memory &                               a_MEMORY,     //
    [[maybe_unused]] const int                   a_ITERATION,  //
    const Option &                               a_OPTION,     //
    [[maybe_unused]] const int                   a_TABU_TENURE) noexcept {
    if (a_ITERATION == 0) {
        return 0.0;
    }

    const auto &update_counts = a_MEMORY.update_counts();

    int move_update_count = 0;
    for (const auto &alteration : a_MOVE.alterations) {
        move_update_count += update_counts[alteration.first->id()]
                                          [alteration.first->flat_index()];
    }
    return move_update_count *
           a_OPTION.tabu_search.frequency_penalty_coefficient /
           static_cast<double>(a_MEMORY.total_update_counts());
}

/*****************************************************************************/
template <class T_Variable, class T_Expression>
inline TabuSearchMoveScore evaluate_move(
    const model::Move<T_Variable, T_Expression> &a_MOVE,       //
    const int                                    a_ITERATION,  //
    const Memory &                               a_MEMORY,     //
    const Option &                               a_OPTION,     //
    const int                                    a_TABU_TENURE) noexcept {
    TabuSearchMoveScore score;

    score.is_permissible = compute_permissibility(a_MOVE, a_MEMORY, a_ITERATION,
                                                  a_OPTION, a_TABU_TENURE);
    score.frequency_penalty = compute_frequency_penalty(
        a_MOVE, a_MEMORY, a_ITERATION, a_OPTION, a_TABU_TENURE);

    return score;
}
}  // namespace tabu_search
}  // namespace solver
}  // namespace printemps

#endif
/*****************************************************************************/
// END
/*****************************************************************************/