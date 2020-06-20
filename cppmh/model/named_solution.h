/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef CPPMH_MODEL_NAMED_SOLUTION_H__
#define CPPMH_MODEL_NAMED_SOLUTION_H__

#include <vector>
#include <iostream>
#include <fstream>
#include <unordered_map>

namespace cppmh {
namespace model {
/*****************************************************************************/
template <class T_Value>
class ValueProxy;

/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Model;

/*****************************************************************************/
template <class T_Variable, class T_Expression>
class NamedSolution {
    friend class Model<T_Variable, T_Expression>;

   private:
    std::unordered_map<std::string, ValueProxy<T_Variable>>
        m_variable_value_proxies;
    std::unordered_map<std::string, ValueProxy<T_Expression>>
        m_expression_value_proxies;
    std::unordered_map<std::string, ValueProxy<T_Expression>>
        m_constraint_value_proxies;
    std::unordered_map<std::string, ValueProxy<T_Expression>>
        m_violation_value_proxies;

    T_Expression m_objective;
    bool         m_is_feasible;

    /*************************************************************************/
    template <class T_Value>
    inline static void print_values(
        const std::unordered_map<std::string, ValueProxy<T_Value>>
                           a_VALUE_PROXIES,
        const std::string& a_CATEGORY) {
        for (const auto& item : a_VALUE_PROXIES) {
            auto& proxy              = item.second;
            int   number_of_elements = proxy.number_of_elements();
            for (auto i = 0; i < number_of_elements; i++) {
                utility::print(a_CATEGORY + "." + proxy.flat_indexed_names(i) +
                               " = " +
                               std::to_string(proxy.flat_indexed_values(i)));
            }
        }
    }

    /*************************************************************************/
    template <class T_Value>
    inline static void write_values_by_name(
        std::ofstream* a_ofs,
        const std::unordered_map<std::string, ValueProxy<T_Value>>
                           a_VALUE_PROXIES,
        const std::string& a_CATEGORY, const int a_INDENT_LEVEL) {
        int indent_level = a_INDENT_LEVEL;

        *a_ofs << indent_spaces(indent_level) << "\"" + a_CATEGORY + "\" : {"
               << std::endl;
        indent_level++;

        int count              = 0;
        int value_proxies_size = a_VALUE_PROXIES.size();
        for (const auto& item : a_VALUE_PROXIES) {
            auto& proxy              = item.second;
            int   number_of_elements = proxy.number_of_elements();
            for (auto i = 0; i < number_of_elements; i++) {
                *a_ofs << indent_spaces(indent_level)
                       << "\"" + proxy.flat_indexed_names(i) + "\" : " +
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
        *a_ofs << indent_spaces(indent_level) << "}," << std::endl;
    }

    /*************************************************************************/
    template <class T_Value>
    inline static void write_values_by_array(
        std::ofstream* a_ofs,
        const std::unordered_map<std::string, ValueProxy<T_Value>>
                           a_VALUE_PROXIES,
        const std::string& a_CATEGORY, const int a_INDENT_LEVEL) {
        int indent_level = a_INDENT_LEVEL;

        *a_ofs << indent_spaces(indent_level) << "\"" + a_CATEGORY + "\" : {"
               << std::endl;
        indent_level++;

        int count              = 0;
        int value_proxies_size = a_VALUE_PROXIES.size();
        for (const auto& item : a_VALUE_PROXIES) {
            auto& proxy                = item.second;
            int   number_of_dimensions = proxy.number_of_dimensions();
            int   number_of_elements   = proxy.number_of_elements();

            *a_ofs << indent_spaces(indent_level)
                   << "\"" + item.first + "\" : [" << std::endl;
            indent_level++;

            int current_dimension = 0;
            for (auto i = 0; i < number_of_elements; i++) {
                std::vector<int> index = proxy.multi_dimensional_index(i);

                for (auto j = current_dimension; j < number_of_dimensions - 1;
                     j++) {
                    if (index[j + 1] == 0) {
                        *a_ofs << indent_spaces(indent_level) << "["
                               << std::endl;
                        indent_level++;
                        current_dimension++;
                    } else {
                        break;
                    }
                }
                if (index[current_dimension] ==
                    proxy.shape()[current_dimension] - 1) {
                    *a_ofs << indent_spaces(indent_level)
                           << std::to_string(proxy.flat_indexed_values(i))
                           << std::endl;
                } else {
                    *a_ofs << indent_spaces(indent_level)
                           << std::to_string(proxy.flat_indexed_values(i))
                           << "," << std::endl;
                }

                for (auto j = current_dimension; j > 0; j--) {
                    if (index[j] == proxy.shape()[j] - 1) {
                        indent_level--;
                        current_dimension--;
                        if (index[j - 1] == proxy.shape()[j - 1] - 1) {
                            *a_ofs << indent_spaces(indent_level) << "]"
                                   << std::endl;
                        } else {
                            *a_ofs << indent_spaces(indent_level) << "],"
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
                *a_ofs << indent_spaces(indent_level) << "]" << std::endl;
            } else {
                *a_ofs << indent_spaces(indent_level) << "]," << std::endl;
            }
        }

        indent_level--;
        *a_ofs << indent_spaces(indent_level) << "}," << std::endl;
    }

    /*************************************************************************/
    inline static std::string indent_spaces(const int a_INDENT_LEVEL) {
        std::string indent_width  = "    ";
        std::string indent_spaces = "";
        for (auto i = 0; i < a_INDENT_LEVEL; i++) {
            indent_spaces += indent_width;
        }
        return indent_spaces;
    }

   public:
    /*************************************************************************/
    NamedSolution(void) {
        this->initialize();
    }

    /*************************************************************************/
    virtual ~NamedSolution(void) {
        /// nothing to do
    }

    /*************************************************************************/
    inline void initialize(void) {
        m_objective   = 0;
        m_is_feasible = false;
    }

    /*************************************************************************/
    inline void print_variable_values(void) const {
        print_values(m_variable_value_proxies, "variable");
    }

    /*************************************************************************/
    inline void print_expression_values(void) const {
        print_values(m_expression_value_proxies, "expression");
    }

    /*************************************************************************/
    inline void print_constraint_values(void) const {
        print_values(m_constraint_value_proxies, "constraint");
    }

    /*************************************************************************/
    inline void print_violation_values(void) const {
        print_values(m_violation_value_proxies, "violation");
    }

    /*************************************************************************/
    inline void write_json_by_name(const std::string& a_FILE_NAME) const {
        int indent_level = 0;

        std::ofstream ofs(a_FILE_NAME.c_str());
        ofs << indent_spaces(indent_level) << "{" << std::endl;
        indent_level++;

        /// Decision variables
        write_values_by_name(&ofs, m_variable_value_proxies, "variables",
                             indent_level);

        /// Expressions
        write_values_by_name(&ofs, m_expression_value_proxies, "expressions",
                             indent_level);

        /// Constraints
        write_values_by_name(&ofs, m_constraint_value_proxies, "constraints",
                             indent_level);

        /// Violations
        write_values_by_name(&ofs, m_violation_value_proxies, "violations",
                             indent_level);

        /// Others
        ofs << indent_spaces(indent_level)
            << "\"is_feasible\" : " << (m_is_feasible ? "true," : "false,")
            << std::endl;
        ofs << indent_spaces(indent_level)
            << "\"objective\" : " + std::to_string(m_objective) << std::endl;

        indent_level--;
        ofs << indent_spaces(indent_level) << "}" << std::endl;
        ofs.close();
    }

    /*************************************************************************/
    inline void write_json_by_array(const std::string& a_FILE_NAME) const {
        int indent_level = 0;

        std::ofstream ofs(a_FILE_NAME.c_str());
        ofs << indent_spaces(indent_level) << "{" << std::endl;
        indent_level++;

        /// Decision variables
        write_values_by_array(&ofs, m_variable_value_proxies, "variables",
                              indent_level);

        /// Expressions
        write_values_by_array(&ofs, m_expression_value_proxies, "expressions",
                              indent_level);

        /// Constraints
        write_values_by_array(&ofs, m_constraint_value_proxies, "constraints",
                              indent_level);

        /// Violations
        write_values_by_array(&ofs, m_violation_value_proxies, "violations",
                              indent_level);

        /// Others
        ofs << indent_spaces(indent_level)
            << "\"is_feasible\" : " << (m_is_feasible ? "true," : "false,")
            << std::endl;
        ofs << indent_spaces(indent_level)
            << "\"objective\" : " + std::to_string(m_objective) << std::endl;

        indent_level--;
        ofs << indent_spaces(indent_level) << "}" << std::endl;
        ofs.close();
    }

    /*************************************************************************/
    inline void write_solution(const std::string& a_FILE_NAME) const {
        std::ofstream ofs(a_FILE_NAME.c_str());
        if (m_is_feasible) {
            ofs << "=obj= " << m_objective << std::endl;
            for (const auto& item : m_variable_value_proxies) {
                auto& proxy              = item.second;
                int   number_of_elements = proxy.number_of_elements();
                for (auto i = 0; i < number_of_elements; i++) {
                    ofs << proxy.flat_indexed_names(i) << " "
                        << proxy.flat_indexed_values(i) << std::endl;
                }
            }
        } else {
            ofs << "=infeas=" << std::endl;
        }
        ofs.close();
    }

    /*************************************************************************/
    inline const std::unordered_map<std::string, ValueProxy<T_Variable>>&
    variables(void) const {
        return m_variable_value_proxies;
    }

    /*************************************************************************/
    inline const ValueProxy<T_Variable>& variables(
        const std::string& a_NAME) const {
        return m_variable_value_proxies.at(a_NAME);
    }

    /*************************************************************************/
    inline const std::unordered_map<std::string, ValueProxy<T_Expression>>&
    expressions(void) const {
        return m_expression_value_proxies;
    }

    /*************************************************************************/
    inline const ValueProxy<T_Expression>& expressions(
        const std::string& a_NAME) const {
        return m_expression_value_proxies.at(a_NAME);
    }

    /*************************************************************************/
    inline const std::unordered_map<std::string, ValueProxy<T_Expression>>&
    constraints(void) const {
        return m_constraint_value_proxies;
    }

    /*************************************************************************/
    inline const ValueProxy<T_Expression>& constraints(
        const std::string& a_NAME) const {
        return m_constraint_value_proxies.at(a_NAME);
    }

    /*************************************************************************/
    inline const std::unordered_map<std::string, ValueProxy<T_Expression>>&
    violations(void) const {
        return m_violation_value_proxies;
    }

    /*************************************************************************/
    inline const ValueProxy<T_Expression>& violations(
        const std::string& a_NAME) const {
        return m_violation_value_proxies.at(a_NAME);
    }

    /*************************************************************************/
    inline T_Expression objective(void) const {
        return m_objective;
    }

    /*************************************************************************/
    inline bool is_feasible(void) const {
        return m_is_feasible;
    }
};
using IPNamedSolution = NamedSolution<int, double>;
}  // namespace model
}  // namespace cppmh
#endif
/*****************************************************************************/
// END
/*****************************************************************************/