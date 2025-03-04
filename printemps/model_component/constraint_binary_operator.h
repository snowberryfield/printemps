/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MODEL_COMPONENT_CONSTRAINT_BINARY_OPERATOR_H__
#define PRINTEMPS_MODEL_COMPONENT_CONSTRAINT_BINARY_OPERATOR_H__

namespace printemps::neighborhood {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct Move;
}  // namespace printemps::neighborhood

namespace printemps ::model_component {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Variable;

/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Expression;

/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Constraint;
}  // namespace printemps::model_component

namespace printemps::model_component {
/*****************************************************************************/
// LESS
/*****************************************************************************/
template <class T_Variable, class T_Expression, class T_Value,
          template <class, class> class T_ExpressionLike>
inline auto operator<=(
    const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE,
    const T_Value                                     a_TARGET)
    -> decltype(a_EXPRESSION_LIKE.to_expression() - a_TARGET,
                model_component::Constraint<T_Variable, T_Expression>()) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        a_EXPRESSION_LIKE.to_expression() - a_TARGET, ConstraintSense::Less);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression, class T_Value,
          template <class, class> class T_ExpressionLike>
inline auto operator<=(
    const T_Value                                     a_TARGET,
    const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE)
    -> decltype(-a_EXPRESSION_LIKE.to_expression() + a_TARGET,
                model_component::Constraint<T_Variable, T_Expression>()) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        -a_EXPRESSION_LIKE.to_expression() + a_TARGET, ConstraintSense::Less);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression, class T_Value>
inline auto operator<=(const Expression<T_Variable, T_Expression> &a_EXPRESSION,
                       const T_Value                               a_TARGET)
    -> decltype(a_EXPRESSION.self() - a_TARGET,
                model_component::Constraint<T_Variable, T_Expression>()) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        a_EXPRESSION.self() - a_TARGET, ConstraintSense::Less);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression, class T_Value>
inline auto operator<=(const T_Value                               a_TARGET,
                       const Expression<T_Variable, T_Expression> &a_EXPRESSION)
    -> decltype(-a_EXPRESSION.self() + a_TARGET,
                model_component::Constraint<T_Variable, T_Expression>()) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        -a_EXPRESSION.self() + a_TARGET, ConstraintSense::Less);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression,
          template <class, class> class T_ExpressionLike>
inline auto operator<=(
    const Expression<T_Variable, T_Expression> &      a_EXPRESSION,
    const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE)
    -> decltype(a_EXPRESSION.self() - a_EXPRESSION_LIKE.to_expression(),
                model_component::Constraint<T_Variable, T_Expression>()) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        a_EXPRESSION.self() - a_EXPRESSION_LIKE.to_expression(),
        ConstraintSense::Less);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression,
          template <class, class> class T_ExpressionLike>
inline auto operator<=(
    const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE,
    const Expression<T_Variable, T_Expression> &      a_EXPRESSION)
    -> decltype(a_EXPRESSION_LIKE.to_expression() - a_EXPRESSION.self(),
                model_component::Constraint<T_Variable, T_Expression>()) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        a_EXPRESSION_LIKE.to_expression() - a_EXPRESSION.self(),
        ConstraintSense::Less);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression,
          template <class, class> class T_ExpressionLikeLeft,
          template <class, class> class T_ExpressionLikeRight>
inline auto operator<=(const T_ExpressionLikeLeft<T_Variable, T_Expression>
                           &a_EXPRESSION_LIKE_LEFT,
                       const T_ExpressionLikeRight<T_Variable, T_Expression>
                           &a_EXPRESSION_LIKE_RIGHT)
    -> decltype(a_EXPRESSION_LIKE_LEFT.to_expression() -
                    a_EXPRESSION_LIKE_RIGHT.to_expression(),
                model_component::Constraint<T_Variable, T_Expression>()) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        a_EXPRESSION_LIKE_LEFT.to_expression() -
            a_EXPRESSION_LIKE_RIGHT.to_expression(),
        ConstraintSense::Less);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression>
inline auto operator<=(
    const Expression<T_Variable, T_Expression> &a_EXPRESSION_LEFT,
    const Expression<T_Variable, T_Expression> &a_EXPRESSION_RIGHT)
    -> decltype(a_EXPRESSION_LEFT.self() - a_EXPRESSION_RIGHT.self(),
                model_component::Constraint<T_Variable, T_Expression>()) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        a_EXPRESSION_LEFT.self() - a_EXPRESSION_RIGHT.self(),
        ConstraintSense::Less);
}

/*****************************************************************************/
// Equal
/*****************************************************************************/
template <class T_Variable, class T_Expression, class T_Value,
          template <class, class> class T_ExpressionLike>
inline auto operator==(
    const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE,
    const T_Value                                     a_TARGET)
    -> decltype(a_EXPRESSION_LIKE.to_expression() - a_TARGET,
                model_component::Constraint<T_Variable, T_Expression>()) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        a_EXPRESSION_LIKE.to_expression() - a_TARGET, ConstraintSense::Equal);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression, class T_Value,
          template <class, class> class T_ExpressionLike>
inline auto operator==(
    const T_Value                                     a_TARGET,
    const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE)
    -> decltype(-a_EXPRESSION_LIKE.to_expression() + a_TARGET,
                model_component::Constraint<T_Variable, T_Expression>()) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        -a_EXPRESSION_LIKE.to_expression() + a_TARGET, ConstraintSense::Equal);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression, class T_Value>
inline auto operator==(const Expression<T_Variable, T_Expression> &a_EXPRESSION,
                       const T_Value                               a_TARGET)
    -> decltype(a_EXPRESSION.self() - a_TARGET,
                model_component::Constraint<T_Variable, T_Expression>()) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        a_EXPRESSION.self() - a_TARGET, ConstraintSense::Equal);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression, class T_Value>
inline auto operator==(const T_Value                               a_TARGET,
                       const Expression<T_Variable, T_Expression> &a_EXPRESSION)
    -> decltype(-a_EXPRESSION.self() + a_TARGET,
                model_component::Constraint<T_Variable, T_Expression>()) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        -a_EXPRESSION.self() + a_TARGET, ConstraintSense::Equal);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression,
          template <class, class> class T_ExpressionLike>
inline auto operator==(
    const Expression<T_Variable, T_Expression> &      a_EXPRESSION,
    const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE)
    -> decltype(a_EXPRESSION.self() - a_EXPRESSION_LIKE.to_expression(),
                model_component::Constraint<T_Variable, T_Expression>()) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        a_EXPRESSION.self() - a_EXPRESSION_LIKE.to_expression(),
        ConstraintSense::Equal);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression,
          template <class, class> class T_ExpressionLike>
inline auto operator==(
    const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE,
    const Expression<T_Variable, T_Expression> &      a_EXPRESSION)
    -> decltype(a_EXPRESSION_LIKE.to_expression() - a_EXPRESSION.self(),
                model_component::Constraint<T_Variable, T_Expression>()) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        a_EXPRESSION_LIKE.to_expression() - a_EXPRESSION.self(),
        ConstraintSense::Equal);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression,
          template <class, class> class T_ExpressionLikeLeft,
          template <class, class> class T_ExpressionLikeRight>
inline auto operator==(const T_ExpressionLikeLeft<T_Variable, T_Expression>
                           &a_EXPRESSION_LIKE_LEFT,
                       const T_ExpressionLikeRight<T_Variable, T_Expression>
                           &a_EXPRESSION_LIKE_RIGHT)
    -> decltype(a_EXPRESSION_LIKE_LEFT.to_expression() -
                    a_EXPRESSION_LIKE_RIGHT.to_expression(),
                model_component::Constraint<T_Variable, T_Expression>()) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        a_EXPRESSION_LIKE_LEFT.to_expression() -
            a_EXPRESSION_LIKE_RIGHT.to_expression(),
        ConstraintSense::Equal);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression>
inline auto operator==(
    const Expression<T_Variable, T_Expression> &a_EXPRESSION_LEFT,
    const Expression<T_Variable, T_Expression> &a_EXPRESSION_RIGHT)
    -> decltype(a_EXPRESSION_LEFT.self() - a_EXPRESSION_RIGHT.self(),
                model_component::Constraint<T_Variable, T_Expression>()) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        a_EXPRESSION_LEFT.self() - a_EXPRESSION_RIGHT.self(),
        ConstraintSense::Equal);
}

/*****************************************************************************/
// GREATER
/*****************************************************************************/
template <class T_Variable, class T_Expression, class T_Value,
          template <class, class> class T_ExpressionLike>
inline auto operator>=(
    const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE,
    const T_Value                                     a_TARGET)
    -> decltype(a_EXPRESSION_LIKE.to_expression() - a_TARGET,
                model_component::Constraint<T_Variable, T_Expression>()) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        a_EXPRESSION_LIKE.to_expression() - a_TARGET, ConstraintSense::Greater);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression, class T_Value,
          template <class, class> class T_ExpressionLike>
inline auto operator>=(
    const T_Value                                     a_TARGET,
    const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE)
    -> decltype(-a_EXPRESSION_LIKE.to_expression() + a_TARGET,
                model_component::Constraint<T_Variable, T_Expression>()) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        -a_EXPRESSION_LIKE.to_expression() + a_TARGET,
        ConstraintSense::Greater);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression, class T_Value>
inline auto operator>=(const Expression<T_Variable, T_Expression> &a_EXPRESSION,
                       const T_Value                               a_TARGET)
    -> decltype(a_EXPRESSION.self() - a_TARGET,
                model_component::Constraint<T_Variable, T_Expression>()) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        a_EXPRESSION.self() - a_TARGET, ConstraintSense::Greater);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression, class T_Value>
inline auto operator>=(const T_Value                               a_TARGET,
                       const Expression<T_Variable, T_Expression> &a_EXPRESSION)
    -> decltype(-a_EXPRESSION.self() + a_TARGET,
                model_component::Constraint<T_Variable, T_Expression>()) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        -a_EXPRESSION.self() + a_TARGET, ConstraintSense::Greater);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression,
          template <class, class> class T_ExpressionLike>
inline auto operator>=(
    const Expression<T_Variable, T_Expression> &      a_EXPRESSION,
    const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE)
    -> decltype(a_EXPRESSION.self() - a_EXPRESSION_LIKE.to_expression(),
                model_component::Constraint<T_Variable, T_Expression>()) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        a_EXPRESSION.self() - a_EXPRESSION_LIKE.to_expression(),
        ConstraintSense::Greater);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression,
          template <class, class> class T_ExpressionLike>
inline auto operator>=(
    const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE,
    const Expression<T_Variable, T_Expression> &      a_EXPRESSION)
    -> decltype(a_EXPRESSION_LIKE.to_expression() - a_EXPRESSION.self(),
                model_component::Constraint<T_Variable, T_Expression>()) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        a_EXPRESSION_LIKE.to_expression() - a_EXPRESSION.self(),
        ConstraintSense::Greater);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression,
          template <class, class> class T_ExpressionLikeLeft,
          template <class, class> class T_ExpressionLikeRight>
inline auto operator>=(const T_ExpressionLikeLeft<T_Variable, T_Expression>
                           &a_EXPRESSION_LIKE_LEFT,
                       const T_ExpressionLikeRight<T_Variable, T_Expression>
                           &a_EXPRESSION_LIKE_RIGHT)
    -> decltype(a_EXPRESSION_LIKE_LEFT.to_expression() -
                    a_EXPRESSION_LIKE_RIGHT.to_expression(),
                model_component::Constraint<T_Variable, T_Expression>()) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        a_EXPRESSION_LIKE_LEFT.to_expression() -
            a_EXPRESSION_LIKE_RIGHT.to_expression(),
        ConstraintSense::Greater);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression>
inline auto operator>=(
    const Expression<T_Variable, T_Expression> &a_EXPRESSION_LEFT,
    const Expression<T_Variable, T_Expression> &a_EXPRESSION_RIGHT)
    -> decltype(a_EXPRESSION_LEFT.self() - a_EXPRESSION_RIGHT.self(),
                model_component::Constraint<T_Variable, T_Expression>()) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        a_EXPRESSION_LEFT.self() - a_EXPRESSION_RIGHT.self(),
        ConstraintSense::Greater);
}
}  // namespace printemps::model_component
#endif
/*****************************************************************************/
// END
/*****************************************************************************/