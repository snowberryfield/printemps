/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_PREPROCESS_PARTIAL_FEASIBLE_ENUMERATOR_H__
#define PRINTEMPS_PREPROCESS_PARTIAL_FEASIBLE_ENUMERATOR_H__

namespace printemps::preprocess {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class PartialFeasibleEnumerator {
   private:
    model::Model<T_Variable, T_Expression>* m_model_ptr;
    std::vector<model_component::ConstraintGroup<T_Variable, T_Expression>>
        m_small_constraint_groups;

   private:
    /*************************************************************************/
    inline T_Expression evaluate_violation(
        const model_component::Constraint<T_Variable, T_Expression>*
            constraint_ptr,
        const std::vector<std::pair<
            model_component::Variable<T_Variable, T_Expression>*, int>>&
            a_SOLUTION) const {
        const auto& EXPRESSION    = constraint_ptr->expression();
        const auto& SENSITIVITIES = EXPRESSION.sensitivities();

        T_Expression value = EXPRESSION.constant_value();

        for (const auto& pair : a_SOLUTION) {
            value += SENSITIVITIES.at(pair.first) * pair.second;
        }

        for (const auto& sensitivity : SENSITIVITIES) {
            if (sensitivity.first->is_fixed()) {
                value += sensitivity.first->value() * sensitivity.second;
            }
        }

        T_Expression violation = static_cast<T_Expression>(0);

        switch (constraint_ptr->sense()) {
            case model_component::ConstraintSense::Less:
                violation = std::max(value, static_cast<T_Expression>(0));
                break;
            case model_component::ConstraintSense::Equal:
                violation = std::abs(value);
                break;
            case model_component::ConstraintSense::Greater:
                violation = std::max(-value, static_cast<T_Expression>(0));
                break;
        }

        return violation;
    }

    /*************************************************************************/
    inline void build_small_constraint_groups(void) {
        std::map<
            std::vector<model_component::Variable<T_Variable, T_Expression>*>,
            std::vector<model_component::Constraint<T_Variable, T_Expression>*>>
            grouped_constraints_map;

        for (auto* constraint_ptr :
             m_model_ptr->reference().constraint.enabled_constraint_ptrs) {
            if (!constraint_ptr->structure()
                     .has_only_binary_or_selection_variable) {
                continue;
            }

            const auto& SENSITIVITIES =
                constraint_ptr->expression().sensitivities();

            std::vector<model_component::Variable<T_Variable, T_Expression>*>
                variable_ptrs;

            for (const auto& sensitivity : SENSITIVITIES) {
                if (!sensitivity.first->is_fixed()) {
                    variable_ptrs.push_back(sensitivity.first);
                }
            }

            const int NUMBER_OF_MUTABLE_VARIABLES =
                static_cast<int>(variable_ptrs.size());

            if (NUMBER_OF_MUTABLE_VARIABLES <= 2 ||
                NUMBER_OF_MUTABLE_VARIABLES > 8) {
                continue;
            }

            std::stable_sort(variable_ptrs.begin(), variable_ptrs.end(),
                             [](const auto& a_FIRST, const auto& a_SECOND) {
                                 return a_FIRST->name() < a_SECOND->name();
                             });
            grouped_constraints_map[variable_ptrs].push_back(constraint_ptr);
        }

        m_small_constraint_groups.clear();
        m_small_constraint_groups.reserve(grouped_constraints_map.size());

        for (const auto& grouped_constraints_pair : grouped_constraints_map) {
            if (grouped_constraints_pair.second.size() < 2) {
                continue;
            }
            model_component::ConstraintGroup<T_Variable, T_Expression>
                constraint_group;

            constraint_group.variable_ptrs   = grouped_constraints_pair.first;
            constraint_group.constraint_ptrs = grouped_constraints_pair.second;
            constraint_group.solutions.clear();

            m_small_constraint_groups.push_back(constraint_group);
        }
    }

    /*************************************************************************/
    inline void enumerate_feasible_solutions(void) {
        for (auto& constraint_group : m_small_constraint_groups) {
            const auto& VARIABLE_PTRS   = constraint_group.variable_ptrs;
            const auto& CONSTRAINT_PTRS = constraint_group.constraint_ptrs;

            const int number_of_mutable_variables =
                static_cast<int>(VARIABLE_PTRS.size());

            const int total_patterns = 1 << number_of_mutable_variables;
            constraint_group.solutions.clear();

            for (int mask = 0; mask < total_patterns; mask++) {
                std::vector<std::pair<
                    model_component::Variable<T_Variable, T_Expression>*, int>>
                    solution;
                solution.reserve(number_of_mutable_variables);
                for (auto i = 0; i < number_of_mutable_variables; i++) {
                    solution.push_back({VARIABLE_PTRS[i], (mask >> i) & 1});
                }

                bool is_feasible = true;
                for (auto* constraint_ptr : CONSTRAINT_PTRS) {
                    const auto VIOLATION =
                        this->evaluate_violation(constraint_ptr, solution);

                    if (!(VIOLATION < constant::EPSILON)) {
                        is_feasible = false;
                        break;
                    }
                }

                if (!is_feasible) {
                    continue;
                }

                constraint_group.solutions.push_back(solution);
            }
        }
    }

    /*************************************************************************/
    inline void filter_target_small_constraint_groups(void) {
        std::vector<model_component::ConstraintGroup<T_Variable, T_Expression>>
            new_small_constraint_groups;
        new_small_constraint_groups.reserve(m_small_constraint_groups.size());

        for (auto& constraint_group : m_small_constraint_groups) {
            if (constraint_group.solutions.size() <
                constraint_group.variable_ptrs.size()) {
                new_small_constraint_groups.push_back(constraint_group);
            }
        }
        m_small_constraint_groups = new_small_constraint_groups;
    }

    /*************************************************************************/
    inline void print_statistics(void) const {
        const int SMALL_CONSTRAINT_GROUPS_SIZE =
            m_small_constraint_groups.size();
        if (SMALL_CONSTRAINT_GROUPS_SIZE > 0) {
            utility::print_message(
                "Following constraint groups and partial feasible solutions "
                "are found:",
                true);

            for (auto i = 0; i < SMALL_CONSTRAINT_GROUPS_SIZE; i++) {
                const auto& constraint_group = m_small_constraint_groups[i];
                utility::print_info(
                    " -- Group " + utility::to_string(i, "%d") + " : #Vars = " +
                        utility::to_string(
                            constraint_group.variable_ptrs.size(), "%d") +
                        ", #Cons = " +
                        utility::to_string(
                            constraint_group.constraint_ptrs.size(), "%d") +
                        ", #Partial feas. sol = " +
                        utility::to_string(constraint_group.solutions.size(),
                                           "%d"),
                    true);
            }
        } else {
            utility::print_message("No constraint groups were are found.",
                                   true);
        }
    }

   public:
    /*************************************************************************/
    PartialFeasibleEnumerator(void) {
        this->initialize();
    }

    /*************************************************************************/
    PartialFeasibleEnumerator(
        model::Model<T_Variable, T_Expression>* a_model_ptr) {
        this->setup(a_model_ptr);
    }

    /*************************************************************************/
    inline void setup(model::Model<T_Variable, T_Expression>* a_model_ptr) {
        this->initialize();
        m_model_ptr = a_model_ptr;
    }

    /*************************************************************************/
    inline void initialize(void) {
        m_model_ptr = nullptr;
        m_small_constraint_groups.clear();
    }

    /*************************************************************************/
    inline void enumerate(const bool a_IS_ENABLED_PRINT) {
        utility::print_single_line(a_IS_ENABLED_PRINT);
        utility::print_message(
            "Enumerating partial feasible solutions for small constraint "
            "groups...",
            a_IS_ENABLED_PRINT);

        this->build_small_constraint_groups();
        this->enumerate_feasible_solutions();
        this->filter_target_small_constraint_groups();
        if (a_IS_ENABLED_PRINT) {
            this->print_statistics();
        }

        utility::print_message("Done.", a_IS_ENABLED_PRINT);
    }

    /*************************************************************************/
    inline const std::vector<
        model_component::ConstraintGroup<T_Variable, T_Expression>>&
    small_constraint_groups(void) const {
        return m_small_constraint_groups;
    }
};
}  // namespace printemps::preprocess
#endif
/*****************************************************************************/
// END
/*****************************************************************************/
