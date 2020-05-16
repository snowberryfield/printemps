/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef CPPMH_MODEL_CONSTRAINT_H__
#define CPPMH_MODEL_CONSTRAINT_H__

#include <functional>
#include <vector>

#include "abstract_multi_array_element.h"
#include "move.h"

namespace cppmh {
namespace model {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Expression;

/*****************************************************************************/
enum class ConstraintSense {
    Lower,  // <= target
    Equal,  // = target
    Upper   // >= target
};

/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Constraint : public AbstractMultiArrayElement {
    /**
     * [Access controls for special member functions]
     *  -- Default constructor : default, private
     *  -- Copy constructor    : default, private
     *  -- Copy assignment     : user-defined, public
     *  -- Move constructor    : default, public
     *  -- Move assignment     : user-defined, public
     */

   public:
    std::function<T_Expression(const Move<T_Variable, T_Expression> &)>
        m_function;

    std::function<T_Expression(const Move<T_Variable, T_Expression> &)>
        m_constraint_function;

    std::function<T_Expression(const Move<T_Variable, T_Expression> &)>
        m_violation_function;

    Expression<T_Variable, T_Expression> m_expression;
    ConstraintSense                      m_sense;
    T_Expression                         m_constraint_value;
    T_Expression                         m_violation_value;
    bool                                 m_is_linear;
    bool                                 m_is_enabled;

    /*************************************************************************/
    /// Default constructor
    Constraint(void) {
        this->initialize();
    }

    /*************************************************************************/
    /// Copy constructor
    Constraint(const Constraint<T_Variable, T_Expression> &) = default;

    /*************************************************************************/
    Constraint(const std::function<T_Expression(
                   const Move<T_Variable, T_Expression> &)> &a_FUNCTION,
               const ConstraintSense                         a_SENSE) {
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
        if (a_constraint.m_is_linear) {
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
    create_instance(const std::function<T_Expression(
                        const Move<T_Variable, T_Expression> &)> &a_FUNCTION,
                    const ConstraintSense                         a_SENSE) {
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
    inline constexpr void initialize(void) {
        AbstractMultiArrayElement::initialize();

        m_function =  //
            []([[maybe_unused]] const Move<T_Variable, T_Expression> &a_MOVE) {
                return static_cast<T_Expression>(0);
            };
        m_constraint_function =
            []([[maybe_unused]] const Move<T_Variable, T_Expression> &a_MOVE) {
                return static_cast<T_Expression>(0);
            };
        m_violation_function =
            []([[maybe_unused]] const Move<T_Variable, T_Expression> &a_MOVE) {
                return static_cast<T_Expression>(0);
            };

        m_expression.initialize();
        m_sense            = ConstraintSense::Lower;
        m_constraint_value = 0;
        m_violation_value  = 0;
        m_is_linear        = true;
        m_is_enabled       = true;
    }

    /*************************************************************************/
    inline constexpr void setup(
        const std::function<
            T_Expression(const Move<T_Variable, T_Expression> &)> &a_FUNCTION,
        const ConstraintSense                                      a_SENSE) {
        this->initialize();

        m_function  = a_FUNCTION;
        m_sense     = a_SENSE;
        m_is_linear = false;

        m_constraint_function =
            [this](const Move<T_Variable, T_Expression> &a_MOVE) {
                return m_function(a_MOVE);
            };

        switch (m_sense) {
            case ConstraintSense::Lower: {
                m_violation_function =
                    [this](const Move<T_Variable, T_Expression> &a_MOVE) {
                        return std::max(m_function(a_MOVE),
                                        static_cast<T_Expression>(0));
                    };
                break;
            }
            case ConstraintSense::Equal: {
                m_violation_function =
                    [this](const Move<T_Variable, T_Expression> &a_MOVE) {
                        return std::abs(m_function(a_MOVE));
                    };
                break;
            }
            case ConstraintSense::Upper: {
                m_violation_function =
                    [this](const Move<T_Variable, T_Expression> &a_MOVE) {
                        return std::max(-m_function(a_MOVE),
                                        static_cast<T_Expression>(0));
                    };
                break;
            }
            default: {
                break;
            }
        }
    }

    /*************************************************************************/
    inline constexpr void setup(
        const Expression<T_Variable, T_Expression> &a_EXPRESSION,
        const ConstraintSense                       a_SENSE) {
        this->initialize();

        m_expression = a_EXPRESSION;
        m_sense      = a_SENSE;
        m_is_linear  = true;

        m_expression.setup_fixed_sensitivities();

        m_constraint_function =
            [this](const Move<T_Variable, T_Expression> &a_MOVE) {
                return m_expression.evaluate(a_MOVE);
            };

        switch (m_sense) {
            case ConstraintSense::Lower: {
                m_violation_function =
                    [this](const Move<T_Variable, T_Expression> &a_MOVE) {
                        return std::max(m_expression.evaluate(a_MOVE),
                                        static_cast<T_Expression>(0));
                    };
                break;
            }
            case ConstraintSense::Equal: {
                m_violation_function =
                    [this](const Move<T_Variable, T_Expression> &a_MOVE) {
                        return std::abs(m_expression.evaluate(a_MOVE));
                    };
                break;
            }
            case ConstraintSense::Upper: {
                m_violation_function =
                    [this](const Move<T_Variable, T_Expression> &a_MOVE) {
                        return std::max(-m_expression.evaluate(a_MOVE),
                                        static_cast<T_Expression>(0));
                    };
                break;
            }
            default: {
                break;
            }
        }
    }

    /*************************************************************************/
    inline constexpr T_Expression evaluate_constraint(void) const noexcept {
        return m_constraint_function({});
    }

    /*************************************************************************/
    inline constexpr T_Expression evaluate_constraint(
        const Move<T_Variable, T_Expression> &a_MOVE) const noexcept {
        return m_constraint_function(a_MOVE);
    }

    /*************************************************************************/
    inline constexpr T_Expression evaluate_violation(void) const noexcept {
        return m_violation_function({});
    }

    /*************************************************************************/
    inline constexpr T_Expression evaluate_violation(
        const Move<T_Variable, T_Expression> &a_MOVE) const noexcept {
        return m_violation_function(a_MOVE);
    }

    /*************************************************************************/
    inline constexpr void update(void) {
        /**
         * m_expression must be updated at first.
         */
        if (m_is_linear) {
            m_expression.update();
        }
        m_constraint_value = m_constraint_function({});
        m_violation_value  = m_violation_function({});
    }

    /*************************************************************************/
    inline constexpr void update(const Move<T_Variable, T_Expression> &a_MOVE) {
        /**
         * m_expression must be updated after m_constraint_value and
         * m_violation.
         */
        m_constraint_value = m_constraint_function(a_MOVE);
        m_violation_value  = m_violation_function(a_MOVE);

        if (m_is_linear) {
            m_expression.update(a_MOVE);
        }
    }

    /*************************************************************************/
    inline constexpr Expression<T_Variable, T_Expression> &expression(void) {
        return m_expression;
    }

    /*************************************************************************/
    inline constexpr const Expression<T_Variable, T_Expression> &expression(
        void) const {
        return m_expression;
    }

    /*************************************************************************/
    inline constexpr ConstraintSense sense(void) const {
        return m_sense;
    }

    /*************************************************************************/
    inline constexpr T_Expression constraint_value(void) const {
        return m_constraint_value;
    }

    /*************************************************************************/
    inline constexpr T_Expression violation_value(void) const {
        return m_violation_value;
    }

    /*************************************************************************/
    inline constexpr bool is_linear(void) const {
        return m_is_linear;
    }

    /*************************************************************************/
    inline constexpr bool is_enabled(void) const {
        return m_is_enabled;
    }

    /*************************************************************************/
    inline constexpr void enable(void) {
        m_is_enabled = true;
    }

    /*************************************************************************/
    inline constexpr void disable(void) {
        m_is_enabled = false;
    }
};
using IPConstraint = Constraint<int, double>;
}  // namespace model
}  // namespace cppmh
#endif
/*****************************************************************************/
// END
/*****************************************************************************/