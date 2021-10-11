/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_PRESOLVER_PROBLEM_SIZE_REDUCER_H__
#define PRINTEMPS_PRESOLVER_PROBLEM_SIZE_REDUCER_H__

namespace printemps {
namespace presolver {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
constexpr bool remove_independent_variable(
    model_component::Variable<T_Variable, T_Expression> *a_variable_ptr,     //
    const bool &                                         a_IS_MINIMIZATION,  //
    const bool                                           a_IS_ENABLED_PRINT) {
    bool is_removed = false;

    if (a_variable_ptr->related_constraint_ptrs().size() == 0) {
        auto sensitivity = a_variable_ptr->objective_sensitivity();
        if (fabs(sensitivity) < constant::EPSILON_10) {
            utility::print_message(
                "The value of the decision variable " + a_variable_ptr->name() +
                    " was fixed as " + std::to_string(0) +
                    " because it does not have sensitivity to any constraint "
                    "or objective function.",
                a_IS_ENABLED_PRINT);
            a_variable_ptr->fix_by(0);
            is_removed = true;
        } else {
            if (sensitivity > 0) {
                if (a_IS_MINIMIZATION) {
                    auto fix_value = a_variable_ptr->lower_bound();
                    utility::print_message(
                        "The value of the decision variable " +
                            a_variable_ptr->name() +
                            " was fixed as its lower bound " +
                            std::to_string(fix_value) +
                            " because it does not have sensitivity to any "
                            "constraint, and the sensitivity to the objective "
                            "function to be minimized is positive",
                        a_IS_ENABLED_PRINT);
                    a_variable_ptr->fix_by(fix_value);
                    is_removed = true;
                } else {
                    auto fix_value = a_variable_ptr->upper_bound();
                    utility::print_message(
                        "The value of the decision variable " +
                            a_variable_ptr->name() +
                            " was fixed as its upper bound " +
                            std::to_string(fix_value) +
                            " because it does not have sensitivity to any "
                            "constraint, and the sensitivity to the objective "
                            "function to be maximized is positive",
                        a_IS_ENABLED_PRINT);
                    a_variable_ptr->fix_by(fix_value);
                    is_removed = true;
                }
            } else {
                if (a_IS_MINIMIZATION) {
                    auto fix_value = a_variable_ptr->upper_bound();
                    utility::print_message(
                        "The value of the decision variable " +
                            a_variable_ptr->name() +
                            " was fixed as its upper bound " +
                            std::to_string(fix_value) +
                            " because it does not have sensitivity to any "
                            "constraint, and the sensitivity to the objective "
                            "function to be minimized is negative",
                        a_IS_ENABLED_PRINT);
                    a_variable_ptr->fix_by(fix_value);
                    is_removed = true;
                } else {
                    auto fix_value = a_variable_ptr->lower_bound();
                    utility::print_message(
                        "The value of the decision variable " +
                            a_variable_ptr->name() +
                            " was fixed as its lower bound " +
                            std::to_string(fix_value) +
                            " because it does not have sensitivity to any "
                            "constraint, and the sensitivity to the objective "
                            "function to be maximized is positive",
                        a_IS_ENABLED_PRINT);
                    a_variable_ptr->fix_by(fix_value);
                    is_removed = true;
                }
            }
        }
    }

    return is_removed;
}

/*****************************************************************************/
template <class T_Variable, class T_Expression>
constexpr bool fix_implicit_fixed_variable(
    model_component::Variable<T_Variable, T_Expression> *a_variable_ptr,  //
    const bool                                           a_IS_ENABLED_PRINT) {
    bool is_removed = false;

    auto lower_bound = a_variable_ptr->lower_bound();
    auto upper_bound = a_variable_ptr->upper_bound();
    if (lower_bound == upper_bound) {
        auto fixed_value = lower_bound;

        utility::print_message(
            "The value of the decision variable " + a_variable_ptr->name() +
                " was fixed as " + std::to_string(fixed_value) +
                " because the lower bound " + std::to_string(lower_bound) +
                " and the upper_bound " + std::to_string(upper_bound) +
                " implicitly fix the value.",
            a_IS_ENABLED_PRINT);
        a_variable_ptr->fix_by(fixed_value);
        is_removed = true;
    }
    return is_removed;
}

/*****************************************************************************/
template <class T_Variable, class T_Expression>
constexpr int remove_independent_variables(
    model::Model<T_Variable, T_Expression> *a_model_ptr,  //
    const bool                              a_IS_ENABLED_PRINT) {
    int number_of_newly_fixed_variables = 0;
    for (auto &&proxy : a_model_ptr->variable_proxies()) {
        for (auto &&variable : proxy.flat_indexed_variables()) {
            /**
             * If the decision variable has already been fixed, the
             * following procedures will be skipped.
             */
            if (variable.is_fixed()) {
                continue;
            }
            if (remove_independent_variable(&variable,                       //
                                            a_model_ptr->is_minimization(),  //
                                            a_IS_ENABLED_PRINT)) {
                number_of_newly_fixed_variables++;
            }
        }
    }
    return number_of_newly_fixed_variables;
}

/*****************************************************************************/
template <class T_Variable, class T_Expression>
constexpr int fix_implicit_fixed_variables(
    model::Model<T_Variable, T_Expression> *a_model_ptr,  //
    const bool                              a_IS_ENABLED_PRINT) {
    int number_of_newly_fixed_variables = 0;
    for (auto &&proxy : a_model_ptr->variable_proxies()) {
        for (auto &&variable : proxy.flat_indexed_variables()) {
            /**
             * If the decision variable has already been fixed, the
             * following procedures will be skipped.
             */
            if (variable.is_fixed()) {
                continue;
            }

            if (fix_implicit_fixed_variable(&variable, a_IS_ENABLED_PRINT)) {
                number_of_newly_fixed_variables++;
            }
        }
    }
    return number_of_newly_fixed_variables;
}
/*****************************************************************************/
template <class T_Variable, class T_Expression>
constexpr int fix_redundant_set_variables(
    model::Model<T_Variable, T_Expression> *a_model_ptr,  //
    const bool                              a_IS_ENABLED_PRINT) {
    int number_of_set_partitionings =
        a_model_ptr->constraint_type_reference().set_partitioning_ptrs.size();
    int number_of_set_coverings =
        a_model_ptr->constraint_type_reference().set_covering_ptrs.size();
    int number_of_set_packings =
        a_model_ptr->constraint_type_reference().set_packing_ptrs.size();

    /**
     * If the problem is unconstrained, the following procedures will be
     * skipped.
     */
    if (a_model_ptr->number_of_constraints() == 0) {
        return 0;
    }

    /**
     * If the problem is not pure set partitioning/covering/packing problem, the
     * following procedures will be skipped.
     */
    if (a_model_ptr->number_of_constraints() !=
        (number_of_set_partitionings + number_of_set_coverings +
         number_of_set_packings)) {
        return 0;
    }
    int  number_of_newly_fixed_variables = 0;
    auto variable_ptrs = a_model_ptr->variable_reference().variable_ptrs;

    /**
     * Pre-sort the decision variables pointers for efficient subsequent
     * procedures.
     */
    if (a_model_ptr->is_minimization()) {
        std::sort(variable_ptrs.begin(), variable_ptrs.end(),
                  [](const auto &a_LHS, const auto &a_RHS) {
                      return (a_LHS->related_constraint_ptrs() ==
                              a_RHS->related_constraint_ptrs())
                                 ? (a_LHS->objective_sensitivity() >
                                    a_RHS->objective_sensitivity())
                                 : (a_LHS->related_constraint_ptrs().size() <
                                    a_RHS->related_constraint_ptrs().size());
                  });
    } else {
        std::sort(variable_ptrs.begin(), variable_ptrs.end(),
                  [](const auto &a_LHS, const auto &a_RHS) {
                      return (a_LHS->related_constraint_ptrs() ==
                              a_RHS->related_constraint_ptrs())
                                 ? (a_LHS->objective_sensitivity() <
                                    a_RHS->objective_sensitivity())
                                 : (a_LHS->related_constraint_ptrs().size() <
                                    a_RHS->related_constraint_ptrs().size());
                  });
    }

    for (auto i = 0; i < a_model_ptr->number_of_variables(); i++) {
        /**
         * If the decision variable has already been fixed, the
         * following procedures will be skipped.
         */
        if (variable_ptrs[i]->is_fixed()) {
            continue;
        }

        /**
         * Fix the value of x_{i} by 0 if there exists a decision variable x_{j}
         * which has same constraint coefficient patterns as x_{i} and not
         * inferior objective coefficient than that of x_{i}.
         */
        for (auto j = i + 1; j < a_model_ptr->number_of_variables(); j++) {
            /**
             * If the number of non-zero coefficients of x_{j} is larger than
             * that of x_{i}, the inner loop can be terminated because the all
             * the following decision variables also have larger number of
             * non-zero coefficients, by the pre-sort.
             */
            if (variable_ptrs[i]->related_constraint_ptrs().size() <
                variable_ptrs[j]->related_constraint_ptrs().size()) {
                break;
            }

            /**
             * If x_{j} is fixed as 0, the following procedure can be skipped.
             */
            if (variable_ptrs[j]->is_fixed() &&
                variable_ptrs[j]->value() == 0) {
                continue;
            }

            /**
             * If the constraint coefficient pattern of x_{j} differs from that
             * of x_{i}, the following procedure can be skipped.
             */
            if (variable_ptrs[i]->constraint_sensitivities() !=
                variable_ptrs[j]->constraint_sensitivities()) {
                continue;
            }

            /**
             * If x_{j} has superior objective coefficient than that of x_{i},
             * the value of x_{i} will be fixed as 0 and break.
             */
            if ((a_model_ptr->is_minimization() &&
                 (variable_ptrs[i]->objective_sensitivity() >=
                  variable_ptrs[j]->objective_sensitivity())) ||
                (!a_model_ptr->is_minimization() &&
                 (variable_ptrs[i]->objective_sensitivity() <=
                  variable_ptrs[j]->objective_sensitivity()))) {
                variable_ptrs[i]->fix_by(0);
                utility::print_message(
                    "The value of redundant decision variable " +
                        variable_ptrs[i]->name() + " was fixed as " +
                        std::to_string(0) + ".",
                    a_IS_ENABLED_PRINT);

                number_of_newly_fixed_variables++;
                break;

            }
            /**
             * If x_{j} does not have superior objective coefficient than that
             * of x_{i}, the value of x_{j} will be fixed as 0.
             */
            else if ((a_model_ptr->is_minimization() &&
                      (variable_ptrs[i]->objective_sensitivity() <
                       variable_ptrs[j]->objective_sensitivity())) ||
                     (!a_model_ptr->is_minimization() &&
                      (variable_ptrs[i]->objective_sensitivity() >
                       variable_ptrs[j]->objective_sensitivity()))) {
                variable_ptrs[j]->fix_by(0);
                utility::print_message(
                    "The value of redundant decision variable " +
                        variable_ptrs[j]->name() + " was fixed as " +
                        std::to_string(0) + ".",
                    a_IS_ENABLED_PRINT);

                number_of_newly_fixed_variables++;
            }
        }
    }

    return number_of_newly_fixed_variables;
}

/*****************************************************************************/
template <class T_Variable, class T_Expression>
constexpr bool remove_redundant_constraint_with_tightening_variable_bound(
    model_component::Constraint<T_Variable, T_Expression> *a_constraint_ptr,  //
    const bool                                             a_IS_ENABLED_PRINT) {
    const int BOUND_LIMIT = 100000;

    bool is_removed = false;

    auto mutable_variable_sensitivities =
        a_constraint_ptr->expression().mutable_variable_sensitivities();

    auto positive_mutable_variable_sensitivities =
        a_constraint_ptr->expression()
            .positive_mutable_variable_sensitivities();

    auto negative_mutable_variable_sensitivities =
        a_constraint_ptr->expression()
            .negative_mutable_variable_sensitivities();

    double constraint_lower_bound =
        a_constraint_ptr->expression().lower_bound();

    double constraint_upper_bound =
        a_constraint_ptr->expression().upper_bound();

    double constraint_fixed_term_value =
        a_constraint_ptr->expression().fixed_term_value();

    auto constraint_constant_value =
        a_constraint_ptr->expression().constant_value();

    /**
     * If the constraint is always satisfied obviously, it will be removed.
     */
    if ((a_constraint_ptr->sense() == model_component::ConstraintSense::Equal &&
         constraint_lower_bound == 0 && constraint_upper_bound == 0) ||
        (a_constraint_ptr->sense() == model_component::ConstraintSense::Less &&
         constraint_upper_bound <= 0) ||
        (a_constraint_ptr->sense() ==
             model_component::ConstraintSense::Greater &&
         constraint_lower_bound >= 0)) {
        utility::print_message("The redundant constraint " +
                                   a_constraint_ptr->name() + " was removed.",
                               a_IS_ENABLED_PRINT);

        if (a_constraint_ptr->is_enabled()) {
            a_constraint_ptr->disable();
            is_removed = true;
            return is_removed;
        }
    }

    /**
     * The detected singleton constaint will be disabled instead of fixing or
     * tightening the lower and upper bounds of the decision variable included
     * in the constraint.
     */
    if (mutable_variable_sensitivities.size() == 1) {
        auto variable_ptr = mutable_variable_sensitivities.begin()->first;
        auto coefficient  = mutable_variable_sensitivities.begin()->second;

        auto variable_lower_bound = variable_ptr->lower_bound();
        auto variable_upper_bound = variable_ptr->upper_bound();

        auto bound_temp =
            -(constraint_fixed_term_value + constraint_constant_value) /
            coefficient;

        if (a_constraint_ptr->sense() ==
            model_component::ConstraintSense::Equal) {
            /**
             * If the singleton constraint is defined by an equality as ax+b=0,
             * the value of the decision variable x will be fixed as -b/a.
             */
            utility::print_message(  //
                "The constraint " + a_constraint_ptr->name() +
                    " was removed instead of fixing the value of the decision "
                    "variable " +
                    variable_ptr->name() + " by " + std::to_string(bound_temp) +
                    ".",
                a_IS_ENABLED_PRINT);

            variable_ptr->fix_by(bound_temp);
            if (a_constraint_ptr->is_enabled()) {
                a_constraint_ptr->disable();
                is_removed = true;
                return is_removed;
            }
        } else if ((a_constraint_ptr->sense() ==
                        model_component::ConstraintSense::Less &&
                    coefficient > 0) ||
                   (a_constraint_ptr->sense() ==
                        model_component::ConstraintSense::Greater &&
                    coefficient < 0)) {
            /**
             * If the singleton constraint is defined by an inequality as
             * ax+b<=0 with a>0 (or ax+b>=0 with a<0), the lower bound of the
             * decision variable will be tightened by floor(-b/a).
             */
            auto bound_floor = static_cast<T_Variable>(std::floor(bound_temp));

            if (bound_floor < variable_upper_bound &&
                abs(bound_floor) < BOUND_LIMIT) {
                if (a_constraint_ptr->name() == "") {
                    utility::print_message(
                        "The upper bound of the decision variable " +
                            variable_ptr->name() + " was tightened by " +
                            std::to_string(bound_floor) + ".",
                        a_IS_ENABLED_PRINT);
                } else {
                    utility::print_message(
                        "The constraint " + a_constraint_ptr->name() +
                            " was removed instead of tightening the upper "
                            "bound of the decision variable " +
                            variable_ptr->name() + " by " +
                            std::to_string(bound_floor) + ".",
                        a_IS_ENABLED_PRINT);
                }
                variable_ptr->set_bound(variable_lower_bound, bound_floor);
            } else {
                utility::print_message(  //
                    "The redundant constraint " + a_constraint_ptr->name() +
                        " was removed.",
                    a_IS_ENABLED_PRINT);
            }
            if (a_constraint_ptr->is_enabled()) {
                a_constraint_ptr->disable();
                is_removed = true;
                return is_removed;
            }

        } else if ((a_constraint_ptr->sense() ==
                        model_component::ConstraintSense::Greater &&
                    coefficient > 0) ||
                   (a_constraint_ptr->sense() ==
                        model_component::ConstraintSense::Less &&
                    coefficient < 0)) {
            /**
             * If the singleton constraint is defined by an inequality as
             * ax+b>=0 with a>0 (or ax+b<=0 with a<0), the upper bound of the
             * decision variable will be tightened by ceil(-b/a).
             */
            auto bound_ceil = static_cast<T_Variable>(std::ceil(bound_temp));

            if (bound_ceil > variable_lower_bound &&
                abs(bound_ceil) < BOUND_LIMIT) {
                if (a_constraint_ptr->name() == "") {
                    utility::print_message(
                        "The lower bound of the decision variable " +
                            variable_ptr->name() + " was tightened by " +
                            std::to_string(bound_ceil) + ".",
                        a_IS_ENABLED_PRINT);
                } else {
                    utility::print_message(
                        "The constraint " + a_constraint_ptr->name() +
                            " was removed instead of tightening the lower "
                            "bound of the decision variable " +
                            variable_ptr->name() + " by " +
                            std::to_string(bound_ceil) + ".",
                        a_IS_ENABLED_PRINT);
                }
                variable_ptr->set_bound(bound_ceil, variable_upper_bound);
            } else {
                utility::print_message(  //
                    "The redundant constraint " + a_constraint_ptr->name() +
                        " was removed.",
                    a_IS_ENABLED_PRINT);
            }
            if (a_constraint_ptr->is_enabled()) {
                a_constraint_ptr->disable();
                is_removed = true;
                return is_removed;
            }
        }
        return is_removed;
    }

    /**
     * Tighten the lower and upper bounds of the decision variables based on the
     * bounds of the rest part.
     */
    for (auto &&sensitivity : positive_mutable_variable_sensitivities) {
        auto variable_ptr = sensitivity.first;
        auto coefficient  = sensitivity.second;

        auto variable_lower_bound = variable_ptr->lower_bound();
        auto variable_upper_bound = variable_ptr->upper_bound();

        if (a_constraint_ptr->is_greater_or_equal()) {
            auto bound_temp =
                -(constraint_upper_bound - coefficient * variable_upper_bound) /
                coefficient;
            auto bound_ceil = static_cast<T_Variable>(std::ceil(bound_temp));
            if (bound_ceil > variable_lower_bound &&
                abs(bound_ceil) < BOUND_LIMIT) {
                utility::print_message(
                    "The lower bound of the decision variable " +
                        variable_ptr->name() + " was tightened by " +
                        std::to_string(bound_ceil) + ".",
                    a_IS_ENABLED_PRINT);
                variable_ptr->set_bound(bound_ceil, variable_upper_bound);
            }
        }
        if (a_constraint_ptr->is_less_or_equal()) {
            auto bound_temp =
                -(constraint_lower_bound - coefficient * variable_lower_bound) /
                coefficient;
            auto bound_floor = static_cast<T_Variable>(std::floor(bound_temp));
            if (bound_floor < variable_upper_bound &&
                abs(bound_floor) < BOUND_LIMIT) {
                utility::print_message(
                    "The upper bound of the decision variable " +
                        variable_ptr->name() + " was tightened by " +
                        std::to_string(bound_floor) + ".",
                    a_IS_ENABLED_PRINT);
                variable_ptr->set_bound(variable_lower_bound, bound_floor);
            }
        }
    }

    for (auto &&sensitivity : negative_mutable_variable_sensitivities) {
        auto variable_ptr = sensitivity.first;
        auto coefficient  = sensitivity.second;

        auto variable_lower_bound = variable_ptr->lower_bound();
        auto variable_upper_bound = variable_ptr->upper_bound();

        if (a_constraint_ptr->is_greater_or_equal()) {
            auto bound_temp =
                -(constraint_upper_bound - coefficient * variable_lower_bound) /
                coefficient;
            auto bound_floor = static_cast<T_Variable>(std::floor(bound_temp));
            if (bound_floor < variable_upper_bound &&
                abs(bound_floor) < BOUND_LIMIT) {
                utility::print_message(
                    "The upper bound of the decision variable " +
                        variable_ptr->name() + " was tightened by " +
                        std::to_string(bound_floor) + ".",
                    a_IS_ENABLED_PRINT);
                variable_ptr->set_bound(variable_lower_bound, bound_floor);
            }
        }
        if (a_constraint_ptr->is_less_or_equal()) {
            auto bound_temp =
                -(constraint_lower_bound - coefficient * variable_upper_bound) /
                coefficient;
            auto bound_ceil = static_cast<T_Variable>(std::ceil(bound_temp));
            if (bound_ceil > variable_lower_bound &&
                abs(bound_ceil) < BOUND_LIMIT) {
                utility::print_message(
                    "The lower bound of the decision variable " +
                        variable_ptr->name() + " was tightened by " +
                        std::to_string(bound_ceil) + ".",
                    a_IS_ENABLED_PRINT);
                variable_ptr->set_bound(bound_ceil, variable_upper_bound);
            }
        }
    }
    return is_removed;
}

/*****************************************************************************/
template <class T_Variable, class T_Expression>
constexpr int remove_redundant_constraints_with_tightening_variable_bounds(
    model::Model<T_Variable, T_Expression> *a_model_ptr,  //
    const bool                              a_IS_ENABLED_PRINT) {
    int number_of_newly_disabled_constraints = 0;
    for (auto &&proxy : a_model_ptr->constraint_proxies()) {
        for (auto &&constraint : proxy.flat_indexed_constraints()) {
            /**
             * If the constraint is nonlinear, the following procedures
             * will be skipped.
             */
            if (!constraint.is_linear()) {
                continue;
            }

            /**
             * If the constraint has already been disabled, the following
             * procedures will be skipped.
             */
            if (!constraint.is_enabled()) {
                continue;
            }

            if (remove_redundant_constraint_with_tightening_variable_bound(
                    &constraint,  //
                    a_IS_ENABLED_PRINT)) {
                number_of_newly_disabled_constraints++;
            }
        }
    }
    return number_of_newly_disabled_constraints;
}

/*****************************************************************************/
template <class T_Variable, class T_Expression>
constexpr std::pair<int, int> remove_redundant_set_constraints(
    model::Model<T_Variable, T_Expression> *a_model_ptr,  //
    const bool                              a_IS_ENABLED_PRINT) {
    /**
     * NOTE: This function is preliminarily  implemented and not incorporated in
     * the algorithm.
     */
    utility::print_single_line(a_IS_ENABLED_PRINT);
    utility::print_message(
        "Removing redundant set patritioning constraints and included decision "
        "variables...",
        a_IS_ENABLED_PRINT);

    auto set_partitioning_ptrs =
        a_model_ptr->constraint_type_reference().set_partitioning_ptrs;

    int number_of_newly_disabled_constraints = 0;
    int number_of_newly_fixed_variables      = 0;

    const int SET_PARTITIONINGS_SIZE = set_partitioning_ptrs.size();

    std::sort(set_partitioning_ptrs.begin(), set_partitioning_ptrs.end(),
              [](const auto &a_LHS, const auto &a_RHS) {
                  return a_LHS->expression().sensitivities().size() >
                         a_RHS->expression().sensitivities().size();
              });

    std::vector<std::unordered_set<
        model_component::Variable<T_Variable, T_Expression> *>>
        variable_ptr_sets(SET_PARTITIONINGS_SIZE);

    for (auto i = 0; i < SET_PARTITIONINGS_SIZE; i++) {
        for (const auto &sensitivity :
             set_partitioning_ptrs[i]->expression().sensitivities()) {
            variable_ptr_sets[i].insert(sensitivity.first);
        }
    }

    for (auto i = 0; i < SET_PARTITIONINGS_SIZE - 1; i++) {
        for (auto j = i + 1; j < SET_PARTITIONINGS_SIZE; j++) {
            bool is_included = true;
            for (const auto &variable_ptr : variable_ptr_sets[j]) {
                if (variable_ptr_sets[i].find(variable_ptr) ==
                    variable_ptr_sets[i].end()) {
                    is_included = false;
                    break;
                }
            }

            if (is_included) {
                set_partitioning_ptrs[i]->disable();
                utility::print_message(  //
                    "The redundant set partitioning constraint " +
                        set_partitioning_ptrs[i]->name() + " was removed.",
                    a_IS_ENABLED_PRINT);

                for (const auto &variable_ptr : variable_ptr_sets[i]) {
                    if (variable_ptr_sets[j].find(variable_ptr) ==
                        variable_ptr_sets[j].end()) {
                        variable_ptr->fix_by(0);
                        utility::print_message(  //
                            "The value of redundant decision variable " +
                                variable_ptr->name() +
                                " in partitioning constraint " +
                                set_partitioning_ptrs[i]->name() +
                                " was fixed as 0.",
                            a_IS_ENABLED_PRINT);
                        number_of_newly_fixed_variables++;
                    }
                }

                number_of_newly_disabled_constraints++;
                break;
            }
        }
    }

    utility::print_message("Done.", a_IS_ENABLED_PRINT);
    return std::make_pair(number_of_newly_disabled_constraints,
                          number_of_newly_fixed_variables);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression>
constexpr bool reduce_problem_size(
    model::Model<T_Variable, T_Expression> *a_model_ptr,  //
    const bool a_IS_ENABLED_PAIRWISE_PRESOLVE, const bool a_IS_ENABLED_PRINT) {
    utility::print_single_line(a_IS_ENABLED_PRINT);
    utility::print_message("Reducing problem size...", a_IS_ENABLED_PRINT);

    int number_of_disabled_constaints = 0;
    int number_of_fixed_variables     = 0;

    if (a_model_ptr->is_linear()) {
        number_of_fixed_variables +=
            remove_independent_variables(a_model_ptr, a_IS_ENABLED_PRINT);
    }

    while (true) {
        int number_of_newly_disabled_constaints  //
            = remove_redundant_constraints_with_tightening_variable_bounds(
                a_model_ptr, a_IS_ENABLED_PRINT);

        int number_of_newly_fixed_variables  //
            = fix_implicit_fixed_variables(a_model_ptr, a_IS_ENABLED_PRINT);

        number_of_disabled_constaints += number_of_newly_disabled_constaints;
        number_of_fixed_variables += number_of_newly_fixed_variables;

        if (number_of_newly_disabled_constaints == 0 &&
            number_of_newly_fixed_variables == 0) {
            break;
        }
    }

    if (a_model_ptr->is_linear() && a_IS_ENABLED_PAIRWISE_PRESOLVE) {
        const int MAX_CONSIDERABLE_NUMBER_OF_VARIABLES = 100000;
        if (a_model_ptr->number_of_variables() <=
            MAX_CONSIDERABLE_NUMBER_OF_VARIABLES) {
            number_of_fixed_variables +=
                fix_redundant_set_variables(a_model_ptr, a_IS_ENABLED_PRINT);
        }
    }
    utility::print_message("Done.", a_IS_ENABLED_PRINT);

    return (number_of_disabled_constaints > 0) ||
           (number_of_fixed_variables > 0);
}
}  // namespace presolver
}  // namespace printemps
#endif
/*****************************************************************************/
// END
/*****************************************************************************/