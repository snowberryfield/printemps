/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_PRESOLVER_FLIPPABLE_VARIABLE_PAIR_EXTRACTOR_H__
#define PRINTEMPS_PRESOLVER_FLIPPABLE_VARIABLE_PAIR_EXTRACTOR_H__

namespace printemps {
namespace presolver {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct FlippableVariablePair {
    model_component::Variable<T_Variable, T_Expression> *variable_ptr_first;
    model_component::Variable<T_Variable, T_Expression> *variable_ptr_second;
    int    number_of_common_element;
    double overlap_rate;

    FlippableVariablePair(  //
        model_component::Variable<T_Variable, T_Expression>
            *a_variable_ptr_first,
        model_component::Variable<T_Variable, T_Expression>
            *        a_variable_ptr_second,
        const int    a_NUMBER_OF_COMMON_ELEMENTS,  //
        const double a_OVERLAP_RATE)
        : variable_ptr_first(a_variable_ptr_first),
          variable_ptr_second(a_variable_ptr_second),
          number_of_common_element(a_NUMBER_OF_COMMON_ELEMENTS),
          overlap_rate(a_OVERLAP_RATE) {
        /// nothing to do
    }
};

/*****************************************************************************/
template <class T_Variable, class T_Expression>
inline void write_flippable_variable_pairs(
    const std::vector<FlippableVariablePair<T_Variable, T_Expression> >
        &              a_FLIPPABLE_VARIABLE_PAIRS,
    const std::string &a_FILE_NAME) {
    std::ofstream ofs(a_FILE_NAME);
    for (const auto &pair : a_FLIPPABLE_VARIABLE_PAIRS) {
        ofs << pair.variable_ptr_first->name() << " "
            << pair.variable_ptr_second->name() << " "
            << pair.variable_ptr_first->related_constraint_ptrs().size() << " "
            << pair.variable_ptr_second->related_constraint_ptrs().size() << " "
            << pair.number_of_common_element << " " << pair.overlap_rate << " "
            << std::endl;
    }
    ofs.close();
}

/*****************************************************************************/
template <class T_Variable, class T_Expression>
std::vector<FlippableVariablePair<T_Variable, T_Expression> >
extract_flippable_variable_pairs(
    const std::vector<model_component::Constraint<T_Variable, T_Expression> *>
        &      a_CONSTRAINT_PTRS,         //
    const int  a_MINIMUM_COMMON_ELEMENT,  //
    const bool a_IS_ENABLED_PRINT) {
    utility::print_single_line(a_IS_ENABLED_PRINT);
    utility::print_message("Extracting flippable variable pairs...",
                           a_IS_ENABLED_PRINT);

    std::unordered_set<model_component::Variable<T_Variable, T_Expression> *>
        variable_ptrs_set;
    for (const auto &constraint_ptr : a_CONSTRAINT_PTRS) {
        for (const auto &sensitivity :
             constraint_ptr->expression().sensitivities()) {
            auto variable_ptr = sensitivity.first;
            if (!variable_ptr->is_fixed() &&
                (variable_ptr->sense() ==
                     model_component::VariableSense::Binary ||
                 variable_ptr->sense() ==
                     model_component::VariableSense::Selection) &&
                static_cast<int>(
                    variable_ptr->related_constraint_ptrs().size()) >=
                    a_MINIMUM_COMMON_ELEMENT) {
                variable_ptrs_set.insert(variable_ptr);
            }
        }
    }
    auto      variable_ptrs  = utility::to_vector(variable_ptrs_set);
    const int VARIABLES_SIZE = variable_ptrs.size();

    std::vector<FlippableVariablePair<T_Variable, T_Expression> >
        flippable_variable_pairs;
    flippable_variable_pairs.reserve(VARIABLES_SIZE);

    for (auto i = 0; i < VARIABLES_SIZE; i++) {
        auto &constraint_ptrs_first =
            variable_ptrs[i]->related_constraint_ptrs();
        for (auto j = i + 1; j < VARIABLES_SIZE; j++) {
            auto &constraint_ptrs_second =
                variable_ptrs[j]->related_constraint_ptrs();
            auto intersection_set = utility::intersection_set(
                constraint_ptrs_first, constraint_ptrs_second);
            auto union_set = utility::union_set(constraint_ptrs_first,
                                                constraint_ptrs_second);

            int    number_of_common_elements = intersection_set.size();
            double overlap_rate =
                static_cast<double>(number_of_common_elements) /
                static_cast<double>(union_set.size());

            if (number_of_common_elements >= a_MINIMUM_COMMON_ELEMENT) {
                flippable_variable_pairs.emplace_back(
                    variable_ptrs[i], variable_ptrs[j],
                    number_of_common_elements, overlap_rate);
                utility::print_message(
                    "A flippable variable pair (" + variable_ptrs[i]->name() +
                        ", " + variable_ptrs[j]->name() + ")" + " was found " +
                        "(" + std::to_string(number_of_common_elements) + "/" +
                        std::to_string(overlap_rate) + ").",
                    a_IS_ENABLED_PRINT);
            }
        }
    }
    std::sort(flippable_variable_pairs.begin(), flippable_variable_pairs.end(),
              [](const auto &a_FIRST, const auto &a_SECOND) {
                  return a_FIRST.overlap_rate > a_SECOND.overlap_rate;
              });
    utility::print_message(std::to_string(flippable_variable_pairs.size()) +
                               " flippable variable pairs were found.",
                           a_IS_ENABLED_PRINT);

    utility::print_message("Done.", a_IS_ENABLED_PRINT);
    return flippable_variable_pairs;
}

}  // namespace presolver
}  // namespace printemps
#endif
/*****************************************************************************/
// END
/*****************************************************************************/