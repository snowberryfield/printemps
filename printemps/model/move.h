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
enum class MoveSense {
    Binary,         //
    Integer,        //
    Selection,      //
    Precedence,     //
    Aggregation,    //
    VariableBound,  //
    Exclusive,      //
    Chain,          //
    UserDefined
};

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

    /*************************************************************************/
    inline static constexpr bool has_duplicate_variable(
        const Move<T_Variable, T_Expression> &a_MOVE) {
        auto &alterations      = a_MOVE.alterations;
        int   alterations_size = alterations.size();
        for (auto i = 0; i < alterations_size; i++) {
            for (auto j = i + 1; j < alterations_size; j++) {
                if (alterations[i].first == alterations[j].first) {
                    return true;
                }
            }
        }
        return false;
    };
};

/*****************************************************************************/
template <class T_Variable, class T_Expression>
inline constexpr Move<T_Variable, T_Expression> operator+(
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

using IPMove = Move<int, double>;
}  // namespace model
}  // namespace printemps
#endif
/*****************************************************************************/
// END
/*****************************************************************************/