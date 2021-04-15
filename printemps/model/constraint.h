/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MODEL_CONSTRAINT_H__
#define PRINTEMPS_MODEL_CONSTRAINT_H__

#include "../multi_array/abstract_multi_array_element.h"

namespace printemps {
namespace neighborhood {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct Move;
}  // namespace neighborhood
}  // namespace printemps

namespace printemps {
namespace model {
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
    ConstraintSense                      m_sense;
    T_Expression                         m_constraint_value;
    T_Expression                         m_violation_value;
    T_Expression                         m_positive_part;
    T_Expression                         m_negative_part;
    bool                                 m_is_linear;
    bool                                 m_is_enabled;
    bool                                 m_is_less_or_equal;     /// <= or ==
    bool                                 m_is_greater_or_equal;  /// >= or ==

    double m_local_penalty_coefficient_less;
    double m_local_penalty_coefficient_greater;
    double m_global_penalty_coefficient;

    bool m_is_singleton;
    bool m_is_aggregation;
    bool m_is_precedence;
    bool m_is_variable_bound;
    bool m_is_set_partitioning;
    bool m_is_set_packing;
    bool m_is_set_covering;
    bool m_is_cardinality;
    bool m_is_invariant_knapsack;
    bool m_is_equation_knapsack;
    bool m_is_bin_packing;
    bool m_is_knapsack;
    bool m_is_integer_knapsack;
    bool m_is_min_max;
    bool m_is_max_min;
    bool m_is_intermediate;
    bool m_is_general_linear;
    bool m_is_zero_one_coefficient;

    Variable<T_Variable, T_Expression> *m_intermediate_variable_ptr;

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
        m_is_enabled                        = true;
        m_is_less_or_equal                  = false;
        m_is_greater_or_equal               = false;
        m_local_penalty_coefficient_less    = HUGE_VALF;
        m_local_penalty_coefficient_greater = HUGE_VALF;
        m_global_penalty_coefficient        = HUGE_VALF;

        this->clear_constraint_type();
    }

    /*************************************************************************/
    inline constexpr void clear_constraint_type(void) {
        m_is_singleton            = false;
        m_is_aggregation          = false;
        m_is_precedence           = false;
        m_is_variable_bound       = false;
        m_is_set_partitioning     = false;
        m_is_set_packing          = false;
        m_is_set_covering         = false;
        m_is_cardinality          = false;
        m_is_invariant_knapsack   = false;
        m_is_equation_knapsack    = false;
        m_is_bin_packing          = false;
        m_is_knapsack             = false;
        m_is_integer_knapsack     = false;
        m_is_min_max              = false;
        m_is_max_min              = false;
        m_is_intermediate         = false;
        m_is_general_linear       = false;
        m_is_zero_one_coefficient = false;

        m_intermediate_variable_ptr = nullptr;
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
        m_is_enabled       = true;

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

        /// Aggregation
        if (m_expression.sensitivities().size() == 2 &&
            m_sense == ConstraintSense::Equal) {
            m_is_aggregation = true;
            return;
        }

        /// Precedence or Variable Bound
        if (m_expression.sensitivities().size() == 2 &&
            m_sense != ConstraintSense::Equal) {
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
             * At least one decision variables of the two must be binary
             * according to the definition of Variable Bound proposed by MIPLIB
             * 2017. But for convenience of the neighborhood definition, both of
             * them can be integer(non-binary) variable.
             */
            m_is_variable_bound = true;
            return;
        }

        /// Set Partitioning or Set Packing, Set Covering, Cardinality,
        /// Invariant Knapsack.
        {
            bool is_zero_one_coefficient_constraint = true;
            for (const auto &sensitivity : m_expression.sensitivities()) {
                if ((sensitivity.first->sense() != VariableSense::Binary) &&
                    (sensitivity.first->sense() != VariableSense::Selection)) {
                    is_zero_one_coefficient_constraint = false;
                    break;
                }
                if (sensitivity.second != 1) {
                    is_zero_one_coefficient_constraint = false;
                    break;
                }
            }
            if (is_zero_one_coefficient_constraint) {
                m_is_zero_one_coefficient = true;
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
            } else {
                m_is_zero_one_coefficient = false;
            }
        }

        /// Equation knapsack, or Binpacking, Knapsack
        {
            bool has_only_binary_variables = true;
            bool has_bin_packing_variable  = false;

            for (const auto &sensitivity : m_expression.sensitivities()) {
                if ((sensitivity.first->sense() != VariableSense::Binary) &&
                    (sensitivity.first->sense() != VariableSense::Selection)) {
                    has_only_binary_variables = false;
                    break;
                }
                if (sensitivity.second == -m_expression.constant_value()) {
                    has_bin_packing_variable = true;
                }
            }

            if (has_only_binary_variables) {
                /// Equation knapsack
                if (m_expression.constant_value() <= -2 &&
                    m_sense == ConstraintSense::Equal) {
                    m_is_equation_knapsack = true;
                    return;
                }

                /// Bin Packing
                if (has_bin_packing_variable &&
                    ((m_expression.constant_value() <= -2 &&
                      m_sense == ConstraintSense::Less) ||
                     (m_expression.constant_value() >= 2 &&
                      m_sense == ConstraintSense::Greater))) {
                    m_is_bin_packing = true;
                    return;
                }

                /// Knapsack
                if ((m_expression.constant_value() <= -2 &&
                     m_sense == ConstraintSense::Less) ||
                    (m_expression.constant_value() >= 2 &&
                     m_sense == ConstraintSense::Greater)) {
                    m_is_knapsack = true;
                    return;
                }
            }
        }

        /// Integer Knapsack
        if ((m_expression.constant_value() < 0 &&
             m_sense == ConstraintSense::Less) ||
            (m_expression.constant_value() > 0 &&
             m_sense == ConstraintSense::Greater)) {
            m_is_integer_knapsack = true;
            return;
        }

        /// Min-Max, Max-Min, Intermediate
        {
            int                                 number_of_integer_variables = 0;
            Variable<T_Variable, T_Expression> *variable_ptr = nullptr;
            T_Expression                        coefficient  = 0;

            auto is_integer = [](const T_Expression a_VALUE) {
                return fabs(a_VALUE - floor(a_VALUE)) < constant::EPSILON_10;
            };

            auto constant_value = m_expression.constant_value();

            if (is_integer(constant_value)) {
                T_Expression lower_bound = constant_value;
                T_Expression upper_bound = constant_value;
                bool         is_valid    = true;

                for (const auto &item : m_expression.sensitivities()) {
                    if (!is_integer(item.second)) {
                        is_valid = false;
                        break;
                    }

                    if (item.first->sense() == VariableSense::Integer &&
                        abs(item.second) == 1) {
                        variable_ptr = item.first;
                        coefficient  = item.second;
                        number_of_integer_variables++;
                    } else if (item.first->sense() == VariableSense::Binary ||
                               item.first->sense() == VariableSense::Integer) {
                        if (item.second > 0) {
                            upper_bound +=
                                item.second * item.first->upper_bound();
                            lower_bound -=
                                item.second * item.first->lower_bound();
                        } else {
                            lower_bound +=
                                item.second * item.first->upper_bound();
                            upper_bound -=
                                item.second * item.first->lower_bound();
                        }
                    } else {
                        is_valid = false;
                        break;
                    }
                    if (number_of_integer_variables > 1) {
                        break;
                    }
                }

                if (is_valid && number_of_integer_variables == 1) {
                    if (coefficient > 0) {
                        std::swap(lower_bound, upper_bound);
                        lower_bound *= -1.0;
                        upper_bound *= -1.0;
                    }

                    if ((variable_ptr->lower_bound() ==
                             constant::INT_HALF_MIN ||
                         variable_ptr->lower_bound() <= lower_bound) &&
                        (variable_ptr->upper_bound() ==
                             constant::INT_HALF_MAX ||
                         variable_ptr->upper_bound() >= upper_bound)) {
                        if ((m_sense == ConstraintSense::Less &&
                             coefficient < 0) ||
                            (m_sense == ConstraintSense::Greater &&
                             coefficient > 0)) {
                            m_is_min_max = true;
                            return;
                        }
                        if ((m_sense == ConstraintSense::Greater &&
                             coefficient < 0) ||
                            (m_sense == ConstraintSense::Less &&
                             coefficient > 0)) {
                            m_is_max_min = true;
                            return;
                        }
                        if (m_sense == ConstraintSense::Equal) {
                            m_is_intermediate           = true;
                            m_intermediate_variable_ptr = variable_ptr;
                            return;
                        }
                    }
                }
            }
        }

        /// Otherwise, the constraint type is set to general linear.
        m_is_general_linear = true;
    }

    /*************************************************************************/
    inline constexpr T_Expression evaluate_constraint(void) const noexcept {
#ifdef _MPS_SOLVER
        return m_expression.evaluate({});
#else
        return m_constraint_function({});
#endif
    }

    /*************************************************************************/
    inline constexpr T_Expression evaluate_constraint(
        const neighborhood::Move<T_Variable, T_Expression> &a_MOVE)
        const noexcept {
#ifdef _MPS_SOLVER
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
        const neighborhood::Move<T_Variable, T_Expression> &a_MOVE)
        const noexcept {
        return m_violation_function(a_MOVE);
    }

    /*************************************************************************/
    inline constexpr T_Expression evaluate_violation_diff(
        const neighborhood::Move<T_Variable, T_Expression> &a_MOVE)
        const noexcept {
        return m_violation_function(a_MOVE) - m_violation_value;
    }

    /*************************************************************************/
    inline constexpr void update(void) {
        /**
         * m_expression must be updated at first.
         */
#ifdef _MPS_SOLVER
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

#ifdef _MPS_SOLVER
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
    inline constexpr double &local_penalty_coefficient_less(void) noexcept {
        return m_local_penalty_coefficient_less;
    }

    /*************************************************************************/
    inline constexpr double local_penalty_coefficient_less(
        void) const noexcept {
        return m_local_penalty_coefficient_less;
    }

    /*************************************************************************/
    inline constexpr double &local_penalty_coefficient_greater(void) noexcept {
        return m_local_penalty_coefficient_greater;
    }
    /*************************************************************************/
    inline constexpr double local_penalty_coefficient_greater(
        void) const noexcept {
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
    inline constexpr bool is_linear(void) const noexcept {
        return m_is_linear;
    }

    /*************************************************************************/
    inline constexpr bool is_singleton(void) const noexcept {
        return m_is_singleton;
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
    inline constexpr bool is_general_linear(void) const noexcept {
        return m_is_general_linear;
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
    inline constexpr bool is_zero_one_coefficient(void) const noexcept {
        return m_is_zero_one_coefficient;
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
    inline constexpr Variable<T_Variable, T_Expression>
        *intermediate_variable_ptr(void) const {
        return const_cast<Variable<T_Variable, T_Expression> *>(
            m_intermediate_variable_ptr);
    }
};
using IPConstraint = Constraint<int, double>;
}  // namespace model
}  // namespace printemps
#endif
/*****************************************************************************/
// END
/*****************************************************************************/