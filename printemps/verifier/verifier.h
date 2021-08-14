/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_VERIFIER_VERIFIER_H__
#define PRINTEMPS_VERIFIER_VERIFIER_H__

namespace printemps {
namespace model {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Model;
}  // namespace model
}  // namespace printemps

namespace printemps {
namespace verifier {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
constexpr void verify_problem(
    model::Model<T_Variable, T_Expression> *a_model_ptr,  //
    const bool                              a_IS_ENABLED_PRINT) {
    utility::print_single_line(a_IS_ENABLED_PRINT);
    utility::print_message("Verifying the problem...", a_IS_ENABLED_PRINT);

    if (a_model_ptr->variable_proxies().size() == 0) {
        throw std::logic_error(utility::format_error_location(
            __FILE__, __LINE__, __func__,
            "No decision variables are defined."));
    }
    if (a_model_ptr->constraint_proxies().size() == 0 &&
        !a_model_ptr->is_defined_objective()) {
        throw std::logic_error(utility::format_error_location(
            __FILE__, __LINE__, __func__,
            "Neither objective nor constraint functions are defined."));
    }
    utility::print_message("Done.", a_IS_ENABLED_PRINT);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression>
constexpr void verify_and_correct_selection_variables_initial_values(
    model::Model<T_Variable, T_Expression> *a_model_ptr,             //
    const bool                              a_IS_ENABLED_CORRECTON,  //
    const bool                              a_IS_ENABLED_PRINT) {
    utility::print_single_line(a_IS_ENABLED_PRINT);
    utility::print_message(
        "Verifying the initial values of the binary decision　variables "
        "included in the selection constraints...",
        a_IS_ENABLED_PRINT);

    for (auto &&selection : a_model_ptr->selections()) {
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
            if (a_IS_ENABLED_CORRECTON) {
                for (auto &&variable_ptr : invalid_variable_ptrs) {
                    T_Variable old_value = variable_ptr->value();
                    T_Variable new_value = 0;

                    variable_ptr->set_value_if_mutable(new_value);

                    utility::print_warning(
                        "The initial value " + variable_ptr->name() + " = " +
                            std::to_string(old_value) + " was corrected to " +
                            std::to_string(new_value) + ".",
                        a_IS_ENABLED_PRINT);
                }

            } else {
                throw std::logic_error(utility::format_error_location(
                    __FILE__, __LINE__, __func__,
                    "There is a variable of which initial value violates "
                    "binary constraint."));
            }
        }

        /**
         * Correct initial values or return logic error if there are
         * more than 1 selected variables.
         */
        if (selected_variable_ptrs.size() > 1) {
            if (a_IS_ENABLED_CORRECTON) {
                model_component::Variable<T_Variable, T_Expression>
                    *selected_variable_ptr = nullptr;
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

                        variable_ptr->set_value_if_mutable(new_value);

                        utility::print_warning(
                            "The initial value " + variable_ptr->name() +
                                " = " + std::to_string(old_value) +
                                " was corrected to " +
                                std::to_string(new_value) + ".",
                            a_IS_ENABLED_PRINT);
                    }
                }

                selected_variable_ptr->set_value_if_mutable(1);
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
                        variable_ptr->set_value_if_mutable(new_value);

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
                        "The initial value could not be modified because all "
                        "variables are fixed."));
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

/*****************************************************************************/
template <class T_Variable, class T_Expression>
constexpr void verify_and_correct_binary_variables_initial_values(
    model::Model<T_Variable, T_Expression> *a_model_ptr,             //
    const bool                              a_IS_ENABLED_CORRECTON,  //
    const bool                              a_IS_ENABLED_PRINT) {
    utility::print_single_line(a_IS_ENABLED_PRINT);
    utility::print_message(
        "Verifying the initial values of the binary decision variables.",
        a_IS_ENABLED_PRINT);

    for (auto &&proxy : a_model_ptr->variable_proxies()) {
        for (auto &&variable : proxy.flat_indexed_variables()) {
            if (variable.sense() == model_component::VariableSense::Binary) {
                if (variable.value() != 0 && variable.value() != 1) {
                    if (variable.is_fixed()) {
                        throw std::logic_error(utility::format_error_location(
                            __FILE__, __LINE__, __func__,
                            "There is an invalid fixed variable."));
                    }

                    if (a_IS_ENABLED_CORRECTON) {
                        T_Variable old_value = variable.value();
                        T_Variable new_value = variable.value();
                        if (variable.value() < variable.lower_bound()) {
                            new_value = variable.lower_bound();
                        } else if (variable.value() > variable.upper_bound()) {
                            new_value = variable.upper_bound();
                        }

                        variable.set_value_if_mutable(new_value);

                        utility::print_warning(
                            "The initial value " + variable.name() + " = " +
                                std::to_string(old_value) +
                                " was corrected to " +
                                std::to_string(new_value) + ".",
                            a_IS_ENABLED_PRINT);
                    } else {
                        throw std::logic_error(utility::format_error_location(
                            __FILE__, __LINE__, __func__,
                            "An initial value violates binary constraint."));
                    }
                }
            }
        }
    }
    utility::print_message("Done.", a_IS_ENABLED_PRINT);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression>
constexpr void verify_and_correct_integer_variables_initial_values(
    model::Model<T_Variable, T_Expression> *a_model_ptr,             //
    const bool                              a_IS_ENABLED_CORRECTON,  //
    const bool                              a_IS_ENABLED_PRINT) {
    utility::print_single_line(a_IS_ENABLED_PRINT);
    utility::print_message(
        "Verifying the initial values of the integer decision variables.",
        a_IS_ENABLED_PRINT);

    for (auto &&proxy : a_model_ptr->variable_proxies()) {
        for (auto &&variable : proxy.flat_indexed_variables()) {
            if (variable.sense() == model_component::VariableSense::Integer &&
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

                    variable.set_value_if_mutable(new_value);

                    utility::print_warning(
                        "The initial value " + variable.name() + " = " +
                            std::to_string(old_value) + " was corrected to " +
                            std::to_string(new_value) + ".",
                        a_IS_ENABLED_PRINT);
                } else {
                    throw std::logic_error(utility::format_error_location(
                        __FILE__, __LINE__, __func__,
                        "An initial value violates the lower or upper bound "
                        "constraint."));
                }
            }
        }
    }
    utility::print_message("Done.", a_IS_ENABLED_PRINT);
}
}  // namespace verifier
}  // namespace printemps
#endif
/*****************************************************************************/
// END
/*****************************************************************************/