/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MODEL_COMPONENT_CONSTRAINT_BINARY_OPERATOR_H__
#define PRINTEMPS_MODEL_COMPONENT_CONSTRAINT_BINARY_OPERATOR_H__

namespace printemps {
namespace neighborhood {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct Move;
}  // namespace neighborhood
}  // namespace printemps

namespace printemps {
namespace model_component {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Variable;

/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Expression;

/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Constraint;
}  // namespace model_component
}  // namespace printemps

namespace printemps {
namespace model_component {
/*****************************************************************************/
// LESS
/*****************************************************************************/
template <class T_Variable, class T_Expression, class T_Value,
          template <class, class> class T_ExpressionLike>
constexpr Constraint<T_Variable, T_Expression> operator<=(
    const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE,
    const T_Value                                     a_TARGET) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        a_EXPRESSION_LIKE.to_expression() - a_TARGET, ConstraintSense::Less);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression, class T_Value,
          template <class, class> class T_ExpressionLike>
constexpr Constraint<T_Variable, T_Expression> operator<=(
    const T_Value                                     a_TARGET,
    const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        -a_EXPRESSION_LIKE.to_expression() + a_TARGET, ConstraintSense::Less);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression, class T_Value>
constexpr Constraint<T_Variable, T_Expression> operator<=(
    const Expression<T_Variable, T_Expression> &a_EXPRESSION,
    const T_Value                               a_TARGET) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        a_EXPRESSION.self() - a_TARGET, ConstraintSense::Less);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression, class T_Value>
constexpr Constraint<T_Variable, T_Expression> operator<=(
    const T_Value                               a_TARGET,
    const Expression<T_Variable, T_Expression> &a_EXPRESSION) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        -a_EXPRESSION.self() + a_TARGET, ConstraintSense::Less);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression,
          template <class, class> class T_ExpressionLike>
constexpr Constraint<T_Variable, T_Expression> operator<=(
    const Expression<T_Variable, T_Expression> &      a_EXPRESSION,
    const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        a_EXPRESSION.self() - a_EXPRESSION_LIKE.to_expression(),
        ConstraintSense::Less);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression,
          template <class, class> class T_ExpressionLike>
constexpr Constraint<T_Variable, T_Expression> operator<=(
    const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE,
    const Expression<T_Variable, T_Expression> &      a_EXPRESSION) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        a_EXPRESSION_LIKE.to_expression() - a_EXPRESSION.self(),
        ConstraintSense::Less);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression,
          template <class, class> class T_ExpressionLikeLeft,
          template <class, class> class T_ExpressionLikeRight>
constexpr Constraint<T_Variable, T_Expression> operator<=(
    const T_ExpressionLikeLeft<T_Variable, T_Expression>
        &a_EXPRESSION_LIKE_LEFT,
    const T_ExpressionLikeRight<T_Variable, T_Expression>
        &a_EXPRESSION_LIKE_RIGHT) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        a_EXPRESSION_LIKE_LEFT.to_expression() -
            a_EXPRESSION_LIKE_RIGHT.to_expression(),
        ConstraintSense::Less);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression>
constexpr Constraint<T_Variable, T_Expression> operator<=(
    const Expression<T_Variable, T_Expression> &a_EXPRESSION_LEFT,
    const Expression<T_Variable, T_Expression> &a_EXPRESSION_RIGHT) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        a_EXPRESSION_LEFT.self() - a_EXPRESSION_RIGHT.self(),
        ConstraintSense::Less);
}

/*****************************************************************************/
// Equal
/*****************************************************************************/
template <class T_Variable, class T_Expression, class T_Value,
          template <class, class> class T_ExpressionLike>
constexpr Constraint<T_Variable, T_Expression> operator==(
    const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE,
    const T_Value                                     a_TARGET) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        a_EXPRESSION_LIKE.to_expression() - a_TARGET, ConstraintSense::Equal);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression, class T_Value,
          template <class, class> class T_ExpressionLike>
constexpr Constraint<T_Variable, T_Expression> operator==(
    const T_Value                                     a_TARGET,
    const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        -a_EXPRESSION_LIKE.to_expression() + a_TARGET, ConstraintSense::Equal);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression, class T_Value>
constexpr Constraint<T_Variable, T_Expression> operator==(
    const Expression<T_Variable, T_Expression> &a_EXPRESSION,
    const T_Value                               a_TARGET) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        a_EXPRESSION.self() - a_TARGET, ConstraintSense::Equal);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression, class T_Value>
constexpr Constraint<T_Variable, T_Expression> operator==(
    const T_Value                               a_TARGET,
    const Expression<T_Variable, T_Expression> &a_EXPRESSION) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        -a_EXPRESSION.self() + a_TARGET, ConstraintSense::Equal);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression,
          template <class, class> class T_ExpressionLike>
constexpr Constraint<T_Variable, T_Expression> operator==(
    const Expression<T_Variable, T_Expression> &      a_EXPRESSION,
    const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        a_EXPRESSION.self() - a_EXPRESSION_LIKE.to_expression(),
        ConstraintSense::Equal);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression,
          template <class, class> class T_ExpressionLike>
constexpr Constraint<T_Variable, T_Expression> operator==(
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
constexpr Constraint<T_Variable, T_Expression> operator==(
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
constexpr Constraint<T_Variable, T_Expression> operator==(
    const Expression<T_Variable, T_Expression> &a_EXPRESSION_LEFT,
    const Expression<T_Variable, T_Expression> &a_EXPRESSION_RIGHT) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        a_EXPRESSION_LEFT.self() - a_EXPRESSION_RIGHT.self(),
        ConstraintSense::Equal);
}

/*****************************************************************************/
// GREATER
/*****************************************************************************/
template <class T_Variable, class T_Expression, class T_Value,
          template <class, class> class T_ExpressionLike>
constexpr Constraint<T_Variable, T_Expression> operator>=(
    const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE,
    const T_Value                                     a_TARGET) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        a_EXPRESSION_LIKE.to_expression() - a_TARGET, ConstraintSense::Greater);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression, class T_Value,
          template <class, class> class T_ExpressionLike>
constexpr Constraint<T_Variable, T_Expression> operator>=(
    const T_Value                                     a_TARGET,
    const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        -a_EXPRESSION_LIKE.to_expression() + a_TARGET,
        ConstraintSense::Greater);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression, class T_Value>
constexpr Constraint<T_Variable, T_Expression> operator>=(
    const Expression<T_Variable, T_Expression> &a_EXPRESSION,
    const T_Value                               a_TARGET) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        a_EXPRESSION.self() - a_TARGET, ConstraintSense::Greater);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression, class T_Value>
constexpr Constraint<T_Variable, T_Expression> operator>=(
    const T_Value                               a_TARGET,
    const Expression<T_Variable, T_Expression> &a_EXPRESSION) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        -a_EXPRESSION.self() + a_TARGET, ConstraintSense::Greater);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression,
          template <class, class> class T_ExpressionLike>
constexpr Constraint<T_Variable, T_Expression> operator>=(
    const Expression<T_Variable, T_Expression> &      a_EXPRESSION,
    const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        a_EXPRESSION.self() - a_EXPRESSION_LIKE.to_expression(),
        ConstraintSense::Greater);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression,
          template <class, class> class T_ExpressionLike>
constexpr Constraint<T_Variable, T_Expression> operator>=(
    const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE,
    const Expression<T_Variable, T_Expression> &      a_EXPRESSION) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        a_EXPRESSION_LIKE.to_expression() - a_EXPRESSION.self(),
        ConstraintSense::Greater);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression,
          template <class, class> class T_ExpressionLikeLeft,
          template <class, class> class T_ExpressionLikeRight>
constexpr Constraint<T_Variable, T_Expression> operator>=(
    const T_ExpressionLikeLeft<T_Variable, T_Expression>
        &a_EXPRESSION_LIKE_LEFT,
    const T_ExpressionLikeRight<T_Variable, T_Expression>
        &a_EXPRESSION_LIKE_RIGHT) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        a_EXPRESSION_LIKE_LEFT.to_expression() -
            a_EXPRESSION_LIKE_RIGHT.to_expression(),
        ConstraintSense::Greater);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression>
constexpr Constraint<T_Variable, T_Expression> operator>=(
    const Expression<T_Variable, T_Expression> &a_EXPRESSION_LEFT,
    const Expression<T_Variable, T_Expression> &a_EXPRESSION_RIGHT) {
    return Constraint<T_Variable, T_Expression>::create_instance(
        a_EXPRESSION_LEFT.self() - a_EXPRESSION_RIGHT.self(),
        ConstraintSense::Greater);
}
}  // namespace model_component
}  // namespace printemps

/**
 * NOTE: The following operators must be defined in global namespace.
 */
/*****************************************************************************/
// LESS
/*****************************************************************************/
template <class T_Variable, class T_Expression, class T_Value>
constexpr printemps::model_component::Constraint<T_Variable, T_Expression>
operator<=(const std::function<T_Expression(
               const printemps::neighborhood::Move<T_Variable, T_Expression> &)>
               &         a_FUNCTION,
           const T_Value a_TARGET) {
    return printemps::model_component::Constraint<T_Variable, T_Expression>::
        create_instance(
            [&a_FUNCTION, a_TARGET](
                const printemps::neighborhood::Move<T_Variable, T_Expression>
                    &a_MOVE) { return a_FUNCTION(a_MOVE) - a_TARGET; },
            printemps::model_component::ConstraintSense::Less);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression, class T_Value>
constexpr printemps::model_component::Constraint<T_Variable, T_Expression>
operator<=(const T_Value a_TARGET,
           const std::function<T_Expression(
               const printemps::neighborhood::Move<T_Variable, T_Expression> &)>
               &a_FUNCTION) {
    return printemps::model_component::Constraint<T_Variable, T_Expression>::
        create_instance(
            [&a_FUNCTION, a_TARGET](
                const printemps::neighborhood::Move<T_Variable, T_Expression>
                    &a_MOVE) { return a_TARGET - a_FUNCTION(a_MOVE); },
            printemps::model_component::ConstraintSense::Less);
}

/*****************************************************************************/
// Equal
/*****************************************************************************/
template <class T_Variable, class T_Expression, class T_Value>
constexpr printemps::model_component::Constraint<T_Variable, T_Expression>
operator==(const std::function<T_Expression(
               const printemps::neighborhood::Move<T_Variable, T_Expression> &)>
               &         a_FUNCTION,
           const T_Value a_TARGET) {
    return printemps::model_component::Constraint<T_Variable, T_Expression>::
        create_instance(
            [&a_FUNCTION, a_TARGET](
                const printemps::neighborhood::Move<T_Variable, T_Expression>
                    &a_MOVE) { return a_FUNCTION(a_MOVE) - a_TARGET; },
            printemps::model_component::ConstraintSense::Equal);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression, class T_Value>
constexpr printemps::model_component::Constraint<T_Variable, T_Expression>
operator==(const T_Value a_TARGET,
           const std::function<T_Expression(
               const printemps::neighborhood::Move<T_Variable, T_Expression> &)>
               &a_FUNCTION) {
    return printemps::model_component::Constraint<T_Variable, T_Expression>::
        create_instance(
            [&a_FUNCTION, a_TARGET](
                const printemps::neighborhood::Move<T_Variable, T_Expression>
                    &a_MOVE) { return a_TARGET - a_FUNCTION(a_MOVE); },
            printemps::model_component::ConstraintSense::Equal);
}

/*****************************************************************************/
// Greater
/*****************************************************************************/
template <class T_Variable, class T_Expression, class T_Value>
constexpr printemps::model_component::Constraint<T_Variable, T_Expression>
operator>=(const std::function<T_Expression(
               const printemps::neighborhood::Move<T_Variable, T_Expression> &)>
               &         a_FUNCTION,
           const T_Value a_TARGET) {
    return printemps::model_component::Constraint<T_Variable, T_Expression>::
        create_instance(
            [&a_FUNCTION, a_TARGET](
                const printemps::neighborhood::Move<T_Variable, T_Expression>
                    &a_MOVE) { return a_FUNCTION(a_MOVE) - a_TARGET; },
            printemps::model_component::ConstraintSense::Greater);
}
/*****************************************************************************/
template <class T_Variable, class T_Expression, class T_Value>
constexpr printemps::model_component::Constraint<T_Variable, T_Expression>
operator>=(const T_Value a_TARGET,
           const std::function<T_Expression(
               const printemps::neighborhood::Move<T_Variable, T_Expression> &)>
               &a_FUNCTION) {
    return printemps::model_component::Constraint<T_Variable, T_Expression>::
        create_instance(
            [&a_FUNCTION, a_TARGET](
                const printemps::neighborhood::Move<T_Variable, T_Expression>
                    &a_MOVE) { return a_TARGET - a_FUNCTION(a_MOVE); },
            printemps::model_component::ConstraintSense::Greater);
}

#endif
/*****************************************************************************/
// END
/*****************************************************************************/