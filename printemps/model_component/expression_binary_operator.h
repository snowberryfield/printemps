/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MODEL_COMPONENT_EXPRESSION_BINARY_OPERATOR_H__
#define PRINTEMPS_MODEL_COMPONENT_EXPRESSION_BINARY_OPERATOR_H__

namespace printemps::model_component {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Variable;

/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Expression;

// https://gintenlabo.hatenablog.com/entry/20110825/1314303669

/*****************************************************************************/
template <class T_Variable, class T_Expression, class T_Value,
          template <class, class> class T_ExpressionLike>
inline auto operator+(
    const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE,
    const T_Value                                     a_VALUE)  //
    -> decltype(a_EXPRESSION_LIKE.to_expression() + a_VALUE) {
    return a_EXPRESSION_LIKE.to_expression() + a_VALUE;
}

/*****************************************************************************/
template <class T_Variable, class T_Expression, class T_Value,
          template <class, class> class T_ExpressionLike>
inline auto operator+(
    const T_Value                                     a_VALUE,
    const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE)
    -> decltype(a_VALUE + a_EXPRESSION_LIKE.to_expression()) {
    return a_VALUE + a_EXPRESSION_LIKE.to_expression();
}

/*****************************************************************************/
template <class T_Variable, class T_Expression,
          template <class, class> class T_ExpressionLikeLeft,
          template <class, class> class T_ExpressionLikeRight>
inline auto operator+(const T_ExpressionLikeLeft<T_Variable, T_Expression>
                          &a_EXPRESSION_LIKE_LEFT,
                      const T_ExpressionLikeRight<T_Variable, T_Expression>
                          &a_EXPRESSION_LIKE_RIGHT)
    -> decltype(a_EXPRESSION_LIKE_LEFT.to_expression() +
                a_EXPRESSION_LIKE_RIGHT.to_expression()) {
    return a_EXPRESSION_LIKE_LEFT.to_expression() +
           a_EXPRESSION_LIKE_RIGHT.to_expression();
}

/*****************************************************************************/
template <class T_Variable, class T_Expression, class T_Value,
          template <class, class> class T_ExpressionLike>
auto operator-(
    const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE,
    const T_Value                                     a_VALUE)  //
    -> decltype(a_EXPRESSION_LIKE.to_expression() - a_VALUE) {
    return a_EXPRESSION_LIKE.to_expression() - a_VALUE;
}

/*****************************************************************************/
template <class T_Variable, class T_Expression, class T_Value,
          template <class, class> class T_ExpressionLike>
inline auto operator-(
    const T_Value                                     a_VALUE,
    const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE)
    -> decltype(a_VALUE - a_EXPRESSION_LIKE.to_expression()) {
    return a_VALUE - a_EXPRESSION_LIKE.to_expression();
}

/*****************************************************************************/
template <class T_Variable, class T_Expression,
          template <class, class> class T_ExpressionLikeLeft,
          template <class, class> class T_ExpressionLikeRight>
inline auto operator-(const T_ExpressionLikeLeft<T_Variable, T_Expression>
                          &a_EXPRESSION_LIKE_LEFT,
                      const T_ExpressionLikeRight<T_Variable, T_Expression>
                          &a_EXPRESSION_LIKE_RIGHT)
    -> decltype(a_EXPRESSION_LIKE_LEFT.to_expression() -
                a_EXPRESSION_LIKE_RIGHT.to_expression()) {
    return a_EXPRESSION_LIKE_LEFT.to_expression() -
           a_EXPRESSION_LIKE_RIGHT.to_expression();
}

/*****************************************************************************/
template <class T_Variable, class T_Expression, class T_Value,
          template <class, class> class T_ExpressionLike>
auto operator*(
    const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE,
    const T_Value                                     a_VALUE)  //
    -> decltype(a_EXPRESSION_LIKE.to_expression() * a_VALUE) {
    return a_EXPRESSION_LIKE.to_expression() * a_VALUE;
}

/*****************************************************************************/
template <class T_Variable, class T_Expression, class T_Value,
          template <class, class> class T_ExpressionLike>
auto operator*(
    const T_Value                                     a_VALUE,
    const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE)
    -> decltype(a_VALUE * a_EXPRESSION_LIKE.to_expression()) {
    return a_VALUE * a_EXPRESSION_LIKE.to_expression();
}

/*****************************************************************************/
template <class T_Variable, class T_Expression, class T_Value,
          template <class, class> class T_ExpressionLike>
inline auto operator/(
    const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE,
    const T_Value                                     a_VALUE)  //
    -> decltype(a_EXPRESSION_LIKE.to_expression() / a_VALUE) {
    return a_EXPRESSION_LIKE.to_expression() / a_VALUE;
}

/*****************************************************************************/
// EXPRESSION and EXPRESSION_LIKE
/*****************************************************************************/
template <class T_Variable, class T_Expression,
          template <class, class> class T_ExpressionLike>
inline auto operator+(
    const Expression<T_Variable, T_Expression> &      a_EXPRESSION,
    const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE)
    -> decltype(a_EXPRESSION.copy() + a_EXPRESSION_LIKE.to_expression()) {
    return a_EXPRESSION.copy() + a_EXPRESSION_LIKE.to_expression();
}

/*****************************************************************************/
template <class T_Variable, class T_Expression,
          template <class, class> class T_ExpressionLike>
inline auto operator+(
    const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE,
    const Expression<T_Variable, T_Expression> &      a_EXPRESSION)
    -> decltype(a_EXPRESSION_LIKE.to_expression() + a_EXPRESSION.copy()) {
    return a_EXPRESSION_LIKE.to_expression() + a_EXPRESSION.copy();
}

/*****************************************************************************/
template <class T_Variable, class T_Expression,
          template <class, class> class T_ExpressionLike>
inline auto operator-(
    const Expression<T_Variable, T_Expression> &      a_EXPRESSION,
    const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE)
    -> decltype(a_EXPRESSION.copy() - a_EXPRESSION_LIKE.to_expression()) {
    return a_EXPRESSION.copy() - a_EXPRESSION_LIKE.to_expression();
}

/*****************************************************************************/
template <class T_Variable, class T_Expression,
          template <class, class> class T_ExpressionLike>
inline auto operator-(
    const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE,
    const Expression<T_Variable, T_Expression> &      a_EXPRESSION)
    -> decltype(a_EXPRESSION_LIKE.to_expression() - a_EXPRESSION.copy()) {
    return a_EXPRESSION_LIKE.to_expression() - a_EXPRESSION.copy();
}

/*****************************************************************************/
// EXPRESSION
/*****************************************************************************/
template <class T_Variable, class T_Expression, class T_Value>
inline Expression<T_Variable, T_Expression> operator+(
    const Expression<T_Variable, T_Expression> &a_EXPRESSION,
    const T_Value                               a_VALUE) {
    auto result = a_EXPRESSION.copy();
    result += a_VALUE;
    return result;
}

/*****************************************************************************/
template <class T_Variable, class T_Expression, class T_Value>
inline Expression<T_Variable, T_Expression> operator+(
    const T_Value                               a_VALUE,
    const Expression<T_Variable, T_Expression> &a_EXPRESSION) {
    auto result = a_EXPRESSION.copy();
    result += a_VALUE;
    return result;
}

/*****************************************************************************/
template <class T_Variable, class T_Expression>
inline Expression<T_Variable, T_Expression> operator+(
    const Expression<T_Variable, T_Expression> &a_EXPRESSION_1,
    const Expression<T_Variable, T_Expression> &a_EXPRESSION_2) {
    auto result = a_EXPRESSION_1.copy();
    result += a_EXPRESSION_2.copy();
    return result;
}

/*****************************************************************************/
template <class T_Variable, class T_Expression, class T_Value>
inline Expression<T_Variable, T_Expression> operator-(
    const Expression<T_Variable, T_Expression> &a_EXPRESSION,
    const T_Value &                             a_VALUE) {
    auto result = a_EXPRESSION.copy();
    result -= a_VALUE;
    return result;
}
/*****************************************************************************/
template <class T_Variable, class T_Expression, class T_Value>
inline Expression<T_Variable, T_Expression> operator-(
    const T_Value                               a_VALUE,
    const Expression<T_Variable, T_Expression> &a_EXPRESSION) {
    auto result = -a_EXPRESSION.copy();
    result += a_VALUE;
    return result;
}

/*****************************************************************************/
template <class T_Variable, class T_Expression>
inline Expression<T_Variable, T_Expression> operator-(
    const Expression<T_Variable, T_Expression> &a_EXPRESSION_1,
    const Expression<T_Variable, T_Expression> &a_EXPRESSION_2) {
    auto result = a_EXPRESSION_1.copy();
    result -= a_EXPRESSION_2.copy();
    return result;
}

/*****************************************************************************/
template <class T_Variable, class T_Expression, class T_Value>
inline Expression<T_Variable, T_Expression> operator*(
    const Expression<T_Variable, T_Expression> &a_EXPRESSION,
    const T_Value                               a_VALUE) {
    auto result = a_EXPRESSION.copy();
    result *= a_VALUE;
    return result;
}

/*****************************************************************************/
template <class T_Variable, class T_Expression, class T_Value>
inline Expression<T_Variable, T_Expression> operator*(
    const T_Value                               a_VALUE,
    const Expression<T_Variable, T_Expression> &a_EXPRESSION) {
    auto result = a_EXPRESSION.copy();
    result *= a_VALUE;
    return result;
}

/*****************************************************************************/
template <class T_Variable, class T_Expression, class T_Value>
inline Expression<T_Variable, T_Expression> operator/(
    const Expression<T_Variable, T_Expression> &a_EXPRESSION,
    const T_Value                               a_VALUE) {
    auto result = a_EXPRESSION.copy();
    result /= a_VALUE;
    return result;
}
}  // namespace printemps::model_component

#endif

/*****************************************************************************/
// END
/*****************************************************************************/
