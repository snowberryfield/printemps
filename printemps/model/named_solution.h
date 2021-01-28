/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MODEL_NAMED_SOLUTION_H__
#define PRINTEMPS_MODEL_NAMED_SOLUTION_H__

#include <vector>
#include <iostream>
#include <fstream>
#include <unordered_map>

namespace printemps {
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
    T_Expression m_total_violation;
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
    void initialize(void) {
        m_variable_value_proxies.clear();
        m_expression_value_proxies.clear();
        m_constraint_value_proxies.clear();
        m_violation_value_proxies.clear();

        m_objective       = 0;
        m_total_violation = 0;
        m_is_feasible     = false;
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
    void write_json_by_name(const std::string&  a_FILE_NAME,
                            const ModelSummary& a_MODEL_SUMMARY) const {
        int indent_level = 0;

        std::ofstream ofs(a_FILE_NAME.c_str());
        ofs << utility::indent_spaces(indent_level) << "{" << std::endl;
        indent_level++;

        /// Summary
        ofs << utility::indent_spaces(indent_level) << "\"version\" : "
            << "\"" << constant::VERSION << "\"," << std::endl;

        ofs << utility::indent_spaces(indent_level) << "\"name\" : "
            << "\"" << a_MODEL_SUMMARY.name << "\"," << std::endl;

        ofs << utility::indent_spaces(indent_level)
            << "\"number_of_variables\" : "
            << a_MODEL_SUMMARY.number_of_variables << "," << std::endl;

        ofs << utility::indent_spaces(indent_level)
            << "\"number_of_constraints\" : "
            << a_MODEL_SUMMARY.number_of_constraints << "," << std::endl;

        ofs << utility::indent_spaces(indent_level)
            << "\"is_feasible\" : " << (m_is_feasible ? "true," : "false,")
            << std::endl;

        ofs << utility::indent_spaces(indent_level)
            << "\"objective\" : " + utility::to_string(m_objective, "%.10e")
            << "," << std::endl;

        ofs << utility::indent_spaces(indent_level)
            << "\"total_violation\" : " +
                   utility::to_string(m_total_violation, "%.10e")
            << "," << std::endl;

        /// Decision variables
        write_values_by_name(&ofs,                      //
                             m_variable_value_proxies,  //
                             "variables",               //
                             indent_level,              //
                             "%d",                      //
                             true);

        /// Expressions
        write_values_by_name(&ofs,                        //
                             m_expression_value_proxies,  //
                             "expressions",               //
                             indent_level,                //
                             "%.10e",                     //
                             true);

        /// Constraints
        write_values_by_name(&ofs,                        //
                             m_constraint_value_proxies,  //
                             "constraints",               //
                             indent_level,                //
                             "%.10e",                     //
                             true);

        /// Violations
        write_values_by_name(&ofs,                       //
                             m_violation_value_proxies,  //
                             "violations",               //
                             indent_level,               //
                             "%.10e",                    //
                             false);

        indent_level--;
        ofs << utility::indent_spaces(indent_level) << "}" << std::endl;
        ofs.close();
    }

    /*************************************************************************/
    void write_json_by_array(const std::string&  a_FILE_NAME,
                             const ModelSummary& a_MODEL_SUMMARY) const {
        int indent_level = 0;

        std::ofstream ofs(a_FILE_NAME.c_str());
        ofs << utility::indent_spaces(indent_level) << "{" << std::endl;
        indent_level++;

        /// Summary
        ofs << utility::indent_spaces(indent_level) << "\"version\" : "
            << "\"" << constant::VERSION << "\"," << std::endl;

        ofs << utility::indent_spaces(indent_level) << "\"name\" : "
            << "\"" << a_MODEL_SUMMARY.name << "\"," << std::endl;

        ofs << utility::indent_spaces(indent_level)
            << "\"number_of_variables\" : "
            << a_MODEL_SUMMARY.number_of_variables << "," << std::endl;

        ofs << utility::indent_spaces(indent_level)
            << "\"number_of_constraints\" : "
            << a_MODEL_SUMMARY.number_of_constraints << "," << std::endl;

        ofs << utility::indent_spaces(indent_level)
            << "\"is_feasible\" : " << (m_is_feasible ? "true," : "false,")
            << std::endl;

        ofs << utility::indent_spaces(indent_level)
            << "\"objective\" : " + utility::to_string(m_objective, "%.10e")
            << "," << std::endl;

        ofs << utility::indent_spaces(indent_level)
            << "\"total_violation\" : " +
                   utility::to_string(m_total_violation, "%.10e")
            << "," << std::endl;

        /// Decision variables
        write_values_by_array(&ofs,                      //
                              m_variable_value_proxies,  //
                              "variables",               //
                              indent_level,              //
                              "%d",                      //
                              true);

        /// Expressions
        write_values_by_array(&ofs,                        //
                              m_expression_value_proxies,  //
                              "expressions",               //
                              indent_level,                //
                              "%.10e",                     //
                              true);

        /// Constraints
        write_values_by_array(&ofs,                        //
                              m_constraint_value_proxies,  //
                              "constraints",               //
                              indent_level,                //
                              "%.10e",                     //
                              true);

        /// Violations
        write_values_by_array(&ofs,                       //
                              m_violation_value_proxies,  //
                              "violations",               //
                              indent_level,               //
                              "%.10e",                    //
                              false);

        indent_level--;
        ofs << utility::indent_spaces(indent_level) << "}" << std::endl;
        ofs.close();
    }

    /*************************************************************************/
    void write_solution(const std::string& a_FILE_NAME) const {
        std::ofstream ofs(a_FILE_NAME.c_str());

        if (m_is_feasible) {
            ofs << "=obj= " << utility::to_string(m_objective, "%.10e")
                << std::endl;
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
    inline constexpr const std::unordered_map<std::string,
                                              ValueProxy<T_Variable>>&
    variables(void) const {
        return m_variable_value_proxies;
    }

    /*************************************************************************/
    inline constexpr const ValueProxy<T_Variable>& variables(
        const std::string& a_NAME) const {
        return m_variable_value_proxies.at(a_NAME);
    }

    /*************************************************************************/
    inline constexpr const std::unordered_map<std::string,
                                              ValueProxy<T_Expression>>&
    expressions(void) const {
        return m_expression_value_proxies;
    }

    /*************************************************************************/
    inline constexpr const ValueProxy<T_Expression>& expressions(
        const std::string& a_NAME) const {
        return m_expression_value_proxies.at(a_NAME);
    }

    /*************************************************************************/
    inline constexpr const std::unordered_map<std::string,
                                              ValueProxy<T_Expression>>&
    constraints(void) const {
        return m_constraint_value_proxies;
    }

    /*************************************************************************/
    inline constexpr const ValueProxy<T_Expression>& constraints(
        const std::string& a_NAME) const {
        return m_constraint_value_proxies.at(a_NAME);
    }

    /*************************************************************************/
    inline constexpr const std::unordered_map<std::string,
                                              ValueProxy<T_Expression>>&
    violations(void) const {
        return m_violation_value_proxies;
    }

    /*************************************************************************/
    inline constexpr const ValueProxy<T_Expression>& violations(
        const std::string& a_NAME) const {
        return m_violation_value_proxies.at(a_NAME);
    }

    /*************************************************************************/
    inline constexpr T_Expression objective(void) const {
        return m_objective;
    }

    /*************************************************************************/
    inline constexpr T_Expression total_violation(void) const {
        return m_total_violation;
    }

    /*************************************************************************/
    inline constexpr bool is_feasible(void) const {
        return m_is_feasible;
    }
};
using IPNamedSolution = NamedSolution<int, double>;
}  // namespace model
}  // namespace printemps
#endif
/*****************************************************************************/
// END
/*****************************************************************************/