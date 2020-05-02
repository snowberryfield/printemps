/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef CPPMH_MODEL_CONSTRAINT_LOWER_BINARY_OPERATOR_H__
#define CPPMH_MODEL_CONSTRAINT_LOWER_BINARY_OPERATOR_H__

#include <functional>
#include "move.h"

namespace cppmh {
namespace model {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Variable;

/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Expression;

/*****************************************************************************/
template <class T_Variable, class T_Expression>
class VariableProxy;

/*****************************************************************************/
template <class T_Variable, class T_Expression>
class ExpressionProxy;

/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Constraint;

/*****************************************************************************/
enum class ConstraintSense;

/*****************************************************************************/
// LOWER
/*****************************************************************************/
template <class T_Variable, class T_Expression, class T_Value>
inline constexpr Constraint<T_Variable, T_Expression> operator<=(
    const std::function<T_Expression(const Move<T_Variable, T_Expression> &)>
        &         a_FUNCTION,
    const T_Value a_TARGET) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        [&a_FUNCTION, a_TARGET](const Move<T_Variable, T_Expression> &a_MOVE) {
            return a_FUNCTION(a_MOVE) - a_TARGET;
        },
        ConstraintSense::Lower);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression, class T_Value>
inline constexpr Constraint<T_Variable, T_Expression> operator<=(
    const T_Value a_TARGET,
    const std::function<T_Expression(const Move<T_Variable, T_Expression> &)>
        &a_FUNCTION) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        [&a_FUNCTION, a_TARGET](const Move<T_Variable, T_Expression> &a_MOVE) {
            return a_TARGET - a_FUNCTION(a_MOVE);
        },
        ConstraintSense::Lower);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression, class T_Value,
          template <class, class> class T_ExpressionLike>
inline constexpr Constraint<T_Variable, T_Expression> operator<=(
    const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE,
    const T_Value                                     a_TARGET) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        a_EXPRESSION_LIKE.to_expression() - a_TARGET, ConstraintSense::Lower);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression, class T_Value,
          template <class, class> class T_ExpressionLike>
inline constexpr Constraint<T_Variable, T_Expression> operator<=(
    const T_Value                                     a_TARGET,
    const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        -a_EXPRESSION_LIKE.to_expression() + a_TARGET, ConstraintSense::Lower);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression, class T_Value>
inline constexpr Constraint<T_Variable, T_Expression> operator<=(
    const Expression<T_Variable, T_Expression> &a_EXPRESSION,
    const T_Value                               a_TARGET) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        a_EXPRESSION.self() - a_TARGET, ConstraintSense::Lower);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression, class T_Value>
inline constexpr Constraint<T_Variable, T_Expression> operator<=(
    const T_Value                               a_TARGET,
    const Expression<T_Variable, T_Expression> &a_EXPRESSION) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        -a_EXPRESSION.self() + a_TARGET, ConstraintSense::Lower);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression,
          template <class, class> class T_ExpressionLike>
inline constexpr Constraint<T_Variable, T_Expression> operator<=(
    const Expression<T_Variable, T_Expression> &      a_EXPRESSION,
    const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        a_EXPRESSION.self() - a_EXPRESSION_LIKE.to_expression(),
        ConstraintSense::Lower);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression,
          template <class, class> class T_ExpressionLike>
inline constexpr Constraint<T_Variable, T_Expression> operator<=(
    const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE,
    const Expression<T_Variable, T_Expression> &      a_EXPRESSION) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        a_EXPRESSION_LIKE.to_expression() - a_EXPRESSION.self(),
        ConstraintSense::Lower);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression,
          template <class, class> class T_ExpressionLikeLeft,
          template <class, class> class T_ExpressionLikeRight>
inline constexpr Constraint<T_Variable, T_Expression> operator<=(
    const T_ExpressionLikeLeft<T_Variable, T_Expression>
        &a_EXPRESSION_LIKE_LEFT,
    const T_ExpressionLikeRight<T_Variable, T_Expression>
        &a_EXPRESSION_LIKE_RIGHT) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        a_EXPRESSION_LIKE_LEFT.to_expression() -
            a_EXPRESSION_LIKE_RIGHT.to_expression(),
        ConstraintSense::Lower);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression>
inline constexpr Constraint<T_Variable, T_Expression> operator<=(
    const Expression<T_Variable, T_Expression> &a_EXPRESSION_LEFT,
    const Expression<T_Variable, T_Expression> &a_EXPRESSION_RIGHT) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        a_EXPRESSION_LEFT.self() - a_EXPRESSION_RIGHT.self(),
        ConstraintSense::Lower);
}

/*****************************************************************************/
// Equal
/*****************************************************************************/
template <class T_Variable, class T_Expression, class T_Value>
inline constexpr Constraint<T_Variable, T_Expression> operator==(
    const std::function<T_Expression(const Move<T_Variable, T_Expression> &)>
        &         a_FUNCTION,
    const T_Value a_TARGET) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        [&a_FUNCTION, a_TARGET](const Move<T_Variable, T_Expression> &a_MOVE) {
            return a_FUNCTION(a_MOVE) - a_TARGET;
        },
        ConstraintSense::Equal);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression, class T_Value>
inline constexpr Constraint<T_Variable, T_Expression> operator==(
    const T_Value a_TARGET,
    const std::function<T_Expression(const Move<T_Variable, T_Expression> &)>
        &a_FUNCTION) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        [&a_FUNCTION, a_TARGET](const Move<T_Variable, T_Expression> &a_MOVE) {
            return a_TARGET - a_FUNCTION(a_MOVE);
        },
        ConstraintSense::Equal);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression, class T_Value,
          template <class, class> class T_ExpressionLike>
inline constexpr Constraint<T_Variable, T_Expression> operator==(
    const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE,
    const T_Value                                     a_TARGET) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        a_EXPRESSION_LIKE.to_expression() - a_TARGET, ConstraintSense::Equal);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression, class T_Value,
          template <class, class> class T_ExpressionLike>
inline constexpr Constraint<T_Variable, T_Expression> operator==(
    const T_Value                                     a_TARGET,
    const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        -a_EXPRESSION_LIKE.to_expression() + a_TARGET, ConstraintSense::Equal);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression, class T_Value>
inline constexpr Constraint<T_Variable, T_Expression> operator==(
    const Expression<T_Variable, T_Expression> &a_EXPRESSION,
    const T_Value                               a_TARGET) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        a_EXPRESSION.self() - a_TARGET, ConstraintSense::Equal);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression, class T_Value>
inline constexpr Constraint<T_Variable, T_Expression> operator==(
    const T_Value                               a_TARGET,
    const Expression<T_Variable, T_Expression> &a_EXPRESSION) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        -a_EXPRESSION.self() + a_TARGET, ConstraintSense::Equal);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression,
          template <class, class> class T_ExpressionLike>
inline constexpr Constraint<T_Variable, T_Expression> operator==(
    const Expression<T_Variable, T_Expression> &      a_EXPRESSION,
    const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        a_EXPRESSION.self() - a_EXPRESSION_LIKE.to_expression(),
        ConstraintSense::Equal);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression,
          template <class, class> class T_ExpressionLike>
inline constexpr Constraint<T_Variable, T_Expression> operator==(
    const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE,
    const Expression<T_Variable, T_Expression> &      a_EXPRESSION) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        a_EXPRESSION_LIKE.to_expression() - a_EXPRESSION.self(),
        ConstraintSense::Equal);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression,
          template <class, class> class T_ExpressionLikeLeft,
          template <class, class> class T_ExpressionLikeRight>
inline constexpr Constraint<T_Variable, T_Expression> operator==(
    const T_ExpressionLikeLeft<T_Variable, T_Expression>
        &a_EXPRESSION_LIKE_LEFT,
    const T_ExpressionLikeRight<T_Variable, T_Expression>
        &a_EXPRESSION_LIKE_RIGHT) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        a_EXPRESSION_LIKE_LEFT.to_expression() -
            a_EXPRESSION_LIKE_RIGHT.to_expression(),
        ConstraintSense::Equal);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression>
inline constexpr Constraint<T_Variable, T_Expression> operator==(
    const Expression<T_Variable, T_Expression> &a_EXPRESSION_LEFT,
    const Expression<T_Variable, T_Expression> &a_EXPRESSION_RIGHT) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        a_EXPRESSION_LEFT.self() - a_EXPRESSION_RIGHT.self(),
        ConstraintSense::Equal);
}

/*****************************************************************************/
// Upper
/*****************************************************************************/
template <class T_Variable, class T_Expression, class T_Value>
inline constexpr Constraint<T_Variable, T_Expression> operator>=(
    const std::function<T_Expression(const Move<T_Variable, T_Expression> &)>
        &         a_FUNCTION,
    const T_Value a_TARGET) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        [&a_FUNCTION, a_TARGET](const Move<T_Variable, T_Expression> &a_MOVE) {
            return a_FUNCTION(a_MOVE) - a_TARGET;
        },
        ConstraintSense::Upper);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression, class T_Value>
inline constexpr Constraint<T_Variable, T_Expression> operator>=(
    const T_Value a_TARGET,
    const std::function<T_Expression(const Move<T_Variable, T_Expression> &)>
        &a_FUNCTION) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        [&a_FUNCTION, a_TARGET](const Move<T_Variable, T_Expression> &a_MOVE) {
            return a_TARGET - a_FUNCTION(a_MOVE);
        },
        ConstraintSense::Upper);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression, class T_Value,
          template <class, class> class T_ExpressionLike>
inline constexpr Constraint<T_Variable, T_Expression> operator>=(
    const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE,
    const T_Value                                     a_TARGET) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        a_EXPRESSION_LIKE.to_expression() - a_TARGET, ConstraintSense::Upper);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression, class T_Value,
          template <class, class> class T_ExpressionLike>
inline constexpr Constraint<T_Variable, T_Expression> operator>=(
    const T_Value                                     a_TARGET,
    const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        -a_EXPRESSION_LIKE.to_expression() + a_TARGET, ConstraintSense::Upper);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression, class T_Value>
inline constexpr Constraint<T_Variable, T_Expression> operator>=(
    const Expression<T_Variable, T_Expression> &a_EXPRESSION,
    const T_Value                               a_TARGET) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        a_EXPRESSION.self() - a_TARGET, ConstraintSense::Upper);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression, class T_Value>
inline constexpr Constraint<T_Variable, T_Expression> operator>=(
    const T_Value                               a_TARGET,
    const Expression<T_Variable, T_Expression> &a_EXPRESSION) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        -a_EXPRESSION.self() + a_TARGET, ConstraintSense::Upper);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression,
          template <class, class> class T_ExpressionLike>
inline constexpr Constraint<T_Variable, T_Expression> operator>=(
    const Expression<T_Variable, T_Expression> &      a_EXPRESSION,
    const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        a_EXPRESSION.self() - a_EXPRESSION_LIKE.to_expression(),
        ConstraintSense::Upper);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression,
          template <class, class> class T_ExpressionLike>
inline constexpr Constraint<T_Variable, T_Expression> operator>=(
    const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE,
    const Expression<T_Variable, T_Expression> &      a_EXPRESSION) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        a_EXPRESSION_LIKE.to_expression() - a_EXPRESSION.self(),
        ConstraintSense::Upper);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression,
          template <class, class> class T_ExpressionLikeLeft,
          template <class, class> class T_ExpressionLikeRight>
inline constexpr Constraint<T_Variable, T_Expression> operator>=(
    const T_ExpressionLikeLeft<T_Variable, T_Expression>
        &a_EXPRESSION_LIKE_LEFT,
    const T_ExpressionLikeRight<T_Variable, T_Expression>
        &a_EXPRESSION_LIKE_RIGHT) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        a_EXPRESSION_LIKE_LEFT.to_expression() -
            a_EXPRESSION_LIKE_RIGHT.to_expression(),
        ConstraintSense::Upper);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression>
inline constexpr Constraint<T_Variable, T_Expression> operator>=(
    const Expression<T_Variable, T_Expression> &a_EXPRESSION_LEFT,
    const Expression<T_Variable, T_Expression> &a_EXPRESSION_RIGHT) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        a_EXPRESSION_LEFT.self() - a_EXPRESSION_RIGHT.self(),
        ConstraintSense::Upper);
}
}  // namespace model
}  // namespace cppmh
#endif
/*****************************************************************************/
// END
/*****************************************************************************/