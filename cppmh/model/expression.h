/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef CPPMH_MODEL_EXPRESSION_H__
#define CPPMH_MODEL_EXPRESSION_H__

#include <vector>
#include <unordered_map>

#include "abstract_multi_array_element.h"
#include "fixed_size_hash_map.h"
#include "move.h"

namespace cppmh {
namespace model {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Variable;

/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Objective;

/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Constraint;

/*****************************************************************************/
struct ExpressionConstant {
    static constexpr int DEFAULT_SENSITIVITY_RESERVE_SIZE = 1000;
};

/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Expression : public AbstractMultiArrayElement {
    /**
     * [Access controls for special member functions]
     *  -- Default constructor : default, private
     *  -- Copy constructor    : default, private (for std::vector.reserve())
     *  -- Copy assignment     : default  public
     *  -- Move constructor    : default, public
     *  -- Move assignment     : default, public
     *
     * NOTE: The Move constructor is enabled and active, so that declaring an
     * Expression object by Expression<int,int> e1 = e2.copy(); is permissible.
     * This is due to describing definitions of binary operators.
     */

    /**
     * These friend declarations are required to have an Expression object as a
     * member.
     */
    friend class Objective<T_Variable, T_Expression>;
    friend class Constraint<T_Variable, T_Expression>;

   private:
    T_Expression m_constant_value;
    T_Expression m_value;
    bool         m_is_enabled;

    std::unordered_map<Variable<T_Variable, T_Expression> *, T_Expression>
        m_sensitivities;

    FixedSizeHashMap<Variable<T_Variable, T_Expression> *, T_Expression>
        m_fixed_sensitivities;

    /*************************************************************************/
    /// Default constructor
    Expression(void) {
        this->initialize();
    }

    /*************************************************************************/
    /// Copy constructor
    Expression(const Expression<T_Variable, T_Expression> &) = default;

    /*************************************************************************/
    Expression(const std::unordered_map<Variable<T_Variable, T_Expression> *,
                                        T_Expression> &a_SENSITIVITIES,
               const T_Expression                      a_CONSTANT_VALUE) {
        this->initialize();
        m_sensitivities  = a_SENSITIVITIES;
        m_constant_value = a_CONSTANT_VALUE;
    }

   public:
    /*************************************************************************/
    /// Copy assignment
    /**
     * NOTE: The members id and flat_index will be discarded in the copy
     * assignment and related methods. It's not a problem because the program
     * does not use flat_index at the moment.
     */
    Expression<T_Variable, T_Expression> &operator    =(
        const Expression<T_Variable, T_Expression> &) = default;

    /*************************************************************************/
    /// Move constructor
    Expression(Expression<T_Variable, T_Expression> &&) = default;

    /*************************************************************************/
    /// Move assignment
    Expression<T_Variable, T_Expression> &operator=(
        Expression<T_Variable, T_Expression> &&) = default;

    /*************************************************************************/
    inline static constexpr Expression<T_Variable, T_Expression>
    create_instance(void) {
        /**
         * When instantiation, instead of constructor, create_instance() should
         * be called.
         */
        Expression<T_Variable, T_Expression> expression;
        return expression;
    }

    /*************************************************************************/
    inline static constexpr Expression<T_Variable, T_Expression>
    create_instance(
        const std::unordered_map<Variable<T_Variable, T_Expression> *,
                                 T_Expression> &a_sensitivity,
        const T_Expression                      a_CONSTANT_VALUE) {
        /**
         * When instantiation, instead of constructor, create_instance() should
         * be called.
         */
        Expression<T_Variable, T_Expression> expression(a_sensitivity,
                                                        a_CONSTANT_VALUE);
        return expression;
    }

    /*************************************************************************/
    inline constexpr void initialize(void) {
        AbstractMultiArrayElement::initialize();
        m_constant_value = 0;
        m_value          = 0;
        m_is_enabled     = true;
        m_sensitivities.clear();
        m_fixed_sensitivities.initialize();
    }

    /*************************************************************************/
    inline constexpr void set_sensitivities(
        const std::unordered_map<Variable<T_Variable, T_Expression> *,
                                 T_Expression> &a_SENSITIVITIES) {
        m_sensitivities = a_SENSITIVITIES;
    }

    /*************************************************************************/
    inline constexpr const std::unordered_map<
        Variable<T_Variable, T_Expression> *, T_Expression>
        &sensitivities(void) const {
        return m_sensitivities;
    }

    /*************************************************************************/
    inline constexpr void setup_fixed_sensitivities(void) {
        /**
         * std::unordered_map is slow in hashing because it uses modulo
         * operations. For efficient evaluations of solutions, a hash map
         * without modulo operations is set up by converting from the
         * std::unordered map.
         */
        m_fixed_sensitivities.setup(m_sensitivities,
                                    sizeof(Variable<T_Variable, T_Expression>));
    }

    /*************************************************************************/
    inline constexpr T_Expression constant_value(void) const {
        return m_constant_value;
    }

    /*************************************************************************/
    inline constexpr T_Expression evaluate(void) const noexcept {
        T_Expression value = m_constant_value;

        for (const auto &sensitivity : m_sensitivities) {
            value += sensitivity.first->value() * sensitivity.second;
        }
        return value;
    }

    /*************************************************************************/
    inline constexpr T_Expression evaluate(
        const Move<T_Variable, T_Expression> &a_MOVE) const noexcept {
        /// The following code is required for nonlinear objective functions.
        if (a_MOVE.alterations.size() == 0) {
            return this->evaluate();
        }

        T_Expression new_value = m_value;
        for (const auto &alteration : a_MOVE.alterations) {
            new_value += m_fixed_sensitivities.at(alteration.first) *
                         (alteration.second - alteration.first->value());
        }
        return new_value;
    }

    /*************************************************************************/
    inline constexpr void update(void) {
        m_value = this->evaluate();
    }

    /*************************************************************************/
    inline constexpr void update(const Move<T_Variable, T_Expression> &a_MOVE) {
        m_value = this->evaluate(a_MOVE);
    }

    /*************************************************************************/
    inline constexpr T_Expression value(void) const noexcept {
        return m_value;
    }

    /*************************************************************************/
    inline constexpr const Expression<T_Variable, T_Expression> &self(
        void) const {
        return *this;
    }

    /*************************************************************************/
    inline constexpr Expression<T_Variable, T_Expression> copy(void) const {
        return create_instance(this->sensitivities(), this->constant_value());
    }

    /*************************************************************************/
    inline constexpr bool is_enabled(void) const {
        return m_is_enabled;
    }

    /*************************************************************************/
    inline constexpr void enable(void) {
        m_is_enabled = true;
    }

    /*************************************************************************/
    inline constexpr void disable(void) {
        m_is_enabled = false;
    }

    /*************************************************************************/
    inline constexpr Expression<T_Variable, T_Expression> operator+(
        void) const {
        return create_instance(this->sensitivities(), this->constant_value());
    }

    /*************************************************************************/
    inline constexpr Expression<T_Variable, T_Expression> operator-(
        void) const {
        auto result =
            create_instance(this->sensitivities(), this->constant_value());
        for (auto &&sensitivity : result.m_sensitivities) {
            sensitivity.second *= -1;
        }
        result.m_constant_value *= -1;
        result.m_value *= -1;
        return result;
    }

    /*************************************************************************/
    template <class T_Value>
    inline constexpr Expression<T_Variable, T_Expression> &operator=(
        const T_Value a_VALUE) {
        m_sensitivities.clear();
        m_constant_value = a_VALUE;
        return *this;
    }

    /*************************************************************************/
    template <template <class, class> class T_ExpressionLike>
    inline constexpr Expression<T_Variable, T_Expression> &operator=(
        const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE) {
        m_sensitivities  = a_EXPRESSION_LIKE.to_expression().sensitivities();
        m_constant_value = 0;
        return *this;
    }

    /*************************************************************************/
    template <template <class, class> class T_ExpressionLike>
    inline constexpr Expression<T_Variable, T_Expression> &operator=(
        const Expression<T_Variable, T_Expression> &a_EXPRESSION) {
        m_sensitivities  = a_EXPRESSION.sensitivities();
        m_constant_value = a_EXPRESSION.constant_value();
        return *this;
    }

    /*************************************************************************/
    template <class T_Value>
    inline constexpr Expression<T_Variable, T_Expression> &operator+=(
        const T_Value a_VALUE) {
        m_constant_value += a_VALUE;
        return *this;
    }

    /*************************************************************************/
    template <template <class, class> class T_ExpressionLike>
    inline constexpr Expression<T_Variable, T_Expression> &operator+=(
        const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE) {
        *this += a_EXPRESSION_LIKE.to_expression();
        return *this;
    }

    /*************************************************************************/
    inline constexpr Expression<T_Variable, T_Expression> &operator+=(
        const Expression<T_Variable, T_Expression> &a_EXPRESSION) {
        for (const auto &append : a_EXPRESSION.m_sensitivities) {
            if (m_sensitivities.find(append.first) != m_sensitivities.end()) {
                m_sensitivities[append.first] += append.second;
            } else {
                m_sensitivities[append.first] = append.second;
            }
        }
        m_constant_value += a_EXPRESSION.m_constant_value;
        return *this;
    }

    /*************************************************************************/
    template <class T_Value>
    inline constexpr Expression<T_Variable, T_Expression> &operator-=(
        const T_Value a_VALUE) {
        m_constant_value -= a_VALUE;
        return *this;
    }

    /*************************************************************************/
    template <template <class, class> class T_ExpressionLike>
    inline constexpr Expression<T_Variable, T_Expression> &operator-=(
        const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE) {
        *this -= a_EXPRESSION_LIKE.to_expression();
        return *this;
    }

    /*************************************************************************/
    inline constexpr Expression<T_Variable, T_Expression> &operator-=(
        const Expression<T_Variable, T_Expression> &a_EXPRESSION) {
        *this += (-a_EXPRESSION);
        return *this;
    }

    /*************************************************************************/
    template <class T_Value>
    inline constexpr Expression<T_Variable, T_Expression> &operator*=(
        const T_Value a_VALUE) {
        for (auto &&sensitivity : m_sensitivities) {
            sensitivity.second *= a_VALUE;
        }
        m_constant_value *= a_VALUE;
        return *this;
    }

    /*************************************************************************/
    template <class T_Value>
    inline constexpr Expression<T_Variable, T_Expression> &operator/=(
        const T_Value a_VALUE) {
        for (auto &&sensitivity : m_sensitivities) {
            sensitivity.second /= a_VALUE;
        }
        m_constant_value /= a_VALUE;
        return *this;
    }
};  // namespace model
using IPExpression = Expression<int, double>;
}  // namespace model
}  // namespace cppmh
#endif
/*****************************************************************************/
// END
/*****************************************************************************/