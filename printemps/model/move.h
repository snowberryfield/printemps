/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MODEL_MOVE_H__
#define PRINTEMPS_MODEL_MOVE_H__

#include <vector>
#include <unordered_set>

#include "../utility/utility.h"

namespace printemps {
namespace model {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Variable;

/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Constraint;

/*****************************************************************************/
template <class T_Variable, class T_Expression>
using Alteration = std::pair<Variable<T_Variable, T_Expression> *, T_Variable>;

/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct Move {
    std::vector<Alteration<T_Variable, T_Expression>> alterations;
    MoveSense                                         sense;
    std::unordered_set<Constraint<T_Variable, T_Expression> *>
        related_constraint_ptrs;

    /**
     * The following members are for special neighborhood moves.
     */
    bool is_special_neighborhood_move;
    bool is_available;

    /**
     * The following member is for Chain moves.
     */
    double overlap_rate;

    /*************************************************************************/
    Move(void)
        : is_special_neighborhood_move(false),
          is_available(true),
          overlap_rate(0.0) {
        /// nothing to do
    }
};

/*****************************************************************************/
template <class T_Variable, class T_Expression>
constexpr bool has_duplicate_variable(
    const Move<T_Variable, T_Expression> &a_MOVE) {
    auto &    alterations      = a_MOVE.alterations;
    const int ALTERATIONS_SIZE = alterations.size();
    for (auto i = 0; i < ALTERATIONS_SIZE; i++) {
        for (auto j = i + 1; j < ALTERATIONS_SIZE; j++) {
            if (alterations[i].first == alterations[j].first) {
                return true;
            }
        }
    }
    return false;
}

/*****************************************************************************/
template <class T_Variable, class T_Expression>
constexpr double compute_overlap_rate(
    const std::vector<Alteration<T_Variable, T_Expression>> &a_ALTERATIONS) {
    auto union_ptrs = a_ALTERATIONS.front().first->related_constraint_ptrs();
    auto intersection_ptrs =
        a_ALTERATIONS.front().first->related_constraint_ptrs();

    const int ALTERATIONS_SIZE = a_ALTERATIONS.size();
    for (auto i = 1; i < ALTERATIONS_SIZE; i++) {
        utility::update_union_set(  //
            &union_ptrs,            //
            a_ALTERATIONS[i].first->related_constraint_ptrs());

        utility::update_intersection_set(
            &intersection_ptrs,  //
            a_ALTERATIONS[i].first->related_constraint_ptrs());
    }

    /**
     * If the union of related constraints is empty, the overlap rate is set 0.
     * Otherwise, the overlap rate is computed bu (#intersection / #union)
     * ^{1/(#alterations -1)}, where # denotes the number of elements in a set.
     */
    if (union_ptrs.size() == 0) {
        return 0;
    } else {
        return pow(static_cast<double>(intersection_ptrs.size()) /
                       static_cast<double>(union_ptrs.size()),
                   1.0 / (a_ALTERATIONS.size() - 1));
    }
};

/*****************************************************************************/
template <class T_Variable, class T_Expression>
constexpr std::uint_fast64_t compute_hash(
    const std::vector<Alteration<T_Variable, T_Expression>> &a_ALTERATIONS) {
    /**
     * NOTE: Chain moves will be sorted in descending order by overlap_ratio to
     * apply std::unique()in Neighborhood.deduplicate_chain_move().A hash is
     * calculated and added to the overlap_rate so that the same moves are
     * sorted next to each other as much as possible (In the deduplication, it
     * is not required completely uniqueness).
     */
    std::uint_fast64_t integer_hash = 0;
    for (const auto &alteration : a_ALTERATIONS) {
        integer_hash = integer_hash ^
                       reinterpret_cast<std::uint_fast64_t>(alteration.first);
    }
    constexpr const int BUCKET_SIZE = 1 << 10;

    /**
     * Computed has will be normalized.
     */
    return static_cast<double>(integer_hash & (BUCKET_SIZE - 1)) / BUCKET_SIZE;
};

/*****************************************************************************/
template <class T_Variable, class T_Expression>
constexpr bool is_binary_swap(const Move<T_Variable, T_Expression> &a_MOVE) {
    for (const auto &alteration : a_MOVE.alterations) {
        if (alteration.first->sense() != VariableSense::Binary) {
            return false;
        }
    }

    return true;
}

/*****************************************************************************/
template <class T_Variable, class T_Expression>
constexpr std::unordered_set<Variable<T_Variable, T_Expression> *>
related_variable_ptrs(const Move<T_Variable, T_Expression> &a_MOVE) {
    std::unordered_set<Variable<T_Variable, T_Expression> *> result;
    for (const auto &alteration : a_MOVE.alterations) {
        result.insert(alteration.first);
    }

    return result;
}

/*****************************************************************************/
template <class T_Variable, class T_Expression>
constexpr Move<T_Variable, T_Expression> operator+(
    const Move<T_Variable, T_Expression> &a_MOVE_FIRST,
    const Move<T_Variable, T_Expression> &a_MOVE_SECOND) {
    auto result = a_MOVE_FIRST;

    result.alterations.insert(result.alterations.end(),
                              a_MOVE_SECOND.alterations.begin(),
                              a_MOVE_SECOND.alterations.end());
    result.related_constraint_ptrs.insert(
        a_MOVE_SECOND.related_constraint_ptrs.begin(),
        a_MOVE_SECOND.related_constraint_ptrs.end());

    result.sense                        = MoveSense::Chain;
    result.is_available                 = false;
    result.is_special_neighborhood_move = true;

    auto         overlap_rate = compute_overlap_rate(result.alterations);
    auto         hash         = compute_hash(result.alterations);
    const double HASH_WEIGHT  = 1E-4;

    result.overlap_rate = overlap_rate + HASH_WEIGHT * hash;

    return result;
};

/*****************************************************************************/
template <class T_Variable, class T_Expression>
constexpr bool operator==(const Move<T_Variable, T_Expression> &a_MOVE_FIRST,
                          const Move<T_Variable, T_Expression> &a_MOVE_SECOND) {
    /**
     * If the numbers of alterations of two moves are different, they
     * must be different.
     */
    if (a_MOVE_FIRST.alterations.size() != a_MOVE_SECOND.alterations.size()) {
        return false;
    }

    /**
     * If the overlap_rates including hashes of two moves are different, they
     * are likely to be different. See compute_hash() for detail.
     */
    if (fabs(a_MOVE_FIRST.overlap_rate - a_MOVE_SECOND.overlap_rate) >
        constant::EPSILON_10) {
        return false;
    }

    /**
     * Otherwise, check the variables included in two moves. Moves including
     * same variables but different in their order, are regarded as different
     * moves.
     */
    const int ALTERATIONS_SIZE = a_MOVE_FIRST.alterations.size();

    for (auto i = 0; i < ALTERATIONS_SIZE; i++) {
        if (a_MOVE_FIRST.alterations[i] != a_MOVE_SECOND.alterations[i]) {
            return false;
        }
    }

    return true;
};

using IPMove = Move<int, double>;
}  // namespace model
}  // namespace printemps
#endif
/*****************************************************************************/
// END
/*****************************************************************************/