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
     *  -- Copy constructor    : delete, (private)
     *  -- Copy assignment     : user-defined, public
     *  -- Move constructor    : default, public
     *  -- Move assignment     : user-defined, public
     */
    friend class ConstraintTypeClassifier<T_Variable, T_Expression>;

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
    bool m_has_only_binary_or_selection_variable;

    ConstraintType m_type;

    std::unique_ptr<ExpressionStructure<T_Variable, T_Expression>> m_structure;

    /*************************************************************************/
    /// Default constructor
    Constraint(void) {
        this->initialize();
    }

    /*************************************************************************/
    /// Copy constructor
    Constraint(const Constraint<T_Variable, T_Expression> &) = delete;

    /*************************************************************************/
    Constraint(const Expression<T_Variable, T_Expression> &a_EXPRESSION,
               const ConstraintSense                       a_SENSE) {
        this->initialize();
        this->setup(a_EXPRESSION, a_SENSE);
    }

   public:
    /*************************************************************************/
    /// Copy assignment
    Constraint<T_Variable, T_Expression> &operator=(
        const Constraint<T_Variable, T_Expression> &a_CONSTRAINT) {
        this->initialize();
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
        m_sense            = ConstraintSense::Less;
        m_constraint_value = 0;
        m_violation_value  = 0;
        m_margin_value     = 0;
        m_positive_part    = 0;
        m_negative_part    = 0;

        m_local_penalty_coefficient_less    = HUGE_VALF;
        m_local_penalty_coefficient_greater = HUGE_VALF;
        m_global_penalty_coefficient        = HUGE_VALF;

        m_key_variable_ptr = nullptr;
        m_violation_count = 0;

        m_is_enabled                     = true;
        m_is_less_or_equal               = false;
        m_is_greater_or_equal            = false;
        m_is_selection                   = false;
        m_has_margin                     = false;
        m_is_user_defined_selection      = false;

        m_type = ConstraintType::Unknown;
        m_key_variable_ptr = nullptr;

        m_structure =
            std::make_unique<ExpressionStructure<T_Variable, T_Expression>>();
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

        this->update_structure();
        this->update_constraint_type();
    }

    /*************************************************************************/
    inline void update_structure(void) {
        *m_structure = m_expression.structure();

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
        /**
         * Skip updating the constraint type if it has already been extracted as
         * a dependency-defining constraint (and thus deactivated).
         */
        if (m_key_variable_ptr != nullptr && !m_is_enabled &&
            (m_key_variable_ptr->type() == VariableType::DependentBinary ||
             m_key_variable_ptr->type() == VariableType::DependentInteger)) {
            return;
        }

        ConstraintTypeClassifier classifier(this);
        auto [type, key_variable_ptr] = classifier.classify_type();

        m_type             = type;
        m_key_variable_ptr = key_variable_ptr;
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
        m_has_margin = m_structure->has_only_binary_or_selection_variable &
                       (m_margin_value >= m_structure->max_abs_coefficient -
                                              constant::EPSILON_10);
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
        m_has_margin = m_structure->has_only_binary_or_selection_variable &
                       (m_margin_value >= m_structure->max_abs_coefficient -
                                              constant::EPSILON_10);
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
    inline bool is_evaluation_ignorable(void) const noexcept {
        return !m_is_enabled || m_has_margin;
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

    /*************************************************************************/
    inline ExpressionStructure<T_Variable, T_Expression> &structure(
        void) const noexcept {
        return *m_structure;
    }
};
using IPConstraint = Constraint<int, double>;
}  // namespace printemps::model_component
#endif
/*****************************************************************************/
// END
/*****************************************************************************/