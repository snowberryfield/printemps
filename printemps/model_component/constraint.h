/*****************************************************************************/
// Copyright (c) 2020-2023 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MODEL_COMPONENT_CONSTRAINT_H__
#define PRINTEMPS_MODEL_COMPONENT_CONSTRAINT_H__

namespace printemps::neighborhood {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct Move;
}  // namespace printemps::neighborhood

namespace printemps::model_component {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Expression;

/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Constraint : public multi_array::AbstractMultiArrayElement {
    /**
     * [Access controls for special member functions]
     *  -- Default constructor : default, private
     *  -- Copy constructor    : default, private
     *  -- Copy assignment     : user-defined, public
     *  -- Move constructor    : default, public
     *  -- Move assignment     : user-defined, public
     */

   public:
    std::function<T_Expression(
        const neighborhood::Move<T_Variable, T_Expression> &)>
        m_function;

    std::function<T_Expression(
        const neighborhood::Move<T_Variable, T_Expression> &)>
        m_constraint_function;

    std::function<T_Expression(
        const neighborhood::Move<T_Variable, T_Expression> &)>
        m_violation_function;

    Expression<T_Variable, T_Expression> m_expression;

    ConstraintSense m_sense;
    T_Expression    m_constraint_value;
    T_Expression    m_violation_value;
    T_Expression    m_positive_part;
    T_Expression    m_negative_part;
    bool            m_is_linear;
    bool            m_is_integer;
    bool            m_is_enabled;
    bool            m_is_less_or_equal;     /// <= or ==
    bool            m_is_greater_or_equal;  /// >= or ==

    double m_local_penalty_coefficient_less;
    double m_local_penalty_coefficient_greater;
    double m_global_penalty_coefficient;

    bool m_is_user_defined_selection;

    bool m_is_singleton;
    bool m_is_exclusive_or;
    bool m_is_exclusive_nor;
    bool m_is_inverted_integers;
    bool m_is_balanced_integers;
    bool m_is_constant_sum_integers;
    bool m_is_constant_difference_integers;
    bool m_is_constant_ratio_integers;
    bool m_is_aggregation;
    bool m_is_precedence;
    bool m_is_variable_bound;
    bool m_is_trinomial_exclusive_nor;
    bool m_is_set_partitioning;
    bool m_is_set_packing;
    bool m_is_set_covering;
    bool m_is_cardinality;
    bool m_is_invariant_knapsack;
    bool m_is_multiple_covering;
    bool m_is_equation_knapsack;
    bool m_is_binary_flow;
    bool m_is_integer_flow;
    bool m_is_soft_selection;
    bool m_is_min_max;
    bool m_is_max_min;
    bool m_is_intermediate;
    bool m_is_bin_packing;
    bool m_is_knapsack;
    bool m_is_integer_knapsack;
    bool m_is_gf2;
    bool m_is_general_linear;
    bool m_has_only_binary_coefficient;

    Variable<T_Variable, T_Expression> *m_key_variable_ptr;

    /*************************************************************************/
    /// Default constructor
    Constraint(void) {
        this->initialize();
    }

    /*************************************************************************/
    /// Copy constructor
    Constraint(const Constraint<T_Variable, T_Expression> &) = default;

    /*************************************************************************/
    Constraint(
        const std::function<T_Expression(
            const neighborhood::Move<T_Variable, T_Expression> &)> &a_FUNCTION,
        const ConstraintSense                                       a_SENSE) {
        this->setup(a_FUNCTION, a_SENSE);
    }

    /*************************************************************************/
    Constraint(const Expression<T_Variable, T_Expression> &a_EXPRESSION,
               const ConstraintSense                       a_SENSE) {
        this->setup(a_EXPRESSION, a_SENSE);
    }

   public:
    /*************************************************************************/
    /// Copy assignment
    Constraint<T_Variable, T_Expression> &operator=(
        const Constraint<T_Variable, T_Expression> &a_CONSTRAINT) {
        if (a_CONSTRAINT.m_is_linear) {
            this->setup(a_CONSTRAINT.m_expression, a_CONSTRAINT.m_sense);
        } else {
            this->setup(a_CONSTRAINT.m_function, a_CONSTRAINT.m_sense);
        }
        return *this;
    }

    /*************************************************************************/
    /// Move constructor
    Constraint(Constraint<T_Variable, T_Expression> &&) = default;

    /*************************************************************************/
    /// Move assignment
    Constraint<T_Variable, T_Expression> &operator=(
        Constraint<T_Variable, T_Expression> &&a_constraint) {
        /**
         * Re-setup is required on move assignment.
         */
        if (this == &a_constraint) {
            return *this;
        }

        if (a_constraint.is_linear()) {
            this->setup(a_constraint.m_expression, a_constraint.m_sense);
        } else {
            this->setup(a_constraint.m_function, a_constraint.m_sense);
        }
        return *this;
    }

    /*************************************************************************/
    inline static constexpr Constraint<T_Variable, T_Expression>
    create_instance(void) {
        /**
         * When instantiation, instead of constructor, create_instance() should
         * be called.
         */
        Constraint<T_Variable, T_Expression> constraint;
        return constraint;
    }

    /*************************************************************************/
    inline static constexpr Constraint<T_Variable, T_Expression>
    create_instance(
        const std::function<T_Expression(
            const neighborhood::Move<T_Variable, T_Expression> &)> &a_FUNCTION,
        const ConstraintSense                                       a_SENSE) {
        /**
         * When instantiation, instead of constructor, create_instance() should
         * be called.
         */
        Constraint<T_Variable, T_Expression> constraint(a_FUNCTION, a_SENSE);
        return constraint;
    }

    /*************************************************************************/
    inline static constexpr Constraint<T_Variable, T_Expression>
    create_instance(const Expression<T_Variable, T_Expression> &a_EXPRESSION,
                    const ConstraintSense                       a_SENSE) {
        /**
         * When instantiation, instead of constructor, create_instance() should
         * be called.
         */
        Constraint<T_Variable, T_Expression> constraint(a_EXPRESSION, a_SENSE);
        return constraint;
    }

    /*************************************************************************/
    void initialize(void) {
        multi_array::AbstractMultiArrayElement::initialize();

        m_function =  //
            []([[maybe_unused]] const neighborhood::Move<
                T_Variable, T_Expression> &a_MOVE) {
                return static_cast<T_Expression>(0);
            };
        m_constraint_function = []([[maybe_unused]] const neighborhood::Move<
                                    T_Variable, T_Expression> &a_MOVE) {
            return static_cast<T_Expression>(0);
        };
        m_violation_function = []([[maybe_unused]] const neighborhood::Move<
                                   T_Variable, T_Expression> &a_MOVE) {
            return static_cast<T_Expression>(0);
        };

        m_expression.initialize();
        m_sense                             = ConstraintSense::Less;
        m_constraint_value                  = 0;
        m_violation_value                   = 0;
        m_positive_part                     = 0;
        m_negative_part                     = 0;
        m_is_linear                         = true;
        m_is_integer                        = false;
        m_is_enabled                        = true;
        m_is_less_or_equal                  = false;
        m_is_greater_or_equal               = false;
        m_local_penalty_coefficient_less    = HUGE_VALF;
        m_local_penalty_coefficient_greater = HUGE_VALF;
        m_global_penalty_coefficient        = HUGE_VALF;

        m_is_user_defined_selection = false;
        this->clear_constraint_type();
    }

    /*************************************************************************/
    inline constexpr void clear_constraint_type(void) {
        m_is_singleton                    = false;
        m_is_exclusive_or                 = false;
        m_is_exclusive_nor                = false;
        m_is_inverted_integers            = false;
        m_is_balanced_integers            = false;
        m_is_constant_sum_integers        = false;
        m_is_constant_difference_integers = false;
        m_is_constant_ratio_integers      = false;
        m_is_aggregation                  = false;
        m_is_precedence                   = false;
        m_is_variable_bound               = false;
        m_is_trinomial_exclusive_nor      = false;
        m_is_set_partitioning             = false;
        m_is_set_packing                  = false;
        m_is_set_covering                 = false;
        m_is_cardinality                  = false;
        m_is_invariant_knapsack           = false;
        m_is_multiple_covering            = false;
        m_is_equation_knapsack            = false;
        m_is_binary_flow                  = false;
        m_is_integer_flow                 = false;
        m_is_soft_selection               = false;
        m_is_min_max                      = false;
        m_is_max_min                      = false;
        m_is_intermediate                 = false;
        m_is_bin_packing                  = false;
        m_is_knapsack                     = false;
        m_is_integer_knapsack             = false;
        m_is_gf2                          = false;
        m_is_general_linear               = false;

        m_has_only_binary_coefficient = false;

        m_key_variable_ptr = nullptr;
    }

    /*************************************************************************/
    constexpr void setup(
        const std::function<T_Expression(
            const neighborhood::Move<T_Variable, T_Expression> &)> &a_FUNCTION,
        const ConstraintSense                                       a_SENSE) {
        m_function = a_FUNCTION;
        m_expression.initialize();
        m_sense            = a_SENSE;
        m_constraint_value = 0;
        m_violation_value  = 0;
        m_positive_part    = 0;
        m_negative_part    = 0;
        m_is_linear        = false;
        m_is_integer       = false;
        m_is_enabled       = true;

        this->clear_constraint_type();

        m_constraint_function =
            [this](const neighborhood::Move<T_Variable, T_Expression> &a_MOVE) {
                return m_function(a_MOVE);
            };

        switch (m_sense) {
            case ConstraintSense::Less: {
                m_violation_function =
                    [this](const neighborhood::Move<T_Variable, T_Expression>
                               &a_MOVE) {
                        return std::max(m_function(a_MOVE),
                                        static_cast<T_Expression>(0));
                    };
                m_is_less_or_equal    = true;
                m_is_greater_or_equal = false;
                break;
            }
            case ConstraintSense::Equal: {
                m_violation_function =
                    [this](const neighborhood::Move<T_Variable, T_Expression> &
                               a_MOVE) { return std::abs(m_function(a_MOVE)); };
                m_is_less_or_equal    = true;
                m_is_greater_or_equal = true;
                break;
            }
            case ConstraintSense::Greater: {
                m_violation_function =
                    [this](const neighborhood::Move<T_Variable, T_Expression>
                               &a_MOVE) {
                        return std::max(-m_function(a_MOVE),
                                        static_cast<T_Expression>(0));
                    };
                m_is_less_or_equal    = false;
                m_is_greater_or_equal = true;
                break;
            }
            default: {
                break;
            }
        }
    }

    /*************************************************************************/
    constexpr void setup(
        const Expression<T_Variable, T_Expression> &a_EXPRESSION,
        const ConstraintSense                       a_SENSE) {
        m_function =  //
            []([[maybe_unused]] const neighborhood::Move<
                T_Variable, T_Expression> &a_MOVE) {
                return static_cast<T_Expression>(0);
            };
        m_expression       = a_EXPRESSION;
        m_sense            = a_SENSE;
        m_constraint_value = 0;
        m_violation_value  = 0;
        m_positive_part    = 0;
        m_negative_part    = 0;
        m_is_linear        = true;
        m_is_integer       = true;
        m_is_enabled       = true;

        auto is_integer = [](const T_Expression a_VALUE) {
            return fabs(a_VALUE - floor(a_VALUE)) < constant::EPSILON_10;
        };

        if (!is_integer(m_expression.constant_value())) {
            m_is_integer = false;
        }

        for (const auto &SENSITIVITY : m_expression.sensitivities()) {
            if (!is_integer(SENSITIVITY.second)) {
                m_is_integer = false;
                break;
            }
        }

        this->clear_constraint_type();

        m_constraint_function =
            [this](const neighborhood::Move<T_Variable, T_Expression> &a_MOVE) {
                return m_expression.evaluate(a_MOVE);
            };

        switch (m_sense) {
            case ConstraintSense::Less: {
                m_violation_function =
                    [this](const neighborhood::Move<T_Variable, T_Expression>
                               &a_MOVE) {
                        return std::max(m_expression.evaluate(a_MOVE),
                                        static_cast<T_Expression>(0));
                    };
                m_is_less_or_equal    = true;
                m_is_greater_or_equal = false;
                break;
            }
            case ConstraintSense::Equal: {
                m_violation_function =
                    [this](const neighborhood::Move<T_Variable, T_Expression>
                               &a_MOVE) {
                        return std::abs(m_expression.evaluate(a_MOVE));
                    };
                m_is_less_or_equal    = true;
                m_is_greater_or_equal = true;
                break;
            }
            case ConstraintSense::Greater: {
                m_violation_function =
                    [this](const neighborhood::Move<T_Variable, T_Expression>
                               &a_MOVE) {
                        return std::max(-m_expression.evaluate(a_MOVE),
                                        static_cast<T_Expression>(0));
                    };
                m_is_less_or_equal    = false;
                m_is_greater_or_equal = true;
                break;
            }
            default: {
                break;
            }
        }
    }

    /*************************************************************************/
    constexpr void setup_constraint_type(void) {
        this->clear_constraint_type();

        /// Singleton
        if (m_expression.sensitivities().size() == 1) {
            m_is_singleton = true;
            return;
        }

        /// XOR, XNOR, Inverted Integers, Balanced Integers, Constant Sum
        /// Integers, Constant Difference Integer, Constant Ratio Integers, and
        /// Aggregation
        if (m_expression.sensitivities().size() == 2 &&
            m_sense == ConstraintSense::Equal) {
            bool is_valid = true;
            for (const auto &sensitivity : m_expression.sensitivities()) {
                if (sensitivity.first->is_fixed()) {
                    is_valid = false;
                    break;
                }
            }

            if (is_valid) {
                auto &     sensitivities  = m_expression.sensitivities();
                const auto CONSTANT_VALUE = m_expression.constant_value();
                std::vector<Variable<T_Variable, T_Expression> *> variable_ptrs;
                std::vector<T_Expression>                         coefficients;

                for (const auto &sensitivity : sensitivities) {
                    variable_ptrs.push_back(sensitivity.first);
                    coefficients.push_back(sensitivity.second);
                }

                if ((variable_ptrs[0]->sense() == VariableSense::Binary ||
                     variable_ptrs[0]->sense() ==
                         VariableSense::DependentBinary) &&
                    (variable_ptrs[1]->sense() == VariableSense::Binary ||
                     variable_ptrs[1]->sense() ==
                         VariableSense::DependentBinary)) {
                    if (variable_ptrs[0]->name() < variable_ptrs[1]->name()) {
                        m_key_variable_ptr = variable_ptrs[0];
                    } else {
                        m_key_variable_ptr = variable_ptrs[1];
                    }
                    if (coefficients[0] == 1 && coefficients[1] == 1 &&
                        CONSTANT_VALUE == -1) {
                        m_is_exclusive_or = true;
                        return;
                    } else if (coefficients[0] == -1 && coefficients[1] == -1 &&
                               CONSTANT_VALUE == 1) {
                        m_is_exclusive_or = true;
                        return;
                    } else if (coefficients[0] == 1 && coefficients[1] == -1 &&
                               CONSTANT_VALUE == 0) {
                        m_is_exclusive_nor = true;
                        return;
                    } else if (coefficients[0] == -1 && coefficients[1] == 1 &&
                               CONSTANT_VALUE == 0) {
                        m_is_exclusive_nor = true;
                        return;
                    }
                    m_key_variable_ptr = nullptr;
                }

                if ((variable_ptrs[0]->sense() == VariableSense::Integer ||
                     variable_ptrs[0]->sense() ==
                         VariableSense::DependentInteger) &&
                    (variable_ptrs[1]->sense() == VariableSense::Integer ||
                     variable_ptrs[1]->sense() ==
                         VariableSense::DependentInteger)) {
                    if (variable_ptrs[0]->name() < variable_ptrs[1]->name()) {
                        m_key_variable_ptr = variable_ptrs[0];
                    } else {
                        m_key_variable_ptr = variable_ptrs[1];
                    }
                    if (coefficients[0] == 1 && coefficients[1] == 1 &&
                        CONSTANT_VALUE == 0) {
                        m_is_inverted_integers = true;
                        return;
                    } else if (coefficients[0] == -1 && coefficients[1] == -1 &&
                               CONSTANT_VALUE == 0) {
                        m_is_inverted_integers = true;
                        return;
                    } else if (coefficients[0] == 1 && coefficients[1] == -1 &&
                               CONSTANT_VALUE == 0) {
                        m_is_balanced_integers = true;
                        return;
                    } else if (coefficients[0] == -1 && coefficients[1] == 1 &&
                               CONSTANT_VALUE == 0) {
                        m_is_balanced_integers = true;
                        return;
                    } else if (coefficients[0] == 1 && coefficients[1] == 1 &&
                               CONSTANT_VALUE != 0) {
                        m_is_constant_sum_integers = true;
                        return;
                    } else if (coefficients[0] == -1 && coefficients[1] == -1 &&
                               CONSTANT_VALUE != 0) {
                        m_is_constant_sum_integers = true;
                        return;
                    } else if (coefficients[0] == 1 && coefficients[1] == -1 &&
                               CONSTANT_VALUE != 0) {
                        m_is_constant_difference_integers = true;
                        return;
                    } else if (coefficients[0] == -1 && coefficients[1] == 1 &&
                               CONSTANT_VALUE != 0) {
                        m_is_constant_difference_integers = true;
                        return;
                    } else if (fabs(coefficients[0]) == 1 &&
                               fabs(coefficients[1]) != 1 &&
                               CONSTANT_VALUE == 0) {
                        m_is_constant_ratio_integers = true;
                        m_key_variable_ptr           = variable_ptrs[0];
                        return;
                    } else if (fabs(coefficients[0]) != 1 &&
                               fabs(coefficients[1]) == 1 &&
                               CONSTANT_VALUE == 0) {
                        m_is_constant_ratio_integers = true;
                        m_key_variable_ptr           = variable_ptrs[1];
                        return;
                    } else if (fabs(coefficients[0]) == 1 &&
                               fabs(coefficients[1]) != 1 && m_is_integer) {
                        m_is_intermediate  = true;
                        m_key_variable_ptr = variable_ptrs[0];
                        return;
                    } else if (fabs(coefficients[0]) != 1 &&
                               fabs(coefficients[1]) == 1 && m_is_integer) {
                        m_is_intermediate  = true;
                        m_key_variable_ptr = variable_ptrs[1];
                        return;
                    }
                    m_key_variable_ptr = nullptr;
                }

                m_is_aggregation = true;
                return;
            }
        }

        /// Precedence or Variable Bound
        if (m_expression.sensitivities().size() == 2 &&
            m_sense != ConstraintSense::Equal) {
            bool is_valid = true;
            for (const auto &sensitivity : m_expression.sensitivities()) {
                if (sensitivity.first->is_fixed()) {
                    is_valid = false;
                    break;
                }
            }
            if (is_valid) {
                auto &sensitivities = m_expression.sensitivities();
                std::vector<Variable<T_Variable, T_Expression> *> variable_ptrs;
                std::vector<T_Expression>                         coefficients;

                for (const auto &sensitivity : sensitivities) {
                    variable_ptrs.push_back(sensitivity.first);
                    coefficients.push_back(sensitivity.second);
                }

                /// Precedence
                if ((variable_ptrs[0]->sense() == variable_ptrs[1]->sense()) &&
                    (coefficients[0] == -coefficients[1])) {
                    m_is_precedence = true;
                    return;
                }

                /// Variable Bound
                /**
                 * At least one variables of the two must be binary according to
                 * the definition of Variable Bound proposed by MIPLIB 2017. But
                 * for convenience of the neighborhood definition, both of them
                 * can be integer(non-binary) variable.
                 */
                m_is_variable_bound = true;
                return;
            }
        }

        /// Trinomial XNOR
        if (m_expression.sensitivities().size() == 3 &&
            m_sense == ConstraintSense::Equal) {
            bool is_valid = true;
            for (const auto &sensitivity : m_expression.sensitivities()) {
                if (sensitivity.first->is_fixed()) {
                    is_valid = false;
                    break;
                }
            }

            if (is_valid) {
                auto &sensitivities = m_expression.sensitivities();
                std::vector<Variable<T_Variable, T_Expression> *>
                    plus_one_binary_variable_ptrs;
                std::vector<Variable<T_Variable, T_Expression> *>
                    minus_one_binary_variable_ptrs;
                std::vector<Variable<T_Variable, T_Expression> *>
                    plus_two_binary_variable_ptrs;
                std::vector<Variable<T_Variable, T_Expression> *>
                    minus_two_binary_variable_ptrs;

                for (const auto &sensitivity : sensitivities) {
                    if (sensitivity.first->sense() != VariableSense::Binary &&
                        sensitivity.first->sense() !=
                            VariableSense::DependentBinary) {
                        is_valid = false;
                        break;
                    }
                    if (sensitivity.second == 1) {
                        plus_one_binary_variable_ptrs.push_back(
                            sensitivity.first);
                    } else if (sensitivity.second == -1) {
                        minus_one_binary_variable_ptrs.push_back(
                            sensitivity.first);
                    } else if (sensitivity.second == 2) {
                        plus_two_binary_variable_ptrs.push_back(
                            sensitivity.first);
                    } else if (sensitivity.second == -2) {
                        minus_two_binary_variable_ptrs.push_back(
                            sensitivity.first);
                    }
                }

                if (is_valid && plus_one_binary_variable_ptrs.size() == 2 &&
                    minus_two_binary_variable_ptrs.size() == 1) {
                    m_is_trinomial_exclusive_nor = true;
                    m_key_variable_ptr = minus_two_binary_variable_ptrs[0];
                    return;
                } else if (is_valid &&
                           minus_one_binary_variable_ptrs.size() == 2 &&
                           plus_two_binary_variable_ptrs.size() == 1) {
                    m_is_trinomial_exclusive_nor = true;
                    m_key_variable_ptr = plus_two_binary_variable_ptrs[0];
                    return;
                }
            }
        }

        /// Set Partitioning or Set Packing, Set Covering, Cardinality,
        /// Invariant Knapsack.
        {
            bool has_only_binary_coefficient = true;
            for (const auto &sensitivity : m_expression.sensitivities()) {
                if ((sensitivity.first->sense() != VariableSense::Binary) &&
                    (sensitivity.first->sense() != VariableSense::Selection)) {
                    has_only_binary_coefficient = false;
                    break;
                }
                if (sensitivity.second != 1) {
                    has_only_binary_coefficient = false;
                    break;
                }
            }
            if (has_only_binary_coefficient) {
                m_has_only_binary_coefficient = true;
                /// Set Partitioning
                if (m_expression.constant_value() == -1 &&
                    m_sense == ConstraintSense::Equal) {
                    m_is_set_partitioning = true;
                    return;
                }

                /// Set Packing
                if (m_expression.constant_value() == -1 &&
                    m_sense == ConstraintSense::Less) {
                    m_is_set_packing = true;
                    return;
                }

                /// Set Covering
                if (m_expression.constant_value() == -1 &&
                    m_sense == ConstraintSense::Greater) {
                    m_is_set_covering = true;
                    return;
                }

                /// Cardinality
                if (m_expression.constant_value() <= -2 &&
                    m_sense == ConstraintSense::Equal) {
                    m_is_cardinality = true;
                    return;
                }

                /// Invariant Knapsack
                if (m_expression.constant_value() <= -2 &&
                    m_sense == ConstraintSense::Less) {
                    m_is_invariant_knapsack = true;
                    return;
                }

                /// Multiple Covering
                if (m_expression.constant_value() <= -2 &&
                    m_sense == ConstraintSense::Greater) {
                    m_is_multiple_covering = true;
                    return;
                }
            } else {
                m_has_only_binary_coefficient = false;
            }
        }

        /// Binary Flow, Integer Flow, Soft Selection
        {
            if (m_sense == ConstraintSense::Equal) {
                std::vector<Variable<T_Variable, T_Expression> *>
                    plus_one_variable_ptrs;
                std::vector<Variable<T_Variable, T_Expression> *>
                    minus_one_variable_ptrs;

                bool is_plus_or_minus_one_coefficient = true;
                for (const auto &sensitivity : m_expression.sensitivities()) {
                    if (abs(sensitivity.second) != 1) {
                        is_plus_or_minus_one_coefficient = false;
                        break;
                    }
                    if (sensitivity.second == 1) {
                        plus_one_variable_ptrs.push_back(sensitivity.first);
                    } else {
                        minus_one_variable_ptrs.push_back(sensitivity.first);
                    }
                }

                if (is_plus_or_minus_one_coefficient) {
                    bool       has_only_binary_variables  = true;
                    bool       has_only_integer_variables = true;
                    const auto MINUS_ONE_SIZE = minus_one_variable_ptrs.size();
                    const auto PLUS_ONE_SIZE  = plus_one_variable_ptrs.size();

                    for (const auto &sensitivity :
                         m_expression.sensitivities()) {
                        if ((sensitivity.first->sense() !=
                             VariableSense::Binary) &&
                            (sensitivity.first->sense() !=
                             VariableSense::Selection)) {
                            has_only_binary_variables = false;
                        }
                        if (sensitivity.first->sense() !=
                            VariableSense::Integer) {
                            has_only_integer_variables = false;
                        }
                        if (!has_only_binary_variables &&
                            !has_only_integer_variables) {
                            break;
                        }
                    }
                    if (has_only_binary_variables) {
                        if (PLUS_ONE_SIZE == 1 && MINUS_ONE_SIZE > 0 &&
                            !plus_one_variable_ptrs[0]->is_fixed() &&
                            m_expression.constant_value() == 0) {
                            m_is_soft_selection = true;
                            m_key_variable_ptr  = plus_one_variable_ptrs[0];
                        } else if  //
                            (PLUS_ONE_SIZE > 0 && MINUS_ONE_SIZE == 1 &&
                             !minus_one_variable_ptrs[0]->is_fixed() &&
                             m_expression.constant_value() == 0) {
                            m_is_soft_selection = true;
                            m_key_variable_ptr  = minus_one_variable_ptrs[0];
                        } else {
                            m_is_binary_flow = true;
                        }

                        return;
                    } else if (has_only_integer_variables &&
                               PLUS_ONE_SIZE > 1 && MINUS_ONE_SIZE > 1) {
                        m_is_integer_flow = true;
                        return;
                    }
                }
            }
        }

        /// Min-Max, Max-Min, Intermediate
        {
            bool is_valid = true;
            std::vector<Variable<T_Variable, T_Expression> *>
                plus_one_integer_variable_ptrs;
            std::vector<Variable<T_Variable, T_Expression> *>
                minus_one_integer_variable_ptrs;

            if (!m_is_integer) {
                is_valid = false;
            }

            if (is_valid) {
                for (const auto &sensitivity : m_expression.sensitivities()) {
                    auto variable_ptr = sensitivity.first;
                    auto coefficient  = sensitivity.second;

                    if ((variable_ptr->sense() == VariableSense::Integer ||
                         variable_ptr->sense() ==
                             VariableSense::DependentInteger) &&
                        !variable_ptr->is_fixed()) {
                        if (coefficient == 1) {
                            plus_one_integer_variable_ptrs.push_back(
                                variable_ptr);
                        } else if (coefficient == -1) {
                            minus_one_integer_variable_ptrs.push_back(
                                variable_ptr);
                        }
                    }
                }
            }

            if (is_valid) {
                const auto MINUS_ONE_SIZE =
                    minus_one_integer_variable_ptrs.size();
                const auto PLUS_ONE_SIZE =
                    plus_one_integer_variable_ptrs.size();
                const auto &SENSITIVITIES = m_expression.sensitivities();

                if (m_sense == ConstraintSense::Less &&  //
                    MINUS_ONE_SIZE == 1 && PLUS_ONE_SIZE == 0 &&
                    SENSITIVITIES.at(minus_one_integer_variable_ptrs[0]) < 0) {
                    m_is_min_max       = true;
                    m_key_variable_ptr = minus_one_integer_variable_ptrs[0];
                    return;
                }

                if (m_sense == ConstraintSense::Greater &&  //
                    PLUS_ONE_SIZE == 1 && MINUS_ONE_SIZE == 0 &&
                    SENSITIVITIES.at(plus_one_integer_variable_ptrs[0]) > 0) {
                    m_is_min_max       = true;
                    m_key_variable_ptr = plus_one_integer_variable_ptrs[0];
                    return;
                }

                if (m_sense == ConstraintSense::Greater &&  //
                    MINUS_ONE_SIZE == 1 && PLUS_ONE_SIZE == 0 &&
                    SENSITIVITIES.at(minus_one_integer_variable_ptrs[0]) < 0) {
                    m_is_max_min       = true;
                    m_key_variable_ptr = minus_one_integer_variable_ptrs[0];
                    return;
                }

                if (m_sense == ConstraintSense::Less &&  //
                    PLUS_ONE_SIZE == 1 && MINUS_ONE_SIZE == 0 &&
                    SENSITIVITIES.at(plus_one_integer_variable_ptrs[0]) > 0) {
                    m_is_max_min       = true;
                    m_key_variable_ptr = plus_one_integer_variable_ptrs[0];
                    return;
                }

                if (m_sense == ConstraintSense::Equal &&  //
                    MINUS_ONE_SIZE == 1 && PLUS_ONE_SIZE != 1) {
                    m_is_intermediate  = true;
                    m_key_variable_ptr = minus_one_integer_variable_ptrs[0];
                    return;
                }

                if (m_sense == ConstraintSense::Equal &&  //
                    PLUS_ONE_SIZE == 1 && MINUS_ONE_SIZE != 1) {
                    m_is_intermediate  = true;
                    m_key_variable_ptr = plus_one_integer_variable_ptrs[0];
                    return;
                }

                m_key_variable_ptr = nullptr;
            }
        }

        /// Equation knapsack, Bin Packing, Knapsack, and Integer Knapsack
        {
            bool has_only_binary_variables      = true;
            bool has_bin_packing_variable       = false;
            bool has_only_positive_coefficients = true;
            bool has_only_negative_coefficients = true;

            for (const auto &sensitivity : m_expression.sensitivities()) {
                if ((sensitivity.first->sense() != VariableSense::Binary) &&
                    (sensitivity.first->sense() != VariableSense::Selection)) {
                    has_only_binary_variables = false;
                }
                if (sensitivity.second == -m_expression.constant_value()) {
                    has_bin_packing_variable = true;
                }

                if (sensitivity.second < 0) {
                    has_only_positive_coefficients = false;
                }

                if (sensitivity.second > 0) {
                    has_only_negative_coefficients = false;
                }
            }

            if (has_only_binary_variables) {
                /// Equation knapsack
                if (m_sense == ConstraintSense::Equal &&
                    (has_only_positive_coefficients ||
                     has_only_negative_coefficients)) {
                    m_is_equation_knapsack = true;
                    return;
                }

                /// Bin Packing
                if (has_bin_packing_variable &&
                    ((has_only_positive_coefficients &&
                      m_sense == ConstraintSense::Less) ||
                     (has_only_negative_coefficients &&
                      m_sense == ConstraintSense::Greater))) {
                    m_is_bin_packing = true;
                    return;
                }

                /// Knapsack
                if ((has_only_positive_coefficients &&
                     m_sense == ConstraintSense::Less) ||
                    (has_only_negative_coefficients &&
                     m_sense == ConstraintSense::Greater)) {
                    m_is_knapsack = true;
                    return;
                }
            } else {
                /// Integer Knapsack
                if ((has_only_positive_coefficients &&
                     m_sense == ConstraintSense::Less) ||
                    (has_only_negative_coefficients &&
                     m_sense == ConstraintSense::Greater)) {
                    m_is_integer_knapsack = true;
                    return;
                }
            }
        }

        /// GF2
        {
            bool is_valid                                    = true;
            int  number_of_integer_variables_coefficient_two = 0;

            Variable<T_Variable, T_Expression> *key_variable_ptr = nullptr;
            T_Expression                        key_variable_coefficient = 0;

            auto constant_value = m_expression.constant_value();
            if (constant_value != 0 && abs(constant_value) != 1) {
                is_valid = false;
            }

            if (m_sense != ConstraintSense::Equal) {
                is_valid = false;
            }

            if (is_valid) {
                for (const auto &sensitivity : m_expression.sensitivities()) {
                    auto variable_ptr = sensitivity.first;
                    auto coefficient  = sensitivity.second;

                    if (variable_ptr->is_fixed()) {
                        is_valid = false;
                        break;
                    }

                    if ((variable_ptr->sense() == VariableSense::Integer ||
                         variable_ptr->sense() == VariableSense::Binary) &&
                        abs(coefficient) == 2) {
                        key_variable_ptr         = variable_ptr;
                        key_variable_coefficient = coefficient;
                        number_of_integer_variables_coefficient_two++;
                    } else if (variable_ptr->sense() != VariableSense::Binary ||
                               abs(coefficient) != 1) {
                        is_valid = false;
                        break;
                    }
                }
            }

            if (number_of_integer_variables_coefficient_two != 1) {
                is_valid = false;
            }

            if (is_valid) {
                auto rest_part_expression = m_expression.copy();
                rest_part_expression.erase(key_variable_ptr);
                T_Expression rest_part_lower_bound =
                    rest_part_expression.lower_bound();
                T_Expression rest_part_upper_bound =
                    rest_part_expression.upper_bound();

                if (key_variable_coefficient > 0) {
                    std::swap(rest_part_lower_bound, rest_part_upper_bound);
                    rest_part_lower_bound *= -1.0;
                    rest_part_upper_bound *= -1.0;
                }

                if (key_variable_ptr->lower_bound() != constant::INT_HALF_MIN &&
                    key_variable_ptr->lower_bound() >
                        static_cast<int>(
                            std::ceil(rest_part_lower_bound * 0.5))) {
                    is_valid = false;
                }

                if (key_variable_ptr->upper_bound() != constant::INT_HALF_MAX &&
                    key_variable_ptr->upper_bound() <
                        static_cast<int>(
                            std::floor(rest_part_upper_bound * 0.5))) {
                    is_valid = false;
                }
            }

            if (is_valid) {
                m_is_gf2           = true;
                m_key_variable_ptr = key_variable_ptr;
                return;
            }
        }

        /// Otherwise, the constraint type is set to general linear.
        m_is_general_linear = true;
    }

    /*************************************************************************/
    inline constexpr T_Expression evaluate_constraint(void) const noexcept {
#ifdef _PRINTEMPS_LINEAR_MINIMIZATION
        return m_expression.evaluate({});
#else
        return m_constraint_function({});
#endif
    }

    /*************************************************************************/
    inline constexpr T_Expression evaluate_constraint(
        const neighborhood::Move<T_Variable, T_Expression> &a_MOVE) const
        noexcept {
#ifdef _PRINTEMPS_LINEAR_MINIMIZATION
        return m_expression.evaluate(a_MOVE);
#else
        return m_constraint_function(a_MOVE);
#endif
    }

    /*************************************************************************/
    inline constexpr T_Expression evaluate_violation(void) const noexcept {
        return m_violation_function({});
    }

    /*************************************************************************/
    inline constexpr T_Expression evaluate_violation(
        const neighborhood::Move<T_Variable, T_Expression> &a_MOVE) const
        noexcept {
        return m_violation_function(a_MOVE);
    }

    /*************************************************************************/
    inline constexpr T_Expression evaluate_violation_diff(
        const neighborhood::Move<T_Variable, T_Expression> &a_MOVE) const
        noexcept {
        return m_violation_function(a_MOVE) - m_violation_value;
    }

    /*************************************************************************/
    inline constexpr void update(void) {
        /**
         * m_expression must be updated at first.
         */
#ifdef _PRINTEMPS_LINEAR_MINIMIZATION
        m_expression.update();
#else
        if (m_is_linear) {
            m_expression.update();
        }
#endif
        m_constraint_value = m_constraint_function({});
        m_violation_value  = m_violation_function({});
        m_positive_part =
            std::max(m_constraint_value, static_cast<T_Expression>(0));
        m_negative_part =
            -std::min(m_constraint_value, static_cast<T_Expression>(0));
    }

    /*************************************************************************/
    inline constexpr void update(
        const neighborhood::Move<T_Variable, T_Expression> &a_MOVE) {
        /**
         * m_expression must be updated after m_constraint_value and
         * m_violation.
         */
        m_constraint_value = m_constraint_function(a_MOVE);
        m_violation_value  = m_violation_function(a_MOVE);
        m_positive_part =
            std::max(m_constraint_value, static_cast<T_Expression>(0));
        m_negative_part =
            -std::min(m_constraint_value, static_cast<T_Expression>(0));

#ifdef _PRINTEMPS_LINEAR_MINIMIZATION
        m_expression.update(a_MOVE);
#else
        if (m_is_linear) {
            m_expression.update(a_MOVE);
        }
#endif
    }

    /*************************************************************************/
    inline constexpr Expression<T_Variable, T_Expression> &expression(
        void) noexcept {
        return m_expression;
    }

    /*************************************************************************/
    inline constexpr const Expression<T_Variable, T_Expression> &expression(
        void) const noexcept {
        return m_expression;
    }

    /*************************************************************************/
    inline constexpr ConstraintSense sense(void) const noexcept {
        return m_sense;
    }

    /*************************************************************************/
    inline constexpr T_Expression constraint_value(void) const noexcept {
        return m_constraint_value;
    }

    /*************************************************************************/
    inline constexpr T_Expression violation_value(void) const noexcept {
        return m_violation_value;
    }

    /*************************************************************************/
    inline constexpr T_Expression positive_part(void) const noexcept {
        return m_positive_part;
    }

    /*************************************************************************/
    inline constexpr T_Expression negative_part(void) const noexcept {
        return m_negative_part;
    }

    /*************************************************************************/
    inline constexpr bool is_feasible(void) const noexcept {
        return m_violation_value < constant::EPSILON;
    }

    /*************************************************************************/
    inline constexpr double &local_penalty_coefficient_less(void) noexcept {
        return m_local_penalty_coefficient_less;
    }

    /*************************************************************************/
    inline constexpr double local_penalty_coefficient_less(void) const
        noexcept {
        return m_local_penalty_coefficient_less;
    }

    /*************************************************************************/
    inline constexpr double &local_penalty_coefficient_greater(void) noexcept {
        return m_local_penalty_coefficient_greater;
    }
    /*************************************************************************/
    inline constexpr double local_penalty_coefficient_greater(void) const
        noexcept {
        return m_local_penalty_coefficient_greater;
    }

    /*************************************************************************/
    inline constexpr double &global_penalty_coefficient(void) noexcept {
        return m_global_penalty_coefficient;
    }

    /*************************************************************************/
    inline constexpr double global_penalty_coefficient(void) const noexcept {
        return m_global_penalty_coefficient;
    }

    /*************************************************************************/
    inline constexpr void reset_local_penalty_coefficient(void) noexcept {
        m_local_penalty_coefficient_less    = m_global_penalty_coefficient;
        m_local_penalty_coefficient_greater = m_global_penalty_coefficient;
    }

    /*************************************************************************/
    inline constexpr void limit_local_penalty_coefficient(void) noexcept {
        m_local_penalty_coefficient_less = std::min(
            m_local_penalty_coefficient_less, m_global_penalty_coefficient);
        m_local_penalty_coefficient_greater = std::min(
            m_local_penalty_coefficient_greater, m_global_penalty_coefficient);
    }

    /*************************************************************************/
    inline constexpr bool is_user_defined_selection(void) const noexcept {
        return m_is_user_defined_selection;
    }

    /*************************************************************************/
    inline constexpr void set_is_user_defined_selection(
        const bool a_IS_USER_DEFINED_SELECTION) noexcept {
        m_is_user_defined_selection = a_IS_USER_DEFINED_SELECTION;
    }

    /*************************************************************************/
    inline constexpr bool is_linear(void) const noexcept {
        return m_is_linear;
    }

    /*************************************************************************/
    inline constexpr bool is_integer(void) const noexcept {
        return m_is_integer;
    }

    /*************************************************************************/
    inline constexpr bool is_singleton(void) const noexcept {
        return m_is_singleton;
    }

    /*************************************************************************/
    inline constexpr bool is_exclusive_or(void) const noexcept {
        return m_is_exclusive_or;
    }

    /*************************************************************************/
    inline constexpr bool is_exclusive_nor(void) const noexcept {
        return m_is_exclusive_nor;
    }

    /*************************************************************************/
    inline constexpr bool is_inverted_integers(void) const noexcept {
        return m_is_inverted_integers;
    }

    /*************************************************************************/
    inline constexpr bool is_balanced_integers(void) const noexcept {
        return m_is_balanced_integers;
    }

    /*************************************************************************/
    inline constexpr bool is_constant_sum_integers(void) const noexcept {
        return m_is_constant_sum_integers;
    }

    /*************************************************************************/
    inline constexpr bool is_constant_difference_integers(void) const noexcept {
        return m_is_constant_difference_integers;
    }

    /*************************************************************************/
    inline constexpr bool is_constant_ratio_integers(void) const noexcept {
        return m_is_constant_ratio_integers;
    }

    /*************************************************************************/
    inline constexpr bool is_aggregation(void) const noexcept {
        return m_is_aggregation;
    }

    /*************************************************************************/
    inline constexpr bool is_precedence(void) const noexcept {
        return m_is_precedence;
    }

    /*************************************************************************/
    inline constexpr bool is_variable_bound(void) const noexcept {
        return m_is_variable_bound;
    }

    /*************************************************************************/
    inline constexpr bool is_trinomial_exclusive_nor(void) const noexcept {
        return m_is_trinomial_exclusive_nor;
    }

    /*************************************************************************/
    inline constexpr bool is_set_partitioning(void) const noexcept {
        return m_is_set_partitioning;
    }

    /*************************************************************************/
    inline constexpr bool is_set_packing(void) const noexcept {
        return m_is_set_packing;
    }

    /*************************************************************************/
    inline constexpr bool is_set_covering(void) const noexcept {
        return m_is_set_covering;
    }

    /*************************************************************************/
    inline constexpr bool is_cardinality(void) const noexcept {
        return m_is_cardinality;
    }

    /*************************************************************************/
    inline constexpr bool is_invariant_knapsack(void) const noexcept {
        return m_is_invariant_knapsack;
    }

    /*************************************************************************/
    inline constexpr bool is_multiple_covering(void) const noexcept {
        return m_is_multiple_covering;
    }

    /*************************************************************************/
    inline constexpr bool is_binary_flow(void) const noexcept {
        return m_is_binary_flow;
    }

    /*************************************************************************/
    inline constexpr bool is_integer_flow(void) const noexcept {
        return m_is_integer_flow;
    }

    /*************************************************************************/
    inline constexpr bool is_soft_selection(void) const noexcept {
        return m_is_soft_selection;
    }

    /*************************************************************************/
    inline constexpr bool is_min_max(void) const noexcept {
        return m_is_min_max;
    }

    /*************************************************************************/
    inline constexpr bool is_max_min(void) const noexcept {
        return m_is_max_min;
    }

    /*************************************************************************/
    inline constexpr bool is_intermediate(void) const noexcept {
        return m_is_intermediate;
    }

    /*************************************************************************/
    inline constexpr bool is_equation_knapsack(void) const noexcept {
        return m_is_equation_knapsack;
    }

    /*************************************************************************/
    inline constexpr bool is_bin_packing(void) const noexcept {
        return m_is_bin_packing;
    }

    /*************************************************************************/
    inline constexpr bool is_knapsack(void) const noexcept {
        return m_is_knapsack;
    }

    /*************************************************************************/
    inline constexpr bool is_integer_knapsack(void) const noexcept {
        return m_is_integer_knapsack;
    }

    /*************************************************************************/
    inline constexpr bool is_gf2(void) const noexcept {
        return m_is_gf2;
    }

    /*************************************************************************/
    inline constexpr bool is_general_linear(void) const noexcept {
        return m_is_general_linear;
    }

    /*************************************************************************/
    inline constexpr bool has_only_binary_coefficient(void) const noexcept {
        return m_has_only_binary_coefficient;
    }

    /*************************************************************************/
    inline constexpr bool is_enabled(void) const noexcept {
        return m_is_enabled;
    }

    /*************************************************************************/
    inline constexpr bool is_less_or_equal(void) const noexcept {
        return m_is_less_or_equal;
    }

    /*************************************************************************/
    inline constexpr bool is_greater_or_equal(void) const noexcept {
        return m_is_greater_or_equal;
    }

    /*************************************************************************/
    inline constexpr void enable(void) noexcept {
        m_is_enabled = true;
    }

    /*************************************************************************/
    inline constexpr void disable(void) noexcept {
        m_is_enabled = false;
    }

    /*************************************************************************/
    inline constexpr Variable<T_Variable, T_Expression> *key_variable_ptr(
        void) const {
        return const_cast<Variable<T_Variable, T_Expression> *>(
            m_key_variable_ptr);
    }
};
using IPConstraint = Constraint<int, double>;
}  // namespace printemps::model_component
#endif
/*****************************************************************************/
// END
/*****************************************************************************/