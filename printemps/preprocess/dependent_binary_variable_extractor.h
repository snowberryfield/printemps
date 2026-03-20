/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_PREPROCESS_DEPENDENT_BINARY_VARIABLE_EXTRACTOR_H__
#define PRINTEMPS_PREPROCESS_DEPENDENT_BINARY_VARIABLE_EXTRACTOR_H__

namespace printemps::preprocess {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class DependentBinaryVariableExtractor {
   private:
    model::Model<T_Variable, T_Expression> *m_model_ptr;

    std::vector<model_component::Constraint<T_Variable, T_Expression> *>
        m_candidate_constraint_ptrs;

    std::vector<model_component::Variable<T_Variable, T_Expression> *>
        m_target_variable_ptrs;

    std::vector<model_component::Constraint<T_Variable, T_Expression> *>
        m_target_constraint_ptrs;

    std::vector<model_component::Variable<T_Variable, T_Expression> *>
        m_dependent_variable_ptrs;

    std::vector<model_component::Expression<T_Variable, T_Expression>>
        m_additional_expressions;

    utility::UnionFind<model_component::Variable<T_Variable, T_Expression> *>
        m_uf;

    std::vector<utility::UnionFindRootGroup<
        model_component::Variable<T_Variable, T_Expression> *>>
        m_groups;

   public:
    /*************************************************************************/
    DependentBinaryVariableExtractor(void) {
        this->initialize();
    }

    /*************************************************************************/
    DependentBinaryVariableExtractor(
        model::Model<T_Variable, T_Expression> *a_model_ptr) {
        this->setup(a_model_ptr);
    }

    /*************************************************************************/
    inline void setup(model::Model<T_Variable, T_Expression> *a_model_ptr) {
        this->initialize();
        m_model_ptr = a_model_ptr;
    }

    /*************************************************************************/
    inline void initialize(void) {
        m_model_ptr = nullptr;
        this->clear_work_buffers();
    }

    /*************************************************************************/
    inline void clear_work_buffers(void) {
        m_target_variable_ptrs.clear();
        m_target_constraint_ptrs.clear();
        m_dependent_variable_ptrs.clear();
        m_additional_expressions.clear();

        m_uf.initialize();
        m_groups.clear();
    }

    /*************************************************************************/
    inline void build_binary_equivalent_groups(const option::Option &a_OPTION) {
        auto &reference = m_model_ptr->reference().constraint_type;

        std::vector<std::vector<
            model_component::Constraint<T_Variable, T_Expression> *> *>
            constraint_ptrs_ptrs;

        if (a_OPTION.preprocess.is_enabled_extract_dependent_exclusive_or) {
            constraint_ptrs_ptrs.push_back(&reference.exclusive_or_ptrs);
        }
        if (a_OPTION.preprocess.is_enabled_extract_dependent_exclusive_nor) {
            constraint_ptrs_ptrs.push_back(&reference.exclusive_nor_ptrs);
        }
        if (a_OPTION.preprocess
                .is_enabled_extract_dependent_trinomial_exclusive_nor) {
            constraint_ptrs_ptrs.push_back(
                &reference.trinomial_exclusive_nor_ptrs);
        }
        if (a_OPTION.preprocess.is_enabled_extract_dependent_all_or_nothing) {
            constraint_ptrs_ptrs.push_back(&reference.all_or_nothing_ptrs);
        }

        std::unordered_set<
            model_component::Variable<T_Variable, T_Expression> *>
            variable_ptrs;
        for (const auto &constraint_ptrs : constraint_ptrs_ptrs) {
            for (const auto &constraint_ptr : *constraint_ptrs) {
                if (!constraint_ptr->is_enabled() ||
                    constraint_ptr->is_defining_dependent_variable()) {
                    continue;
                }
                const auto NOMIAL = constraint_ptr->to_multinomial_constraint();
                for (const auto &variable_ptr : NOMIAL.variable_ptrs) {
                    variable_ptrs.insert(variable_ptr);
                }
                m_target_constraint_ptrs.push_back(constraint_ptr);
            }
        }

        std::vector<model_component::Variable<T_Variable, T_Expression> *>
            variable_ptr_vector(variable_ptrs.begin(), variable_ptrs.end());
        std::stable_sort(variable_ptr_vector.begin(), variable_ptr_vector.end(),
                         [](const auto &a_FIRST, const auto &a_SECOND) {
                             return a_FIRST->name() < a_SECOND->name();
                         });

        utility::UnionFind<
            model_component::Variable<T_Variable, T_Expression> *>
            uf(variable_ptr_vector);

        bool is_consistent = true;
        if (a_OPTION.preprocess.is_enabled_extract_dependent_exclusive_or) {
            for (const auto &constraint_ptr : reference.exclusive_or_ptrs) {
                if (!constraint_ptr->is_enabled() ||
                    constraint_ptr->is_defining_dependent_variable()) {
                    continue;
                }
                const auto NOMIAL = constraint_ptr->to_binomial_constraint();
                is_consistent &= uf.unite(NOMIAL.variable_ptrs[0],
                                          NOMIAL.variable_ptrs[1], 1);
            }
        }

        if (a_OPTION.preprocess.is_enabled_extract_dependent_exclusive_nor) {
            for (const auto &constraint_ptr : reference.exclusive_nor_ptrs) {
                if (!constraint_ptr->is_enabled() ||
                    constraint_ptr->is_defining_dependent_variable()) {
                    continue;
                }
                const auto NOMIAL = constraint_ptr->to_binomial_constraint();
                is_consistent &= uf.unite(NOMIAL.variable_ptrs[0],
                                          NOMIAL.variable_ptrs[1], 0);
            }
        }

        if (a_OPTION.preprocess
                .is_enabled_extract_dependent_trinomial_exclusive_nor) {
            for (const auto &constraint_ptr :
                 reference.trinomial_exclusive_nor_ptrs) {
                if (!constraint_ptr->is_enabled() ||
                    constraint_ptr->is_defining_dependent_variable()) {
                    continue;
                }
                const auto NOMIAL = constraint_ptr->to_trinomial_constraint();
                is_consistent &= uf.unite(NOMIAL.variable_ptrs[0],
                                          NOMIAL.variable_ptrs[1], 0);
                is_consistent &= uf.unite(NOMIAL.variable_ptrs[0],
                                          NOMIAL.variable_ptrs[2], 0);
            }
        }

        if (a_OPTION.preprocess.is_enabled_extract_dependent_all_or_nothing) {
            for (const auto &constraint_ptr : reference.all_or_nothing_ptrs) {
                if (!constraint_ptr->is_enabled() ||
                    constraint_ptr->is_defining_dependent_variable()) {
                    continue;
                }
                const auto NOMIAL = constraint_ptr->to_multinomial_constraint();
                const int  VARIABLES_SIZE = NOMIAL.variable_ptrs.size();
                for (auto i = 1; i < VARIABLES_SIZE; i++) {
                    is_consistent &= uf.unite(NOMIAL.variable_ptrs[0],
                                              NOMIAL.variable_ptrs[i], 0);
                }
            }
        }

        if (a_OPTION.preprocess
                .is_enabled_extract_dependent_using_partial_feasible_enumeration) {
            auto &constraint_groups = m_model_ptr->partial_feasible_enumerator()
                                          .small_constraint_groups();
            for (const auto &constraint_group : constraint_groups) {
                const auto &equal_variable_pairs =
                    constraint_group.equal_variable_pairs;
                for (const auto &equal_variable_pair : equal_variable_pairs) {
                    is_consistent &= uf.unite(equal_variable_pair.first,
                                              equal_variable_pair.second, 0);
                }
                const auto &not_equal_variable_pairs =
                    constraint_group.not_equal_variable_pairs;
                for (const auto &not_equal_variable_pair :
                     not_equal_variable_pairs) {
                    is_consistent &=
                        uf.unite(not_equal_variable_pair.first,
                                 not_equal_variable_pair.second, 1);
                }
            }
        }

        if (!is_consistent) {
            throw error_handler::InfeasibleError(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "Infeasibility was detected while extracting dependent binary "
                "variables."));
        }
        m_uf     = uf;
        m_groups = uf.groups();
    }

    /*************************************************************************/
    inline int extract_dependent_variables(const bool a_IS_ENABLED_PRINT) {
        for (const auto &group : m_groups) {
            auto      representative_variable_ptr = group.root;
            const int MEMBERS_SIZE                = group.members.size();
            if (MEMBERS_SIZE <= 1) {
                continue;
            }
            for (auto i = 0; i < MEMBERS_SIZE; i++) {
                auto variable_ptr = group.members[i].first;
                if (variable_ptr == representative_variable_ptr) {
                    continue;
                }
                utility::print_message(
                    "The variable " + variable_ptr->name() +
                        " was extracted as a dependent binary variable.",
                    a_IS_ENABLED_PRINT);

                m_dependent_variable_ptrs.push_back(variable_ptr);
                const int PARITY = m_uf.parity_between(
                    variable_ptr, representative_variable_ptr);
                if (PARITY == 0) {
                    m_additional_expressions.emplace_back(
                        representative_variable_ptr->to_expression());
                } else {
                    m_additional_expressions.emplace_back(
                        1 - *representative_variable_ptr);
                }
                m_additional_expressions.back().set_name(variable_ptr->name() +
                                                         "_dependent");
            }
        }
        return m_dependent_variable_ptrs.size();
    }

    /*************************************************************************/
    inline int run(const option::Option &a_OPTION,
                   const bool            a_IS_ENABLED_PRINT) {
        utility::print_single_line(a_IS_ENABLED_PRINT);
        utility::print_message("Extracting dependent binary variables...",
                               a_IS_ENABLED_PRINT);

        this->clear_work_buffers();
        this->build_binary_equivalent_groups(a_OPTION);

        if (m_groups.empty()) {
            utility::print_message("Done.", a_IS_ENABLED_PRINT);
            return 0;
        }

        int number_of_extracted_dependent_variables =
            this->extract_dependent_variables(a_IS_ENABLED_PRINT);

        this->reflect_to_model(a_IS_ENABLED_PRINT);
        utility::print_message("Done.", a_IS_ENABLED_PRINT);
        return number_of_extracted_dependent_variables;
    }

    /*************************************************************************/
    inline void reflect_to_model(
        [[maybe_unused]] const bool a_IS_ENABLED_PRINT) {
        for (auto &constraint_ptr : m_target_constraint_ptrs) {
            constraint_ptr->disable();
            constraint_ptr->set_is_defining_dependent_variable(true);
        }

        const int EXTRACTED_DEPENDENT_VARIABLES_SIZE =
            m_dependent_variable_ptrs.size();

        if (EXTRACTED_DEPENDENT_VARIABLES_SIZE == 0) {
            return;
        }

        auto &proxy = m_model_ptr->component_creator().create_expressions(
            "additional", EXTRACTED_DEPENDENT_VARIABLES_SIZE);

        for (int i = 0; i < EXTRACTED_DEPENDENT_VARIABLES_SIZE; i++) {
            proxy(i) = m_additional_expressions[i];
            proxy(i).set_name(m_additional_expressions[i].name());
            proxy(i).set_flat_index(i);
            m_dependent_variable_ptrs[i]->set_dependent_expression_ptr(
                &proxy(i));
        }
    }
};
}  // namespace printemps::preprocess
#endif
/*****************************************************************************/
// END
/*****************************************************************************/
