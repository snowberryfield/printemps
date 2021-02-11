/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MODEL_MOVE_H__
#define PRINTEMPS_MODEL_MOVE_H__

#include <vector>
#include <unordered_set>

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
    const Move<T_Variable, T_Expression> &a_MOVE_1,
    const Move<T_Variable, T_Expression> &a_MOVE_2) {
    auto result = a_MOVE_1;
    result.alterations.insert(result.alterations.end(),
                              a_MOVE_2.alterations.begin(),
                              a_MOVE_2.alterations.end());
    result.related_constraint_ptrs.insert(
        a_MOVE_2.related_constraint_ptrs.begin(),
        a_MOVE_2.related_constraint_ptrs.end());
    result.sense = MoveSense::Chain;
    return result;
};

/*****************************************************************************/
template <class T_Variable, class T_Expression>
constexpr bool operator==(const Move<T_Variable, T_Expression> &a_MOVE_1,
                          const Move<T_Variable, T_Expression> &a_MOVE_2) {
    if (a_MOVE_1.alterations.size() != a_MOVE_2.alterations.size()) {
        return false;
    }

    int ALTERATIONS_SIZE = a_MOVE_1.alterations.size();
    for (auto i = 0; i < ALTERATIONS_SIZE; i++) {
        if (a_MOVE_1.alterations[i] != a_MOVE_2.alterations[i]) {
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