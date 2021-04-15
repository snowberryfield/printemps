/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_PRESOLVER_INTERMEDIATE_VARIABLE_EXTRACTOR_H__
#define PRINTEMPS_PRESOLVER_INTERMEDIATE_VARIABLE_EXTRACTOR_H__

namespace printemps {
namespace model {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Variable;

/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Constraint;

}  // namespace model
}  // namespace printemps

namespace printemps {
namespace presolver {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
constexpr int extract_independent_intermediate_variables(
    model::Model<T_Variable, T_Expression> *a_model,  //
    const int a_ITERATION, const bool a_IS_ENABLED_PRINT) {
    utility::print_single_line(a_IS_ENABLED_PRINT);
    utility::print_message(
        "Extracting independent intermediate variables...(Round " +
            std::to_string(a_ITERATION) + ")",
        a_IS_ENABLED_PRINT);

    int number_of_newly_extracted_independent_intermediate_vairables = 0;
    std::unordered_map<model::Variable<T_Variable, T_Expression> *, int>
        intermediate_variable_counts;

    for (auto &&proxy : a_model->constraint_proxies()) {
        for (auto &&constraint : proxy.flat_indexed_constraints()) {
            if (constraint.is_intermediate()) {
                auto intermediate_variable_ptr =
                    constraint.intermediate_variable_ptr();
                if (intermediate_variable_counts.find(
                        intermediate_variable_ptr) !=
                    intermediate_variable_counts.end()) {
                    intermediate_variable_counts[intermediate_variable_ptr]++;
                } else {
                    intermediate_variable_counts[intermediate_variable_ptr] = 1;
                }
            }
        }
    }

    for (auto &&proxy : a_model->constraint_proxies()) {
        for (auto &&constraint : proxy.flat_indexed_constraints()) {
            if (constraint.is_intermediate()) {
                auto intermediate_variable_ptr =
                    constraint.intermediate_variable_ptr();
                if (intermediate_variable_counts[intermediate_variable_ptr] ==
                    1) {
                    utility::print_message(
                        "The decision variable " +
                            intermediate_variable_ptr->name() +
                            " in the constraint " + constraint.name() +
                            " was extracted as an independent intermediate "
                            "variable. ",
                        a_IS_ENABLED_PRINT);
                    constraint.disable();
                    intermediate_variable_ptr->set_dependent_constraint_ptr(
                        &constraint);
                    number_of_newly_extracted_independent_intermediate_vairables++;
                }
            }
        }
    }
    utility::print_message("Done.", a_IS_ENABLED_PRINT);
    return number_of_newly_extracted_independent_intermediate_vairables;
}

/*****************************************************************************/
template <class T_Variable, class T_Expression>
constexpr int eliminate_independent_intermediate_variables(
    model::Model<T_Variable, T_Expression> *a_model,  //
    const int a_ITERATION, const bool a_IS_ENABLED_PRINT) {
    utility::print_single_line(a_IS_ENABLED_PRINT);
    utility::print_message(
        "Eliminating independent intermediate variables...(Round " +
            std::to_string(a_ITERATION) + ")",
        a_IS_ENABLED_PRINT);

    int number_of_newly_eliminated_independent_intermediate_vairables = 0;

    /// Objective
    {
        auto sensitivities = a_model->objective().expression().sensitivities();

        for (auto &&variable_ptr :
             a_model->variable_reference().intermediate_variable_ptrs) {
            if (sensitivities.find(variable_ptr) != sensitivities.end()) {
                auto dependent_constraint_ptr =
                    variable_ptr->dependent_constraint_ptr();
                auto &sensitivities =
                    dependent_constraint_ptr->expression().sensitivities();

                double sign = sensitivities.at(variable_ptr) > 0 ? -1.0 : 1.0;

                a_model->objective().expression().substitute(
                    variable_ptr,
                    sign * dependent_constraint_ptr->expression());
                number_of_newly_eliminated_independent_intermediate_vairables++;

                utility::print_message(
                    "The independent intermediate variable " +
                        variable_ptr->name() +
                        " in the objective function was eliminated. ",
                    a_IS_ENABLED_PRINT);
            }
        }
    }

    /// Constraint
    for (auto &&proxy : a_model->constraint_proxies()) {
        for (auto &&constraint : proxy.flat_indexed_constraints()) {
            auto sensitivities = constraint.expression().sensitivities();

            for (auto &&variable_ptr :
                 a_model->variable_reference().intermediate_variable_ptrs) {
                if (&constraint == variable_ptr->dependent_constraint_ptr()) {
                    continue;
                }
                if (sensitivities.find(variable_ptr) != sensitivities.end()) {
                    auto dependent_constraint_ptr =
                        variable_ptr->dependent_constraint_ptr();
                    auto &sensitivities =
                        dependent_constraint_ptr->expression().sensitivities();

                    double sign =
                        sensitivities.at(variable_ptr) > 0 ? -1.0 : 1.0;

                    constraint.expression().substitute(
                        variable_ptr,
                        sign * dependent_constraint_ptr->expression());

                    number_of_newly_eliminated_independent_intermediate_vairables++;

                    utility::print_message(
                        "The independent intermediate variable " +
                            variable_ptr->name() + " in the constraint " +
                            constraint.name() + " was eliminated. ",
                        a_IS_ENABLED_PRINT);
                }
            }
        }
    }
    utility::print_message("Done.", a_IS_ENABLED_PRINT);
    return number_of_newly_eliminated_independent_intermediate_vairables;
}

}  // namespace presolver
}  // namespace printemps
#endif
/*****************************************************************************/
// END
/*****************************************************************************/