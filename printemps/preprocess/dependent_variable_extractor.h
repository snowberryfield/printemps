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

    utility::BinaryMatrix m_constraint_adjacency;
    std::vector<bool>     m_extractable_flags;

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
        m_constraint_adjacency.initialize();
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
    inline std::vector<model_component::Constraint<T_Variable, T_Expression> *>
    collect_candidate_constraint_ptrs(void) const {
        auto &reference = m_model_ptr->reference().constraint_type;

        std::vector<std::vector<
            model_component::Constraint<T_Variable, T_Expression> *>>
            constraint_ptrs_list = {reference.exclusive_or_ptrs,
                                    reference.exclusive_nor_ptrs,
                                    reference.inverted_integers_ptrs,
                                    reference.balanced_integers_ptrs,
                                    reference.constant_sum_integers_ptrs,
                                    reference.constant_difference_integers_ptrs,
                                    reference.constant_ratio_integers_ptrs,
                                    reference.trinomial_exclusive_nor_ptrs,
                                    reference.all_or_nothing_ptrs,
                                    reference.intermediate_ptrs};

        std::vector<model_component::Constraint<T_Variable, T_Expression> *>
            candidate_constraint_ptrs;

        for (auto &&constraint_ptrs : constraint_ptrs_list) {
            for (auto &&constraint_ptr : constraint_ptrs) {
                if (!constraint_ptr->is_enabled()) {
                    continue;
                }
                candidate_constraint_ptrs.push_back(constraint_ptr);
            }
        }

        return candidate_constraint_ptrs;
    }

    /*************************************************************************/
    inline std::unordered_map<
        model_component::Variable<T_Variable, T_Expression> *, int>
    count_candidate_dependent_variable_ptrs(
        const std::vector<model_component::Constraint<T_Variable, T_Expression>
                              *> &a_CONSTRAINT_PTRS) const {
        std::unordered_map<
            model_component::Variable<T_Variable, T_Expression> *, int>
            counts;

        for (const auto &constraint_ptr : a_CONSTRAINT_PTRS) {
            auto key_variable_ptr = constraint_ptr->key_variable_ptr();
            if (constraint_ptr->has_representative_variable()) {
                const auto &sensitivities =
                    constraint_ptr->expression().sensitivities();
                for (const auto &sensitivity : sensitivities) {
                    auto variable_ptr = sensitivity.first;
                    if (variable_ptr == key_variable_ptr) {
                        continue;
                    }
                    counts[variable_ptr]++;
                }
            } else {
                counts[key_variable_ptr]++;
            }
        }
        return counts;
    }

    /*************************************************************************/
    inline utility::BinaryMatrix create_constraint_adjacency(
        const std::vector<model_component::Constraint<T_Variable, T_Expression>
                              *> &a_CONSTRAINT_PTRS) const {
        const int SIZE = a_CONSTRAINT_PTRS.size();
        auto      adj  = utility::BinaryMatrix::identity(SIZE);

        std::unordered_map<
            model_component::Variable<T_Variable, T_Expression> *,
            std::vector<int>>
            variable_to_constraint_indices;

        for (int j = 0; j < SIZE; ++j) {
            const auto &sensitivities =
                a_CONSTRAINT_PTRS[j]->expression().sensitivities();
            for (const auto &sensitivity : sensitivities) {
                variable_to_constraint_indices[sensitivity.first].push_back(j);
            }
        }

        for (int i = 0; i < SIZE; i++) {
            auto constraint_ptr   = a_CONSTRAINT_PTRS[i];
            auto key_variable_ptr = constraint_ptr->key_variable_ptr();

            if (constraint_ptr->has_representative_variable()) {
                const auto &sensitivities =
                    constraint_ptr->expression().sensitivities();
                for (const auto &sensitivity : sensitivities) {
                    auto variable_ptr = sensitivity.first;
                    if (variable_ptr == key_variable_ptr ||
                        variable_ptr->is_fixed()) {
                        continue;
                    }
                    auto it = variable_to_constraint_indices.find(variable_ptr);
                    if (it == variable_to_constraint_indices.end()) {
                        continue;
                    }
                    for (int j : it->second) {
                        if (i != j) {
                            adj[i][j] = 1;
                        }
                    }
                }
            } else {
                for (int j = 0; j < SIZE; j++) {
                    if (i == j) {
                        continue;
                    }
                    const auto &sensitivities =
                        a_CONSTRAINT_PTRS[j]->expression().sensitivities();
                    if (sensitivities.find(key_variable_ptr) !=
                        sensitivities.end()) {
                        adj[i][j] = 1;
                    }
                }
            }
        }

        return adj;
    }

    /*************************************************************************/
    inline std::vector<bool> create_extractable_flags(
        const utility::BinaryMatrix &a_ADJ) const {
        const int         SIZE = a_ADJ.number_of_rows();
        std::vector<bool> flags(SIZE, true);

        const auto REACHABILITY = a_ADJ.reachability();
        for (int i = 0; i < SIZE; i++) {
            for (int j = i + 1; j < SIZE; j++) {
                if (REACHABILITY[i][j] > 0 && REACHABILITY[j][i] > 0) {
                    flags[i] = false;
                    flags[j] = false;
                }
            }
        }
        return flags;
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
    inline int extract(const option::Option &a_OPTION,
                       const bool            a_IS_ENABLED_PRINT) {
        utility::print_single_line(a_IS_ENABLED_PRINT);
        utility::print_message("Extracting dependent variables...",
                               a_IS_ENABLED_PRINT);

        this->clear_work_buffers();

        m_candidate_constraint_ptrs = this->collect_candidate_constraint_ptrs();

        if (m_candidate_constraint_ptrs.empty()) {
            utility::print_message(
                "No candidate constraints for extracting dependent "
                "variables were found.",
                a_IS_ENABLED_PRINT);
            return 0;
        }

        m_candidate_dependent_variable_ptr_counts =
            this->count_candidate_dependent_variable_ptrs(
                m_candidate_constraint_ptrs);

        m_constraint_adjacency =
            this->create_constraint_adjacency(m_candidate_constraint_ptrs);

        m_extractable_flags =
            this->create_extractable_flags(m_constraint_adjacency);

        auto enable_map = this->create_enable_map(a_OPTION);

        int number_of_newly_extracted_dependent_variables =
            this->extract_dependent_variables(enable_map, a_IS_ENABLED_PRINT);

        this->reflect_to_model(a_IS_ENABLED_PRINT);

        utility::print_message("Done.", a_IS_ENABLED_PRINT);
        return number_of_newly_extracted_dependent_variables;
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
    inline const utility::BinaryMatrix &constraint_adjacency(
        void) const noexcept {
        return m_constraint_adjacency;
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
