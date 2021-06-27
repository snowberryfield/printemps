/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_PRESOLVER_CONSTRAINT_REMOVER_H__
#define PRINTEMPS_PRESOLVER_CONSTRAINT_REMOVER_H__

namespace printemps {
namespace presolver {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
constexpr bool remove_redundant_constraints_with_tightening_variable_bound(
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
             * the value of the decision variable x will be fixed by -b/a.
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
                utility::print_message(
                    "The constraint " + a_constraint_ptr->name() +
                        " was removed instead of tightening the upper "
                        "bound of the decision variable " +
                        variable_ptr->name() + " by " +
                        std::to_string(bound_floor) + ".",
                    a_IS_ENABLED_PRINT);
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
                utility::print_message(
                    "The constraint " + a_constraint_ptr->name() +
                        " was removed instead of tightening the lower "
                        "bound of the decision variable " +
                        variable_ptr->name() + " by " +
                        std::to_string(bound_ceil) + ".",
                    a_IS_ENABLED_PRINT);
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

            if (remove_redundant_constraints_with_tightening_variable_bound(
                    &constraint,  //
                    a_IS_ENABLED_PRINT)) {
                number_of_newly_disabled_constraints++;
            }
        }
    }
    return number_of_newly_disabled_constraints;
}

}  // namespace presolver
}  // namespace printemps
#endif
/*****************************************************************************/
// END
/*****************************************************************************/