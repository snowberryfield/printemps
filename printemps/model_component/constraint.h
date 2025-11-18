/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
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
    Expression<T_Variable, T_Expression> m_expression;

    ConstraintSense m_sense;
    T_Expression    m_constraint_value;
    T_Expression    m_violation_value;
    T_Expression    m_margin_value;
    T_Expression    m_positive_part;
    T_Expression    m_negative_part;
    T_Expression    m_max_abs_coefficient;

    double m_local_penalty_coefficient_less;
    double m_local_penalty_coefficient_greater;
    double m_global_penalty_coefficient;

    Variable<T_Variable, T_Expression> *m_key_variable_ptr;

    long m_violation_count;

    bool m_is_integer;
    bool m_is_enabled;
    bool m_is_less_or_equal;     /// <= or ==
    bool m_is_greater_or_equal;  /// >= or ==
    bool m_is_selection;
    bool m_has_margin;

    bool m_is_user_defined_selection;
    bool m_has_only_binary_coefficient;
    bool m_has_only_binary_variable;

    ConstraintType m_type;

    /*************************************************************************/
    /// Default constructor
    Constraint(void) {
        this->initialize();
    }

    /*************************************************************************/
    /// Copy constructor
    Constraint(const Constraint<T_Variable, T_Expression> &) = default;

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
        this->setup(a_CONSTRAINT.m_expression, a_CONSTRAINT.m_sense);
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
        this->setup(a_constraint.m_expression, a_constraint.m_sense);
        return *this;
    }

    /*************************************************************************/
    inline static Constraint<T_Variable, T_Expression> create_instance(void) {
        /**
         * When instantiation, instead of constructor, create_instance() should
         * be called.
         */
        Constraint<T_Variable, T_Expression> constraint;
        return constraint;
    }

    /*************************************************************************/
    inline static Constraint<T_Variable, T_Expression> create_instance(
        const Expression<T_Variable, T_Expression> &a_EXPRESSION,
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
        m_expression.initialize();
        m_sense               = ConstraintSense::Less;
        m_constraint_value    = 0;
        m_violation_value     = 0;
        m_margin_value        = 0;
        m_positive_part       = 0;
        m_negative_part       = 0;
        m_max_abs_coefficient = 0;

        m_local_penalty_coefficient_less    = HUGE_VALF;
        m_local_penalty_coefficient_greater = HUGE_VALF;
        m_global_penalty_coefficient        = HUGE_VALF;

        m_key_variable_ptr = nullptr;
        m_violation_count  = 0;

        m_is_integer          = false;
        m_is_enabled          = true;
        m_is_less_or_equal    = false;
        m_is_greater_or_equal = false;
        m_is_selection        = false;
        m_has_margin          = false;

        m_is_user_defined_selection   = false;
        m_has_only_binary_coefficient = false;
        m_has_only_binary_variable    = false;

        m_type             = ConstraintType::Unknown;
        m_key_variable_ptr = nullptr;
    }

    /*************************************************************************/
    inline void setup(const Expression<T_Variable, T_Expression> &a_EXPRESSION,
                      const ConstraintSense                       a_SENSE) {
        m_expression       = a_EXPRESSION;
        m_sense            = a_SENSE;
        m_constraint_value = 0;
        m_violation_value  = 0;
        m_margin_value     = 0;
        m_positive_part    = 0;
        m_negative_part    = 0;
        m_is_enabled       = true;

        this->update_basic_structure();
        this->update_constraint_type();
    }

    /*************************************************************************/
    inline void update_basic_structure(void) {
        m_is_integer          = m_expression.is_integer();
        m_max_abs_coefficient = m_expression.max_abs_coefficient();
        m_has_only_binary_coefficient =
            m_expression.has_only_binary_coefficient();
        m_has_only_binary_variable = m_expression.has_only_binary_variable();

        switch (m_sense) {
            case ConstraintSense::Less: {
                m_is_less_or_equal    = true;
                m_is_greater_or_equal = false;
                break;
            }
            case ConstraintSense::Equal: {
                m_is_less_or_equal    = true;
                m_is_greater_or_equal = true;
                break;
            }
            case ConstraintSense::Greater: {
                m_is_less_or_equal    = false;
                m_is_greater_or_equal = true;
                break;
            }
            default: {
                throw std::logic_error(utility::format_error_location(
                    __FILE__, __LINE__, __func__,
                    "Constraint sense is invalid."));
            }
        }
    }

    /*************************************************************************/
    inline void update_constraint_type(void) {
        m_type             = ConstraintType::Unknown;
        m_key_variable_ptr = nullptr;

        /// Singleton
        if (m_expression.sensitivities().size() == 1) {
            m_type = ConstraintType::Singleton;
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
                auto      &sensitivities  = m_expression.sensitivities();
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
                        m_type = ConstraintType::ExclusiveOR;
                        return;
                    } else if (coefficients[0] == -1 && coefficients[1] == -1 &&
                               CONSTANT_VALUE == 1) {
                        m_type = ConstraintType::ExclusiveOR;
                        return;
                    } else if (coefficients[0] == 1 && coefficients[1] == -1 &&
                               CONSTANT_VALUE == 0) {
                        m_type = ConstraintType::ExclusiveNOR;
                        return;
                    } else if (coefficients[0] == -1 && coefficients[1] == 1 &&
                               CONSTANT_VALUE == 0) {
                        m_type = ConstraintType::ExclusiveNOR;
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
                        m_type = ConstraintType::InvertedIntegers;
                        return;
                    } else if (coefficients[0] == -1 && coefficients[1] == -1 &&
                               CONSTANT_VALUE == 0) {
                        m_type = ConstraintType::InvertedIntegers;
                        return;
                    } else if (coefficients[0] == 1 && coefficients[1] == -1 &&
                               CONSTANT_VALUE == 0) {
                        m_type = ConstraintType::BalancedIntegers;
                        return;
                    } else if (coefficients[0] == -1 && coefficients[1] == 1 &&
                               CONSTANT_VALUE == 0) {
                        m_type = ConstraintType::BalancedIntegers;
                        return;
                    } else if (coefficients[0] == 1 && coefficients[1] == 1 &&
                               CONSTANT_VALUE != 0) {
                        m_type = ConstraintType::ConstantSumIntegers;
                        return;
                    } else if (coefficients[0] == -1 && coefficients[1] == -1 &&
                               CONSTANT_VALUE != 0) {
                        m_type = ConstraintType::ConstantSumIntegers;
                        return;
                    } else if (coefficients[0] == 1 && coefficients[1] == -1 &&
                               CONSTANT_VALUE != 0) {
                        m_type = ConstraintType::ConstantDifferenceIntegers;
                        return;
                    } else if (coefficients[0] == -1 && coefficients[1] == 1 &&
                               CONSTANT_VALUE != 0) {
                        m_type = ConstraintType::ConstantDifferenceIntegers;
                        return;
                    } else if (std::abs(coefficients[0]) == 1 &&
                               std::abs(coefficients[1]) != 1 &&
                               CONSTANT_VALUE == 0) {
                        m_type = ConstraintType::ConstantRatioIntegers;
                        m_key_variable_ptr = variable_ptrs[0];
                        return;
                    } else if (std::abs(coefficients[0]) != 1 &&
                               std::abs(coefficients[1]) == 1 &&
                               CONSTANT_VALUE == 0) {
                        m_type = ConstraintType::ConstantRatioIntegers;
                        m_key_variable_ptr = variable_ptrs[1];
                        return;
                    } else if (std::abs(coefficients[0]) == 1 &&
                               std::abs(coefficients[1]) != 1 && m_is_integer) {
                        m_type             = ConstraintType::Intermediate;
                        m_key_variable_ptr = variable_ptrs[0];
                        return;
                    } else if (std::abs(coefficients[0]) != 1 &&
                               std::abs(coefficients[1]) == 1 && m_is_integer) {
                        m_type             = ConstraintType::Intermediate;
                        m_key_variable_ptr = variable_ptrs[1];
                        return;
                    }
                    m_key_variable_ptr = nullptr;
                }

                m_type = ConstraintType::Aggregation;
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
                    m_type = ConstraintType::Precedence;
                    return;
                }

                /// Variable Bound
                /**
                 * At least one variables of the two must be binary according to
                 * the definition of Variable Bound proposed by MIPLIB 2017. But
                 * for convenience of the neighborhood definition, both of them
                 * can be integer(non-binary) variable.
                 */
                m_type = ConstraintType::VariableBound;
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
                    m_type             = ConstraintType::TrinomialExclusiveNOR;
                    m_key_variable_ptr = minus_two_binary_variable_ptrs[0];
                    return;
                } else if (is_valid &&
                           minus_one_binary_variable_ptrs.size() == 2 &&
                           plus_two_binary_variable_ptrs.size() == 1) {
                    m_type             = ConstraintType::TrinomialExclusiveNOR;
                    m_key_variable_ptr = plus_two_binary_variable_ptrs[0];
                    return;
                }
            }
        }

        /// Set Partitioning or Set Packing, Set Covering, Cardinality,
        /// Invariant Knapsack.
        {
            if (m_has_only_binary_coefficient && m_has_only_binary_variable) {
                /// Set Partitioning
                if (m_expression.constant_value() == -1 &&
                    m_sense == ConstraintSense::Equal) {
                    m_type = ConstraintType::SetPartitioning;
                    return;
                }

                /// Set Packing
                if (m_expression.constant_value() == -1 &&
                    m_sense == ConstraintSense::Less) {
                    m_type = ConstraintType::SetPacking;
                    return;
                }

                /// Set Covering
                if (m_expression.constant_value() == -1 &&
                    m_sense == ConstraintSense::Greater) {
                    m_type = ConstraintType::SetCovering;
                    return;
                }

                /// Cardinality
                if (m_expression.constant_value() <= -2 &&
                    m_sense == ConstraintSense::Equal) {
                    m_type = ConstraintType::Cardinality;
                    return;
                }

                /// Invariant Knapsack
                if (m_expression.constant_value() <= -2 &&
                    m_sense == ConstraintSense::Less) {
                    m_type = ConstraintType::InvariantKnapsack;
                    return;
                }

                /// Multiple Covering
                if (m_expression.constant_value() <= -2 &&
                    m_sense == ConstraintSense::Greater) {
                    m_type = ConstraintType::MultipleCovering;
                    return;
                }
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
                            m_type             = ConstraintType::SoftSelection;
                            m_key_variable_ptr = plus_one_variable_ptrs[0];
                        } else if  //
                            (PLUS_ONE_SIZE > 0 && MINUS_ONE_SIZE == 1 &&
                             !minus_one_variable_ptrs[0]->is_fixed() &&
                             m_expression.constant_value() == 0) {
                            m_type             = ConstraintType::SoftSelection;
                            m_key_variable_ptr = minus_one_variable_ptrs[0];
                        } else {
                            m_type = ConstraintType::BinaryFlow;
                        }

                        return;
                    } else if (has_only_integer_variables &&
                               PLUS_ONE_SIZE > 1 && MINUS_ONE_SIZE > 1) {
                        m_type = ConstraintType::IntegerFlow;
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
                    m_type             = ConstraintType::MinMax;
                    m_key_variable_ptr = minus_one_integer_variable_ptrs[0];
                    return;
                }

                if (m_sense == ConstraintSense::Greater &&  //
                    PLUS_ONE_SIZE == 1 && MINUS_ONE_SIZE == 0 &&
                    SENSITIVITIES.at(plus_one_integer_variable_ptrs[0]) > 0) {
                    m_type             = ConstraintType::MinMax;
                    m_key_variable_ptr = plus_one_integer_variable_ptrs[0];
                    return;
                }

                if (m_sense == ConstraintSense::Greater &&  //
                    MINUS_ONE_SIZE == 1 && PLUS_ONE_SIZE == 0 &&
                    SENSITIVITIES.at(minus_one_integer_variable_ptrs[0]) < 0) {
                    m_type             = ConstraintType::MaxMin;
                    m_key_variable_ptr = minus_one_integer_variable_ptrs[0];
                    return;
                }

                if (m_sense == ConstraintSense::Less &&  //
                    PLUS_ONE_SIZE == 1 && MINUS_ONE_SIZE == 0 &&
                    SENSITIVITIES.at(plus_one_integer_variable_ptrs[0]) > 0) {
                    m_type             = ConstraintType::MaxMin;
                    m_key_variable_ptr = plus_one_integer_variable_ptrs[0];
                    return;
                }

                if (m_sense == ConstraintSense::Equal &&  //
                    MINUS_ONE_SIZE == 1 && PLUS_ONE_SIZE != 1) {
                    m_type             = ConstraintType::Intermediate;
                    m_key_variable_ptr = minus_one_integer_variable_ptrs[0];
                    return;
                }

                if (m_sense == ConstraintSense::Equal &&  //
                    PLUS_ONE_SIZE == 1 && MINUS_ONE_SIZE != 1) {
                    m_type             = ConstraintType::Intermediate;
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
                    m_type = ConstraintType::EquationKnapsack;
                    return;
                }

                /// Bin Packing
                if (has_bin_packing_variable &&
                    ((has_only_positive_coefficients &&
                      m_sense == ConstraintSense::Less) ||
                     (has_only_negative_coefficients &&
                      m_sense == ConstraintSense::Greater))) {
                    m_type = ConstraintType::BinPacking;
                    return;
                }

                /// Knapsack
                if ((has_only_positive_coefficients &&
                     m_sense == ConstraintSense::Less) ||
                    (has_only_negative_coefficients &&
                     m_sense == ConstraintSense::Greater)) {
                    m_type = ConstraintType::Knapsack;
                    return;
                }
            } else {
                /// Integer Knapsack
                if ((has_only_positive_coefficients &&
                     m_sense == ConstraintSense::Less) ||
                    (has_only_negative_coefficients &&
                     m_sense == ConstraintSense::Greater)) {
                    m_type = ConstraintType::IntegerKnapsack;
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
                m_type             = ConstraintType::GF2;
                m_key_variable_ptr = key_variable_ptr;
                return;
            }
        }

        /// Otherwise, the constraint type is set to general linear.
        m_type = ConstraintType::GeneralLinear;
    }

    /*************************************************************************/
    inline T_Expression evaluate_constraint(void) const noexcept {
        return m_expression.evaluate({});
    }

    /*************************************************************************/
    inline T_Expression evaluate_constraint(
        const neighborhood::Move<T_Variable, T_Expression> &a_MOVE)
        const noexcept {
        return m_expression.evaluate(a_MOVE);
    }

    /*************************************************************************/
    inline void update(void) {
        /**
         * m_expression must be updated at first.
         */
        m_expression.update();
        m_constraint_value = m_expression.evaluate({});

        m_positive_part =
            std::max(m_constraint_value, static_cast<T_Expression>(0));
        m_negative_part =
            -std::min(m_constraint_value, static_cast<T_Expression>(0));
        switch (m_sense) {
            case ConstraintSense::Less: {
                m_violation_value = m_positive_part;
                m_margin_value    = m_negative_part;
                break;
            }
            case ConstraintSense::Equal: {
                m_violation_value = m_positive_part + m_negative_part;
                m_margin_value    = 0;
                break;
            }
            case ConstraintSense::Greater: {
                m_violation_value = m_negative_part;
                m_margin_value    = m_positive_part;
                break;
            }
        }
        m_has_margin =
            m_has_only_binary_variable &
            (m_margin_value >= m_max_abs_coefficient - constant::EPSILON_10);
    }

    /*************************************************************************/
    inline void update(
        const neighborhood::Move<T_Variable, T_Expression> &a_MOVE) {
        /**
         * m_expression must be updated after m_constraint_value and
         * m_violation.
         */
        m_constraint_value = m_expression.evaluate(a_MOVE);
        m_positive_part =
            std::max(m_constraint_value, static_cast<T_Expression>(0));
        m_negative_part =
            -std::min(m_constraint_value, static_cast<T_Expression>(0));
        switch (m_sense) {
            case ConstraintSense::Less: {
                m_violation_value = m_positive_part;
                m_margin_value    = m_negative_part;
                break;
            }
            case ConstraintSense::Equal: {
                m_violation_value = m_positive_part + m_negative_part;
                m_margin_value    = 0;
                break;
            }
            case ConstraintSense::Greater: {
                m_violation_value = m_negative_part;
                m_margin_value    = m_positive_part;
                break;
            }
            default: {
                throw std::logic_error(utility::format_error_location(
                    __FILE__, __LINE__, __func__,
                    "Constraint sense is invalid."));
            }
        }
        m_has_margin = m_has_only_binary_variable &
                       (m_margin_value >= m_max_abs_coefficient);
        m_expression.update(a_MOVE);
    }

    /*************************************************************************/
    inline Expression<T_Variable, T_Expression> &expression(void) noexcept {
        return m_expression;
    }

    /*************************************************************************/
    inline const Expression<T_Variable, T_Expression> &expression(
        void) const noexcept {
        return m_expression;
    }

    /*************************************************************************/
    inline ConstraintSense sense(void) const noexcept {
        return m_sense;
    }

    /*************************************************************************/
    inline T_Expression constraint_value(void) const noexcept {
        return m_constraint_value;
    }

    /*************************************************************************/
    inline T_Expression violation_value(void) const noexcept {
        return m_violation_value;
    }

    /*************************************************************************/
    inline T_Expression margin_value(void) const noexcept {
        return m_margin_value;
    }

    /*************************************************************************/
    inline T_Expression positive_part(void) const noexcept {
        return m_positive_part;
    }

    /*************************************************************************/
    inline T_Expression negative_part(void) const noexcept {
        return m_negative_part;
    }

    /*************************************************************************/
    inline T_Expression max_abs_coefficient(void) const noexcept {
        return m_max_abs_coefficient;
    }

    /*************************************************************************/
    inline bool is_feasible(void) const noexcept {
        return m_violation_value < constant::EPSILON;
    }

    /*************************************************************************/
    inline double &local_penalty_coefficient_less(void) noexcept {
        return m_local_penalty_coefficient_less;
    }

    /*************************************************************************/
    inline double local_penalty_coefficient_less(void) const noexcept {
        return m_local_penalty_coefficient_less;
    }

    /*************************************************************************/
    inline double &local_penalty_coefficient_greater(void) noexcept {
        return m_local_penalty_coefficient_greater;
    }
    /*************************************************************************/
    inline double local_penalty_coefficient_greater(void) const noexcept {
        return m_local_penalty_coefficient_greater;
    }

    /*************************************************************************/
    inline double &global_penalty_coefficient(void) noexcept {
        return m_global_penalty_coefficient;
    }

    /*************************************************************************/
    inline double global_penalty_coefficient(void) const noexcept {
        return m_global_penalty_coefficient;
    }

    /*************************************************************************/
    inline void reset_local_penalty_coefficient(void) noexcept {
        m_local_penalty_coefficient_less    = m_global_penalty_coefficient;
        m_local_penalty_coefficient_greater = m_global_penalty_coefficient;
    }

    /*************************************************************************/
    inline void limit_local_penalty_coefficient(void) noexcept {
        m_local_penalty_coefficient_less = std::min(
            m_local_penalty_coefficient_less, m_global_penalty_coefficient);
        m_local_penalty_coefficient_greater = std::min(
            m_local_penalty_coefficient_greater, m_global_penalty_coefficient);
    }

    /*************************************************************************/
    inline Variable<T_Variable, T_Expression> *key_variable_ptr(void) const {
        return const_cast<Variable<T_Variable, T_Expression> *>(
            m_key_variable_ptr);
    }

    /*************************************************************************/
    inline void increment_violation_count(void) noexcept {
        m_violation_count++;
    }

    /*************************************************************************/
    inline void reset_violation_count(void) noexcept {
        m_violation_count = 0;
    }

    /*************************************************************************/
    inline long violation_count(void) const noexcept {
        return m_violation_count;
    }

    /*************************************************************************/
    inline bool is_user_defined_selection(void) const noexcept {
        return m_is_user_defined_selection;
    }

    /*************************************************************************/
    inline void set_is_user_defined_selection(
        const bool a_IS_USER_DEFINED_SELECTION) noexcept {
        m_is_user_defined_selection = a_IS_USER_DEFINED_SELECTION;
    }

    /*************************************************************************/
    inline bool has_only_binary_coefficient(void) const noexcept {
        return m_has_only_binary_coefficient;
    }

    /*************************************************************************/
    inline bool has_only_binary_variable(void) const noexcept {
        return m_has_only_binary_variable;
    }

    /*************************************************************************/
    inline bool is_evaluation_ignorable(void) const noexcept {
        return !m_is_enabled || m_has_margin;
    }

    /*************************************************************************/
    inline bool is_integer(void) const noexcept {
        return m_is_integer;
    }

    /*************************************************************************/
    inline void enable(void) noexcept {
        m_is_enabled = true;
    }

    /*************************************************************************/
    inline void disable(void) noexcept {
        m_is_enabled = false;
    }

    /*************************************************************************/
    inline bool is_enabled(void) const noexcept {
        return m_is_enabled;
    }

    /*************************************************************************/
    inline bool is_less_or_equal(void) const noexcept {
        return m_is_less_or_equal;
    }

    /*************************************************************************/
    inline bool is_greater_or_equal(void) const noexcept {
        return m_is_greater_or_equal;
    }

    /*************************************************************************/
    inline void set_is_selection(const bool a_IS_SELECTION) noexcept {
        m_is_selection = a_IS_SELECTION;
    }

    /*************************************************************************/
    inline bool is_selection(void) const noexcept {
        return m_is_selection;
    }

    /*************************************************************************/
    inline bool has_margin(void) const noexcept {
        return m_has_margin;
    }

    /*************************************************************************/
    inline bool is_type(const ConstraintType &a_TYPE) const noexcept {
        return m_type == a_TYPE;
    }

    /*************************************************************************/
    inline ConstraintType type(void) const noexcept {
        return m_type;
    }

    /*************************************************************************/
    inline std::string type_label(void) const noexcept {
        return ConstraintTypeInverseMap[m_type];
    }
};
using IPConstraint = Constraint<int, double>;
}  // namespace printemps::model_component
#endif
/*****************************************************************************/
// END
/*****************************************************************************/