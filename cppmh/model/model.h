/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef CPPMH_MODEL_MODEL_H__
#define CPPMH_MODEL_MODEL_H__

#include <vector>
#include <string>
#include <numeric>
#include <functional>

#include "range.h"
#include "move.h"
#include "variable_proxy.h"
#include "expression_proxy.h"
#include "constraint_proxy.h"
#include "objective.h"
#include "value_proxy.h"
#include "solution.h"
#include "named_solution.h"
#include "solution_score.h"
#include "neighborhood.h"

#include "expression_binary_operator.h"
#include "constraint_binary_operator.h"

namespace cppmh {
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
    std::vector<VariableProxy<T_Variable, T_Expression>>   m_variable_proxies;
    std::vector<ExpressionProxy<T_Variable, T_Expression>> m_expression_proxies;
    std::vector<ConstraintProxy<T_Variable, T_Expression>> m_constraint_proxies;

    Objective<T_Variable, T_Expression> m_objective;

    std::vector<std::string> m_variable_names;
    std::vector<std::string> m_expression_names;
    std::vector<std::string> m_constraint_names;

    bool                                   m_is_defined_objective;
    bool                                   m_is_enabled_fast_evaluation;
    bool                                   m_is_minimization;
    Neighborhood<T_Variable, T_Expression> m_neighborhood;
    std::function<void(void)>              m_callback;

   public:
    /*************************************************************************/
    Model(void) {
        this->initialize();
    }

    /*************************************************************************/
    virtual ~Model(void) {
        /// nothing to do
    }

    /*************************************************************************/
    inline void initialize(void) {
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
        m_is_minimization            = true;
        m_neighborhood.initialize();
        m_callback = [](void) {};
    }

    /*************************************************************************/
    inline constexpr VariableProxy<T_Variable, T_Expression> &create_variable(
        const std::string &a_NAME) {
        int id = m_variable_proxies.size();

        if (id >= ModelConstant::MAX_NUMBER_OF_VARIABLE_PROXIES) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of variable definitions must be equal to or "
                "less than " +
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
        auto &variable_proxy = create_variable(a_NAME);
        variable_proxy.set_bound(a_LOWER_BOUND, a_UPPER_BOUND);

        return m_variable_proxies.back();
    }

    /*************************************************************************/
    inline VariableProxy<T_Variable, T_Expression> &create_variables(
        const std::string &a_NAME, const int a_NUMBER_OF_ELEMENTS) {
        int id = m_variable_proxies.size();

        if (id >= ModelConstant::MAX_NUMBER_OF_VARIABLE_PROXIES) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of variable definitions must be equal to or "
                "less than " +
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
        int id = m_variable_proxies.size();

        if (id >= ModelConstant::MAX_NUMBER_OF_VARIABLE_PROXIES) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of variable definitions must be equal to or "
                "less than " +
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
    inline constexpr bool is_defined_objective(void) const {
        return m_is_defined_objective;
    }

    /*************************************************************************/
    inline constexpr bool is_enabled_fast_evaluation(void) const {
        return m_is_enabled_fast_evaluation;
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
    inline constexpr int number_of_variables(void) const {
        int result = 0;
        for (auto &&proxy : m_variable_proxies) {
            result += proxy.number_of_elements();
        }
        return result;
    }

    /*************************************************************************/
    inline constexpr int number_of_constraints(void) const {
        int result = 0;
        for (auto &&proxy : m_constraint_proxies) {
            result += proxy.number_of_elements();
        }
        return result;
    }

    /*************************************************************************/
    inline constexpr Neighborhood<T_Variable, T_Expression> &neighborhood(
        void) {
        return m_neighborhood;
    }

    /*************************************************************************/
    inline constexpr void setup(
        const bool a_IS_ENABLED_PARALLEL_NEIGHBORHOOD_UPDATE,
        const bool a_IS_ENABLED_INITIAL_VALUE_CORRECTION,
        const bool a_IS_ENABLED_PRINT, const SelectionMode &a_SELECTION_MODE) {
        this->verify_problem(a_IS_ENABLED_PRINT);

        this->setup_variable_related_constraints();
        this->setup_variable_sense();
        this->setup_unique_name();

        this->setup_is_enabled_fast_evaluation();

        this->setup_default_neighborhood(
            a_IS_ENABLED_PARALLEL_NEIGHBORHOOD_UPDATE, a_IS_ENABLED_PRINT,
            a_SELECTION_MODE);

        /**
         * If the user-defined_neighborhood is set, default neighborhood should
         * be disabled to avoid possible inconsistencies.
         */
        if (m_neighborhood.is_enabled_user_defined_move()) {
            m_neighborhood.disable_default_move();
        }

        this->verify_and_correct_selection_variables_initial_values(
            a_IS_ENABLED_INITIAL_VALUE_CORRECTION, a_IS_ENABLED_PRINT);

        this->verify_and_correct_binary_variables_initial_values(
            a_IS_ENABLED_INITIAL_VALUE_CORRECTION, a_IS_ENABLED_PRINT);

        this->verify_and_correct_integer_variables_initial_values(
            a_IS_ENABLED_INITIAL_VALUE_CORRECTION, a_IS_ENABLED_PRINT);

        this->setup_fixed_sensitivities(a_IS_ENABLED_PRINT);
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
    inline constexpr void setup_variable_sense(void) {
        /**
         * This method is for re-optimizations. After an optimization,
         * the senses of the "Binary" decision variables can be changed
         * to "Selection" by automatic detection of the neighborhood.
         * This method recovers the "Selection" decision variables into
         * "Binary".
         */

        for (auto &&proxy : m_variable_proxies) {
            for (auto &&variable : proxy.flat_indexed_variables()) {
                variable.setup_sense();
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
    inline constexpr void setup_default_neighborhood(
        const bool a_IS_ENABLED_PARALLEL, const bool a_IS_ENABLED_PRINT,
        const SelectionMode &a_SELECTION_MODE) {
        utility::print_single_line(a_IS_ENABLED_PRINT);
        utility::print_message("Detecting the neighborhood structure...",
                               a_IS_ENABLED_PRINT);
        m_neighborhood.setup_has_fixed_variables(m_variable_proxies);
        m_neighborhood.setup_default_neighborhood(
            &m_variable_proxies, &m_constraint_proxies, a_IS_ENABLED_PARALLEL,
            a_SELECTION_MODE);
        utility::print_message("Done.", a_IS_ENABLED_PRINT);
    }

    /*************************************************************************/
    inline constexpr void verify_and_correct_selection_variables_initial_values(
        const bool a_IS_ENABLED_CORRECTON, const bool a_IS_ENABLED_PRINT) {
        utility::print_single_line(a_IS_ENABLED_PRINT);
        utility::print_message(
            "Verifying the initial values of the binary decision "
            "variables "
            "included in the selection constraints...",
            a_IS_ENABLED_PRINT);

        for (auto &&selection : m_neighborhood.selections()) {
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
                            "The initial value " +
                                m_variable_names[variable_ptr->id()] +
                                m_variable_proxies[variable_ptr->id()]
                                    .indices_label(variable_ptr->flat_index()) +
                                " = " + std::to_string(old_value) +
                                " is corrected to " +
                                std::to_string(new_value) + ".",
                            a_IS_ENABLED_PRINT);
                    }

                } else {
                    throw std::logic_error(utility::format_error_location(
                        __FILE__, __LINE__, __func__,
                        "There is a variable of which initial "
                        "value "
                        "violates "
                        "binary constraint."));
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
                                "The initial value " +
                                    m_variable_names[variable_ptr->id()] +
                                    m_variable_proxies[variable_ptr->id()]
                                        .indices_label(
                                            variable_ptr->flat_index()) +
                                    " = " + std::to_string(old_value) +
                                    " is corrected to " +
                                    std::to_string(new_value) + ".",
                                a_IS_ENABLED_PRINT);
                        }
                    }

                    selected_variable_ptr->set_value_if_not_fixed(1);
                    selected_variable_ptr->select();

                } else {
                    throw std::logic_error(utility::format_error_location(
                        __FILE__, __LINE__, __func__,
                        "There are more than one selected "
                        "variables."));
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
                                "The initial value " +
                                    m_variable_names[variable_ptr->id()] +
                                    m_variable_proxies[variable_ptr->id()]
                                        .indices_label(
                                            variable_ptr->flat_index()) +
                                    " = " + std::to_string(old_value) +
                                    " is corrected to " +
                                    std::to_string(new_value) + ".",
                                a_IS_ENABLED_PRINT);
                            is_corrected = true;
                            break;
                        }
                    }
                    if (!is_corrected) {
                        throw std::logic_error(utility::format_error_location(
                            __FILE__, __LINE__, __func__,
                            "The initial value could not be "
                            "modified "
                            "because "
                            "all variables are fixed."));
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
                                    "There is an invalid fixed "
                                    "variable."));
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
                                "The initial value " +
                                    m_variable_names[variable.id()] +
                                    proxy.indices_label(variable.flat_index()) +
                                    " = " + std::to_string(old_value) +
                                    " is corrected to " +
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
                            "The initial value " +
                                m_variable_names[variable.id()] +
                                proxy.indices_label(variable.flat_index()) +
                                " = " + std::to_string(old_value) +
                                " is corrected to " +
                                std::to_string(new_value) + ".",
                            a_IS_ENABLED_PRINT);
                    } else {
                        throw std::logic_error(utility::format_error_location(
                            __FILE__, __LINE__, __func__,
                            "An initial value violates the lower or upper "
                            "bound constraint."));
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
        utility::print_message("Done.", a_IS_ENABLED_PRINT);
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

        m_objective.update();
    }

    /*************************************************************************/
    inline constexpr void update(const Move<T_Variable, T_Expression> &a_MOVE) {
        /**
         * Update in order of objective, constraints -> expressions ->
         * variables.
         */
        m_objective.update(a_MOVE);

        for (auto &&proxy : m_constraint_proxies) {
            for (auto &&constraint : proxy.flat_indexed_constraints()) {
                if (constraint.is_enabled()) {
                    constraint.update(a_MOVE);
                }
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

                if (violation < constraints[j].violation_value()) {
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

        double objective = m_objective.evaluate(a_MOVE) * this->sign();
        double objective_improvement =
            (m_objective.value() - objective) * this->sign();

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

        for (auto &&constraint_ptr : a_MOVE.related_constraint_ptrs) {
            if (!constraint_ptr->is_enabled()) {
                continue;
            }
            double violation_diff = constraint_ptr->evaluate_violation(a_MOVE) -
                                    constraint_ptr->violation_value();
            total_violation += violation_diff;

            if (violation_diff < 0) {
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

        double objective = m_objective.evaluate(a_MOVE) * this->sign();
        double objective_improvement =
            (m_objective.value() - objective) * this->sign();

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
    template <class T_Value>
    inline constexpr std::vector<ValueProxy<T_Value>>
    generate_variable_parameter_proxies(const T_Value a_VALUE) const {
        std::vector<ValueProxy<T_Value>> variable_parameter_proxies;

        for (const auto &proxy : m_variable_proxies) {
            ValueProxy<T_Value> variable_parameter_proxy(proxy.id(),
                                                         proxy.shape());
            variable_parameter_proxy.fill(a_VALUE);
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
            constraint_parameter_proxies.push_back(constraint_parameter_proxy);
        }
        return constraint_parameter_proxies;
    }

    /*************************************************************************/
    inline Solution<T_Variable, T_Expression> export_solution(void) const {
        // cannot be constexpr by clang
        Solution<T_Variable, T_Expression> solution;

        for (const auto &proxy : m_variable_proxies) {
            solution.variable_value_proxies.push_back(
                proxy.export_values_and_names());
        }

        for (const auto &proxy : m_expression_proxies) {
            solution.expression_value_proxies.push_back(
                proxy.export_values_and_names());
        }

        for (const auto &proxy : m_constraint_proxies) {
            solution.constraint_value_proxies.push_back(
                proxy.export_values_and_names());
            solution.violation_value_proxies.push_back(
                proxy.export_violations_and_names());
        }

        solution.objective = m_objective.value();
        bool is_feasible   = true;
        for (const auto &proxy : solution.violation_value_proxies) {
            if (utility::max(proxy.flat_indexed_values()) > constant::EPSILON) {
                is_feasible = false;
                break;
            }
        }
        solution.is_feasible = is_feasible;

        return solution;
    }

    /*************************************************************************/
    inline NamedSolution<T_Variable, T_Expression> convert_to_named_solution(
        const Solution<T_Variable, T_Expression> &a_SOLUTION) const {
        /// cannot be constexpr by clang
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

        named_solution.m_objective   = a_SOLUTION.objective;
        named_solution.m_is_feasible = a_SOLUTION.is_feasible;

        return named_solution;
    }

    /*************************************************************************/
    inline constexpr const std::vector<VariableProxy<T_Variable, T_Expression>>
        &variable_proxies(void) const {
        return m_variable_proxies;
    }

    /*************************************************************************/
    inline constexpr const std::vector<
        ExpressionProxy<T_Variable, T_Expression>>
        &expression_proxies(void) const {
        return m_expression_proxies;
    }

    /*************************************************************************/
    inline constexpr const std::vector<
        ConstraintProxy<T_Variable, T_Expression>>
        &constraint_proxies(void) const {
        return m_constraint_proxies;
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
};  // namespace model
using IPModel = Model<int, double>;
}  // namespace model
}  // namespace cppmh
#endif
/*****************************************************************************/
// END
/*****************************************************************************/