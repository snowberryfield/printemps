/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MODEL_HANDLER_MODEL_OPB_HANDLER_H__
#define PRINTEMPS_MODEL_HANDLER_MODEL_OPB_HANDLER_H__

namespace printemps::model_handler {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class ModelOPBHandler {
   private:
    model::Model<T_Variable, T_Expression> *m_model_ptr;

   public:
    /*************************************************************************/
    ModelOPBHandler(void) {
        m_model_ptr = nullptr;
    }

    /*************************************************************************/
    ModelOPBHandler(model::Model<T_Variable, T_Expression> *a_model_ptr) {
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

    /*********************************************************************/
    void import(const opb::OPB &a_OPB) {
        auto &model = *m_model_ptr;

        using VariableMap = std::unordered_map<
            std::string, model_component::Variable<T_Variable, T_Expression> *>;
        using Sensitivities = std::unordered_map<
            model_component::Variable<T_Variable, T_Expression> *,
            T_Expression>;

        VariableMap variable_ptrs;
        model.set_is_minimization(a_OPB.objective.is_minimization);

        /**
         * Set up variables.
         */
        const int NUMBER_OF_VARIABLES = a_OPB.variable_names.size();
        auto     &variable_proxy = model.component_creator().create_variables(
            "variables", NUMBER_OF_VARIABLES);
        for (auto i = 0; i < NUMBER_OF_VARIABLES; i++) {
            const auto &VARIABLE_NAME = a_OPB.variable_names[i];
            variable_proxy(i).set_bound(0, 1);
            variable_proxy(i).set_name(VARIABLE_NAME);
            variable_ptrs[VARIABLE_NAME] = &variable_proxy(i);
        }

        const int NUMBER_OF_NEGATED_VARIABLES =
            a_OPB.negated_variable_names.size();
        auto &negated_variable_proxy =
            model.component_creator().create_variables(
                "negated_variables", NUMBER_OF_NEGATED_VARIABLES);
        for (auto i = 0; i < NUMBER_OF_NEGATED_VARIABLES; i++) {
            const auto &NEGATED_VARIABLE_NAME =
                "~" + a_OPB.negated_variable_names[i];
            negated_variable_proxy(i).set_bound(0, 1);
            negated_variable_proxy(i).set_name(NEGATED_VARIABLE_NAME);
            variable_ptrs[NEGATED_VARIABLE_NAME] = &negated_variable_proxy(i);
        }

        const int NUMBER_OF_PRODUCT_VARIABLES =
            a_OPB.product_variable_names.size();
        auto &product_variable_proxy =
            model.component_creator().create_variables(
                "product_variables", NUMBER_OF_PRODUCT_VARIABLES);

        for (auto i = 0; i < NUMBER_OF_PRODUCT_VARIABLES; i++) {
            const auto &PRODUCT_VARIABLE_NAME =
                a_OPB.product_variable_names[i].first;
            product_variable_proxy(i).set_bound(0, 1);
            product_variable_proxy(i).set_name(PRODUCT_VARIABLE_NAME);
            variable_ptrs[PRODUCT_VARIABLE_NAME] = &product_variable_proxy(i);
        }

        int number_of_soft_constraints_less    = 0;
        int number_of_soft_constraints_equal   = 0;
        int number_of_soft_constraints_greater = 0;

        std::unordered_map<int, int> soft_constraint_indices_less;
        std::unordered_map<int, int> soft_constraint_indices_equal;
        std::unordered_map<int, int> soft_constraint_indices_greater;

        const int NUMBER_OF_SOFT_CONSTRAINTS = a_OPB.soft_constraints.size();

        for (auto i = 0; i < NUMBER_OF_SOFT_CONSTRAINTS; i++) {
            const auto &SOFT_CONSTRAINT = a_OPB.soft_constraints[i];
            switch (SOFT_CONSTRAINT.sense) {
                case opb::OPBConstraintSense::Less: {
                    soft_constraint_indices_less[i] =
                        number_of_soft_constraints_less++;
                    break;
                }
                case opb::OPBConstraintSense::Equal: {
                    soft_constraint_indices_equal[i] =
                        number_of_soft_constraints_equal++;
                    break;
                }
                case opb::OPBConstraintSense::Greater: {
                    soft_constraint_indices_greater[i] =
                        number_of_soft_constraints_greater++;
                    break;
                }
            }
        }

        auto &soft_constraint_slack_proxy_less_plus =
            model.component_creator().create_variables(
                "slack_variables_less_plus", number_of_soft_constraints_less);

        for (auto i = 0; i < number_of_soft_constraints_less; i++) {
            const auto &SLACK_VARIABLE_NAME_LESS_PLUS =
                a_OPB.soft_constraints[i].name + "_less_plus";
            soft_constraint_slack_proxy_less_plus(i).set_bound(0, 1);
            soft_constraint_slack_proxy_less_plus(i).set_name(
                SLACK_VARIABLE_NAME_LESS_PLUS);
            variable_ptrs[SLACK_VARIABLE_NAME_LESS_PLUS] =
                &soft_constraint_slack_proxy_less_plus(i);
        }

        auto &soft_constraint_slack_proxy_equal_plus =
            model.component_creator().create_variables(
                "slack_variables_equal_plus", number_of_soft_constraints_equal);
        auto &soft_constraint_slack_proxy_equal_minus =
            model.component_creator().create_variables(
                "slack_variables_equal_minus",
                number_of_soft_constraints_equal);

        for (auto i = 0; i < number_of_soft_constraints_equal; i++) {
            const auto &SLACK_VARIABLE_NAME_EQUAL_PLUS =
                a_OPB.soft_constraints[i].name + "_equal_plus";
            soft_constraint_slack_proxy_equal_plus(i).set_bound(0, 1);
            soft_constraint_slack_proxy_equal_plus(i).set_name(
                SLACK_VARIABLE_NAME_EQUAL_PLUS);
            variable_ptrs[SLACK_VARIABLE_NAME_EQUAL_PLUS] =
                &soft_constraint_slack_proxy_equal_plus(i);

            const auto &SLACK_VARIABLE_NAME_EQUAL_MINUS =
                a_OPB.soft_constraints[i].name + "_equal_minus";
            soft_constraint_slack_proxy_equal_minus(i).set_bound(0, 1);
            soft_constraint_slack_proxy_equal_minus(i).set_name(
                SLACK_VARIABLE_NAME_EQUAL_MINUS);
            variable_ptrs[SLACK_VARIABLE_NAME_EQUAL_MINUS] =
                &soft_constraint_slack_proxy_equal_minus(i);
        }

        auto &soft_constraint_slack_proxy_greater_minus =
            model.component_creator().create_variables(
                "slack_variables_greater_minus",
                number_of_soft_constraints_greater);

        for (auto i = 0; i < number_of_soft_constraints_greater; i++) {
            const auto &SLACK_VARIABLE_NAME_GREATER_MINUS =
                a_OPB.soft_constraints[i].name + "_greater_minus";
            soft_constraint_slack_proxy_greater_minus(i).set_bound(0, 1);
            soft_constraint_slack_proxy_greater_minus(i).set_name(
                SLACK_VARIABLE_NAME_GREATER_MINUS);
            variable_ptrs[SLACK_VARIABLE_NAME_GREATER_MINUS] =
                &soft_constraint_slack_proxy_greater_minus(i);
        }

        /**
         * Set up soft constraints.
         */
        auto &soft_constraint_proxy_less_plus =
            model.component_creator().create_constraints(
                "soft_constraint_less_plus", number_of_soft_constraints_less);
        auto &soft_constraint_proxy_equal_plus =
            model.component_creator().create_constraints(
                "soft_constraint_equal_plus", number_of_soft_constraints_equal);
        auto &soft_constraint_proxy_equal_minus =
            model.component_creator().create_constraints(
                "soft_constraint_equal_minus",
                number_of_soft_constraints_equal);

        auto &soft_constraint_proxy_greater_minus =
            model.component_creator().create_constraints(
                "soft_constraint_greater_minus",
                number_of_soft_constraints_greater);
        auto objective_penalty =
            model_component::Expression<T_Variable,
                                        T_Expression>::create_instance();

#ifdef _OPENMP
#pragma omp parallel for schedule(static)
#endif
        for (auto i = 0; i < NUMBER_OF_SOFT_CONSTRAINTS; i++) {
            const auto &SOFT_CONSTRAINT = a_OPB.soft_constraints[i];
            auto        expression =
                model_component::Expression<T_Variable,
                                            T_Expression>::create_instance();

            Sensitivities expression_sensitivities;
            for (const auto &term : SOFT_CONSTRAINT.terms) {
                const auto &VARIABLE_NAME = term.concated_variable_name();
                const T_Expression COEFFICIENT =
                    static_cast<T_Expression>(term.coefficient);
                expression_sensitivities[variable_ptrs[VARIABLE_NAME]] =
                    COEFFICIENT;
            }
            expression.set_sensitivities(expression_sensitivities);
            expression -= SOFT_CONSTRAINT.rhs;
            const auto LOWER_BOUND = expression.lower_bound();
            const auto UPPER_BOUND = expression.upper_bound();

            switch (SOFT_CONSTRAINT.sense) {
                case opb::OPBConstraintSense::Less: {
                    const auto INDEX = soft_constraint_indices_less[i];
                    if (SOFT_CONSTRAINT.is_all_coefficient_negative()) {
                        soft_constraint_proxy_less_plus(INDEX) =
                            (-expression >=
                             -UPPER_BOUND *
                                 soft_constraint_slack_proxy_less_plus(INDEX));
                    } else {
                        soft_constraint_proxy_less_plus(INDEX) =
                            (expression <=
                             UPPER_BOUND *
                                 soft_constraint_slack_proxy_less_plus(INDEX));
                    }
                    soft_constraint_proxy_less_plus(INDEX).set_name(
                        SOFT_CONSTRAINT.name + "_less_plus");

                    break;
                }

                case opb::OPBConstraintSense::Equal: {
                    const auto INDEX = soft_constraint_indices_equal[i];
                    if (SOFT_CONSTRAINT.is_all_coefficient_negative()) {
                        soft_constraint_proxy_equal_plus(INDEX) =
                            (-expression >=
                             -UPPER_BOUND *
                                 soft_constraint_slack_proxy_equal_plus(INDEX));
                        soft_constraint_proxy_equal_minus(INDEX) =
                            (-expression <=
                             -LOWER_BOUND *
                                 soft_constraint_slack_proxy_equal_minus(
                                     INDEX));
                    } else {
                        soft_constraint_proxy_equal_plus(INDEX) =
                            (expression <=
                             UPPER_BOUND *
                                 soft_constraint_slack_proxy_equal_plus(INDEX));
                        soft_constraint_proxy_equal_minus(INDEX) =
                            (expression >=
                             LOWER_BOUND *
                                 soft_constraint_slack_proxy_equal_minus(
                                     INDEX));
                    }
                    soft_constraint_proxy_equal_plus(INDEX).set_name(
                        SOFT_CONSTRAINT.name + "_equal_plus");
                    soft_constraint_proxy_equal_minus(INDEX).set_name(
                        SOFT_CONSTRAINT.name + "_equal_minus");

                    break;
                }

                case opb::OPBConstraintSense::Greater: {
                    const auto INDEX = soft_constraint_indices_greater[i];
                    if (SOFT_CONSTRAINT.is_all_coefficient_negative()) {
                        soft_constraint_proxy_greater_minus(INDEX) =
                            (-expression <=
                             -LOWER_BOUND *
                                 soft_constraint_slack_proxy_greater_minus(
                                     INDEX));
                    } else {
                        soft_constraint_proxy_greater_minus(INDEX) =
                            (expression >=
                             LOWER_BOUND *
                                 soft_constraint_slack_proxy_greater_minus(
                                     INDEX));
                    }
                    soft_constraint_proxy_greater_minus(INDEX).set_name(
                        SOFT_CONSTRAINT.name + "_greater_minus");

                    break;
                }
            }
        }

        for (auto i = 0; i < NUMBER_OF_SOFT_CONSTRAINTS; i++) {
            const auto &SOFT_CONSTRAINT = a_OPB.soft_constraints[i];
            switch (SOFT_CONSTRAINT.sense) {
                case opb::OPBConstraintSense::Less: {
                    const auto INDEX = soft_constraint_indices_less[i];
                    objective_penalty +=
                        SOFT_CONSTRAINT.weight *
                        soft_constraint_slack_proxy_less_plus(INDEX);

                    break;
                }

                case opb::OPBConstraintSense::Equal: {
                    const auto INDEX = soft_constraint_indices_equal[i];
                    objective_penalty +=
                        SOFT_CONSTRAINT.weight *
                        (soft_constraint_slack_proxy_equal_plus(INDEX) +
                         soft_constraint_slack_proxy_equal_minus(INDEX));

                    break;
                }

                case opb::OPBConstraintSense::Greater: {
                    const auto INDEX = soft_constraint_indices_greater[i];
                    objective_penalty +=
                        SOFT_CONSTRAINT.weight *
                        soft_constraint_slack_proxy_greater_minus(INDEX);

                    break;
                }
            }
        }

        /**
         * Set up hard constraints.
         */
        const int NUMBER_OF_HARD_CONSTRAINTS = a_OPB.hard_constraints.size();

        auto &hard_constraint_proxy =
            model.component_creator().create_constraints(
                "hard_constraints", NUMBER_OF_HARD_CONSTRAINTS);
#ifdef _OPENMP
#pragma omp parallel for schedule(static)
#endif
        for (auto i = 0; i < NUMBER_OF_HARD_CONSTRAINTS; i++) {
            const auto &HARD_CONSTRAINT = a_OPB.hard_constraints[i];
            auto        expression =
                model_component::Expression<T_Variable,
                                            T_Expression>::create_instance();

            Sensitivities expression_sensitivities;
            for (const auto &term : HARD_CONSTRAINT.terms) {
                const auto &VARIABLE_NAME = term.concated_variable_name();
                const T_Expression COEFFICIENT =
                    static_cast<T_Expression>(term.coefficient);
                expression_sensitivities[variable_ptrs[VARIABLE_NAME]] =
                    COEFFICIENT;
            }
            expression.set_sensitivities(expression_sensitivities);

            switch (HARD_CONSTRAINT.sense) {
                case opb::OPBConstraintSense::Less: {
                    if (HARD_CONSTRAINT.is_all_coefficient_negative()) {
                        hard_constraint_proxy(i) =
                            (-expression >= -HARD_CONSTRAINT.rhs);
                    } else {
                        hard_constraint_proxy(i) =
                            (expression <= HARD_CONSTRAINT.rhs);
                    }
                    break;
                }

                case opb::OPBConstraintSense::Equal: {
                    if (HARD_CONSTRAINT.is_all_coefficient_negative()) {
                        hard_constraint_proxy(i) =
                            (-expression == -HARD_CONSTRAINT.rhs);
                    } else {
                        hard_constraint_proxy(i) =
                            (expression == HARD_CONSTRAINT.rhs);
                    }
                    break;
                }

                case opb::OPBConstraintSense::Greater: {
                    if (HARD_CONSTRAINT.is_all_coefficient_negative()) {
                        hard_constraint_proxy(i) =
                            (-expression <= -HARD_CONSTRAINT.rhs);
                    } else {
                        hard_constraint_proxy(i) =
                            (expression >= HARD_CONSTRAINT.rhs);
                    }
                    break;
                }
            }
            hard_constraint_proxy(i).set_name(HARD_CONSTRAINT.name);
        }

        /**
         * Set up constraints for nagated variables.
         */
        const int NUMBER_OF_NEGATED_VARIABLE_CONSTRAINTS =
            a_OPB.negated_variable_names.size();
        if (NUMBER_OF_NEGATED_VARIABLE_CONSTRAINTS > 0) {
            auto &negated_variable_constraint_proxy =
                model.component_creator().create_constraints(
                    "negated_variable_constraints",
                    NUMBER_OF_NEGATED_VARIABLE_CONSTRAINTS);
#ifdef _OPENMP
#pragma omp parallel for schedule(static)
#endif
            for (auto i = 0; i < NUMBER_OF_NEGATED_VARIABLE_CONSTRAINTS; i++) {
                const auto &NEGATED_VARIABLE_NAME =
                    a_OPB.negated_variable_names[i];
                negated_variable_constraint_proxy(i) =
                    *variable_ptrs[NEGATED_VARIABLE_NAME] +
                        *variable_ptrs["~" + NEGATED_VARIABLE_NAME] ==
                    1;
                hard_constraint_proxy(i).set_name(
                    "negated_variable_constraints_" + NEGATED_VARIABLE_NAME);
            }
        }

        /**
         * Set up constraints for product variables.
         */
        const int NUMBER_OF_PRODUCT_VARIABLE_CONSTRAINTS =
            a_OPB.product_variable_names.size();
        if (NUMBER_OF_PRODUCT_VARIABLE_CONSTRAINTS > 0) {
            auto &product_variable_constraint_proxy =
                model.component_creator().create_constraints(
                    "product_variable_constraints",
                    NUMBER_OF_PRODUCT_VARIABLE_CONSTRAINTS * 2);
#ifdef _OPENMP
#pragma omp parallel for schedule(static)
#endif
            for (auto i = 0; i < NUMBER_OF_PRODUCT_VARIABLE_CONSTRAINTS; i++) {
                const auto &PRODUCT_VARIABLE_NAME =
                    a_OPB.product_variable_names[i].first;
                const int PRODUCT_SIZE =
                    a_OPB.product_variable_names[i].second.size();

                auto expression_lower = model_component::Expression<
                    T_Variable, T_Expression>::create_instance();
                Sensitivities expression_sensitivities_lower;
                expression_sensitivities_lower
                    [variable_ptrs[PRODUCT_VARIABLE_NAME]] = 1;

                for (const auto &variable_name :
                     a_OPB.product_variable_names[i].second) {
                    expression_sensitivities_lower
                        [variable_ptrs[variable_name]] = -1;
                }

                expression_lower.set_sensitivities(
                    expression_sensitivities_lower);
                product_variable_constraint_proxy(2 * i) =
                    (expression_lower >= -PRODUCT_SIZE + 1);
                product_variable_constraint_proxy(2 * i).set_name(
                    "product_variable_constraints_lower_" +
                    PRODUCT_VARIABLE_NAME);

                auto expression_upper = model_component::Expression<
                    T_Variable, T_Expression>::create_instance();

                Sensitivities expression_sensitivities_upper;
                expression_sensitivities_upper
                    [variable_ptrs[PRODUCT_VARIABLE_NAME]] = PRODUCT_SIZE;

                for (const auto &variable_name :
                     a_OPB.product_variable_names[i].second) {
                    expression_sensitivities_upper
                        [variable_ptrs[variable_name]] = -1;
                }

                expression_upper.set_sensitivities(
                    expression_sensitivities_upper);
                product_variable_constraint_proxy(2 * i + 1) =
                    (expression_upper <= 0);
                product_variable_constraint_proxy(2 * i + 1).set_name(
                    "product_variable_constraints_upper_" +
                    PRODUCT_VARIABLE_NAME);
            }
        }

        /**
         * Set up top cost constraint for wbo instances.
         */
        if (a_OPB.top_cost.is_defined) {
            auto &top_cost_constraint_proxy =
                model.component_creator().create_constraints(
                    "top_cost_constraint", 1);

            top_cost_constraint_proxy(0) =
                objective_penalty <= a_OPB.top_cost.value - 1;
        }

        /**
         * Set up objective function.
         */
        auto objective =
            model_component::Expression<T_Variable,
                                        T_Expression>::create_instance();
        Sensitivities objective_sensitivities;
        for (const auto &term : a_OPB.objective.terms) {
            const std::string  VARIABLE_NAME = term.concated_variable_name();
            const T_Expression COEFFICIENT =
                static_cast<T_Expression>(term.coefficient);
            objective_sensitivities[variable_ptrs[VARIABLE_NAME]] = COEFFICIENT;
        }
        objective.set_sensitivities(objective_sensitivities);
        if (objective_penalty.sensitivities().size() > 0) {
            if (model.is_minimization()) {
                objective += objective_penalty;
            } else {
                objective -= objective_penalty;
            }
        }
        model.minimize(objective);
    }
};
}  // namespace printemps::model_handler
#endif
/*****************************************************************************/
// END
/*****************************************************************************/