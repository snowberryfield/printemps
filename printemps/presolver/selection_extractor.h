/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_PRESOLVER_SELECTION_EXTRACTOR_H__
#define PRINTEMPS_PRESOLVER_SELECTION_EXTRACTOR_H__

namespace printemps {
namespace presolver {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
constexpr std::vector<model_component::Selection<T_Variable, T_Expression>>
convert_to_selections(
    const std::vector<model_component::Constraint<T_Variable, T_Expression> *>
        &a_CONSTRAINT_PTRS) {
    std::vector<model_component::Selection<T_Variable, T_Expression>>
        selections;
    for (auto &&constraint_ptr : a_CONSTRAINT_PTRS) {
        if (!constraint_ptr->is_enabled()) {
            continue;
        }

        model_component::Selection<T_Variable, T_Expression> selection(
            constraint_ptr);
        selections.push_back(selection);
    }
    return selections;
}

/*****************************************************************************/
template <class T_Variable, class T_Expression>
constexpr void extract_selections_by_defined_order(
    model::Model<T_Variable, T_Expression> *a_model_ptr,
    const bool                              a_IS_ENABLED_PRINT) {
    /**
     * NOTE: This function should be called from extract_selections().
     */
    utility::print_single_line(a_IS_ENABLED_PRINT);
    utility::print_message("Extracting selection by defined order...",
                           a_IS_ENABLED_PRINT);

    std::vector<model_component::Selection<T_Variable, T_Expression>>
        included_selections;
    std::vector<model_component::Selection<T_Variable, T_Expression>>
        raw_selections = convert_to_selections(
            a_model_ptr->constraint_type_reference().set_partitioning_ptrs);

    std::vector<model_component::Variable<T_Variable, T_Expression> *>
        extracted_variable_ptrs;

    for (auto &&selection : raw_selections) {
        bool has_excluded_variable_ptr = false;
        for (auto &&variable_ptr : selection.variable_ptrs) {
            if (std::find(extracted_variable_ptrs.begin(),
                          extracted_variable_ptrs.end(),
                          variable_ptr) != extracted_variable_ptrs.end()) {
                has_excluded_variable_ptr = true;
                break;
            }
        }
        if (has_excluded_variable_ptr) {
            continue;
        } else {
            utility::print_message(  //
                "The constraint " + selection.constraint_ptr->name() +
                    " was detected as selection constraint.",
                a_IS_ENABLED_PRINT);
            included_selections.push_back(selection);
            extracted_variable_ptrs.insert(extracted_variable_ptrs.end(),
                                           selection.variable_ptrs.begin(),
                                           selection.variable_ptrs.end());
        }
    }

    for (auto &&selection : included_selections) {
        selection.constraint_ptr->disable();
        selection.setup_related_constraint_ptrs();
    }

    a_model_ptr->set_selections(included_selections);

    utility::print_message("Done.", a_IS_ENABLED_PRINT);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression>
constexpr void extract_selections_by_number_of_variables_order(
    model::Model<T_Variable, T_Expression> *a_model_ptr,         //
    const bool                              a_IS_SMALLER_ORDER,  //
    const bool                              a_IS_ENABLED_PRINT) {
    /**
     * NOTE: This function should be called from extract_selections().
     */
    utility::print_single_line(a_IS_ENABLED_PRINT);
    if (a_IS_SMALLER_ORDER) {
        utility::print_message(
            "Extracting selection by order of smaller number of variables...",
            a_IS_ENABLED_PRINT);
    } else {
        utility::print_message(
            "Extracting selection by order of larger number of variables... ",
            a_IS_ENABLED_PRINT);
    }

    std::vector<model_component::Selection<T_Variable, T_Expression>>
        included_selections;
    std::vector<model_component::Selection<T_Variable, T_Expression>>
        raw_selections = convert_to_selections(
            a_model_ptr->constraint_type_reference().set_partitioning_ptrs);

    if (a_IS_SMALLER_ORDER) {
        std::sort(raw_selections.begin(), raw_selections.end(),
                  [](const auto &a_LHS, const auto &a_RHS) {
                      return a_LHS.variable_ptrs.size() <
                             a_RHS.variable_ptrs.size();
                  });
    } else {
        std::sort(raw_selections.begin(), raw_selections.end(),
                  [](const auto &a_LHS, const auto &a_RHS) {
                      return a_LHS.variable_ptrs.size() >
                             a_RHS.variable_ptrs.size();
                  });
    }

    std::vector<model_component::Variable<T_Variable, T_Expression> *>
        extracted_variable_ptrs;

    for (auto &&selection : raw_selections) {
        bool has_excluded_variable_ptr = false;
        for (auto &&variable_ptr : selection.variable_ptrs) {
            if (std::find(extracted_variable_ptrs.begin(),
                          extracted_variable_ptrs.end(),
                          variable_ptr) != extracted_variable_ptrs.end()) {
                has_excluded_variable_ptr = true;
                break;
            }
        }
        if (has_excluded_variable_ptr) {
            continue;
        } else {
            utility::print_message(  //
                "The constraint " + selection.constraint_ptr->name() +
                    " was detected as selection constraint.",
                a_IS_ENABLED_PRINT);
            included_selections.push_back(selection);
            extracted_variable_ptrs.insert(extracted_variable_ptrs.end(),
                                           selection.variable_ptrs.begin(),
                                           selection.variable_ptrs.end());
        }
    }

    for (auto &&selection : included_selections) {
        selection.constraint_ptr->disable();
        selection.setup_related_constraint_ptrs();
    }

    a_model_ptr->set_selections(included_selections);

    utility::print_message("Done.", a_IS_ENABLED_PRINT);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression>
constexpr void extract_independent_selections(
    model::Model<T_Variable, T_Expression> *a_model_ptr,
    const bool                              a_IS_ENABLED_PRINT) {
    /**
     * NOTE: This function should be called from extract_selections().
     */
    utility::print_single_line(a_IS_ENABLED_PRINT);
    utility::print_message("Extracting independent selection variables...",
                           a_IS_ENABLED_PRINT);

    std::vector<model_component::Selection<T_Variable, T_Expression>>
        included_selections;
    std::vector<model_component::Selection<T_Variable, T_Expression>>
        raw_selections = convert_to_selections(
            a_model_ptr->constraint_type_reference().set_partitioning_ptrs);

    std::vector<model_component::Variable<T_Variable, T_Expression> *>
        extracted_variable_ptrs;

    const int RAW_SELECTIONS_SIZE = raw_selections.size();
    for (auto i = 0; i < RAW_SELECTIONS_SIZE; i++) {
        bool has_overlap = false;
        for (auto &&variable_ptr : raw_selections[i].variable_ptrs) {
            for (auto j = 0; j < RAW_SELECTIONS_SIZE; j++) {
                if (j != i && std::find(raw_selections[j].variable_ptrs.begin(),
                                        raw_selections[j].variable_ptrs.end(),
                                        variable_ptr) !=
                                  raw_selections[j].variable_ptrs.end()) {
                    has_overlap = true;
                    break;
                }
            }
            if (has_overlap) {
                break;
            }
        }
        if (has_overlap) {
            continue;
        } else {
            utility::print_message(  //
                "The constraint " + raw_selections[i].constraint_ptr->name() +
                    " was detected as selection constraint.",
                a_IS_ENABLED_PRINT);
            included_selections.push_back(raw_selections[i]);
            extracted_variable_ptrs.insert(
                extracted_variable_ptrs.end(),
                raw_selections[i].variable_ptrs.begin(),
                raw_selections[i].variable_ptrs.end());
        }
    }

    for (auto &&selection : included_selections) {
        selection.constraint_ptr->disable();
        selection.setup_related_constraint_ptrs();
    }

    a_model_ptr->set_selections(included_selections);

    utility::print_message("Done.", a_IS_ENABLED_PRINT);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression>
constexpr void extract_selections(
    model::Model<T_Variable, T_Expression> *     a_model_ptr,  //
    const option::selection_mode::SelectionMode &a_SELECTION_MODE,
    const bool                                   a_IS_ENABLED_PRINT) {
    switch (a_SELECTION_MODE) {
        case option::selection_mode::None: {
            break;
        }
        case option::selection_mode::Defined: {
            extract_selections_by_defined_order(a_model_ptr,  //
                                                a_IS_ENABLED_PRINT);
            break;
        }
        case option::selection_mode::Smaller: {
            extract_selections_by_number_of_variables_order(a_model_ptr,  //
                                                            true,         //
                                                            a_IS_ENABLED_PRINT);
            break;
        }
        case option::selection_mode::Larger: {
            extract_selections_by_number_of_variables_order(a_model_ptr,  //
                                                            false,        //
                                                            a_IS_ENABLED_PRINT);
            break;
        }
        case option::selection_mode::Independent: {
            extract_independent_selections(a_model_ptr,  //
                                           a_IS_ENABLED_PRINT);
            break;
        }
        default: {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The specified selection mode is invalid."));
        }
    }
}

}  // namespace presolver
}  // namespace printemps
#endif
/*****************************************************************************/
// END
/*****************************************************************************/