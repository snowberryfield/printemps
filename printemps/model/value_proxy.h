/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/

#ifndef printemps_MODEL_VALUE_PROXY_H__
#define printemps_MODEL_VALUE_PROXY_H__

#include "abstract_multi_array.h"
#include <vector>
#include <iostream>
#include <fstream>

namespace printemps {
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
    inline constexpr const std::string &flat_indexed_names(
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
    inline constexpr const std::string &name(void) const {
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
    inline constexpr const std::string &names(
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
    inline constexpr const std::string &names(Args... args) const {
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
};

/*****************************************************************************/
template <class T_Value>
inline constexpr bool operator==(const ValueProxy<T_Value> &a_LEFT,
                                 const ValueProxy<T_Value> &a_RIGHT) {
    return a_LEFT.flat_indexed_values() == a_RIGHT.flat_indexed_values();
}

/*****************************************************************************/
template <class T_Value>
inline constexpr bool operator!=(const ValueProxy<T_Value> &a_LEFT,
                                 const ValueProxy<T_Value> &a_RIGHT) {
    return !(a_LEFT == a_RIGHT);
}

/*****************************************************************************/
template <class T_Value>
inline void print_values(
    const std::unordered_map<std::string, ValueProxy<T_Value>> a_VALUE_PROXIES,
    const std::string &                                        a_CATEGORY) {
    for (const auto &item : a_VALUE_PROXIES) {
        auto &proxy              = item.second;
        int   number_of_elements = proxy.number_of_elements();
        for (auto i = 0; i < number_of_elements; i++) {
            utility::print(a_CATEGORY + "." + proxy.flat_indexed_names(i) +
                           " = " +
                           std::to_string(proxy.flat_indexed_values(i)));
        }
    }
}

/*****************************************************************************/
template <class T_Value>
inline void write_values_by_name(
    std::ofstream *                                            a_ofs,
    const std::unordered_map<std::string, ValueProxy<T_Value>> a_VALUE_PROXIES,
    const std::string &a_CATEGORY, const int a_INDENT_LEVEL) {
    int indent_level = a_INDENT_LEVEL;

    *a_ofs << utility::indent_spaces(indent_level)
           << "\"" + a_CATEGORY + "\" : {" << std::endl;
    indent_level++;

    int count              = 0;
    int value_proxies_size = a_VALUE_PROXIES.size();
    for (const auto &item : a_VALUE_PROXIES) {
        auto &proxy              = item.second;
        int   number_of_elements = proxy.number_of_elements();
        for (auto i = 0; i < number_of_elements; i++) {
            *a_ofs << utility::indent_spaces(indent_level)
                   << "\"" +
                          utility::delete_space(proxy.flat_indexed_names(i)) +
                          "\" : " +
                          std::to_string(proxy.flat_indexed_values(i));

            if ((i == number_of_elements - 1) &&
                (count == value_proxies_size - 1)) {
                *a_ofs << std::endl;
            } else {
                *a_ofs << "," << std::endl;
            }
        }
        count++;
    }
    indent_level--;
    *a_ofs << utility::indent_spaces(indent_level) << "}" << std::endl;
}

/*****************************************************************************/
template <class T_Value>
inline void write_values_by_array(
    std::ofstream *                                            a_ofs,
    const std::unordered_map<std::string, ValueProxy<T_Value>> a_VALUE_PROXIES,
    const std::string &a_CATEGORY, const int a_INDENT_LEVEL) {
    int indent_level = a_INDENT_LEVEL;

    *a_ofs << utility::indent_spaces(indent_level)
           << "\"" + a_CATEGORY + "\" : {" << std::endl;
    indent_level++;

    int count              = 0;
    int value_proxies_size = a_VALUE_PROXIES.size();
    for (const auto &item : a_VALUE_PROXIES) {
        auto &proxy                = item.second;
        int   number_of_dimensions = proxy.number_of_dimensions();
        int   number_of_elements   = proxy.number_of_elements();

        *a_ofs << utility::indent_spaces(indent_level)
               << "\"" + item.first + "\" : [" << std::endl;
        indent_level++;

        int current_dimension = 0;
        for (auto i = 0; i < number_of_elements; i++) {
            std::vector<int> index = proxy.multi_dimensional_index(i);

            for (auto j = current_dimension; j < number_of_dimensions - 1;
                 j++) {
                if (index[j + 1] == 0) {
                    *a_ofs << utility::indent_spaces(indent_level) << "["
                           << std::endl;
                    indent_level++;
                    current_dimension++;
                } else {
                    break;
                }
            }
            if (index[current_dimension] ==
                proxy.shape()[current_dimension] - 1) {
                *a_ofs << utility::indent_spaces(indent_level)
                       << std::to_string(proxy.flat_indexed_values(i))
                       << std::endl;
            } else {
                *a_ofs << utility::indent_spaces(indent_level)
                       << std::to_string(proxy.flat_indexed_values(i)) << ","
                       << std::endl;
            }

            for (auto j = current_dimension; j > 0; j--) {
                if (index[j] == proxy.shape()[j] - 1) {
                    indent_level--;
                    current_dimension--;
                    if (index[j - 1] == proxy.shape()[j - 1] - 1) {
                        *a_ofs << utility::indent_spaces(indent_level) << "]"
                               << std::endl;
                    } else {
                        *a_ofs << utility::indent_spaces(indent_level) << "],"
                               << std::endl;
                    }
                } else {
                    break;
                }
            }
        }

        indent_level--;
        count++;

        if (count == value_proxies_size) {
            *a_ofs << utility::indent_spaces(indent_level) << "]" << std::endl;
        } else {
            *a_ofs << utility::indent_spaces(indent_level) << "]," << std::endl;
        }
    }

    indent_level--;
    *a_ofs << utility::indent_spaces(indent_level) << "}" << std::endl;
}
}  // namespace model
}  // namespace printemps
#endif

/*****************************************************************************/
// END
/*****************************************************************************/
