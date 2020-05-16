/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef CPPMH_MODEL_MOVE_H__
#define CPPMH_MODEL_MOVE_H__

#include <vector>
#include <unordered_set>

namespace cppmh {
namespace model {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Variable;

/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Constraint;

/*****************************************************************************/
enum class MoveSense {
    Selection,  //
    Binary,     //
    Integer,    //
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
    Move(void) : sense(MoveSense::UserDefined) {
        ;
    }
};
using IPMove = Move<int, double>;
}  // namespace model
}  // namespace cppmh
#endif
/*****************************************************************************/
// END
/*****************************************************************************/