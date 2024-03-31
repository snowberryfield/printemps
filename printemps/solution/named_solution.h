/*****************************************************************************/
// Copyright (c) 2020-2024 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLUTION_NAMED_SOLUTION_H__
#define PRINTEMPS_SOLUTION_NAMED_SOLUTION_H__

namespace printemps::solution {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class NamedSolution {
    friend class model::Model<T_Variable, T_Expression>;

   private:
    std::unordered_map<std::string, multi_array::ValueProxy<T_Variable>>
        m_variable_value_proxies;
    std::unordered_map<std::string, multi_array::ValueProxy<T_Expression>>
        m_expression_value_proxies;
    std::unordered_map<std::string, multi_array::ValueProxy<T_Expression>>
        m_constraint_value_proxies;
    std::unordered_map<std::string, multi_array::ValueProxy<T_Expression>>
        m_violation_value_proxies;

    std::string  m_name;
    int          m_number_of_variables;
    int          m_number_of_constraints;
    T_Expression m_objective;
    T_Expression m_total_violation;
    double       m_global_augmented_objective;
    bool         m_is_feasible;

   public:
    /*************************************************************************/
    NamedSolution(void) {
        this->initialize();
    }

    /*************************************************************************/
    void initialize(void) {
        m_variable_value_proxies.clear();
        m_expression_value_proxies.clear();
        m_constraint_value_proxies.clear();
        m_violation_value_proxies.clear();

        m_name                       = "";
        m_number_of_variables        = 0;
        m_number_of_constraints      = 0;
        m_objective                  = 0;
        m_total_violation            = 0;
        m_global_augmented_objective = 0.0;
        m_is_feasible                = false;
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
    void write_json_by_name(const std::string& a_FILE_NAME) const {
        utility::json::JsonObject object;

        // Summary
        object.emplace_back("version", constant::VERSION);
        object.emplace_back("name", m_name);
        object.emplace_back("number_of_variables", m_number_of_variables);
        object.emplace_back("number_of_constraints", m_number_of_constraints);
        object.emplace_back("is_found_feasible_solution", m_is_feasible);
        object.emplace_back("objective", m_objective);
        object.emplace_back("total_violation", m_total_violation);

        /// Decision variables
        object.emplace_back(
            "variables",  //
            multi_array::create_json_object(m_variable_value_proxies));

        /// Expressions
        object.emplace_back(
            "expressions",  //
            multi_array::create_json_object(m_expression_value_proxies));

        /// Constraints
        object.emplace_back(
            "constraints",  //
            multi_array::create_json_object(m_constraint_value_proxies));

        /// Violations
        object.emplace_back(
            "violations",  //
            multi_array::create_json_object(m_violation_value_proxies));

        utility::json::write_json_object(object, a_FILE_NAME);
    }

    /*************************************************************************/
    void write_json_by_array(const std::string& a_FILE_NAME) const {
        utility::json::JsonObject object;

        // Summary
        object.emplace_back("version", constant::VERSION);
        object.emplace_back("name", m_name);
        object.emplace_back("number_of_variables", m_number_of_variables);
        object.emplace_back("number_of_constraints", m_number_of_constraints);
        object.emplace_back("is_found_feasible_solution", m_is_feasible);
        object.emplace_back("objective", m_objective);
        object.emplace_back("total_violation", m_total_violation);

        /// Decision variables
        object.emplace_back(
            "variables",  //
            multi_array::create_json_array(m_variable_value_proxies));

        /// Expressions
        object.emplace_back(
            "expressions",  //
            multi_array::create_json_array(m_expression_value_proxies));

        /// Constraints
        object.emplace_back(
            "constraints",  //
            multi_array::create_json_array(m_constraint_value_proxies));

        /// Violations
        object.emplace_back(
            "violations",  //
            multi_array::create_json_array(m_violation_value_proxies));

        utility::json::write_json_object(object, a_FILE_NAME);
    }

    /*************************************************************************/
    void write_solution(const std::string& a_FILE_NAME) const {
        std::ofstream ofs(a_FILE_NAME.c_str());

        if (m_is_feasible) {
            ofs << "=obj= " << utility::to_string(m_objective, "%.10e")
                << std::endl;
        } else {
            ofs << "=infeas=" << std::endl;
        }
        for (const auto& item : m_variable_value_proxies) {
            const auto& proxy              = item.second;
            int         number_of_elements = proxy.number_of_elements();
            for (auto i = 0; i < number_of_elements; i++) {
                ofs << proxy.flat_indexed_names(i) << " "
                    << proxy.flat_indexed_values(i) << std::endl;
            }
        }

        ofs.close();
    }

    /*************************************************************************/
    inline const std::unordered_map<std::string,
                                    multi_array::ValueProxy<T_Variable>>&
    variables(void) const {
        return m_variable_value_proxies;
    }

    /*************************************************************************/
    inline const multi_array::ValueProxy<T_Variable>& variables(
        const std::string& a_NAME) const {
        return m_variable_value_proxies.at(a_NAME);
    }

    /*************************************************************************/
    inline const std::unordered_map<std::string,
                                    multi_array::ValueProxy<T_Expression>>&
    expressions(void) const {
        return m_expression_value_proxies;
    }

    /*************************************************************************/
    inline const multi_array::ValueProxy<T_Expression>& expressions(
        const std::string& a_NAME) const {
        return m_expression_value_proxies.at(a_NAME);
    }

    /*************************************************************************/
    inline const std::unordered_map<std::string,
                                    multi_array::ValueProxy<T_Expression>>&
    constraints(void) const {
        return m_constraint_value_proxies;
    }

    /*************************************************************************/
    inline const multi_array::ValueProxy<T_Expression>& constraints(
        const std::string& a_NAME) const {
        return m_constraint_value_proxies.at(a_NAME);
    }

    /*************************************************************************/
    inline const std::unordered_map<std::string,
                                    multi_array::ValueProxy<T_Expression>>&
    violations(void) const {
        return m_violation_value_proxies;
    }

    /*************************************************************************/
    inline const multi_array::ValueProxy<T_Expression>& violations(
        const std::string& a_NAME) const {
        return m_violation_value_proxies.at(a_NAME);
    }

    /*************************************************************************/
    inline const std::string& name(void) const {
        return m_name;
    }

    /*************************************************************************/
    inline int number_of_variables(void) const {
        return m_number_of_variables;
    }

    /*************************************************************************/
    inline int number_of_constraints(void) const {
        return m_number_of_constraints;
    }

    /*************************************************************************/
    inline T_Expression objective(void) const {
        return m_objective;
    }

    /*************************************************************************/
    inline T_Expression total_violation(void) const {
        return m_total_violation;
    }

    /*************************************************************************/
    inline bool is_feasible(void) const {
        return m_is_feasible;
    }
};
using IPNamedSolution = NamedSolution<int, double>;
}  // namespace printemps::solution
#endif
/*****************************************************************************/
// END
/*****************************************************************************/