/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MODEL_MODEL_H__
#define PRINTEMPS_MODEL_MODEL_H__

#include <vector>
#include <string>
#include <numeric>
#include <functional>
#include <cmath>

#include "variable_sense.h"
#include "constraint_sense.h"
#include "move_sense.h"
#include "range.h"
#include "selection_mode.h"

#include "move.h"
#include "variable_proxy.h"
#include "expression_proxy.h"
#include "constraint_proxy.h"
#include "objective.h"
#include "value_proxy.h"
#include "solution.h"
#include "model_summary.h"
#include "named_solution.h"
#include "plain_solution.h"
#include "solution_score.h"
#include "selection.h"
#include "neighborhood.h"

#include "expression_binary_operator.h"
#include "constraint_binary_operator.h"

#include "variable_reference.h"
#include "constraint_reference.h"
#include "constraint_type_reference.h"

namespace printemps {
namespace model {
/*****************************************************************************/
struct ModelConstant {
    /**
     * Since the addresses of created variables, expressions, and constraints
     * must not be reallocated, addresses for them are reserved beforehand, and
     * the number of their defintions cannot  exceed the following limits.
     */
    static constexpr int MAX_NUMBER_OF_VARIABLE_PROXIES   = 100;
    static constexpr int MAX_NUMBER_OF_EXPRESSION_PROXIES = 100;
    static constexpr int MAX_NUMBER_OF_CONSTRAINT_PROXIES = 100;
};

/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Model {
   private:
    std::string m_name;

    std::vector<VariableProxy<T_Variable, T_Expression>>   m_variable_proxies;
    std::vector<ExpressionProxy<T_Variable, T_Expression>> m_expression_proxies;
    std::vector<ConstraintProxy<T_Variable, T_Expression>> m_constraint_proxies;

    Objective<T_Variable, T_Expression> m_objective;

    std::vector<std::string> m_variable_names;
    std::vector<std::string> m_expression_names;
    std::vector<std::string> m_constraint_names;

    bool m_is_defined_objective;
    bool m_is_enabled_fast_evaluation;
    bool m_is_linear;
    bool m_is_minimization;
    bool m_is_solved;
    bool m_is_feasible;

    std::vector<Selection<T_Variable, T_Expression>> m_selections;
    VariableReference<T_Variable, T_Expression>      m_variable_reference;
    ConstraintReference<T_Variable, T_Expression>    m_constraint_reference;
    ConstraintTypeReference<T_Variable, T_Expression>
        m_constraint_type_reference;

    Neighborhood<T_Variable, T_Expression> m_neighborhood;
    std::function<void(void)>              m_callback;

    /*************************************************************************/
    Model(const Model &) = default;

    /*************************************************************************/
    Model &operator=(const Model &) = default;

   public:
    /*************************************************************************/
    Model(void) {
        this->initialize();
    }

    /*************************************************************************/
    Model(const std::string &a_NAME) {
        this->initialize();
        this->set_name(a_NAME);
    }

    /*************************************************************************/
    virtual ~Model(void) {
        /// nothing to do
    }

    /*************************************************************************/
    inline void initialize(void) {
        m_name = "";

        m_variable_proxies.reserve(
            ModelConstant::MAX_NUMBER_OF_VARIABLE_PROXIES);
        m_expression_proxies.reserve(
            ModelConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES);
        m_constraint_proxies.reserve(
            ModelConstant::MAX_NUMBER_OF_CONSTRAINT_PROXIES);
        m_objective.initialize();

        m_variable_names.clear();
        m_expression_names.clear();
        m_constraint_names.clear();

        m_is_defined_objective       = false;
        m_is_enabled_fast_evaluation = true;
        m_is_linear                  = true;
        m_is_minimization            = true;
        m_is_solved                  = false;
        m_is_feasible                = false;

        m_selections.clear();
        m_variable_reference.initialize();
        m_constraint_reference.initialize();
        m_constraint_type_reference.initialize();

        m_neighborhood.initialize();
        m_callback = [](void) {};
    }

    /*************************************************************************/
    inline constexpr void set_name(const std::string &a_NAME) {
        m_name = a_NAME;
    }

    /*************************************************************************/
    inline constexpr const std::string &name(void) const {
        return m_name;
    }

    /*************************************************************************/
    inline constexpr VariableProxy<T_Variable, T_Expression> &create_variable(
        const std::string &a_NAME) {
        if (utility::has_space(a_NAME)) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The name of decision variable must not contain spaces."));
        }

        int id = m_variable_proxies.size();

        if (id >= ModelConstant::MAX_NUMBER_OF_VARIABLE_PROXIES) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of decision variable definitions must be equal to "
                "or less than " +
                    std::to_string(
                        ModelConstant::MAX_NUMBER_OF_VARIABLE_PROXIES) +
                    "."));
        }

        m_variable_proxies.emplace_back(
            VariableProxy<T_Variable, T_Expression>::create_instance(id));
        m_variable_names.push_back(a_NAME);

        return m_variable_proxies.back();
    }

    /*************************************************************************/
    inline constexpr VariableProxy<T_Variable, T_Expression> &create_variable(
        const std::string &a_NAME, const T_Variable a_LOWER_BOUND,
        const T_Variable a_UPPER_BOUND) {
        auto &variable_proxy = this->create_variable(a_NAME);
        variable_proxy.set_bound(a_LOWER_BOUND, a_UPPER_BOUND);

        return m_variable_proxies.back();
    }

    /*************************************************************************/
    inline VariableProxy<T_Variable, T_Expression> &create_variables(
        const std::string &a_NAME, const int a_NUMBER_OF_ELEMENTS) {
        if (utility::has_space(a_NAME)) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The name of decision variable must not contain spaces."));
        }

        int id = m_variable_proxies.size();

        if (id >= ModelConstant::MAX_NUMBER_OF_VARIABLE_PROXIES) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of decision variable definitions must be equal to "
                "or less than " +
                    std::to_string(
                        ModelConstant::MAX_NUMBER_OF_VARIABLE_PROXIES) +
                    "."));
        }

        m_variable_proxies.emplace_back(
            VariableProxy<T_Variable, T_Expression>::create_instance(
                id, a_NUMBER_OF_ELEMENTS));
        m_variable_names.push_back(a_NAME);

        return m_variable_proxies.back();
    }

    /*************************************************************************/
    inline VariableProxy<T_Variable, T_Expression> &create_variables(
        const std::string &a_NAME, const int a_NUMBER_OF_ELEMENTS,
        const T_Variable a_LOWER_BOUND, const T_Variable a_UPPER_BOUND) {
        auto &variable_proxy = create_variables(a_NAME, a_NUMBER_OF_ELEMENTS);
        variable_proxy.set_bound(a_LOWER_BOUND, a_UPPER_BOUND);

        return m_variable_proxies.back();
    }

    /*************************************************************************/
    inline constexpr VariableProxy<T_Variable, T_Expression> &create_variables(
        const std::string &a_NAME, const std::vector<int> &a_SHAPE) {
        if (utility::has_space(a_NAME)) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The name of decision variable must not contain spaces."));
        }

        int id = m_variable_proxies.size();

        if (id >= ModelConstant::MAX_NUMBER_OF_VARIABLE_PROXIES) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of decision variable definitions must be equal to "
                "or less than " +
                    std::to_string(
                        ModelConstant::MAX_NUMBER_OF_VARIABLE_PROXIES) +
                    "."));
        }

        m_variable_proxies.emplace_back(
            VariableProxy<T_Variable, T_Expression>::create_instance(id,
                                                                     a_SHAPE));
        m_variable_names.push_back(a_NAME);

        return m_variable_proxies.back();
    }

    /*************************************************************************/
    inline constexpr VariableProxy<T_Variable, T_Expression> &create_variables(
        const std::string &a_NAME, const std::vector<int> &a_SHAPE,
        const T_Variable a_LOWER_BOUND, const T_Variable a_UPPER_BOUND) {
        auto &variable_proxy = create_variables(a_NAME, a_SHAPE);
        variable_proxy.set_bound(a_LOWER_BOUND, a_UPPER_BOUND);

        return m_variable_proxies.back();
    }

    /*************************************************************************/
    inline constexpr ExpressionProxy<T_Variable, T_Expression>
        &create_expression(const std::string &a_NAME) {
        if (utility::has_space(a_NAME)) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The name of expression must not contain spaces."));
        }

        int id = m_expression_proxies.size();

        if (id >= ModelConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of expression definitions must be equal to or "
                "less than " +
                    std::to_string(
                        ModelConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES) +
                    "."));
        }

        m_expression_proxies.emplace_back(
            ExpressionProxy<T_Variable, T_Expression>::create_instance(id));
        m_expression_names.push_back(a_NAME);

        return m_expression_proxies.back();
    }

    /*************************************************************************/
    inline constexpr ExpressionProxy<T_Variable, T_Expression> &
    create_expressions(const std::string &a_NAME, int a_NUMBER_OF_ELEMENTS) {
        if (utility::has_space(a_NAME)) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The name of expression must not contain spaces."));
        }

        int id = m_expression_proxies.size();

        if (id >= ModelConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of expression definitions must be equal to or "
                "less than " +
                    std::to_string(
                        ModelConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES) +
                    "."));
        }

        m_expression_proxies.emplace_back(
            ExpressionProxy<T_Variable, T_Expression>::create_instance(
                id, a_NUMBER_OF_ELEMENTS));
        m_expression_names.push_back(a_NAME);

        return m_expression_proxies.back();
    }

    /*************************************************************************/
    inline constexpr ExpressionProxy<T_Variable, T_Expression>
        &create_expressions(const std::string &     a_NAME,
                            const std::vector<int> &a_SHAPE) {
        if (utility::has_space(a_NAME)) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The name of expression must not contain spaces."));
        }

        int id = m_expression_proxies.size();

        if (id >= ModelConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of expression definitions must be equal to or "
                "less than " +
                    std::to_string(
                        ModelConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES) +
                    "."));
        }

        m_expression_proxies.emplace_back(
            ExpressionProxy<T_Variable, T_Expression>::create_instance(
                id, a_SHAPE));
        m_expression_names.push_back(a_NAME);

        return m_expression_proxies.back();
    }

    /*************************************************************************/
    template <template <class, class> class T_ExpressionLike>
    inline constexpr ExpressionProxy<T_Variable, T_Expression> &
    create_expression(
        const std::string &                               a_NAME,
        const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE) {
        if (utility::has_space(a_NAME)) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The name of expression must not contain spaces."));
        }

        int id = m_expression_proxies.size();

        if (id >= ModelConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of expression definitions must be equal to or "
                "less than " +
                    std::to_string(
                        ModelConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES) +
                    "."));
        }

        m_expression_proxies.emplace_back(
            ExpressionProxy<T_Variable, T_Expression>::create_instance(id));
        m_expression_names.push_back(a_NAME);
        m_expression_proxies.back() = a_EXPRESSION_LIKE.to_expression();
        return m_expression_proxies.back();
    }

    /*************************************************************************/
    inline constexpr ExpressionProxy<T_Variable, T_Expression>
        &create_expression(
            const std::string &                         a_NAME,
            const Expression<T_Variable, T_Expression> &a_EXPRESSION) {
        if (utility::has_space(a_NAME)) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The name of expression must not contain spaces."));
        }

        int id = m_expression_proxies.size();

        if (id >= ModelConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of expression definitions must be equal to or "
                "less than " +
                    std::to_string(
                        ModelConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES) +
                    "."));
        }

        m_expression_proxies.emplace_back(
            ExpressionProxy<T_Variable, T_Expression>::create_instance(id));
        m_expression_names.push_back(a_NAME);
        m_expression_proxies.back() = a_EXPRESSION;
        return m_expression_proxies.back();
    }

    /*************************************************************************/
    inline constexpr ConstraintProxy<T_Variable, T_Expression>
        &create_constraint(const std::string &a_NAME) {
        if (utility::has_space(a_NAME)) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The name of constraint must not contain spaces."));
        }

        int id = m_constraint_proxies.size();

        if (id >= ModelConstant::MAX_NUMBER_OF_CONSTRAINT_PROXIES) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of constraint definitions must be equal to or "
                "less than " +
                    std::to_string(
                        ModelConstant::MAX_NUMBER_OF_CONSTRAINT_PROXIES) +
                    "."));
        }

        m_constraint_proxies.emplace_back(
            ConstraintProxy<T_Variable, T_Expression>::create_instance(id));
        m_constraint_names.push_back(a_NAME);

        return m_constraint_proxies.back();
    }

    /*************************************************************************/
    inline constexpr ConstraintProxy<T_Variable, T_Expression> &
    create_constraints(const std::string &a_NAME, int a_NUMBER_OF_ELEMENTS) {
        if (utility::has_space(a_NAME)) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The name of constraint must not contain spaces."));
        }

        int id = m_constraint_proxies.size();

        if (id >= ModelConstant::MAX_NUMBER_OF_CONSTRAINT_PROXIES) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of constraint definitions must be equal to or "
                "less than " +
                    std::to_string(
                        ModelConstant::MAX_NUMBER_OF_CONSTRAINT_PROXIES) +
                    "."));
        }

        m_constraint_proxies.emplace_back(
            ConstraintProxy<T_Variable, T_Expression>::create_instance(
                id, a_NUMBER_OF_ELEMENTS));
        m_constraint_names.push_back(a_NAME);

        return m_constraint_proxies.back();
    }

    /*************************************************************************/
    inline constexpr ConstraintProxy<T_Variable, T_Expression>
        &create_constraints(const std::string &     a_NAME,
                            const std::vector<int> &a_SHAPE) {
        if (utility::has_space(a_NAME)) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The name of constraint must not contain spaces."));
        }

        int id = m_constraint_proxies.size();

        if (id >= ModelConstant::MAX_NUMBER_OF_CONSTRAINT_PROXIES) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of constraint definitions must be equal to or "
                "less than " +
                    std::to_string(
                        ModelConstant::MAX_NUMBER_OF_CONSTRAINT_PROXIES) +
                    "."));
        }

        m_constraint_proxies.emplace_back(
            ConstraintProxy<T_Variable, T_Expression>::create_instance(
                id, a_SHAPE));
        m_constraint_names.push_back(a_NAME);

        return m_constraint_proxies.back();
    }

    /*************************************************************************/
    inline constexpr ConstraintProxy<T_Variable, T_Expression>
        &create_constraint(
            const std::string &                         a_NAME,
            const Constraint<T_Variable, T_Expression> &a_CONSTRAINT) {
        if (utility::has_space(a_NAME)) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The name of constraint must not contain spaces."));
        }

        int id = m_constraint_proxies.size();

        if (id >= ModelConstant::MAX_NUMBER_OF_CONSTRAINT_PROXIES) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of constraint definitions must be equal to or "
                "less than " +
                    std::to_string(
                        ModelConstant::MAX_NUMBER_OF_CONSTRAINT_PROXIES) +
                    "."));
        }

        m_constraint_proxies.emplace_back(
            ConstraintProxy<T_Variable, T_Expression>::create_instance(id));
        m_constraint_names.push_back(a_NAME);
        m_constraint_proxies.back() = a_CONSTRAINT;

        return m_constraint_proxies.back();
    }

    /*************************************************************************/
    inline constexpr void minimize(
        const std::function<
            T_Expression(const Move<T_Variable, T_Expression> &)> &a_FUNCTION) {
        auto objective =
            Objective<T_Variable, T_Expression>::create_instance(a_FUNCTION);
        m_objective            = objective;
        m_is_defined_objective = true;
        m_is_minimization      = true;
    }

    /*************************************************************************/
    template <template <class, class> class T_ExpressionLike>
    inline constexpr void minimize(
        const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE) {
        auto objective = Objective<T_Variable, T_Expression>::create_instance(
            a_EXPRESSION_LIKE.to_expression());

        m_objective            = objective;
        m_is_defined_objective = true;
        m_is_minimization      = true;
    }

    /*************************************************************************/
    inline constexpr void minimize(
        const Expression<T_Variable, T_Expression> &a_EXPRESSION) {
        auto objective =
            Objective<T_Variable, T_Expression>::create_instance(a_EXPRESSION);
        m_objective            = objective;
        m_is_defined_objective = true;
        m_is_minimization      = true;
    }

    /*************************************************************************/
    inline constexpr void maximize(
        const std::function<
            T_Expression(const Move<T_Variable, T_Expression> &)> &a_FUNCTION) {
        auto objective =
            Objective<T_Variable, T_Expression>::create_instance(a_FUNCTION);
        m_objective            = objective;
        m_is_defined_objective = true;
        m_is_minimization      = false;
    }

    /*************************************************************************/
    template <template <class, class> class T_ExpressionLike>
    inline constexpr void maximize(
        const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE) {
        auto objective = Objective<T_Variable, T_Expression>::create_instance(
            a_EXPRESSION_LIKE.to_expression());
        m_objective            = objective;
        m_is_defined_objective = true;
        m_is_minimization      = false;
    }

    /*************************************************************************/
    inline constexpr void maximize(
        const Expression<T_Variable, T_Expression> &a_EXPRESSION) {
        auto objective =
            Objective<T_Variable, T_Expression>::create_instance(a_EXPRESSION);
        m_objective            = objective;
        m_is_defined_objective = true;
        m_is_minimization      = false;
    }

    /*************************************************************************/
    inline constexpr void setup(
        const bool           a_IS_ENABLED_PRESOLVE,                  //
        const bool           a_IS_ENABLED_INITIAL_VALUE_CORRECTION,  //
        const bool           a_IS_ENABLED_AGGREGATION_MOVE,
        const bool           a_IS_ENABLED_PRECEDENCE_MOVE,
        const bool           a_IS_ENABLED_VARIABLE_BOUND_MOVE,
        const bool           a_IS_ENABLED_EXCLUSIVE_MOVE,
        const bool           a_IS_ENABLED_CHAIN_MOVE,
        const bool           a_IS_ENABLED_USER_DEFINED_MOVE,
        const SelectionMode &a_SELECTION_MODE,  //
        const bool           a_IS_ENABLED_PRINT) {
        this->verify_problem(a_IS_ENABLED_PRINT);

        this->setup_variable_related_constraints();
        this->setup_unique_name();
        this->setup_is_linear();

        if (this->is_linear()) {
            this->setup_variable_sensitivity();
        }

        /**
         * Presolve the problem by removing redundant constraints and fixing
         * decision variables implicitly fixed.
         */
        if (a_IS_ENABLED_PRESOLVE) {
            this->presolve(a_IS_ENABLED_PRINT);
        }

        this->categorize_variables();
        this->categorize_constraints();

        if (a_SELECTION_MODE != SelectionMode::None) {
            this->extract_selections(a_SELECTION_MODE);
        }

        this->setup_neighborhood(a_IS_ENABLED_AGGREGATION_MOVE,     //
                                 a_IS_ENABLED_PRECEDENCE_MOVE,      //
                                 a_IS_ENABLED_VARIABLE_BOUND_MOVE,  //
                                 a_IS_ENABLED_EXCLUSIVE_MOVE,       //
                                 a_IS_ENABLED_USER_DEFINED_MOVE,    //
                                 a_IS_ENABLED_CHAIN_MOVE,           //
                                 a_IS_ENABLED_PRINT);

        this->verify_and_correct_selection_variables_initial_values(
            a_IS_ENABLED_INITIAL_VALUE_CORRECTION, a_IS_ENABLED_PRINT);

        this->verify_and_correct_binary_variables_initial_values(
            a_IS_ENABLED_INITIAL_VALUE_CORRECTION, a_IS_ENABLED_PRINT);

        this->verify_and_correct_integer_variables_initial_values(
            a_IS_ENABLED_INITIAL_VALUE_CORRECTION, a_IS_ENABLED_PRINT);

        this->setup_fixed_sensitivities(a_IS_ENABLED_PRINT);

        this->setup_is_enabled_fast_evaluation();
    }

    /*************************************************************************/
    inline constexpr void verify_problem(const bool a_IS_ENABLED_PRINT) {
        utility::print_single_line(a_IS_ENABLED_PRINT);
        utility::print_message("Verifying the problem...", a_IS_ENABLED_PRINT);

        if (m_variable_proxies.size() == 0) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "No decision variables are defined."));
        }
        if (m_constraint_proxies.size() == 0 && !m_is_defined_objective) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "Neither objective nor constraint functions are "
                "defined."));
        }
        utility::print_message("Done.", a_IS_ENABLED_PRINT);
    }

    /*************************************************************************/
    inline constexpr void setup_variable_related_constraints(void) {
        for (auto &&proxy : m_variable_proxies) {
            for (auto &&variable : proxy.flat_indexed_variables()) {
                variable.reset_related_constraint_ptrs();
            }
        }
        for (auto &&proxy : m_constraint_proxies) {
            for (auto &&constraint : proxy.flat_indexed_constraints()) {
                for (auto &&sensitivity :
                     constraint.expression().sensitivities()) {
                    sensitivity.first->register_related_constraint_ptr(
                        &constraint);
                }
            }
        }
    }

    /*************************************************************************/
    inline constexpr void setup_unique_name(void) {
        int variable_proxies_size   = m_variable_proxies.size();
        int expression_proxies_size = m_expression_proxies.size();
        int constraint_proxies_size = m_constraint_proxies.size();

        for (auto i = 0; i < variable_proxies_size; i++) {
            int number_of_elements = m_variable_proxies[i].number_of_elements();
            for (auto j = 0; j < number_of_elements; j++) {
                auto &variable =
                    m_variable_proxies[i].flat_indexed_variables(j);

                if (variable.name() == "") {
                    variable.set_name(m_variable_names[i] +
                                      m_variable_proxies[i].indices_label(j));
                }
            }
        }

        /// Expression
        for (auto i = 0; i < expression_proxies_size; i++) {
            int number_of_elements =
                m_expression_proxies[i].number_of_elements();
            for (auto j = 0; j < number_of_elements; j++) {
                auto &expression =
                    m_expression_proxies[i].flat_indexed_expressions(j);

                if (expression.name() == "") {
                    expression.set_name(
                        m_expression_names[i] +
                        m_expression_proxies[i].indices_label(j));
                }
            }
        }

        /// Constraint
        for (auto i = 0; i < constraint_proxies_size; i++) {
            int number_of_elements =
                m_constraint_proxies[i].number_of_elements();
            for (auto j = 0; j < number_of_elements; j++) {
                auto &constraint =
                    m_constraint_proxies[i].flat_indexed_constraints(j);

                if (constraint.name() == "") {
                    constraint.set_name(
                        m_constraint_names[i] +
                        m_constraint_proxies[i].indices_label(j));
                }
            }
        }
    }

    /*************************************************************************/
    inline constexpr void setup_is_linear(void) {
        m_is_linear = true;
        for (auto &&proxy : m_constraint_proxies) {
            for (auto &&constraint : proxy.flat_indexed_constraints()) {
                if (!constraint.is_linear()) {
                    m_is_linear = false;
                }
            }
        }

        if (!m_objective.is_linear()) {
            m_is_linear = false;
        }
    }

    /*************************************************************************/
    inline constexpr void setup_is_enabled_fast_evaluation(void) {
        m_is_enabled_fast_evaluation = true;
        for (auto &&proxy : m_constraint_proxies) {
            for (auto &&constraint : proxy.flat_indexed_constraints()) {
                if (!constraint.is_linear()) {
                    m_is_enabled_fast_evaluation = false;
                }
            }
        }

        if (m_neighborhood.is_enabled_user_defined_move()) {
            m_is_enabled_fast_evaluation = false;
        }
    }

    /*************************************************************************/
    inline constexpr void setup_variable_sensitivity(void) {
        for (auto &&proxy : m_variable_proxies) {
            for (auto &&variable : proxy.flat_indexed_variables()) {
                variable.reset_constraint_sensitivities();
            }
        }
        for (auto &&proxy : m_constraint_proxies) {
            for (auto &&constraint : proxy.flat_indexed_constraints()) {
                for (auto &&sensitivity :
                     constraint.expression().sensitivities()) {
                    sensitivity.first->register_constraint_sensitivity(
                        &constraint, sensitivity.second);
                }
            }
        }
        for (auto &&sensitivity : m_objective.expression().sensitivities()) {
            sensitivity.first->set_objective_sensitivity(sensitivity.second);
        }
    }

    /*************************************************************************/
    inline constexpr void presolve(const bool a_IS_ENABLED_PRINT) {
        utility::print_single_line(a_IS_ENABLED_PRINT);
        utility::print_message("Presolving...", a_IS_ENABLED_PRINT);

        if (this->is_linear()) {
            this->remove_independent_variables(a_IS_ENABLED_PRINT);
        }

        while (true) {
            int number_of_newly_disabled_constaints = 0;
            int number_of_newly_fixed_variables     = 0;

            number_of_newly_disabled_constaints +=
                this->remove_redundant_constraints_with_tightening_variable_bounds(
                    a_IS_ENABLED_PRINT);

            number_of_newly_fixed_variables +=
                this->fix_implicit_fixed_variables(a_IS_ENABLED_PRINT);

            if (number_of_newly_disabled_constaints == 0 &&
                number_of_newly_fixed_variables == 0) {
                break;
            }
        }
        utility::print_message("Done.", a_IS_ENABLED_PRINT);
    }

    /*************************************************************************/
    inline constexpr int remove_independent_variables(
        const bool a_IS_ENABLED_PRINT) {
        int number_of_newly_fixed_variables = 0;
        for (auto &&proxy : m_variable_proxies) {
            for (auto &&variable : proxy.flat_indexed_variables()) {
                /**
                 * If the decision variable has already been fixed, the
                 * following procedures will be skipped.
                 */
                if (variable.is_fixed()) {
                    continue;
                }
                auto &objective_sensitivities =
                    m_objective.expression().sensitivities();

                if (variable.related_constraint_ptrs().size() == 0) {
                    if (objective_sensitivities.find(&variable) ==
                        objective_sensitivities.end()) {
                        utility::print_message(
                            "The value of decision variable " +
                                variable.name() + " was fixed by " +
                                std::to_string(0) +
                                " because it does not have sensitivity to any "
                                "constraint or objective function.",
                            a_IS_ENABLED_PRINT);
                        variable.fix_by(0);
                        number_of_newly_fixed_variables++;
                    } else {
                        auto sensitivity =
                            objective_sensitivities.at(&variable);

                        if (sensitivity > 0) {
                            if (this->is_minimization()) {
                                auto fix_value = variable.lower_bound();
                                utility::print_message(
                                    "The value of decision variable " +
                                        variable.name() +
                                        " was fixed by its lower bound " +
                                        std::to_string(fix_value) +
                                        " because it does not have "
                                        "sensitivity to any "
                                        "constraint, and the sensitivity to "
                                        "the objective function to be "
                                        "minimized is positive",
                                    a_IS_ENABLED_PRINT);
                                variable.fix_by(fix_value);
                                number_of_newly_fixed_variables++;
                            } else {
                                auto fix_value = variable.upper_bound();
                                utility::print_message(
                                    "The value of decision variable " +
                                        variable.name() +
                                        " was fixed by its upper bound " +
                                        std::to_string(fix_value) +
                                        " because it does not have "
                                        "sensitivity to any "
                                        "constraint, and the sensitivity to "
                                        "the objective function to be "
                                        "maximized is positive",
                                    a_IS_ENABLED_PRINT);
                                variable.fix_by(fix_value);
                                number_of_newly_fixed_variables++;
                            }
                        } else {
                            if (this->is_minimization()) {
                                auto fix_value = variable.upper_bound();
                                utility::print_message(
                                    "The value of decision variable " +
                                        variable.name() +
                                        " was fixed by its upper bound " +
                                        std::to_string(fix_value) +
                                        " because it does not have "
                                        "sensitivity to any "
                                        "constraint, and the sensitivity to "
                                        "the objective function to be "
                                        "minimized is negative",
                                    a_IS_ENABLED_PRINT);
                                variable.fix_by(fix_value);
                                number_of_newly_fixed_variables++;
                            } else {
                                auto fix_value = variable.lower_bound();
                                utility::print_message(
                                    "The value of decision variable " +
                                        variable.name() +
                                        " was fixed by its lower bound " +
                                        std::to_string(fix_value) +
                                        " because it does not have "
                                        "sensitivity to any "
                                        "constraint, and the sensitivity to "
                                        "the objective function to be "
                                        "maximized is positive",
                                    a_IS_ENABLED_PRINT);
                                variable.fix_by(fix_value);
                                number_of_newly_fixed_variables++;
                            }
                        }
                    }
                }
            }
        }
        return number_of_newly_fixed_variables;
    }

    /*************************************************************************/
    inline constexpr int
    remove_redundant_constraints_with_tightening_variable_bounds(
        const bool a_IS_ENABLED_PRINT) {
        const int BOUND_LIMIT = 100000;

        int number_of_newly_disabled_constraints = 0;
        for (auto &&proxy : m_constraint_proxies) {
            for (auto &&constraint : proxy.flat_indexed_constraints()) {
                /**
                 * If the constraint is nonlinear, the following procedures
                 * will be skipped.
                 */
                if (!constraint.is_linear()) {
                    continue;
                }

                /**
                 * If the constraint has already been disabled, the following
                 * procedures will be skipped.
                 */
                if (!constraint.is_enabled()) {
                    continue;
                }

                auto &sensitivities  = constraint.expression().sensitivities();
                auto  constant_value = constraint.expression().constant_value();

                std::vector<std::pair<Variable<T_Variable, T_Expression> *,
                                      T_Expression>>
                    not_fixed_variable_sensitivities;
                std::vector<std::pair<Variable<T_Variable, T_Expression> *,
                                      T_Expression>>
                    positive_coefficient_not_fixed_variable_sensitivities;
                std::vector<std::pair<Variable<T_Variable, T_Expression> *,
                                      T_Expression>>
                    negative_coefficient_not_fixed_variable_sensitivities;

                not_fixed_variable_sensitivities.reserve(sensitivities.size());
                positive_coefficient_not_fixed_variable_sensitivities.reserve(
                    sensitivities.size());
                negative_coefficient_not_fixed_variable_sensitivities.reserve(
                    sensitivities.size());
                int number_of_fixed_variables = 0;

                /**
                 * The lower and bounds of a^{T}x and fixed value in b^{T}y in
                 * a^{T}x + b^{T}y + c <=(>=,=) 0, where a^{T}x are terms whose
                 * decision variables are not fixed, b^{T}y are terms with fixed
                 * decision variables, and c is the constant term.
                 */
                double not_fixed_term_lower_bound = 0.0;
                double not_fixed_term_upper_bound = 0.0;
                double fixed_term_value           = 0.0;

                for (const auto &sensitivity : sensitivities) {
                    if (sensitivity.first->is_fixed()) {
                        number_of_fixed_variables++;
                        fixed_term_value +=
                            sensitivity.first->value() * sensitivity.second;
                    } else {
                        if (sensitivity.second > 0) {
                            not_fixed_term_lower_bound +=
                                sensitivity.first->lower_bound() *
                                sensitivity.second;
                            not_fixed_term_upper_bound +=
                                sensitivity.first->upper_bound() *
                                sensitivity.second;
                            positive_coefficient_not_fixed_variable_sensitivities
                                .push_back(sensitivity);
                            not_fixed_variable_sensitivities.push_back(
                                sensitivity);
                        } else {
                            not_fixed_term_lower_bound +=
                                sensitivity.first->upper_bound() *
                                sensitivity.second;
                            not_fixed_term_upper_bound +=
                                sensitivity.first->lower_bound() *
                                sensitivity.second;
                            negative_coefficient_not_fixed_variable_sensitivities
                                .push_back(sensitivity);
                            not_fixed_variable_sensitivities.push_back(
                                sensitivity);
                        }
                    }
                }

                /**
                 * If the constraint is always satisfied obviously, it will be
                 * removed.
                 */
                if ((constraint.sense() == ConstraintSense::Equal &&
                     not_fixed_variable_sensitivities.size() == 0 &&
                     fixed_term_value + constant_value == 0) ||
                    (constraint.sense() == ConstraintSense::Lower &&
                     not_fixed_term_upper_bound + fixed_term_value +
                             constant_value <=
                         0) ||
                    (constraint.sense() == ConstraintSense::Upper &&
                     not_fixed_term_lower_bound + fixed_term_value +
                             constant_value >=
                         0)) {
                    utility::print_message("The constraint " +
                                               constraint.name() +
                                               " was removed for redundancy.",
                                           a_IS_ENABLED_PRINT);

                    if (constraint.is_enabled()) {
                        constraint.disable();
                        number_of_newly_disabled_constraints++;
                    }
                    continue;
                }

                /**
                 * The detected singleton constaint will be disabled instead
                 * of fixing or tightening the lower and upper bounds of the
                 * decision variable included in the constraint.
                 */
                if (not_fixed_variable_sensitivities.size() == 1) {
                    auto variable_ptr =
                        not_fixed_variable_sensitivities.front().first;
                    auto coefficient =
                        not_fixed_variable_sensitivities.front().second;

                    auto lower_bound = variable_ptr->lower_bound();
                    auto upper_bound = variable_ptr->upper_bound();

                    auto bound_temp =
                        -(fixed_term_value + constant_value) / coefficient;

                    if (constraint.sense() == ConstraintSense::Equal) {
                        /**
                         * If the singleton constraint is defined by an
                         * equality as ax+b=0, the value of the decision
                         * variable x will be fixed by -b/a.
                         */

                        utility::print_message(
                            "The constraint " + constraint.name() +
                                " was removed instead of fixing the value "
                                "of the decision variable " +
                                variable_ptr->name() + " by " +
                                std::to_string(bound_temp) + ".",
                            a_IS_ENABLED_PRINT);

                        variable_ptr->fix_by(bound_temp);
                        if (constraint.is_enabled()) {
                            constraint.disable();
                            number_of_newly_disabled_constraints++;
                        }
                    } else if ((constraint.sense() == ConstraintSense::Lower &&
                                coefficient > 0) ||
                               (constraint.sense() == ConstraintSense::Upper &&
                                coefficient < 0)) {
                        /**
                         * If the singleton constraint is defined by an
                         * equality as ax+b<=0 with a>0 (or ax+b>=0 with a<0),
                         * the lower bound of the decision variable will be
                         * tightened by floor(-b/a).
                         */
                        auto bound_floor =
                            static_cast<T_Variable>(std::floor(bound_temp));

                        if (bound_floor < upper_bound &&
                            abs(bound_floor) < BOUND_LIMIT) {
                            utility::print_message(
                                "The constraint " + constraint.name() +
                                    " was removed instead of tightening the "
                                    "upper bound of the decision variable " +
                                    variable_ptr->name() + " by " +
                                    std::to_string(bound_floor) + ".",
                                a_IS_ENABLED_PRINT);
                            variable_ptr->set_bound(lower_bound, bound_floor);
                        } else {
                            utility::print_message(
                                "The constraint " + constraint.name() +
                                    " was removed for redundancy.",
                                a_IS_ENABLED_PRINT);
                        }
                        if (constraint.is_enabled()) {
                            constraint.disable();
                            number_of_newly_disabled_constraints++;
                        }

                    } else if ((constraint.sense() == ConstraintSense::Upper &&
                                coefficient > 0) ||
                               (constraint.sense() == ConstraintSense::Lower &&
                                coefficient < 0)) {
                        /**
                         * If the singleton constraint is defined by an
                         * equality as ax+b>=0 with a>0 (or ax+b<=0 with a<0),
                         * the upper bound of the decision variable will be
                         * tightened by ceil(-b/a).
                         */
                        auto bound_ceil =
                            static_cast<T_Variable>(std::ceil(bound_temp));

                        if (bound_ceil > lower_bound &&
                            abs(bound_ceil) < BOUND_LIMIT) {
                            utility::print_message(
                                "The constraint " + constraint.name() +
                                    " was removed instead of tightening the "
                                    "lower bound of the decision variable " +
                                    variable_ptr->name() + " by " +
                                    std::to_string(bound_ceil) + ".",
                                a_IS_ENABLED_PRINT);
                            variable_ptr->set_bound(bound_ceil, upper_bound);
                        } else {
                            utility::print_message(
                                "The constraint " + constraint.name() +
                                    " was removed for redundancy.",
                                a_IS_ENABLED_PRINT);
                        }
                        if (constraint.is_enabled()) {
                            constraint.disable();
                            number_of_newly_disabled_constraints++;
                        }
                    }
                    continue;
                }

                /**
                 * Tighten the lower and upper bounds of the decision variables
                 * based on the bounds of the rest part.
                 */
                for (auto &sensitivity :
                     positive_coefficient_not_fixed_variable_sensitivities) {
                    auto variable_ptr = sensitivity.first;
                    auto coefficient  = sensitivity.second;

                    auto lower_bound = variable_ptr->lower_bound();
                    auto upper_bound = variable_ptr->upper_bound();

                    if (constraint.sense() == ConstraintSense::Upper) {
                        auto bound_temp = -(not_fixed_term_upper_bound -
                                            coefficient * upper_bound +
                                            fixed_term_value + constant_value) /
                                          coefficient;
                        auto bound_ceil =
                            static_cast<T_Variable>(std::ceil(bound_temp));
                        if (bound_ceil > lower_bound &&
                            abs(bound_ceil) < BOUND_LIMIT) {
                            utility::print_message(
                                "The lower bound of the decision variable " +
                                    variable_ptr->name() +
                                    " was tightened by " +
                                    std::to_string(bound_ceil) + ".",
                                a_IS_ENABLED_PRINT);
                            variable_ptr->set_bound(bound_ceil, upper_bound);
                        }
                    } else if (constraint.sense() == ConstraintSense::Lower) {
                        auto bound_temp = -(not_fixed_term_lower_bound -
                                            coefficient * lower_bound +
                                            fixed_term_value + constant_value) /
                                          coefficient;
                        auto bound_floor =
                            static_cast<T_Variable>(std::floor(bound_temp));
                        if (bound_floor < upper_bound &&
                            abs(bound_floor) < BOUND_LIMIT) {
                            utility::print_message(
                                "The upper bound of the decision variable " +
                                    variable_ptr->name() +
                                    " was tightened by " +
                                    std::to_string(bound_floor) + ".",
                                a_IS_ENABLED_PRINT);
                            variable_ptr->set_bound(lower_bound, bound_floor);
                        }
                    }
                }

                for (auto &sensitivity :
                     negative_coefficient_not_fixed_variable_sensitivities) {
                    auto variable_ptr = sensitivity.first;
                    auto coefficient  = sensitivity.second;

                    auto lower_bound = variable_ptr->lower_bound();
                    auto upper_bound = variable_ptr->upper_bound();

                    if (constraint.sense() == ConstraintSense::Upper) {
                        auto bound_temp = -(not_fixed_term_upper_bound -
                                            coefficient * lower_bound +
                                            fixed_term_value + constant_value) /
                                          coefficient;
                        auto bound_floor =
                            static_cast<T_Variable>(std::floor(bound_temp));
                        if (bound_floor < upper_bound &&
                            abs(bound_floor) < BOUND_LIMIT) {
                            utility::print_message(
                                "The upper bound of the decision variable " +
                                    variable_ptr->name() +
                                    " was tightened by " +
                                    std::to_string(bound_floor) + ".",
                                a_IS_ENABLED_PRINT);
                            variable_ptr->set_bound(lower_bound, bound_floor);
                        }
                    }

                    else if (constraint.sense() == ConstraintSense::Lower) {
                        auto bound_temp = -(not_fixed_term_lower_bound -
                                            coefficient * upper_bound +
                                            fixed_term_value + constant_value) /
                                          coefficient;
                        auto bound_ceil =
                            static_cast<T_Variable>(std::ceil(bound_temp));
                        if (bound_ceil > lower_bound &&
                            abs(bound_ceil) < BOUND_LIMIT) {
                            utility::print_message(
                                "The lower bound of the decision variable " +
                                    variable_ptr->name() +
                                    " was tightened by " +
                                    std::to_string(bound_ceil) + ".",
                                a_IS_ENABLED_PRINT);
                            variable_ptr->set_bound(bound_ceil, upper_bound);
                        }
                    }
                }
            }
        }
        return number_of_newly_disabled_constraints;
    }

    /*************************************************************************/
    inline constexpr int fix_implicit_fixed_variables(
        const bool a_IS_ENABLED_PRINT) {
        int number_of_newly_fixed_variables = 0;
        for (auto &&proxy : m_variable_proxies) {
            for (auto &&variable : proxy.flat_indexed_variables()) {
                /**
                 * If the decision variable has already been fixed, the
                 * following procedures will be skipped.
                 */
                if (variable.is_fixed()) {
                    continue;
                }

                auto lower_bound = variable.lower_bound();
                auto upper_bound = variable.upper_bound();
                if (lower_bound == upper_bound) {
                    auto fixed_value = lower_bound;

                    utility::print_message(
                        "The value of decision variable " + variable.name() +
                            " was fixed by " + std::to_string(fixed_value) +
                            " because the lower bound " +
                            std::to_string(lower_bound) +
                            " and the upper_bound " +
                            std::to_string(upper_bound) +
                            " implicitly fix the value.",
                        a_IS_ENABLED_PRINT);
                    variable.fix_by(fixed_value);
                    number_of_newly_fixed_variables++;
                }
            }
        }
        return number_of_newly_fixed_variables;
    }

    /*************************************************************************/
    inline constexpr void categorize_variables(void) {
        VariableReference<T_Variable, T_Expression> variable_reference;

        for (auto &&proxy : m_variable_proxies) {
            for (auto &&variable : proxy.flat_indexed_variables()) {
                variable_reference.variable_ptrs.push_back(&variable);
                if (variable.is_fixed()) {
                    variable_reference.fixed_variable_ptrs.push_back(&variable);
                } else {
                    variable_reference.not_fixed_variable_ptrs.push_back(
                        &variable);
                }
                if (variable.sense() == VariableSense::Selection) {
                    variable_reference.selection_variable_ptrs.push_back(
                        &variable);
                }
                if (variable.sense() == VariableSense::Binary) {
                    variable_reference.binary_variable_ptrs.push_back(
                        &variable);
                }
                if (variable.sense() == VariableSense::Integer) {
                    variable_reference.integer_variable_ptrs.push_back(
                        &variable);
                }
            }
        }
        m_variable_reference = variable_reference;
    }

    /*************************************************************************/
    inline constexpr void categorize_constraints(void) {
        ConstraintReference<T_Variable, T_Expression> constraint_reference;
        ConstraintTypeReference<T_Variable, T_Expression>
            constraint_type_reference;

        for (auto &&proxy : m_constraint_proxies) {
            for (auto &&constraint : proxy.flat_indexed_constraints()) {
                constraint.setup_constraint_type();
            }
        }

        for (auto &&proxy : m_constraint_proxies) {
            for (auto &&constraint : proxy.flat_indexed_constraints()) {
                constraint_reference.constraint_ptrs.push_back(&constraint);
                if (!constraint.is_enabled()) {
                    constraint_reference.disabled_constraint_ptrs.push_back(
                        &constraint);
                }
                if (!constraint.is_linear()) {
                    constraint_type_reference.nonlinear_ptrs.push_back(
                        &constraint);
                } else {
                    if (constraint.is_singleton()) {
                        constraint_type_reference.singleton_ptrs.push_back(
                            &constraint);
                    }
                    if (constraint.is_aggregation()) {
                        constraint_type_reference.aggregation_ptrs.push_back(
                            &constraint);
                    }
                    if (constraint.is_precedence()) {
                        constraint_type_reference.precedence_ptrs.push_back(
                            &constraint);
                    }
                    if (constraint.is_variable_bound()) {
                        constraint_type_reference.variable_bound_ptrs.push_back(
                            &constraint);
                    }
                    if (constraint.is_set_partitioning()) {
                        constraint_type_reference.set_partitioning_ptrs
                            .push_back(&constraint);
                    }
                    if (constraint.is_set_packing()) {
                        constraint_type_reference.set_packing_ptrs.push_back(
                            &constraint);
                    }
                    if (constraint.is_set_covering()) {
                        constraint_type_reference.set_covering_ptrs.push_back(
                            &constraint);
                    }
                    if (constraint.is_cardinality()) {
                        constraint_type_reference.cardinality_ptrs.push_back(
                            &constraint);
                    }
                    if (constraint.is_invariant_knapsack()) {
                        constraint_type_reference.invariant_knapsack_ptrs
                            .push_back(&constraint);
                    }
                    if (constraint.is_equation_knapsack()) {
                        constraint_type_reference.equation_knapsack_ptrs
                            .push_back(&constraint);
                    }
                    if (constraint.is_bin_packing()) {
                        constraint_type_reference.bin_packing_ptrs.push_back(
                            &constraint);
                    }
                    if (constraint.is_knapsack()) {
                        constraint_type_reference.knapsack_ptrs.push_back(
                            &constraint);
                    }
                    if (constraint.is_integer_knapsack()) {
                        constraint_type_reference.integer_knapsack_ptrs
                            .push_back(&constraint);
                    }
                    if (constraint.is_general_linear()) {
                        constraint_type_reference.general_linear_ptrs.push_back(
                            &constraint);
                    }
                }
            }
        }
        m_constraint_reference      = constraint_reference;
        m_constraint_type_reference = constraint_type_reference;
    }

    /*************************************************************************/
    inline constexpr void extract_selections(
        const SelectionMode &a_SELECTION_MODE) {
        std::vector<Variable<T_Variable, T_Expression> *>
            extracted_selection_variable_ptrs;

        std::vector<Selection<T_Variable, T_Expression>> raw_selections;
        std::vector<Selection<T_Variable, T_Expression>> selections;

        for (auto &&constraint_ptr :
             m_constraint_type_reference.set_partitioning_ptrs) {
            if (!constraint_ptr->is_enabled()) {
                continue;
            }

            Selection<T_Variable, T_Expression> selection;
            selection.constraint_ptr = constraint_ptr;

            for (const auto &sensitivity :
                 constraint_ptr->expression().sensitivities()) {
                selection.variable_ptrs.push_back(sensitivity.first);
            }
            raw_selections.push_back(selection);
        }

        switch (a_SELECTION_MODE) {
            case SelectionMode::None: {
                break;
            }
            case SelectionMode::Defined: {
                break;
            }
            case SelectionMode::Smaller: {
                std::sort(raw_selections.begin(), raw_selections.end(),
                          [](auto const &a_LHS, auto const &a_RHS) {
                              return a_LHS.variable_ptrs.size() <
                                     a_RHS.variable_ptrs.size();
                          });
                break;
            }
            case SelectionMode::Larger: {
                std::sort(raw_selections.begin(), raw_selections.end(),
                          [](auto const &a_LHS, auto const &a_RHS) {
                              return a_LHS.variable_ptrs.size() >
                                     a_RHS.variable_ptrs.size();
                          });
                break;
            }
            case SelectionMode::Independent: {
                break;
            }
            default: {
            }
        }

        if (a_SELECTION_MODE == SelectionMode::Defined ||
            a_SELECTION_MODE == SelectionMode::Smaller ||
            a_SELECTION_MODE == SelectionMode::Larger) {
            for (auto &&selection : raw_selections) {
                bool has_eliminated_variable_ptr = false;
                for (auto &&variable_ptr : selection.variable_ptrs) {
                    if (std::find(extracted_selection_variable_ptrs.begin(),
                                  extracted_selection_variable_ptrs.end(),
                                  variable_ptr) !=
                        extracted_selection_variable_ptrs.end()) {
                        has_eliminated_variable_ptr = true;
                        break;
                    }
                }
                if (has_eliminated_variable_ptr) {
                    continue;
                } else {
                    selections.push_back(selection);
                    extracted_selection_variable_ptrs.insert(
                        extracted_selection_variable_ptrs.end(),
                        selection.variable_ptrs.begin(),
                        selection.variable_ptrs.end());
                }
            }
        } else if (a_SELECTION_MODE == SelectionMode::Independent) {
            int raw_selections_size = raw_selections.size();
            for (auto i = 0; i < raw_selections_size; i++) {
                bool has_overlap = false;
                for (auto &&variable_ptr : raw_selections[i].variable_ptrs) {
                    for (auto j = 0; j < raw_selections_size; j++) {
                        if (j != i &&
                            std::find(raw_selections[j].variable_ptrs.begin(),
                                      raw_selections[j].variable_ptrs.end(),
                                      variable_ptr) !=
                                raw_selections[j].variable_ptrs.end()) {
                            has_overlap = true;
                            break;
                        }
                    }
                    if (has_overlap) {
                        break;
                    }
                }
                if (has_overlap) {
                    continue;
                } else {
                    selections.push_back(raw_selections[i]);
                    extracted_selection_variable_ptrs.insert(
                        extracted_selection_variable_ptrs.end(),
                        raw_selections[i].variable_ptrs.begin(),
                        raw_selections[i].variable_ptrs.end());
                }
            }
        }

        std::vector<Variable<T_Variable, T_Expression> *>
            selection_variable_ptrs;
        std::vector<Constraint<T_Variable, T_Expression> *>
             selection_constraint_ptrs;
        auto binary_variable_ptrs = m_variable_reference.binary_variable_ptrs;
        auto disabled_constraint_ptrs =
            m_constraint_reference.disabled_constraint_ptrs;

        for (auto &&selection : selections) {
            selection.constraint_ptr->disable();
            selection_constraint_ptrs.push_back(selection.constraint_ptr);
            disabled_constraint_ptrs.push_back(selection.constraint_ptr);

            for (auto &&variable_ptr : selection.variable_ptrs) {
                selection_variable_ptrs.push_back(variable_ptr);
                binary_variable_ptrs.erase(
                    std::remove(binary_variable_ptrs.begin(),
                                binary_variable_ptrs.end(), variable_ptr),
                    binary_variable_ptrs.end());
            }
        }

        for (auto &&selection : selections) {
            for (auto &variable_ptr : selection.variable_ptrs) {
                auto &constraint_ptrs = variable_ptr->related_constraint_ptrs();
                selection.related_constraint_ptrs.insert(
                    constraint_ptrs.begin(), constraint_ptrs.end());
            }
        }

        m_selections                                 = selections;
        m_variable_reference.selection_variable_ptrs = selection_variable_ptrs;
        m_constraint_reference.selection_constraint_ptrs =
            selection_constraint_ptrs;
        m_constraint_reference.disabled_constraint_ptrs =
            disabled_constraint_ptrs;
        m_variable_reference.binary_variable_ptrs = binary_variable_ptrs;

        /**
         * The following block must be after setting m_selections because
         * variables have pointers to a element of m_selections.
         */
        for (auto &&selection : m_selections) {
            for (auto &&variable_ptr : selection.variable_ptrs) {
                /**
                 * Register the selection object to the variables which is
                 * covered by the corresponding selection constraint, and
                 * categorize the variable into "Selection".
                 */
                variable_ptr->set_selection_ptr(&selection);
            }
        }
    }

    /*************************************************************************/
    inline constexpr void setup_neighborhood(
        const bool a_IS_ENABLED_AGGREGATION_MOVE,
        const bool a_IS_ENABLED_PRECEDENCE_MOVE,
        const bool a_IS_ENABLED_VARIABLE_BOUND_MOVE,
        const bool a_IS_ENABLED_EXCLUSIVE_MOVE,     //
        const bool a_IS_ENABLED_CHAIN_MOVE,         //
        const bool a_IS_ENABLED_USER_DEFINED_MOVE,  //
        const bool a_IS_ENABLED_PRINT) {
        utility::print_single_line(a_IS_ENABLED_PRINT);
        utility::print_message("Detecting the neighborhood structure...",
                               a_IS_ENABLED_PRINT);
        bool has_fixed_variables = this->number_of_fixed_variables() > 0;
        bool has_selection_variables =
            this->number_of_selection_variables() > 0;

        m_neighborhood.set_has_fixed_variables(has_fixed_variables);
        m_neighborhood.set_has_selection_variables(has_selection_variables);

        m_neighborhood.setup_binary_move_updater(
            m_variable_reference.binary_variable_ptrs);

        m_neighborhood.setup_integer_move_updater(
            m_variable_reference.integer_variable_ptrs);

        m_neighborhood.setup_selection_move_updater(
            m_variable_reference.selection_variable_ptrs);

        if (a_IS_ENABLED_AGGREGATION_MOVE) {
            m_neighborhood.setup_aggregation_move_updater(
                m_constraint_type_reference.aggregation_ptrs);
        }

        if (a_IS_ENABLED_PRECEDENCE_MOVE) {
            m_neighborhood.setup_precedence_move_updater(
                m_constraint_type_reference.precedence_ptrs);
        }

        if (a_IS_ENABLED_EXCLUSIVE_MOVE) {
            m_neighborhood.setup_exclusive_move_updater(
                m_constraint_type_reference.set_partitioning_ptrs,  //
                m_constraint_type_reference.set_packing_ptrs);
        }

        if (a_IS_ENABLED_VARIABLE_BOUND_MOVE) {
            m_neighborhood.setup_variable_bound_move_updater(
                m_constraint_type_reference.variable_bound_ptrs);
        }

        if (a_IS_ENABLED_CHAIN_MOVE) {
            m_neighborhood.setup_chain_move_updater();
        }

        if (a_IS_ENABLED_USER_DEFINED_MOVE) {
            m_neighborhood.setup_user_defined_move_updater();
        }

        utility::print_message("Done.", a_IS_ENABLED_PRINT);
    }

    /*************************************************************************/
    inline constexpr void verify_and_correct_selection_variables_initial_values(
        const bool a_IS_ENABLED_CORRECTON, const bool a_IS_ENABLED_PRINT) {
        utility::print_single_line(a_IS_ENABLED_PRINT);
        utility::print_message(
            "Verifying the initial values of the binary decision "
            "variables included in the selection constraints...",
            a_IS_ENABLED_PRINT);

        for (auto &&selection : m_selections) {
            std::vector<Variable<T_Variable, T_Expression> *>
                fixed_selected_variable_ptrs;
            std::vector<Variable<T_Variable, T_Expression> *>
                selected_variable_ptrs;
            std::vector<Variable<T_Variable, T_Expression> *>
                fixed_invalid_variable_ptrs;
            std::vector<Variable<T_Variable, T_Expression> *>
                invalid_variable_ptrs;

            for (auto &&variable_ptr : selection.variable_ptrs) {
                if (variable_ptr->value() == 1) {
                    selected_variable_ptrs.push_back(variable_ptr);
                    if (variable_ptr->is_fixed()) {
                        fixed_selected_variable_ptrs.push_back(variable_ptr);
                    }
                }
                if (variable_ptr->value() != 0 && variable_ptr->value() != 1) {
                    invalid_variable_ptrs.push_back(variable_ptr);
                    if (variable_ptr->is_fixed()) {
                        fixed_invalid_variable_ptrs.push_back(variable_ptr);
                    }
                }
            }

            /**
             * Return logic error if there is an invalid fixed variable.
             */
            if (fixed_invalid_variable_ptrs.size() > 0) {
                throw std::logic_error(utility::format_error_location(
                    __FILE__, __LINE__, __func__,
                    "There is an invalid fixed variable."));
            }

            /**
             * Return logic error if there are more than 1 fixed
             * selected variables.
             */
            if (fixed_selected_variable_ptrs.size() > 1) {
                throw std::logic_error(utility::format_error_location(
                    __FILE__, __LINE__, __func__,
                    "There are more than one fixed selected "
                    "variables."));
            }

            /**
             * Correct initial values or return logic error if there is
             * a variable of which initial value violates binary
             * constraint.
             */
            if (invalid_variable_ptrs.size() > 0) {
                if (a_IS_ENABLED_CORRECTON) {
                    for (auto &&variable_ptr : invalid_variable_ptrs) {
                        T_Variable old_value = variable_ptr->value();
                        T_Variable new_value = 0;

                        variable_ptr->set_value_if_not_fixed(new_value);

                        utility::print_warning(
                            "The initial value " + variable_ptr->name() +
                                " = " + std::to_string(old_value) +
                                " was corrected to " +
                                std::to_string(new_value) + ".",
                            a_IS_ENABLED_PRINT);
                    }

                } else {
                    throw std::logic_error(utility::format_error_location(
                        __FILE__, __LINE__, __func__,
                        "There is a variable of which initial "
                        "value violates binary constraint."));
                }
            }

            /**
             * Correct initial values or return logic error if there are
             * more than 1 selected variables.
             */
            if (selected_variable_ptrs.size() > 1) {
                if (a_IS_ENABLED_CORRECTON) {
                    Variable<T_Variable, T_Expression> *selected_variable_ptr =
                        nullptr;
                    if (fixed_selected_variable_ptrs.size() == 1) {
                        selected_variable_ptr =
                            fixed_selected_variable_ptrs.front();
                    } else {
                        selected_variable_ptr = selected_variable_ptrs.front();
                    }

                    for (auto &&variable_ptr : selected_variable_ptrs) {
                        if (variable_ptr != selected_variable_ptr) {
                            T_Variable old_value = 1;
                            T_Variable new_value = 0;

                            variable_ptr->set_value_if_not_fixed(new_value);

                            utility::print_warning(
                                "The initial value " + variable_ptr->name() +
                                    " = " + std::to_string(old_value) +
                                    " was corrected to " +
                                    std::to_string(new_value) + ".",
                                a_IS_ENABLED_PRINT);
                        }
                    }

                    selected_variable_ptr->set_value_if_not_fixed(1);
                    selected_variable_ptr->select();

                } else {
                    throw std::logic_error(utility::format_error_location(
                        __FILE__, __LINE__, __func__,
                        "There are more than one selected variables."));
                }
            }
            /**
             * Correct initial values or return logic error if there is
             * no selected variables.
             */
            else if (selected_variable_ptrs.size() == 0) {
                if (a_IS_ENABLED_CORRECTON) {
                    T_Variable old_value    = 0;
                    T_Variable new_value    = 1;
                    bool       is_corrected = false;
                    for (auto &&variable_ptr : selection.variable_ptrs) {
                        if (!variable_ptr->is_fixed()) {
                            variable_ptr->set_value_if_not_fixed(new_value);

                            utility::print_warning(
                                "The initial value " + variable_ptr->name() +
                                    " = " + std::to_string(old_value) +
                                    " was corrected to " +
                                    std::to_string(new_value) + ".",
                                a_IS_ENABLED_PRINT);
                            is_corrected = true;
                            break;
                        }
                    }
                    if (!is_corrected) {
                        throw std::logic_error(utility::format_error_location(
                            __FILE__, __LINE__, __func__,
                            "The initial value could not be modified "
                            "because all variables are fixed."));
                    };
                } else {
                    throw std::logic_error(utility::format_error_location(
                        __FILE__, __LINE__, __func__,
                        "There is no selected variables."));
                }
            } else {
                selected_variable_ptrs.front()->select();
            }
        }
        utility::print_message("Done.", a_IS_ENABLED_PRINT);
    }

    /*************************************************************************/
    inline constexpr void verify_and_correct_binary_variables_initial_values(
        const bool a_IS_ENABLED_CORRECTON, const bool a_IS_ENABLED_PRINT) {
        utility::print_single_line(a_IS_ENABLED_PRINT);
        utility::print_message(
            "Verifying the initial values of the binary decision "
            "variables.",
            a_IS_ENABLED_PRINT);

        for (auto &&proxy : m_variable_proxies) {
            for (auto &&variable : proxy.flat_indexed_variables()) {
                if (variable.sense() == VariableSense::Binary) {
                    if (variable.value() != 0 && variable.value() != 1) {
                        if (variable.is_fixed()) {
                            throw std::logic_error(
                                utility::format_error_location(
                                    __FILE__, __LINE__, __func__,
                                    "There is an invalid fixed variable."));
                        }

                        if (a_IS_ENABLED_CORRECTON) {
                            T_Variable old_value = variable.value();
                            T_Variable new_value = variable.value();
                            if (variable.value() < variable.lower_bound()) {
                                new_value = variable.lower_bound();
                            } else if (variable.value() >
                                       variable.upper_bound()) {
                                new_value = variable.upper_bound();
                            }

                            variable.set_value_if_not_fixed(new_value);

                            utility::print_warning(
                                "The initial value " + variable.name() + " = " +
                                    std::to_string(old_value) +
                                    " was corrected to " +
                                    std::to_string(new_value) + ".",
                                a_IS_ENABLED_PRINT);
                        } else {
                            throw std::logic_error(
                                utility::format_error_location(
                                    __FILE__, __LINE__, __func__,
                                    "An initial value violates binary "
                                    "constraint."));
                        }
                    }
                }
            }
        }
        utility::print_message("Done.", a_IS_ENABLED_PRINT);
    }

    /*************************************************************************/
    inline constexpr void verify_and_correct_integer_variables_initial_values(
        const bool a_IS_ENABLED_CORRECTON, const bool a_IS_ENABLED_PRINT) {
        utility::print_single_line(a_IS_ENABLED_PRINT);
        utility::print_message(
            "Verifying the initial values of the integer decision "
            "variables.",
            a_IS_ENABLED_PRINT);

        for (auto &&proxy : m_variable_proxies) {
            for (auto &&variable : proxy.flat_indexed_variables()) {
                if (variable.sense() == VariableSense::Integer &&
                    (variable.value() < variable.lower_bound() ||
                     variable.value() > variable.upper_bound())) {
                    if (variable.is_fixed()) {
                        throw std::logic_error(utility::format_error_location(
                            __FILE__, __LINE__, __func__,
                            "There is an invalid fixed variable"));
                    }

                    if (a_IS_ENABLED_CORRECTON) {
                        T_Variable old_value = variable.value();
                        T_Variable new_value = variable.value();
                        if (variable.value() < variable.lower_bound()) {
                            new_value = variable.lower_bound();
                        } else if (variable.value() > variable.upper_bound()) {
                            new_value = variable.upper_bound();
                        }

                        variable.set_value_if_not_fixed(new_value);

                        utility::print_warning(
                            "The initial value " + variable.name() + " = " +
                                std::to_string(old_value) +
                                " was corrected to " +
                                std::to_string(new_value) + ".",
                            a_IS_ENABLED_PRINT);
                    } else {
                        throw std::logic_error(utility::format_error_location(
                            __FILE__, __LINE__, __func__,
                            "An initial value violates the lower or "
                            "upper bound constraint."));
                    }
                }
            }
        }
        utility::print_message("Done.", a_IS_ENABLED_PRINT);
    }

    /*************************************************************************/
    inline constexpr void setup_fixed_sensitivities(
        const bool a_IS_ENABLED_PRINT) {
        utility::print_single_line(a_IS_ENABLED_PRINT);
        utility::print_message("Creating the sensitivity matrix...",
                               a_IS_ENABLED_PRINT);

        for (auto &&proxy : m_expression_proxies) {
            for (auto &&expression : proxy.flat_indexed_expressions()) {
                expression.setup_fixed_sensitivities();
            }
        }

        /**
         * The fixed sensitivities for the constraints and the objective are
         * build in their own setup() methods.
         */

        utility::print_message("Done.", a_IS_ENABLED_PRINT);
    }

    /*************************************************************************/
    inline constexpr void print_number_of_variables(void) const {
        utility::print_single_line(true);
        if (this->number_of_fixed_variables() == 0) {
            utility::print_info(
                "The number of decision variables: " +
                    utility::to_string(this->number_of_variables(), "%d"),
                true);
        } else {
            utility::print_info(
                "The number of decision variables: " +
                    utility::to_string(this->number_of_variables(), "%d") +
                    " (reduced to " +
                    utility::to_string((this->number_of_variables() -
                                        this->number_of_fixed_variables()),
                                       "%d") +
                    ")",
                true);
        }

        utility::print_info(
            " -- Selection: " +
                utility::to_string(this->number_of_selection_variables(), "%d"),
            true);
        utility::print_info(
            " -- Binary: " +
                utility::to_string(this->number_of_binary_variables(), "%d"),
            true);
        utility::print_info(
            " -- Integer: " +
                utility::to_string(this->number_of_integer_variables(), "%d"),
            true);

        utility::print_info(
            "The number of fixed decision variables: " +
                utility::to_string(this->number_of_fixed_variables(), "%d"),
            true);
    }

    /*************************************************************************/
    inline constexpr void print_number_of_constraints(void) const {
        utility::print_single_line(true);
        if (this->number_of_disabled_constraints() == 0) {
            utility::print_info(
                "The number of constraints: " +
                    utility::to_string(this->number_of_constraints(), "%d"),
                true);
        } else {
            utility::print_info(
                "The number of constraints: " +
                    utility::to_string(this->number_of_constraints(), "%d") +
                    " (reduced to " +
                    utility::to_string((this->number_of_constraints() -
                                        this->number_of_disabled_constraints()),
                                       "%d") +
                    ")",
                true);
        }

        utility::print_info(
            " -- Singleton: " +
                utility::to_string(
                    m_constraint_type_reference.singleton_ptrs.size(), "%d"),
            true);
        utility::print_info(
            " -- Aggregation: " +
                utility::to_string(
                    m_constraint_type_reference.aggregation_ptrs.size(), "%d"),
            true);
        utility::print_info(
            " -- Precedence: " +
                utility::to_string(
                    m_constraint_type_reference.precedence_ptrs.size(), "%d"),
            true);
        utility::print_info(
            " -- Variable Bound: " +
                utility::to_string(
                    m_constraint_type_reference.variable_bound_ptrs.size(),
                    "%d"),
            true);
        utility::print_info(
            " -- Set Partitioning: " +
                utility::to_string(
                    m_constraint_type_reference.set_partitioning_ptrs.size(),
                    "%d"),
            true);
        utility::print_info(
            " -- Set Packing: " +
                utility::to_string(
                    m_constraint_type_reference.set_packing_ptrs.size(), "%d"),
            true);
        utility::print_info(
            " -- Set Covering: " +
                utility::to_string(
                    m_constraint_type_reference.set_covering_ptrs.size(), "%d"),
            true);
        utility::print_info(
            " -- Cardinality: " +
                utility::to_string(
                    m_constraint_type_reference.cardinality_ptrs.size(), "%d"),
            true);
        utility::print_info(
            " -- Invariant Knapsack: " +
                utility::to_string(
                    m_constraint_type_reference.invariant_knapsack_ptrs.size(),
                    "%d"),
            true);
        utility::print_info(
            " -- Equation Knapsack: " +
                utility::to_string(
                    m_constraint_type_reference.equation_knapsack_ptrs.size(),
                    "%d"),
            true);
        utility::print_info(
            " -- Bin Packing: " +
                utility::to_string(
                    m_constraint_type_reference.bin_packing_ptrs.size(), "%d"),
            true);
        utility::print_info(
            " -- Knapsack: " +
                utility::to_string(
                    m_constraint_type_reference.knapsack_ptrs.size(), "%d"),
            true);
        utility::print_info(
            " -- Integer Knapsack: " +
                utility::to_string(
                    m_constraint_type_reference.integer_knapsack_ptrs.size(),
                    "%d"),
            true);
        utility::print_info(
            " -- General Linear: " +
                utility::to_string(
                    m_constraint_type_reference.general_linear_ptrs.size(),
                    "%d"),
            true);
        utility::print_info(
            " -- Nonlinear: " +
                utility::to_string(
                    m_constraint_type_reference.nonlinear_ptrs.size(), "%d"),
            true);

        utility::print_info(
            "The number of reduced constraints: " +
                utility::to_string(this->number_of_disabled_constraints(),
                                   "%d"),
            true);
    }

    /*************************************************************************/
    inline constexpr void set_callback(
        const std::function<void(void)> &a_CALLBACK) {
        m_callback = a_CALLBACK;
    }

    /*************************************************************************/
    inline constexpr void callback(void) {
        m_callback();
    }

    /*************************************************************************/
    inline constexpr void import_variable_values(
        const std::vector<ValueProxy<T_Variable>> &a_PROXIES) {
        for (auto &&proxy : m_variable_proxies) {
            for (auto &&variable : proxy.flat_indexed_variables()) {
                int id         = variable.id();
                int flat_index = variable.flat_index();
                variable.set_value_if_not_fixed(
                    a_PROXIES[id].flat_indexed_values(flat_index));
            }
        }
        this->verify_and_correct_selection_variables_initial_values(false,
                                                                    false);
        this->verify_and_correct_binary_variables_initial_values(false, false);
        this->verify_and_correct_integer_variables_initial_values(false, false);
    }

    /*************************************************************************/
    inline constexpr void update(void) {
        /**
         * Update in order of expressions -> objective, constraints. For
         * typical problem.
         */
        for (auto &&proxy : m_expression_proxies) {
            for (auto &&expression : proxy.flat_indexed_expressions()) {
                expression.update();
            }
        }

        for (auto &&proxy : m_constraint_proxies) {
            for (auto &&constraint : proxy.flat_indexed_constraints()) {
                constraint.update();
            }
        }

        if (m_is_defined_objective) {
            m_objective.update();
        }

        this->update_feasibility();
    }

    /*************************************************************************/
    inline constexpr void update(const Move<T_Variable, T_Expression> &a_MOVE) {
        /**
         * Update in order of objective, constraints -> expressions ->
         * variables.
         */
        if (m_is_defined_objective) {
            m_objective.update(a_MOVE);
        }

        if (m_neighborhood.is_enabled_user_defined_move()) {
            for (auto &&proxy : m_constraint_proxies) {
                for (auto &&constraint : proxy.flat_indexed_constraints()) {
                    if (constraint.is_enabled()) {
                        constraint.update(a_MOVE);
                    }
                }
            }
        } else {
            for (auto &&constraint_ptr : a_MOVE.related_constraint_ptrs) {
                constraint_ptr->update(a_MOVE);
            }
        }

        for (auto &&proxy : m_expression_proxies) {
            for (auto &&expression : proxy.flat_indexed_expressions()) {
                if (expression.is_enabled()) {
                    expression.update(a_MOVE);
                }
            }
        }

        for (auto &&alteration : a_MOVE.alterations) {
            alteration.first->set_value_if_not_fixed(alteration.second);
        }

        if (a_MOVE.sense == MoveSense::Selection) {
            a_MOVE.alterations[1].first->select();
        }

        this->update_feasibility();
    }

    /*************************************************************************/
    inline constexpr void reset_variable_objective_improvability(
        const std::vector<Variable<T_Variable, T_Expression> *>
            &a_VARIABLE_PTRS) {
        for (auto &&variable_ptr : a_VARIABLE_PTRS) {
            variable_ptr->set_is_objective_improvable(false);
        }
    }

    /*************************************************************************/
    inline constexpr void reset_variable_objective_improvability(void) {
        this->reset_variable_objective_improvability(
            this->variable_reference().variable_ptrs);
    }

    /*************************************************************************/
    inline constexpr void reset_variable_feasibility_improvability(
        const std::vector<Variable<T_Variable, T_Expression> *>
            &a_VARIABLE_PTRS) const noexcept {
        for (auto &&variable_ptr : a_VARIABLE_PTRS) {
            variable_ptr->set_is_feasibility_improvable(false);
        }
    }

    /*************************************************************************/
    inline constexpr void reset_variable_feasibility_improvability(
        const std::vector<Constraint<T_Variable, T_Expression> *>
            &a_CONSTRAINT_PTRS) const noexcept {
        for (const auto &constraint_ptr : a_CONSTRAINT_PTRS) {
            if (!constraint_ptr->is_enabled()) {
                continue;
            }
            auto &sensitivities = constraint_ptr->expression().sensitivities();
            for (const auto &sensitivity : sensitivities) {
                sensitivity.first->set_is_feasibility_improvable(false);
            }
        }
    }

    /*************************************************************************/
    inline constexpr void reset_variable_feasibility_improvability(void) {
        this->reset_variable_feasibility_improvability(
            this->variable_reference().variable_ptrs);
    }

    /*************************************************************************/
    inline constexpr void update_variable_objective_improvability(void) {
        this->update_variable_objective_improvability(
            this->variable_reference().variable_ptrs);
    }

    /*************************************************************************/
    inline constexpr void update_variable_objective_improvability(
        const std::vector<Variable<T_Variable, T_Expression> *>
            &a_VARIABLE_PTRS) const noexcept {
        for (const auto &variable_ptr : a_VARIABLE_PTRS) {
            auto coefficient =
                variable_ptr->objective_sensitivity() * this->sign();
            if (coefficient > 0 &&  //
                (variable_ptr->value() > variable_ptr->lower_bound())) {
                variable_ptr->set_is_objective_improvable(true);
            } else if (coefficient < 0 &&  //
                       (variable_ptr->value() < variable_ptr->upper_bound())) {
                variable_ptr->set_is_objective_improvable(true);
            } else {
                variable_ptr->set_is_objective_improvable(false);
            }
        }
    }

    /*************************************************************************/
    inline constexpr void update_variable_feasibility_improvability(void) {
        this->update_variable_feasibility_improvability(
            this->constraint_reference().constraint_ptrs);
    }

    /*************************************************************************/
    inline constexpr void update_variable_feasibility_improvability(
        const std::vector<Constraint<T_Variable, T_Expression> *>
            &a_CONSTRAINT_PTRS) const noexcept {
        for (const auto &constraint_ptr : a_CONSTRAINT_PTRS) {
            if (!constraint_ptr->is_enabled()) {
                continue;
            }
            if (constraint_ptr->violation_value() < constant::EPSILON) {
                continue;
            }
            const auto &sensitivities =
                constraint_ptr->expression().sensitivities();
            const auto &constraint_value = constraint_ptr->constraint_value();

            const int MASK_LOWER_OR_EQUAL = 0b10;
            const int MASK_UPPER_OR_EQUAL = 0b11;

            if (constraint_value > 0) {
                if ((constraint_ptr->sense() & MASK_LOWER_OR_EQUAL) == 0) {
                    for (const auto &sensitivity : sensitivities) {
                        const auto &variable_ptr = sensitivity.first;
                        const auto &coefficient  = sensitivity.second;

                        if (variable_ptr->is_feasibility_improvable()) {
                            continue;
                        }

                        if (coefficient > 0 && (variable_ptr->value() >
                                                variable_ptr->lower_bound())) {
                            variable_ptr->set_is_feasibility_improvable(true);

                        } else if (coefficient < 0 &&
                                   (variable_ptr->value() <
                                    variable_ptr->upper_bound())) {
                            variable_ptr->set_is_feasibility_improvable(true);
                        }
                    }
                }
            } else if (constraint_value < 0) {
                if ((constraint_ptr->sense() & MASK_UPPER_OR_EQUAL)) {
                    for (const auto &sensitivity : sensitivities) {
                        const auto &variable_ptr = sensitivity.first;
                        const auto &coefficient  = sensitivity.second;

                        if (variable_ptr->is_feasibility_improvable()) {
                            continue;
                        }

                        if (coefficient > 0 && (variable_ptr->value() <
                                                variable_ptr->upper_bound())) {
                            variable_ptr->set_is_feasibility_improvable(true);

                        } else if (coefficient < 0 &&
                                   (variable_ptr->value() >
                                    variable_ptr->lower_bound())) {
                            variable_ptr->set_is_feasibility_improvable(true);
                        }
                    }
                }
            }
        }
    }

    /*************************************************************************/
    inline constexpr void update_feasibility(void) {
        for (const auto &proxy : m_constraint_proxies) {
            for (const auto &constraint : proxy.flat_indexed_constraints()) {
                if (constraint.violation_value() > constant::EPSILON) {
                    m_is_feasible = false;
                    return;
                }
            }
        }
        m_is_feasible = true;
    }

    /*************************************************************************/
    inline SolutionScore evaluate(
        const Move<T_Variable, T_Expression> &a_MOVE,
        const std::vector<ValueProxy<double>>
            &a_LOCAL_PENALTY_COEFFICIENT_PROXIES,
        const std::vector<ValueProxy<double>>
            &a_GLOBAL_PENALTY_COEFFICIENT_PROXIES) const noexcept {
        double total_violation = 0.0;
        double local_penalty   = 0.0;
        double global_penalty  = 0.0;

        int  constraint_proxies_size  = m_constraint_proxies.size();
        bool is_constraint_improvable = false;

        for (auto i = 0; i < constraint_proxies_size; i++) {
            auto &constraints =
                m_constraint_proxies[i].flat_indexed_constraints();

            int constraints_size = constraints.size();
            for (auto j = 0; j < constraints_size; j++) {
                if (!constraints[j].is_enabled()) {
                    continue;
                }
                double violation = constraints[j].evaluate_violation(a_MOVE);

                if (violation + constant::EPSILON <
                    constraints[j].violation_value()) {
                    is_constraint_improvable = true;
                }
                total_violation += violation;

                local_penalty +=
                    a_LOCAL_PENALTY_COEFFICIENT_PROXIES[i].flat_indexed_values(
                        j) *
                    violation;
                global_penalty +=
                    a_GLOBAL_PENALTY_COEFFICIENT_PROXIES[i].flat_indexed_values(
                        j) *
                    violation;
            }
        }

        double objective             = 0.0;
        double objective_improvement = 0.0;

        if (m_is_defined_objective) {
            objective = m_objective.evaluate(a_MOVE) * this->sign();
            objective_improvement =
                m_objective.value() * this->sign() - objective;
        }

        double local_augmented_objective  = objective + local_penalty;
        double global_augmented_objective = objective + global_penalty;

        SolutionScore score;

        score.objective                  = objective;
        score.objective_improvement      = objective_improvement;
        score.total_violation            = total_violation;
        score.local_penalty              = local_penalty;
        score.global_penalty             = global_penalty;
        score.local_augmented_objective  = local_augmented_objective;
        score.global_augmented_objective = global_augmented_objective;
        score.is_objective_improvable =
            objective_improvement > constant::EPSILON;
        score.is_constraint_improvable = is_constraint_improvable;
        score.is_feasible              = !(total_violation > constant::EPSILON);

        return score;
    }

    /*************************************************************************/
    inline SolutionScore evaluate(
        const Move<T_Variable, T_Expression> &a_MOVE,
        const SolutionScore &                 a_CURRENT_SCORE,
        const std::vector<ValueProxy<double>>
            &a_LOCAL_PENALTY_COEFFICIENT_PROXIES,
        const std::vector<ValueProxy<double>>
            &a_GLOBAL_PENALTY_COEFFICIENT_PROXIES) const noexcept {
        SolutionScore score = a_CURRENT_SCORE;

        bool is_constraint_improvable = false;

        double total_violation = score.total_violation;
        double local_penalty   = score.local_penalty;
        double global_penalty  = score.global_penalty;

        for (const auto &constraint_ptr : a_MOVE.related_constraint_ptrs) {
            if (!constraint_ptr->is_enabled()) {
                continue;
            }
            double violation_diff = constraint_ptr->evaluate_violation(a_MOVE) -
                                    constraint_ptr->violation_value();
            total_violation += violation_diff;

            if (violation_diff + constant::EPSILON < 0) {
                is_constraint_improvable = true;
            }

            int id         = constraint_ptr->id();
            int flat_index = constraint_ptr->flat_index();

            local_penalty +=
                violation_diff *
                a_LOCAL_PENALTY_COEFFICIENT_PROXIES[id].flat_indexed_values(
                    flat_index);
            global_penalty +=
                violation_diff *
                a_GLOBAL_PENALTY_COEFFICIENT_PROXIES[id].flat_indexed_values(
                    flat_index);
        }

        double objective             = 0.0;
        double objective_improvement = 0.0;

        if (m_is_defined_objective) {
            objective = m_objective.evaluate(a_MOVE) * this->sign();
            objective_improvement =
                m_objective.value() * this->sign() - objective;
        }

        double local_augmented_objective  = objective + local_penalty;
        double global_augmented_objective = objective + global_penalty;

        score.objective                  = objective;
        score.objective_improvement      = objective_improvement;
        score.total_violation            = total_violation;
        score.local_penalty              = local_penalty;
        score.global_penalty             = global_penalty;
        score.local_augmented_objective  = local_augmented_objective;
        score.global_augmented_objective = global_augmented_objective;
        score.is_objective_improvable =
            objective_improvement > constant::EPSILON;
        score.is_constraint_improvable = is_constraint_improvable;
        score.is_feasible              = !(total_violation > constant::EPSILON);

        return score;
    }

    /*************************************************************************/
    double compute_lagrangian(const std::vector<model::ValueProxy<double>>
                                  &a_LAGRANGE_MULTIPLIER_PROXIES) {
        double lagrangian = m_objective.value();

        for (auto &&constraint_ptr : m_constraint_reference.constraint_ptrs) {
            int id         = constraint_ptr->id();
            int flat_index = constraint_ptr->flat_index();

            lagrangian += a_LAGRANGE_MULTIPLIER_PROXIES[id].flat_indexed_values(
                              flat_index) *
                          constraint_ptr->constraint_value();
        }
        return lagrangian;
    }

    /*************************************************************************/
    template <class T_Value>
    inline constexpr std::vector<ValueProxy<T_Value>>
    generate_variable_parameter_proxies(const T_Value a_VALUE) const {
        std::vector<ValueProxy<T_Value>> variable_parameter_proxies;

        for (const auto &proxy : m_variable_proxies) {
            ValueProxy<T_Value> variable_parameter_proxy(proxy.id(),
                                                         proxy.shape());
            variable_parameter_proxy.fill(a_VALUE);
            int number_of_elements = proxy.number_of_elements();
            for (auto i = 0; i < number_of_elements; i++) {
                variable_parameter_proxy.flat_indexed_names(i) =
                    proxy.flat_indexed_variables(i).name();
            }

            variable_parameter_proxies.push_back(variable_parameter_proxy);
        }
        return variable_parameter_proxies;
    }

    /*************************************************************************/
    template <class T_Value>
    inline constexpr std::vector<ValueProxy<T_Value>>
    generate_expression_parameter_proxies(const T_Value a_VALUE) const {
        std::vector<ValueProxy<T_Value>> expression_parameter_proxies;

        for (const auto &proxy : m_expression_proxies) {
            ValueProxy<T_Value> expression_parameter_proxy(proxy.id(),
                                                           proxy.shape());
            expression_parameter_proxy.fill(a_VALUE);
            int number_of_elements = proxy.number_of_elements();
            for (auto i = 0; i < number_of_elements; i++) {
                expression_parameter_proxy.flat_indexed_names(i) =
                    proxy.flat_indexed_expressions(i).name();
            }
            expression_parameter_proxies.push_back(expression_parameter_proxy);
        }
        return expression_parameter_proxies;
    }

    /*************************************************************************/
    template <class T_Value>
    inline constexpr std::vector<ValueProxy<T_Value>>
    generate_constraint_parameter_proxies(const T_Value a_VALUE) const {
        std::vector<ValueProxy<T_Value>> constraint_parameter_proxies;

        for (const auto &proxy : m_constraint_proxies) {
            ValueProxy<T_Value> constraint_parameter_proxy(proxy.id(),
                                                           proxy.shape());
            constraint_parameter_proxy.fill(a_VALUE);
            int number_of_elements = proxy.number_of_elements();
            for (auto i = 0; i < number_of_elements; i++) {
                constraint_parameter_proxy.flat_indexed_names(i) =
                    proxy.flat_indexed_constraints(i).name();
            }
            constraint_parameter_proxies.push_back(constraint_parameter_proxy);
        }
        return constraint_parameter_proxies;
    }

    /*************************************************************************/
    inline Solution<T_Variable, T_Expression> export_solution(void) const {
        /// This method cannot be constexpr by clang.
        Solution<T_Variable, T_Expression> solution;

        /// Decision variables
        for (const auto &proxy : m_variable_proxies) {
            solution.variable_value_proxies.push_back(
                proxy.export_values_and_names());
        }

        /// Expressions
        for (const auto &proxy : m_expression_proxies) {
            solution.expression_value_proxies.push_back(
                proxy.export_values_and_names());
        }

        /// Constraints
        for (const auto &proxy : m_constraint_proxies) {
            solution.constraint_value_proxies.push_back(
                proxy.export_values_and_names());
            solution.violation_value_proxies.push_back(
                proxy.export_violations_and_names());
        }

        /// Total violation
        T_Expression total_violation = 0;
        for (const auto &proxy : m_constraint_proxies) {
            for (const auto &constraint : proxy.flat_indexed_constraints()) {
                total_violation += constraint.violation_value();
            }
        }

        solution.objective       = m_objective.value();
        solution.total_violation = total_violation;
        solution.is_feasible     = this->is_feasible();

        return solution;
    }

    /*************************************************************************/
    inline constexpr NamedSolution<T_Variable, T_Expression>
    export_named_solution(void) const {
        return this->convert_to_named_solution(this->export_solution());
    }

    /*************************************************************************/
    inline NamedSolution<T_Variable, T_Expression> convert_to_named_solution(
        const Solution<T_Variable, T_Expression> &a_SOLUTION) const {
        /// This method cannot be constexpr by clang.
        NamedSolution<T_Variable, T_Expression> named_solution;

        int variable_proxies_size   = m_variable_proxies.size();
        int expression_proxies_size = m_expression_proxies.size();
        int constraint_proxies_size = m_constraint_proxies.size();

        /// Decision variables
        for (auto i = 0; i < variable_proxies_size; i++) {
            named_solution.m_variable_value_proxies[m_variable_names[i]] =
                a_SOLUTION.variable_value_proxies[i];
        }

        /// Expression
        for (auto i = 0; i < expression_proxies_size; i++) {
            named_solution.m_expression_value_proxies[m_expression_names[i]] =
                a_SOLUTION.expression_value_proxies[i];
        }

        /// Constraint
        for (auto i = 0; i < constraint_proxies_size; i++) {
            named_solution.m_constraint_value_proxies[m_constraint_names[i]] =
                a_SOLUTION.constraint_value_proxies[i];
        }

        /// Violation
        for (auto i = 0; i < constraint_proxies_size; i++) {
            named_solution.m_violation_value_proxies[m_constraint_names[i]] =
                a_SOLUTION.violation_value_proxies[i];
        }

        named_solution.m_objective       = a_SOLUTION.objective;
        named_solution.m_total_violation = a_SOLUTION.total_violation;
        named_solution.m_is_feasible     = a_SOLUTION.is_feasible;

        return named_solution;
    }

    /*************************************************************************/
    inline constexpr PlainSolution<T_Variable, T_Expression>
    export_plain_solution(void) const {
        PlainSolution<T_Variable, T_Expression> plain_solution;

        /// Decision variables
        for (const auto &proxy : m_variable_proxies) {
            for (const auto &variable : proxy.flat_indexed_variables()) {
                plain_solution.variables.push_back(variable.value());
            }
        }

        /// Total violation
        T_Expression total_violation = 0;
        for (const auto &proxy : m_constraint_proxies) {
            for (const auto &constraint : proxy.flat_indexed_constraints()) {
                total_violation += constraint.violation_value();
            }
        }

        plain_solution.objective       = m_objective.value();
        plain_solution.total_violation = total_violation;
        plain_solution.is_feasible     = this->is_feasible();

        return plain_solution;
    }

    /*************************************************************************/
    inline constexpr PlainSolution<T_Variable, T_Expression>
    convert_to_plain_solution(
        const Solution<T_Variable, T_Expression> &a_SOLUTION) const {
        PlainSolution<T_Variable, T_Expression> plain_solution;

        /// Decision variables
        for (const auto &proxy : a_SOLUTION.variable_value_proxies) {
            for (const auto &value : proxy.flat_indexed_values()) {
                plain_solution.variables.push_back(value);
            }
        }

        plain_solution.objective       = a_SOLUTION.objective;
        plain_solution.total_violation = a_SOLUTION.total_violation;
        plain_solution.is_feasible     = a_SOLUTION.is_feasible;

        return plain_solution;
    }

    /*************************************************************************/
    inline constexpr void import_solution(
        const std::unordered_map<std::string, int> &a_SOLUTION) {
        for (auto &&proxy : m_variable_proxies) {
            for (auto &&variable : proxy.flat_indexed_variables()) {
                if (a_SOLUTION.find(variable.name()) != a_SOLUTION.end()) {
                    variable = a_SOLUTION.at(variable.name());
                } else {
                    variable = 0;
                }
            }
        }
    }

    /*************************************************************************/
    inline ModelSummary export_summary(void) const {
        /// This method cannot be constexpr by clang.
        ModelSummary summary;
        summary.name                  = m_name;
        summary.number_of_variables   = this->number_of_variables();
        summary.number_of_constraints = this->number_of_constraints();
        return summary;
    }

    /*************************************************************************/
    inline constexpr std::vector<VariableProxy<T_Variable, T_Expression>>
        &variable_proxies(void) {
        return m_variable_proxies;
    }

    /*************************************************************************/
    inline constexpr const std::vector<VariableProxy<T_Variable, T_Expression>>
        &variable_proxies(void) const {
        return m_variable_proxies;
    }

    /*************************************************************************/
    inline constexpr std::vector<ExpressionProxy<T_Variable, T_Expression>>
        &expression_proxies(void) {
        return m_expression_proxies;
    }

    /*************************************************************************/
    inline constexpr const std::vector<
        ExpressionProxy<T_Variable, T_Expression>>
        &expression_proxies(void) const {
        return m_expression_proxies;
    }

    /*************************************************************************/
    inline constexpr std::vector<ConstraintProxy<T_Variable, T_Expression>>
        &constraint_proxies(void) {
        return m_constraint_proxies;
    }

    /*************************************************************************/
    inline constexpr const std::vector<
        ConstraintProxy<T_Variable, T_Expression>>
        &constraint_proxies(void) const {
        return m_constraint_proxies;
    }

    /*************************************************************************/
    inline constexpr Objective<T_Variable, T_Expression> &objective(void) {
        return m_objective;
    }

    /*************************************************************************/
    inline constexpr const Objective<T_Variable, T_Expression> &objective(
        void) const {
        return m_objective;
    }

    /*************************************************************************/
    inline constexpr const std::vector<std::string> &variable_names(
        void) const {
        return m_variable_names;
    }

    /*************************************************************************/
    inline constexpr const std::vector<std::string> &expression_names(
        void) const {
        return m_expression_names;
    }

    /*************************************************************************/
    inline constexpr const std::vector<std::string> &constraint_names(
        void) const {
        return m_constraint_names;
    }

    /*************************************************************************/
    inline constexpr const std::vector<Selection<T_Variable, T_Expression>>
        &selections(void) const {
        return m_selections;
    }

    /*************************************************************************/
    inline constexpr VariableReference<T_Variable, T_Expression>
    variable_reference(void) const {
        return m_variable_reference;
    }

    /*************************************************************************/
    inline constexpr ConstraintReference<T_Variable, T_Expression>
    constraint_reference(void) const {
        return m_constraint_reference;
    }

    /*************************************************************************/
    inline constexpr ConstraintTypeReference<T_Variable, T_Expression>
    constraint_type_reference(void) const {
        return m_constraint_type_reference;
    }

    /*************************************************************************/
    inline constexpr bool is_defined_objective(void) const {
        return m_is_defined_objective;
    }

    /*************************************************************************/
    inline constexpr bool is_enabled_fast_evaluation(void) const {
        return m_is_enabled_fast_evaluation;
    }

    /*************************************************************************/
    inline constexpr bool is_linear(void) const {
        return m_is_linear;
    }

    /*************************************************************************/
    inline constexpr bool is_minimization(void) const {
        return m_is_minimization;
    }

    /*************************************************************************/
    inline constexpr double sign(void) const {
        /**
         * In this program, maximization problems are solved as minimization
         * problems by nagating the objective function values. This method is
         * used to show objective function values for output.
         */
        return m_is_minimization ? 1.0 : -1.0;
    }

    /*************************************************************************/
    inline constexpr void set_is_solved(const bool a_IS_SOLVED) {
        m_is_solved = a_IS_SOLVED;
    }

    /*************************************************************************/
    inline constexpr bool is_solved(void) const {
        return m_is_solved;
    }

    /*************************************************************************/
    inline constexpr bool is_feasible(void) const {
        return m_is_feasible;
    }

    /*************************************************************************/
    inline constexpr int number_of_variables(void) const {
        return m_variable_reference.variable_ptrs.size();
    }

    /*************************************************************************/
    inline constexpr int number_of_fixed_variables(void) const {
        return m_variable_reference.fixed_variable_ptrs.size();
    }

    /*************************************************************************/
    inline constexpr int number_of_not_fixed_variables(void) const {
        return m_variable_reference.not_fixed_variable_ptrs.size();
    }

    /*************************************************************************/
    inline constexpr int number_of_selection_variables(void) const {
        return m_variable_reference.selection_variable_ptrs.size();
    }

    /*************************************************************************/
    inline constexpr int number_of_binary_variables(void) const {
        return m_variable_reference.binary_variable_ptrs.size();
    }

    /*************************************************************************/
    inline constexpr int number_of_integer_variables(void) const {
        return m_variable_reference.integer_variable_ptrs.size();
    }

    /*************************************************************************/
    inline constexpr int number_of_constraints(void) const {
        return m_constraint_reference.constraint_ptrs.size();
    }

    /*************************************************************************/
    inline constexpr int number_of_selection_constraints(void) const {
        return m_constraint_reference.selection_constraint_ptrs.size();
    }

    /*************************************************************************/
    inline constexpr int number_of_disabled_constraints(void) const {
        return m_constraint_reference.disabled_constraint_ptrs.size();
    }

    /*************************************************************************/
    inline constexpr Neighborhood<T_Variable, T_Expression> &neighborhood(
        void) {
        return m_neighborhood;
    }
};
using IPModel = Model<int, double>;
}  // namespace model
}  // namespace printemps
#endif
/*****************************************************************************/
// END
/*****************************************************************************/