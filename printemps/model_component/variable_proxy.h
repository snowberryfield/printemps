/*****************************************************************************/
// Copyright (c) 2020-2023 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MODEL_COMPONENT_VARIABLE_PROXY_H__
#define PRINTEMPS_MODEL_COMPONENT_VARIABLE_PROXY_H__

#include "variable.h"

namespace printemps::neighborhood {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct Move;
}  // namespace printemps::neighborhood

namespace printemps::model_component {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Expression;

/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Constraint;

/*****************************************************************************/
template <class T_Variable, class T_Expression>
class VariableProxy : public multi_array::AbstractMultiArray {
    /**
     * [Access controls for special member functions]
     *  -- Default constructor : default, private
     *  -- Copy constructor    : default, private
     *  -- Copy assignment     : delete, (private)
     *  -- Move constructor    : default, public
     *  -- Move assignment     : default, public
     */

   private:
    std::vector<Variable<T_Variable, T_Expression>> m_variables;

    /*************************************************************************/
    /// Default constructor
    VariableProxy(void) = default;

    /*************************************************************************/
    /// Copy constructor
    VariableProxy(const VariableProxy<T_Variable, T_Expression> &) = default;

    /*************************************************************************/
    /// Copy assignment
    VariableProxy<T_Variable, T_Expression> &operator    =(
        const VariableProxy<T_Variable, T_Expression> &) = delete;

    /*************************************************************************/
    VariableProxy(const int a_ID) : multi_array::AbstractMultiArray(a_ID) {
        this->setup_variables();
    }

    /*************************************************************************/
    VariableProxy(const int a_ID, const int a_NUMBER_OF_ELEMENTS)
        : multi_array::AbstractMultiArray(a_ID, a_NUMBER_OF_ELEMENTS) {
        this->setup_variables();
    }

    /*************************************************************************/
    VariableProxy(const int a_ID, const std::vector<int> &a_SHAPE)
        : multi_array::AbstractMultiArray(a_ID, a_SHAPE) {
        this->setup_variables();
    }

    /*************************************************************************/
    void setup_variables(void) {
        /**
         * m_variable.resize(this->number_of_elements()) does not work because
         * default constructor of Variable class is deleted.
         */
        m_variables.reserve(this->number_of_elements());
        for (auto i = 0; i < this->number_of_elements(); i++) {
            m_variables.emplace_back(
                Variable<T_Variable, T_Expression>::create_instance());
        }

        int flat_index = 0;

        std::vector<int> multi_dimensional_index(this->number_of_dimensions());
        for (auto &&variable : m_variables) {
            variable.set_proxy_index(m_index);
            variable.set_flat_index(flat_index);
            this->update_multi_dimensional_index(&multi_dimensional_index,
                                                 flat_index);
            variable.set_multi_dimensional_index(multi_dimensional_index);
            flat_index++;
        }
    }

   public:
    /*************************************************************************/
    /// Move constructor
    VariableProxy(VariableProxy<T_Variable, T_Expression> &&) = default;

    /*************************************************************************/
    /// Move assignment
    VariableProxy<T_Variable, T_Expression> &operator=(
        VariableProxy<T_Variable, T_Expression> &&) = default;

    /*************************************************************************/
    inline static constexpr VariableProxy<T_Variable, T_Expression>
    create_instance(const int a_ID) {
        /**
         * When instantiation, instead of constructor, create_instance() should
         * be called.
         */
        VariableProxy<T_Variable, T_Expression> proxy(a_ID);
        return proxy;
    }

    /*************************************************************************/
    inline static constexpr VariableProxy<T_Variable, T_Expression>
    create_instance(const int a_ID, const int a_NUMBER_OF_ELEMENTS) {
        /**
         * When instantiation, instead of constructor, create_instance() should
         * be called.
         */
        VariableProxy<T_Variable, T_Expression> proxy(a_ID,
                                                      a_NUMBER_OF_ELEMENTS);
        return proxy;
    }

    /*************************************************************************/
    inline static constexpr VariableProxy<T_Variable, T_Expression>
    create_instance(const int a_ID, const std::vector<int> &a_SHAPE) {
        /**
         * When instantiation, instead of constructor, create_instance() should
         * be called.
         */
        VariableProxy<T_Variable, T_Expression> proxy(a_ID, a_SHAPE);
        return proxy;
    }

    /*************************************************************************/
    inline constexpr T_Variable value(void) const {
        if (this->number_of_elements() != 1) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of elements is not one."));
        }
        return m_variables[0].value();
    }

    /*************************************************************************/
    inline constexpr void set_value(T_Variable a_VALUE) {
        if (this->number_of_elements() != 1) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of elements is not one."));
        }
        m_variables[0] = a_VALUE;
    }

    /*************************************************************************/
    inline constexpr T_Expression evaluate(void) const {
        if (this->number_of_elements() != 1) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of elements is not one."));
        }
        return m_variables[0].value();
    }

    /*************************************************************************/
    inline constexpr T_Expression evaluate(
        const neighborhood::Move<T_Variable, T_Expression> &a_MOVE) const {
        if (this->number_of_elements() != 1) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of elements is not one."));
        }
        return m_variables[0].evaluate(a_MOVE);
    }

    /*************************************************************************/
    inline constexpr void fix(void) {
        if (this->number_of_elements() != 1) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of elements is not one."));
        }
        m_variables[0].fix();
    }

    /*************************************************************************/
    inline constexpr void unfix(void) {
        if (this->number_of_elements() != 1) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of elements is not one."));
        }
        m_variables[0].unfix();
    }

    /*************************************************************************/
    inline constexpr bool is_fixed(void) const {
        if (this->number_of_elements() != 1) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of elements is not one."));
        }
        return m_variables[0].is_fixed();
    }

    /*************************************************************************/
    inline constexpr void fix_by(const T_Variable a_VALUE) {
        if (this->number_of_elements() != 1) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of elements is not one."));
        }
        m_variables[0].fix_by(a_VALUE);
    }

    /*************************************************************************/
    inline constexpr void set_bound(const T_Variable a_LOWER_BOUND,
                                    const T_Variable a_UPPER_BOUND) {
        if (a_LOWER_BOUND > a_UPPER_BOUND) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The specified lower bound is bigger than the specified upper "
                "bound. lower bound: " +
                    std::to_string(a_LOWER_BOUND) +
                    ", upper bound: " + std::to_string(a_UPPER_BOUND)));
        }
        for (auto &&variable : m_variables) {
            variable.set_bound(a_LOWER_BOUND, a_UPPER_BOUND);
        }
    }

    /*************************************************************************/
    inline constexpr void reset_bound(void) {
        for (auto &&variable : m_variables) {
            variable.reset_bound();
        }
    }

    /*************************************************************************/
    inline constexpr T_Variable lower_bound(void) const {
        if (this->number_of_elements() != 1) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of elements is not one."));
        }
        return m_variables[0].lower_bound();
    }

    /*************************************************************************/
    inline constexpr T_Variable upper_bound(void) const {
        if (this->number_of_elements() != 1) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of elements is not one."));
        }
        return m_variables[0].upper_bound();
    }

    /*************************************************************************/
    inline constexpr bool has_bounds(void) const {
        if (this->number_of_elements() != 1) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of elements is not one."));
        }
        return m_variables[0].has_bounds();
    }

    /*************************************************************************/
    inline constexpr VariableSense sense(void) const {
        if (this->number_of_elements() != 1) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of elements is not one."));
        }
        return m_variables[0].sense();
    }

    /*************************************************************************/
    inline constexpr void set_name(const std::string &a_NAME) {
        if (this->number_of_elements() != 1) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of elements is not one."));
        }
        m_variables[0].set_name(a_NAME);
    }

    /*************************************************************************/
    inline constexpr const std::string &name(void) const {
        if (this->number_of_elements() != 1) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of elements is not one."));
        }
        return m_variables[0].name();
    }

    /*************************************************************************/
    inline constexpr std::vector<Variable<T_Variable, T_Expression>>
        &flat_indexed_variables(void) {
        return m_variables;
    }

    /*************************************************************************/
    inline constexpr const std::vector<Variable<T_Variable, T_Expression>>
        &flat_indexed_variables(void) const {
        return m_variables;
    }

    /*************************************************************************/
    inline constexpr Variable<T_Variable, T_Expression> &flat_indexed_variables(
        const int a_FLAT_INDEX) {
        return m_variables[a_FLAT_INDEX];
    }

    /*************************************************************************/
    inline constexpr const Variable<T_Variable, T_Expression>
        &flat_indexed_variables(const int a_FLAT_INDEX) const {
        return m_variables[a_FLAT_INDEX];
    }

    /*************************************************************************/
    inline constexpr multi_array::ValueProxy<T_Variable>
    export_values_and_names(void) const {
        multi_array::ValueProxy<T_Variable> proxy(m_index, m_shape);

        int number_of_elements = this->number_of_elements();
        for (auto i = 0; i < number_of_elements; i++) {
            proxy.flat_indexed_values()[i] = m_variables[i].value();
            proxy.flat_indexed_names()[i]  = m_variables[i].name();
        }
        return proxy;
    }

    /*************************************************************************/
    inline constexpr Expression<T_Variable, T_Expression> to_expression(
        void) const {
        if (this->number_of_elements() != 1) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of elements is not one."));
        }
        return m_variables[0].to_expression();
    }

    /*************************************************************************/
    inline constexpr Expression<T_Variable, T_Expression> sum(void) const {
        std::unordered_map<Variable<T_Variable, T_Expression> *, T_Expression>
            sensitivities;
        sensitivities.reserve(this->number_of_elements());

        for (auto i = 0; i < this->number_of_elements(); i++) {
            sensitivities[m_variables[i].reference()] = 1;
        }
        auto result = Expression<T_Variable, T_Expression>::create_instance();
        result.set_sensitivities(sensitivities);

        return result;
    }

    /*************************************************************************/
    inline constexpr Expression<T_Variable, T_Expression> sum(
        const std::vector<int> &a_MULTI_DIMENSIONAL_INDEX) const {
        const int MULTI_DIMENSIONAL_INDEX_SIZE =
            a_MULTI_DIMENSIONAL_INDEX.size();
        if (this->number_of_dimensions() != MULTI_DIMENSIONAL_INDEX_SIZE) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of dimensions does not match."));
        }

        int number_of_partial_elements = 1;
        for (auto j = 0; j < this->number_of_dimensions(); j++) {
            if (a_MULTI_DIMENSIONAL_INDEX[j] == Range::All) {
                number_of_partial_elements *= m_shape[j];
            }
        }

        std::vector<Variable<T_Variable, T_Expression> *> variable_ptrs;
        variable_ptrs.reserve(number_of_partial_elements);

        for (auto i = 0; i < this->number_of_elements(); i++) {
            bool is_covered = true;
            for (auto j = 0; j < this->number_of_dimensions(); j++) {
                if (a_MULTI_DIMENSIONAL_INDEX[j] != Range::All &&
                    m_variables[i].multi_dimensional_index()[j] !=
                        a_MULTI_DIMENSIONAL_INDEX[j]) {
                    is_covered = false;
                    break;
                }
            }
            if (is_covered) {
                variable_ptrs.push_back(m_variables[i].reference());
            }
        }

        std::unordered_map<Variable<T_Variable, T_Expression> *, T_Expression>
            sensitivities;
        sensitivities.reserve(number_of_partial_elements);

        for (auto &&variable_ptr : variable_ptrs) {
            sensitivities[variable_ptr] = 1;
        }

        auto result = Expression<T_Variable, T_Expression>::create_instance();
        result.set_sensitivities(sensitivities);

        return result;
    }

    /*************************************************************************/
    template <class T_Array>
    inline constexpr Expression<T_Variable, T_Expression> dot(
        const T_Array &a_COEFFICIENTS) {
        if (this->number_of_dimensions() != 1) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of dimensions does not one."));
        }

        const int COEFFICIENTS_SIZE = a_COEFFICIENTS.size();
        if (this->number_of_elements() != COEFFICIENTS_SIZE) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of elements does not match."));
        }

        std::unordered_map<Variable<T_Variable, T_Expression> *, T_Expression>
            sensitivities;
        sensitivities.reserve(this->number_of_elements());

        for (auto i = 0; i < this->number_of_elements(); i++) {
            sensitivities[m_variables[i].reference()] = a_COEFFICIENTS[i];
        }

        auto result = Expression<T_Variable, T_Expression>::create_instance();
        result.set_sensitivities(sensitivities);

        return result;
    }

    /*************************************************************************/
    template <class T_Array>
    inline constexpr Expression<T_Variable, T_Expression> dot(
        const std::vector<int> &a_MULTI_DIMENSIONAL_INDEX,
        const T_Array &         a_COEFFICIENTS) {
        const int MULTI_DIMENSIONAL_INDEX_SIZE =
            a_MULTI_DIMENSIONAL_INDEX.size();
        if (this->number_of_dimensions() != MULTI_DIMENSIONAL_INDEX_SIZE) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of dimensions does not match."));
        }

        if (std::count(a_MULTI_DIMENSIONAL_INDEX.begin(),
                       a_MULTI_DIMENSIONAL_INDEX.end(), Range::All) >= 2) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of specified Range::All is more than one."));
        }

        int number_of_partial_elements = 1;
        for (auto j = 0; j < this->number_of_dimensions(); j++) {
            if (a_MULTI_DIMENSIONAL_INDEX[j] == Range::All) {
                number_of_partial_elements *= m_shape[j];
            }
        }

        std::vector<Variable<T_Variable, T_Expression> *> variable_ptrs;
        variable_ptrs.reserve(number_of_partial_elements);

        for (auto i = 0; i < this->number_of_elements(); i++) {
            bool is_covered = true;
            for (auto j = 0; j < this->number_of_dimensions(); j++) {
                if (a_MULTI_DIMENSIONAL_INDEX[j] != Range::All &&
                    m_variables[i].multi_dimensional_index()[j] !=
                        a_MULTI_DIMENSIONAL_INDEX[j]) {
                    is_covered = false;
                    break;
                }
            }
            if (is_covered) {
                variable_ptrs.push_back(m_variables[i].reference());
            }
        }

        if (variable_ptrs.size() != a_COEFFICIENTS.size()) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "Number of elements does not match."));
        }

        std::unordered_map<Variable<T_Variable, T_Expression> *, T_Expression>
            sensitivities;
        sensitivities.reserve(number_of_partial_elements);

        int number_of_variable_ptrs = variable_ptrs.size();
        for (auto i = 0; i < number_of_variable_ptrs; i++) {
            sensitivities[variable_ptrs[i]] = a_COEFFICIENTS[i];
        }

        auto result = Expression<T_Variable, T_Expression>::create_instance();
        result.set_sensitivities(sensitivities);

        return result;
    }

    /*************************************************************************/
    inline constexpr Constraint<T_Variable, T_Expression> selection(
        void) const {
        return this->sum() == 1;
    }

    /*************************************************************************/
    inline constexpr Constraint<T_Variable, T_Expression> selection(
        const std::vector<int> &a_MULTI_DIMENSIONAL_INDEX) const {
        return this->sum(a_MULTI_DIMENSIONAL_INDEX) == 1;
    }

    /*************************************************************************/
    inline constexpr Expression<T_Variable, T_Expression> operator+(
        void) const {
        if (this->number_of_elements() != 1) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of elements is not one."));
        }
        return m_variables[0].to_expression();
    }

    /*************************************************************************/
    inline constexpr Expression<T_Variable, T_Expression> operator-(
        void) const {
        if (this->number_of_elements() != 1) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of elements is not one."));
        }
        return -m_variables[0].to_expression();
    }

    /*************************************************************************/
    inline constexpr VariableProxy<T_Variable, T_Expression> &operator=(
        const T_Variable a_VALUE) {
        if (this->number_of_elements() != 1) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of elements is not one."));
        }
        m_variables[0] = a_VALUE;
        return *this;
    }

    /*************************************************************************/
    inline constexpr Variable<T_Variable, T_Expression> &operator[](
        const int a_FLAT_INDEX) {
        return m_variables[a_FLAT_INDEX];
    }

    /*************************************************************************/
    inline constexpr const Variable<T_Variable, T_Expression> &operator[](
        const int a_FLAT_INDEX) const {
        return m_variables[a_FLAT_INDEX];
    }

    /*************************************************************************/
    inline constexpr Variable<T_Variable, T_Expression> &operator()(
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

        return m_variables[flat_index];
    }

    /*************************************************************************/
    inline constexpr const Variable<T_Variable, T_Expression> &operator()(
        const std::vector<int> &a_MULTI_DIMENSIONAL_INDEX) const {
        if (this->number_of_dimensions() != a_MULTI_DIMENSIONAL_INDEX.size()) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of dimensions does not match."));
        }

        auto flat_index = std::inner_product(a_MULTI_DIMENSIONAL_INDEX.begin(),
                                             a_MULTI_DIMENSIONAL_INDEX.end(),
                                             m_strides.begin(), 0);

        return m_variables[flat_index];
    }

    /*************************************************************************/
    template <class... Args>
    inline constexpr Variable<T_Variable, T_Expression> &operator()(
        Args... args) {
        return this->operator()({args...});
    }

    /*************************************************************************/
    template <class... Args>
    inline constexpr const Variable<T_Variable, T_Expression> &operator()(
        Args... args) const {
        return this->operator()({args...});
    }
};
using IPVariableProxy = VariableProxy<int, double>;
}  // namespace printemps::model_component
#endif
/*****************************************************************************/
// END
/*****************************************************************************/
