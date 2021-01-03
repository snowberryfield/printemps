/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MODEL_PRESOLVER_H__
#define PRINTEMPS_MODEL_PRESOLVER_H__

#include <vector>
#include <string>
#include <numeric>
#include <functional>
#include <cmath>

#include "variable_sense.h"
#include "constraint_sense.h"
#include "move_sense.h"
#include "range.h"
#include "selection_mode.h"

#include "move.h"
#include "variable_proxy.h"
#include "expression_proxy.h"
#include "constraint_proxy.h"
#include "objective.h"
#include "value_proxy.h"
#include "solution.h"
#include "model_summary.h"
#include "named_solution.h"
#include "plain_solution.h"
#include "solution_score.h"
#include "selection.h"
#include "neighborhood.h"

#include "expression_binary_operator.h"
#include "constraint_binary_operator.h"

#include "variable_reference.h"
#include "constraint_reference.h"
#include "constraint_type_reference.h"

namespace printemps {
namespace model {
/*************************************************************************/
template <class T_Variable, class T_Expression>
inline constexpr int remove_independent_variables(
    Model<T_Variable, T_Expression> *a_model, const bool a_IS_ENABLED_PRINT) {
    int number_of_newly_fixed_variables = 0;
    for (auto &&proxy : a_model->variable_proxies()) {
        for (auto &&variable : proxy.flat_indexed_variables()) {
            /**
             * If the decision variable has already been fixed, the
             * following procedures will be skipped.
             */
            if (variable.is_fixed()) {
                continue;
            }
            auto &objective_sensitivities =
                a_model->objective().expression().sensitivities();

            if (variable.related_constraint_ptrs().size() == 0) {
                if (objective_sensitivities.find(&variable) ==
                    objective_sensitivities.end()) {
                    utility::print_message(
                        "The value of decision variable " + variable.name() +
                            " was fixed by " + std::to_string(0) +
                            "because it does not have sensitivity to any "
                            "constraint or objective function.",
                        a_IS_ENABLED_PRINT);
                    variable.fix_by(0);
                    number_of_newly_fixed_variables++;
                } else {
                    auto sensitivity = objective_sensitivities.at(&variable);

                    if (sensitivity > 0) {
                        if (a_model->is_minimization()) {
                            auto fix_value = variable.lower_bound();
                            utility::print_message(
                                "The value of decision variable " +
                                    variable.name() +
                                    " was fixed by its lower bound " +
                                    std::to_string(fix_value) +
                                    " because it does not have sensitivity to "
                                    "any constraint, and the sensitivity to "
                                    "the objective function to be minimized is "
                                    "positive",
                                a_IS_ENABLED_PRINT);
                            variable.fix_by(fix_value);
                            number_of_newly_fixed_variables++;
                        } else {
                            auto fix_value = variable.upper_bound();
                            utility::print_message(
                                "The value of decision variable " +
                                    variable.name() +
                                    " was fixed by its upper bound " +
                                    std::to_string(fix_value) +
                                    " because it does not have sensitivity to "
                                    "any constraint, and the sensitivity to "
                                    "the objective function to be maximized is "
                                    "positive",
                                a_IS_ENABLED_PRINT);
                            variable.fix_by(fix_value);
                            number_of_newly_fixed_variables++;
                        }
                    } else {
                        if (a_model->is_minimization()) {
                            auto fix_value = variable.upper_bound();
                            utility::print_message(
                                "The value of decision variable " +
                                    variable.name() +
                                    " was fixed by its upper bound " +
                                    std::to_string(fix_value) +
                                    " because it does not have sensitivity to "
                                    "any constraint, and the sensitivity to "
                                    "the objective function to be minimized is "
                                    "negative",
                                a_IS_ENABLED_PRINT);
                            variable.fix_by(fix_value);
                            number_of_newly_fixed_variables++;
                        } else {
                            auto fix_value = variable.lower_bound();
                            utility::print_message(
                                "The value of decision variable " +
                                    variable.name() +
                                    " was fixed by its lower bound " +
                                    std::to_string(fix_value) +
                                    " because it does not have sensitivity to "
                                    "any constraint, and the sensitivity to "
                                    "the objective function to be maximized is "
                                    "positive",
                                a_IS_ENABLED_PRINT);
                            variable.fix_by(fix_value);
                            number_of_newly_fixed_variables++;
                        }
                    }
                }
            }
        }
    }
    return number_of_newly_fixed_variables;
}

/*************************************************************************/
template <class T_Variable, class T_Expression>
inline constexpr int
remove_redundant_constraints_with_tightening_variable_bounds(
    Model<T_Variable, T_Expression> *a_model, const bool a_IS_ENABLED_PRINT) {
    const int BOUND_LIMIT = 100000;

    int number_of_newly_disabled_constraints = 0;
    for (auto &&proxy : a_model->constraint_proxies()) {
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

            auto &sensitivities  = constraint.expression().sensitivities();
            auto  constant_value = constraint.expression().constant_value();

            std::vector<
                std::pair<Variable<T_Variable, T_Expression> *, T_Expression>>
                not_fixed_variable_sensitivities;
            std::vector<
                std::pair<Variable<T_Variable, T_Expression> *, T_Expression>>
                positive_coefficient_not_fixed_variable_sensitivities;
            std::vector<
                std::pair<Variable<T_Variable, T_Expression> *, T_Expression>>
                negative_coefficient_not_fixed_variable_sensitivities;

            not_fixed_variable_sensitivities.reserve(sensitivities.size());
            positive_coefficient_not_fixed_variable_sensitivities.reserve(
                sensitivities.size());
            negative_coefficient_not_fixed_variable_sensitivities.reserve(
                sensitivities.size());
            int number_of_fixed_variables = 0;

            /**
             * The lower and bounds of a^{T}x and fixed value in b^{T}y in
             * a^{T}x + b^{T}y + c <=(>=,=) 0, where a^{T}x are terms whose
             * decision variables are not fixed, b^{T}y are terms with fixed
             * decision variables, and c is the constant term.
             */
            double not_fixed_term_lower_bound = 0.0;
            double not_fixed_term_upper_bound = 0.0;
            double fixed_term_value           = 0.0;

            for (const auto &sensitivity : sensitivities) {
                if (sensitivity.first->is_fixed()) {
                    number_of_fixed_variables++;
                    fixed_term_value +=
                        sensitivity.first->value() * sensitivity.second;
                } else {
                    if (sensitivity.second > 0) {
                        not_fixed_term_lower_bound +=
                            sensitivity.first->lower_bound() *
                            sensitivity.second;
                        not_fixed_term_upper_bound +=
                            sensitivity.first->upper_bound() *
                            sensitivity.second;
                        positive_coefficient_not_fixed_variable_sensitivities
                            .push_back(sensitivity);
                        not_fixed_variable_sensitivities.push_back(sensitivity);
                    } else {
                        not_fixed_term_lower_bound +=
                            sensitivity.first->upper_bound() *
                            sensitivity.second;
                        not_fixed_term_upper_bound +=
                            sensitivity.first->lower_bound() *
                            sensitivity.second;
                        negative_coefficient_not_fixed_variable_sensitivities
                            .push_back(sensitivity);
                        not_fixed_variable_sensitivities.push_back(sensitivity);
                    }
                }
            }

            /**
             * If the constraint is always satisfied obviously, it will be
             * removed.
             */
            if ((constraint.sense() == ConstraintSense::Equal &&
                 not_fixed_variable_sensitivities.size() == 0 &&
                 fixed_term_value + constant_value == 0) ||
                (constraint.sense() == ConstraintSense::Lower &&
                 not_fixed_term_upper_bound + fixed_term_value +
                         constant_value <=
                     0) ||
                (constraint.sense() == ConstraintSense::Upper &&
                 not_fixed_term_lower_bound + fixed_term_value +
                         constant_value >=
                     0)) {
                utility::print_message("The constraint " + constraint.name() +
                                           " was removed for redundancy.",
                                       a_IS_ENABLED_PRINT);

                if (constraint.is_enabled()) {
                    constraint.disable();
                    number_of_newly_disabled_constraints++;
                }
                continue;
            }

            /**
             * The detected singleton constaint will be disabled instead
             * of fixing or tightening the lower and upper bounds of the
             * decision variable included in the constraint.
             */
            if (not_fixed_variable_sensitivities.size() == 1) {
                auto variable_ptr =
                    not_fixed_variable_sensitivities.front().first;
                auto coefficient =
                    not_fixed_variable_sensitivities.front().second;

                auto lower_bound = variable_ptr->lower_bound();
                auto upper_bound = variable_ptr->upper_bound();

                auto bound_temp =
                    -(fixed_term_value + constant_value) / coefficient;

                if (constraint.sense() == ConstraintSense::Equal) {
                    /**
                     * If the singleton constraint is defined by an
                     * equality as ax+b=0, the value of the decision
                     * variable x will be fixed by -b/a.
                     */

                    utility::print_message(
                        "The constraint " + constraint.name() +
                            " was removed instead of fixing the value "
                            "of the decision variable " +
                            variable_ptr->name() + " by " +
                            std::to_string(bound_temp) + ".",
                        a_IS_ENABLED_PRINT);

                    variable_ptr->fix_by(bound_temp);
                    if (constraint.is_enabled()) {
                        constraint.disable();
                        number_of_newly_disabled_constraints++;
                    }
                } else if ((constraint.sense() == ConstraintSense::Lower &&
                            coefficient > 0) ||
                           (constraint.sense() == ConstraintSense::Upper &&
                            coefficient < 0)) {
                    /**
                     * If the singleton constraint is defined by an
                     * equality as ax+b<=0 with a>0 (or ax+b>=0 with a<0),
                     * the lower bound of the decision variable will be
                     * tightened by floor(-b/a).
                     */
                    auto bound_floor =
                        static_cast<T_Variable>(std::floor(bound_temp));

                    if (bound_floor < upper_bound &&
                        abs(bound_floor) < BOUND_LIMIT) {
                        utility::print_message(
                            "The constraint " + constraint.name() +
                                " was removed instead of tightening the upper "
                                "bound of the decision variable " +
                                variable_ptr->name() + " by " +
                                std::to_string(bound_floor) + ".",
                            a_IS_ENABLED_PRINT);
                        variable_ptr->set_bound(lower_bound, bound_floor);
                    } else {
                        utility::print_message(
                            "The constraint " + constraint.name() +
                                " was removed for redundancy.",
                            a_IS_ENABLED_PRINT);
                    }
                    if (constraint.is_enabled()) {
                        constraint.disable();
                        number_of_newly_disabled_constraints++;
                    }

                } else if ((constraint.sense() == ConstraintSense::Upper &&
                            coefficient > 0) ||
                           (constraint.sense() == ConstraintSense::Lower &&
                            coefficient < 0)) {
                    /**
                     * If the singleton constraint is defined by an
                     * equality as ax+b>=0 with a>0 (or ax+b<=0 with a<0),
                     * the upper bound of the decision variable will be
                     * tightened by ceil(-b/a).
                     */
                    auto bound_ceil =
                        static_cast<T_Variable>(std::ceil(bound_temp));

                    if (bound_ceil > lower_bound &&
                        abs(bound_ceil) < BOUND_LIMIT) {
                        utility::print_message(
                            "The constraint " + constraint.name() +
                                " was removed instead of tightening the lower "
                                "bound of the decision variable " +
                                variable_ptr->name() + " by " +
                                std::to_string(bound_ceil) + ".",
                            a_IS_ENABLED_PRINT);
                        variable_ptr->set_bound(bound_ceil, upper_bound);
                    } else {
                        utility::print_message(
                            "The constraint " + constraint.name() +
                                " was removed for redundancy.",
                            a_IS_ENABLED_PRINT);
                    }
                    if (constraint.is_enabled()) {
                        constraint.disable();
                        number_of_newly_disabled_constraints++;
                    }
                }
                continue;
            }

            /**
             * Tighten the lower and upper bounds of the decision variables
             * based on the bounds of the rest part.
             */
            for (auto &sensitivity :
                 positive_coefficient_not_fixed_variable_sensitivities) {
                auto variable_ptr = sensitivity.first;
                auto coefficient  = sensitivity.second;

                auto lower_bound = variable_ptr->lower_bound();
                auto upper_bound = variable_ptr->upper_bound();

                if (constraint.sense() == ConstraintSense::Upper) {
                    auto bound_temp = -(not_fixed_term_upper_bound -
                                        coefficient * upper_bound +
                                        fixed_term_value + constant_value) /
                                      coefficient;
                    auto bound_ceil =
                        static_cast<T_Variable>(std::ceil(bound_temp));
                    if (bound_ceil > lower_bound &&
                        abs(bound_ceil) < BOUND_LIMIT) {
                        utility::print_message(
                            "The lower bound of the decision variable " +
                                variable_ptr->name() + " was tightened by " +
                                std::to_string(bound_ceil) + ".",
                            a_IS_ENABLED_PRINT);
                        variable_ptr->set_bound(bound_ceil, upper_bound);
                    }
                } else if (constraint.sense() == ConstraintSense::Lower) {
                    auto bound_temp = -(not_fixed_term_lower_bound -
                                        coefficient * lower_bound +
                                        fixed_term_value + constant_value) /
                                      coefficient;
                    auto bound_floor =
                        static_cast<T_Variable>(std::floor(bound_temp));
                    if (bound_floor < upper_bound &&
                        abs(bound_floor) < BOUND_LIMIT) {
                        utility::print_message(
                            "The upper bound of the decision variable " +
                                variable_ptr->name() + " was tightened by " +
                                std::to_string(bound_floor) + ".",
                            a_IS_ENABLED_PRINT);
                        variable_ptr->set_bound(lower_bound, bound_floor);
                    }
                }
            }

            for (auto &sensitivity :
                 negative_coefficient_not_fixed_variable_sensitivities) {
                auto variable_ptr = sensitivity.first;
                auto coefficient  = sensitivity.second;

                auto lower_bound = variable_ptr->lower_bound();
                auto upper_bound = variable_ptr->upper_bound();

                if (constraint.sense() == ConstraintSense::Upper) {
                    auto bound_temp = -(not_fixed_term_upper_bound -
                                        coefficient * lower_bound +
                                        fixed_term_value + constant_value) /
                                      coefficient;
                    auto bound_floor =
                        static_cast<T_Variable>(std::floor(bound_temp));
                    if (bound_floor < upper_bound &&
                        abs(bound_floor) < BOUND_LIMIT) {
                        utility::print_message(
                            "The upper bound of the decision variable " +
                                variable_ptr->name() + " was tightened by " +
                                std::to_string(bound_floor) + ".",
                            a_IS_ENABLED_PRINT);
                        variable_ptr->set_bound(lower_bound, bound_floor);
                    }
                }

                else if (constraint.sense() == ConstraintSense::Lower) {
                    auto bound_temp = -(not_fixed_term_lower_bound -
                                        coefficient * upper_bound +
                                        fixed_term_value + constant_value) /
                                      coefficient;
                    auto bound_ceil =
                        static_cast<T_Variable>(std::ceil(bound_temp));
                    if (bound_ceil > lower_bound &&
                        abs(bound_ceil) < BOUND_LIMIT) {
                        utility::print_message(
                            "The lower bound of the decision variable " +
                                variable_ptr->name() + " was tightened by " +
                                std::to_string(bound_ceil) + ".",
                            a_IS_ENABLED_PRINT);
                        variable_ptr->set_bound(bound_ceil, upper_bound);
                    }
                }
            }
        }
    }
    return number_of_newly_disabled_constraints;
}

/*************************************************************************/
template <class T_Variable, class T_Expression>
inline constexpr int fix_implicit_fixed_variables(
    Model<T_Variable, T_Expression> *a_model, const bool a_IS_ENABLED_PRINT) {
    int number_of_newly_fixed_variables = 0;
    for (auto &&proxy : a_model->variable_proxies()) {
        for (auto &&variable : proxy.flat_indexed_variables()) {
            /**
             * If the decision variable has already been fixed, the
             * following procedures will be skipped.
             */
            if (variable.is_fixed()) {
                continue;
            }

            auto lower_bound = variable.lower_bound();
            auto upper_bound = variable.upper_bound();
            if (lower_bound == upper_bound) {
                auto fixed_value = lower_bound;

                utility::print_message(
                    "The value of decision variable " + variable.name() +
                        " was fixed by " + std::to_string(fixed_value) +
                        " because the lower bound " +
                        std::to_string(lower_bound) + " and the upper_bound " +
                        std::to_string(upper_bound) +
                        " implicitly fix the value.",
                    a_IS_ENABLED_PRINT);
                variable.fix_by(fixed_value);
                number_of_newly_fixed_variables++;
            }
        }
    }
    return number_of_newly_fixed_variables;
}
/*****************************************************************************/
template <class T_Variable, class T_Expression>
inline constexpr void presolve(Model<T_Variable, T_Expression> *a_model,
                               const bool a_IS_ENABLED_PRINT) {
    utility::print_single_line(a_IS_ENABLED_PRINT);
    utility::print_message("Presolving...", a_IS_ENABLED_PRINT);

    if (a_model->is_linear()) {
        remove_independent_variables(a_model, a_IS_ENABLED_PRINT);
    }

    while (true) {
        int number_of_newly_disabled_constaints = 0;
        int number_of_newly_fixed_variables     = 0;

        number_of_newly_disabled_constaints  //
            += remove_redundant_constraints_with_tightening_variable_bounds(
                a_model, a_IS_ENABLED_PRINT);

        number_of_newly_fixed_variables  //
            += fix_implicit_fixed_variables(a_model, a_IS_ENABLED_PRINT);

        if (number_of_newly_disabled_constaints == 0 &&
            number_of_newly_fixed_variables == 0) {
            break;
        }
    }
    utility::print_message("Done.", a_IS_ENABLED_PRINT);
}
}  // namespace model
}  // namespace printemps
#endif
/*****************************************************************************/
// END
/*****************************************************************************/