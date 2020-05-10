/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/

#ifndef CPPMH_MODEL_VALUE_PROXY_H__
#define CPPMH_MODEL_VALUE_PROXY_H__

#include "abstract_multi_array.h"
#include <vector>

namespace cppmh {
namespace model {
/*****************************************************************************/
template <class T_Value>
class ValueProxy : public AbstractMultiArray {
   private:
    std::vector<T_Value>     m_values;
    std::vector<std::string> m_names;

   public:
    /*************************************************************************/
    ValueProxy(void) {
        /// nothing to do
    }

    /*************************************************************************/
    ValueProxy(const int a_ID) : AbstractMultiArray(a_ID) {
        m_values.resize(m_number_of_elements);
        m_names.resize(m_number_of_elements);
    }

    /*************************************************************************/
    ValueProxy(const int a_ID, const int a_NUMBER_OF_ELEMENTS)
        : AbstractMultiArray(a_ID, a_NUMBER_OF_ELEMENTS) {
        m_values.resize(m_number_of_elements);
        m_names.resize(m_number_of_elements);
    }

    /*************************************************************************/
    ValueProxy(const int a_ID, const std::vector<int> &a_SHAPE)
        : AbstractMultiArray(a_ID, a_SHAPE) {
        m_values.resize(m_number_of_elements);
        m_names.resize(m_number_of_elements);
    }

    /*************************************************************************/
    inline constexpr T_Value &flat_indexed_values(const int a_FLAT_INDEX) {
        return m_values[a_FLAT_INDEX];
    }

    /*************************************************************************/
    inline constexpr T_Value flat_indexed_values(const int a_FLAT_INDEX) const {
        return m_values[a_FLAT_INDEX];
    }

    /*************************************************************************/
    inline constexpr std::vector<T_Value> &flat_indexed_values(void) {
        return m_values;
    }

    /*************************************************************************/
    inline constexpr const std::vector<T_Value> &flat_indexed_values(
        void) const {
        return m_values;
    }

    /*************************************************************************/
    inline constexpr std::string &flat_indexed_names(const int a_FLAT_INDEX) {
        return m_names[a_FLAT_INDEX];
    }

    /*************************************************************************/
    inline constexpr std::string &flat_indexed_names(
        const int a_FLAT_INDEX) const {
        return m_names[a_FLAT_INDEX];
    }

    /*************************************************************************/
    inline constexpr std::vector<std::string> &flat_indexed_names(void) {
        return m_names;
    }

    /*************************************************************************/
    inline constexpr const std::vector<std::string> &flat_indexed_names(
        void) const {
        return m_names;
    }

    /*************************************************************************/
    inline constexpr T_Value &value(void) {
        if (this->number_of_elements() != 1) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of elements is not one."));
        }
        return m_values[0];
    }

    /*************************************************************************/
    inline constexpr T_Value value(void) const {
        if (this->number_of_elements() != 1) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of elements is not one."));
        }
        return m_values[0];
    }

    /*************************************************************************/
    inline constexpr T_Value &values(
        const std::vector<int> &a_MULTI_DIMENSIONAL_INDEX) {
        int multi_dimensional_index_size = a_MULTI_DIMENSIONAL_INDEX.size();
        if (this->number_of_dimensions() != multi_dimensional_index_size) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of dimensions does not match."));
        }

        auto flat_index = std::inner_product(a_MULTI_DIMENSIONAL_INDEX.begin(),
                                             a_MULTI_DIMENSIONAL_INDEX.end(),
                                             m_strides.begin(), 0);
        return m_values[flat_index];
    }

    /*************************************************************************/
    inline constexpr T_Value values(
        const std::vector<int> &a_MULTI_DIMENSIONAL_INDEX) const {
        int multi_dimensional_index_size = a_MULTI_DIMENSIONAL_INDEX.size();
        if (this->number_of_dimensions() != multi_dimensional_index_size) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of dimensions does not match."));
        }

        auto flat_index = std::inner_product(a_MULTI_DIMENSIONAL_INDEX.begin(),
                                             a_MULTI_DIMENSIONAL_INDEX.end(),
                                             m_strides.begin(), 0);
        return m_values[flat_index];
    }

    /*************************************************************************/
    template <class... Args>
    inline constexpr T_Value &values(Args... args) {
        return this->values({args...});
    }

    /*************************************************************************/
    template <class... Args>
    inline constexpr T_Value values(Args... args) const {
        return this->values({args...});
    }

    /*************************************************************************/
    inline constexpr std::string &name(void) {
        if (this->number_of_elements() != 1) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of elements is not one."));
        }
        return m_names[0];
    }

    /*************************************************************************/
    inline constexpr std::string &name(void) const {
        if (this->number_of_elements() != 1) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of elements is not one."));
        }
        return m_names[0];
    }

    /*************************************************************************/
    inline constexpr std::string &names(
        const std::vector<int> &a_MULTI_DIMENSIONAL_INDEX) {
        if (this->number_of_dimensions() != a_MULTI_DIMENSIONAL_INDEX.size()) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of dimensions does not match."));
        }

        auto flat_index = std::inner_product(a_MULTI_DIMENSIONAL_INDEX.begin(),
                                             a_MULTI_DIMENSIONAL_INDEX.end(),
                                             m_strides.begin(), 0);
        return m_names[flat_index];
    }

    /*************************************************************************/
    inline std::string &names(
        const std::vector<int> &a_MULTI_DIMENSIONAL_INDEX) const {
        if (this->number_of_dimensions() != a_MULTI_DIMENSIONAL_INDEX.size()) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of dimensions does not match."));
        }

        auto flat_index = std::inner_product(a_MULTI_DIMENSIONAL_INDEX.begin(),
                                             a_MULTI_DIMENSIONAL_INDEX.end(),
                                             m_strides.begin(), 0);
        return m_names[flat_index];
    }

    /*************************************************************************/
    template <class... Args>
    inline constexpr std::string &names(Args... args) {
        return this->names({args...});
    }

    /*************************************************************************/
    template <class... Args>
    inline std::string names(Args... args) const {
        /// This method cannot be constexpr for Clang-6.
        return this->names({args...});
    }

    /*************************************************************************/
    inline constexpr void fill(const T_Value a_VALUE) {
        for (auto &&value : m_values) {
            value = a_VALUE;
        }
    }

    /*************************************************************************/
    inline constexpr ValueProxy<T_Value> operator=(const T_Value a_VALUE) {
        if (this->number_of_elements() != 1) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of elements is not one."));
        }
        m_values[0] = a_VALUE;
        return *this;
    }

    /*************************************************************************/
    inline constexpr T_Value &operator[](const int a_FLAT_INDEX) {
        return m_values[a_FLAT_INDEX];
    }

    /*************************************************************************/
    inline constexpr T_Value operator[](const int a_FLAT_INDEX) const {
        return m_values[a_FLAT_INDEX];
    }

    /*************************************************************************/
    template <class... Args>
    inline constexpr T_Value &operator()(Args... args) {
        return this->values({args...});
    }

    /*************************************************************************/
    template <class... Args>
    inline constexpr T_Value operator()(Args... args) const {
        return this->values({args...});
    }

};  // namespace model
}  // namespace model
}  // namespace cppmh
#endif

/*****************************************************************************/
// END
/*****************************************************************************/
