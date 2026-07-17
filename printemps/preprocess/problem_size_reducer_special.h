/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_PREPROCESS_PROBLEM_SIZE_REDUCER_SPECIAL_H__
#define PRINTEMPS_PREPROCESS_PROBLEM_SIZE_REDUCER_SPECIAL_H__

namespace printemps::preprocess {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class ProblemSizeReducerSpecial {
   private:
    model::Model<T_Variable, T_Expression> *m_model_ptr;

   public:
    /*************************************************************************/
    ProblemSizeReducerSpecial(void) {
        this->initialize();
    }

    /*************************************************************************/
    ProblemSizeReducerSpecial(
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
    }

    /*************************************************************************/
    inline ProblemSizeReducerResult remove_redundant_set_variables(
        const bool a_IS_ENABLED_PRINT) {
        utility::print_single_line(a_IS_ENABLED_PRINT);
        utility::print_message(
            "Removing redundant variables which compose set "
            "paritioning/covering/packing constraints...",
            a_IS_ENABLED_PRINT);

        std::vector<model_component::Constraint<T_Variable, T_Expression> *>
            constraint_ptrs;

        auto &set_partitioning_ptrs =
            m_model_ptr->reference().constraint_type.set_partitioning_ptrs;
        auto &set_covering_ptrs =
            m_model_ptr->reference().constraint_type.set_covering_ptrs;
        auto &set_packing_ptrs =
            m_model_ptr->reference().constraint_type.set_packing_ptrs;

        constraint_ptrs.reserve(set_covering_ptrs.size() +
                                set_partitioning_ptrs.size() +
                                set_packing_ptrs.size());
        for (auto &&constraint_ptr : set_partitioning_ptrs) {
            if (constraint_ptr->is_enabled()) {
                constraint_ptrs.push_back(constraint_ptr);
            }
        }

        for (auto &&constraint_ptr : set_covering_ptrs) {
            if (constraint_ptr->is_enabled()) {
                constraint_ptrs.push_back(constraint_ptr);
            }
        }

        for (auto &&constraint_ptr : set_packing_ptrs) {
            if (constraint_ptr->is_enabled()) {
                constraint_ptrs.push_back(constraint_ptr);
            }
        }

        int number_of_fixed_variables = 0;

        /**
         * If the problem is unconstrained, the following procedures will be
         * skipped.
         */
        if (m_model_ptr->reference()
                .constraint.enabled_constraint_ptrs.size() == 0) {
            return ProblemSizeReducerResult(0, 0, 0);
        }

        /**
         * If the problem is not pure set partitioning/covering/packing problem,
         * the following procedures will be skipped.
         */
        if (m_model_ptr->reference()
                .constraint.enabled_constraint_ptrs.size() !=
            (set_covering_ptrs.size() + set_partitioning_ptrs.size() +
             set_packing_ptrs.size())) {
            return ProblemSizeReducerResult(0, 0, 0);
        }

        auto &variable_ptrs =
            m_model_ptr->reference().variable.mutable_independent_variable_ptrs;
        const int VARIABLES_SIZE = variable_ptrs.size();

        /**
         * Compute a hash for each variable.
         */
        for (auto &&variable_ptr : variable_ptrs) {
            variable_ptr->setup_hash();
        }

        /**
         * Pre-sort the variables pointers for efficient subsequent procedures.
         */
        if (m_model_ptr->is_minimization()) {
            std::stable_sort(variable_ptrs.begin(), variable_ptrs.end(),
                             [](const auto &a_LHS, const auto &a_RHS) {
                                 if (a_LHS->hash() != a_RHS->hash()) {
                                     return a_LHS->hash() < a_RHS->hash();
                                 }

                                 if (a_LHS->objective_sensitivity() !=
                                     a_RHS->objective_sensitivity()) {
                                     return a_LHS->objective_sensitivity() <
                                            a_RHS->objective_sensitivity();
                                 }

                                 if (a_LHS->value() != a_RHS->value()) {
                                     return a_LHS->value() > a_RHS->value();
                                 }
                                 return a_LHS->name() < a_RHS->name();
                             });
        } else {
            std::stable_sort(variable_ptrs.begin(), variable_ptrs.end(),
                             [](const auto &a_LHS, const auto &a_RHS) {
                                 if (a_LHS->hash() != a_RHS->hash()) {
                                     return a_LHS->hash() < a_RHS->hash();
                                 }

                                 if (a_LHS->objective_sensitivity() !=
                                     a_RHS->objective_sensitivity()) {
                                     return a_LHS->objective_sensitivity() >
                                            a_RHS->objective_sensitivity();
                                 }

                                 if (a_LHS->value() != a_RHS->value()) {
                                     return a_LHS->value() > a_RHS->value();
                                 }
                                 return a_LHS->name() < a_RHS->name();
                             });
        }

        int i = 0;

        while (i < VARIABLES_SIZE) {
            /**
             * If the variable has already been fixed, the following procedures
             * will be skipped.
             */
            if (variable_ptrs[i]->is_fixed()) {
                i++;
                continue;
            }

            int j = i + 1;
            while (j < VARIABLES_SIZE) {
                /**
                 * If the hashes of variables i and j are different, the inner
                 * loop can be terminated.
                 */
                if (variable_ptrs[i]->hash() != variable_ptrs[j]->hash()) {
                    i++;
                    break;
                }

                /**
                 * If the variable j is fixed at 0, the following procedure can
                 * be skipped.
                 */
                if (variable_ptrs[j]->is_fixed() &&
                    variable_ptrs[j]->value() == 0) {
                    j++;
                    continue;
                }

                /**
                 * If the constraint coefficient pattern of the variables i and
                 * j is same, fix the value of the variable j at 0.
                 */
                if (variable_ptrs[i]->related_constraint_ptrs() ==
                    variable_ptrs[j]->related_constraint_ptrs()) {
                    variable_ptrs[j]->fix_by(0);
                    utility::print_message(  //
                        "The value of redundant variable " +
                            variable_ptrs[j]->name() + " was fixed at " +
                            std::to_string(0) + ".",
                        a_IS_ENABLED_PRINT);
                    number_of_fixed_variables++;
                }
                j++;
            }
            if (j == VARIABLES_SIZE) {
                break;
            }
        }
        utility::print_message("Done.", a_IS_ENABLED_PRINT);
        return ProblemSizeReducerResult(number_of_fixed_variables, 0, 0);
    }

    /*************************************************************************/
    inline ProblemSizeReducerResult remove_redundant_set_constraints(
        const bool a_IS_ENABLED_PRINT) {
        utility::print_single_line(a_IS_ENABLED_PRINT);
        utility::print_message(
            "Removing redundant exclusive OR and set partitioning constraints "
            "and included variables... ",
            a_IS_ENABLED_PRINT);

        std::vector<model_component::Constraint<T_Variable, T_Expression> *>
            constraint_ptrs;

        auto &exclusive_or_ptrs =
            m_model_ptr->reference().constraint_type.exclusive_or_ptrs;
        auto &set_partitioning_constraint_ptrs =
            m_model_ptr->reference().constraint_type.set_partitioning_ptrs;

        constraint_ptrs.reserve(exclusive_or_ptrs.size() +
                                set_partitioning_constraint_ptrs.size());
        for (auto &&constraint_ptr : exclusive_or_ptrs) {
            if (constraint_ptr->is_enabled()) {
                constraint_ptrs.push_back(constraint_ptr);
            }
        }
        for (auto &&constraint_ptr : set_partitioning_constraint_ptrs) {
            if (constraint_ptr->is_enabled()) {
                constraint_ptrs.push_back(constraint_ptr);
            }
        }

        int number_of_disabled_constraints = 0;
        int number_of_fixed_variables      = 0;

        const int CONSTRAINTS_SIZE = constraint_ptrs.size();

        std::stable_sort(constraint_ptrs.begin(), constraint_ptrs.end(),
                         [](const auto &a_LHS, const auto &a_RHS) {
                             return a_LHS->expression().sensitivities().size() >
                                    a_RHS->expression().sensitivities().size();
                         });

        std::vector<std::unordered_set<
            model_component::Variable<T_Variable, T_Expression> *>>
            variable_ptr_sets(CONSTRAINTS_SIZE);

        for (auto i = 0; i < CONSTRAINTS_SIZE; i++) {
            for (const auto &sensitivity :
                 constraint_ptrs[i]->expression().sensitivities()) {
                variable_ptr_sets[i].insert(sensitivity.first);
            }
        }

        for (auto i = 0; i < CONSTRAINTS_SIZE - 1; i++) {
            for (auto j = i + 1; j < CONSTRAINTS_SIZE; j++) {
                bool is_included = true;
                for (const auto &variable_ptr : variable_ptr_sets[j]) {
                    if (variable_ptr_sets[i].find(variable_ptr) ==
                        variable_ptr_sets[i].end()) {
                        is_included = false;
                        break;
                    }
                }

                if (is_included) {
                    constraint_ptrs[i]->disable();
                    utility::print_message(  //
                        "The redundant constraint " +
                            constraint_ptrs[i]->name() + " was removed.",
                        a_IS_ENABLED_PRINT);

                    for (const auto &variable_ptr : variable_ptr_sets[i]) {
                        if (variable_ptr_sets[j].find(variable_ptr) ==
                            variable_ptr_sets[j].end()) {
                            variable_ptr->fix_by(0);
                            utility::print_message(  //
                                "The value of redundant variable " +
                                    variable_ptr->name() + " in constraint " +
                                    constraint_ptrs[i]->name() +
                                    " was fixed at 0.",
                                a_IS_ENABLED_PRINT);
                            number_of_fixed_variables++;
                        }
                    }

                    number_of_disabled_constraints++;
                    break;
                }
            }
        }

        utility::print_message("Done.", a_IS_ENABLED_PRINT);
        return ProblemSizeReducerResult(number_of_disabled_constraints,
                                        number_of_fixed_variables, 0);
    }

    /*************************************************************************/
    inline ProblemSizeReducerResult extract_implicit_equality_constraints(
        const bool a_IS_ENABLED_PRINT) {
        utility::print_single_line(a_IS_ENABLED_PRINT);
        utility::print_message("Extracting implicit equality constraints...",
                               a_IS_ENABLED_PRINT);

        auto &reference = m_model_ptr->reference().constraint_type;
        int   number_of_disabled_constraints = 0;

        std::vector<model_component::Constraint<T_Variable, T_Expression> *>
                   constraint_ptrs;
        const auto constraint_ptrs_ptrs = {&reference.precedence_ptrs,
                                           &reference.variable_bound_ptrs,
                                           &reference.set_packing_ptrs,
                                           &reference.set_covering_ptrs,
                                           &reference.invariant_knapsack_ptrs,
                                           &reference.multiple_covering_ptrs,
                                           &reference.min_max_ptrs,
                                           &reference.max_min_ptrs,
                                           &reference.bin_packing_ptrs,
                                           &reference.knapsack_ptrs,
                                           &reference.general_linear_ptrs};

        for (const auto &constraint_ptrs_ptr : constraint_ptrs_ptrs) {
            constraint_ptrs.insert(constraint_ptrs.end(),
                                   constraint_ptrs_ptr->begin(),
                                   constraint_ptrs_ptr->end());
        }

        /**
         * NOTE: Do not append equality constraints.
         */
        for (auto &&constraint_ptr : reference.general_linear_ptrs) {
            if (constraint_ptr->sense() !=
                model_component::ConstraintSense::Equal) {
                constraint_ptrs.push_back(constraint_ptr);
            }
        }

        if (constraint_ptrs.size() <= 1) {
            return ProblemSizeReducerResult(0, number_of_disabled_constraints,
                                            0);
        }

        for (auto constraint_ptr : constraint_ptrs) {
            constraint_ptr->expression().setup_hash();
        }

        const int CONSTRAINTS_SIZE = constraint_ptrs.size();

        std::stable_sort(constraint_ptrs.begin(), constraint_ptrs.end(),
                         [](const auto &a_FIRST, const auto &a_SECOND) {
                             return a_FIRST->expression().hash() <
                                    a_SECOND->expression().hash();
                         });

        std::vector<model_component::Constraint<T_Variable, T_Expression>>
            additional_constraints;

        int i = 0;
        while (i < CONSTRAINTS_SIZE) {
            if (!constraint_ptrs[i]->is_enabled()) {
                i++;
                continue;
            }
            if (constraint_ptrs[i]->sense() ==
                model_component::ConstraintSense::Equal) {
                i++;
                continue;
            }
            int j = i + 1;
            while (j < CONSTRAINTS_SIZE) {
                /**
                 * If the hashes of constraint i and j are different, the inner
                 * loop can be terminated.
                 */
                if (constraint_ptrs[i]->expression().hash() !=
                    constraint_ptrs[j]->expression().hash()) {
                    i = j;
                    break;
                }

                /**
                 * If the constraint j is disabled, the following procedure
                 * can be skipped.
                 */
                if (!constraint_ptrs[j]->is_enabled()) {
                    j++;
                    continue;
                }

                /**
                 * If the variable coefficient pattern of the constraints i and
                 * j is same, disable both of constraints i and j instead of
                 * adding a equality constraint that these implicitly imply.
                 */
                if (constraint_ptrs[i]->sense() !=
                        constraint_ptrs[j]->sense() &&
                    constraint_ptrs[i]->expression().equal(
                        constraint_ptrs[j]->expression())) {
                    constraint_ptrs[i]->disable();
                    constraint_ptrs[j]->disable();
                    utility::print_message(  //
                        "The constraints " + constraint_ptrs[i]->name() +
                            " and " + constraint_ptrs[j]->name() +
                            " were removed instead of adding a equality "
                            "constraint that these implicitly imply.",
                        a_IS_ENABLED_PRINT);

                    additional_constraints.emplace_back(
                        constraint_ptrs[i]->expression() == 0);
                    additional_constraints.back().set_name(
                        constraint_ptrs[i]->name() + "_implicit_equality");

                    number_of_disabled_constraints++;
                    i++;
                    break;
                }

                auto expression_sign_inverted =
                    -constraint_ptrs[j]->expression();
                expression_sign_inverted.setup_hash();

                if (constraint_ptrs[i]->sense() ==
                        constraint_ptrs[j]->sense() &&
                    constraint_ptrs[i]->expression().equal(
                        expression_sign_inverted)) {
                    constraint_ptrs[i]->disable();
                    constraint_ptrs[j]->disable();
                    utility::print_message(  //
                        "The constraints " + constraint_ptrs[i]->name() +
                            " and " + constraint_ptrs[j]->name() +
                            " were removed instead of adding a equality "
                            "constraint that these implicitly imply.",
                        a_IS_ENABLED_PRINT);

                    additional_constraints.emplace_back(
                        constraint_ptrs[i]->expression() == 0);
                    additional_constraints.back().set_name(
                        constraint_ptrs[i]->name() + "_implicit_equality");

                    number_of_disabled_constraints++;
                    i++;
                    break;
                }
                j++;
            }
            if (j == CONSTRAINTS_SIZE) {
                break;
            }
        }
        if (number_of_disabled_constraints > 0) {
            auto &additional_constraint_proxy =
                m_model_ptr->component_creator().create_constraints(
                    "additional", number_of_disabled_constraints);
            for (auto i = 0; i < number_of_disabled_constraints; i++) {
                additional_constraint_proxy(i) = additional_constraints[i];
                additional_constraint_proxy(i).set_name(
                    additional_constraints[i].name());
                additional_constraint_proxy(i).set_flat_index(i);
            }
        }

        utility::print_message("Done.", a_IS_ENABLED_PRINT);
        return ProblemSizeReducerResult(0, number_of_disabled_constraints, 0);
    }

    /*************************************************************************/
    inline ProblemSizeReducerResult remove_duplicated_constraints(
        const bool a_IS_ENABLED_PRINT) {
        utility::print_single_line(a_IS_ENABLED_PRINT);
        utility::print_message("Removing duplicated constraints...",
                               a_IS_ENABLED_PRINT);

        auto &reference = m_model_ptr->reference().constraint_type;
        int   number_of_disabled_constraints = 0;

        const auto constraint_ptrs_ptrs = {
            &reference.singleton_ptrs,
            &reference.exclusive_or_ptrs,
            &reference.exclusive_nor_ptrs,
            &reference.inverted_integers_ptrs,
            &reference.balanced_integers_ptrs,
            &reference.constant_sum_integers_ptrs,
            &reference.constant_difference_integers_ptrs,
            &reference.constant_ratio_integers_ptrs,
            &reference.aggregation_ptrs,
            &reference.precedence_ptrs,
            &reference.variable_bound_ptrs,
            &reference.trinomial_exclusive_nor_ptrs,
            &reference.set_partitioning_ptrs,
            &reference.set_packing_ptrs,
            &reference.set_covering_ptrs,
            &reference.cardinality_ptrs,
            &reference.invariant_knapsack_ptrs,
            &reference.multiple_covering_ptrs,
            &reference.binary_flow_ptrs,
            &reference.integer_flow_ptrs,
            &reference.soft_selection_ptrs,
            &reference.min_max_ptrs,
            &reference.max_min_ptrs,
            &reference.intermediate_ptrs,
            &reference.equation_knapsack_ptrs,
            &reference.bin_packing_ptrs,
            &reference.integer_knapsack_ptrs,
            &reference.gf2_ptrs,
            &reference.general_linear_ptrs,
        };

        for (const auto &constraint_ptrs_ptr : constraint_ptrs_ptrs) {
            auto constraint_ptrs = *constraint_ptrs_ptr;
            if (constraint_ptrs.size() <= 1) {
                continue;
            }

            for (auto constraint_ptr : constraint_ptrs) {
                constraint_ptr->expression().setup_hash();
            }

            const int CONSTRAINTS_SIZE = constraint_ptrs.size();

            std::stable_sort(constraint_ptrs.begin(), constraint_ptrs.end(),
                             [](const auto &a_FIRST, const auto &a_SECOND) {
                                 return a_FIRST->expression().hash() <
                                        a_SECOND->expression().hash();
                             });

            int i = 0;
            while (i < CONSTRAINTS_SIZE) {
                if (!constraint_ptrs[i]->is_enabled()) {
                    i++;
                    continue;
                }
                int j = i + 1;
                while (j < CONSTRAINTS_SIZE) {
                    /**
                     * If the hashes of constraint i and j are different, the
                     * inner loop can be terminated.
                     */
                    if (constraint_ptrs[i]->expression().hash() !=
                        constraint_ptrs[j]->expression().hash()) {
                        i = j;
                        break;
                    }

                    /**
                     * If the constraint j is disabled, the following procedure
                     * can be skipped.
                     */
                    if (!constraint_ptrs[j]->is_enabled()) {
                        j++;
                        continue;
                    }

                    /**
                     * If the senses of constraint i and j are different, the
                     * following procedure can be skipped.
                     */
                    if (constraint_ptrs[i]->sense() !=
                        constraint_ptrs[j]->sense()) {
                        j++;
                        continue;
                    }

                    /**
                     * If the variable coefficient pattern of the constraints i
                     * and j is same, disable constraint j.
                     */
                    if (constraint_ptrs[i]->expression().equal(
                            constraint_ptrs[j]->expression())) {
                        constraint_ptrs[j]->disable();
                        utility::print_message(  //
                            "The duplicated constraint " +
                                constraint_ptrs[j]->name() + " was removed.",
                            a_IS_ENABLED_PRINT);
                        number_of_disabled_constraints++;
                    }
                    j++;
                }
                if (j == CONSTRAINTS_SIZE) {
                    break;
                }
            }
        }

        utility::print_message("Done.", a_IS_ENABLED_PRINT);
        return ProblemSizeReducerResult(0, number_of_disabled_constraints, 0);
    }

    /*************************************************************************/
    inline ProblemSizeReducerResult
    remove_implicit_fixed_variables_from_small_constraint_groups(
        const std::vector<
            model_component::ConstraintGroup<T_Variable, T_Expression>>
                  &a_SMALL_CONSTRAINT_GROUPS,
        const bool a_IS_ENABLED_PRINT) {
        utility::print_single_line(a_IS_ENABLED_PRINT);
        utility::print_message(
            "Removing implicit fixed variables from small constraint groups...",
            a_IS_ENABLED_PRINT);

        int number_of_fixed_variables = 0;

        for (auto &constraint_group : a_SMALL_CONSTRAINT_GROUPS) {
            if (constraint_group.solutions.empty()) {
                continue;
            }

            const auto &first_solution = constraint_group.solutions.front();

            for (const auto &first_pair : first_solution) {
                auto     *variable_ptr = first_pair.first;
                const int fixed_value  = first_pair.second;

                bool is_same_for_all = true;

                for (const auto &solution : constraint_group.solutions) {
                    bool found = false;

                    for (const auto &pair : solution) {
                        if (pair.first == variable_ptr) {
                            if (pair.second != fixed_value) {
                                is_same_for_all = false;
                            }
                            found = true;
                            break;
                        }
                    }

                    if (!found || !is_same_for_all) {
                        is_same_for_all = false;
                        break;
                    }
                }

                if (is_same_for_all && !variable_ptr->is_fixed()) {
                    variable_ptr->fix_by(fixed_value);
                    number_of_fixed_variables++;

                    if (a_IS_ENABLED_PRINT) {
                        utility::print_message(
                            "The value of the variable " +
                                variable_ptr->name() + " was fixed at " +
                                utility::to_string(fixed_value, "%d") +
                                " by partial feasible enumeration.",
                            a_IS_ENABLED_PRINT);
                    }
                }
            }
        }
        utility::print_message("Done.", a_IS_ENABLED_PRINT);
        return ProblemSizeReducerResult(number_of_fixed_variables, 0, 0);
    }

    /*************************************************************************/
    inline ProblemSizeReducerResult
    remove_implicit_fixed_variables_from_precedence_constraints(
        const bool a_IS_ENABLED_PRINT) {
        utility::print_single_line(a_IS_ENABLED_PRINT);
        utility::print_message(
            "Removing implicit fixed variables from precedence and set "
            "partitioning/packing constraints...",
            a_IS_ENABLED_PRINT);

        auto &reference = m_model_ptr->reference().constraint_type;
        int   number_of_fixed_variables = 0;

        auto set_partitioning_packing_ptrs = reference.set_partitioning_ptrs;

        set_partitioning_packing_ptrs.insert(  //
            set_partitioning_packing_ptrs.end(),
            reference.set_packing_ptrs.begin(),
            reference.set_packing_ptrs.end());

        if (reference.precedence_ptrs.empty() ||
            set_partitioning_packing_ptrs.empty()) {
            utility::print_message("Done.", a_IS_ENABLED_PRINT);
            return ProblemSizeReducerResult(number_of_fixed_variables, 0, 0);
        }

        for (auto &&precedence_ptr : reference.precedence_ptrs) {
            if (!precedence_ptr->is_enabled()) {
                continue;
            }
            const auto &SENSITIVITIES_PRECEDENCE =
                precedence_ptr->expression().sensitivities();
            for (auto &&set_partitioning_packing_ptr :
                 set_partitioning_packing_ptrs) {
                if (!set_partitioning_packing_ptr->is_enabled()) {
                    continue;
                }

                const auto &SENSITIVITIES_SET_PARTITIONING_PACKING =
                    set_partitioning_packing_ptr->expression().sensitivities();

                bool is_all_included = true;
                for (const auto &sensitivity : SENSITIVITIES_PRECEDENCE) {
                    if (sensitivity.first->is_fixed()) {
                        continue;
                    }

                    if (SENSITIVITIES_SET_PARTITIONING_PACKING.find(
                            sensitivity.first) ==
                        SENSITIVITIES_SET_PARTITIONING_PACKING.end()) {
                        is_all_included = false;
                    }
                }
                if (is_all_included) {
                    for (const auto &sensitivity : SENSITIVITIES_PRECEDENCE) {
                        if (sensitivity.first->is_fixed()) {
                            continue;
                        }

                        const bool FIX_FLAG =
                            (sensitivity.second > 0 &&
                             precedence_ptr->sense() ==
                                 model_component::ConstraintSense::Less) ||
                            (sensitivity.second < 0 &&
                             precedence_ptr->sense() ==
                                 model_component::ConstraintSense::Greater);

                        if (FIX_FLAG) {
                            sensitivity.first->fix_by(0);
                            number_of_fixed_variables++;

                            if (a_IS_ENABLED_PRINT) {
                                utility::print_message(
                                    "The value of the variable " +
                                        sensitivity.first->name() +
                                        " was fixed at 0.",
                                    a_IS_ENABLED_PRINT);
                            }
                            precedence_ptr->disable();
                        }
                    }
                }
            }
        }

        utility::print_message("Done.", a_IS_ENABLED_PRINT);
        return ProblemSizeReducerResult(number_of_fixed_variables, 0, 0);
    }
};
}  // namespace printemps::preprocess
#endif
/*****************************************************************************/
// END
/*****************************************************************************/
