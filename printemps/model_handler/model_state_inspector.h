/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MODEL_HANDLER_MODEL_STATE_INSPECTOR_H__
#define PRINTEMPS_MODEL_HANDLER_MODEL_STATE_INSPECTOR_H__

namespace printemps::model_handler {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class ModelStateInspector {
   private:
    const model::Model<T_Variable, T_Expression> *m_model_ptr;

   public:
    /*************************************************************************/
    ModelStateInspector(void) {
        m_model_ptr = nullptr;
    }

    /*************************************************************************/
    ModelStateInspector(
        const model::Model<T_Variable, T_Expression> *a_model_ptr) {
        this->initialize();
        this->setup(a_model_ptr);
    }

    /*************************************************************************/
    inline void initialize(void) noexcept {
        m_model_ptr = nullptr;
    }

    /*************************************************************************/
    inline void setup(
        const model::Model<T_Variable, T_Expression> *a_model_ptr) noexcept {
        m_model_ptr = a_model_ptr;
    }

    /*************************************************************************/
    template <class T_Value>
    inline std::vector<multi_array::ValueProxy<T_Value>>
    generate_variable_parameter_proxies(const T_Value a_VALUE) const {
        const auto &MODEL = *m_model_ptr;

        std::vector<multi_array::ValueProxy<T_Value>>
            variable_parameter_proxies;

        const auto &VARIABLE_PROXIES = MODEL.variable_proxies();
        variable_parameter_proxies.reserve(VARIABLE_PROXIES.size());

        for (const auto &proxy : VARIABLE_PROXIES) {
            multi_array::ValueProxy<T_Value> variable_parameter_proxy(
                proxy.index(), proxy.shape());
            variable_parameter_proxy.fill(a_VALUE);
            const int NUMBER_OF_ELEMENTS = proxy.number_of_elements();
            for (auto i = 0; i < NUMBER_OF_ELEMENTS; i++) {
                variable_parameter_proxy.flat_indexed_names(i) =
                    proxy.flat_indexed_variables(i).name();
            }

            variable_parameter_proxies.push_back(variable_parameter_proxy);
        }
        return variable_parameter_proxies;
    }

    /*************************************************************************/
    template <class T_Value>
    inline std::vector<multi_array::ValueProxy<T_Value>>
    generate_expression_parameter_proxies(const T_Value a_VALUE) const {
        const auto &MODEL = *m_model_ptr;

        std::vector<multi_array::ValueProxy<T_Value>>
            expression_parameter_proxies;

        const auto &EXPRESSION_PROXIES = MODEL.expression_proxies();
        expression_parameter_proxies.reserve(EXPRESSION_PROXIES.size());

        for (const auto &proxy : EXPRESSION_PROXIES) {
            multi_array::ValueProxy<T_Value> expression_parameter_proxy(
                proxy.index(), proxy.shape());
            expression_parameter_proxy.fill(a_VALUE);
            int NUMBER_OF_ELEMENTS = proxy.number_of_elements();
            for (auto i = 0; i < NUMBER_OF_ELEMENTS; i++) {
                expression_parameter_proxy.flat_indexed_names(i) =
                    proxy.flat_indexed_expressions(i).name();
            }
            expression_parameter_proxies.push_back(expression_parameter_proxy);
        }
        return expression_parameter_proxies;
    }

    /*************************************************************************/
    template <class T_Value>
    inline std::vector<multi_array::ValueProxy<T_Value>>
    generate_constraint_parameter_proxies(const T_Value a_VALUE) const {
        const auto &MODEL = *m_model_ptr;

        std::vector<multi_array::ValueProxy<T_Value>>
            constraint_parameter_proxies;

        const auto &CONSTRAINT_PROXIES = MODEL.constraint_proxies();
        constraint_parameter_proxies.reserve(CONSTRAINT_PROXIES.size());

        for (const auto &proxy : CONSTRAINT_PROXIES) {
            multi_array::ValueProxy<T_Value> constraint_parameter_proxy(
                proxy.index(), proxy.shape());
            constraint_parameter_proxy.fill(a_VALUE);
            const int NUMBER_OF_ELEMENTS = proxy.number_of_elements();
            for (auto i = 0; i < NUMBER_OF_ELEMENTS; i++) {
                constraint_parameter_proxy.flat_indexed_names(i) =
                    proxy.flat_indexed_constraints(i).name();
            }
            constraint_parameter_proxies.push_back(constraint_parameter_proxy);
        }
        return constraint_parameter_proxies;
    }

    /*************************************************************************/
    inline std::vector<multi_array::ValueProxy<double>>
    export_local_penalty_coefficient_proxies(void) const {
        const auto &MODEL = *m_model_ptr;

        std::vector<multi_array::ValueProxy<double>>
            local_penalty_coefficient_proxies;

        const auto &CONSTRAINT_PROXIES = MODEL.constraint_proxies();
        local_penalty_coefficient_proxies.reserve(CONSTRAINT_PROXIES.size());

        for (const auto &proxy : CONSTRAINT_PROXIES) {
            multi_array::ValueProxy<double> local_penalty_coefficient_proxy(
                proxy.index(), proxy.shape());

            const int NUMBER_OF_ELEMENTS = proxy.number_of_elements();

            for (auto i = 0; i < NUMBER_OF_ELEMENTS; i++) {
                local_penalty_coefficient_proxy.flat_indexed_names(i) =
                    proxy.flat_indexed_constraints(i).name();
                local_penalty_coefficient_proxy.flat_indexed_values(i) =
                    std::max(proxy.flat_indexed_constraints(i)
                                 .local_penalty_coefficient_less(),
                             proxy.flat_indexed_constraints(i)
                                 .local_penalty_coefficient_greater());
            }
            local_penalty_coefficient_proxies.push_back(
                local_penalty_coefficient_proxy);
        }
        return local_penalty_coefficient_proxies;
    }

    /*************************************************************************/
    inline std::vector<multi_array::ValueProxy<long>>
    export_update_count_proxies(void) const {
        const auto &MODEL = *m_model_ptr;

        std::vector<multi_array::ValueProxy<long>> update_count_proxies;

        const auto &VARIABLE_PROXIES = MODEL.variable_proxies();
        update_count_proxies.reserve(VARIABLE_PROXIES.size());

        for (const auto &proxy : VARIABLE_PROXIES) {
            multi_array::ValueProxy<long> update_count_proxy(proxy.index(),
                                                             proxy.shape());

            const int NUMBER_OF_ELEMENTS = proxy.number_of_elements();

            for (auto i = 0; i < NUMBER_OF_ELEMENTS; i++) {
                update_count_proxy.flat_indexed_names(i) =
                    proxy.flat_indexed_variables(i).name();
                update_count_proxy.flat_indexed_values(i) =
                    proxy.flat_indexed_variables(i).update_count();
            }
            update_count_proxies.push_back(update_count_proxy);
        }
        return update_count_proxies;
    }

    /*************************************************************************/
    inline std::vector<multi_array::ValueProxy<long>>
    export_violation_count_proxies(void) const {
        const auto &MODEL = *m_model_ptr;

        std::vector<multi_array::ValueProxy<long>> violation_count_proxies;

        const auto &CONSTRAINT_PROXIES = MODEL.constraint_proxies();
        violation_count_proxies.reserve(CONSTRAINT_PROXIES.size());

        for (const auto &proxy : CONSTRAINT_PROXIES) {
            multi_array::ValueProxy<long> violation_count_proxy(proxy.index(),
                                                                proxy.shape());

            const int NUMBER_OF_ELEMENTS = proxy.number_of_elements();

            for (auto i = 0; i < NUMBER_OF_ELEMENTS; i++) {
                violation_count_proxy.flat_indexed_names(i) =
                    proxy.flat_indexed_constraints(i).name();
                violation_count_proxy.flat_indexed_values(i) =
                    proxy.flat_indexed_constraints(i).violation_count();
            }
            violation_count_proxies.push_back(violation_count_proxy);
        }
        return violation_count_proxies;
    }

    /*************************************************************************/
    inline solution::DenseSolution<T_Variable, T_Expression>
    export_dense_solution(void) const {
        const auto &MODEL = *m_model_ptr;

        solution::DenseSolution<T_Variable, T_Expression> solution;

        const auto &VARIABLE_PROXIES   = MODEL.variable_proxies();
        const auto &EXPRESSION_PROXIES = MODEL.expression_proxies();
        const auto &CONSTRAINT_PROXIES = MODEL.constraint_proxies();

        /// Decision variables
        for (const auto &proxy : VARIABLE_PROXIES) {
            solution.variable_value_proxies.push_back(
                proxy.export_values_and_names());
        }

        /// Expressions
        for (const auto &proxy : EXPRESSION_PROXIES) {
            solution.expression_value_proxies.push_back(
                proxy.export_values_and_names());
        }

        /// Constraints
        for (const auto &proxy : CONSTRAINT_PROXIES) {
            solution.constraint_value_proxies.push_back(
                proxy.export_values_and_names());
            solution.violation_value_proxies.push_back(
                proxy.export_violations_and_names());
        }

        /// Total violation
        T_Expression total_violation = 0;
        for (const auto &proxy : CONSTRAINT_PROXIES) {
            for (const auto &constraint : proxy.flat_indexed_constraints()) {
                total_violation += constraint.violation_value();
            }
        }

        solution.objective       = MODEL.objective().value();
        solution.total_violation = total_violation;
        solution.global_augmented_objective =
            solution.objective +
            MODEL.global_penalty_coefficient() * total_violation;
        solution.is_feasible = MODEL.is_feasible();

        return solution;
    }

    /*************************************************************************/
    inline solution::SparseSolution<T_Variable, T_Expression>
    export_sparse_solution(void) const {
        const auto &MODEL = *m_model_ptr;

        solution::SparseSolution<T_Variable, T_Expression> solution;

        const auto &VARIABLE_PROXIES   = MODEL.variable_proxies();
        const auto &CONSTRAINT_PROXIES = MODEL.constraint_proxies();

        /// Decision variables
        for (const auto &proxy : VARIABLE_PROXIES) {
            for (const auto &variable : proxy.flat_indexed_variables()) {
                if (variable.value() != 0) {
                    solution.variables[variable.name()] = variable.value();
                }
            }
        }

        /// Total violation
        T_Expression total_violation = 0;
        for (const auto &proxy : CONSTRAINT_PROXIES) {
            for (const auto &constraint : proxy.flat_indexed_constraints()) {
                total_violation += constraint.violation_value();
            }
        }

        solution.objective       = MODEL.objective().value();
        solution.total_violation = total_violation;
        solution.global_augmented_objective =
            solution.objective +
            MODEL.global_penalty_coefficient() * total_violation;
        solution.is_feasible = MODEL.is_feasible();

        return solution;
    }

    /*************************************************************************/
    inline solution::NamedSolution<T_Variable, T_Expression>
    export_named_solution(void) const {
        const auto &MODEL = *m_model_ptr;

        solution::NamedSolution<T_Variable, T_Expression> solution;

        const auto &VARIABLE_PROXIES   = MODEL.variable_proxies();
        const auto &EXPRESSION_PROXIES = MODEL.expression_proxies();
        const auto &CONSTRAINT_PROXIES = MODEL.constraint_proxies();

        const int VARIABLE_PROXIES_SIZE   = VARIABLE_PROXIES.size();
        const int EXPRESSION_PROXIES_SIZE = EXPRESSION_PROXIES.size();
        const int CONSTRAINT_PROXIES_SIZE = CONSTRAINT_PROXIES.size();

        const auto &VARIABLE_NAMES   = MODEL.variable_names();
        const auto &EXPRESSION_NAMES = MODEL.expression_names();
        const auto &CONSTRAINT_NAMES = MODEL.constraint_names();

        /// Decision variables
        for (auto i = 0; i < VARIABLE_PROXIES_SIZE; i++) {
            solution.m_variable_value_proxies[VARIABLE_NAMES[i]] =
                VARIABLE_PROXIES[i].export_values_and_names();
        }

        /// Expression
        for (auto i = 0; i < EXPRESSION_PROXIES_SIZE; i++) {
            solution.m_expression_value_proxies[EXPRESSION_NAMES[i]] =
                EXPRESSION_PROXIES[i].export_values_and_names();
        }

        /// Constraint
        for (auto i = 0; i < CONSTRAINT_PROXIES_SIZE; i++) {
            solution.m_constraint_value_proxies[CONSTRAINT_NAMES[i]] =
                CONSTRAINT_PROXIES[i].export_values_and_names();
        }

        /// Violation
        for (auto i = 0; i < CONSTRAINT_PROXIES_SIZE; i++) {
            solution.m_violation_value_proxies[CONSTRAINT_NAMES[i]] =
                CONSTRAINT_PROXIES[i].export_violations_and_names();
        }

        /// Total violation
        T_Expression total_violation = 0;
        for (const auto &proxy : CONSTRAINT_PROXIES) {
            for (const auto &constraint : proxy.flat_indexed_constraints()) {
                total_violation += constraint.violation_value();
            }
        }

        solution.m_name = MODEL.name();
        solution.m_number_of_variables =
            MODEL.reference().number_of_variables();
        solution.m_number_of_constraints =
            MODEL.reference().number_of_constraints();
        solution.m_objective       = MODEL.objective().value();
        solution.m_total_violation = total_violation;

        solution.m_is_feasible = MODEL.is_feasible();

        return solution;
    }

    /*************************************************************************/
    inline std::unordered_map<std::string, multi_array::ValueProxy<double>>
    export_named_penalty_coefficients(void) {
        const auto &MODEL = *m_model_ptr;

        std::unordered_map<std::string, multi_array::ValueProxy<double>>
            named_penalty_coefficients;

        auto local_penalty_coefficient_proxies =
            this->export_local_penalty_coefficient_proxies();

        const auto &CONSTRAINT_PROXIES      = MODEL.constraint_proxies();
        const int   CONSTRAINT_PROXIES_SIZE = CONSTRAINT_PROXIES.size();
        const auto &CONSTRAINT_NAMES        = MODEL.constraint_names();

        for (auto i = 0; i < CONSTRAINT_PROXIES_SIZE; i++) {
            named_penalty_coefficients[CONSTRAINT_NAMES[i]] =
                local_penalty_coefficient_proxies[i];
        }
        return named_penalty_coefficients;
    }

    /*************************************************************************/
    inline std::unordered_map<std::string, multi_array::ValueProxy<long>>
    export_named_update_counts(void) {
        const auto &MODEL = *m_model_ptr;

        std::unordered_map<std::string, multi_array::ValueProxy<long>>
            named_update_counts;

        auto update_count_proxies = this->export_update_count_proxies();

        const auto &VARIABLE_PROXIES      = MODEL.variable_proxies();
        const int   VARIABLE_PROXIES_SIZE = VARIABLE_PROXIES.size();
        const auto &VARIABLE_NAMES        = MODEL.variable_names();

        for (auto i = 0; i < VARIABLE_PROXIES_SIZE; i++) {
            named_update_counts[VARIABLE_NAMES[i]] = update_count_proxies[i];
        }
        return named_update_counts;
    }

    /*************************************************************************/
    inline std::unordered_map<std::string, multi_array::ValueProxy<long>>
    export_named_violation_counts(void) {
        const auto &MODEL = *m_model_ptr;

        std::unordered_map<std::string, multi_array::ValueProxy<long>>
            named_violation_counts;

        auto violation_count_proxies = this->export_violation_count_proxies();

        const auto &CONSTRAINT_PROXIES      = MODEL.constraint_proxies();
        const int   CONSTRAINT_PROXIES_SIZE = CONSTRAINT_PROXIES.size();
        const auto &CONSTRAINT_NAMES        = MODEL.constraint_names();

        for (auto i = 0; i < CONSTRAINT_PROXIES_SIZE; i++) {
            named_violation_counts[CONSTRAINT_NAMES[i]] =
                violation_count_proxies[i];
        }
        return named_violation_counts;
    }
};
}  // namespace printemps::model_handler
#endif
/*****************************************************************************/
// END
/*****************************************************************************/