/*****************************************************************************/
// Copyright (c) 2020-2023 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MODEL_MODEL_H__
#define PRINTEMPS_MODEL_MODEL_H__

namespace printemps::model {
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

    std::vector<model_component::VariableProxy<T_Variable, T_Expression>>
        m_variable_proxies;
    std::vector<model_component::ExpressionProxy<T_Variable, T_Expression>>
        m_expression_proxies;
    std::vector<model_component::ConstraintProxy<T_Variable, T_Expression>>
        m_constraint_proxies;

    model_component::Objective<T_Variable, T_Expression> m_objective;

    std::unordered_map<model_component::Variable<T_Variable, T_Expression> *,
                       model_component::Expression<T_Variable, T_Expression> *>
        m_dependent_expression_map;

    std::vector<std::string> m_variable_names;
    std::vector<std::string> m_expression_names;
    std::vector<std::string> m_constraint_names;

    bool m_is_defined_objective;
    bool m_is_enabled_fast_evaluation;
    bool m_is_integer;
    bool m_is_linear;
    bool m_is_minimization;
    bool m_is_solved;
    bool m_is_feasible;

    double m_global_penalty_coefficient;

    std::vector<model_component::Selection<T_Variable, T_Expression>>
        m_selections;

    model_component::VariableReference<T_Variable, T_Expression>  //
        m_variable_reference_original;
    model_component::VariableTypeReference<T_Variable, T_Expression>  //
        m_variable_type_reference_original;
    model_component::ConstraintReference<T_Variable, T_Expression>  //
        m_constraint_reference_original;
    model_component::ConstraintTypeReference<T_Variable, T_Expression>  //
        m_constraint_type_reference_original;

    model_component::VariableReference<T_Variable, T_Expression>  //
        m_variable_reference;
    model_component::VariableTypeReference<T_Variable, T_Expression>  //
        m_variable_type_reference;
    model_component::ConstraintReference<T_Variable, T_Expression>  //
        m_constraint_reference;
    model_component::ConstraintTypeReference<T_Variable, T_Expression>  //
        m_constraint_type_reference;

    std::vector<model_component::Constraint<T_Variable, T_Expression> *>
        m_violative_constraint_ptrs;

    std::vector<
        std::pair<model_component::Variable<T_Variable, T_Expression> *,
                  model_component::Variable<T_Variable, T_Expression> *>>
        m_flippable_variable_ptr_pairs;

    neighborhood::Neighborhood<T_Variable, T_Expression> m_neighborhood;
    preprocess::ProblemSizeReducer<T_Variable, T_Expression>
        m_problem_size_reducer;

    std::function<void(option::Option *,
                       solution::IncumbentHolder<T_Variable, T_Expression> *)>
        m_callback;

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
        m_dependent_expression_map.clear();

        m_variable_names.clear();
        m_expression_names.clear();
        m_constraint_names.clear();

        m_is_defined_objective       = false;
        m_is_enabled_fast_evaluation = true;
        m_is_integer                 = false;
        m_is_linear                  = true;
        m_is_minimization            = true;
        m_is_solved                  = false;
        m_is_feasible                = false;

        m_global_penalty_coefficient = 0.0;

        m_selections.clear();
        m_variable_reference_original.initialize();
        m_variable_type_reference_original.initialize();
        m_constraint_reference_original.initialize();
        m_constraint_type_reference_original.initialize();

        m_variable_reference.initialize();
        m_variable_type_reference.initialize();
        m_constraint_reference.initialize();
        m_constraint_type_reference.initialize();

        m_violative_constraint_ptrs.clear();
        m_flippable_variable_ptr_pairs.clear();

        m_neighborhood.initialize();
        m_problem_size_reducer.initialize();
        m_callback = [](option::Option *,
                        solution::IncumbentHolder<T_Variable, T_Expression> *) {
        };
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
    constexpr model_component::VariableProxy<T_Variable, T_Expression>
        &create_variable(const std::string &a_NAME) {
        if (utility::has_space(a_NAME)) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The name of variable must not contain spaces."));
        }

        const int PROXY_INDEX = m_variable_proxies.size();

        if (PROXY_INDEX >= ModelConstant::MAX_NUMBER_OF_VARIABLE_PROXIES) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of variable definitions must be equal to or less "
                "than " +
                    std::to_string(
                        ModelConstant::MAX_NUMBER_OF_VARIABLE_PROXIES) +
                    "."));
        }

        m_variable_proxies.emplace_back(
            model_component::VariableProxy<
                T_Variable, T_Expression>::create_instance(PROXY_INDEX));
        m_variable_names.push_back(a_NAME);

        return m_variable_proxies.back();
    }

    /*************************************************************************/
    constexpr model_component::VariableProxy<T_Variable, T_Expression>
        &create_variable(const std::string &a_NAME,         //
                         const T_Variable   a_LOWER_BOUND,  //
                         const T_Variable   a_UPPER_BOUND) {
        auto &variable_proxy = this->create_variable(a_NAME);
        variable_proxy.set_bound(a_LOWER_BOUND, a_UPPER_BOUND);

        return m_variable_proxies.back();
    }

    /*************************************************************************/
    constexpr model_component::VariableProxy<T_Variable, T_Expression>
        &create_variables(const std::string &a_NAME,  //
                          const int          a_NUMBER_OF_ELEMENTS) {
        if (utility::has_space(a_NAME)) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The name of variable must not contain spaces."));
        }

        const int PROXY_INDEX = m_variable_proxies.size();

        if (PROXY_INDEX >= ModelConstant::MAX_NUMBER_OF_VARIABLE_PROXIES) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of variable definitions must be equal to or less "
                "than " +
                    std::to_string(
                        ModelConstant::MAX_NUMBER_OF_VARIABLE_PROXIES) +
                    "."));
        }

        m_variable_proxies.emplace_back(
            model_component::VariableProxy<T_Variable, T_Expression>::
                create_instance(PROXY_INDEX, a_NUMBER_OF_ELEMENTS));
        m_variable_names.push_back(a_NAME);

        return m_variable_proxies.back();
    }

    /*************************************************************************/
    constexpr model_component::VariableProxy<T_Variable, T_Expression>
        &create_variables(const std::string &a_NAME,                //
                          const int          a_NUMBER_OF_ELEMENTS,  //
                          const T_Variable   a_LOWER_BOUND,         //
                          const T_Variable   a_UPPER_BOUND) {
        auto &variable_proxy = create_variables(a_NAME, a_NUMBER_OF_ELEMENTS);
        variable_proxy.set_bound(a_LOWER_BOUND, a_UPPER_BOUND);

        return m_variable_proxies.back();
    }

    /*************************************************************************/
    constexpr model_component::VariableProxy<T_Variable, T_Expression>
        &create_variables(const std::string &     a_NAME,  //
                          const std::vector<int> &a_SHAPE) {
        if (utility::has_space(a_NAME)) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The name of variable must not contain spaces."));
        }

        const int PROXY_INDEX = m_variable_proxies.size();

        if (PROXY_INDEX >= ModelConstant::MAX_NUMBER_OF_VARIABLE_PROXIES) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of variable definitions must be equal to or less "
                "than " +
                    std::to_string(
                        ModelConstant::MAX_NUMBER_OF_VARIABLE_PROXIES) +
                    "."));
        }

        m_variable_proxies.emplace_back(
            model_component::VariableProxy<
                T_Variable, T_Expression>::create_instance(PROXY_INDEX,
                                                           a_SHAPE));
        m_variable_names.push_back(a_NAME);

        return m_variable_proxies.back();
    }

    /*************************************************************************/
    constexpr model_component::VariableProxy<T_Variable, T_Expression>
        &create_variables(const std::string &     a_NAME,         //
                          const std::vector<int> &a_SHAPE,        //
                          const T_Variable        a_LOWER_BOUND,  //
                          const T_Variable        a_UPPER_BOUND) {
        auto &variable_proxy = create_variables(a_NAME, a_SHAPE);
        variable_proxy.set_bound(a_LOWER_BOUND, a_UPPER_BOUND);

        return m_variable_proxies.back();
    }

    /*************************************************************************/
    inline constexpr model_component::ExpressionProxy<T_Variable, T_Expression>
        &create_expression(const std::string &a_NAME) {
        if (utility::has_space(a_NAME)) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The name of expression must not contain spaces."));
        }

        const int PROXY_INDEX = m_expression_proxies.size();

        if (PROXY_INDEX >= ModelConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of expression definitions must be equal to or "
                "less than " +
                    std::to_string(
                        ModelConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES) +
                    "."));
        }

        m_expression_proxies.emplace_back(
            model_component::ExpressionProxy<
                T_Variable, T_Expression>::create_instance(PROXY_INDEX));
        m_expression_names.push_back(a_NAME);

        return m_expression_proxies.back();
    }

    /*************************************************************************/
    constexpr model_component::ExpressionProxy<T_Variable, T_Expression>
        &create_expressions(const std::string &a_NAME,  //
                            int                a_NUMBER_OF_ELEMENTS) {
        if (utility::has_space(a_NAME)) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The name of expression must not contain spaces."));
        }

        const int PROXY_INDEX = m_expression_proxies.size();

        if (PROXY_INDEX >= ModelConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of expression definitions must be equal to or "
                "less than " +
                    std::to_string(
                        ModelConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES) +
                    "."));
        }

        m_expression_proxies.emplace_back(
            model_component::ExpressionProxy<T_Variable, T_Expression>::
                create_instance(PROXY_INDEX, a_NUMBER_OF_ELEMENTS));
        m_expression_names.push_back(a_NAME);

        return m_expression_proxies.back();
    }

    /*************************************************************************/
    constexpr model_component::ExpressionProxy<T_Variable, T_Expression>
        &create_expressions(const std::string &     a_NAME,  //
                            const std::vector<int> &a_SHAPE) {
        if (utility::has_space(a_NAME)) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The name of expression must not contain spaces."));
        }

        const int PROXY_INDEX = m_expression_proxies.size();

        if (PROXY_INDEX >= ModelConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of expression definitions must be equal to or "
                "less than " +
                    std::to_string(
                        ModelConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES) +
                    "."));
        }

        m_expression_proxies.emplace_back(
            model_component::ExpressionProxy<
                T_Variable, T_Expression>::create_instance(PROXY_INDEX,
                                                           a_SHAPE));
        m_expression_names.push_back(a_NAME);

        return m_expression_proxies.back();
    }

    /*************************************************************************/
    template <template <class, class> class T_ExpressionLike>
    constexpr model_component::ExpressionProxy<T_Variable, T_Expression> &
    create_expression(
        const std::string &                               a_NAME,  //
        const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE) {
        if (utility::has_space(a_NAME)) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The name of expression must not contain spaces."));
        }

        const int PROXY_INDEX = m_expression_proxies.size();

        if (PROXY_INDEX >= ModelConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of expression definitions must be equal to or "
                "less than " +
                    std::to_string(
                        ModelConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES) +
                    "."));
        }

        m_expression_proxies.emplace_back(
            model_component::ExpressionProxy<
                T_Variable, T_Expression>::create_instance(PROXY_INDEX));
        m_expression_names.push_back(a_NAME);
        m_expression_proxies.back() = a_EXPRESSION_LIKE.to_expression();

        return m_expression_proxies.back();
    }

    /*************************************************************************/
    constexpr model_component::ExpressionProxy<T_Variable, T_Expression>
        &create_expression(
            const std::string &a_NAME,  //
            const model_component::Expression<T_Variable, T_Expression>
                &a_EXPRESSION) {
        if (utility::has_space(a_NAME)) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The name of expression must not contain spaces."));
        }

        const int PROXY_INDEX = m_expression_proxies.size();

        if (PROXY_INDEX >= ModelConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of expression definitions must be equal to or "
                "less than " +
                    std::to_string(
                        ModelConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES) +
                    "."));
        }

        m_expression_proxies.emplace_back(
            model_component::ExpressionProxy<
                T_Variable, T_Expression>::create_instance(PROXY_INDEX));
        m_expression_names.push_back(a_NAME);
        m_expression_proxies.back() = a_EXPRESSION;

        return m_expression_proxies.back();
    }

    /*************************************************************************/
    constexpr model_component::ConstraintProxy<T_Variable, T_Expression>
        &create_constraint(const std::string &a_NAME) {
        if (utility::has_space(a_NAME)) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The name of constraint must not contain spaces."));
        }

        const int PROXY_INDEX = m_constraint_proxies.size();

        if (PROXY_INDEX >= ModelConstant::MAX_NUMBER_OF_CONSTRAINT_PROXIES) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of constraint definitions must be equal to or "
                "less than " +
                    std::to_string(
                        ModelConstant::MAX_NUMBER_OF_CONSTRAINT_PROXIES) +
                    "."));
        }

        m_constraint_proxies.emplace_back(
            model_component::ConstraintProxy<
                T_Variable, T_Expression>::create_instance(PROXY_INDEX));
        m_constraint_names.push_back(a_NAME);

        return m_constraint_proxies.back();
    }

    /*************************************************************************/
    constexpr model_component::ConstraintProxy<T_Variable, T_Expression>
        &create_constraints(const std::string &a_NAME,  //
                            int                a_NUMBER_OF_ELEMENTS) {
        if (utility::has_space(a_NAME)) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The name of constraint must not contain spaces."));
        }

        const int PROXY_INDEX = m_constraint_proxies.size();

        if (PROXY_INDEX >= ModelConstant::MAX_NUMBER_OF_CONSTRAINT_PROXIES) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of constraint definitions must be equal to or "
                "less than " +
                    std::to_string(
                        ModelConstant::MAX_NUMBER_OF_CONSTRAINT_PROXIES) +
                    "."));
        }

        m_constraint_proxies.emplace_back(
            model_component::ConstraintProxy<T_Variable, T_Expression>::
                create_instance(PROXY_INDEX, a_NUMBER_OF_ELEMENTS));
        m_constraint_names.push_back(a_NAME);

        return m_constraint_proxies.back();
    }

    /*************************************************************************/
    constexpr model_component::ConstraintProxy<T_Variable, T_Expression>
        &create_constraints(const std::string &     a_NAME,  //
                            const std::vector<int> &a_SHAPE) {
        if (utility::has_space(a_NAME)) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The name of constraint must not contain spaces."));
        }

        const int PROXY_INDEX = m_constraint_proxies.size();

        if (PROXY_INDEX >= ModelConstant::MAX_NUMBER_OF_CONSTRAINT_PROXIES) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of constraint definitions must be equal to or "
                "less than " +
                    std::to_string(
                        ModelConstant::MAX_NUMBER_OF_CONSTRAINT_PROXIES) +
                    "."));
        }

        m_constraint_proxies.emplace_back(
            model_component::ConstraintProxy<
                T_Variable, T_Expression>::create_instance(PROXY_INDEX,
                                                           a_SHAPE));
        m_constraint_names.push_back(a_NAME);

        return m_constraint_proxies.back();
    }

    /*************************************************************************/
    constexpr model_component::ConstraintProxy<T_Variable, T_Expression>
        &create_constraint(
            const std::string &a_NAME,  //
            const model_component::Constraint<T_Variable, T_Expression>
                &a_CONSTRAINT) {
        if (utility::has_space(a_NAME)) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The name of constraint must not contain spaces."));
        }

        const int PROXY_INDEX = m_constraint_proxies.size();

        if (PROXY_INDEX >= ModelConstant::MAX_NUMBER_OF_CONSTRAINT_PROXIES) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of constraint definitions must be equal to or "
                "less than " +
                    std::to_string(
                        ModelConstant::MAX_NUMBER_OF_CONSTRAINT_PROXIES) +
                    "."));
        }

        m_constraint_proxies.emplace_back(
            model_component::ConstraintProxy<
                T_Variable, T_Expression>::create_instance(PROXY_INDEX));
        m_constraint_names.push_back(a_NAME);
        m_constraint_proxies.back() = a_CONSTRAINT;

        return m_constraint_proxies.back();
    }

    /*************************************************************************/
    inline constexpr void minimize(
        const std::function<
            T_Expression(const neighborhood::Move<T_Variable, T_Expression> &)>
            &a_FUNCTION) {
        auto objective = model_component::Objective<
            T_Variable, T_Expression>::create_instance(a_FUNCTION);
        m_objective            = objective;
        m_is_defined_objective = true;
        m_is_minimization      = true;
    }

    /*************************************************************************/
    template <template <class, class> class T_ExpressionLike>
    inline constexpr void minimize(
        const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE) {
        auto objective = model_component::Objective<T_Variable, T_Expression>::
            create_instance(a_EXPRESSION_LIKE.to_expression());

        m_objective            = objective;
        m_is_defined_objective = true;
        m_is_minimization      = true;
    }

    /*************************************************************************/
    inline constexpr void minimize(
        const model_component::Expression<T_Variable, T_Expression>
            &a_EXPRESSION) {
        auto objective = model_component::Objective<
            T_Variable, T_Expression>::create_instance(a_EXPRESSION);
        m_objective            = objective;
        m_is_defined_objective = true;
        m_is_minimization      = true;
    }

    /*************************************************************************/
    inline constexpr void maximize(
        const std::function<
            T_Expression(const neighborhood::Move<T_Variable, T_Expression> &)>
            &a_FUNCTION) {
        auto objective = model_component::Objective<
            T_Variable, T_Expression>::create_instance(a_FUNCTION);
        m_objective            = objective;
        m_is_defined_objective = true;
        m_is_minimization      = false;
    }

    /*************************************************************************/
    template <template <class, class> class T_ExpressionLike>
    inline constexpr void maximize(
        const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE) {
        auto objective = model_component::Objective<T_Variable, T_Expression>::
            create_instance(a_EXPRESSION_LIKE.to_expression());
        m_objective            = objective;
        m_is_defined_objective = true;
        m_is_minimization      = false;
    }

    /*************************************************************************/
    inline constexpr void maximize(
        const model_component::Expression<T_Variable, T_Expression>
            &a_EXPRESSION) {
        auto objective = model_component::Objective<
            T_Variable, T_Expression>::create_instance(a_EXPRESSION);
        m_objective            = objective;
        m_is_defined_objective = true;
        m_is_minimization      = false;
    }

    /*************************************************************************/
    constexpr void setup(const option::Option &a_OPTION,
                         const bool            a_IS_ENABLED_PRINT) {
        /**
         * Verify the problem.
         */
        preprocess::Verifier<T_Variable, T_Expression> verifier(this);
        verifier.verify_problem(a_IS_ENABLED_PRINT);

        /**
         * Determine unique name of variables and constraints.
         */
        this->setup_unique_names();

        /**
         * Determine the linearity.
         */
        this->setup_is_linear();

        /**
         * Determine if the fast evaluation can be enabled.
         */
        this->setup_is_enabled_fast_evaluation();

        /**
         * Initial structure analysis.
         */
        this->setup_structure();

        /**
         * Store original categorization results. The final categorization would
         * be changed by presolving, extracting/eliminating intermediate
         * variables, and extracting selection constraints.
         */
        m_variable_reference_original        = m_variable_reference;
        m_variable_type_reference_original   = m_variable_type_reference;
        m_constraint_reference_original      = m_constraint_reference;
        m_constraint_type_reference_original = m_constraint_type_reference;

        /**
         * Presolve the problem by removing redundant constraints and fixing
         * variables implicitly fixed.
         */
        if (a_OPTION.preprocess.is_enabled_presolve) {
            m_problem_size_reducer.setup(this);

            if (a_OPTION.preprocess
                    .is_enabled_extract_implicit_equality_constraints) {
                m_problem_size_reducer.extract_implicit_equality_constraints(
                    a_IS_ENABLED_PRINT);
            }

            if (a_OPTION.preprocess
                    .is_enabled_remove_redundant_set_constraints) {
                m_problem_size_reducer.remove_redundant_set_constraints(
                    a_IS_ENABLED_PRINT);
            }

            m_problem_size_reducer.reduce_problem_size(a_IS_ENABLED_PRINT);
        }

        /**
         * Extract and eliminate the intermediate variables.
         */
        this->setup_structure();

        if (a_OPTION.preprocess.is_enabled_presolve && m_is_linear &&
            a_OPTION.preprocess.is_enabled_extract_dependent()) {
            preprocess::DependentVariableExtractor<T_Variable, T_Expression>
                dependent_variable_extractor(this);
            while (true) {
                if (dependent_variable_extractor.extract(
                        a_OPTION, a_IS_ENABLED_PRINT) == 0) {
                    break;
                }

                while (true) {
                    this->setup_structure();
                    if (dependent_variable_extractor.eliminate(
                            a_IS_ENABLED_PRINT) == 0) {
                        break;
                    }
                }

                m_problem_size_reducer.reduce_problem_size(a_IS_ENABLED_PRINT);
                this->setup_structure();
            }
        }

        /**
         * Remove redundant set variables.
         */
        int number_of_fixed_variables = 0;
        if (a_OPTION.preprocess.is_enabled_presolve && m_is_linear &&
            a_OPTION.preprocess.is_enabled_remove_redundant_set_variables) {
            number_of_fixed_variables =
                m_problem_size_reducer.remove_redundant_set_variables(
                    a_IS_ENABLED_PRINT);
        }

        /**
         * Remove duplicated constraints.
         */
        int number_of_removed_constraints = 0;
        if (a_OPTION.preprocess.is_enabled_presolve &&
            a_OPTION.preprocess.is_enabled_remove_duplicated_constraints) {
            number_of_removed_constraints =
                m_problem_size_reducer.remove_duplicated_constraints(
                    a_IS_ENABLED_PRINT);
        }

        /**
         * Perform setup_structure again if there are new removed(disabled)
         * variables or constraints.
         */
        if (number_of_fixed_variables > 0 ||
            number_of_removed_constraints > 0) {
            this->setup_structure();
        }

        /**
         * Extract selection constraints. If the number of constraints is bigger
         * than that of variables, this process will be skipped because it would
         * affect computational efficiency.
         */
        if (a_OPTION.neighborhood.selection_mode !=
                option::selection_mode::Off &&
            this->number_of_variables() > this->number_of_constraints()) {
            preprocess::SelectionExtractor<T_Variable, T_Expression>
                selection_extractor(this);
            selection_extractor.extract(a_OPTION.neighborhood.selection_mode,
                                        a_IS_ENABLED_PRINT);
        }

        /**
         * Final structure analysis.
         */
        this->setup_structure();

        /**
         * Setup the neighborhood generators.
         */
        this->setup_neighborhood(a_OPTION, a_IS_ENABLED_PRINT);

        /**
         * Verify and correct the initial values.
         */
        verifier.verify_and_correct_selection_variables_initial_values(  //
            a_OPTION.preprocess.is_enabled_initial_value_correction,
            a_IS_ENABLED_PRINT);

        verifier.verify_and_correct_binary_variables_initial_values(
            a_OPTION.preprocess.is_enabled_initial_value_correction,
            a_IS_ENABLED_PRINT);

        verifier.verify_and_correct_integer_variables_initial_values(
            a_OPTION.preprocess.is_enabled_initial_value_correction,
            a_IS_ENABLED_PRINT);

        /**
         * Solve GF(2) equations if needed.
         */
        if (a_OPTION.preprocess.is_enabled_presolve &&
            m_constraint_type_reference.gf2_ptrs.size() > 0) {
            preprocess::GF2Solver<T_Variable, T_Expression> gf2_solver(this);
            const auto IS_SOLVED = gf2_solver.solve(a_IS_ENABLED_PRINT);

            /**
             * Update fixed variables.
             */
            if (IS_SOLVED) {
                this->categorize_variables();
            }
        }

        /**
         * Set up the fixed sensitivities for fast evaluation.
         */
        this->setup_fixed_sensitivities(a_IS_ENABLED_PRINT);

        /**
         * Set up the constraint sensitivities of variables.
         */
        this->setup_variable_constraint_sensitivities();

        /**
         * Set up the indices of related constraints for selection variables.
         */
        this->setup_variable_related_selection_constraint_ptr_index();

        /**
         * Set up the integrity of constraints.
         */
        this->setup_is_integer();

        /**
         * Store the global penalty coefficient for evaluation.
         */
        this->set_global_penalty_coefficient(
            a_OPTION.penalty.initial_penalty_coefficient);

        /**
         * Modify the global penalty coefficient.
         */
        if (m_is_integer && m_is_linear &&
            a_OPTION.penalty.is_enabled_shrink_penalty_coefficient) {
            this->shrink_global_penalty_coefficient(a_IS_ENABLED_PRINT);
        }

        for (auto &&proxy : m_constraint_proxies) {
            for (auto &&constraint : proxy.flat_indexed_constraints()) {
                constraint.global_penalty_coefficient() =
                    m_global_penalty_coefficient;
            }
        }

        m_problem_size_reducer.set_is_preprocess(false);
    }

    /*************************************************************************/
    constexpr void setup_structure(void) {
        this->categorize_variables();
        this->categorize_constraints();

        this->setup_variable_related_constraints();
        this->setup_variable_related_binary_coefficient_constraints();
        this->setup_variable_objective_sensitivities();
        this->setup_variable_constraint_sensitivities();
    }

    /*************************************************************************/
    constexpr void setup_unique_names(void) {
        const int VARIABLE_PROXIES_SIZE   = m_variable_proxies.size();
        const int EXPRESSION_PROXIES_SIZE = m_expression_proxies.size();
        const int CONSTRAINT_PROXIES_SIZE = m_constraint_proxies.size();

        for (auto i = 0; i < VARIABLE_PROXIES_SIZE; i++) {
            const int NUMBER_OF_ELEMENTS =
                m_variable_proxies[i].number_of_elements();
            for (auto j = 0; j < NUMBER_OF_ELEMENTS; j++) {
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
            const int NUMBER_OF_ELEMENTS =
                m_expression_proxies[i].number_of_elements();
            for (auto j = 0; j < NUMBER_OF_ELEMENTS; j++) {
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
            const int NUMBER_OF_ELEMENTS =
                m_constraint_proxies[i].number_of_elements();
            for (auto j = 0; j < NUMBER_OF_ELEMENTS; j++) {
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
        /**
         * NOTE: In this method, m_constraint_reference is not referred because
         * the object may not have been set up at the stage this method is
         * called.
         */
        m_is_enabled_fast_evaluation = true;
        for (auto &&proxy : m_constraint_proxies) {
            for (auto &&constraint : proxy.flat_indexed_constraints()) {
                if (!constraint.is_linear()) {
                    m_is_enabled_fast_evaluation = false;
                }
            }
        }

        if (m_neighborhood.user_defined().is_enabled()) {
            m_is_enabled_fast_evaluation = false;
        }
    }

    /*************************************************************************/
    constexpr void setup_is_integer(void) {
        /**
         * NOTE: In this method, m_constraint_reference is not referred because
         * the object may not have been set up at the stage this method is
         * called.
         */
        m_is_integer = true;
        for (auto &&proxy : m_constraint_proxies) {
            for (auto &&constraint : proxy.flat_indexed_constraints()) {
                if (!constraint.is_integer()) {
                    m_is_integer = false;
                    return;
                }
            }
        }
    }

    /*************************************************************************/
    constexpr void setup_variable_related_constraints(void) {
        /**
         * NOTE: In this method, m_variable_reference is not referred because
         * the object would not be set up at the stage when this method is
         * called.
         */
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

        for (auto &&proxy : m_variable_proxies) {
            for (auto &&variable : proxy.flat_indexed_variables()) {
                variable.sort_and_unique_related_constraint_ptrs();
            }
        }
    }

    /*************************************************************************/
    constexpr void setup_variable_objective_sensitivities(void) {
        for (auto &&sensitivity : m_objective.expression().sensitivities()) {
            sensitivity.first->set_objective_sensitivity(sensitivity.second);
        }
    }

    /*************************************************************************/
    constexpr void setup_variable_constraint_sensitivities(void) {
        /**
         * NOTE: In this method, m_variable_reference is not referred because
         * the object may not have been set up at the stage this method is
         * called.
         */
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
        for (auto &&proxy : m_variable_proxies) {
            for (auto &&variable : proxy.flat_indexed_variables()) {
                variable.sort_constraint_sensitivities();
            }
        }
    }

    /*************************************************************************/
    constexpr void setup_variable_related_selection_constraint_ptr_index(void) {
        for (auto &&variable_ptr :
             m_variable_type_reference.selection_variable_ptrs) {
            variable_ptr->setup_related_selection_constraint_ptr_index();
        }
    }

    /*************************************************************************/
    constexpr void setup_variable_related_binary_coefficient_constraints(void) {
        /**
         * NOTE: In this method, m_variable_reference is not referred because
         * the object may not have been set up at the stage this method is
         * called.
         */
        for (auto &&proxy : m_variable_proxies) {
            for (auto &&variable : proxy.flat_indexed_variables()) {
                variable.reset_related_binary_coefficient_constraint_ptrs();
                variable.setup_related_binary_coefficient_constraint_ptrs();
            }
        }
    }

    /*************************************************************************/
    constexpr void categorize_variables(void) {
        model_component::VariableReference<T_Variable, T_Expression>
            variable_reference;
        model_component::VariableTypeReference<T_Variable, T_Expression>
            variable_type_reference;

        for (auto &&proxy : m_variable_proxies) {
            for (auto &&variable : proxy.flat_indexed_variables()) {
                variable_reference.variable_ptrs.push_back(&variable);
                if (variable.is_fixed()) {
                    variable_reference.fixed_variable_ptrs.push_back(&variable);
                } else {
                    variable_reference.mutable_variable_ptrs.push_back(
                        &variable);
                }
                if (variable.sense() ==
                    model_component::VariableSense::Binary) {
                    variable_type_reference.binary_variable_ptrs.push_back(
                        &variable);
                }
                if (variable.sense() ==
                    model_component::VariableSense::Integer) {
                    variable_type_reference.integer_variable_ptrs.push_back(
                        &variable);
                }
                if (variable.sense() ==
                    model_component::VariableSense::Selection) {
                    variable_type_reference.selection_variable_ptrs.push_back(
                        &variable);
                }

                if (variable.sense() ==
                    model_component::VariableSense::DependentBinary) {
                    variable_type_reference.dependent_binary_variable_ptrs
                        .push_back(&variable);
                }

                if (variable.sense() ==
                    model_component::VariableSense::DependentInteger) {
                    variable_type_reference.dependent_integer_variable_ptrs
                        .push_back(&variable);
                }
            }
        }
        m_variable_reference      = variable_reference;
        m_variable_type_reference = variable_type_reference;
    }

    /*************************************************************************/
    constexpr void categorize_constraints(void) {
        model_component::ConstraintReference<T_Variable, T_Expression>
            constraint_reference;
        model_component::ConstraintTypeReference<T_Variable, T_Expression>
            constraint_type_reference;

        for (auto &&proxy : m_constraint_proxies) {
            for (auto &&constraint : proxy.flat_indexed_constraints()) {
                constraint.setup_constraint_type();
            }
        }

        for (auto &&proxy : m_constraint_proxies) {
            for (auto &&constraint : proxy.flat_indexed_constraints()) {
                constraint_reference.constraint_ptrs.push_back(&constraint);
                if (constraint.is_enabled()) {
                    constraint_reference.enabled_constraint_ptrs.push_back(
                        &constraint);

                    switch (constraint.sense()) {
                        case model_component::ConstraintSense::Less: {
                            constraint_reference.less_ptrs.push_back(
                                &constraint);
                            break;
                        }
                        case model_component::ConstraintSense::Equal: {
                            constraint_reference.equal_ptrs.push_back(
                                &constraint);
                            break;
                        }
                        case model_component::ConstraintSense::Greater: {
                            constraint_reference.greater_ptrs.push_back(
                                &constraint);
                            break;
                        }
                        default: {
                            /** nothing to do*/
                        }
                    }
                } else {
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
                    if (constraint.is_exclusive_or()) {
                        constraint_type_reference.exclusive_or_ptrs.push_back(
                            &constraint);
                    }
                    if (constraint.is_exclusive_nor()) {
                        constraint_type_reference.exclusive_nor_ptrs.push_back(
                            &constraint);
                    }
                    if (constraint.is_inverted_integers()) {
                        constraint_type_reference.inverted_integers_ptrs
                            .push_back(&constraint);
                    }
                    if (constraint.is_balanced_integers()) {
                        constraint_type_reference.balanced_integers_ptrs
                            .push_back(&constraint);
                    }
                    if (constraint.is_constant_sum_integers()) {
                        constraint_type_reference.constant_sum_integers_ptrs
                            .push_back(&constraint);
                    }
                    if (constraint.is_constant_difference_integers()) {
                        constraint_type_reference
                            .constant_difference_integers_ptrs.push_back(
                                &constraint);
                    }
                    if (constraint.is_constant_ratio_integers()) {
                        constraint_type_reference.constant_ratio_integers_ptrs
                            .push_back(&constraint);
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
                    if (constraint.is_trinomial_exclusive_nor()) {
                        constraint_type_reference.trinomial_exclusive_nor_ptrs
                            .push_back(&constraint);
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
                    if (constraint.is_multiple_covering()) {
                        constraint_type_reference.multiple_covering_ptrs
                            .push_back(&constraint);
                    }
                    if (constraint.is_binary_flow()) {
                        constraint_type_reference.binary_flow_ptrs.push_back(
                            &constraint);
                    }
                    if (constraint.is_integer_flow()) {
                        constraint_type_reference.integer_flow_ptrs.push_back(
                            &constraint);
                    }
                    if (constraint.is_soft_selection()) {
                        constraint_type_reference.soft_selection_ptrs.push_back(
                            &constraint);
                    }
                    if (constraint.is_min_max()) {
                        constraint_type_reference.min_max_ptrs.push_back(
                            &constraint);
                    }
                    if (constraint.is_max_min()) {
                        constraint_type_reference.max_min_ptrs.push_back(
                            &constraint);
                    }
                    if (constraint.is_intermediate()) {
                        constraint_type_reference.intermediate_ptrs.push_back(
                            &constraint);
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
                    if (constraint.is_gf2()) {
                        constraint_type_reference.gf2_ptrs.push_back(
                            &constraint);
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
    constexpr void setup_neighborhood(const option::Option &a_OPTION,
                                      const bool a_IS_ENABLED_PRINT) {
        utility::print_single_line(a_IS_ENABLED_PRINT);
        utility::print_message("Detecting the neighborhood structure...",
                               a_IS_ENABLED_PRINT);

        m_neighborhood.binary().setup(
            m_variable_type_reference.binary_variable_ptrs);

        m_neighborhood.integer().setup(
            m_variable_type_reference.integer_variable_ptrs);

        m_neighborhood.selection().setup(
            m_variable_type_reference.selection_variable_ptrs);

        if (a_OPTION.neighborhood.is_enabled_exclusive_or_move) {
            m_neighborhood.exclusive_or().setup(
                m_constraint_type_reference.exclusive_or_ptrs);
        }

        if (a_OPTION.neighborhood.is_enabled_exclusive_nor_move) {
            m_neighborhood.exclusive_nor().setup(
                m_constraint_type_reference.exclusive_nor_ptrs);
        }

        if (a_OPTION.neighborhood.is_enabled_inverted_integers_move) {
            m_neighborhood.inverted_integers().setup(
                m_constraint_type_reference.inverted_integers_ptrs);
        }

        if (a_OPTION.neighborhood.is_enabled_balanced_integers_move) {
            m_neighborhood.balanced_integers().setup(
                m_constraint_type_reference.balanced_integers_ptrs);
        }

        if (a_OPTION.neighborhood.is_enabled_constant_sum_integers_move) {
            m_neighborhood.constant_sum_integers().setup(
                m_constraint_type_reference.constant_sum_integers_ptrs);
        }

        if (a_OPTION.neighborhood
                .is_enabled_constant_difference_integers_move) {
            m_neighborhood.constant_difference_integers().setup(
                m_constraint_type_reference.constant_difference_integers_ptrs);
        }

        if (a_OPTION.neighborhood.is_enabled_constant_ratio_integers_move) {
            m_neighborhood.constant_ratio_integers().setup(
                m_constraint_type_reference.constant_ratio_integers_ptrs);
        }

        if (a_OPTION.neighborhood.is_enabled_aggregation_move) {
            m_neighborhood.aggregation().setup(
                m_constraint_type_reference.aggregation_ptrs);
        }

        if (a_OPTION.neighborhood.is_enabled_precedence_move) {
            m_neighborhood.precedence().setup(
                m_constraint_type_reference.precedence_ptrs);
        }

        if (a_OPTION.neighborhood.is_enabled_variable_bound_move) {
            m_neighborhood.variable_bound().setup(
                m_constraint_type_reference.variable_bound_ptrs);
        }

        if (a_OPTION.neighborhood.is_enabled_trinomial_exclusive_nor_move) {
            m_neighborhood.trinomial_exclusive_nor().setup(
                m_constraint_type_reference.trinomial_exclusive_nor_ptrs);
        }

        if (a_OPTION.neighborhood.is_enabled_soft_selection_move) {
            m_neighborhood.soft_selection().setup(
                m_constraint_type_reference.soft_selection_ptrs);
        }

        if (a_OPTION.neighborhood.is_enabled_chain_move) {
            m_neighborhood.chain().setup();
        }

        if (a_OPTION.neighborhood.is_enabled_two_flip_move &&
            m_flippable_variable_ptr_pairs.size() > 0) {
            m_neighborhood.two_flip().setup(m_flippable_variable_ptr_pairs);
        }

        if (a_OPTION.neighborhood.is_enabled_user_defined_move) {
            m_neighborhood.user_defined().setup();
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

        for (auto &&proxy : m_constraint_proxies) {
            for (auto &&constraint : proxy.flat_indexed_constraints()) {
                constraint.expression().setup_fixed_sensitivities();
            }
        }
        m_objective.expression().setup_fixed_sensitivities();

        /**
         * The fixed sensitivities for the constraints and the objective are
         * build in their own setup() methods.
         */
        utility::print_message("Done.", a_IS_ENABLED_PRINT);
    }

    /*************************************************************************/
    inline linear_programming::LinearProgramming export_lp_instance(
        void) const {
        utility::BidirectionalMap<  //
            model_component::Variable<T_Variable, T_Expression> *, int>
            mutable_variable_index_map;
        utility::BidirectionalMap<  //
            model_component::Constraint<T_Variable, T_Expression> *, int>
            enabled_constraint_index_map;

        std::pair<int, int> less_enabled_constraint_index_range    = {0, 0};
        std::pair<int, int> equal_enabled_constraint_index_range   = {0, 0};
        std::pair<int, int> greater_enabled_constraint_index_range = {0, 0};

        linear_programming::LinearProgramming lp;

        int variable_index = 0;
        for (auto &&variable_ptr :
             m_variable_type_reference.binary_variable_ptrs) {
            if (!variable_ptr->is_fixed()) {
                mutable_variable_index_map.insert(variable_ptr,
                                                  variable_index++);
            }
        }
        for (auto &&variable_ptr :
             m_variable_type_reference.integer_variable_ptrs) {
            if (!variable_ptr->is_fixed()) {
                mutable_variable_index_map.insert(variable_ptr,
                                                  variable_index++);
            }
        }

        for (auto &&variable_ptr :
             m_variable_type_reference.selection_variable_ptrs) {
            if (!variable_ptr->is_fixed()) {
                mutable_variable_index_map.insert(variable_ptr,
                                                  variable_index++);
            }
        }

        int constraint_index = 0;
        int offset           = 0;
        for (auto &&constraint_ptr : m_constraint_reference.less_ptrs) {
            if (constraint_ptr->is_enabled()) {
                enabled_constraint_index_map.insert(constraint_ptr,
                                                    constraint_index++);
            }
        }
        less_enabled_constraint_index_range = {offset, constraint_index};
        offset                              = constraint_index;

        for (auto &&selection : m_selections) {
            if (!selection.constraint_ptr->is_enabled()) {
                enabled_constraint_index_map.insert(selection.constraint_ptr,
                                                    constraint_index++);
            }
        }

        for (auto &&constraint_ptr : m_constraint_reference.equal_ptrs) {
            if (constraint_ptr->is_enabled()) {
                enabled_constraint_index_map.insert(constraint_ptr,
                                                    constraint_index++);
            }
        }

        equal_enabled_constraint_index_range = {offset, constraint_index};
        offset                               = constraint_index;

        for (auto &&constraint_ptr : m_constraint_reference.greater_ptrs) {
            if (constraint_ptr->is_enabled()) {
                enabled_constraint_index_map.insert(constraint_ptr,
                                                    constraint_index++);
            }
        }
        greater_enabled_constraint_index_range = {offset, constraint_index};
        offset += constraint_index;

        using utility::sparse::SparseMatrix;
        using utility::sparse::Vector;

        double objective_offset = 0.0;
        for (const auto &variable_ptr :
             m_variable_reference.fixed_variable_ptrs) {
            objective_offset +=
                variable_ptr->value() * variable_ptr->objective_sensitivity();
        }

        lp.objective_offset = objective_offset;

        const int MUTABLE_VARIABLES_SIZE = mutable_variable_index_map.size();
        const int ENABLED_CONSTRAINTS_SIZE =
            enabled_constraint_index_map.size();
        const int NONZERO_RESERVATION = MUTABLE_VARIABLES_SIZE * 5;

        std::vector<int> is_primal_lower_unbounded(MUTABLE_VARIABLES_SIZE, 0);
        std::vector<int> is_primal_upper_unbounded(MUTABLE_VARIABLES_SIZE, 0);

        Vector primal_objective_coefficients(MUTABLE_VARIABLES_SIZE, 0.0);
        Vector primal_lower_bounds(MUTABLE_VARIABLES_SIZE, 0.0);
        Vector primal_upper_bounds(MUTABLE_VARIABLES_SIZE, 0.0);
        Vector primal_initial_solution(MUTABLE_VARIABLES_SIZE, 0.0);

        for (auto i = 0; i < MUTABLE_VARIABLES_SIZE; i++) {
            primal_initial_solution[i] =
                mutable_variable_index_map.reverse_at(i)->value();
        }

        for (auto i = 0; i < MUTABLE_VARIABLES_SIZE; i++) {
            const auto &VARIABLE_PTR = mutable_variable_index_map.reverse_at(i);

            primal_objective_coefficients[i] =
                VARIABLE_PTR->objective_sensitivity();
            primal_lower_bounds[i] = VARIABLE_PTR->lower_bound();
            primal_upper_bounds[i] = VARIABLE_PTR->upper_bound();
            if (VARIABLE_PTR->lower_bound() < (constant::INT_HALF_MIN >> 1)) {
                is_primal_lower_unbounded[i] = 1;
            }
            if (VARIABLE_PTR->upper_bound() > (constant::INT_HALF_MAX >> 1)) {
                is_primal_upper_unbounded[i] = true;
            }
        }

        primal_initial_solution.clamp(primal_lower_bounds, primal_upper_bounds);

        Vector dual_objective_coefficients(ENABLED_CONSTRAINTS_SIZE, 0.0);
        Vector dual_lower_bounds(ENABLED_CONSTRAINTS_SIZE,
                                 std::numeric_limits<double>::lowest());
        Vector dual_upper_bounds(ENABLED_CONSTRAINTS_SIZE,
                                 std::numeric_limits<double>::max());
        Vector dual_initial_solution(ENABLED_CONSTRAINTS_SIZE, 0.0);

        for (auto i = 0; i < ENABLED_CONSTRAINTS_SIZE; i++) {
            const auto &CONSTRAINT_PTR =
                enabled_constraint_index_map.reverse_at(i);
            const auto &EXPRESSION = CONSTRAINT_PTR->expression();

            double dual_objective_coefficient = EXPRESSION.constant_value();
            for (const auto &sensitivity : EXPRESSION.sensitivities()) {
                if (sensitivity.first->is_fixed()) {
                    dual_objective_coefficient +=
                        sensitivity.first->value() * sensitivity.second;
                }
            }

            if (CONSTRAINT_PTR->sense() ==
                model_component::ConstraintSense::Less) {
                dual_objective_coefficients[i] = dual_objective_coefficient;
            } else {
                dual_objective_coefficients[i] = -dual_objective_coefficient;
            }
        }

        for (auto i = less_enabled_constraint_index_range.first;
             i < less_enabled_constraint_index_range.second; i++) {
            dual_lower_bounds[i] = 0.0;
        }

        for (auto i = greater_enabled_constraint_index_range.first;
             i < greater_enabled_constraint_index_range.second; i++) {
            dual_lower_bounds[i] = 0.0;
        }

        std::vector<int>          row_indices;
        std::vector<int>          column_indices;
        std::vector<T_Expression> values;

        row_indices.reserve(MUTABLE_VARIABLES_SIZE);
        column_indices.reserve(MUTABLE_VARIABLES_SIZE);
        values.reserve(NONZERO_RESERVATION);

        std::vector<std::tuple<int, int, double>> row_records;

        for (auto i = 0; i < ENABLED_CONSTRAINTS_SIZE; i++) {
            const auto &CONSTRAINT_PTR =
                enabled_constraint_index_map.reverse_at(i);
            const auto &EXPRESSION = CONSTRAINT_PTR->expression();

            row_records.clear();

            for (const auto &sensitivity : EXPRESSION.sensitivities()) {
                if (sensitivity.first->is_fixed()) {
                    continue;
                }
                const auto COLUMN_INDEX =
                    mutable_variable_index_map.forward_at(sensitivity.first);
                const auto VALUE = sensitivity.second;

                if (CONSTRAINT_PTR->sense() ==
                    model_component::ConstraintSense::Less) {
                    row_records.emplace_back(i, COLUMN_INDEX, -VALUE);
                } else {
                    row_records.emplace_back(i, COLUMN_INDEX, VALUE);
                }
            }
            std::sort(row_records.begin(), row_records.end(),
                      [](const auto &a_FIRST, const auto &a_SECOND) {
                          return std::get<1>(a_FIRST) < std::get<1>(a_SECOND);
                      });
            for (const auto &record : row_records) {
                row_indices.push_back(std::get<0>(record));
                column_indices.push_back(std::get<1>(record));
                values.push_back(std::get<2>(record));
            }
        }

        SparseMatrix primal_constraint_coefficients(
            values, row_indices, column_indices, ENABLED_CONSTRAINTS_SIZE,
            MUTABLE_VARIABLES_SIZE);

        lp.number_of_rows    = ENABLED_CONSTRAINTS_SIZE;
        lp.number_of_columns = MUTABLE_VARIABLES_SIZE;

        lp.objective_offset = objective_offset;

        lp.primal_constraint_coefficients = primal_constraint_coefficients;
        lp.dual_constraint_coefficients   = utility::sparse::SparseMatrix(
            primal_constraint_coefficients.transpose());

        lp.primal_objective_coefficients = primal_objective_coefficients;
        lp.primal_lower_bounds           = primal_lower_bounds;
        lp.primal_upper_bounds           = primal_upper_bounds;
        lp.primal_initial_solution       = primal_initial_solution;

        lp.is_primal_lower_unbounded = is_primal_lower_unbounded;
        lp.is_primal_upper_unbounded = is_primal_upper_unbounded;

        lp.dual_objective_coefficients = dual_objective_coefficients;
        lp.dual_lower_bounds           = dual_lower_bounds;
        lp.dual_upper_bounds           = dual_upper_bounds;
        lp.dual_initial_solution       = dual_initial_solution;

        lp.less_constraint_index_range  = less_enabled_constraint_index_range;
        lp.equal_constraint_index_range = equal_enabled_constraint_index_range;
        lp.greater_constraint_index_range =
            greater_enabled_constraint_index_range;

        lp.is_minimization = m_is_minimization;
        if (!lp.is_minimization) {
            lp.objective_offset *= -1.0;
            lp.primal_objective_coefficients *= -1.0;
        }
        return lp;
    }

    /*************************************************************************/
    constexpr void shrink_global_penalty_coefficient(
        const bool a_IS_ENABLED_PRINT) {
        utility::print_single_line(a_IS_ENABLED_PRINT);
        utility::print_message("Modifying the global penalty coefficient...",
                               a_IS_ENABLED_PRINT);

        const auto ORIGINAL = m_global_penalty_coefficient;
        const auto MODIFIED =
            std::min(m_global_penalty_coefficient,
                     m_objective.expression().upper_bound() -
                         m_objective.expression().lower_bound() + 1.0);

        if (MODIFIED < ORIGINAL) {
            m_global_penalty_coefficient = MODIFIED;
            utility::print_message(
                "Done (New global penalty coefficient is " +
                    utility::to_string(m_global_penalty_coefficient, "%.5e") +
                    ")",
                a_IS_ENABLED_PRINT);
        } else {
            utility::print_message(
                "Done (global penalty coefficient remains at the original "
                "value).",
                a_IS_ENABLED_PRINT);
        }
    }

    /*************************************************************************/
    constexpr void set_selections(
        const std::vector<model_component::Selection<T_Variable, T_Expression>>
            &a_SELECTIONS) {
        m_selections = a_SELECTIONS;

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
    constexpr int update_variable_bounds(const double a_OBJECTIVE,
                                         const bool   a_IS_PRIMAL,
                                         const bool   a_IS_ENABLED_PRINT) {
        model_component::Constraint<T_Variable, T_Expression> constraint;
        if (m_is_minimization && a_IS_PRIMAL) {
            constraint = m_objective.expression() <= a_OBJECTIVE;
        } else {
            constraint = m_objective.expression() >= a_OBJECTIVE;
        }
        bool is_bound_tightened =
            m_problem_size_reducer
                .remove_redundant_constraint_with_tightening_variable_bound(
                    &constraint, a_IS_ENABLED_PRINT);

        int number_of_newly_fixed_variables = 0;
        if (is_bound_tightened) {
            m_problem_size_reducer.remove_implicit_fixed_variables(
                a_IS_ENABLED_PRINT);
        }

        return number_of_newly_fixed_variables;
    }

    /*************************************************************************/
    inline constexpr void set_global_penalty_coefficient(
        const double a_GLOBAL_PENALTY_COEFFICIENT) {
        m_global_penalty_coefficient = a_GLOBAL_PENALTY_COEFFICIENT;
    }

    /*************************************************************************/
    inline constexpr double global_penalty_coefficient(void) const noexcept {
        return m_global_penalty_coefficient;
    }

    /*************************************************************************/
    constexpr void print_number_of_variables(void) const {
        utility::print_single_line(true);

        const auto &ORIGINAL  = m_variable_reference_original;
        const auto &PRESOLVED = m_variable_reference;

        const auto &ORIGINAL_TYPE  = m_variable_type_reference_original;
        const auto &PRESOLVED_TYPE = m_variable_type_reference;

        auto compute_number_of_variables = [](const auto &a_VARIABLE_PTRS) {
            return a_VARIABLE_PTRS.size();
        };

        auto compute_number_of_mutable_variables =
            [](const auto &a_VARIABLE_PTRS) {
                return std::count_if(a_VARIABLE_PTRS.begin(),
                                     a_VARIABLE_PTRS.end(),
                                     [](const auto *a_VARIABLE_PTR) {
                                         return !a_VARIABLE_PTR->is_fixed();
                                     });
            };

        utility::print_info(  //
            "The number of variables: " +
                utility::to_string(               //
                    compute_number_of_variables(  //
                        ORIGINAL.variable_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                       //
                    compute_number_of_mutable_variables(  //
                        PRESOLVED.variable_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(  //
            " -- Binary: " +
                utility::to_string(               //
                    compute_number_of_variables(  //
                        ORIGINAL_TYPE.binary_variable_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                       //
                    compute_number_of_mutable_variables(  //
                        PRESOLVED_TYPE.binary_variable_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(  //
            " -- Integer: " +
                utility::to_string(               //
                    compute_number_of_variables(  //
                        ORIGINAL_TYPE.integer_variable_ptrs),
                    "%d") +
                " (" +
                utility::to_string(  //
                    compute_number_of_mutable_variables(
                        PRESOLVED_TYPE.integer_variable_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(  //
            " -- Selection: " +
                utility::to_string(  //
                    compute_number_of_variables(
                        ORIGINAL_TYPE.selection_variable_ptrs),
                    "%d") +
                " (" +
                utility::to_string(  //
                    compute_number_of_mutable_variables(
                        PRESOLVED_TYPE.selection_variable_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(  //
            " -- Dependent Binary: " +
                utility::to_string(  //
                    compute_number_of_variables(
                        ORIGINAL_TYPE.dependent_binary_variable_ptrs),
                    "%d") +
                " (" +
                utility::to_string(  //
                    compute_number_of_mutable_variables(
                        PRESOLVED_TYPE.dependent_binary_variable_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(  //
            " -- Dependent Integer: " +
                utility::to_string(  //
                    compute_number_of_variables(
                        ORIGINAL_TYPE.dependent_integer_variable_ptrs),
                    "%d") +
                " (" +
                utility::to_string(  //
                    compute_number_of_mutable_variables(
                        PRESOLVED_TYPE.dependent_integer_variable_ptrs),
                    "%d") +
                ")",
            true);

        utility::print(  //
            "          ( ) : Number of mutable variables after presolve.",
            true);
    }

    /*************************************************************************/
    constexpr void print_number_of_constraints(void) const {
        utility::print_single_line(true);

        const auto &ORIGINAL  = m_constraint_reference_original;
        const auto &PRESOLVED = m_constraint_reference;

        const auto &ORIGINAL_TYPE  = m_constraint_type_reference_original;
        const auto &PRESOLVED_TYPE = m_constraint_type_reference;

        auto compute_number_of_constraints = [](const auto &a_CONSTRAINT_PTRS) {
            return a_CONSTRAINT_PTRS.size();
        };

        auto compute_number_of_enabled_constraints =
            [](const auto &a_CONSTRAINT_PTRS) {
                return std::count_if(a_CONSTRAINT_PTRS.begin(),
                                     a_CONSTRAINT_PTRS.end(),
                                     [](const auto *a_CONSTRAINT_PTR) {
                                         return a_CONSTRAINT_PTR->is_enabled();
                                     });
            };

        utility::print_info(  //
            "The number of constraints: " +
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL.constraint_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED.constraint_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(                        //
            "[<= : " +                              //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL.less_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED.less_ptrs),
                    "%d") +
                "), " + "== : " +                   //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL.equal_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED.equal_ptrs),
                    "%d") +
                "), " + ">= : " +                   //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL.greater_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED.greater_ptrs),
                    "%d") +
                ")]",
            true);

        utility::print_info(                        //
            " -- Singleton: " +                     //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL_TYPE.singleton_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED_TYPE.singleton_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(                        //
            " -- Exclusive OR: " +                  //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL_TYPE.exclusive_or_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED_TYPE.exclusive_or_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(                        //
            " -- Exclusive NOR: " +                 //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL_TYPE.exclusive_nor_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED_TYPE.exclusive_nor_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(                        //
            " -- Inverted Integers: " +             //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL_TYPE.inverted_integers_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED_TYPE.inverted_integers_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(                        //
            " -- Balanced Integers: " +             //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL_TYPE.balanced_integers_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED_TYPE.balanced_integers_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(                        //
            " -- Constant Sum Integers: " +         //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL_TYPE.constant_sum_integers_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED_TYPE.constant_sum_integers_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(                        //
            " -- Constant Difference Integers: " +  //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL_TYPE.constant_difference_integers_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED_TYPE.constant_difference_integers_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(                        //
            " -- Constant Ratio Integers: " +       //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL_TYPE.constant_ratio_integers_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED_TYPE.constant_ratio_integers_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(                        //
            " -- Aggregation: " +                   //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL_TYPE.aggregation_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED_TYPE.aggregation_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(                        //
            " -- Precedence: " +                    //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL_TYPE.precedence_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED_TYPE.precedence_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(                        //
            " -- Variable Bound: " +                //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL_TYPE.variable_bound_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED_TYPE.variable_bound_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(                        //
            " -- Trinomial Exclusive NOR: " +       //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL_TYPE.trinomial_exclusive_nor_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED_TYPE.trinomial_exclusive_nor_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(                        //
            " -- Set Partitioning: " +              //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL_TYPE.set_partitioning_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED_TYPE.set_partitioning_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(                        //
            " -- Set Packing: " +                   //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL_TYPE.set_packing_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED_TYPE.set_packing_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(                        //
            " -- Set Covering: " +                  //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL_TYPE.set_covering_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED_TYPE.set_covering_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(                        //
            " -- Cardinality: " +                   //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL_TYPE.cardinality_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED_TYPE.cardinality_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(                        //
            " -- Invariant Knapsack: " +            //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL_TYPE.invariant_knapsack_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED_TYPE.invariant_knapsack_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(                        //
            " -- Multiple Covering: " +             //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL_TYPE.multiple_covering_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED_TYPE.multiple_covering_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(                        //
            " -- Binary Flow: " +                   //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL_TYPE.binary_flow_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED_TYPE.binary_flow_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(                        //
            " -- Integer Flow: " +                  //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL_TYPE.integer_flow_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED_TYPE.integer_flow_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(                        //
            " -- Soft Selection: " +                //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL_TYPE.soft_selection_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED_TYPE.soft_selection_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(                        //
            " -- Min-Max: " +                       //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL_TYPE.min_max_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED_TYPE.min_max_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(                        //
            " -- Max-Min: " +                       //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL_TYPE.max_min_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED_TYPE.max_min_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(                        //
            " -- Intermediate: " +                  //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL_TYPE.intermediate_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED_TYPE.intermediate_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(                        //
            " -- Equation Knapsack: " +             //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL_TYPE.equation_knapsack_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED_TYPE.equation_knapsack_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(                        //
            " -- Bin Packing: " +                   //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL_TYPE.bin_packing_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED_TYPE.bin_packing_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(                        //
            " -- Knapsack: " +                      //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL_TYPE.knapsack_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED_TYPE.knapsack_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(                        //
            " -- Integer Knapsack: " +              //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL_TYPE.integer_knapsack_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED_TYPE.integer_knapsack_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(                        //
            " -- GF(2): " +                         //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL_TYPE.gf2_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED_TYPE.gf2_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(                        //
            " -- General Linear: " +                //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL_TYPE.general_linear_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED_TYPE.general_linear_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(                        //
            " -- Nonlinear: " +                     //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL_TYPE.nonlinear_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED_TYPE.nonlinear_ptrs),
                    "%d") +
                ")",
            true);

        utility::print(  //
            "          ( ) : Number of enabled constraints after presolve.",
            true);
    }

    /*************************************************************************/
    inline constexpr void set_callback(
        const std::function<
            void(option::Option *,
                 solution::IncumbentHolder<T_Variable, T_Expression> *)>
            &a_CALLBACK) {
        m_callback = a_CALLBACK;
    }

    /*************************************************************************/
    inline constexpr void callback(
        option::Option *a_option_ptr,
        solution::IncumbentHolder<T_Variable, T_Expression>
            *a_incumbent_holder_ptr) {
        m_callback(a_option_ptr, a_incumbent_holder_ptr);
    }

    /*************************************************************************/
    constexpr void import_variable_values(
        const std::vector<multi_array::ValueProxy<T_Variable>> &a_PROXIES) {
        for (auto &&proxy : m_variable_proxies) {
            for (auto &&variable : proxy.flat_indexed_variables()) {
                const int PROXY_INDEX = variable.proxy_index();
                const int FLAT_INDEX  = variable.flat_index();
                variable.set_value_if_mutable(
                    a_PROXIES[PROXY_INDEX].flat_indexed_values(FLAT_INDEX));
            }
        }
        preprocess::Verifier<T_Variable, T_Expression> verifier(this);
        verifier.verify_and_correct_selection_variables_initial_values(  //
            false, false);
        verifier.verify_and_correct_binary_variables_initial_values(  //
            false, false);
        verifier.verify_and_correct_integer_variables_initial_values(  //
            false, false);
    }

    /*************************************************************************/
    constexpr void update(void) {
        /**
         * Update in order of expressions -> dependent variables -> objective,
         * constraints.
         */

        for (auto &&proxy : m_expression_proxies) {
            for (auto &&expression : proxy.flat_indexed_expressions()) {
                expression.update();
            }
        }

        for (auto &&variable_ptr :
             m_variable_type_reference.dependent_integer_variable_ptrs) {
            variable_ptr->update();
        }

        for (auto &&variable_ptr :
             m_variable_type_reference.dependent_binary_variable_ptrs) {
            variable_ptr->update();
        }

        for (auto &&proxy : m_constraint_proxies) {
            for (auto &&constraint : proxy.flat_indexed_constraints()) {
                constraint.update();
            }
        }

        if (m_is_defined_objective) {
            m_objective.update();
        }

        this->update_violative_constraint_ptrs_and_feasibility();
    }

    /*************************************************************************/
    constexpr void update_dependent_variables_and_disabled_constraints(void) {
        /**
         * Update in order of expressions -> dependent variables, and
         * constraints.
         */
        for (auto &&proxy : m_expression_proxies) {
            for (auto &&expression : proxy.flat_indexed_expressions()) {
                expression.update();
            }
        }

        for (auto &&variable_ptr :
             m_variable_type_reference.dependent_integer_variable_ptrs) {
            variable_ptr->update();
        }

        for (auto &&variable_ptr :
             m_variable_type_reference.dependent_binary_variable_ptrs) {
            variable_ptr->update();
        }

        for (auto &&constraint_ptr :
             m_constraint_reference.disabled_constraint_ptrs) {
            constraint_ptr->update();
        }
    }

    /*************************************************************************/
    constexpr void update(
        const neighborhood::Move<T_Variable, T_Expression> &a_MOVE) {
        /**
         * Update in order of objective, constraints -> expressions ->
         * variables. Note that this method DOES NOT update disabled constraints
         * and dependent variables. If the consistent solution is required,
         * perform update() before obtaining solution.
         */
        if (m_is_defined_objective) {
            m_objective.update(a_MOVE);
        }

        if (m_neighborhood.user_defined().is_enabled()) {
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
            alteration.first->set_value_if_mutable(alteration.second);
        }

        if (a_MOVE.sense == neighborhood::MoveSense::Selection) {
            a_MOVE.alterations[1].first->select();
        }

        this->update_violative_constraint_ptrs_and_feasibility();
    }

    /*************************************************************************/
    inline constexpr void reset_variable_objective_improvabilities(
        const std::vector<model_component::Variable<T_Variable, T_Expression> *>
            &a_VARIABLE_PTRS) {
        for (auto &&variable_ptr : a_VARIABLE_PTRS) {
            variable_ptr->set_is_objective_improvable(false);
        }
    }

    /*************************************************************************/
    inline constexpr void reset_variable_objective_improvabilities(void) {
        this->reset_variable_objective_improvabilities(
            this->variable_reference().variable_ptrs);
    }

    /*************************************************************************/
    inline constexpr void reset_variable_feasibility_improvabilities(
        const std::vector<model_component::Variable<T_Variable, T_Expression> *>
            &a_VARIABLE_PTRS) const noexcept {
        for (auto &&variable_ptr : a_VARIABLE_PTRS) {
            variable_ptr->set_is_feasibility_improvable(false);
        }
    }

    /*************************************************************************/
    inline constexpr void reset_variable_feasibility_improvabilities(
        const std::vector<model_component::Constraint<T_Variable, T_Expression>
                              *> &a_CONSTRAINT_PTRS) const noexcept {
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
    inline constexpr void reset_variable_feasibility_improvabilities(void) {
        this->reset_variable_feasibility_improvabilities(
            this->variable_reference().variable_ptrs);
    }

    /*************************************************************************/
    inline constexpr void update_variable_objective_improvabilities(void) {
        this->update_variable_objective_improvabilities(
            this->variable_reference().mutable_variable_ptrs);
    }

    /*************************************************************************/
    constexpr void update_variable_objective_improvabilities(
        const std::vector<model_component::Variable<T_Variable, T_Expression> *>
            &a_VARIABLE_PTRS) const noexcept {
        double coefficient             = 0.0;
        bool   is_objective_improvable = false;
        for (const auto &variable_ptr : a_VARIABLE_PTRS) {
#ifdef _PRINTEMPS_LINEAR_MINIMIZATION
            coefficient = variable_ptr->objective_sensitivity();
#else
            coefficient = variable_ptr->objective_sensitivity() * this->sign();
#endif
            is_objective_improvable =
                (coefficient > 0 && variable_ptr->has_lower_bound_margin()) ||
                (coefficient < 0 && variable_ptr->has_upper_bound_margin());

            variable_ptr->set_is_objective_improvable(is_objective_improvable);
        }
    }

    /*************************************************************************/
    inline constexpr void update_variable_feasibility_improvabilities(void) {
        this->update_variable_feasibility_improvabilities(
            this->constraint_reference().enabled_constraint_ptrs);
    }

    /*************************************************************************/
    constexpr void update_variable_feasibility_improvabilities(
        const std::vector<model_component::Constraint<T_Variable, T_Expression>
                              *> &a_CONSTRAINT_PTRS) const noexcept {
        double coefficient = 0.0;
        for (const auto &constraint_ptr : a_CONSTRAINT_PTRS) {
            if (constraint_ptr->is_feasible()) {
                continue;
            }

            if (!constraint_ptr->is_enabled()) {
                continue;
            }

            for (const auto &sensitivity :
                 constraint_ptr->expression().sensitivities()) {
                if (sensitivity.first->is_feasibility_improvable() ||
                    sensitivity.first->is_fixed()) {
                    continue;
                }

                coefficient =
                    (constraint_ptr->constraint_value() > constant::EPSILON &&
                     constraint_ptr->is_less_or_equal())
                        ? sensitivity.second
                        : -sensitivity.second;

                if ((coefficient > 0 &&
                     sensitivity.first->has_lower_bound_margin()) ||
                    (coefficient < 0 &&
                     sensitivity.first->has_upper_bound_margin())) {
                    sensitivity.first->set_is_feasibility_improvable_or(true);
                }
            }
        }
    }

    /*************************************************************************/
    inline constexpr void update_violative_constraint_ptrs_and_feasibility(
        void) {
        m_violative_constraint_ptrs.clear();
        for (auto &&constraint_ptr :
             m_constraint_reference.enabled_constraint_ptrs) {
            if (!constraint_ptr->is_feasible()) {
                m_violative_constraint_ptrs.push_back(constraint_ptr);
            }
        }
        m_is_feasible = (m_violative_constraint_ptrs.size() == 0);
    }

    /*************************************************************************/
    inline solution::SolutionScore evaluate(
        const neighborhood::Move<T_Variable, T_Expression> &a_MOVE) const
        noexcept {
        solution::SolutionScore score;
        this->evaluate(&score, a_MOVE);
        return score;
    }

    /*************************************************************************/
    inline solution::SolutionScore evaluate(
        const neighborhood::Move<T_Variable, T_Expression> &a_MOVE,
        const solution::SolutionScore &a_CURRENT_SCORE) const noexcept {
        solution::SolutionScore score;
        if (a_MOVE.alterations.size() == 1) {
            this->evaluate_single(&score, a_MOVE, a_CURRENT_SCORE);
        } else {
            this->evaluate_multi(&score, a_MOVE, a_CURRENT_SCORE);
        }
        return score;
    }

    /*************************************************************************/
    constexpr void evaluate(
        solution::SolutionScore *                           a_score_ptr,  //
        const neighborhood::Move<T_Variable, T_Expression> &a_MOVE) const
        noexcept {
        double total_violation = 0.0;
        double local_penalty   = 0.0;

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
                double constraint_value =
                    constraints[j].evaluate_constraint(a_MOVE);
                double positive_part = std::max(constraint_value, 0.0);
                double negative_part = std::max(-constraint_value, 0.0);
                double violation     = 0.0;

                if (constraints[j].is_less_or_equal()) {
                    violation = positive_part;
                    total_violation += violation;
                    if (violation + constant::EPSILON <
                        constraints[j].positive_part()) {
                        is_feasibility_improvable = true;
                    }
                    local_penalty +=
                        violation *
                        constraints[j].local_penalty_coefficient_less();
                }

                if (constraints[j].is_greater_or_equal()) {
                    violation = negative_part;
                    total_violation += violation;
                    if (violation + constant::EPSILON <
                        constraints[j].negative_part()) {
                        is_feasibility_improvable = true;
                    }
                    local_penalty +=
                        violation *
                        constraints[j].local_penalty_coefficient_greater();
                }
            }
        }

#ifdef _PRINTEMPS_LINEAR_MINIMIZATION
        double objective             = m_objective.evaluate(a_MOVE);
        double objective_improvement = m_objective.value() - objective;

#else
        double objective = 0.0;
        double objective_improvement = 0.0;
        if (m_is_defined_objective) {
            objective = m_objective.evaluate(a_MOVE) * this->sign();
            objective_improvement =
                m_objective.value() * this->sign() - objective;
        }
#endif

        const double GLOBAL_PENALTY =
            total_violation * m_global_penalty_coefficient;

        a_score_ptr->objective                  = objective;
        a_score_ptr->objective_improvement      = objective_improvement;
        a_score_ptr->total_violation            = total_violation;
        a_score_ptr->local_penalty              = local_penalty;
        a_score_ptr->global_penalty             = GLOBAL_PENALTY;
        a_score_ptr->local_augmented_objective  = objective + local_penalty;
        a_score_ptr->global_augmented_objective = objective + GLOBAL_PENALTY;
        a_score_ptr->is_feasible = !(total_violation > constant::EPSILON);
        a_score_ptr->is_objective_improvable =
            objective_improvement > constant::EPSILON;
        a_score_ptr->is_feasibility_improvable = is_feasibility_improvable;
    }

    /*************************************************************************/
    constexpr void evaluate_single(
        solution::SolutionScore *                           a_score_ptr,  //
        const neighborhood::Move<T_Variable, T_Expression> &a_MOVE,
        const solution::SolutionScore &a_CURRENT_SCORE) const noexcept {
        double total_violation  = a_CURRENT_SCORE.total_violation;
        double local_penalty    = a_CURRENT_SCORE.local_penalty;
        double constraint_value = 0.0;
        double violation_diff   = 0.0;

        const auto &variable_ptr = a_MOVE.alterations.front().first;
        const auto  variable_value_diff =
            a_MOVE.alterations.front().second - variable_ptr->value();
        const auto &constraint_sensitivities =
            variable_ptr->constraint_sensitivities();

        for (const auto &sensitivity : constraint_sensitivities) {
            const auto &constraint_ptr = sensitivity.first;
            if (!constraint_ptr->is_enabled()) {
                continue;
            }
            constraint_value = constraint_ptr->constraint_value() +
                               sensitivity.second * variable_value_diff;

            if (constraint_ptr->is_less_or_equal()) {
                violation_diff = std::max(constraint_value, 0.0) -
                                 constraint_ptr->positive_part();
                total_violation += violation_diff;

                local_penalty +=
                    violation_diff *
                    constraint_ptr->local_penalty_coefficient_less();
            }

            if (constraint_ptr->is_greater_or_equal()) {
                violation_diff = std::min(constraint_value, 0.0) +
                                 constraint_ptr->negative_part();
                total_violation -= violation_diff;

                local_penalty -=
                    violation_diff *
                    constraint_ptr->local_penalty_coefficient_greater();
            }
        }

#ifdef _PRINTEMPS_LINEAR_MINIMIZATION
        double objective             = m_objective.evaluate(a_MOVE);
        double objective_improvement = m_objective.value() - objective;

#else
        double objective = 0.0;
        double objective_improvement = 0.0;
        if (m_is_defined_objective) {
            objective = m_objective.evaluate(a_MOVE) * this->sign();
            objective_improvement =
                m_objective.value() * this->sign() - objective;
        }
#endif

        const double GLOBAL_PENALTY =
            total_violation * m_global_penalty_coefficient;

        a_score_ptr->objective                  = objective;
        a_score_ptr->objective_improvement      = objective_improvement;
        a_score_ptr->total_violation            = total_violation;
        a_score_ptr->local_penalty              = local_penalty;
        a_score_ptr->global_penalty             = GLOBAL_PENALTY;
        a_score_ptr->local_augmented_objective  = objective + local_penalty;
        a_score_ptr->global_augmented_objective = objective + GLOBAL_PENALTY;
        a_score_ptr->is_feasible = !(total_violation > constant::EPSILON);
        a_score_ptr->is_objective_improvable =
            objective_improvement > constant::EPSILON;
        a_score_ptr->is_feasibility_improvable = true;  /// do not care.
    }

    /*************************************************************************/
    constexpr void evaluate_selection(
        solution::SolutionScore *                           a_score_ptr,  //
        const neighborhood::Move<T_Variable, T_Expression> &a_MOVE,
        const solution::SolutionScore &a_CURRENT_SCORE) const noexcept {
        bool is_feasibility_improvable = false;

        double total_violation  = a_CURRENT_SCORE.total_violation;
        double local_penalty    = a_CURRENT_SCORE.local_penalty;
        double constraint_value = 0.0;
        double violation_diff   = 0.0;

        const auto &RELATED_CONSTRAINT_PTRS = a_MOVE.alterations.front()
                                                  .first->selection_ptr()
                                                  ->related_constraint_ptrs;

        const auto &INDEX_MIN_FIRST =
            a_MOVE.alterations[0]
                .first->related_selection_constraint_ptr_index_min();
        const auto &INDEX_MAX_FIRST =
            a_MOVE.alterations[0]
                .first->related_selection_constraint_ptr_index_max();
        const auto &INDEX_MIN_SECOND =
            a_MOVE.alterations[1]
                .first->related_selection_constraint_ptr_index_min();
        const auto &INDEX_MAX_SECOND =
            a_MOVE.alterations[1]
                .first->related_selection_constraint_ptr_index_max();

        const int MIN_INDEX = std::min(INDEX_MIN_FIRST, INDEX_MIN_SECOND);
        const int MAX_INDEX = std::max(INDEX_MAX_FIRST, INDEX_MAX_SECOND);
        if (MIN_INDEX >= 0 && MAX_INDEX >= 0) {
            if ((INDEX_MAX_FIRST < INDEX_MIN_SECOND) ||
                (INDEX_MAX_SECOND < INDEX_MIN_FIRST)) {
                for (const auto &alteration : a_MOVE.alterations) {
                    const auto &variable_ptr = alteration.first;
                    const auto  variable_value_diff =
                        alteration.second - variable_ptr->value();
                    const auto &constraint_sensitivities =
                        variable_ptr->constraint_sensitivities();

                    for (const auto &sensitivity : constraint_sensitivities) {
                        const auto &constraint_ptr = sensitivity.first;
                        if (!constraint_ptr->is_enabled()) {
                            continue;
                        }
                        constraint_value =
                            constraint_ptr->constraint_value() +
                            sensitivity.second * variable_value_diff;

                        if (constraint_ptr->is_less_or_equal()) {
                            violation_diff = std::max(constraint_value, 0.0) -
                                             constraint_ptr->positive_part();
                            total_violation += violation_diff;

                            local_penalty +=
                                violation_diff *
                                constraint_ptr
                                    ->local_penalty_coefficient_less();
                        }

                        if (constraint_ptr->is_greater_or_equal()) {
                            violation_diff = std::min(constraint_value, 0.0) +
                                             constraint_ptr->negative_part();
                            total_violation -= violation_diff;

                            local_penalty -=
                                violation_diff *
                                constraint_ptr
                                    ->local_penalty_coefficient_greater();
                        }
                    }
                }
                is_feasibility_improvable = true;
            } else {
                double violation_diff_negative = 0.0;
                double violation_diff_positive = 0.0;

                for (auto i = MIN_INDEX; i <= MAX_INDEX; i++) {
                    auto &constraint_ptr = RELATED_CONSTRAINT_PTRS[i];
                    if (!constraint_ptr->is_enabled()) {
                        continue;
                    }

                    constraint_value =
                        constraint_ptr->evaluate_constraint(a_MOVE);

                    if (fabs(constraint_value -
                             constraint_ptr->constraint_value()) <
                        constant::EPSILON_10) {
                        continue;
                    }

                    violation_diff_positive =
                        constraint_ptr->is_less_or_equal()
                            ? std::max(constraint_value, 0.0) -
                                  constraint_ptr->positive_part()
                            : 0.0;

                    violation_diff_negative =
                        constraint_ptr->is_greater_or_equal()
                            ? std::max(-constraint_value, 0.0) -
                                  constraint_ptr->negative_part()
                            : 0.0;
                    violation_diff =
                        violation_diff_positive + violation_diff_negative;
                    local_penalty +=
                        violation_diff_positive *
                            constraint_ptr->local_penalty_coefficient_less() +
                        violation_diff_negative *
                            constraint_ptr->local_penalty_coefficient_greater();

                    total_violation += violation_diff;
                    is_feasibility_improvable |=
                        violation_diff < -constant::EPSILON;
                }
            }
        }

#ifdef _PRINTEMPS_LINEAR_MINIMIZATION
        double objective             = m_objective.evaluate(a_MOVE);
        double objective_improvement = m_objective.value() - objective;

#else
        double objective = 0.0;
        double objective_improvement = 0.0;
        if (m_is_defined_objective) {
            objective = m_objective.evaluate(a_MOVE) * this->sign();
            objective_improvement =
                m_objective.value() * this->sign() - objective;
        }
#endif
        const double GLOBAL_PENALTY =
            total_violation * m_global_penalty_coefficient;

        a_score_ptr->objective                  = objective;
        a_score_ptr->objective_improvement      = objective_improvement;
        a_score_ptr->total_violation            = total_violation;
        a_score_ptr->local_penalty              = local_penalty;
        a_score_ptr->global_penalty             = GLOBAL_PENALTY;
        a_score_ptr->local_augmented_objective  = objective + local_penalty;
        a_score_ptr->global_augmented_objective = objective + GLOBAL_PENALTY;
        a_score_ptr->is_feasible = !(total_violation > constant::EPSILON);
        a_score_ptr->is_objective_improvable =
            objective_improvement > constant::EPSILON;
        a_score_ptr->is_feasibility_improvable = is_feasibility_improvable;
    }

    /*************************************************************************/
    constexpr void evaluate_multi(
        solution::SolutionScore *                           a_score_ptr,  //
        const neighborhood::Move<T_Variable, T_Expression> &a_MOVE,
        const solution::SolutionScore &a_CURRENT_SCORE) const noexcept {
        bool is_feasibility_improvable = false;

        double total_violation         = a_CURRENT_SCORE.total_violation;
        double local_penalty           = a_CURRENT_SCORE.local_penalty;
        double constraint_value        = 0.0;
        double violation_diff_negative = 0.0;
        double violation_diff_positive = 0.0;
        double violation_diff          = 0.0;

        for (const auto &constraint_ptr : a_MOVE.related_constraint_ptrs) {
            if (!constraint_ptr->is_enabled()) {
                continue;
            }

            constraint_value = constraint_ptr->evaluate_constraint(a_MOVE);

            if (fabs(constraint_value - constraint_ptr->constraint_value()) <
                constant::EPSILON_10) {
                continue;
            }

            violation_diff_positive = constraint_ptr->is_less_or_equal()
                                          ? std::max(constraint_value, 0.0) -
                                                constraint_ptr->positive_part()
                                          : 0.0;

            violation_diff_negative = constraint_ptr->is_greater_or_equal()
                                          ? std::max(-constraint_value, 0.0) -
                                                constraint_ptr->negative_part()
                                          : 0.0;

            violation_diff = violation_diff_positive + violation_diff_negative;
            local_penalty +=
                violation_diff_positive *
                    constraint_ptr->local_penalty_coefficient_less() +
                violation_diff_negative *
                    constraint_ptr->local_penalty_coefficient_greater();

            total_violation += violation_diff;
            is_feasibility_improvable |= violation_diff < -constant::EPSILON;
        }

#ifdef _PRINTEMPS_LINEAR_MINIMIZATION
        double objective             = m_objective.evaluate(a_MOVE);
        double objective_improvement = m_objective.value() - objective;

#else
        double objective = 0.0;
        double objective_improvement = 0.0;
        if (m_is_defined_objective) {
            objective = m_objective.evaluate(a_MOVE) * this->sign();
            objective_improvement =
                m_objective.value() * this->sign() - objective;
        }
#endif
        const double GLOBAL_PENALTY =
            total_violation * m_global_penalty_coefficient;

        a_score_ptr->objective                  = objective;
        a_score_ptr->objective_improvement      = objective_improvement;
        a_score_ptr->total_violation            = total_violation;
        a_score_ptr->local_penalty              = local_penalty;
        a_score_ptr->global_penalty             = GLOBAL_PENALTY;
        a_score_ptr->local_augmented_objective  = objective + local_penalty;
        a_score_ptr->global_augmented_objective = objective + GLOBAL_PENALTY;
        a_score_ptr->is_feasible = !(total_violation > constant::EPSILON);
        a_score_ptr->is_objective_improvable =
            objective_improvement > constant::EPSILON;
        a_score_ptr->is_feasibility_improvable = is_feasibility_improvable;
    }

    /*************************************************************************/
    constexpr double compute_lagrangian(
        const std::vector<multi_array::ValueProxy<double>>
            &a_LAGRANGE_MULTIPLIER_PROXIES) const noexcept {
        double lagrangian = m_objective.value();

        for (auto &&constraint_ptr : m_constraint_reference.constraint_ptrs) {
            const int PROXY_INDEX = constraint_ptr->proxy_index();
            const int FLAT_INDEX  = constraint_ptr->flat_index();

            lagrangian +=
                a_LAGRANGE_MULTIPLIER_PROXIES[PROXY_INDEX].flat_indexed_values(
                    FLAT_INDEX) *
                constraint_ptr->constraint_value();
        }
        return lagrangian;
    }

    /*************************************************************************/
    constexpr double compute_naive_dual_bound(void) const noexcept {
        double dual_bound = m_is_minimization
                                ? std::numeric_limits<double>::lowest()
                                : std::numeric_limits<double>::max();

        if (!m_is_linear) {
            return dual_bound;
        }

        dual_bound = m_objective.expression().constant_value();
        for (auto &&sensitivity : m_objective.expression().sensitivities()) {
            const auto VARIABLE_PTR = sensitivity.first;
            const auto COEFFICIENT  = sensitivity.second;
            if (VARIABLE_PTR->is_fixed()) {
                dual_bound += VARIABLE_PTR->value() * COEFFICIENT;
            } else {
                const auto LOWER_BOUND = VARIABLE_PTR->lower_bound();
                const auto UPPER_BOUND = VARIABLE_PTR->upper_bound();
                if (m_is_minimization == (COEFFICIENT > 0)) {
                    dual_bound += LOWER_BOUND * COEFFICIENT;
                } else {
                    dual_bound += UPPER_BOUND * COEFFICIENT;
                }
            }
        }
        return dual_bound;
    }

    /*************************************************************************/
    template <class T_Value>
    constexpr std::vector<multi_array::ValueProxy<T_Value>>
    generate_variable_parameter_proxies(const T_Value a_VALUE) const {
        std::vector<multi_array::ValueProxy<T_Value>>
            variable_parameter_proxies;

        for (const auto &proxy : m_variable_proxies) {
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
    constexpr std::vector<multi_array::ValueProxy<T_Value>>
    generate_expression_parameter_proxies(const T_Value a_VALUE) const {
        std::vector<multi_array::ValueProxy<T_Value>>
            expression_parameter_proxies;

        for (const auto &proxy : m_expression_proxies) {
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
    constexpr std::vector<multi_array::ValueProxy<T_Value>>
    generate_constraint_parameter_proxies(const T_Value a_VALUE) const {
        std::vector<multi_array::ValueProxy<T_Value>>
            constraint_parameter_proxies;

        for (const auto &proxy : m_constraint_proxies) {
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
    std::vector<multi_array::ValueProxy<double>>
    export_local_penalty_coefficient_proxies(void) const {
        std::vector<multi_array::ValueProxy<double>>
            local_penalty_coefficient_proxies;
        for (const auto &proxy : m_constraint_proxies) {
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
                                 .local_penalty_coefficient_less());
            }
            local_penalty_coefficient_proxies.push_back(
                local_penalty_coefficient_proxy);
        }
        return local_penalty_coefficient_proxies;
    }

    /*************************************************************************/
    solution::DenseSolution<T_Variable, T_Expression> export_solution(
        void) const {
        /// This method cannot be constexpr by clang.
        solution::DenseSolution<T_Variable, T_Expression> solution;

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
    constexpr solution::NamedSolution<T_Variable, T_Expression>
    export_named_solution(void) const {
        return this->convert_to_named_solution(this->export_solution());
    }

    /*************************************************************************/
    solution::NamedSolution<T_Variable, T_Expression> convert_to_named_solution(
        const solution::DenseSolution<T_Variable, T_Expression> &a_SOLUTION)
        const {
        /// This method cannot be constexpr by clang.
        solution::NamedSolution<T_Variable, T_Expression> named_solution;

        const int VARIABLE_PROXIES_SIZE   = m_variable_proxies.size();
        const int EXPRESSION_PROXIES_SIZE = m_expression_proxies.size();
        const int CONSTRAINT_PROXIES_SIZE = m_constraint_proxies.size();

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

        named_solution.m_name                  = m_name;
        named_solution.m_number_of_variables   = this->number_of_variables();
        named_solution.m_number_of_constraints = this->number_of_constraints();
        named_solution.m_objective             = a_SOLUTION.objective;
        named_solution.m_total_violation       = a_SOLUTION.total_violation;
        named_solution.m_is_feasible           = a_SOLUTION.is_feasible;

        return named_solution;
    }

    /*************************************************************************/
    solution::SparseSolution<T_Variable, T_Expression> export_sparse_solution(
        void) const {
        solution::SparseSolution<T_Variable, T_Expression> sparse_solution;

        /// Decision variables
        for (const auto &proxy : m_variable_proxies) {
            for (const auto &variable : proxy.flat_indexed_variables()) {
                if (variable.value() != 0) {
                    sparse_solution.variables[variable.name()] =
                        variable.value();
                }
            }
        }

        /// Total violation
        T_Expression total_violation = 0;
        for (const auto &proxy : m_constraint_proxies) {
            for (const auto &constraint : proxy.flat_indexed_constraints()) {
                total_violation += constraint.violation_value();
            }
        }

        sparse_solution.objective       = m_objective.value();
        sparse_solution.total_violation = total_violation;
        sparse_solution.is_feasible     = this->is_feasible();

        return sparse_solution;
    }

    /*************************************************************************/
    solution::SparseSolution<T_Variable, T_Expression>
    convert_to_sparse_solution(
        const solution::DenseSolution<T_Variable, T_Expression> &a_SOLUTION)
        const {
        solution::SparseSolution<T_Variable, T_Expression> sparse_solution;

        /// Decision variables
        for (const auto &proxy : m_variable_proxies) {
            for (const auto &variable : proxy.flat_indexed_variables()) {
                if (variable.value() != 0) {
                    sparse_solution.variables[variable.name()] =
                        variable.value();
                }
            }
        }

        sparse_solution.objective       = a_SOLUTION.objective;
        sparse_solution.total_violation = a_SOLUTION.total_violation;
        sparse_solution.is_feasible     = a_SOLUTION.is_feasible;

        return sparse_solution;
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
    constexpr void fix_variables(
        const std::unordered_map<std::string, int> &a_SOLUTION) {
        for (auto &&proxy : m_variable_proxies) {
            for (auto &&variable : proxy.flat_indexed_variables()) {
                if (a_SOLUTION.find(variable.name()) != a_SOLUTION.end()) {
                    variable.fix_by(a_SOLUTION.at(variable.name()));
                }
            }
        }
    }

    /*************************************************************************/
    constexpr void unfix_variables(
        const std::unordered_set<std::string> &a_VARIABLE_NAMES) {
        for (auto &&proxy : m_variable_proxies) {
            for (auto &&variable : proxy.flat_indexed_variables()) {
                variable.fix_by(0);
                if (a_VARIABLE_NAMES.find(variable.name()) !=
                    a_VARIABLE_NAMES.end()) {
                    variable.unfix();
                }
            }
        }
    }

    /*************************************************************************/
    constexpr void set_user_defined_selection_constraints(
        const std::unordered_set<std::string> &a_CONSTRAINT_NAMES) {
        for (auto &&proxy : m_constraint_proxies) {
            for (auto &&constraint : proxy.flat_indexed_constraints()) {
                if (a_CONSTRAINT_NAMES.find(constraint.name()) !=
                    a_CONSTRAINT_NAMES.end()) {
                    constraint.set_is_user_defined_selection(true);
                }
            }
        }
    }

    /*************************************************************************/
    constexpr void setup_flippable_variable_ptr_pairs(
        const std::vector<std::pair<std::string, std::string>>
            &a_VARIABLE_NAME_PAIRS) {
        std::unordered_map<
            std::string, model_component::Variable<T_Variable, T_Expression> *>
            variable_ptrs;

        for (auto &&proxy : m_variable_proxies) {
            for (auto &&variable : proxy.flat_indexed_variables()) {
                variable_ptrs[variable.name()] = &variable;
            }
        }

        std::vector<
            std::pair<model_component::Variable<T_Variable, T_Expression> *,
                      model_component::Variable<T_Variable, T_Expression> *>>
            flippable_variable_ptr_pairs;

        for (const auto &pair : a_VARIABLE_NAME_PAIRS) {
            if (variable_ptrs.find(pair.first) != variable_ptrs.end() &&
                variable_ptrs.find(pair.second) != variable_ptrs.end()) {
                flippable_variable_ptr_pairs.emplace_back(
                    variable_ptrs[pair.first],  //
                    variable_ptrs[pair.second]);
            }
        }

        m_flippable_variable_ptr_pairs = flippable_variable_ptr_pairs;
    }

    /*********************************************************************/
    void import_mps(const mps::MPS &a_MPS, const bool a_ACCEPT_CONTINUOUS) {
        using VariableMap = std::unordered_map<
            std::string, model_component::Variable<T_Variable, T_Expression> *>;
        using Sensitivities = std::unordered_map<
            model_component::Variable<T_Variable, T_Expression> *,
            T_Expression>;

        VariableMap variable_ptrs;

        auto &variable_proxy =
            this->create_variables("variables", a_MPS.variables.size());

        /**
         * Set up the variables.
         */
        const int NUMBER_OF_VARIABLES = a_MPS.variable_names.size();

        for (auto i = 0; i < NUMBER_OF_VARIABLES; i++) {
            const auto &VARIABLE_NAME = a_MPS.variable_names[i];
            const auto &VARIABLE      = a_MPS.variables.at(VARIABLE_NAME);

            if (VARIABLE.sense == mps::MPSVariableSense::Continuous) {
                if (a_ACCEPT_CONTINUOUS) {
                    utility::print_warning(
                        "The continuous variable " + VARIABLE_NAME +
                            " will be regarded as an integer variable.",
                        true);
                } else {
                    throw std::logic_error(utility::format_error_location(
                        __FILE__, __LINE__, __func__,
                        "The MPS file includes continuous variables."));
                }
            }

            variable_proxy(i).set_bound(VARIABLE.integer_lower_bound,
                                        VARIABLE.integer_upper_bound);

            if (VARIABLE.is_fixed) {
                variable_proxy(i).fix_by(VARIABLE.integer_fixed_value);
            }

            variable_proxy(i).set_name(VARIABLE_NAME);
            variable_ptrs[VARIABLE_NAME] = &variable_proxy(i);
        }

        /**
         * Set up the constraints.
         */
        const int        NUMBER_OF_CONSTRAINTS = a_MPS.constraint_names.size();
        std::vector<int> offsets(NUMBER_OF_CONSTRAINTS);

        auto &constraint_proxy =
            this->create_constraints("constraints", NUMBER_OF_CONSTRAINTS);

#ifdef _OPENMP
#pragma omp parallel for schedule(static)
#endif
        for (auto i = 0; i < NUMBER_OF_CONSTRAINTS; i++) {
            const auto &CONSTRAINT_NAME = a_MPS.constraint_names[i];
            const auto &CONSTRAINT      = a_MPS.constraints.at(CONSTRAINT_NAME);
            auto        expression =
                model_component::Expression<T_Variable,
                                            T_Expression>::create_instance();

            Sensitivities expression_sensitivities;
            for (const auto &sensitivity : CONSTRAINT.sensitivities) {
                const auto &       VARIABLE_NAME = sensitivity.first;
                const T_Expression COEFFICIENT =
                    static_cast<T_Expression>(sensitivity.second);
                expression_sensitivities[variable_ptrs[VARIABLE_NAME]] =
                    COEFFICIENT;
            }
            expression.set_sensitivities(expression_sensitivities);

            switch (CONSTRAINT.sense) {
                case mps::MPSConstraintSense::Less: {
                    constraint_proxy(i) = (expression <= CONSTRAINT.rhs);
                    break;
                }

                case mps::MPSConstraintSense::Equal: {
                    constraint_proxy(i) = (expression == CONSTRAINT.rhs);
                    break;
                }

                case mps::MPSConstraintSense::Greater: {
                    constraint_proxy(i) = (expression >= CONSTRAINT.rhs);
                    break;
                }
            }
            constraint_proxy(i).set_name(CONSTRAINT_NAME);
        }

        /**
         * Set up the objective function.
         */
        auto objective =
            model_component::Expression<T_Variable,
                                        T_Expression>::create_instance();
        Sensitivities objective_sensitivities;
        for (const auto &sensitivity : a_MPS.objective.sensitivities) {
            const std::string  VARIABLE_NAME = sensitivity.first;
            const T_Expression COEFFICIENT =
                static_cast<T_Expression>(sensitivity.second);
            objective_sensitivities[variable_ptrs[VARIABLE_NAME]] = COEFFICIENT;
        }
        objective.set_sensitivities(objective_sensitivities);
        this->minimize(objective);
    }

    /*********************************************************************/
    void write_mps(const std::string &a_FILE_NAME) {
        std::ofstream ofs(a_FILE_NAME);
        /**
         * Determine unique name of variables and constraints.
         */
        this->setup_unique_names();

        /**
         * Determine the linearity.
         */
        this->setup_is_linear();

        if (!m_is_linear) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "Nonlinear model cannot be written in MPS format."));
        }
        /**
         * Determine the sensitivities.
         */
        this->setup_variable_constraint_sensitivities();
        this->setup_variable_objective_sensitivities();

        /**
         * Write instance name.
         */
        if (m_name.empty()) {
            ofs << "NAME untitled" << std::endl;
        } else {
            ofs << "NAME " << utility::delete_space(m_name) << std::endl;
        }
        /**
         * Write ROWS section.
         */
        ofs << "ROWS" << std::endl;
        ofs << " N    obj" << std::endl;
        for (const auto &proxy : m_constraint_proxies) {
            for (const auto &constraint : proxy.flat_indexed_constraints()) {
                switch (constraint.sense()) {
                    case model_component::ConstraintSense::Equal: {
                        ofs << " E    "
                            << utility::delete_space(constraint.name())
                            << std::endl;
                        break;
                    }
                    case model_component::ConstraintSense::Less: {
                        ofs << " L    "
                            << utility::delete_space(constraint.name())
                            << std::endl;
                        break;
                    }
                    case model_component::ConstraintSense::Greater: {
                        ofs << " G    "
                            << utility::delete_space(constraint.name())
                            << std::endl;
                        break;
                    }
                    default: {
                    }
                }
            }
        }

        /**
         * Write COLUMNS section.
         */
        ofs << "COLUMNS" << std::endl;
        ofs << "    MARK0000    'MARKER'    'INTORG'" << std::endl;

        for (const auto &proxy : m_variable_proxies) {
            for (const auto &variable : proxy.flat_indexed_variables()) {
                const auto VARIABLE_NAME =
                    utility::delete_space(variable.name());
                /// Objective
                if (fabs(variable.objective_sensitivity()) >
                    constant::EPSILON_10) {
                    if (m_is_minimization) {
                        ofs << "    " << VARIABLE_NAME << "     obj    "
                            << variable.objective_sensitivity() << std::endl;
                    } else {
                        ofs << "    " << VARIABLE_NAME << "     obj    "
                            << -variable.objective_sensitivity() << std::endl;
                    }
                }
                /// Constraints
                for (const auto &sensitivity :
                     variable.constraint_sensitivities()) {
                    const auto CONSTRAINT_NAME =
                        utility::delete_space(sensitivity.first->name());
                    const auto COEFFICIENT = sensitivity.second;
                    ofs << "    " << VARIABLE_NAME << "    " << CONSTRAINT_NAME
                        << "    " << COEFFICIENT << std::endl;
                }
            }
        }
        ofs << "    MARK0001    'MARKER'    'INTEND'" << std::endl;

        /**
         * Write RHS section.
         */
        ofs << "RHS" << std::endl;
        for (const auto &proxy : m_constraint_proxies) {
            for (const auto &constraint : proxy.flat_indexed_constraints()) {
                const auto CONSTRAINT_NAME =
                    utility::delete_space(constraint.name());
                const auto &EXPRESSION = constraint.expression();
                ofs << "    rhs    " << CONSTRAINT_NAME << "    "
                    << -EXPRESSION.constant_value() << std::endl;
            }
        }

        /**
         * Write Bounds section.
         */
        ofs << "BOUNDS" << std::endl;
        for (const auto &proxy : m_variable_proxies) {
            for (const auto &variable : proxy.flat_indexed_variables()) {
                const auto VARIABLE_NAME =
                    utility::delete_space(variable.name());
                if (variable.is_fixed()) {
                    ofs << "    FX    bnd    " << VARIABLE_NAME << "     "
                        << variable.value() << std::endl;
                } else {
                    if (variable.lower_bound() != constant::INT_HALF_MIN) {
                        if (variable.lower_bound() != 0) {
                            ofs << "    LO    bnd    " << VARIABLE_NAME
                                << "    " << variable.lower_bound()
                                << std::endl;
                        }
                    }
                    if (variable.upper_bound() != constant::INT_HALF_MAX) {
                        ofs << "    UP    bnd    " << VARIABLE_NAME << "    "
                            << variable.upper_bound() << std::endl;
                    }
                }
            }
        }

        /**
         * Write END section.
         */
        ofs << "ENDATA" << std::endl;

        ofs.close();
    }

    /*********************************************************************/
    void write_json(const std::string &a_FILE_NAME) {
        /**
         * Determine unique name of variables and constraints.
         */
        this->setup_unique_names();

        /**
         * Determine the linearity.
         */
        this->setup_is_linear();

        if (!m_is_linear) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "Nonlinear model cannot be written in JSON format."));
        }
        /**
         * Determine the sensitivities.
         */
        this->setup_variable_constraint_sensitivities();
        this->setup_variable_objective_sensitivities();

        utility::json::JsonObject object;

        /**
         * Instance name
         */
        if (m_name.empty()) {
            object.emplace_back("name", "untitled");
        } else {
            object.emplace_back("name", utility::delete_space(m_name));
        }

        /**
         * Minimization or not
         */
        object.emplace_back("is_minimization", m_is_minimization);

        /**
         * Variables
         */
        utility::json::JsonArray variable_array;
        for (const auto &proxy : m_variable_proxies) {
            for (const auto &variable : proxy.flat_indexed_variables()) {
                utility::json::JsonObject v_object;
                v_object.emplace_back("name", variable.name());
                v_object.emplace_back("value", variable.value());
                v_object.emplace_back("is_fixed", variable.is_fixed());
                v_object.emplace_back("lower_bound", variable.lower_bound());
                v_object.emplace_back("upper_bound", variable.upper_bound());
                if (variable.sense() ==
                    model_component::VariableSense::Binary) {
                    v_object.emplace_back("sense", std::string("Binary"));
                } else {
                    v_object.emplace_back("sense", std::string("Integer"));
                }
                variable_array.emplace_back(v_object);
            }
        }
        object.emplace_back("variables", variable_array);

        /**
         * Objective
         */
        utility::json::JsonObject objective_object;
        utility::json::JsonArray  objective_sensitivity_array;
        for (const auto &sensitivity :
             m_objective.expression().sensitivities()) {
            utility::json::JsonObject sensitivity_object;
            sensitivity_object.emplace_back("variable",
                                            sensitivity.first->name());
            sensitivity_object.emplace_back("coefficient", sensitivity.second);
            objective_sensitivity_array.emplace_back(sensitivity_object);
        }
        objective_object.emplace_back("sensitivities",
                                      objective_sensitivity_array);
        object.emplace_back("objective", objective_object);

        /**
         * Constraints
         */
        utility::json::JsonArray constraint_array;
        for (const auto &proxy : m_constraint_proxies) {
            for (const auto &constraint : proxy.flat_indexed_constraints()) {
                utility::json::JsonObject c_object;

                const auto &EXPRESSION    = constraint.expression();
                const auto &SENSITIVITIES = EXPRESSION.sensitivities();
                c_object.emplace_back("name", constraint.name());
                c_object.emplace_back("is_enabled", constraint.is_enabled());
                c_object.emplace_back("rhs", -EXPRESSION.constant_value());
                switch (constraint.sense()) {
                    case model_component::ConstraintSense::Equal: {
                        c_object.emplace_back("sense", std::string("="));
                        break;
                    }
                    case model_component::ConstraintSense::Less: {
                        c_object.emplace_back("sense", std::string("<="));
                        break;
                    }
                    case model_component::ConstraintSense::Greater: {
                        c_object.emplace_back("sense", std::string(">="));
                        break;
                    }
                    default: {
                    }
                }

                utility::json::JsonArray constraint_sensitivity_array;
                for (const auto &sensitivity : SENSITIVITIES) {
                    utility::json::JsonObject sensitivity_object;
                    sensitivity_object.emplace_back("variable",
                                                    sensitivity.first->name());
                    sensitivity_object.emplace_back("coefficient",
                                                    sensitivity.second);
                    constraint_sensitivity_array.emplace_back(
                        sensitivity_object);
                }
                c_object.emplace_back("sensitivities",
                                      constraint_sensitivity_array);
                constraint_array.emplace_back(c_object);
            }
        }

        object.emplace_back("constraints", constraint_array);
        utility::json::write_json_object(object, a_FILE_NAME);
    }

    /*************************************************************************/
    inline constexpr std::vector<
        model_component::VariableProxy<T_Variable, T_Expression>>
        &variable_proxies(void) {
        return m_variable_proxies;
    }

    /*************************************************************************/
    inline constexpr const std::vector<
        model_component::VariableProxy<T_Variable, T_Expression>>
        &variable_proxies(void) const {
        return m_variable_proxies;
    }

    /*************************************************************************/
    inline constexpr std::vector<
        model_component::ExpressionProxy<T_Variable, T_Expression>>
        &expression_proxies(void) {
        return m_expression_proxies;
    }

    /*************************************************************************/
    inline constexpr const std::vector<
        model_component::ExpressionProxy<T_Variable, T_Expression>>
        &expression_proxies(void) const {
        return m_expression_proxies;
    }

    /*************************************************************************/
    inline constexpr std::vector<
        model_component::ConstraintProxy<T_Variable, T_Expression>>
        &constraint_proxies(void) {
        return m_constraint_proxies;
    }

    /*************************************************************************/
    inline constexpr const std::vector<
        model_component::ConstraintProxy<T_Variable, T_Expression>>
        &constraint_proxies(void) const {
        return m_constraint_proxies;
    }

    /*************************************************************************/
    inline constexpr model_component::Objective<T_Variable, T_Expression>
        &objective(void) {
        return m_objective;
    }

    /*************************************************************************/
    inline constexpr const model_component::Objective<T_Variable, T_Expression>
        &objective(void) const {
        return m_objective;
    }

    /*************************************************************************/
    inline const std::unordered_map<
        model_component::Variable<T_Variable, T_Expression> *,
        model_component::Expression<T_Variable, T_Expression> *>
        &dependent_expression_map(void) const {
        return m_dependent_expression_map;
    }

    /*************************************************************************/
    inline std::unordered_map<
        model_component::Variable<T_Variable, T_Expression> *,
        model_component::Expression<T_Variable, T_Expression> *>
        &dependent_expression_map(void) {
        return m_dependent_expression_map;
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
    inline constexpr const std::vector<
        model_component::Selection<T_Variable, T_Expression>>
        &selections(void) const {
        return m_selections;
    }

    /*************************************************************************/
    inline constexpr model_component::VariableReference<T_Variable,
                                                        T_Expression>
        &variable_reference(void) {
        return m_variable_reference;
    }

    /*************************************************************************/
    inline constexpr const model_component::VariableReference<T_Variable,
                                                              T_Expression>
        &variable_reference(void) const {
        return m_variable_reference;
    }

    /*************************************************************************/
    inline constexpr model_component::VariableReference<T_Variable,
                                                        T_Expression>
        &variable_reference_original(void) {
        return m_variable_reference_original;
    }

    /*************************************************************************/
    inline constexpr const model_component::VariableReference<T_Variable,
                                                              T_Expression>
        &variable_reference_original(void) const {
        return m_variable_reference_original;
    }

    /*************************************************************************/
    inline constexpr model_component::VariableTypeReference<T_Variable,
                                                            T_Expression>
        &variable_type_reference(void) {
        return m_variable_type_reference;
    }

    /*************************************************************************/
    inline constexpr const model_component::VariableTypeReference<T_Variable,
                                                                  T_Expression>
        &variable_type_reference(void) const {
        return m_variable_type_reference;
    }

    /*************************************************************************/
    inline constexpr model_component::VariableTypeReference<T_Variable,
                                                            T_Expression>
        &variable_type_reference_original(void) {
        return m_variable_type_reference_original;
    }

    /*************************************************************************/
    inline constexpr const model_component::VariableTypeReference<T_Variable,
                                                                  T_Expression>
        &variable_type_reference_original(void) const {
        return m_variable_type_reference_original;
    }

    /*************************************************************************/
    inline constexpr model_component::ConstraintReference<T_Variable,
                                                          T_Expression>
        &constraint_reference(void) {
        return m_constraint_reference;
    }

    /*************************************************************************/
    inline constexpr const model_component::ConstraintReference<T_Variable,
                                                                T_Expression>
        &constraint_reference(void) const {
        return m_constraint_reference;
    }

    /*************************************************************************/
    inline constexpr model_component::ConstraintReference<T_Variable,
                                                          T_Expression>
        &constraint_reference_original(void) {
        return m_constraint_reference_original;
    }

    /*************************************************************************/
    inline constexpr const model_component::ConstraintReference<T_Variable,
                                                                T_Expression>
        &constraint_reference_original(void) const {
        return m_constraint_reference_original;
    }

    /*************************************************************************/
    inline constexpr model_component::ConstraintTypeReference<T_Variable,
                                                              T_Expression>
        &constraint_type_reference(void) {
        return m_constraint_type_reference;
    }

    /*************************************************************************/
    inline constexpr const model_component::ConstraintTypeReference<
        T_Variable, T_Expression>
        &constraint_type_reference(void) const {
        return m_constraint_type_reference;
    }

    /*************************************************************************/
    inline constexpr model_component::ConstraintTypeReference<T_Variable,
                                                              T_Expression>
        &constraint_type_reference_original(void) {
        return m_constraint_type_reference_original;
    }

    /*************************************************************************/
    inline constexpr const model_component::ConstraintTypeReference<
        T_Variable, T_Expression>
        &constraint_type_reference_original(void) const {
        return m_constraint_type_reference_original;
    }

    /*************************************************************************/
    inline constexpr bool is_defined_objective(void) const {
        return m_is_defined_objective;
    }

    /*************************************************************************/
    inline constexpr void disable_fast_evaluation(void) {
        m_is_enabled_fast_evaluation = false;
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
         * problems by nagating the objective function values. This method
         * is used to show objective function values for output.
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
    inline constexpr const std::vector<
        model_component::Constraint<T_Variable, T_Expression> *>
        &violative_constraint_ptrs(void) const {
        return m_violative_constraint_ptrs;
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
    inline constexpr int number_of_mutable_variables(void) const {
        return m_variable_reference.mutable_variable_ptrs.size();
    }

    /*************************************************************************/
    inline constexpr int number_of_binary_variables(void) const {
        return m_variable_type_reference.binary_variable_ptrs.size();
    }

    /*************************************************************************/
    inline constexpr int number_of_integer_variables(void) const {
        return m_variable_type_reference.integer_variable_ptrs.size();
    }

    /*************************************************************************/
    inline constexpr int number_of_selection_variables(void) const {
        return m_variable_type_reference.selection_variable_ptrs.size();
    }

    /*************************************************************************/
    inline constexpr int number_of_dependent_binary_variables(void) const {
        return m_variable_type_reference.dependent_binary_variable_ptrs.size();
    }

    /*************************************************************************/
    inline constexpr int number_of_dependent_integer_variables(void) const {
        return m_variable_type_reference.dependent_integer_variable_ptrs.size();
    }

    /*************************************************************************/
    inline constexpr int number_of_constraints(void) const {
        return m_constraint_reference.constraint_ptrs.size();
    }

    /*************************************************************************/
    inline constexpr int number_of_selection_constraints(void) const {
        return m_selections.size();
    }

    /*************************************************************************/
    inline constexpr int number_of_enabled_constraints(void) const {
        return m_constraint_reference.enabled_constraint_ptrs.size();
    }

    /*************************************************************************/
    inline constexpr int number_of_disabled_constraints(void) const {
        return m_constraint_reference.disabled_constraint_ptrs.size();
    }

    /*************************************************************************/
    inline constexpr bool has_chain_move_effective_constraints(void) const {
        if (m_constraint_type_reference.set_partitioning_ptrs.size() > 0) {
            return true;
        }
        if (m_constraint_type_reference.set_packing_ptrs.size() > 0) {
            return true;
        }
        if (m_constraint_type_reference.set_covering_ptrs.size() > 0) {
            return true;
        }
        if (m_constraint_type_reference.cardinality_ptrs.size() > 0) {
            return true;
        }
        if (m_constraint_type_reference.invariant_knapsack_ptrs.size() > 0) {
            return true;
        }
        if (m_constraint_type_reference.multiple_covering_ptrs.size() > 0) {
            return true;
        }
        return false;
    }

    /*************************************************************************/
    inline constexpr std::vector<
        std::pair<model_component::Variable<T_Variable, T_Expression> *,
                  model_component::Variable<T_Variable, T_Expression> *>>
        &flippable_variable_ptr_pairs(void) {
        return m_flippable_variable_ptr_pairs;
    }

    /*************************************************************************/
    inline constexpr neighborhood::Neighborhood<T_Variable, T_Expression>
        &neighborhood(void) {
        return m_neighborhood;
    }

    /*************************************************************************/
    inline constexpr preprocess::ProblemSizeReducer<T_Variable, T_Expression>
        &problem_size_reducer(void) {
        return m_problem_size_reducer;
    }
};
using IPModel = Model<int, double>;
}  // namespace printemps::model
#endif
/*****************************************************************************/
// END
/*****************************************************************************/