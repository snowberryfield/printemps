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
    const bool                              a_IS_ENABLED_PRINT) {
    utility::print_single_line(a_IS_ENABLED_PRINT);
    utility::print_message("Extracting independent intermediate variables...",
                           a_IS_ENABLED_PRINT);

    int number_of_newly_extracted_independent_intermediate_vairables = 0;
    std::unordered_map<model::Variable<T_Variable, T_Expression> *, int>
        intermediate_variable_counts;

    for (auto &&constraint_ptr :
         a_model->constraint_type_reference().intermediate_ptrs) {
        auto intermediate_variable_ptr =
            constraint_ptr->intermediate_variable_ptr();
        if (intermediate_variable_counts.find(intermediate_variable_ptr) !=
            intermediate_variable_counts.end()) {
            intermediate_variable_counts[intermediate_variable_ptr]++;
        } else {
            intermediate_variable_counts[intermediate_variable_ptr] = 1;
        }
    }

    std::vector<model::Constraint<T_Variable, T_Expression>>
        additional_constraints;

    for (auto &&constraint_ptr :
         a_model->constraint_type_reference().intermediate_ptrs) {
        if (!constraint_ptr->is_enabled()) {
            continue;
        }
        auto intermediate_variable_ptr =
            constraint_ptr->intermediate_variable_ptr();
        if (intermediate_variable_counts[intermediate_variable_ptr] == 1) {
            utility::print_message(
                "The decision variable " + intermediate_variable_ptr->name() +
                    " in the constraint " + constraint_ptr->name() +
                    " was extracted as an independent intermediate variable. ",
                a_IS_ENABLED_PRINT);

            constraint_ptr->disable();
            intermediate_variable_ptr->set_dependent_constraint_ptr(
                constraint_ptr);

            auto & sensitivities = constraint_ptr->expression().sensitivities();
            double sign =
                sensitivities.at(intermediate_variable_ptr) > 0 ? -1.0 : 1.0;

            auto modified_expression = constraint_ptr->expression().copy();
            modified_expression.sensitivities().erase(
                intermediate_variable_ptr);

            if (constraint_ptr->has_intermediate_lower_bound()) {
                additional_constraints.emplace_back(
                    sign * modified_expression >=
                    intermediate_variable_ptr->lower_bound());
                additional_constraints.back().set_name(constraint_ptr->name() +
                                                       "_lower");
            }

            if (constraint_ptr->has_intermediate_upper_bound()) {
                additional_constraints.emplace_back(
                    sign * modified_expression <=
                    intermediate_variable_ptr->upper_bound());
                additional_constraints.back().set_name(constraint_ptr->name() +
                                                       "_upper");
            }
            number_of_newly_extracted_independent_intermediate_vairables++;
        }
    }

    if (additional_constraints.size() > 0) {
        int   count                       = 0;
        auto &additional_constraint_proxy = a_model->create_constraints(
            "additional", additional_constraints.size());
        for (auto &&constraint : additional_constraints) {
            additional_constraint_proxy(count++) = constraint;
            utility::print_info(
                "An extra constraint " + constraint.name() + " was added.",
                a_IS_ENABLED_PRINT);
        }
    }

    utility::print_message("Done.", a_IS_ENABLED_PRINT);
    return number_of_newly_extracted_independent_intermediate_vairables;
}

/*****************************************************************************/
template <class T_Variable, class T_Expression>
constexpr int eliminate_independent_intermediate_variables(
    model::Model<T_Variable, T_Expression> *a_model,  //
    const bool                              a_IS_ENABLED_PRINT) {
    utility::print_single_line(a_IS_ENABLED_PRINT);
    utility::print_message("Eliminating independent intermediate variables...",
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
    for (auto &&constrinat_ptr :
         a_model->constraint_reference().constraint_ptrs) {
        auto sensitivities = constrinat_ptr->expression().sensitivities();

        for (auto &&variable_ptr :
             a_model->variable_reference().intermediate_variable_ptrs) {
            if (constrinat_ptr == variable_ptr->dependent_constraint_ptr()) {
                continue;
            }
            if (sensitivities.find(variable_ptr) != sensitivities.end()) {
                auto dependent_constraint_ptr =
                    variable_ptr->dependent_constraint_ptr();
                auto &sensitivities =
                    dependent_constraint_ptr->expression().sensitivities();

                double sign = sensitivities.at(variable_ptr) > 0 ? -1.0 : 1.0;

                constrinat_ptr->expression().substitute(
                    variable_ptr,
                    sign * dependent_constraint_ptr->expression());

                number_of_newly_eliminated_independent_intermediate_vairables++;

                utility::print_message(
                    "The independent intermediate variable " +
                        variable_ptr->name() + " in the constraint " +
                        constrinat_ptr->name() + " was eliminated. ",
                    a_IS_ENABLED_PRINT);
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