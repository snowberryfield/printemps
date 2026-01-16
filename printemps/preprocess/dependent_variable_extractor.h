/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_PREPROCESS_DEPENDENT_VARIABLE_EXTRACTOR_H__
#define PRINTEMPS_PREPROCESS_DEPENDENT_VARIABLE_EXTRACTOR_H__

namespace printemps::preprocess {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class DependentVariableExtractor {
   private:
    model::Model<T_Variable, T_Expression> *m_model_ptr;

    /* Work buffers (valid only during extract()) */
    std::vector<model_component::Constraint<T_Variable, T_Expression> *>
        m_candidate_constraint_ptrs;

    std::unordered_map<model_component::Variable<T_Variable, T_Expression> *,
                       int>
        m_candidate_dependent_variable_ptr_counts;

    std::vector<std::vector<int>> m_constraint_graph;
    std::vector<bool>             m_extractable_flags;

    std::vector<model_component::Variable<T_Variable, T_Expression> *>
        m_dependent_variable_ptrs;

    std::vector<model_component::Constraint<T_Variable, T_Expression> *>
        m_target_constraint_ptrs;

    std::vector<model_component::Expression<T_Variable, T_Expression>>
        m_additional_expressions;

    std::vector<model_component::Constraint<T_Variable, T_Expression>>
        m_additional_constraints;

   public:
    /*************************************************************************/
    DependentVariableExtractor(void) {
        this->initialize();
    }

    /*************************************************************************/
    DependentVariableExtractor(
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
        this->clear_work_buffers();
    }

    /*************************************************************************/
    inline void clear_work_buffers(void) {
        m_candidate_constraint_ptrs.clear();
        m_candidate_dependent_variable_ptr_counts.clear();
        m_constraint_graph.clear();
        m_extractable_flags.clear();
        m_dependent_variable_ptrs.clear();
        m_target_constraint_ptrs.clear();
        m_additional_expressions.clear();
        m_additional_constraints.clear();
    }

    /*************************************************************************/
    inline static std::unordered_map<model_component::ConstraintType, bool>
    create_enable_map(const option::Option &a_OPTION) {
        std::unordered_map<model_component::ConstraintType, bool> enable_map;
        using namespace model_component;
        auto &option = a_OPTION.preprocess;

        enable_map[ConstraintType::ExclusiveOR] =
            option.is_enabled_extract_dependent_exclusive_or;
        enable_map[ConstraintType::ExclusiveNOR] =
            option.is_enabled_extract_dependent_exclusive_nor;
        enable_map[ConstraintType::InvertedIntegers] =
            option.is_enabled_extract_dependent_inverted_integers;
        enable_map[ConstraintType::BalancedIntegers] =
            option.is_enabled_extract_dependent_balanced_integers;
        enable_map[ConstraintType::ConstantSumIntegers] =
            option.is_enabled_extract_dependent_constant_sum_integers;
        enable_map[ConstraintType::ConstantDifferenceIntegers] =
            option.is_enabled_extract_dependent_constant_difference_integers;
        enable_map[ConstraintType::ConstantRatioIntegers] =
            option.is_enabled_extract_dependent_constant_ratio_integers;
        enable_map[ConstraintType::TrinomialExclusiveNOR] =
            option.is_enabled_extract_dependent_trinomial_exclusive_nor;
        enable_map[ConstraintType::AllOrNothing] =
            option.is_enabled_extract_dependent_all_or_nothing;
        enable_map[ConstraintType::Intermediate] =
            option.is_enabled_extract_dependent_intermediate;

        return enable_map;
    }

    /*************************************************************************/
    inline void collect_candidate_constraint_ptrs(void) {
        auto &reference = m_model_ptr->reference().constraint_type;

        std::vector<std::vector<
            model_component::Constraint<T_Variable, T_Expression> *> *>
            constraint_ptrs_ptrs = {
                &reference.exclusive_or_ptrs,
                &reference.exclusive_nor_ptrs,
                &reference.inverted_integers_ptrs,
                &reference.balanced_integers_ptrs,
                &reference.constant_sum_integers_ptrs,
                &reference.constant_difference_integers_ptrs,
                &reference.constant_ratio_integers_ptrs,
                &reference.trinomial_exclusive_nor_ptrs,
                &reference.all_or_nothing_ptrs,
                &reference.intermediate_ptrs};

        std::vector<model_component::Constraint<T_Variable, T_Expression> *>
            candidate_constraint_ptrs;

        int expected_size = 0;
        for (auto &&constraint_ptrs_ptr : constraint_ptrs_ptrs) {
            expected_size += constraint_ptrs_ptr->size();
        }
        candidate_constraint_ptrs.reserve(expected_size);

        for (auto &&constraint_ptrs_ptr : constraint_ptrs_ptrs) {
            for (auto &&constraint_ptr : *constraint_ptrs_ptr) {
                if (constraint_ptr->is_enabled())
                    candidate_constraint_ptrs.push_back(constraint_ptr);
            }
        }
        m_candidate_constraint_ptrs = candidate_constraint_ptrs;
    }

    /*************************************************************************/
    inline void count_candidate_dependent_variable_ptrs(void) {
        std::unordered_map<
            model_component::Variable<T_Variable, T_Expression> *, int>
            counts;

        for (auto *constraint_ptr : m_candidate_constraint_ptrs) {
            auto key = constraint_ptr->key_variable_ptr();
            if (constraint_ptr->has_representative_variable()) {
                for (auto &&sensitivity :
                     constraint_ptr->expression().sensitivities()) {
                    if (sensitivity.first != key) {
                        counts[sensitivity.first]++;
                    }
                }
            } else {
                counts[key]++;
            }
        }
        m_candidate_dependent_variable_ptr_counts = counts;
    }

    /*************************************************************************/
    inline void build_constraint_graph(void) {
        const int SIZE = m_candidate_constraint_ptrs.size();
        m_constraint_graph.assign(SIZE, {});

        std::unordered_map<
            model_component::Variable<T_Variable, T_Expression> *,
            std::vector<int>>
            variable_to_constraints;

        for (int i = 0; i < SIZE; i++) {
            auto &sensitivities =
                m_candidate_constraint_ptrs[i]->expression().sensitivities();
            for (auto &&sensitivity : sensitivities) {
                variable_to_constraints[sensitivity.first].push_back(i);
            }
        }

        for (int i = 0; i < SIZE; i++) {
            auto constraint_ptr   = m_candidate_constraint_ptrs[i];
            auto key_variable_ptr = constraint_ptr->key_variable_ptr();

            if (constraint_ptr->has_representative_variable()) {
                auto &sensitivities =
                    constraint_ptr->expression().sensitivities();
                for (auto &&sensitivity : sensitivities) {
                    auto variable_ptr = sensitivity.first;
                    if (variable_ptr == key_variable_ptr ||
                        variable_ptr->is_fixed()) {
                        continue;
                    }

                    auto it = variable_to_constraints.find(variable_ptr);
                    if (it == variable_to_constraints.end()) {
                        continue;
                    }

                    for (int j : it->second) {
                        if (i != j)
                            m_constraint_graph[i].push_back(j);
                    }
                }
            } else {
                auto it = variable_to_constraints.find(key_variable_ptr);
                if (it != variable_to_constraints.end()) {
                    for (int j : it->second) {
                        if (i != j)
                            m_constraint_graph[i].push_back(j);
                    }
                }
            }
        }
    }

    /*************************************************************************/
    inline void build_extractable_flags(void) {
        const int         SIZE = m_constraint_graph.size();
        std::vector<int>  dfs_order(SIZE, -1);
        std::vector<int>  lowlink(SIZE, 0);
        std::vector<bool> is_on_stack(SIZE, false);
        std::vector<int>  scc_id(SIZE, -1);
        int               scc_count = 0;

        std::vector<int> dfs_stack;
        dfs_stack.reserve(SIZE);

        int current_order = 0;

        std::function<void(int)> dfs = [&](int node) {
            dfs_order[node] = lowlink[node] = current_order++;
            dfs_stack.push_back(node);
            is_on_stack[node] = true;

            for (int next : m_constraint_graph[node]) {
                if (dfs_order[next] < 0) {
                    dfs(next);
                    lowlink[node] = std::min(lowlink[node], lowlink[next]);
                } else if (is_on_stack[next]) {
                    lowlink[node] = std::min(lowlink[node], dfs_order[next]);
                }
            }

            if (lowlink[node] == dfs_order[node]) {
                while (true) {
                    int v = dfs_stack.back();
                    dfs_stack.pop_back();
                    is_on_stack[v] = false;
                    scc_id[v]      = scc_count;
                    if (v == node) {
                        break;
                    }
                }
                scc_count++;
            }
        };

        for (int i = 0; i < SIZE; ++i) {
            if (dfs_order[i] < 0) {
                dfs(i);
            }
        }

        std::vector<int> scc_size(scc_count, 0);
        for (int i = 0; i < SIZE; i++) {
            scc_size[scc_id[i]]++;
        }

        m_extractable_flags.assign(SIZE, true);
        for (int i = 0; i < SIZE; ++i) {
            if (scc_size[scc_id[i]] > 1) {
                m_extractable_flags[i] = false;
            }
        }
    }
    /*************************************************************************/
    inline int extract(const option::Option &a_OPTION,
                       const bool            a_IS_ENABLED_PRINT) {
        utility::print_single_line(a_IS_ENABLED_PRINT);
        utility::print_message("Extracting dependent variables...",
                               a_IS_ENABLED_PRINT);

        this->clear_work_buffers();
        this->collect_candidate_constraint_ptrs();

        if (m_candidate_constraint_ptrs.empty()) {
            utility::print_message(
                "No candidate constraints for extracting dependent variables "
                "were found.",
                a_IS_ENABLED_PRINT);
            return 0;
        }

        this->count_candidate_dependent_variable_ptrs();
        this->build_constraint_graph();
        this->build_extractable_flags();

        auto enable_map = create_enable_map(a_OPTION);

        int number_of_newly_extracted_dependent_variables =
            this->extract_dependent_variables(enable_map, a_IS_ENABLED_PRINT);

        this->reflect_to_model(a_IS_ENABLED_PRINT);
        utility::print_message("Done.", a_IS_ENABLED_PRINT);
        return number_of_newly_extracted_dependent_variables;
    }

    /*************************************************************************/
    inline int extract_dependent_variables(
        const std::unordered_map<model_component::ConstraintType, bool>
                  &a_ENABLE_MAP,
        const bool a_IS_ENABLED_PRINT) {
        const int CANDIDATE_CONSTRAINTS_SIZE =
            m_candidate_constraint_ptrs.size();

        m_dependent_variable_ptrs.clear();
        m_target_constraint_ptrs.clear();
        m_additional_constraints.clear();
        m_additional_expressions.clear();

        for (int i = 0; i < CANDIDATE_CONSTRAINTS_SIZE; i++) {
            if (!m_extractable_flags[i]) {
                continue;
            }

            auto constraint_ptr   = m_candidate_constraint_ptrs[i];
            auto key_variable_ptr = constraint_ptr->key_variable_ptr();

            if (!a_ENABLE_MAP.at(constraint_ptr->type())) {
                continue;
            }

            if (constraint_ptr->has_representative_variable()) {
                const auto &sensitivities =
                    constraint_ptr->expression().sensitivities();

                bool is_extractable = true;
                for (const auto &sensitivity : sensitivities) {
                    auto variable_ptr = sensitivity.first;
                    if (variable_ptr == key_variable_ptr ||
                        variable_ptr->is_fixed()) {
                        continue;
                    }
                    if (m_candidate_dependent_variable_ptr_counts
                            [variable_ptr] != 1) {
                        is_extractable = false;
                        break;
                    }
                }

                if (!is_extractable) {
                    continue;
                }

                m_target_constraint_ptrs.push_back(constraint_ptr);

                for (const auto &sensitivity : sensitivities) {
                    auto variable_ptr = sensitivity.first;
                    if (variable_ptr == key_variable_ptr ||
                        variable_ptr->is_fixed()) {
                        continue;
                    }

                    utility::print_message(
                        "The variable " + variable_ptr->name() +
                            " in the constraint " + constraint_ptr->name() +
                            " was extracted as a dependent variable. ",
                        a_IS_ENABLED_PRINT);

                    m_dependent_variable_ptrs.push_back(variable_ptr);
                    m_additional_expressions.emplace_back(
                        key_variable_ptr->to_expression());

                    m_additional_expressions.back().set_name(
                        variable_ptr->name() + "_dependent");
                }
            } else {
                if (m_candidate_dependent_variable_ptr_counts
                        [key_variable_ptr] != 1) {
                    continue;
                }

                utility::print_message(
                    "The variable " + key_variable_ptr->name() +
                        " in the constraint " + constraint_ptr->name() +
                        " was extracted as a dependent variable. ",
                    a_IS_ENABLED_PRINT);

                m_target_constraint_ptrs.push_back(constraint_ptr);

                m_dependent_variable_ptrs.push_back(key_variable_ptr);
                m_additional_expressions.emplace_back(
                    constraint_ptr->expression().solve(key_variable_ptr));

                auto &expression = m_additional_expressions.back();
                expression.set_name(key_variable_ptr->name() + "_dependent");

                if (key_variable_ptr->lower_bound() != constant::INT_HALF_MIN &&
                    key_variable_ptr->lower_bound() >
                        expression.lower_bound()) {
                    m_additional_constraints.emplace_back(
                        expression >= key_variable_ptr->lower_bound());
                    m_additional_constraints.back().set_name(
                        constraint_ptr->name() + "_greater");
                }

                if (key_variable_ptr->upper_bound() != constant::INT_HALF_MAX &&
                    key_variable_ptr->upper_bound() <
                        expression.upper_bound()) {
                    m_additional_constraints.emplace_back(
                        expression <= key_variable_ptr->upper_bound());
                    m_additional_constraints.back().set_name(
                        constraint_ptr->name() + "_less");
                }
            }
        }
        return m_dependent_variable_ptrs.size();
    }

    /*************************************************************************/
    inline void reflect_to_model(const bool a_IS_ENABLED_PRINT) {
        for (auto &constraint_ptr : m_target_constraint_ptrs) {
            constraint_ptr->disable();
            constraint_ptr->set_is_defining_dependent_variable(true);
        }

        const int NEWLY_EXTRACTED_DEPENDENT_VARIABLES_SIZE =
            m_dependent_variable_ptrs.size();

        if (NEWLY_EXTRACTED_DEPENDENT_VARIABLES_SIZE == 0) {
            return;
        }

        auto &proxy = m_model_ptr->component_creator().create_expressions(
            "additional", NEWLY_EXTRACTED_DEPENDENT_VARIABLES_SIZE);

        for (int i = 0; i < NEWLY_EXTRACTED_DEPENDENT_VARIABLES_SIZE; i++) {
            proxy(i) = m_additional_expressions[i];
            proxy(i).set_name(m_additional_expressions[i].name());
            m_dependent_variable_ptrs[i]->set_dependent_expression_ptr(
                &proxy(i));
        }

        if (!m_additional_constraints.empty()) {
            const int ADDITIONAL_CONSTRAINTS_SIZE =
                m_additional_constraints.size();

            auto &proxy = m_model_ptr->component_creator().create_constraints(
                "additional", ADDITIONAL_CONSTRAINTS_SIZE);

            for (int i = 0; i < ADDITIONAL_CONSTRAINTS_SIZE; i++) {
                proxy(i) = m_additional_constraints[i];
                proxy(i).set_name(m_additional_constraints[i].name());
                utility::print_message("An extra constraint " +
                                           m_additional_constraints[i].name() +
                                           " was added.",
                                       a_IS_ENABLED_PRINT);
            }
        }
    }

    /*************************************************************************/
    inline const std::vector<
        model_component::Constraint<T_Variable, T_Expression> *> &
    candidate_constraint_ptrs(void) const noexcept {
        return m_candidate_constraint_ptrs;
    }

    /*************************************************************************/
    inline const std::unordered_map<
        model_component::Variable<T_Variable, T_Expression> *, int> &
    candidate_dependent_variable_ptr_counts(void) const noexcept {
        return m_candidate_dependent_variable_ptr_counts;
    }

    /*************************************************************************/
    inline const std::vector<bool> &extractable_flags(void) const noexcept {
        return m_extractable_flags;
    }

    /*************************************************************************/
    inline const std::vector<
        model_component::Variable<T_Variable, T_Expression> *> &
    dependent_variable_ptrs(void) const noexcept {
        return m_dependent_variable_ptrs;
    }

    /*************************************************************************/
    inline const std::vector<
        model_component::Expression<T_Variable, T_Expression>> &
    additional_expressions(void) const noexcept {
        return m_additional_expressions;
    }

    /*************************************************************************/
    inline const std::vector<
        model_component::Constraint<T_Variable, T_Expression>> &
    additional_constraints(void) const noexcept {
        return m_additional_constraints;
    }
};
}  // namespace printemps::preprocess
#endif
/*****************************************************************************/
// END
/*****************************************************************************/
