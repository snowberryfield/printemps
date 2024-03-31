/*****************************************************************************/
// Copyright (c) 2020-2024 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_PREPROCESS_FLIPPABLE_VARIABLE_PAIR_EXTRACTOR_H__
#define PRINTEMPS_PREPROCESS_FLIPPABLE_VARIABLE_PAIR_EXTRACTOR_H__

#include "flippable_variable_pair.h"

namespace printemps::preprocess {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class FlippableVariablePairExtractor {
   private:
    model::Model<T_Variable, T_Expression> *                     m_model_ptr;
    std::vector<FlippableVariablePair<T_Variable, T_Expression>> m_pairs;

   public:
    /*************************************************************************/
    FlippableVariablePairExtractor(void) {
        this->initialize();
    }

    /*************************************************************************/
    FlippableVariablePairExtractor(
        model::Model<T_Variable, T_Expression> *a_model_ptr) {
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
        m_pairs.clear();
    }

    /*************************************************************************/
    inline void extract_pairs(const int  a_MINIMUM_COMMON_ELEMENT,  //
                              const bool a_IS_ENABLED_PRINT) {
        utility::print_single_line(a_IS_ENABLED_PRINT);
        utility::print_message("Extracting flippable variable pairs...",
                               a_IS_ENABLED_PRINT);

        const auto &a_CONSTRAINT_PTRS =
            m_model_ptr->constraint_reference().enabled_constraint_ptrs;

        std::unordered_set<
            model_component::Variable<T_Variable, T_Expression> *>
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

        std::vector<FlippableVariablePair<T_Variable, T_Expression>> pairs;
        pairs.reserve(VARIABLES_SIZE);

        for (auto i = 0; i < VARIABLES_SIZE; i++) {
            std::unordered_set<
                model_component::Constraint<T_Variable, T_Expression> *>
                constraint_ptrs_first(
                    variable_ptrs[i]->related_constraint_ptrs().begin(),
                    variable_ptrs[i]->related_constraint_ptrs().end());
            for (auto j = i + 1; j < VARIABLES_SIZE; j++) {
                std::unordered_set<
                    model_component::Constraint<T_Variable, T_Expression> *>
                    constraint_ptrs_second(
                        variable_ptrs[j]->related_constraint_ptrs().begin(),
                        variable_ptrs[j]->related_constraint_ptrs().end());

                auto intersection_set = utility::intersection_set(
                    constraint_ptrs_first, constraint_ptrs_second);
                auto union_set = utility::union_set(constraint_ptrs_first,
                                                    constraint_ptrs_second);

                int    number_of_common_elements = intersection_set.size();
                double overlap_rate =
                    static_cast<double>(number_of_common_elements) /
                    static_cast<double>(union_set.size());

                if (number_of_common_elements >= a_MINIMUM_COMMON_ELEMENT) {
                    pairs.emplace_back(variable_ptrs[i], variable_ptrs[j],
                                       number_of_common_elements, overlap_rate);
                    utility::print_message(
                        "A flippable variable pair (" +
                            variable_ptrs[i]->name() + ", " +
                            variable_ptrs[j]->name() + ")" + " was found " +
                            "(" + std::to_string(number_of_common_elements) +
                            "/" + std::to_string(overlap_rate) + ").",
                        a_IS_ENABLED_PRINT);
                }
            }
        }
        std::sort(pairs.begin(), pairs.end(),
                  [](const auto &a_FIRST, const auto &a_SECOND) {
                      return a_FIRST.overlap_rate > a_SECOND.overlap_rate;
                  });

        utility::print_message(std::to_string(pairs.size()) +
                                   " flippable variable pairs were found.",
                               a_IS_ENABLED_PRINT);
        utility::print_message("Done.", a_IS_ENABLED_PRINT);

        m_pairs = pairs;
    }

    /*************************************************************************/
    inline void write_pairs(const std::string &a_FILE_NAME) const {
        std::ofstream ofs(a_FILE_NAME);
        for (const auto &pair : m_pairs) {
            ofs << pair.variable_ptr_first->name() << " "
                << pair.variable_ptr_second->name() << " "
                << pair.variable_ptr_first->related_constraint_ptrs().size()
                << " "
                << pair.variable_ptr_second->related_constraint_ptrs().size()
                << " " << pair.number_of_common_element << " "
                << pair.overlap_rate << " " << std::endl;
        }
        ofs.close();
    }

    /*************************************************************************/
    inline const std::vector<FlippableVariablePair<T_Variable, T_Expression>>
        &pairs(void) const {
        return this->m_pairs;
    }
};
using IPFlippableVariablePairExtractor =
    FlippableVariablePairExtractor<int, double>;
}  // namespace printemps::preprocess
#endif
/*****************************************************************************/
// END
/*****************************************************************************/