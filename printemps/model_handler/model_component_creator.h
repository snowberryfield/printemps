/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MODEL_HANDLER_MODEL_COMPONENT_CREATOR_H__
#define PRINTEMPS_MODEL_HANDLER_MODEL_COMPONENT_CREATOR_H__

namespace printemps::model_handler {
/*****************************************************************************/
struct ModelComponentCreatorConstant {
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
class ModelComponentCreator {
   private:
    model::Model<T_Variable, T_Expression> *m_model_ptr;

   public:
    /*************************************************************************/
    ModelComponentCreator(void) {
        m_model_ptr = nullptr;
    }

    /*************************************************************************/
    ModelComponentCreator(model::Model<T_Variable, T_Expression> *a_model_ptr) {
        this->initialize();
        this->setup(a_model_ptr);
    }

    /*************************************************************************/
    inline void initialize(void) noexcept {
        m_model_ptr = nullptr;
    }

    /*************************************************************************/
    inline void setup(
        model::Model<T_Variable, T_Expression> *a_model_ptr) noexcept {
        m_model_ptr = a_model_ptr;
    }
    /*************************************************************************/
    inline model_component::VariableProxy<T_Variable, T_Expression> &
    create_variable(const std::string &a_NAME) {
        if (utility::has_space(a_NAME)) {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The name of variable must not contain spaces."));
        }

        auto     &model       = *m_model_ptr;
        const int PROXY_INDEX = model.m_variable_proxies.size();

        if (PROXY_INDEX >=
            ModelComponentCreatorConstant::MAX_NUMBER_OF_VARIABLE_PROXIES) {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of variable definitions must be equal to or less "
                "than " +
                    std::to_string(ModelComponentCreatorConstant::
                                       MAX_NUMBER_OF_VARIABLE_PROXIES) +
                    "."));
        }

        model.m_variable_proxies.emplace_back(
            model_component::VariableProxy<
                T_Variable, T_Expression>::create_instance(PROXY_INDEX));
        model.m_variable_names.push_back(a_NAME);

        return model.m_variable_proxies.back();
    }

    /*************************************************************************/
    inline model_component::VariableProxy<T_Variable, T_Expression> &
    create_variable(const std::string &a_NAME,         //
                    const T_Variable   a_LOWER_BOUND,  //
                    const T_Variable   a_UPPER_BOUND) {
        auto &model          = *m_model_ptr;
        auto &variable_proxy = this->create_variable(a_NAME);
        variable_proxy.set_bound(a_LOWER_BOUND, a_UPPER_BOUND);

        return model.m_variable_proxies.back();
    }

    /*************************************************************************/
    inline model_component::VariableProxy<T_Variable, T_Expression> &
    create_variables(const std::string &a_NAME,  //
                     const int          a_NUMBER_OF_ELEMENTS) {
        if (utility::has_space(a_NAME)) {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The name of variable must not contain spaces."));
        }

        auto     &model       = *m_model_ptr;
        const int PROXY_INDEX = model.m_variable_proxies.size();

        if (PROXY_INDEX >=
            ModelComponentCreatorConstant::MAX_NUMBER_OF_VARIABLE_PROXIES) {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of variable definitions must be equal to or less "
                "than " +
                    std::to_string(ModelComponentCreatorConstant::
                                       MAX_NUMBER_OF_VARIABLE_PROXIES) +
                    "."));
        }

        model.m_variable_proxies.emplace_back(
            model_component::VariableProxy<T_Variable, T_Expression>::
                create_instance(PROXY_INDEX, a_NUMBER_OF_ELEMENTS));
        model.m_variable_names.push_back(a_NAME);

        return model.m_variable_proxies.back();
    }

    /*************************************************************************/
    inline model_component::VariableProxy<T_Variable, T_Expression> &
    create_variables(const std::string &a_NAME,                //
                     const int          a_NUMBER_OF_ELEMENTS,  //
                     const T_Variable   a_LOWER_BOUND,         //
                     const T_Variable   a_UPPER_BOUND) {
        auto &variable_proxy =
            this->create_variables(a_NAME, a_NUMBER_OF_ELEMENTS);
        variable_proxy.set_bound(a_LOWER_BOUND, a_UPPER_BOUND);

        auto &model = *m_model_ptr;
        return model.m_variable_proxies.back();
    }

    /*************************************************************************/
    inline model_component::VariableProxy<T_Variable, T_Expression> &
    create_variables(const std::string      &a_NAME,  //
                     const std::vector<int> &a_SHAPE) {
        if (utility::has_space(a_NAME)) {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The name of variable must not contain spaces."));
        }

        auto     &model       = *m_model_ptr;
        const int PROXY_INDEX = model.m_variable_proxies.size();

        if (PROXY_INDEX >=
            ModelComponentCreatorConstant::MAX_NUMBER_OF_VARIABLE_PROXIES) {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of variable definitions must be equal to or less "
                "than " +
                    std::to_string(ModelComponentCreatorConstant::
                                       MAX_NUMBER_OF_VARIABLE_PROXIES) +
                    "."));
        }

        model.m_variable_proxies.emplace_back(
            model_component::VariableProxy<
                T_Variable, T_Expression>::create_instance(PROXY_INDEX,
                                                           a_SHAPE));
        model.m_variable_names.push_back(a_NAME);

        return model.m_variable_proxies.back();
    }

    /*************************************************************************/
    inline model_component::VariableProxy<T_Variable, T_Expression> &
    create_variables(const std::string      &a_NAME,         //
                     const std::vector<int> &a_SHAPE,        //
                     const T_Variable        a_LOWER_BOUND,  //
                     const T_Variable        a_UPPER_BOUND) {
        auto &variable_proxy = this->create_variables(a_NAME, a_SHAPE);
        variable_proxy.set_bound(a_LOWER_BOUND, a_UPPER_BOUND);

        auto &model = *m_model_ptr;
        return model.m_variable_proxies.back();
    }

    /*************************************************************************/
    inline model_component::ExpressionProxy<T_Variable, T_Expression> &
    create_expression(const std::string &a_NAME) {
        if (utility::has_space(a_NAME)) {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The name of expression must not contain spaces."));
        }

        auto     &model       = *m_model_ptr;
        const int PROXY_INDEX = model.m_expression_proxies.size();

        if (PROXY_INDEX >=
            ModelComponentCreatorConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES) {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of expression definitions must be equal to or "
                "less than " +
                    std::to_string(ModelComponentCreatorConstant::
                                       MAX_NUMBER_OF_EXPRESSION_PROXIES) +
                    "."));
        }

        model.m_expression_proxies.emplace_back(
            model_component::ExpressionProxy<
                T_Variable, T_Expression>::create_instance(PROXY_INDEX));
        model.m_expression_names.push_back(a_NAME);

        return model.m_expression_proxies.back();
    }

    /*************************************************************************/
    inline model_component::ExpressionProxy<T_Variable, T_Expression> &
    create_expressions(const std::string &a_NAME,  //
                       int                a_NUMBER_OF_ELEMENTS) {
        if (utility::has_space(a_NAME)) {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The name of expression must not contain spaces."));
        }

        auto     &model       = *m_model_ptr;
        const int PROXY_INDEX = model.m_expression_proxies.size();

        if (PROXY_INDEX >=
            ModelComponentCreatorConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES) {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of expression definitions must be equal to or "
                "less than " +
                    std::to_string(ModelComponentCreatorConstant::
                                       MAX_NUMBER_OF_EXPRESSION_PROXIES) +
                    "."));
        }

        model.m_expression_proxies.emplace_back(
            model_component::ExpressionProxy<T_Variable, T_Expression>::
                create_instance(PROXY_INDEX, a_NUMBER_OF_ELEMENTS));
        model.m_expression_names.push_back(a_NAME);

        return model.m_expression_proxies.back();
    }

    /*************************************************************************/
    inline model_component::ExpressionProxy<T_Variable, T_Expression> &
    create_expressions(const std::string      &a_NAME,  //
                       const std::vector<int> &a_SHAPE) {
        if (utility::has_space(a_NAME)) {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The name of expression must not contain spaces."));
        }

        auto     &model       = *m_model_ptr;
        const int PROXY_INDEX = model.m_expression_proxies.size();

        if (PROXY_INDEX >=
            ModelComponentCreatorConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES) {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of expression definitions must be equal to or "
                "less than " +
                    std::to_string(ModelComponentCreatorConstant::
                                       MAX_NUMBER_OF_EXPRESSION_PROXIES) +
                    "."));
        }

        model.m_expression_proxies.emplace_back(
            model_component::ExpressionProxy<
                T_Variable, T_Expression>::create_instance(PROXY_INDEX,
                                                           a_SHAPE));
        model.m_expression_names.push_back(a_NAME);

        return model.m_expression_proxies.back();
    }

    /*************************************************************************/
    template <template <class, class> class T_ExpressionLike>
    inline model_component::ExpressionProxy<T_Variable, T_Expression> &
    create_expression(
        const std::string                                &a_NAME,  //
        const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE) {
        if (utility::has_space(a_NAME)) {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The name of expression must not contain spaces."));
        }

        auto     &model       = *m_model_ptr;
        const int PROXY_INDEX = model.m_expression_proxies.size();

        if (PROXY_INDEX >=
            ModelComponentCreatorConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES) {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of expression definitions must be equal to or "
                "less than " +
                    std::to_string(ModelComponentCreatorConstant::
                                       MAX_NUMBER_OF_EXPRESSION_PROXIES) +
                    "."));
        }

        model.m_expression_proxies.emplace_back(
            model_component::ExpressionProxy<
                T_Variable, T_Expression>::create_instance(PROXY_INDEX));
        model.m_expression_names.push_back(a_NAME);
        model.m_expression_proxies.back() = a_EXPRESSION_LIKE.to_expression();

        return model.m_expression_proxies.back();
    }

    /*************************************************************************/
    inline model_component::ExpressionProxy<T_Variable, T_Expression> &
    create_expression(
        const std::string &a_NAME,  //
        const model_component::Expression<T_Variable, T_Expression>
            &a_EXPRESSION) {
        if (utility::has_space(a_NAME)) {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The name of expression must not contain spaces."));
        }

        auto     &model       = *m_model_ptr;
        const int PROXY_INDEX = model.m_expression_proxies.size();

        if (PROXY_INDEX >=
            ModelComponentCreatorConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES) {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of expression definitions must be equal to or "
                "less than " +
                    std::to_string(ModelComponentCreatorConstant::
                                       MAX_NUMBER_OF_EXPRESSION_PROXIES) +
                    "."));
        }

        model.m_expression_proxies.emplace_back(
            model_component::ExpressionProxy<
                T_Variable, T_Expression>::create_instance(PROXY_INDEX));
        model.m_expression_names.push_back(a_NAME);
        model.m_expression_proxies.back() = a_EXPRESSION;

        return model.m_expression_proxies.back();
    }

    /*************************************************************************/
    inline model_component::ConstraintProxy<T_Variable, T_Expression> &
    create_constraint(const std::string &a_NAME) {
        if (utility::has_space(a_NAME)) {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The name of constraint must not contain spaces."));
        }

        auto     &model       = *m_model_ptr;
        const int PROXY_INDEX = model.m_constraint_proxies.size();

        if (PROXY_INDEX >=
            ModelComponentCreatorConstant::MAX_NUMBER_OF_CONSTRAINT_PROXIES) {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of constraint definitions must be equal to or "
                "less than " +
                    std::to_string(ModelComponentCreatorConstant::
                                       MAX_NUMBER_OF_CONSTRAINT_PROXIES) +
                    "."));
        }

        model.m_constraint_proxies.emplace_back(
            model_component::ConstraintProxy<
                T_Variable, T_Expression>::create_instance(PROXY_INDEX));
        model.m_constraint_names.push_back(a_NAME);

        return model.m_constraint_proxies.back();
    }

    /*************************************************************************/
    inline model_component::ConstraintProxy<T_Variable, T_Expression> &
    create_constraints(const std::string &a_NAME,  //
                       int                a_NUMBER_OF_ELEMENTS) {
        if (utility::has_space(a_NAME)) {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The name of constraint must not contain spaces."));
        }

        auto     &model       = *m_model_ptr;
        const int PROXY_INDEX = model.m_constraint_proxies.size();

        if (PROXY_INDEX >=
            ModelComponentCreatorConstant::MAX_NUMBER_OF_CONSTRAINT_PROXIES) {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of constraint definitions must be equal to or "
                "less than " +
                    std::to_string(ModelComponentCreatorConstant::
                                       MAX_NUMBER_OF_CONSTRAINT_PROXIES) +
                    "."));
        }

        model.m_constraint_proxies.emplace_back(
            model_component::ConstraintProxy<T_Variable, T_Expression>::
                create_instance(PROXY_INDEX, a_NUMBER_OF_ELEMENTS));
        model.m_constraint_names.push_back(a_NAME);

        return model.m_constraint_proxies.back();
    }

    /*************************************************************************/
    inline model_component::ConstraintProxy<T_Variable, T_Expression> &
    create_constraints(const std::string      &a_NAME,  //
                       const std::vector<int> &a_SHAPE) {
        if (utility::has_space(a_NAME)) {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The name of constraint must not contain spaces."));
        }

        auto     &model       = *m_model_ptr;
        const int PROXY_INDEX = model.m_constraint_proxies.size();

        if (PROXY_INDEX >=
            ModelComponentCreatorConstant::MAX_NUMBER_OF_CONSTRAINT_PROXIES) {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of constraint definitions must be equal to or "
                "less than " +
                    std::to_string(ModelComponentCreatorConstant::
                                       MAX_NUMBER_OF_CONSTRAINT_PROXIES) +
                    "."));
        }

        model.m_constraint_proxies.emplace_back(
            model_component::ConstraintProxy<
                T_Variable, T_Expression>::create_instance(PROXY_INDEX,
                                                           a_SHAPE));
        model.m_constraint_names.push_back(a_NAME);

        return model.m_constraint_proxies.back();
    }

    /*************************************************************************/
    inline model_component::ConstraintProxy<T_Variable, T_Expression> &
    create_constraint(
        const std::string &a_NAME,  //
        const model_component::Constraint<T_Variable, T_Expression>
            &a_CONSTRAINT) {
        if (utility::has_space(a_NAME)) {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The name of constraint must not contain spaces."));
        }

        auto     &model       = *m_model_ptr;
        const int PROXY_INDEX = model.m_constraint_proxies.size();

        if (PROXY_INDEX >=
            ModelComponentCreatorConstant::MAX_NUMBER_OF_CONSTRAINT_PROXIES) {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of constraint definitions must be equal to or "
                "less than " +
                    std::to_string(ModelComponentCreatorConstant::
                                       MAX_NUMBER_OF_CONSTRAINT_PROXIES) +
                    "."));
        }

        model.m_constraint_proxies.emplace_back(
            model_component::ConstraintProxy<
                T_Variable, T_Expression>::create_instance(PROXY_INDEX));
        model.m_constraint_names.push_back(a_NAME);
        model.m_constraint_proxies.back() = a_CONSTRAINT;

        return model.m_constraint_proxies.back();
    }

    /*************************************************************************/
    template <template <class, class> class T_ExpressionLike>
    inline void minimize(
        const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE) {
        auto objective = model_component::Objective<T_Variable, T_Expression>::
            create_instance(a_EXPRESSION_LIKE.to_expression());

        auto &model                  = *m_model_ptr;
        model.m_objective            = objective;
        model.m_is_defined_objective = true;
        model.m_is_minimization      = true;
        model.m_sign                 = 1.0;
    }

    /*************************************************************************/
    inline void minimize(
        const model_component::Expression<T_Variable, T_Expression>
            &a_EXPRESSION) {
        auto objective = model_component::Objective<
            T_Variable, T_Expression>::create_instance(a_EXPRESSION);

        auto &model                  = *m_model_ptr;
        model.m_objective            = objective;
        model.m_is_defined_objective = true;
        model.m_is_minimization      = true;
        model.m_sign                 = 1.0;
    }

    /*************************************************************************/
    template <template <class, class> class T_ExpressionLike>
    inline void maximize(
        const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE) {
        auto objective = model_component::Objective<T_Variable, T_Expression>::
            create_instance(a_EXPRESSION_LIKE.to_expression());

        auto &model                  = *m_model_ptr;
        model.m_objective            = objective;
        model.m_is_defined_objective = true;
        model.m_is_minimization      = false;
        model.m_sign                 = -1.0;
    }

    /*************************************************************************/
    inline void maximize(
        const model_component::Expression<T_Variable, T_Expression>
            &a_EXPRESSION) {
        auto objective = model_component::Objective<
            T_Variable, T_Expression>::create_instance(a_EXPRESSION);

        auto &model                  = *m_model_ptr;
        model.m_objective            = objective;
        model.m_is_defined_objective = true;
        model.m_is_minimization      = false;
        model.m_sign                 = -1.0;
    }
};
}  // namespace printemps::model_handler
#endif
/*****************************************************************************/
// END
/*****************************************************************************/