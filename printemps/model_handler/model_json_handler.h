/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MODEL_HANDLER_MODEL_JSON_HANDLER_H__
#define PRINTEMPS_MODEL_HANDLER_MODEL_JSON_HANDLER_H__

namespace printemps::model_handler {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class ModelJSONHandler {
   private:
    model::Model<T_Variable, T_Expression> *m_model_ptr;

   public:
    /*************************************************************************/
    ModelJSONHandler(void) {
        m_model_ptr = nullptr;
    }

    /*************************************************************************/
    ModelJSONHandler(model::Model<T_Variable, T_Expression> *a_model_ptr) {
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
    inline utility::json::JsonObject create_json_object(void) const {
        auto &model = *m_model_ptr;

        /**
         * Determine unique name of variables and constraints.
         */
        model.builder().setup_unique_names();

        /**
         * Determine the sensitivities.
         */
        model.builder().setup_variable_constraint_sensitivities();
        model.builder().setup_variable_objective_sensitivities();

        utility::json::JsonObject object;

        /**
         * Instance name
         */
        if (model.name().empty()) {
            object.emplace_back("name", std::string("untitled"));
        } else {
            object.emplace_back("name", utility::delete_space(model.name()));
        }

        /**
         * Minimization or not
         */
        object.emplace_back("is_minimization", model.is_minimization());

        /**
         * Variables
         */
        utility::json::JsonArray variable_array;
        for (const auto &proxy : model.variable_proxies()) {
            for (const auto &variable : proxy.flat_indexed_variables()) {
                utility::json::JsonObject v_object;
                v_object.emplace_back("name", variable.name());
                v_object.emplace_back("value", variable.value());
                v_object.emplace_back("is_fixed", variable.is_fixed());
                v_object.emplace_back("lower_bound", variable.lower_bound());
                v_object.emplace_back("upper_bound", variable.upper_bound());
                if (variable.sense() ==
                        model_component::VariableSense::Binary ||
                    variable.sense() ==
                        model_component::VariableSense::Selection) {
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

        auto sensitivities_vector = utility::to_pair_vector(
            model.objective().expression().sensitivities());

        std::stable_sort(sensitivities_vector.begin(),
                         sensitivities_vector.end(),
                         [](const auto &a, const auto &b) {
                             return a.first->name() < b.first->name();
                         });

        for (const auto &sensitivity : sensitivities_vector) {
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
        for (const auto &proxy : model.constraint_proxies()) {
            for (const auto &constraint : proxy.flat_indexed_constraints()) {
                utility::json::JsonObject c_object;

                const auto &EXPRESSION    = constraint.expression();
                const auto &SENSITIVITIES = EXPRESSION.sensitivities();
                c_object.emplace_back("name", constraint.name());
                if (constraint.is_selection()) {
                    c_object.emplace_back("is_enabled", true);
                } else {
                    c_object.emplace_back("is_enabled",
                                          constraint.is_enabled());
                }

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

                auto sensitivities_vector =
                    utility::to_pair_vector(SENSITIVITIES);

                std::stable_sort(sensitivities_vector.begin(),
                                 sensitivities_vector.end(),
                                 [](const auto &a, const auto &b) {
                                     return a.first->name() < b.first->name();
                                 });

                for (const auto &sensitivity : sensitivities_vector) {
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
        return object;
    }

    /*************************************************************************/
    void write(const std::string &a_FILE_NAME) {
        auto object = this->create_json_object();
        utility::json::write_json_object(object, a_FILE_NAME);
    }
};
}  // namespace printemps::model_handler
#endif
/*****************************************************************************/
// END
/*****************************************************************************/