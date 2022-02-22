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
class SelectionExtractor {
   private:
    model::Model<T_Variable, T_Expression> *m_model_ptr;
    std::vector<model_component::Selection<T_Variable, T_Expression>>
        m_selections;

    /*************************************************************************/
    inline constexpr std::vector<
        model_component::Selection<T_Variable, T_Expression>>
    extract_raw_selections(void) {
        auto &set_patritioning_ptrs =
            m_model_ptr->constraint_type_reference().set_partitioning_ptrs;

        std::vector<model_component::Selection<T_Variable, T_Expression>>
            raw_selections;
        for (auto &&constraint_ptr : set_patritioning_ptrs) {
            if (!constraint_ptr->is_enabled()) {
                continue;
            }

            model_component::Selection<T_Variable, T_Expression> selection(
                constraint_ptr);
            raw_selections.push_back(selection);
        }
        return raw_selections;
    }

   public:
    /*************************************************************************/
    SelectionExtractor(void) {
        this->initialize();
    }

    /*************************************************************************/
    SelectionExtractor(model::Model<T_Variable, T_Expression> *a_model_ptr) {
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
        m_selections.clear();
    }

    /*****************************************************************************/
    inline constexpr void extract(
        const option::selection_mode::SelectionMode &a_SELECTION_MODE,
        const bool                                   a_IS_ENABLED_PRINT) {
        switch (a_SELECTION_MODE) {
            case option::selection_mode::None: {
                break;
            }
            case option::selection_mode::Defined: {
                extract_selections_by_defined_order(a_IS_ENABLED_PRINT);
                break;
            }
            case option::selection_mode::Smaller: {
                extract_selections_by_number_of_variables_order(
                    true, a_IS_ENABLED_PRINT);
                break;
            }
            case option::selection_mode::Larger: {
                extract_selections_by_number_of_variables_order(
                    false, a_IS_ENABLED_PRINT);
                break;
            }
            case option::selection_mode::Independent: {
                extract_independent_selections(a_IS_ENABLED_PRINT);
                break;
            }
            default: {
                throw std::logic_error(utility::format_error_location(
                    __FILE__, __LINE__, __func__,
                    "The specified selection mode is invalid."));
            }
        }
    }

    /*****************************************************************************/
    inline constexpr void extract_selections_by_defined_order(
        const bool a_IS_ENABLED_PRINT) {
        utility::print_single_line(a_IS_ENABLED_PRINT);
        utility::print_message("Extracting selection by defined order...",
                               a_IS_ENABLED_PRINT);

        std::vector<model_component::Selection<T_Variable, T_Expression>>
             selections;
        auto raw_selections = this->extract_raw_selections();

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
                selections.push_back(selection);
                extracted_variable_ptrs.insert(extracted_variable_ptrs.end(),
                                               selection.variable_ptrs.begin(),
                                               selection.variable_ptrs.end());
            }
        }

        for (auto &&selection : selections) {
            selection.constraint_ptr->disable();
            selection.setup_related_constraint_ptrs();
        }

        m_selections = selections;
        m_model_ptr->set_selections(selections);

        utility::print_message("Done.", a_IS_ENABLED_PRINT);
    }

    /*****************************************************************************/
    inline constexpr void extract_selections_by_number_of_variables_order(
        const bool a_IS_SMALLER_ORDER, const bool a_IS_ENABLED_PRINT) {
        utility::print_single_line(a_IS_ENABLED_PRINT);
        if (a_IS_SMALLER_ORDER) {
            utility::print_message(
                "Extracting selection by order of smaller number of "
                "variables...",
                a_IS_ENABLED_PRINT);
        } else {
            utility::print_message(
                "Extracting selection by order of larger number of "
                "variables... ",
                a_IS_ENABLED_PRINT);
        }

        std::vector<model_component::Selection<T_Variable, T_Expression>>
             selections;
        auto raw_selections = this->extract_raw_selections();

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
                selections.push_back(selection);
                extracted_variable_ptrs.insert(extracted_variable_ptrs.end(),
                                               selection.variable_ptrs.begin(),
                                               selection.variable_ptrs.end());
            }
        }

        for (auto &&selection : selections) {
            selection.constraint_ptr->disable();
            selection.setup_related_constraint_ptrs();
        }

        m_selections = selections;
        m_model_ptr->set_selections(selections);

        utility::print_message("Done.", a_IS_ENABLED_PRINT);
    }

    /*****************************************************************************/
    inline constexpr void extract_independent_selections(
        const bool a_IS_ENABLED_PRINT) {
        utility::print_single_line(a_IS_ENABLED_PRINT);
        utility::print_message("Extracting independent selection variables...",
                               a_IS_ENABLED_PRINT);

        std::vector<model_component::Selection<T_Variable, T_Expression>>
            selections;

        auto raw_selections = this->extract_raw_selections();

        std::vector<model_component::Variable<T_Variable, T_Expression> *>
            extracted_variable_ptrs;

        const int RAW_SELECTIONS_SIZE = raw_selections.size();
        for (auto i = 0; i < RAW_SELECTIONS_SIZE; i++) {
            bool has_overlap = false;
            for (auto &&variable_ptr : raw_selections[i].variable_ptrs) {
                for (auto j = 0; j < RAW_SELECTIONS_SIZE; j++) {
                    if (j != i &&
                        std::find(raw_selections[j].variable_ptrs.begin(),
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
                    "The constraint " +
                        raw_selections[i].constraint_ptr->name() +
                        " was detected as selection constraint.",
                    a_IS_ENABLED_PRINT);
                selections.push_back(raw_selections[i]);
                extracted_variable_ptrs.insert(
                    extracted_variable_ptrs.end(),
                    raw_selections[i].variable_ptrs.begin(),
                    raw_selections[i].variable_ptrs.end());
            }
        }

        for (auto &&selection : selections) {
            selection.constraint_ptr->disable();
            selection.setup_related_constraint_ptrs();
        }

        m_selections = selections;
        m_model_ptr->set_selections(selections);

        utility::print_message("Done.", a_IS_ENABLED_PRINT);
    }

    /*************************************************************************/
    inline constexpr const std::vector<
        model_component::Selection<T_Variable, T_Expression>>
        &selections(void) const {
        return m_selections;
    }

    /*************************************************************************/
    inline constexpr std::vector<
        model_component::Selection<T_Variable, T_Expression>>
        &selections(void) {
        return m_selections;
    }
};
}  // namespace presolver
}  // namespace printemps
#endif
/*****************************************************************************/
// END
/*****************************************************************************/