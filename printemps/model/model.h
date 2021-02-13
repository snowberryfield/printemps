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

#include "presolver.h"
#include "verifier.h"

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
    void initialize(void) {
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
    constexpr VariableProxy<T_Variable, T_Expression> &create_variable(
        const std::string &a_NAME) {
        if (utility::has_space(a_NAME)) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The name of decision variable must not contain spaces."));
        }

        int proxy_index = m_variable_proxies.size();

        if (proxy_index >= ModelConstant::MAX_NUMBER_OF_VARIABLE_PROXIES) {
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
                proxy_index));
        m_variable_names.push_back(a_NAME);

        return m_variable_proxies.back();
    }

    /*************************************************************************/
    constexpr VariableProxy<T_Variable, T_Expression> &create_variable(
        const std::string &a_NAME,         //
        const T_Variable   a_LOWER_BOUND,  //
        const T_Variable   a_UPPER_BOUND) {
        auto &variable_proxy = this->create_variable(a_NAME);
        variable_proxy.set_bound(a_LOWER_BOUND, a_UPPER_BOUND);

        return m_variable_proxies.back();
    }

    /*************************************************************************/
    constexpr VariableProxy<T_Variable, T_Expression> &create_variables(
        const std::string &a_NAME,  //
        const int          a_NUMBER_OF_ELEMENTS) {
        if (utility::has_space(a_NAME)) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The name of decision variable must not contain spaces."));
        }

        int proxy_index = m_variable_proxies.size();

        if (proxy_index >= ModelConstant::MAX_NUMBER_OF_VARIABLE_PROXIES) {
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
                proxy_index, a_NUMBER_OF_ELEMENTS));
        m_variable_names.push_back(a_NAME);

        return m_variable_proxies.back();
    }

    /*************************************************************************/
    constexpr VariableProxy<T_Variable, T_Expression> &create_variables(
        const std::string &a_NAME,                //
        const int          a_NUMBER_OF_ELEMENTS,  //
        const T_Variable   a_LOWER_BOUND,         //
        const T_Variable   a_UPPER_BOUND) {
        auto &variable_proxy = create_variables(a_NAME, a_NUMBER_OF_ELEMENTS);
        variable_proxy.set_bound(a_LOWER_BOUND, a_UPPER_BOUND);

        return m_variable_proxies.back();
    }

    /*************************************************************************/
    constexpr VariableProxy<T_Variable, T_Expression> &create_variables(
        const std::string &     a_NAME,  //
        const std::vector<int> &a_SHAPE) {
        if (utility::has_space(a_NAME)) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The name of decision variable must not contain spaces."));
        }

        int proxy_index = m_variable_proxies.size();

        if (proxy_index >= ModelConstant::MAX_NUMBER_OF_VARIABLE_PROXIES) {
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
                proxy_index, a_SHAPE));
        m_variable_names.push_back(a_NAME);

        return m_variable_proxies.back();
    }

    /*************************************************************************/
    constexpr VariableProxy<T_Variable, T_Expression> &create_variables(
        const std::string &     a_NAME,         //
        const std::vector<int> &a_SHAPE,        //
        const T_Variable        a_LOWER_BOUND,  //
        const T_Variable        a_UPPER_BOUND) {
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

        int proxy_index = m_expression_proxies.size();

        if (proxy_index >= ModelConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES) {
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
                proxy_index));
        m_expression_names.push_back(a_NAME);

        return m_expression_proxies.back();
    }

    /*************************************************************************/
    constexpr ExpressionProxy<T_Variable, T_Expression> &create_expressions(
        const std::string &a_NAME,  //
        int                a_NUMBER_OF_ELEMENTS) {
        if (utility::has_space(a_NAME)) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The name of expression must not contain spaces."));
        }

        int proxy_index = m_expression_proxies.size();

        if (proxy_index >= ModelConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES) {
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
                proxy_index, a_NUMBER_OF_ELEMENTS));
        m_expression_names.push_back(a_NAME);

        return m_expression_proxies.back();
    }

    /*************************************************************************/
    constexpr ExpressionProxy<T_Variable, T_Expression> &create_expressions(
        const std::string &     a_NAME,  //
        const std::vector<int> &a_SHAPE) {
        if (utility::has_space(a_NAME)) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The name of expression must not contain spaces."));
        }

        int proxy_index = m_expression_proxies.size();

        if (proxy_index >= ModelConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES) {
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
                proxy_index, a_SHAPE));
        m_expression_names.push_back(a_NAME);

        return m_expression_proxies.back();
    }

    /*************************************************************************/
    template <template <class, class> class T_ExpressionLike>
    constexpr ExpressionProxy<T_Variable, T_Expression> &create_expression(
        const std::string &                               a_NAME,  //
        const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE) {
        if (utility::has_space(a_NAME)) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The name of expression must not contain spaces."));
        }

        int proxy_index = m_expression_proxies.size();

        if (proxy_index >= ModelConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES) {
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
                proxy_index));
        m_expression_names.push_back(a_NAME);
        m_expression_proxies.back() = a_EXPRESSION_LIKE.to_expression();

        return m_expression_proxies.back();
    }

    /*************************************************************************/
    constexpr ExpressionProxy<T_Variable, T_Expression> &create_expression(
        const std::string &                         a_NAME,  //
        const Expression<T_Variable, T_Expression> &a_EXPRESSION) {
        if (utility::has_space(a_NAME)) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The name of expression must not contain spaces."));
        }

        int proxy_index = m_expression_proxies.size();

        if (proxy_index >= ModelConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES) {
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
                proxy_index));
        m_expression_names.push_back(a_NAME);
        m_expression_proxies.back() = a_EXPRESSION;

        return m_expression_proxies.back();
    }

    /*************************************************************************/
    constexpr ConstraintProxy<T_Variable, T_Expression> &create_constraint(
        const std::string &a_NAME) {
        if (utility::has_space(a_NAME)) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The name of constraint must not contain spaces."));
        }

        int proxy_index = m_constraint_proxies.size();

        if (proxy_index >= ModelConstant::MAX_NUMBER_OF_CONSTRAINT_PROXIES) {
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
                proxy_index));
        m_constraint_names.push_back(a_NAME);

        return m_constraint_proxies.back();
    }

    /*************************************************************************/
    constexpr ConstraintProxy<T_Variable, T_Expression> &create_constraints(
        const std::string &a_NAME,  //
        int                a_NUMBER_OF_ELEMENTS) {
        if (utility::has_space(a_NAME)) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The name of constraint must not contain spaces."));
        }

        int proxy_index = m_constraint_proxies.size();

        if (proxy_index >= ModelConstant::MAX_NUMBER_OF_CONSTRAINT_PROXIES) {
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
                proxy_index, a_NUMBER_OF_ELEMENTS));
        m_constraint_names.push_back(a_NAME);

        return m_constraint_proxies.back();
    }

    /*************************************************************************/
    constexpr ConstraintProxy<T_Variable, T_Expression> &create_constraints(
        const std::string &     a_NAME,  //
        const std::vector<int> &a_SHAPE) {
        if (utility::has_space(a_NAME)) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The name of constraint must not contain spaces."));
        }

        int proxy_index = m_constraint_proxies.size();

        if (proxy_index >= ModelConstant::MAX_NUMBER_OF_CONSTRAINT_PROXIES) {
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
                proxy_index, a_SHAPE));
        m_constraint_names.push_back(a_NAME);

        return m_constraint_proxies.back();
    }

    /*************************************************************************/
    constexpr ConstraintProxy<T_Variable, T_Expression> &create_constraint(
        const std::string &                         a_NAME,  //
        const Constraint<T_Variable, T_Expression> &a_CONSTRAINT) {
        if (utility::has_space(a_NAME)) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The name of constraint must not contain spaces."));
        }

        int proxy_index = m_constraint_proxies.size();

        if (proxy_index >= ModelConstant::MAX_NUMBER_OF_CONSTRAINT_PROXIES) {
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
                proxy_index));
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
    constexpr void setup(const bool a_IS_ENABLED_PRESOLVE,                  //
                         const bool a_IS_ENABLED_INITIAL_VALUE_CORRECTION,  //
                         const bool a_IS_ENABLED_AGGREGATION_MOVE,          //
                         const bool a_IS_ENABLED_PRECEDENCE_MOVE,           //
                         const bool a_IS_ENABLED_VARIABLE_BOUND_MOVE,       //
                         const bool a_IS_ENABLED_EXCLUSIVE_MOVE,            //
                         const bool a_IS_ENABLED_CHAIN_MOVE,                //
                         const bool a_IS_ENABLED_USER_DEFINED_MOVE,         //
                         const SelectionMode &a_SELECTION_MODE,             //
                         const bool           a_IS_ENABLED_PRINT) {
        verify_problem(this, a_IS_ENABLED_PRINT);

        this->setup_variable_related_constraints();
        this->setup_unique_name();
        this->setup_is_linear();

        if (this->is_linear()) {
            this->setup_variable_sensitivity();
        }

        this->categorize_variables();
        this->categorize_constraints();

        /**
         * Presolve the problem by removing redundant constraints and fixing
         * decision variables implicitly fixed.
         */
        if (a_IS_ENABLED_PRESOLVE) {
            presolve(this, a_IS_ENABLED_PRINT);
        }

        /// Categorize again to reflect the presolving result.
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

        verify_and_correct_selection_variables_initial_values(  //
            this,                                               //
            a_IS_ENABLED_INITIAL_VALUE_CORRECTION,              //
            a_IS_ENABLED_PRINT);

        verify_and_correct_binary_variables_initial_values(
            this,                                   //
            a_IS_ENABLED_INITIAL_VALUE_CORRECTION,  //
            a_IS_ENABLED_PRINT);

        verify_and_correct_integer_variables_initial_values(
            this,                                   //
            a_IS_ENABLED_INITIAL_VALUE_CORRECTION,  //
            a_IS_ENABLED_PRINT);

        this->setup_fixed_sensitivities(a_IS_ENABLED_PRINT);
        this->setup_is_enabled_fast_evaluation();
    }

    /*************************************************************************/
    constexpr void setup_variable_related_constraints(void) {
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
    constexpr void setup_unique_name(void) {
        const int VARIABLE_PROXIES_SIZE   = m_variable_proxies.size();
        const int EXPRESSION_PROXIES_SIZE = m_expression_proxies.size();
        const int CONSTRAINT_PROXIES_SIZE = m_constraint_proxies.size();

        for (auto i = 0; i < VARIABLE_PROXIES_SIZE; i++) {
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
        for (auto i = 0; i < EXPRESSION_PROXIES_SIZE; i++) {
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
        for (auto i = 0; i < CONSTRAINT_PROXIES_SIZE; i++) {
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
    constexpr void setup_is_linear(void) {
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
    constexpr void setup_is_enabled_fast_evaluation(void) {
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
    constexpr void setup_variable_sensitivity(void) {
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
    constexpr void categorize_variables(void) {
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
    constexpr void categorize_constraints(void) {
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
    constexpr void extract_selections(const SelectionMode &a_SELECTION_MODE) {
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
            const int RAW_SELECTIONS_SIZE = raw_selections.size();
            for (auto i = 0; i < RAW_SELECTIONS_SIZE; i++) {
                bool has_overlap = false;
                for (auto &&variable_ptr : raw_selections[i].variable_ptrs) {
                    for (auto j = 0; j < RAW_SELECTIONS_SIZE; j++) {
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
    constexpr void setup_neighborhood(
        const bool a_IS_ENABLED_AGGREGATION_MOVE,     //
        const bool a_IS_ENABLED_PRECEDENCE_MOVE,      //
        const bool a_IS_ENABLED_VARIABLE_BOUND_MOVE,  //
        const bool a_IS_ENABLED_EXCLUSIVE_MOVE,       //
        const bool a_IS_ENABLED_CHAIN_MOVE,           //
        const bool a_IS_ENABLED_USER_DEFINED_MOVE,    //
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
    constexpr void setup_fixed_sensitivities(const bool a_IS_ENABLED_PRINT) {
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
    constexpr void print_number_of_variables(void) const {
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
    constexpr void print_number_of_constraints(void) const {
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
    constexpr void import_variable_values(
        const std::vector<ValueProxy<T_Variable>> &a_PROXIES) {
        for (auto &&proxy : m_variable_proxies) {
            for (auto &&variable : proxy.flat_indexed_variables()) {
                int proxy_index = variable.proxy_index();
                int flat_index  = variable.flat_index();
                variable.set_value_if_not_fixed(
                    a_PROXIES[proxy_index].flat_indexed_values(flat_index));
            }
        }
        verify_and_correct_selection_variables_initial_values(  //
            this, false, false);
        verify_and_correct_binary_variables_initial_values(  //
            this, false, false);
        verify_and_correct_integer_variables_initial_values(  //
            this, false, false);
    }

    /*************************************************************************/
    constexpr void update(void) {
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
    constexpr void update(const Move<T_Variable, T_Expression> &a_MOVE) {
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
    constexpr void update_variable_objective_improvability(
        const std::vector<Variable<T_Variable, T_Expression> *>
            &a_VARIABLE_PTRS) const noexcept {
        for (const auto &variable_ptr : a_VARIABLE_PTRS) {
            auto coefficient =
                variable_ptr->objective_sensitivity() * this->sign();
            if (coefficient > 0 && variable_ptr->has_lower_bound_margin()) {
                variable_ptr->set_is_objective_improvable(true);
            } else if (coefficient < 0 &&
                       variable_ptr->has_upper_bound_margin()) {
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
    constexpr void update_variable_feasibility_improvability(
        const std::vector<Constraint<T_Variable, T_Expression> *>
            &a_CONSTRAINT_PTRS) const noexcept {
        const int MASK_LOWER_OR_EQUAL = 0b10;
        const int MASK_UPPER_OR_EQUAL = 0b11;

        const int CONSTRAINTS_SIZE = a_CONSTRAINT_PTRS.size();
        for (auto i = 0; i < CONSTRAINTS_SIZE; i++) {
            const auto constraint_ptr = a_CONSTRAINT_PTRS[i];
            if (!constraint_ptr->is_enabled()) {
                continue;
            }
            if (constraint_ptr->violation_value() < constant::EPSILON) {
                continue;
            }
            const auto &sensitivities =
                constraint_ptr->expression().sensitivities();
            const auto &constraint_value = constraint_ptr->constraint_value();

            if (constraint_value > 0) {
                if ((constraint_ptr->sense() & MASK_LOWER_OR_EQUAL) == 0) {
                    for (const auto &sensitivity : sensitivities) {
                        const auto &variable_ptr = sensitivity.first;
                        const auto &coefficient  = sensitivity.second;

                        if (variable_ptr->is_feasibility_improvable()) {
                            continue;
                        }

                        if (variable_ptr->is_fixed()) {
                            continue;
                        }

                        if (coefficient > 0 &&
                            variable_ptr->has_lower_bound_margin()) {
                            variable_ptr->set_is_feasibility_improvable(true);

                        } else if (coefficient < 0 &&
                                   variable_ptr->has_upper_bound_margin()) {
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

                        if (variable_ptr->is_fixed()) {
                            continue;
                        }

                        if (coefficient > 0 &&
                            variable_ptr->has_upper_bound_margin()) {
                            variable_ptr->set_is_feasibility_improvable(true);
                        } else if (coefficient < 0 &&
                                   variable_ptr->has_lower_bound_margin()) {
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
        const Move<T_Variable, T_Expression> &a_MOVE) const noexcept {
        SolutionScore score;
        this->evaluate(&score, a_MOVE);
        return score;
    }

    /*************************************************************************/
    inline SolutionScore evaluate(const Move<T_Variable, T_Expression> &a_MOVE,
                                  const SolutionScore &a_CURRENT_SCORE) const
        noexcept {
        SolutionScore score;
        this->evaluate(&score, a_MOVE, a_CURRENT_SCORE);
        return score;
    }

    /*************************************************************************/
    constexpr void evaluate(SolutionScore *a_score_ptr,  //
                            const Move<T_Variable, T_Expression> &a_MOVE) const
        noexcept {
        double total_violation = 0.0;
        double local_penalty   = 0.0;
        double global_penalty  = 0.0;

        const int CONSTRAINT_PROXIES_SIZE   = m_constraint_proxies.size();
        bool      is_feasibility_improvable = false;

        for (auto i = 0; i < CONSTRAINT_PROXIES_SIZE; i++) {
            auto &constraints =
                m_constraint_proxies[i].flat_indexed_constraints();

            const int CONSTRAINTS_SIZE = constraints.size();
            for (auto j = 0; j < CONSTRAINTS_SIZE; j++) {
                if (!constraints[j].is_enabled()) {
                    continue;
                }
                double violation = constraints[j].evaluate_violation(a_MOVE);

                if (violation < constraints[j].violation_value()) {
                    is_feasibility_improvable = true;
                }

                total_violation += violation;
                local_penalty +=
                    violation * constraints[j].local_penalty_coefficient();
                global_penalty +=
                    violation * constraints[j].global_penalty_coefficient();
            }
        }

        double objective             = 0.0;
        double objective_improvement = 0.0;
        if (m_is_defined_objective) {
            objective = m_objective.evaluate(a_MOVE) * this->sign();
            objective_improvement =
                m_objective.value() * this->sign() - objective;
        }

        a_score_ptr->objective                  = objective;
        a_score_ptr->objective_improvement      = objective_improvement;
        a_score_ptr->total_violation            = total_violation;
        a_score_ptr->local_penalty              = local_penalty;
        a_score_ptr->global_penalty             = global_penalty;
        a_score_ptr->local_augmented_objective  = objective + local_penalty;
        a_score_ptr->global_augmented_objective = objective + global_penalty;
        a_score_ptr->is_feasible = !(total_violation > constant::EPSILON);
        a_score_ptr->is_objective_improvable =
            objective_improvement > constant::EPSILON;
        a_score_ptr->is_feasibility_improvable = is_feasibility_improvable;
    }

    /*************************************************************************/
    constexpr void evaluate(SolutionScore *a_score_ptr,  //
                            const Move<T_Variable, T_Expression> &a_MOVE,
                            const SolutionScore &a_CURRENT_SCORE) const
        noexcept {
        bool is_feasibility_improvable = false;

        double total_violation = a_CURRENT_SCORE.total_violation;
        double local_penalty   = a_CURRENT_SCORE.local_penalty;
        double global_penalty  = a_CURRENT_SCORE.global_penalty;

        for (const auto &constraint_ptr : a_MOVE.related_constraint_ptrs) {
            if (!constraint_ptr->is_enabled()) {
                continue;
            }
            double violation_diff =
                constraint_ptr->evaluate_violation_diff(a_MOVE);
            total_violation += violation_diff;

            if (violation_diff < 0) {
                is_feasibility_improvable = true;
            }

            local_penalty +=
                violation_diff * constraint_ptr->local_penalty_coefficient();
            global_penalty +=
                violation_diff * constraint_ptr->global_penalty_coefficient();
        }

        double objective             = 0.0;
        double objective_improvement = 0.0;

        if (m_is_defined_objective) {
            objective = m_objective.evaluate(a_MOVE) * this->sign();
            objective_improvement =
                m_objective.value() * this->sign() - objective;
        }

        a_score_ptr->objective                  = objective;
        a_score_ptr->objective_improvement      = objective_improvement;
        a_score_ptr->total_violation            = total_violation;
        a_score_ptr->local_penalty              = local_penalty;
        a_score_ptr->global_penalty             = global_penalty;
        a_score_ptr->local_augmented_objective  = objective + local_penalty;
        a_score_ptr->global_augmented_objective = objective + global_penalty;
        a_score_ptr->is_feasible = !(total_violation > constant::EPSILON);
        a_score_ptr->is_objective_improvable =
            objective_improvement > constant::EPSILON;
        a_score_ptr->is_feasibility_improvable = is_feasibility_improvable;
    }

    /*************************************************************************/
    constexpr double compute_lagrangian(
        const std::vector<model::ValueProxy<double>>
            &a_LAGRANGE_MULTIPLIER_PROXIES) const noexcept {
        double lagrangian = m_objective.value();

        for (auto &&constraint_ptr : m_constraint_reference.constraint_ptrs) {
            int proxy_index = constraint_ptr->proxy_index();
            int flat_index  = constraint_ptr->flat_index();

            lagrangian +=
                a_LAGRANGE_MULTIPLIER_PROXIES[proxy_index].flat_indexed_values(
                    flat_index) *
                constraint_ptr->constraint_value();
        }
        return lagrangian;
    }

    /*************************************************************************/
    template <class T_Value>
    constexpr std::vector<ValueProxy<T_Value>>
    generate_variable_parameter_proxies(const T_Value a_VALUE) const {
        std::vector<ValueProxy<T_Value>> variable_parameter_proxies;

        for (const auto &proxy : m_variable_proxies) {
            ValueProxy<T_Value> variable_parameter_proxy(proxy.index(),
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
    constexpr std::vector<ValueProxy<T_Value>>
    generate_expression_parameter_proxies(const T_Value a_VALUE) const {
        std::vector<ValueProxy<T_Value>> expression_parameter_proxies;

        for (const auto &proxy : m_expression_proxies) {
            ValueProxy<T_Value> expression_parameter_proxy(proxy.index(),
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
    constexpr std::vector<ValueProxy<T_Value>>
    generate_constraint_parameter_proxies(const T_Value a_VALUE) const {
        std::vector<ValueProxy<T_Value>> constraint_parameter_proxies;

        for (const auto &proxy : m_constraint_proxies) {
            ValueProxy<T_Value> constraint_parameter_proxy(proxy.index(),
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
    std::vector<ValueProxy<double>> export_local_penalty_coefficient_proxies(
        void) const {
        std::vector<ValueProxy<double>> local_penalty_coefficient_proxies;
        for (const auto &proxy : m_constraint_proxies) {
            ValueProxy<double> local_penalty_coefficient_proxy(proxy.index(),
                                                               proxy.shape());

            int number_of_elements = proxy.number_of_elements();

            for (auto i = 0; i < number_of_elements; i++) {
                local_penalty_coefficient_proxy.flat_indexed_names(i) =
                    proxy.flat_indexed_constraints(i).name();
                local_penalty_coefficient_proxy.flat_indexed_values(i) =
                    proxy.flat_indexed_constraints(i)
                        .local_penalty_coefficient();
            }
            local_penalty_coefficient_proxies.push_back(
                local_penalty_coefficient_proxy);
        }
        return local_penalty_coefficient_proxies;
    }

    /*************************************************************************/
    Solution<T_Variable, T_Expression> export_solution(void) const {
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
    constexpr NamedSolution<T_Variable, T_Expression> export_named_solution(
        void) const {
        return this->convert_to_named_solution(this->export_solution());
    }

    /*************************************************************************/
    NamedSolution<T_Variable, T_Expression> convert_to_named_solution(
        const Solution<T_Variable, T_Expression> &a_SOLUTION) const {
        /// This method cannot be constexpr by clang.
        NamedSolution<T_Variable, T_Expression> named_solution;

        int VARIABLE_PROXIES_SIZE   = m_variable_proxies.size();
        int EXPRESSION_PROXIES_SIZE = m_expression_proxies.size();
        int CONSTRAINT_PROXIES_SIZE = m_constraint_proxies.size();

        /// Decision variables
        for (auto i = 0; i < VARIABLE_PROXIES_SIZE; i++) {
            named_solution.m_variable_value_proxies[m_variable_names[i]] =
                a_SOLUTION.variable_value_proxies[i];
        }

        /// Expression
        for (auto i = 0; i < EXPRESSION_PROXIES_SIZE; i++) {
            named_solution.m_expression_value_proxies[m_expression_names[i]] =
                a_SOLUTION.expression_value_proxies[i];
        }

        /// Constraint
        for (auto i = 0; i < CONSTRAINT_PROXIES_SIZE; i++) {
            named_solution.m_constraint_value_proxies[m_constraint_names[i]] =
                a_SOLUTION.constraint_value_proxies[i];
        }

        /// Violation
        for (auto i = 0; i < CONSTRAINT_PROXIES_SIZE; i++) {
            named_solution.m_violation_value_proxies[m_constraint_names[i]] =
                a_SOLUTION.violation_value_proxies[i];
        }

        named_solution.m_objective       = a_SOLUTION.objective;
        named_solution.m_total_violation = a_SOLUTION.total_violation;
        named_solution.m_is_feasible     = a_SOLUTION.is_feasible;

        return named_solution;
    }

    /*************************************************************************/
    PlainSolution<T_Variable, T_Expression> export_plain_solution(void) const {
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
    PlainSolution<T_Variable, T_Expression> convert_to_plain_solution(
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
    constexpr void import_solution(
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
    ModelSummary export_summary(void) const {
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