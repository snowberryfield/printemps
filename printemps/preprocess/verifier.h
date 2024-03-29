/*****************************************************************************/
// Copyright (c) 2020-2023 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_PREPROCESS_VERIFIER_H__
#define PRINTEMPS_PREPROCESS_VERIFIER_H__

namespace printemps::model {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Model;
}  // namespace printemps::model

namespace printemps::preprocess {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Verifier {
   private:
    model::Model<T_Variable, T_Expression> *m_model_ptr;

    /*************************************************************************/
    inline void print_correction(const std::string &a_VARIABLE_NAME,
                                 const T_Variable   a_OLD_VALUE,
                                 const T_Variable   a_NEW_VALUE,
                                 const bool         a_IS_ENABLED_PRINT) {
        utility::print_warning(  //
            "The initial value " + a_VARIABLE_NAME + " = " +
                std::to_string(a_OLD_VALUE) + " was corrected to " +
                std::to_string(a_NEW_VALUE) + ".",
            a_IS_ENABLED_PRINT);
    }

   public:
    /*************************************************************************/
    Verifier(void) {
        this->initialize();
    }

    /*************************************************************************/
    Verifier(model::Model<T_Variable, T_Expression> *a_model_ptr) {
        this->setup(a_model_ptr);
    }

    /*************************************************************************/
    inline void setup(model::Model<T_Variable, T_Expression> *a_model_ptr) {
        this->initialize();
        this->m_model_ptr = a_model_ptr;
    }

    /*************************************************************************/
    inline void initialize(void) {
        m_model_ptr = nullptr;
    }

    /*************************************************************************/
    inline void verify_problem(const bool a_IS_ENABLED_PRINT) {
        utility::print_single_line(a_IS_ENABLED_PRINT);
        utility::print_message("Verifying the problem...", a_IS_ENABLED_PRINT);

        if (m_model_ptr->variable_proxies().size() == 0) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__, "No variables are defined."));
        }

        if (m_model_ptr->constraint_proxies().size() == 0 &&
            !m_model_ptr->is_defined_objective()) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "Neither objective nor constraint functions are defined."));
        }

        utility::print_message("Done.", a_IS_ENABLED_PRINT);
    }

    /*************************************************************************/
    inline void verify_and_correct_selection_variables_initial_values(
        const bool a_IS_ENABLED_CORRECTION, const bool a_IS_ENABLED_PRINT) {
        utility::print_single_line(a_IS_ENABLED_PRINT);
        utility::print_message(
            "Verifying the initial values of the binary included in the "
            "selection constraints...",
            a_IS_ENABLED_PRINT);

        std::unordered_map<
            model_component::Constraint<T_Variable, T_Expression> *,
            T_Expression>
            related_constraint_ptr_values;
        for (auto &&selection : m_model_ptr->selections()) {
            for (auto &&constraint_ptr : selection.related_constraint_ptrs) {
                related_constraint_ptr_values[constraint_ptr] =
                    static_cast<T_Expression>(0);
            }
        }

        for (auto &&selection : m_model_ptr->selections()) {
            std::vector<model_component::Variable<T_Variable, T_Expression> *>
                fixed_selected_variable_ptrs;
            std::vector<model_component::Variable<T_Variable, T_Expression> *>
                selected_variable_ptrs;
            std::vector<model_component::Variable<T_Variable, T_Expression> *>
                fixed_invalid_variable_ptrs;
            std::vector<model_component::Variable<T_Variable, T_Expression> *>
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
                    "There are more than one fixed selected variables."));
            }

            /**
             * Correct initial values or return logic error if there is
             * a variable of which initial value violates binary
             * constraint.
             */
            if (invalid_variable_ptrs.size() > 0) {
                if (!a_IS_ENABLED_CORRECTION) {
                    throw std::logic_error(utility::format_error_location(
                        __FILE__, __LINE__, __func__,
                        "There is a variable of which initial value violates "
                        "binary constraint."));
                }

                for (auto &&variable_ptr : invalid_variable_ptrs) {
                    const T_Variable OLD_VALUE = variable_ptr->value();
                    const T_Variable NEW_VALUE = 0;

                    variable_ptr->set_value_if_mutable(NEW_VALUE);
                    this->print_correction(variable_ptr->name(), OLD_VALUE,
                                           NEW_VALUE, a_IS_ENABLED_PRINT);
                }
            }

            /**
             * Correct initial values if there is just 1 fixed selected
             * variables.
             */
            if (fixed_selected_variable_ptrs.size() == 1) {
                if (!a_IS_ENABLED_CORRECTION &&
                    selected_variable_ptrs.size() >= 2) {
                    throw std::logic_error(utility::format_error_location(
                        __FILE__, __LINE__, __func__,
                        "There are more than one selected variables."));
                }

                auto selected_variable_ptr =
                    fixed_selected_variable_ptrs.front();
                for (auto &&variable_ptr : selected_variable_ptrs) {
                    if (variable_ptr != selected_variable_ptr) {
                        const T_Variable OLD_VALUE = 1;
                        const T_Variable NEW_VALUE = 0;
                        variable_ptr->set_value_if_mutable(NEW_VALUE);
                        this->print_correction(variable_ptr->name(), OLD_VALUE,
                                               NEW_VALUE, a_IS_ENABLED_PRINT);
                    }
                }

                selected_variable_ptr->set_value_if_mutable(1);

                for (auto &&sensitivity :
                     selected_variable_ptr->constraint_sensitivities()) {
                    related_constraint_ptr_values[sensitivity.first] +=
                        sensitivity.second;
                }
            }
            /**
             * Correct initial values or return logic error if there are
             * more than 1 selected variables.
             */
            else if (selected_variable_ptrs.size() > 1) {
                if (!a_IS_ENABLED_CORRECTION) {
                    throw std::logic_error(utility::format_error_location(
                        __FILE__, __LINE__, __func__,
                        "There are more than one selected variables."));
                }

                model_component::Variable<T_Variable, T_Expression>
                    *selected_variable_ptr = nullptr;
                int  best_violation        = std::numeric_limits<int>::max();
                for (auto &&variable_ptr : selected_variable_ptrs) {
                    int violation = 0;
                    for (auto &&sensitivity :
                         variable_ptr->constraint_sensitivities()) {
                        const auto CONSTRAINT_VALUE =
                            related_constraint_ptr_values[sensitivity.first] +
                            sensitivity.second +
                            sensitivity.first->expression().constant_value();
                        switch (sensitivity.first->sense()) {
                            case model_component::ConstraintSense::Less: {
                                violation +=
                                    std::max(CONSTRAINT_VALUE,
                                             static_cast<T_Expression>(0));
                                break;
                            }
                            case model_component::ConstraintSense::Equal: {
                                violation += std::abs(CONSTRAINT_VALUE);
                                break;
                            }
                            case model_component::ConstraintSense::Greater: {
                                violation +=
                                    std::max(-CONSTRAINT_VALUE,
                                             static_cast<T_Expression>(0));
                                break;
                            }
                        }
                    }

                    if (violation < best_violation) {
                        best_violation        = violation;
                        selected_variable_ptr = variable_ptr;
                    }
                }

                if (selected_variable_ptr == nullptr) {
                    throw std::logic_error(utility::format_error_location(
                        __FILE__, __LINE__, __func__,
                        "The initial value could not be modified for some "
                        "unknown reasons."));
                };

                for (auto &&variable_ptr : selected_variable_ptrs) {
                    if (variable_ptr != selected_variable_ptr) {
                        const T_Variable OLD_VALUE = 1;
                        const T_Variable NEW_VALUE = 0;

                        variable_ptr->set_value_if_mutable(NEW_VALUE);
                        this->print_correction(variable_ptr->name(), OLD_VALUE,
                                               NEW_VALUE, a_IS_ENABLED_PRINT);
                    }
                }

                selected_variable_ptr->set_value_if_mutable(1);

                for (auto &&sensitivity :
                     selected_variable_ptr->constraint_sensitivities()) {
                    related_constraint_ptr_values[sensitivity.first] +=
                        sensitivity.second;
                }
            }
            /**
             * Correct initial values or return logic error if there is
             * no selected variables.
             */
            else if (selected_variable_ptrs.size() == 0) {
                if (!a_IS_ENABLED_CORRECTION) {
                    throw std::logic_error(utility::format_error_location(
                        __FILE__, __LINE__, __func__,
                        "There is no selected variables."));
                }

                model_component::Variable<T_Variable, T_Expression>
                    *selected_variable_ptr = nullptr;
                int  best_violation        = std::numeric_limits<int>::max();
                for (auto &&variable_ptr : selection.variable_ptrs) {
                    if (variable_ptr->is_fixed()) {
                        continue;
                    }
                    int violation = 0;
                    for (auto &&sensitivity :
                         variable_ptr->constraint_sensitivities()) {
                        const auto CONSTRAINT_VALUE =
                            related_constraint_ptr_values[sensitivity.first] +
                            sensitivity.second +
                            sensitivity.first->expression().constant_value();
                        switch (sensitivity.first->sense()) {
                            case model_component::ConstraintSense::Less: {
                                violation +=
                                    std::max(CONSTRAINT_VALUE,
                                             static_cast<T_Expression>(0));
                                break;
                            }
                            case model_component::ConstraintSense::Equal: {
                                violation += std::abs(CONSTRAINT_VALUE);
                                break;
                            }
                            case model_component::ConstraintSense::Greater: {
                                violation +=
                                    std::max(-CONSTRAINT_VALUE,
                                             static_cast<T_Expression>(0));
                                break;
                            }
                        }
                    }

                    if (violation < best_violation) {
                        best_violation        = violation;
                        selected_variable_ptr = variable_ptr;
                    }
                }

                if (selected_variable_ptr == nullptr) {
                    throw std::logic_error(utility::format_error_location(
                        __FILE__, __LINE__, __func__,
                        "The initial value could not be modified for some "
                        "unknown reasons."));
                };

                selected_variable_ptr->set_value_if_mutable(1);
                this->print_correction(selected_variable_ptr->name(), 0, 1,
                                       a_IS_ENABLED_PRINT);

                for (auto &&sensitivity :
                     selected_variable_ptr->constraint_sensitivities()) {
                    related_constraint_ptr_values[sensitivity.first] +=
                        sensitivity.second;
                }
            } else {
                selected_variable_ptrs.front()->select();
            }
        }

        utility::print_message("Done.", a_IS_ENABLED_PRINT);
    }

    /*************************************************************************/
    inline void verify_and_correct_binary_variables_initial_values(
        const bool a_IS_ENABLED_CORRECTION, const bool a_IS_ENABLED_PRINT) {
        utility::print_single_line(a_IS_ENABLED_PRINT);
        utility::print_message(
            "Verifying the initial values of the binary variables.",
            a_IS_ENABLED_PRINT);

        for (auto &&variable_ptr :
             m_model_ptr->variable_type_reference().binary_variable_ptrs) {
            if (variable_ptr->value() == 0 || variable_ptr->value() == 1) {
                continue;
            }
            if (variable_ptr->is_fixed()) {
                throw std::logic_error(utility::format_error_location(
                    __FILE__, __LINE__, __func__,
                    "There is an invalid fixed variable."));
            }

            if (!a_IS_ENABLED_CORRECTION) {
                throw std::logic_error(utility::format_error_location(
                    __FILE__, __LINE__, __func__,
                    "An initial value violates binary constraint."));
            }

            T_Variable old_value = variable_ptr->value();
            T_Variable new_value = variable_ptr->value();
            if (variable_ptr->value() < variable_ptr->lower_bound()) {
                new_value = variable_ptr->lower_bound();
            } else if (variable_ptr->value() > variable_ptr->upper_bound()) {
                new_value = variable_ptr->upper_bound();
            }

            variable_ptr->set_value_if_mutable(new_value);
            this->print_correction(variable_ptr->name(), old_value, new_value,
                                   a_IS_ENABLED_PRINT);
        }
        utility::print_message("Done.", a_IS_ENABLED_PRINT);
    }

    /*************************************************************************/
    inline void verify_and_correct_integer_variables_initial_values(
        const bool a_IS_ENABLED_CORRECTION, const bool a_IS_ENABLED_PRINT) {
        utility::print_single_line(a_IS_ENABLED_PRINT);
        utility::print_message(
            "Verifying the initial values of the integer variables.",
            a_IS_ENABLED_PRINT);

        for (auto &&variable_ptr :
             m_model_ptr->variable_type_reference().integer_variable_ptrs) {
            if (variable_ptr->value() >= variable_ptr->lower_bound() &&
                variable_ptr->value() <= variable_ptr->upper_bound()) {
                continue;
            }

            if (variable_ptr->is_fixed()) {
                throw std::logic_error(utility::format_error_location(
                    __FILE__, __LINE__, __func__,
                    "There is an invalid fixed variable"));
            }

            if (!a_IS_ENABLED_CORRECTION) {
                throw std::logic_error(utility::format_error_location(
                    __FILE__, __LINE__, __func__,
                    "An initial value violates the lower or upper bound "
                    "constraint."));
            }

            T_Variable old_value = variable_ptr->value();
            T_Variable new_value = variable_ptr->value();
            if (variable_ptr->value() < variable_ptr->lower_bound()) {
                new_value = variable_ptr->lower_bound();
            } else if (variable_ptr->value() > variable_ptr->upper_bound()) {
                new_value = variable_ptr->upper_bound();
            }

            variable_ptr->set_value_if_mutable(new_value);
            this->print_correction(variable_ptr->name(), old_value, new_value,
                                   a_IS_ENABLED_PRINT);
        }
        utility::print_message("Done.", a_IS_ENABLED_PRINT);
    }
};
}  // namespace printemps::preprocess
#endif
/*****************************************************************************/
// END
/*****************************************************************************/