/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MODEL_HANDLER_MODEL_MPS_HANDLER_H__
#define PRINTEMPS_MODEL_HANDLER_MODEL_MPS_HANDLER_H__

namespace printemps::model_handler {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class ModelMPSHandler {
   private:
    model::Model<T_Variable, T_Expression> *m_model_ptr;

   public:
    /*************************************************************************/
    ModelMPSHandler(void) {
        m_model_ptr = nullptr;
    }

    /*************************************************************************/
    ModelMPSHandler(model::Model<T_Variable, T_Expression> *a_model_ptr) {
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
    void import(const mps::MPS &a_MPS, const bool a_ACCEPT_CONTINUOUS) {
        auto &model = *m_model_ptr;

        using VariableMap = std::unordered_map<
            std::string, model_component::Variable<T_Variable, T_Expression> *>;
        using Sensitivities = std::unordered_map<
            model_component::Variable<T_Variable, T_Expression> *,
            T_Expression>;

        VariableMap variable_ptrs;
        model.set_is_minimization(a_MPS.objective.is_minimization);

        auto &variable_proxy = model.component_creator().create_variables(
            "variables", a_MPS.variables.size());

        /**
         * Set up variables.
         */
        const int NUMBER_OF_VARIABLES = a_MPS.variable_names.size();

        for (auto i = 0; i < NUMBER_OF_VARIABLES; i++) {
            const auto &VARIABLE_NAME = a_MPS.variable_names[i];
            const auto &VARIABLE      = a_MPS.variables.at(VARIABLE_NAME);

            if (VARIABLE.type == mps::MPSVariableType::Continuous) {
                if (a_ACCEPT_CONTINUOUS) {
                    utility::print_warning(
                        "The continuous variable " + VARIABLE_NAME +
                            " will be regarded as an integer variable.",
                        true);
                } else {
                    throw std::runtime_error(utility::format_error_location(
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
         * Set up constraints.
         */
        const int NUMBER_OF_CONSTRAINTS = a_MPS.constraint_names.size();
        auto &constraint_proxy = model.component_creator().create_constraints(
            "constraints", NUMBER_OF_CONSTRAINTS);

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
                const auto        &VARIABLE_NAME = sensitivity.first;
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
        model.minimize(objective);
    }

    /*********************************************************************/
    std::string create_mps_string(void) {
        std::ostringstream oss;
        const auto        &model = *m_model_ptr;

        /**
         * Determine unique name of variables and constraints.
         */
        model.builder().setup_unique_names();

        /**
         * Determine the sensitivities.
         */
        model.builder().setup_variable_constraint_sensitivities();
        model.builder().setup_variable_objective_sensitivities();

        /*****************************************************************/
        // NAME
        /*****************************************************************/
        if (model.name.empty()) {
            oss << "NAME untitled" << std::endl;
        } else {
            oss << "NAME " << utility::delete_space(model.name()) << std::endl;
        }

        /*****************************************************************/
        // ROWS
        /*****************************************************************/
        oss << "ROWS" << std::endl;
        oss << " N    obj" << std::endl;
        for (const auto &proxy : model.constraint_proxies()) {
            for (const auto &constraint : proxy.flat_indexed_constraints()) {
                switch (constraint.sense()) {
                    case model_component::ConstraintSense::Equal:
                        oss << " E    "
                            << utility::delete_space(constraint.name())
                            << std::endl;
                        break;
                    case model_component::ConstraintSense::Less:
                        oss << " L    "
                            << utility::delete_space(constraint.name())
                            << std::endl;
                        break;
                    case model_component::ConstraintSense::Greater:
                        oss << " G    "
                            << utility::delete_space(constraint.name())
                            << std::endl;
                        break;
                    default:
                        break;
                }
            }
        }

        /*****************************************************************/
        // COLUMNS
        /*****************************************************************/
        oss << "COLUMNS" << std::endl;
        oss << "    MARK0000    'MARKER'    'INTORG'" << std::endl;

        for (const auto &proxy : model.variable_proxies()) {
            for (const auto &variable : proxy.flat_indexed_variables()) {
                const auto VARIABLE_NAME =
                    utility::delete_space(variable.name());

                /// Objective
                if (std::fabs(variable.objective_sensitivity()) >
                    constant::EPSILON_10) {
                    double coeff = model.is_minimization()
                                       ? variable.objective_sensitivity()
                                       : -variable.objective_sensitivity();
                    oss << "    " << VARIABLE_NAME << "     obj    " << coeff
                        << std::endl;
                }

                /// Constraints
                for (const auto &sensitivity :
                     variable.constraint_sensitivities()) {
                    const auto CONSTRAINT_NAME =
                        utility::delete_space(sensitivity.first->name());
                    const auto COEFFICIENT = sensitivity.second;
                    oss << "    " << VARIABLE_NAME << "    " << CONSTRAINT_NAME
                        << "    " << COEFFICIENT << std::endl;
                }
            }
        }
        oss << "    MARK0001    'MARKER'    'INTEND'" << std::endl;

        /*****************************************************************/
        // RHS
        /*****************************************************************/
        oss << "RHS" << std::endl;
        for (const auto &proxy : model.constraint_proxies()) {
            for (const auto &constraint : proxy.flat_indexed_constraints()) {
                const auto CONSTRAINT_NAME =
                    utility::delete_space(constraint.name());
                const auto &EXPRESSION = constraint.expression();
                oss << "    rhs    " << CONSTRAINT_NAME << "    "
                    << -EXPRESSION.constant_value() << std::endl;
            }
        }

        /*****************************************************************/
        // BOUNDS
        /*****************************************************************/
        oss << "BOUNDS" << std::endl;
        for (const auto &proxy : model.variable_proxies()) {
            for (const auto &variable : proxy.flat_indexed_variables()) {
                const auto VARIABLE_NAME =
                    utility::delete_space(variable.name());
                if (variable.is_fixed()) {
                    oss << "    FX    bnd    " << VARIABLE_NAME << "     "
                        << variable.value() << std::endl;
                } else {
                    if (variable.lower_bound() != constant::INT_HALF_MIN &&
                        variable.lower_bound() != 0) {
                        oss << "    LO    bnd    " << VARIABLE_NAME << "    "
                            << variable.lower_bound() << std::endl;
                    }
                    if (variable.upper_bound() != constant::INT_HALF_MAX) {
                        oss << "    UP    bnd    " << VARIABLE_NAME << "    "
                            << variable.upper_bound() << std::endl;
                    }
                }
            }
        }

        /*****************************************************************/
        // END
        /*****************************************************************/
        oss << "ENDATA" << std::endl;

        return oss.str();
    }

    /*********************************************************************/
    void write(const std::string &a_FILE_NAME) {
        std::ofstream ofs(a_FILE_NAME);
        ofs << this->create_mps_string();
        ofs.close();
    }
};
}  // namespace printemps::model_handler
#endif
/*****************************************************************************/
// END
/*****************************************************************************/