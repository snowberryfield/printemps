/*****************************************************************************/
// Copyright (c) 2020-2023 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_PREPROCESS_PROBLEM_SIZE_REDUCER_H__
#define PRINTEMPS_PREPROCESS_PROBLEM_SIZE_REDUCER_H__

namespace printemps::preprocess {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class ProblemSizeReducer {
   private:
    model::Model<T_Variable, T_Expression> *m_model_ptr;

    int  m_variable_bound_update_count_in_preprocess;
    int  m_variable_bound_update_count_in_optimization;
    bool m_is_preprocess;

   public:
    /*************************************************************************/
    ProblemSizeReducer(void) {
        this->initialize();
    }

    /*************************************************************************/
    ProblemSizeReducer(model::Model<T_Variable, T_Expression> *a_model_ptr) {
        this->setup(a_model_ptr);
    }

    /*************************************************************************/
    inline void setup(model::Model<T_Variable, T_Expression> *a_model_ptr) {
        this->initialize();
        this->m_model_ptr = a_model_ptr;
    }

    /*************************************************************************/
    inline void initialize(void) {
        m_model_ptr                                   = nullptr;
        m_variable_bound_update_count_in_preprocess   = 0;
        m_variable_bound_update_count_in_optimization = 0;
        m_is_preprocess                               = true;
    }

    /*************************************************************************/
    inline constexpr bool reduce_problem_size(const bool a_IS_ENABLED_PRINT) {
        utility::print_single_line(a_IS_ENABLED_PRINT);
        utility::print_message("Reducing problem size...", a_IS_ENABLED_PRINT);

        int number_of_disabled_constraints = 0;
        int number_of_fixed_variables      = 0;

        if (m_model_ptr->is_linear()) {
            number_of_fixed_variables +=
                this->remove_independent_variables(a_IS_ENABLED_PRINT);
        }

        while (true) {
            const int NUMBER_OF_NEWLY_DISABLED_CONSTRAINTS  //
                = this->remove_redundant_constraints_with_tightening_variable_bounds(
                    a_IS_ENABLED_PRINT);

            const int NUMBER_OF_NEWLY_FIXED_VARIABLES  //
                = this->remove_implicit_fixed_variables(a_IS_ENABLED_PRINT);

            number_of_disabled_constraints +=
                NUMBER_OF_NEWLY_DISABLED_CONSTRAINTS;
            number_of_fixed_variables +=  //
                NUMBER_OF_NEWLY_FIXED_VARIABLES;

            if (NUMBER_OF_NEWLY_DISABLED_CONSTRAINTS == 0 &&
                NUMBER_OF_NEWLY_FIXED_VARIABLES == 0) {
                break;
            }
        }

        utility::print_message("Done.", a_IS_ENABLED_PRINT);

        return (number_of_disabled_constraints > 0) ||
               (number_of_fixed_variables > 0);
    }

    /*************************************************************************/
    inline constexpr bool remove_independent_variable(
        model_component::Variable<T_Variable, T_Expression> *a_variable_ptr,  //
        const bool a_IS_MINIMIZATION, const bool a_IS_ENABLED_PRINT) {
        /**
         * NOTE: This function should be called from
         * remove_independent_variables().
         */
        if (a_variable_ptr->related_constraint_ptrs().size() != 0) {
            return false;
        }

        const auto SENSITIVITY = a_variable_ptr->objective_sensitivity();
        if (fabs(SENSITIVITY) < constant::EPSILON_10) {
            utility::print_message(
                "The value of the variable " + a_variable_ptr->name() +
                    " was fixed at " + std::to_string(0) +
                    " because it does not have sensitivity to any "
                    "constraint or objective function.",
                a_IS_ENABLED_PRINT);
            a_variable_ptr->fix_by(0);
            return true;
        }

        if (SENSITIVITY > 0) {
            if (a_IS_MINIMIZATION) {
                const auto FIX_VALUE = a_variable_ptr->lower_bound();
                utility::print_message(
                    "The value of the variable " + a_variable_ptr->name() +
                        " was fixed at its lower bound " +
                        std::to_string(FIX_VALUE) +
                        " because it does not have sensitivity to any "
                        "constraint, and the sensitivity to the objective "
                        "function to be minimized is positive.",
                    a_IS_ENABLED_PRINT);
                a_variable_ptr->fix_by(FIX_VALUE);
                return true;
            } else {
                const auto FIX_VALUE = a_variable_ptr->upper_bound();
                utility::print_message(
                    "The value of the variable " + a_variable_ptr->name() +
                        " was fixed at its upper bound " +
                        std::to_string(FIX_VALUE) +
                        " because it does not have sensitivity to any "
                        "constraint, and the sensitivity to the objective "
                        "function to be maximized is positive.",
                    a_IS_ENABLED_PRINT);
                a_variable_ptr->fix_by(FIX_VALUE);
                return true;
            }
        } else {
            if (a_IS_MINIMIZATION) {
                const auto FIX_VALUE = a_variable_ptr->upper_bound();
                utility::print_message(
                    "The value of the variable " + a_variable_ptr->name() +
                        " was fixed at its upper bound " +
                        std::to_string(FIX_VALUE) +
                        " because it does not have sensitivity to any "
                        "constraint, and the sensitivity to the objective "
                        "function to be minimized is negative.",
                    a_IS_ENABLED_PRINT);
                a_variable_ptr->fix_by(FIX_VALUE);
                return true;
            } else {
                const auto FIX_VALUE = a_variable_ptr->lower_bound();
                utility::print_message(
                    "The value of the variable " + a_variable_ptr->name() +
                        " was fixed at its lower bound " +
                        std::to_string(FIX_VALUE) +
                        " because it does not have sensitivity to any "
                        "constraint, and the sensitivity to the objective "
                        "function to be maximized is positive.",
                    a_IS_ENABLED_PRINT);
                a_variable_ptr->fix_by(FIX_VALUE);
                return true;
            }
        }
        return true;
    }

    /*************************************************************************/
    inline constexpr int remove_independent_variables(
        const bool a_IS_ENABLED_PRINT) {
        int number_of_newly_fixed_variables = 0;
        for (auto &&proxy : m_model_ptr->variable_proxies()) {
            for (auto &&variable : proxy.flat_indexed_variables()) {
                /**
                 * If the variable has already been fixed, the following
                 * procedures will be skipped.
                 */
                if (variable.is_fixed()) {
                    continue;
                }
                if (remove_independent_variable(&variable,
                                                m_model_ptr->is_minimization(),
                                                a_IS_ENABLED_PRINT)) {
                    number_of_newly_fixed_variables++;
                }
            }
        }
        return number_of_newly_fixed_variables;
    }

    /*************************************************************************/
    inline constexpr int
    remove_redundant_constraints_with_tightening_variable_bounds(
        const bool a_IS_ENABLED_PRINT) {
        int number_of_newly_disabled_constraints = 0;
        for (auto &&proxy : m_model_ptr->constraint_proxies()) {
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

    /*************************************************************************/
    inline constexpr bool
    remove_redundant_constraint_with_tightening_variable_bound(
        model_component::Constraint<T_Variable, T_Expression>
            *      a_constraint_ptr,  //
        const bool a_IS_ENABLED_PRINT) {
        const int BOUND_LIMIT = 100000;
        /**
         * NOTE: This function should be called from
         * remove_redundant_constraints_with_tightening_variable_bounds().
         */
        bool is_removed = false;
        int *variable_bound_update_count_ptr =
            m_is_preprocess ? &m_variable_bound_update_count_in_preprocess
                            : &m_variable_bound_update_count_in_optimization;

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
        if ((a_constraint_ptr->sense() ==
                 model_component::ConstraintSense::Equal &&
             constraint_lower_bound == 0 && constraint_upper_bound == 0) ||
            (a_constraint_ptr->sense() ==
                 model_component::ConstraintSense::Less &&
             constraint_upper_bound <= 0) ||
            (a_constraint_ptr->sense() ==
                 model_component::ConstraintSense::Greater &&
             constraint_lower_bound >= 0)) {
            utility::print_message("The redundant constraint " +
                                       a_constraint_ptr->name() +
                                       " was removed.",
                                   a_IS_ENABLED_PRINT);

            if (a_constraint_ptr->is_enabled()) {
                a_constraint_ptr->disable();
                is_removed = true;
                return is_removed;
            }
        }

        /**
         * The detected singleton constraint will be disabled instead of fixing
         * or tightening the lower and upper bounds of the variable included in
         * the constraint.
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
                 * If the singleton constraint is defined by an equality as
                 * ax+b=0, the value of the variable x will be fixed at -b/a.
                 */
                utility::print_message(  //
                    "The constraint " + a_constraint_ptr->name() +
                        " was removed instead of fixing the value of the "
                        "variable " +
                        variable_ptr->name() + " by " +
                        std::to_string(bound_temp) + ".",
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
                 * ax+b<=0 with a>0 (or ax+b>=0 with a<0), the lower bound of
                 * the variable will be tightened by floor(-b/a).
                 */
                auto bound_floor =
                    static_cast<T_Variable>(std::floor(bound_temp));

                if (bound_floor < variable_upper_bound &&
                    abs(bound_floor) < BOUND_LIMIT) {
                    if (a_constraint_ptr->name() == "") {
                        utility::print_message(
                            "The upper bound of the variable " +
                                variable_ptr->name() + " was tightened by " +
                                std::to_string(bound_floor) + ".",
                            a_IS_ENABLED_PRINT);
                    } else {
                        utility::print_message(
                            "The constraint " + a_constraint_ptr->name() +
                                " was removed instead of tightening the upper "
                                "bound of the variable " +
                                variable_ptr->name() + " by " +
                                std::to_string(bound_floor) + ".",
                            a_IS_ENABLED_PRINT);
                    }
                    variable_ptr->set_bound(variable_lower_bound, bound_floor);
                    (*variable_bound_update_count_ptr)++;
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
                 * ax+b>=0 with a>0 (or ax+b<=0 with a<0), the upper bound of
                 * the variable will be tightened by ceil(-b/a).
                 */
                auto bound_ceil =
                    static_cast<T_Variable>(std::ceil(bound_temp));

                if (bound_ceil > variable_lower_bound &&
                    abs(bound_ceil) < BOUND_LIMIT) {
                    if (a_constraint_ptr->name() == "") {
                        utility::print_message(  //
                            "The lower bound of the variable " +
                                variable_ptr->name() + " was tightened by " +
                                std::to_string(bound_ceil) + ".",
                            a_IS_ENABLED_PRINT);
                    } else {
                        utility::print_message(
                            "The constraint " + a_constraint_ptr->name() +
                                " was removed instead of tightening the lower "
                                "bound of the variable " +
                                variable_ptr->name() + " by " +
                                std::to_string(bound_ceil) + ".",
                            a_IS_ENABLED_PRINT);
                    }
                    variable_ptr->set_bound(bound_ceil, variable_upper_bound);
                    (*variable_bound_update_count_ptr)++;
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
         * Tighten the lower and upper bounds of the variables based on the
         * bounds of the rest part.
         */
        for (auto &&sensitivity : positive_mutable_variable_sensitivities) {
            auto variable_ptr = sensitivity.first;
            auto coefficient  = sensitivity.second;

            auto variable_lower_bound = variable_ptr->lower_bound();
            auto variable_upper_bound = variable_ptr->upper_bound();

            if (a_constraint_ptr->is_greater_or_equal()) {
                auto bound_temp = -(constraint_upper_bound -
                                    coefficient * variable_upper_bound) /
                                  coefficient;
                auto bound_ceil =
                    static_cast<T_Variable>(std::ceil(bound_temp));
                if (bound_ceil > variable_lower_bound &&
                    abs(bound_ceil) < BOUND_LIMIT) {
                    utility::print_message(  //
                        "The lower bound of the variable " +
                            variable_ptr->name() + " was tightened by " +
                            std::to_string(bound_ceil) + ".",
                        a_IS_ENABLED_PRINT);
                    variable_ptr->set_bound(bound_ceil, variable_upper_bound);
                    (*variable_bound_update_count_ptr)++;
                }
            }
            if (a_constraint_ptr->is_less_or_equal()) {
                auto bound_temp = -(constraint_lower_bound -
                                    coefficient * variable_lower_bound) /
                                  coefficient;
                auto bound_floor =
                    static_cast<T_Variable>(std::floor(bound_temp));
                if (bound_floor < variable_upper_bound &&
                    abs(bound_floor) < BOUND_LIMIT) {
                    utility::print_message(  //
                        "The upper bound of the variable " +
                            variable_ptr->name() + " was tightened by " +
                            std::to_string(bound_floor) + ".",
                        a_IS_ENABLED_PRINT);
                    variable_ptr->set_bound(variable_lower_bound, bound_floor);
                    (*variable_bound_update_count_ptr)++;
                }
            }
        }

        for (auto &&sensitivity : negative_mutable_variable_sensitivities) {
            auto variable_ptr = sensitivity.first;
            auto coefficient  = sensitivity.second;

            auto variable_lower_bound = variable_ptr->lower_bound();
            auto variable_upper_bound = variable_ptr->upper_bound();

            if (a_constraint_ptr->is_greater_or_equal()) {
                auto bound_temp = -(constraint_upper_bound -
                                    coefficient * variable_lower_bound) /
                                  coefficient;
                auto bound_floor =
                    static_cast<T_Variable>(std::floor(bound_temp));
                if (bound_floor < variable_upper_bound &&
                    abs(bound_floor) < BOUND_LIMIT) {
                    utility::print_message(  //
                        "The upper bound of the variable " +
                            variable_ptr->name() + " was tightened by " +
                            std::to_string(bound_floor) + ".",
                        a_IS_ENABLED_PRINT);
                    variable_ptr->set_bound(variable_lower_bound, bound_floor);
                    (*variable_bound_update_count_ptr)++;
                }
            }
            if (a_constraint_ptr->is_less_or_equal()) {
                auto bound_temp = -(constraint_lower_bound -
                                    coefficient * variable_upper_bound) /
                                  coefficient;
                auto bound_ceil =
                    static_cast<T_Variable>(std::ceil(bound_temp));
                if (bound_ceil > variable_lower_bound &&
                    abs(bound_ceil) < BOUND_LIMIT) {
                    utility::print_message(  //
                        "The lower bound of the variable " +
                            variable_ptr->name() + " was tightened by " +
                            std::to_string(bound_ceil) + ".",
                        a_IS_ENABLED_PRINT);
                    variable_ptr->set_bound(bound_ceil, variable_upper_bound);
                    (*variable_bound_update_count_ptr)++;
                }
            }
        }
        return is_removed;
    }

    /*************************************************************************/
    inline constexpr int remove_implicit_fixed_variables(
        const bool a_IS_ENABLED_PRINT) {
        int number_of_newly_fixed_variables = 0;
        for (auto &&proxy : m_model_ptr->variable_proxies()) {
            for (auto &&variable : proxy.flat_indexed_variables()) {
                /**
                 * If the variable has already been fixed, the following
                 * procedures will be skipped.
                 */
                if (variable.is_fixed()) {
                    continue;
                }

                if (remove_implicit_fixed_variable(&variable,
                                                   a_IS_ENABLED_PRINT)) {
                    number_of_newly_fixed_variables++;
                }
            }
        }
        return number_of_newly_fixed_variables;
    }

    /*************************************************************************/
    inline constexpr bool remove_implicit_fixed_variable(
        model_component::Variable<T_Variable, T_Expression> *a_variable_ptr,  //
        const bool a_IS_ENABLED_PRINT) {
        /**
         * NOTE: This function should be called from
         * remove_implicit_fixed_variables().
         */

        const auto LOWER_BOUND = a_variable_ptr->lower_bound();
        const auto UPPER_BOUND = a_variable_ptr->upper_bound();
        if (LOWER_BOUND != UPPER_BOUND) {
            return false;
        }

        const auto FIXED_VALUE = LOWER_BOUND;
        utility::print_message(
            "The value of the variable " + a_variable_ptr->name() +
                " was fixed at " + std::to_string(FIXED_VALUE) +
                " because the lower bound " + std::to_string(LOWER_BOUND) +
                " and the upper_bound " + std::to_string(UPPER_BOUND) +
                " implicitly fix the value.",
            a_IS_ENABLED_PRINT);
        a_variable_ptr->fix_by(FIXED_VALUE);
        return true;
    }

    /*************************************************************************/
    inline constexpr int remove_redundant_set_variables(
        const bool a_IS_ENABLED_PRINT) {
        utility::print_single_line(a_IS_ENABLED_PRINT);
        utility::print_message(
            "Removing redundant variables which compose set "
            "paritioning/covering/packing constraints...",
            a_IS_ENABLED_PRINT);

        const int SET_PARTITIONINGS_SIZE =
            m_model_ptr->constraint_type_reference()
                .set_partitioning_ptrs.size();
        const int SET_COVERINGS_SIZE =
            m_model_ptr->constraint_type_reference().set_covering_ptrs.size();
        const int SET_PACKINGS_SIZE =
            m_model_ptr->constraint_type_reference().set_packing_ptrs.size();

        int number_of_newly_fixed_variables = 0;

        /**
         * If the problem is unconstrained, the following procedures will be
         * skipped.
         */
        if (m_model_ptr->number_of_constraints() == 0) {
            return number_of_newly_fixed_variables;
        }

        /**
         * If the problem is not pure set partitioning/covering/packing problem,
         * the following procedures will be skipped.
         */
        if (m_model_ptr->number_of_constraints() !=
            (SET_PARTITIONINGS_SIZE + SET_COVERINGS_SIZE + SET_PACKINGS_SIZE)) {
            return number_of_newly_fixed_variables;
        }

        auto variable_ptrs = m_model_ptr->variable_reference().variable_ptrs;
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
            std::sort(variable_ptrs.begin(), variable_ptrs.end(),
                      [](const auto &a_LHS, const auto &a_RHS) {
                          return (a_LHS->hash() == a_RHS->hash())
                                     ? (a_LHS->objective_sensitivity() <
                                        a_RHS->objective_sensitivity())
                                     : (a_LHS->hash() < a_RHS->hash());
                      });
        } else {
            std::sort(variable_ptrs.begin(), variable_ptrs.end(),
                      [](const auto &a_LHS, const auto &a_RHS) {
                          return (a_LHS->hash() == a_RHS->hash())
                                     ? (a_LHS->objective_sensitivity() >
                                        a_RHS->objective_sensitivity())
                                     : (a_LHS->hash() < a_RHS->hash());
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
                    number_of_newly_fixed_variables++;
                }
                j++;
            }
            if (j == VARIABLES_SIZE) {
                break;
            }
        }
        utility::print_message("Done.", a_IS_ENABLED_PRINT);
        return number_of_newly_fixed_variables;
    }

    /*************************************************************************/
    inline constexpr int extract_implicit_equality_constraints(
        const bool a_IS_ENABLED_PRINT) {
        utility::print_single_line(a_IS_ENABLED_PRINT);
        utility::print_message("Removing implicit equality constraints...",
                               a_IS_ENABLED_PRINT);

        auto &reference = m_model_ptr->constraint_type_reference();
        int   number_of_newly_disabled_constraints = 0;
        std::vector<model_component::Constraint<T_Variable, T_Expression> *>
            constraint_ptrs;

        constraint_ptrs.insert(constraint_ptrs.end(),              //
                               reference.precedence_ptrs.begin(),  //
                               reference.precedence_ptrs.end());

        constraint_ptrs.insert(constraint_ptrs.end(),                  //
                               reference.variable_bound_ptrs.begin(),  //
                               reference.variable_bound_ptrs.end());

        constraint_ptrs.insert(constraint_ptrs.end(),               //
                               reference.set_packing_ptrs.begin(),  //
                               reference.set_packing_ptrs.end());

        constraint_ptrs.insert(constraint_ptrs.end(),                //
                               reference.set_covering_ptrs.begin(),  //
                               reference.set_covering_ptrs.end());

        constraint_ptrs.insert(constraint_ptrs.end(),                      //
                               reference.invariant_knapsack_ptrs.begin(),  //
                               reference.invariant_knapsack_ptrs.end());

        constraint_ptrs.insert(constraint_ptrs.end(),                     //
                               reference.multiple_covering_ptrs.begin(),  //
                               reference.multiple_covering_ptrs.end());

        constraint_ptrs.insert(constraint_ptrs.end(),           //
                               reference.min_max_ptrs.begin(),  //
                               reference.min_max_ptrs.end());

        constraint_ptrs.insert(constraint_ptrs.end(),           //
                               reference.max_min_ptrs.begin(),  //
                               reference.max_min_ptrs.end());

        constraint_ptrs.insert(constraint_ptrs.end(),               //
                               reference.bin_packing_ptrs.begin(),  //
                               reference.bin_packing_ptrs.end());

        constraint_ptrs.insert(constraint_ptrs.end(),            //
                               reference.knapsack_ptrs.begin(),  //
                               reference.knapsack_ptrs.end());

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
            return number_of_newly_disabled_constraints;
        }

        for (auto constraint_ptr : constraint_ptrs) {
            constraint_ptr->expression().setup_hash();
        }

        const int CONSTRAINTS_SIZE = constraint_ptrs.size();

        std::sort(constraint_ptrs.begin(), constraint_ptrs.end(),
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

                    number_of_newly_disabled_constraints++;
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

                    number_of_newly_disabled_constraints++;
                    i++;
                    break;
                }
                j++;
            }
            if (j == CONSTRAINTS_SIZE) {
                break;
            }
        }
        if (number_of_newly_disabled_constraints > 0) {
            auto &additional_constraint_proxy = m_model_ptr->create_constraints(
                "additional", number_of_newly_disabled_constraints);
            for (auto i = 0; i < number_of_newly_disabled_constraints; i++) {
                additional_constraint_proxy(i) = additional_constraints[i];
                additional_constraint_proxy(i).set_name(
                    additional_constraints[i].name());
            }
        }

        utility::print_message("Done.", a_IS_ENABLED_PRINT);
        return number_of_newly_disabled_constraints;
    }

    /*************************************************************************/
    inline constexpr int remove_duplicated_constraints(
        const bool a_IS_ENABLED_PRINT) {
        utility::print_single_line(a_IS_ENABLED_PRINT);
        utility::print_message("Removing duplicated constraints...",
                               a_IS_ENABLED_PRINT);

        auto &reference = m_model_ptr->constraint_type_reference();
        int   number_of_newly_disabled_constraints = 0;

        number_of_newly_disabled_constraints +=
            this->remove_duplicated_constraints(  //
                reference.singleton_ptrs, a_IS_ENABLED_PRINT);

        number_of_newly_disabled_constraints +=
            this->remove_duplicated_constraints(  //
                reference.exclusive_or_ptrs, a_IS_ENABLED_PRINT);

        number_of_newly_disabled_constraints +=
            this->remove_duplicated_constraints(  //
                reference.exclusive_nor_ptrs, a_IS_ENABLED_PRINT);

        number_of_newly_disabled_constraints +=
            this->remove_duplicated_constraints(  //
                reference.inverted_integers_ptrs, a_IS_ENABLED_PRINT);

        number_of_newly_disabled_constraints +=
            this->remove_duplicated_constraints(  //
                reference.balanced_integers_ptrs, a_IS_ENABLED_PRINT);

        number_of_newly_disabled_constraints +=
            this->remove_duplicated_constraints(  //
                reference.constant_sum_integers_ptrs, a_IS_ENABLED_PRINT);

        number_of_newly_disabled_constraints +=
            this->remove_duplicated_constraints(  //
                reference.constant_difference_integers_ptrs,
                a_IS_ENABLED_PRINT);

        number_of_newly_disabled_constraints +=
            this->remove_duplicated_constraints(  //
                reference.constant_ratio_integers_ptrs, a_IS_ENABLED_PRINT);

        number_of_newly_disabled_constraints +=
            this->remove_duplicated_constraints(  //
                reference.aggregation_ptrs, a_IS_ENABLED_PRINT);

        number_of_newly_disabled_constraints +=
            this->remove_duplicated_constraints(  //
                reference.precedence_ptrs, a_IS_ENABLED_PRINT);

        number_of_newly_disabled_constraints +=
            this->remove_duplicated_constraints(  //
                reference.variable_bound_ptrs, a_IS_ENABLED_PRINT);

        number_of_newly_disabled_constraints +=
            this->remove_duplicated_constraints(  //
                reference.trinomial_exclusive_nor_ptrs, a_IS_ENABLED_PRINT);

        number_of_newly_disabled_constraints +=
            this->remove_duplicated_constraints(  //
                reference.set_partitioning_ptrs, a_IS_ENABLED_PRINT);

        number_of_newly_disabled_constraints +=
            this->remove_duplicated_constraints(  //
                reference.set_packing_ptrs, a_IS_ENABLED_PRINT);

        number_of_newly_disabled_constraints +=
            this->remove_duplicated_constraints(  //
                reference.set_covering_ptrs, a_IS_ENABLED_PRINT);

        number_of_newly_disabled_constraints +=
            this->remove_duplicated_constraints(  //
                reference.cardinality_ptrs, a_IS_ENABLED_PRINT);

        number_of_newly_disabled_constraints +=
            this->remove_duplicated_constraints(  //
                reference.invariant_knapsack_ptrs, a_IS_ENABLED_PRINT);

        number_of_newly_disabled_constraints +=
            this->remove_duplicated_constraints(  //
                reference.multiple_covering_ptrs, a_IS_ENABLED_PRINT);

        number_of_newly_disabled_constraints +=
            this->remove_duplicated_constraints(  //
                reference.binary_flow_ptrs, a_IS_ENABLED_PRINT);

        number_of_newly_disabled_constraints +=
            this->remove_duplicated_constraints(  //
                reference.integer_flow_ptrs, a_IS_ENABLED_PRINT);

        number_of_newly_disabled_constraints +=
            this->remove_duplicated_constraints(  //
                reference.soft_selection_ptrs, a_IS_ENABLED_PRINT);

        number_of_newly_disabled_constraints +=
            this->remove_duplicated_constraints(  //
                reference.min_max_ptrs, a_IS_ENABLED_PRINT);

        number_of_newly_disabled_constraints +=
            this->remove_duplicated_constraints(  //
                reference.max_min_ptrs, a_IS_ENABLED_PRINT);

        number_of_newly_disabled_constraints +=
            this->remove_duplicated_constraints(  //
                reference.intermediate_ptrs, a_IS_ENABLED_PRINT);

        number_of_newly_disabled_constraints +=
            this->remove_duplicated_constraints(  //
                reference.equation_knapsack_ptrs, a_IS_ENABLED_PRINT);

        number_of_newly_disabled_constraints +=
            this->remove_duplicated_constraints(  //
                reference.bin_packing_ptrs, a_IS_ENABLED_PRINT);

        number_of_newly_disabled_constraints +=
            this->remove_duplicated_constraints(  //
                reference.integer_knapsack_ptrs, a_IS_ENABLED_PRINT);

        number_of_newly_disabled_constraints +=
            this->remove_duplicated_constraints(  //
                reference.gf2_ptrs, a_IS_ENABLED_PRINT);

        number_of_newly_disabled_constraints +=
            this->remove_duplicated_constraints(  //
                reference.general_linear_ptrs, a_IS_ENABLED_PRINT);

        utility::print_message("Done.", a_IS_ENABLED_PRINT);
        return number_of_newly_disabled_constraints;
    }

    /*************************************************************************/
    inline constexpr int remove_duplicated_constraints(
        const std::vector<model_component::Constraint<T_Variable, T_Expression>
                              *> &a_CONSTRAINT_PTRS,  //
        const bool                a_IS_ENABLED_PRINT) {
        /**
         * NOTE: This function should be called from
         * remove_duplicated_constraints() which refers the pointer of the
         * model.
         */
        int number_of_newly_disabled_constraints = 0;

        auto constraint_ptrs = a_CONSTRAINT_PTRS;
        if (constraint_ptrs.size() <= 1) {
            return number_of_newly_disabled_constraints;
        }

        for (auto constraint_ptr : constraint_ptrs) {
            constraint_ptr->expression().setup_hash();
        }

        const int CONSTRAINTS_SIZE = constraint_ptrs.size();

        std::sort(constraint_ptrs.begin(), constraint_ptrs.end(),
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
                 * If the senses of constraint i and j are different, the
                 * following procedure can be skipped.
                 */
                if (constraint_ptrs[i]->sense() !=
                    constraint_ptrs[j]->sense()) {
                    j++;
                    continue;
                }

                /**
                 * If the variable coefficient pattern of the constraints i and
                 * j is same, disable constraint j.
                 */
                if (constraint_ptrs[i]->expression().equal(
                        constraint_ptrs[j]->expression())) {
                    constraint_ptrs[j]->disable();
                    utility::print_message(  //
                        "The duplicated constraint " +
                            constraint_ptrs[j]->name() + " was removed.",
                        a_IS_ENABLED_PRINT);
                    number_of_newly_disabled_constraints++;
                }
                j++;
            }
            if (j == CONSTRAINTS_SIZE) {
                break;
            }
        }
        return number_of_newly_disabled_constraints;
    }

    /*************************************************************************/
    inline constexpr std::pair<int, int> remove_redundant_set_constraints(
        const bool a_IS_ENABLED_PRINT) {
        utility::print_single_line(a_IS_ENABLED_PRINT);
        utility::print_message(
            "Removing redundant exclusive OR and set partitioning constraints "
            "and included variables... ",
            a_IS_ENABLED_PRINT);

        auto &exclusive_or_ptrs =
            m_model_ptr->constraint_type_reference().exclusive_or_ptrs;
        auto &set_partitioning_constraint_ptrs =
            m_model_ptr->constraint_type_reference().set_partitioning_ptrs;
        auto constraint_ptrs = exclusive_or_ptrs;
        constraint_ptrs.insert(constraint_ptrs.end(),
                               set_partitioning_constraint_ptrs.begin(),
                               set_partitioning_constraint_ptrs.end());

        int number_of_newly_disabled_constraints = 0;
        int number_of_newly_fixed_variables      = 0;

        const int CONSTRAINTS_SIZE = constraint_ptrs.size();

        std::sort(constraint_ptrs.begin(), constraint_ptrs.end(),
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

    /*************************************************************************/
    inline constexpr int variable_bound_update_count_in_preprocess(void) const {
        return m_variable_bound_update_count_in_preprocess;
    }

    /*************************************************************************/
    inline constexpr int variable_bound_update_count_in_optimization(
        void) const {
        return m_variable_bound_update_count_in_optimization;
    }

    /*************************************************************************/
    inline constexpr void set_is_preprocess(const bool a_IS_PREPROCESS) {
        m_is_preprocess = a_IS_PREPROCESS;
    }

    /*************************************************************************/
    inline constexpr bool is_preprocess(void) const {
        return m_is_preprocess;
    }
};
}  // namespace printemps::preprocess
#endif
/*****************************************************************************/
// END
/*****************************************************************************/
