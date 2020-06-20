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
        ofs << utility::indent_spaces(indent_level) << "{" << std::endl;
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
        ofs << utility::indent_spaces(indent_level)
            << "\"is_feasible\" : " << (m_is_feasible ? "true," : "false,")
            << std::endl;
        ofs << utility::indent_spaces(indent_level)
            << "\"objective\" : " + std::to_string(m_objective) << std::endl;

        indent_level--;
        ofs << utility::indent_spaces(indent_level) << "}" << std::endl;
        ofs.close();
    }

    /*************************************************************************/
    inline void write_json_by_array(const std::string& a_FILE_NAME) const {
        int indent_level = 0;

        std::ofstream ofs(a_FILE_NAME.c_str());
        ofs << utility::indent_spaces(indent_level) << "{" << std::endl;
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
        ofs << utility::indent_spaces(indent_level)
            << "\"is_feasible\" : " << (m_is_feasible ? "true," : "false,")
            << std::endl;
        ofs << utility::indent_spaces(indent_level)
            << "\"objective\" : " + std::to_string(m_objective) << std::endl;

        indent_level--;
        ofs << utility::indent_spaces(indent_level) << "}" << std::endl;
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