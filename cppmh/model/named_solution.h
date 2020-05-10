/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef CPPMH_MODEL_NAMED_SOLUTION_H__
#define CPPMH_MODEL_NAMED_SOLUTION_H__

#include <vector>
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
        for (const auto& umap : a_VALUE_PROXIES) {
            auto& proxy              = umap.second;
            int   number_of_elements = proxy.number_of_elements();
            for (auto i = 0; i < number_of_elements; i++) {
                utility::print(a_CATEGORY + "." + proxy.flat_indexed_names(i) +
                               " = " +
                               std::to_string(proxy.flat_indexed_values(i)));
            }
        }
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