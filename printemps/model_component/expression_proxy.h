/*****************************************************************************/
// Copyright (c) 2020-2023 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MODEL_COMPONENT_EXPRESSION_PROXY_H__
#define PRINTEMPS_MODEL_COMPONENT_EXPRESSION_PROXY_H__

#include "expression.h"

namespace printemps::model_component {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class ExpressionProxy : public multi_array::AbstractMultiArray {
    /**
     * [Access controls for special member functions]
     *  -- Default constructor : default, private
     *  -- Copy constructor    : default, private
     *  -- Copy assignment     : user-defined, public
     *  -- Move constructor    : default, public
     *  -- Move assignment     : default, public
     */

   private:
    std::vector<Expression<T_Variable, T_Expression>> m_expressions;

    /*************************************************************************/
    /// Default constructor
    ExpressionProxy(void) {
        /* nothing to do*/
    }

    /*************************************************************************/
    /// Copy constructor
    ExpressionProxy(const ExpressionProxy<T_Variable, T_Expression> &) =
        default;

    /*************************************************************************/
    ExpressionProxy(const int a_ID) : multi_array::AbstractMultiArray(a_ID) {
        this->setup_expressions();
    }

    /*************************************************************************/
    ExpressionProxy(const int a_ID, const int a_NUMBER_OF_ELEMENTS)
        : multi_array::AbstractMultiArray(a_ID, a_NUMBER_OF_ELEMENTS) {
        this->setup_expressions();
    }

    /*************************************************************************/
    ExpressionProxy(int a_ID, const std::vector<int> &a_SHAPE)
        : multi_array::AbstractMultiArray(a_ID, a_SHAPE) {
        this->setup_expressions();
    }

    /*************************************************************************/
    void setup_expressions(void) {
        /**
         * m_expression.resize(this->number_of_elements()) does not work
         * because default constructor of Expression class are deleted. for
         * reference:
         * https://cpprefjp.github.io/reference/vector/vector/resize.html
         */
        m_expressions.reserve(this->number_of_elements());
        for (auto i = 0; i < this->number_of_elements(); i++) {
            m_expressions.emplace_back(
                Expression<T_Variable, T_Expression>::create_instance());
        }

        int flat_index = 0;

        std::vector<int> multi_dimensional_index(this->number_of_dimensions());
        for (auto &&expression : m_expressions) {
            expression.set_proxy_index(m_index);
            expression.set_flat_index(flat_index);
            this->update_multi_dimensional_index(&multi_dimensional_index,
                                                 flat_index);
            expression.set_multi_dimensional_index(multi_dimensional_index);

            flat_index++;
        }
    }

   public:
    /*************************************************************************/
    /// Move constructor
    ExpressionProxy(ExpressionProxy<T_Variable, T_Expression> &&) = default;

    /*************************************************************************/
    /// Move assignment
    ExpressionProxy<T_Variable, T_Expression> &operator=(
        ExpressionProxy<T_Variable, T_Expression> &&) = default;

    /*************************************************************************/
    inline static ExpressionProxy<T_Variable, T_Expression> create_instance(
        const int a_ID) {
        /**
         * When instantiation, instead of constructor, create_instance() should
         * be called.
         */
        ExpressionProxy<T_Variable, T_Expression> proxy(a_ID);
        return proxy;
    }

    /*************************************************************************/
    inline static ExpressionProxy<T_Variable, T_Expression> create_instance(
        const int a_ID, const int a_NUMBER_OF_ELEMENTS) {
        /**
         * When instantiation, instead of constructor, create_instance() should
         * be called.
         */
        ExpressionProxy<T_Variable, T_Expression> proxy(a_ID,
                                                        a_NUMBER_OF_ELEMENTS);
        return proxy;
    }

    /*************************************************************************/
    inline static ExpressionProxy<T_Variable, T_Expression> create_instance(
        const int a_ID, const std::vector<int> &a_SHAPE) {
        /**
         * When instantiation, instead of constructor, create_instance() should
         * be called.
         */
        ExpressionProxy<T_Variable, T_Expression> proxy(a_ID, a_SHAPE);
        return proxy;
    }

    /*************************************************************************/
    inline const std::unordered_map<Variable<T_Variable, T_Expression> *,
                                    T_Expression>
        &sensitivities(void) const {
        if (this->number_of_elements() != 1) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of elements is not one."));
        }
        return m_expressions[0].sensitivities();
    }

    /*************************************************************************/
    inline T_Expression constant_value(void) const {
        if (this->number_of_elements() != 1) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of elements is not one."));
        }
        return m_expressions[0].constant_value();
    }

    /*************************************************************************/
    inline T_Expression evaluate(void) const {
        if (this->number_of_elements() != 1) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of elements is not one."));
        }
        return m_expressions[0].evaluate();
    }

    /*************************************************************************/
    inline T_Expression evaluate(
        const neighborhood::Move<T_Variable, T_Expression> &a_MOVE) {
        if (this->number_of_elements() != 1) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of elements is not one."));
        }
        return m_expressions[0].evaluate(a_MOVE);
    }

    /*************************************************************************/
    inline void update(void) {
        if (this->number_of_elements() != 1) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of elements is not one."));
        }
        m_expressions[0].update();
    }

    /*************************************************************************/
    inline void update(
        const neighborhood::Move<T_Variable, T_Expression> &a_MOVE) {
        if (this->number_of_elements() != 1) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of elements is not one."));
        }
        m_expressions[0].update(a_MOVE);
    }

    /*************************************************************************/
    inline T_Expression value(void) const {
        if (this->number_of_elements() != 1) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of elements is not one."));
        }
        return m_expressions[0].value();
    }

    /*************************************************************************/
    inline void set_name(const std::string &a_NAME) {
        if (this->number_of_elements() != 1) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of elements is not one."));
        }
        m_expressions[0].set_name(a_NAME);
    }

    /*************************************************************************/
    inline const std::string &name(void) const {
        if (this->number_of_elements() != 1) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of elements is not one."));
        }
        return m_expressions[0].name();
    }

    /*************************************************************************/
    inline std::vector<Expression<T_Variable, T_Expression>>
        &flat_indexed_expressions(void) {
        return m_expressions;
    }

    /*************************************************************************/
    inline const std::vector<Expression<T_Variable, T_Expression>>
        &flat_indexed_expressions(void) const {
        return m_expressions;
    }

    /*************************************************************************/
    inline Expression<T_Variable, T_Expression> &flat_indexed_expressions(
        const int a_FLAT_INDEX) {
        return m_expressions[a_FLAT_INDEX];
    }

    /*************************************************************************/
    inline const Expression<T_Variable, T_Expression> &flat_indexed_expressions(
        const int a_FLAT_INDEX) const {
        return m_expressions[a_FLAT_INDEX];
    }

    /*************************************************************************/
    inline multi_array::ValueProxy<T_Expression> export_values_and_names(
        void) const {
        multi_array::ValueProxy<T_Expression> proxy(m_index, m_shape);

        const int NUMBER_OF_ELEMENTS = this->number_of_elements();
        for (auto i = 0; i < NUMBER_OF_ELEMENTS; i++) {
            proxy.flat_indexed_values()[i] = m_expressions[i].value();
            proxy.flat_indexed_names()[i]  = m_expressions[i].name();
        }
        return proxy;
    }

    /*************************************************************************/
    inline Expression<T_Variable, T_Expression> to_expression(void) const {
        if (this->number_of_elements() != 1) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of elements is not one."));
        }
        return m_expressions[0].copy();
    }

    /*************************************************************************/
    inline Expression<T_Variable, T_Expression> sum(void) const {
        auto result = Expression<T_Variable, T_Expression>::create_instance();
        for (const auto &expression : m_expressions) {
            result += expression;
        }
        return result;
    }

    /*************************************************************************/
    inline Expression<T_Variable, T_Expression> sum(
        const std::vector<int> &a_MULTI_DIMENSIONAL_INDEX) const {
        const int MULTI_DIMENSIONAL_INDEX_SIZE =
            a_MULTI_DIMENSIONAL_INDEX.size();
        if (this->number_of_dimensions() != MULTI_DIMENSIONAL_INDEX_SIZE) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "Number of dimensions does not match."));
        }

        int number_of_partial_elements = 1;
        for (auto j = 0; j < this->number_of_dimensions(); j++) {
            if (a_MULTI_DIMENSIONAL_INDEX[j] == Range::All) {
                number_of_partial_elements *= m_shape[j];
            }
        }

        std::vector<const Expression<T_Variable, T_Expression> *>
            expression_ptrs;
        expression_ptrs.reserve(number_of_partial_elements);

        for (auto i = 0; i < this->number_of_elements(); i++) {
            bool is_covered = true;
            for (auto j = 0; j < this->number_of_dimensions(); j++) {
                if (a_MULTI_DIMENSIONAL_INDEX[j] != Range::All &&
                    m_expressions[i].multi_dimensional_index()[j] !=
                        a_MULTI_DIMENSIONAL_INDEX[j]) {
                    is_covered = false;
                    break;
                }
            }
            if (is_covered) {
                expression_ptrs.push_back(&m_expressions[i]);
            }
        }

        auto result = Expression<T_Variable, T_Expression>::create_instance();
        for (const auto &expression_ptr : expression_ptrs) {
            result += *expression_ptr;
        }
        return result;
    }

    /*************************************************************************/
    template <class T_Array>
    inline Expression<T_Variable, T_Expression> dot(
        const T_Array &a_COEFFICIENTS) {
        if (this->number_of_dimensions() != 1) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "Number of dimensions does not 1."));
        }
        const int COEFFICIENTS_SIZE = a_COEFFICIENTS.size();
        if (this->number_of_elements() != COEFFICIENTS_SIZE) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "Number of elements does not match."));
        }

        auto result = Expression<T_Variable, T_Expression>::create_instance();
        for (auto i = 0; i < this->number_of_elements(); i++) {
            result += m_expressions[i] * a_COEFFICIENTS[i];
        }

        return result;
    }

    /*************************************************************************/
    template <class T_Array>
    inline Expression<T_Variable, T_Expression> dot(
        const std::vector<int> &a_MULTI_DIMENSIONAL_INDEX,
        const T_Array &         a_COEFFICIENTS) {
        const int MULTI_DIMENSIONAL_INDEX = a_MULTI_DIMENSIONAL_INDEX.size();
        if (this->number_of_dimensions() != MULTI_DIMENSIONAL_INDEX) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "Number of dimensions does not match."));
        }

        if (std::count(a_MULTI_DIMENSIONAL_INDEX.begin(),
                       a_MULTI_DIMENSIONAL_INDEX.end(), Range::All) >= 2) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "Number of specified Range::All is more than 1."));
        }

        int number_of_partial_elements = 1;
        for (auto j = 0; j < this->number_of_dimensions(); j++) {
            if (a_MULTI_DIMENSIONAL_INDEX[j] == Range::All) {
                number_of_partial_elements *= m_shape[j];
            }
        }

        std::vector<const Expression<T_Variable, T_Expression> *>
            expression_ptrs;
        expression_ptrs.reserve(number_of_partial_elements);

        for (auto i = 0; i < this->number_of_elements(); i++) {
            bool is_covered = true;
            for (auto j = 0; j < this->number_of_dimensions(); j++) {
                if (a_MULTI_DIMENSIONAL_INDEX[j] != Range::All &&
                    m_expressions[i].multi_dimensional_index()[j] !=
                        a_MULTI_DIMENSIONAL_INDEX[j]) {
                    is_covered = false;
                    break;
                }
            }
            if (is_covered) {
                expression_ptrs.push_back(&m_expressions[i]);
            }
        }

        if (expression_ptrs.size() != a_COEFFICIENTS.size()) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "Number of elements does not match."));
        }

        auto result = Expression<T_Variable, T_Expression>::create_instance();

        const int EXPRESSIONS_SIZE = expression_ptrs.size();
        for (auto i = 0; i < EXPRESSIONS_SIZE; i++) {
            result += (*expression_ptrs[i]) * a_COEFFICIENTS[i];
        }

        return result;
    }

    /*************************************************************************/
    inline bool is_enabled(void) const {
        if (this->number_of_elements() != 1) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of elements is not one."));
        }
        return m_expressions[0].is_enabled();
    }

    /*************************************************************************/
    inline void enable(void) {
        /**
         *  This method enables all expressions simultaneously.
         */
        for (auto &&expression : m_expressions) {
            expression.enable();
        }
    }

    /*************************************************************************/
    inline void disable(void) {
        /**
         *  This method disables all expressions simultaneously.
         */
        for (auto &&expression : m_expressions) {
            expression.disable();
        }
    }

    /*************************************************************************/
    inline Expression<T_Variable, T_Expression> operator+(void) const {
        if (this->number_of_elements() != 1) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of elements is not one."));
        }
        return m_expressions[0].copy();
    }

    /*************************************************************************/
    inline Expression<T_Variable, T_Expression> operator-(void) const {
        if (this->number_of_elements() != 1) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of elements is not one."));
        }
        return -m_expressions[0].copy();
    }

    /*************************************************************************/
    template <class T_Value>
    inline ExpressionProxy<T_Variable, T_Expression> &operator=(
        const T_Value a_VALUE) {
        if (this->number_of_elements() != 1) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of elements is not one."));
        }
        m_expressions[0] = a_VALUE;
        return *this;
    }

    /*************************************************************************/
    template <template <class, class> class T_ExpressionLike>
    inline ExpressionProxy<T_Variable, T_Expression> &operator=(
        const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE) {
        if (this->number_of_elements() != 1) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of elements is not one."));
        }
        m_expressions[0] = a_EXPRESSION_LIKE.to_expression();
        return *this;
    }

    /*************************************************************************/
    inline ExpressionProxy<T_Variable, T_Expression> &operator=(
        /// explicitly declare operator=
        const ExpressionProxy<T_Variable, T_Expression> &a_EXPRESSION_PROXY) {
        if (this->number_of_elements() != 1) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of elements is not one."));
        }
        m_expressions[0] = a_EXPRESSION_PROXY.to_expression();
        return *this;
    }

    /*************************************************************************/
    inline ExpressionProxy<T_Variable, T_Expression> &operator=(
        const Expression<T_Variable, T_Expression> &a_EXPRESSION) {
        if (this->number_of_elements() != 1) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of elements is not one."));
        }
        m_expressions[0] = a_EXPRESSION;
        return *this;
    }

    /*************************************************************************/
    template <class T_Value>
    inline ExpressionProxy<T_Variable, T_Expression> &operator+=(
        const T_Value a_VALUE) {
        if (this->number_of_elements() != 1) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of elements is not one."));
        }
        m_expressions[0] += a_VALUE;
        return *this;
    }

    /*************************************************************************/
    template <template <class, class> class T_ExpressionLike>
    inline ExpressionProxy<T_Variable, T_Expression> &operator+=(
        const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE) {
        if (this->number_of_elements() != 1) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of elements is not one."));
        }
        m_expressions[0] += a_EXPRESSION_LIKE.to_expression();
        return *this;
    }

    /*************************************************************************/
    inline ExpressionProxy<T_Variable, T_Expression> &operator+=(
        const Expression<T_Variable, T_Expression> &a_EXPRESSION) {
        if (this->number_of_elements() != 1) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of elements is not one."));
        }
        m_expressions[0] += a_EXPRESSION;
        return *this;
    }

    /*************************************************************************/
    template <class T_Value>
    inline ExpressionProxy<T_Variable, T_Expression> &operator-=(
        const T_Value a_VALUE) {
        if (this->number_of_elements() != 1) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of elements is not one."));
        }
        m_expressions[0] -= a_VALUE;
        return *this;
    }

    /*************************************************************************/
    template <template <class, class> class T_ExpressionLike>
    inline ExpressionProxy<T_Variable, T_Expression> &operator-=(
        const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE) {
        if (this->number_of_elements() != 1) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of elements is not one."));
        }
        m_expressions[0] -= a_EXPRESSION_LIKE.to_expression();
        return *this;
    }

    /*************************************************************************/
    inline ExpressionProxy<T_Variable, T_Expression> &operator-=(
        const Expression<T_Variable, T_Expression> &a_EXPRESSION) {
        if (this->number_of_elements() != 1) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of elements is not one."));
        }
        m_expressions[0] -= a_EXPRESSION;
        return *this;
    }

    /*************************************************************************/
    template <class T_Value>
    inline ExpressionProxy<T_Variable, T_Expression> &operator*=(
        const T_Value a_VALUE) {
        if (this->number_of_elements() != 1) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of elements is not one."));
        }
        m_expressions[0] *= a_VALUE;
        return *this;
    }

    /*************************************************************************/
    template <class T_Value>
    inline ExpressionProxy<T_Variable, T_Expression> &operator/=(
        const T_Value a_VALUE) {
        if (this->number_of_elements() != 1) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of elements is not one."));
        }
        m_expressions[0] /= a_VALUE;
        return *this;
    }

    /*************************************************************************/
    inline Expression<T_Variable, T_Expression> &operator[](
        const int a_FLAT_INDEX) {
        return m_expressions[a_FLAT_INDEX];
    }

    /*************************************************************************/
    inline const Expression<T_Variable, T_Expression> &operator[](
        const int a_FLAT_INDEX) const {
        return m_expressions[a_FLAT_INDEX];
    }

    /*************************************************************************/
    inline Expression<T_Variable, T_Expression> &operator()(
        const std::vector<int> &a_MULTI_DIMENSIONAL_INDEX) {
        const int MULTI_DIMENSIONAL_INDEX_SIZE =
            a_MULTI_DIMENSIONAL_INDEX.size();
        if (this->number_of_dimensions() != MULTI_DIMENSIONAL_INDEX_SIZE) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of dimensions does not match."));
        }

        auto flat_index = std::inner_product(a_MULTI_DIMENSIONAL_INDEX.begin(),
                                             a_MULTI_DIMENSIONAL_INDEX.end(),
                                             m_strides.begin(), 0);
        return m_expressions[flat_index];
    }

    /*************************************************************************/
    inline const Expression<T_Variable, T_Expression> &operator()(
        const std::vector<int> &a_MULTI_DIMENSIONAL_INDEX) const {
        const int MULTI_DIMENSIONAL_INDEX_SIZE =
            a_MULTI_DIMENSIONAL_INDEX.size();
        if (this->number_of_dimensions() != MULTI_DIMENSIONAL_INDEX_SIZE) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of dimensions does not match."));
        }

        auto flat_index = std::inner_product(a_MULTI_DIMENSIONAL_INDEX.begin(),
                                             a_MULTI_DIMENSIONAL_INDEX.end(),
                                             m_strides.begin(), 0);

        return m_expressions[flat_index];
    }

    /*************************************************************************/
    template <class... Args>
    inline Expression<T_Variable, T_Expression> &operator()(Args... args) {
        return this->operator()({args...});
    }

    /*************************************************************************/
    template <class... Args>
    inline const Expression<T_Variable, T_Expression> &operator()(
        Args... args) const {
        return this->operator()({args...});
    }
};
using IPExpressionProxy = ExpressionProxy<int, double>;
}  // namespace printemps::model_component
#endif
/*****************************************************************************/
// END
/*****************************************************************************/