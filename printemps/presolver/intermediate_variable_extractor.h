/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_PRESOLVER_INTERMEDIATE_VARIABLE_EXTRACTOR_H__
#define PRINTEMPS_PRESOLVER_INTERMEDIATE_VARIABLE_EXTRACTOR_H__

namespace printemps {
namespace presolver {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
int extract_dependent_intermediate_variables(
    model::Model<T_Variable, T_Expression> *a_model_ptr,  //
    const bool                              a_IS_ENABLED_PRINT) {
    /**
     * NOTE: This function cannot be constexpr with clang for std::vector<bool>.
     */
    utility::print_single_line(a_IS_ENABLED_PRINT);
    utility::print_message("Extracting dependent intermediate variables...",
                           a_IS_ENABLED_PRINT);

    int number_of_newly_extracted_dependent_intermediate_vairables = 0;

    auto &intermediate_ptrs =
        a_model_ptr->constraint_type_reference().intermediate_ptrs;

    int intermediates_size = intermediate_ptrs.size();

    std::vector<bool> has_dependent_intermediate_variable_flags(
        intermediates_size, true);

    std::unordered_set<model_component::Variable<T_Variable, T_Expression> *>
        intermediate_variables;

    for (auto i = 0; i < intermediates_size; i++) {
        auto intermediate_variable_ptr =
            intermediate_ptrs[i]->aux_variable_ptr();
        if (intermediate_variables.find(intermediate_variable_ptr) ==
            intermediate_variables.end()) {
            intermediate_variables.insert(intermediate_variable_ptr);
        } else {
            has_dependent_intermediate_variable_flags[i] = false;
        }
    }

    std::vector<model_component::Constraint<T_Variable, T_Expression>>
        additional_constraints;

    for (auto i = 0; i < intermediates_size; i++) {
        auto constraint_ptr = intermediate_ptrs[i];
        if (!constraint_ptr->is_enabled()) {
            continue;
        }
        auto intermediate_variable_ptr = constraint_ptr->aux_variable_ptr();
        if (has_dependent_intermediate_variable_flags[i]) {
            utility::print_message(
                "The decision variable " + intermediate_variable_ptr->name() +
                    " in the constraint " + constraint_ptr->name() +
                    " was extracted as a dependent intermediate variable. ",
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

            if (constraint_ptr->has_aux_lower_bound()) {
                additional_constraints.emplace_back(
                    sign * modified_expression >=
                    intermediate_variable_ptr->lower_bound());
                additional_constraints.back().set_name(constraint_ptr->name() +
                                                       "_greater");
            }

            if (constraint_ptr->has_aux_upper_bound()) {
                additional_constraints.emplace_back(
                    sign * modified_expression <=
                    intermediate_variable_ptr->upper_bound());
                additional_constraints.back().set_name(constraint_ptr->name() +
                                                       "_less");
            }
            number_of_newly_extracted_dependent_intermediate_vairables++;
        }
    }

    if (additional_constraints.size() > 0) {
        int   count                       = 0;
        auto &additional_constraint_proxy = a_model_ptr->create_constraints(
            "additional", additional_constraints.size());
        for (auto &&constraint : additional_constraints) {
            additional_constraint_proxy(count) = constraint;
            additional_constraint_proxy(count).set_name(constraint.name());
            utility::print_message(
                "An extra constraint " + constraint.name() + " was added.",
                a_IS_ENABLED_PRINT);
            count++;
        }
    }

    utility::print_message("Done.", a_IS_ENABLED_PRINT);
    return number_of_newly_extracted_dependent_intermediate_vairables;
}

/*****************************************************************************/
template <class T_Variable, class T_Expression>
constexpr int eliminate_dependent_intermediate_variables(
    model::Model<T_Variable, T_Expression> *a_model_ptr,  //
    const bool                              a_IS_ENABLED_PRINT) {
    utility::print_single_line(a_IS_ENABLED_PRINT);
    utility::print_message("Eliminating dependent intermediate variables...",
                           a_IS_ENABLED_PRINT);

    int number_of_newly_eliminated_dependent_intermediate_vairables = 0;

    /// Objective
    {
        auto &sensitivities =
            a_model_ptr->objective().expression().sensitivities();

        for (auto &&variable_ptr :
             a_model_ptr->variable_reference().intermediate_variable_ptrs) {
            if (sensitivities.find(variable_ptr) != sensitivities.end()) {
                auto dependent_constraint_ptr =
                    variable_ptr->dependent_constraint_ptr();
                auto &constraint_sensitivities =
                    dependent_constraint_ptr->expression().sensitivities();

                double sign =
                    constraint_sensitivities.at(variable_ptr) > 0 ? -1.0 : 1.0;

                a_model_ptr->objective().expression().substitute(
                    variable_ptr,
                    sign * dependent_constraint_ptr->expression());
                number_of_newly_eliminated_dependent_intermediate_vairables++;

                utility::print_message(
                    "The dependent intermediate variable " +
                        variable_ptr->name() +
                        " in the objective function was eliminated. ",
                    a_IS_ENABLED_PRINT);
            }
        }
    }

    /// Constraint
    for (auto &&variable_ptr :
         a_model_ptr->variable_reference().intermediate_variable_ptrs) {
        for (auto &&constrinat_ptr : variable_ptr->related_constraint_ptrs()) {
            if (constrinat_ptr == variable_ptr->dependent_constraint_ptr()) {
                continue;
            }

            auto dependent_constraint_ptr =
                variable_ptr->dependent_constraint_ptr();
            auto &sensitivities =
                dependent_constraint_ptr->expression().sensitivities();

            double sign = sensitivities.at(variable_ptr) > 0 ? -1.0 : 1.0;

            constrinat_ptr->expression().substitute(
                variable_ptr, sign * dependent_constraint_ptr->expression());

            number_of_newly_eliminated_dependent_intermediate_vairables++;

            utility::print_message(
                "The dependent intermediate variable " + variable_ptr->name() +
                    " in the constraint " + constrinat_ptr->name() +
                    " was eliminated. ",
                a_IS_ENABLED_PRINT);
        }
    }

    utility::print_message("Done.", a_IS_ENABLED_PRINT);
    return number_of_newly_eliminated_dependent_intermediate_vairables;
}

}  // namespace presolver
}  // namespace printemps
#endif
/*****************************************************************************/
// END
/*****************************************************************************/