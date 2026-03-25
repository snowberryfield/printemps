/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_PREPROCESS_PROBLEM_SIZE_REDUCER_BASIC_H__
#define PRINTEMPS_PREPROCESS_PROBLEM_SIZE_REDUCER_BASIC_H__

namespace printemps::preprocess {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class ProblemSizeReducerBasic {
   private:
    model::Model<T_Variable, T_Expression> *m_model_ptr;

    int  m_variable_bound_update_count_in_preprocess;
    int  m_variable_bound_update_count_in_optimization;
    bool m_is_preprocess;

   public:
    /*************************************************************************/
    ProblemSizeReducerBasic(void) {
        this->initialize();
    }

    /*************************************************************************/
    ProblemSizeReducerBasic(
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
        m_model_ptr                                   = nullptr;
        m_variable_bound_update_count_in_preprocess   = 0;
        m_variable_bound_update_count_in_optimization = 0;
        m_is_preprocess                               = true;
    }

    /*************************************************************************/
    inline ProblemSizeReducerResult run(const bool a_IS_ENABLED_PRINT) {
        utility::print_single_line(a_IS_ENABLED_PRINT);
        utility::print_message("Reducing problem size...", a_IS_ENABLED_PRINT);

        ProblemSizeReducerResult result;
        ProblemSizeReducerResult inner_result;

        while (true) {
            inner_result.reset();

            inner_result +=
                this->remove_insensitive_variables(a_IS_ENABLED_PRINT);

            inner_result +=
                this->remove_inactive_binary_variables(a_IS_ENABLED_PRINT);

            inner_result +=
                this->remove_redundant_constraints_with_tightening_variable_bounds(
                    a_IS_ENABLED_PRINT);

            inner_result +=
                this->remove_implicit_fixed_variables(a_IS_ENABLED_PRINT);

            result += inner_result;

            if (!inner_result.is_reduced()) {
                break;
            }

            m_model_ptr->builder().update_derived_components();
        }

        utility::print_message("Done.", a_IS_ENABLED_PRINT);
        return result;
    }

    /*************************************************************************/
    inline ProblemSizeReducerResult remove_insensitive_variables(
        const bool a_IS_ENABLED_PRINT) {
        ProblemSizeReducerResult result;
        for (auto &&proxy : m_model_ptr->variable_proxies()) {
            for (auto &&variable : proxy.flat_indexed_variables()) {
                if (variable.is_fixed()) {
                    continue;
                }

                result += this->remove_insensitive_variable(
                    &variable, m_model_ptr->is_minimization(),
                    a_IS_ENABLED_PRINT);
            }
        }
        return result;
    }

    /*************************************************************************/
    inline ProblemSizeReducerResult remove_insensitive_variable(
        model_component::Variable<T_Variable, T_Expression> *a_variable_ptr,  //
        const bool a_IS_MINIMIZATION, const bool a_IS_ENABLED_PRINT) {
        /**
         * NOTE: This function should be called from
         * remove_insensitive_variables().
         */
        if (a_variable_ptr->related_constraint_ptrs().size() != 0) {
            return ProblemSizeReducerResult(0, 0, 0);
        }

        /**
         * NOTE: Dependent variables cannot be fixed here because their values
         * are computed by other dependent expressions.
         */
        if (a_variable_ptr->type() ==
                model_component::VariableType::DependentBinary ||
            a_variable_ptr->type() ==
                model_component::VariableType::DependentInteger) {
            return ProblemSizeReducerResult(0, 0, 0);
        }

        const auto SENSITIVITY = a_variable_ptr->objective_sensitivity();
        if (std::fabs(SENSITIVITY) < constant::EPSILON_10) {
            utility::print_message(
                "The value of the variable " + a_variable_ptr->name() +
                    " was fixed at " + std::to_string(0) +
                    " because it does not have sensitivity to any "
                    "constraint or objective function.",
                a_IS_ENABLED_PRINT);
            a_variable_ptr->fix_by(0);
            return ProblemSizeReducerResult(1, 0, 0);
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
                return ProblemSizeReducerResult(1, 0, 0);
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
                return ProblemSizeReducerResult(1, 0, 0);
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
                return ProblemSizeReducerResult(1, 0, 0);
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
                return ProblemSizeReducerResult(1, 0, 0);
            }
        }
        return ProblemSizeReducerResult(0, 0, 0);
    }

    /*************************************************************************/
    inline ProblemSizeReducerResult remove_inactive_binary_variables(
        const bool a_IS_ENABLED_PRINT) {
        ProblemSizeReducerResult result;
        for (auto &&proxy : m_model_ptr->variable_proxies()) {
            for (auto &&variable : proxy.flat_indexed_variables()) {
                /**
                 * If the variable has already been fixed, the following
                 * procedures will be skipped.
                 */
                if (variable.is_fixed()) {
                    continue;
                }
                result += this->remove_inactive_binary_variable(
                    &variable, m_model_ptr->is_minimization(),
                    a_IS_ENABLED_PRINT);
            }
        }
        return result;
    }

    /*************************************************************************/
    inline ProblemSizeReducerResult remove_inactive_binary_variable(
        model_component::Variable<T_Variable, T_Expression> *a_variable_ptr,  //
        const bool a_IS_MINIMIZATION, const bool a_IS_ENABLED_PRINT) {
        /**
         * NOTE: This function should be called from
         * remove_inactive_binary_variables().
         */
        if (a_variable_ptr->type() != model_component::VariableType::Binary) {
            return ProblemSizeReducerResult(0, 0, 0);
        }

        std::vector<model_component::Constraint<T_Variable, T_Expression> *>
            related_constraint_ptrs_positive_less;
        std::vector<model_component::Constraint<T_Variable, T_Expression> *>
            related_constraint_ptrs_not_positive_less;
        std::vector<model_component::Constraint<T_Variable, T_Expression> *>
            related_constraint_ptrs_positive_greater;
        std::vector<model_component::Constraint<T_Variable, T_Expression> *>
            related_constraint_ptrs_not_positive_greater;
        std::vector<model_component::Constraint<T_Variable, T_Expression> *>
            related_constraint_ptrs_equal;

        for (auto &&constraint_ptr :
             a_variable_ptr->related_constraint_ptrs()) {
            if (!constraint_ptr->is_enabled()) {
                continue;
            }

            const auto SENSITIVITY =
                constraint_ptr->expression().sensitivities()[a_variable_ptr];
            const auto SENSE = constraint_ptr->sense();

            if (SENSE == model_component::ConstraintSense::Less) {
                if (SENSITIVITY > 0) {
                    related_constraint_ptrs_positive_less.push_back(
                        constraint_ptr);
                } else {
                    related_constraint_ptrs_not_positive_less.push_back(
                        constraint_ptr);
                }
            } else if (SENSE == model_component::ConstraintSense::Greater) {
                if (SENSITIVITY > 0) {
                    related_constraint_ptrs_positive_greater.push_back(
                        constraint_ptr);
                } else {
                    related_constraint_ptrs_not_positive_greater.push_back(
                        constraint_ptr);
                }
            } else {
                related_constraint_ptrs_equal.push_back(constraint_ptr);
            }
        }

        if (!related_constraint_ptrs_equal.empty()) {
            return ProblemSizeReducerResult(0, 0, 0);
        }

        const auto OBJECTIVE_SENSITIVITY =
            a_variable_ptr->objective_sensitivity();

        if ((OBJECTIVE_SENSITIVITY > 0 && a_IS_MINIMIZATION) ||
            (OBJECTIVE_SENSITIVITY < 0 && !a_IS_MINIMIZATION) ||
            std::fabs(OBJECTIVE_SENSITIVITY) < constant::EPSILON_10) {
            if (related_constraint_ptrs_not_positive_less.empty() &&
                related_constraint_ptrs_positive_greater.empty()) {
                utility::print_message(
                    "The value of the variable " + a_variable_ptr->name() +
                        " was fixed at " + std::to_string(0) +
                        ". This fixing does not eliminate all optimal "
                        "solutions, if any exist.",
                    a_IS_ENABLED_PRINT);
                a_variable_ptr->fix_by(0);
                return ProblemSizeReducerResult(1, 0, 0);
            }
        }

        if ((OBJECTIVE_SENSITIVITY > 0 && !a_IS_MINIMIZATION) ||
            (OBJECTIVE_SENSITIVITY < 0 && a_IS_MINIMIZATION) ||
            std::fabs(OBJECTIVE_SENSITIVITY) < constant::EPSILON_10) {
            if (related_constraint_ptrs_positive_less.empty() &&
                related_constraint_ptrs_not_positive_greater.empty()) {
                a_variable_ptr->fix_by(1);
                utility::print_message(
                    "The value of the variable " + a_variable_ptr->name() +
                        " was fixed at " + std::to_string(0) +
                        ". This fixing does not eliminate all optimal "
                        "solutions, if any exist.",
                    a_IS_ENABLED_PRINT);
                return ProblemSizeReducerResult(1, 0, 0);
            }
        }
        return ProblemSizeReducerResult(0, 0, 0);
    }

    /*************************************************************************/
    inline ProblemSizeReducerResult
    remove_redundant_constraints_with_tightening_variable_bounds(
        const bool a_IS_ENABLED_PRINT) {
        ProblemSizeReducerResult result;
        for (auto &&proxy : m_model_ptr->constraint_proxies()) {
            for (auto &&constraint : proxy.flat_indexed_constraints()) {
                if (!constraint.is_enabled()) {
                    continue;
                }

                result +=
                    this->remove_redundant_constraint_with_tightening_variable_bound(
                        &constraint, a_IS_ENABLED_PRINT);
            }
        }
        return result;
    }

    /*************************************************************************/
    inline ProblemSizeReducerResult
    remove_redundant_constraint_with_tightening_variable_bound(
        model_component::Constraint<T_Variable, T_Expression>
                  *a_constraint_ptr,  //
        const bool a_IS_ENABLED_PRINT) {
        const int BOUND_LIMIT = 100000;
        /**
         * NOTE: This function should be called from
         * remove_redundant_constraints_with_tightening_variable_bounds().
         */
        bool is_constraint_disabled    = false;
        bool is_variable_bound_updated = false;

        int *variable_bound_update_count_ptr =
            m_is_preprocess ? &m_variable_bound_update_count_in_preprocess
                            : &m_variable_bound_update_count_in_optimization;

        a_constraint_ptr->expression().setup_mutable_variable_sensitivities();

        auto &mutable_variable_sensitivities =
            a_constraint_ptr->expression().mutable_variable_sensitivities();

        auto &positive_coefficient_mutable_variable_sensitivities =
            a_constraint_ptr->expression()
                .positive_coefficient_mutable_variable_sensitivities();

        auto &negative_coefficient_mutable_variable_sensitivities =
            a_constraint_ptr->expression()
                .negative_coefficient_mutable_variable_sensitivities();

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
                is_constraint_disabled = true;
                return ProblemSizeReducerResult(0, is_constraint_disabled,
                                                is_variable_bound_updated);
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
                    is_constraint_disabled = true;
                    return ProblemSizeReducerResult(0, is_constraint_disabled,
                                                    is_variable_bound_updated);
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
                    is_variable_bound_updated = true;
                    (*variable_bound_update_count_ptr)++;
                } else {
                    utility::print_message(  //
                        "The redundant constraint " + a_constraint_ptr->name() +
                            " was removed.",
                        a_IS_ENABLED_PRINT);
                }
                if (a_constraint_ptr->is_enabled()) {
                    a_constraint_ptr->disable();
                    is_constraint_disabled = true;
                    return ProblemSizeReducerResult(0, is_constraint_disabled,
                                                    is_variable_bound_updated);
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
                    is_variable_bound_updated = true;
                    (*variable_bound_update_count_ptr)++;
                } else {
                    utility::print_message(  //
                        "The redundant constraint " + a_constraint_ptr->name() +
                            " was removed.",
                        a_IS_ENABLED_PRINT);
                }
                if (a_constraint_ptr->is_enabled()) {
                    a_constraint_ptr->disable();
                    is_constraint_disabled = true;
                    return ProblemSizeReducerResult(0, is_constraint_disabled,
                                                    is_variable_bound_updated);
                }
            }
            return ProblemSizeReducerResult(0, is_constraint_disabled,
                                            is_variable_bound_updated);
        }

        /**
         * Tighten the lower and upper bounds of the variables based on the
         * bounds of the rest part.
         */
        for (auto &&sensitivity :
             positive_coefficient_mutable_variable_sensitivities) {
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
                    is_variable_bound_updated = true;
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
                    is_variable_bound_updated = true;
                    (*variable_bound_update_count_ptr)++;
                }
            }
        }

        for (auto &&sensitivity :
             negative_coefficient_mutable_variable_sensitivities) {
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
                    is_variable_bound_updated = true;
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
                    is_variable_bound_updated = true;
                    (*variable_bound_update_count_ptr)++;
                }
            }
        }
        return ProblemSizeReducerResult(0, is_constraint_disabled,
                                        is_variable_bound_updated);
    }

    /*************************************************************************/
    inline ProblemSizeReducerResult remove_implicit_fixed_variables(
        const bool a_IS_ENABLED_PRINT) {
        ProblemSizeReducerResult result;
        for (auto &&proxy : m_model_ptr->variable_proxies()) {
            for (auto &&variable : proxy.flat_indexed_variables()) {
                if (variable.is_fixed()) {
                    continue;
                }

                result += this->remove_implicit_fixed_variable(
                    &variable, a_IS_ENABLED_PRINT);
            }
        }
        return result;
    }

    /*************************************************************************/
    inline ProblemSizeReducerResult remove_implicit_fixed_variable(
        model_component::Variable<T_Variable, T_Expression> *a_variable_ptr,  //
        const bool a_IS_ENABLED_PRINT) {
        /**
         * NOTE: This function should be called from
         * remove_implicit_fixed_variables().
         */
        const auto LOWER_BOUND = a_variable_ptr->lower_bound();
        const auto UPPER_BOUND = a_variable_ptr->upper_bound();
        if (LOWER_BOUND != UPPER_BOUND) {
            return ProblemSizeReducerResult(0, 0, 0);
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
        return ProblemSizeReducerResult(1, 0, 0);
    }

    /*************************************************************************/
    inline int variable_bound_update_count_in_preprocess(void) const {
        return m_variable_bound_update_count_in_preprocess;
    }

    /*************************************************************************/
    inline int variable_bound_update_count_in_optimization(void) const {
        return m_variable_bound_update_count_in_optimization;
    }

    /*************************************************************************/
    inline void set_is_preprocess(const bool a_IS_PREPROCESS) {
        m_is_preprocess = a_IS_PREPROCESS;
    }

    /*************************************************************************/
    inline bool is_preprocess(void) const {
        return m_is_preprocess;
    }
};
}  // namespace printemps::preprocess
#endif
/*****************************************************************************/
// END
/*****************************************************************************/
