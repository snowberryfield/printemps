/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MODEL_VARIABLE_H__
#define PRINTEMPS_MODEL_VARIABLE_H__

#include "abstract_multi_array_element.h"

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
class Constraint;

/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct Selection;

/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Variable : public AbstractMultiArrayElement {
    /**
     * [Access controls for special member functions]
     *  -- Default constructor : default, private
     *  -- Copy constructor    : default, private
     *  -- Copy assignment     : delete, (private)
     *  -- Move constructor    : default, public
     *  -- Move assignment     : default, public
     */

   private:
    bool          m_is_fixed;
    T_Variable    m_value;
    T_Variable    m_lower_bound;
    T_Variable    m_upper_bound;
    bool          m_has_bounds;
    bool          m_is_objective_improvable;
    bool          m_is_feasibility_improvable;
    bool          m_has_lower_bound_margin;
    bool          m_has_upper_bound_margin;
    VariableSense m_sense;

    Selection<T_Variable, T_Expression> *m_selection_ptr;
    std::unordered_set<Constraint<T_Variable, T_Expression> *>
        m_related_constraint_ptrs;

    std::unordered_set<Constraint<T_Variable, T_Expression> *>
        m_related_monic_constraint_ptrs;

    std::unordered_map<Constraint<T_Variable, T_Expression> *, T_Expression>
                 m_constraint_sensitivities;
    T_Expression m_objective_sensitivity;

    /*************************************************************************/
    /// Default constructor
    Variable(void) {
        this->initialize();
    }

    /*************************************************************************/
    /// Copy constructor
    Variable(const Variable<T_Variable, T_Expression> &) = default;

    /*************************************************************************/
    /// Copy assignment
    Variable<T_Variable, T_Expression> &operator    =(
        const Variable<T_Variable, T_Expression> &) = delete;

   public:
    /*************************************************************************/
    /// Move constructor
    Variable(Variable<T_Variable, T_Expression> &&) = default;

    /*************************************************************************/
    /// Move assignment
    Variable<T_Variable, T_Expression> &operator=(
        Variable<T_Variable, T_Expression> &&) = default;

    /*************************************************************************/
    inline static constexpr Variable<T_Variable, T_Expression> create_instance(
        void) {
        /**
         * When instantiation, instead of constructor, create_instance() should
         * be called.
         */
        Variable<T_Variable, T_Expression> variable;
        return variable;
    }

    /*************************************************************************/
    void initialize(void) {
        AbstractMultiArrayElement::initialize();
        m_is_fixed = false;
        m_value    = 0;

        /**
         * The default lower and upper bounds of a decision variable are set by
         * sufficiently large negative and positive integer, respectively. The
         * default bounds have margin to avoid overflows in calculating moves.
         */
        m_lower_bound = constant::INT_HALF_MIN;
        m_upper_bound = constant::INT_HALF_MAX;
        m_has_bounds  = false;

        m_is_objective_improvable   = false;
        m_is_feasibility_improvable = false;

        m_has_lower_bound_margin = true;
        m_has_upper_bound_margin = true;

        m_sense         = VariableSense::Integer;
        m_selection_ptr = nullptr;
        m_related_constraint_ptrs.clear();
        m_constraint_sensitivities.clear();
        m_objective_sensitivity = 0.0;
    }

    /*************************************************************************/
    inline constexpr void set_value_force(const T_Variable a_VALUE) {
        m_value = a_VALUE;
        this->update_margin();
    }

    /*************************************************************************/
    inline constexpr void set_value_if_mutable(const T_Variable a_VALUE) {
        if (!m_is_fixed) {
            m_value = a_VALUE;
            this->update_margin();
        }
    }

    /*************************************************************************/
    inline constexpr T_Variable value(void) const noexcept {
        return m_value;
    }

    /*************************************************************************/
    inline constexpr void set_value(T_Variable a_VALUE) {
        if (m_value != a_VALUE && m_is_fixed) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "A fixed variable was attempted to be changed."));
        }
        m_value = a_VALUE;
        this->update_margin();
    }

    /*************************************************************************/
    inline constexpr T_Expression evaluate(void) const noexcept {
        return this->value();
    }

    /*************************************************************************/
    inline constexpr T_Expression evaluate(
        const neighborhood::Move<T_Variable, T_Expression> &a_MOVE)
        const noexcept {
        if (a_MOVE.alterations.size() == 0) {
            return this->value();
        }

        for (const auto &alteration : a_MOVE.alterations) {
            if (this == alteration.first) {
                return alteration.second;
            }
        }
        return this->value();
    }

    /*************************************************************************/
    inline constexpr void fix(void) {
        m_is_fixed = true;
    }

    /*************************************************************************/
    inline constexpr void unfix(void) {
        m_is_fixed = false;
    }

    /*************************************************************************/
    inline constexpr bool is_fixed(void) const {
        return m_is_fixed;
    }

    /*************************************************************************/
    inline constexpr void fix_by(const T_Variable a_VALUE) {
        m_value    = a_VALUE;
        m_is_fixed = true;
        this->update_margin();
    }

    /*************************************************************************/
    inline constexpr void set_bound(const T_Variable a_LOWER_BOUND,
                                    const T_Variable a_UPPER_BOUND) {
        if (a_LOWER_BOUND > a_UPPER_BOUND) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The specified lower bound is bigger than the specified upper "
                "bound. lower bound: " +
                    std::to_string(a_LOWER_BOUND) +
                    ", upper bound: " + std::to_string(a_UPPER_BOUND)));
        }
        m_lower_bound = a_LOWER_BOUND;
        m_upper_bound = a_UPPER_BOUND;
        m_has_bounds  = true;

        this->setup_sense();
        this->update_margin();
    }

    /*************************************************************************/
    inline constexpr void reset_bound(void) {
        m_lower_bound   = constant::INT_HALF_MIN;
        m_upper_bound   = constant::INT_HALF_MAX;
        m_has_bounds    = false;
        m_sense         = VariableSense::Integer;
        m_selection_ptr = nullptr;
        this->update_margin();
    }

    /*************************************************************************/
    inline constexpr T_Variable lower_bound(void) const noexcept {
        return m_lower_bound;
    }

    /*************************************************************************/
    inline constexpr T_Variable upper_bound(void) const noexcept {
        return m_upper_bound;
    }

    /*************************************************************************/
    inline constexpr bool has_bounds(void) const {
        return m_has_bounds;
    }

    /*************************************************************************/
    inline constexpr void set_is_objective_improvable(
        const bool a_IS_OBJECTIVE_IMPROVABLE) noexcept {
        m_is_objective_improvable = a_IS_OBJECTIVE_IMPROVABLE;
    }

    /*************************************************************************/
    inline constexpr void set_is_feasibility_improvable(
        const bool a_IS_FEASIBILITY_IMPROVABLE) noexcept {
        m_is_feasibility_improvable = a_IS_FEASIBILITY_IMPROVABLE;
    }

    /*************************************************************************/
    inline constexpr bool is_objective_improvable(void) const noexcept {
        return m_is_objective_improvable;
    }

    /*************************************************************************/
    inline constexpr bool is_feasibility_improvable(void) const noexcept {
        return m_is_feasibility_improvable;
    }

    /*************************************************************************/
    inline constexpr VariableSense sense(void) const noexcept {
        return m_sense;
    }

    /*************************************************************************/
    inline constexpr void setup_sense(void) {
        if ((m_lower_bound == 0 && m_upper_bound == 1) ||
            (m_lower_bound == 0 && m_upper_bound == 0) ||
            (m_lower_bound == 1 && m_upper_bound == 1)) {
            m_sense = VariableSense::Binary;
        } else {
            m_sense = VariableSense::Integer;
        }
    }

    /*************************************************************************/
    inline constexpr void set_selection_ptr(
        Selection<T_Variable, T_Expression> *a_SELECTION) {
        m_selection_ptr = a_SELECTION;
        m_sense         = VariableSense::Selection;
    }

    /*************************************************************************/
    inline constexpr Selection<T_Variable, T_Expression> *selection_ptr(
        void) const {
        return m_selection_ptr;
    }

    /*************************************************************************/
    inline constexpr void select(void) const {
        m_selection_ptr->selected_variable_ptr = this->reference();
    }

    /*************************************************************************/
    inline constexpr void register_related_constraint_ptr(
        Constraint<T_Variable, T_Expression> *a_constraint_ptr) {
        m_related_constraint_ptrs.insert(a_constraint_ptr);
    }

    /*************************************************************************/
    inline constexpr void reset_related_constraint_ptrs(void) {
        m_related_constraint_ptrs.clear();
    }

    /*************************************************************************/
    inline constexpr std::unordered_set<Constraint<T_Variable, T_Expression> *>
        &related_constraint_ptrs(void) {
        return m_related_constraint_ptrs;
    }

    /*************************************************************************/
    inline constexpr const std::unordered_set<
        Constraint<T_Variable, T_Expression> *>
        &related_constraint_ptrs(void) const {
        return m_related_constraint_ptrs;
    }

    /*************************************************************************/
    inline constexpr void setup_related_monic_constraint_ptrs(void) {
        /**
         * NOTE: This method must be called after constraint categorization.
         */
        for (const auto &constraint_ptr : m_related_constraint_ptrs) {
            if (constraint_ptr->is_set_partitioning() ||
                constraint_ptr->is_set_packing() ||
                constraint_ptr->is_set_covering() ||
                constraint_ptr->is_cardinality() ||
                constraint_ptr->is_invariant_knapsack()) {
                m_related_monic_constraint_ptrs.insert(constraint_ptr);
            }
        }
    }

    /*************************************************************************/
    inline constexpr void reset_related_monic_constraint_ptrs(void) {
        m_related_monic_constraint_ptrs.clear();
    }

    /*************************************************************************/
    inline constexpr std::unordered_set<Constraint<T_Variable, T_Expression> *>
        &related_monic_constraint_ptrs(void) {
        return m_related_monic_constraint_ptrs;
    }

    /*************************************************************************/
    inline constexpr const std::unordered_set<
        Constraint<T_Variable, T_Expression> *>
        &related_monic_constraint_ptrs(void) const {
        return m_related_monic_constraint_ptrs;
    }

    /*************************************************************************/
    inline constexpr void register_constraint_sensitivity(
        Constraint<T_Variable, T_Expression> *a_constraint_ptr,
        const T_Expression                    a_SENSITIVITY) {
        m_constraint_sensitivities[a_constraint_ptr] = a_SENSITIVITY;
    }

    /*************************************************************************/
    inline constexpr void reset_constraint_sensitivities(void) {
        m_constraint_sensitivities.clear();
    }

    /*************************************************************************/
    inline constexpr std::unordered_map<Constraint<T_Variable, T_Expression> *,
                                        T_Expression>
        &constraint_sensitivities(void) {
        return m_constraint_sensitivities;
    }

    /*************************************************************************/
    inline constexpr const std::unordered_map<
        Constraint<T_Variable, T_Expression> *, T_Expression>
        &constraint_sensitivities(void) const {
        return m_constraint_sensitivities;
    }

    /*************************************************************************/
    inline constexpr void set_objective_sensitivity(
        const T_Expression a_SENSITIVITY) {
        m_objective_sensitivity = a_SENSITIVITY;
    }

    /*************************************************************************/
    inline constexpr T_Expression objective_sensitivity(void) const noexcept {
        return m_objective_sensitivity;
    }

    /*************************************************************************/
    inline constexpr void update_margin(void) {
        m_has_lower_bound_margin = m_value > m_lower_bound;
        m_has_upper_bound_margin = m_value < m_upper_bound;
    }

    /*************************************************************************/
    inline constexpr bool has_lower_bound_margin(void) const noexcept {
        return m_has_lower_bound_margin;
    }

    /*************************************************************************/
    inline constexpr bool has_upper_bound_margin(void) const noexcept {
        return m_has_upper_bound_margin;
    }

    /*************************************************************************/
    inline constexpr Expression<T_Variable, T_Expression> to_expression(
        void) const {
        std::unordered_map<Variable<T_Variable, T_Expression> *, T_Expression>
            sensitivity;

        sensitivity[this->reference()] = 1;
        return Expression<T_Variable, T_Expression>::create_instance(
            sensitivity, 0);
    }

    /*************************************************************************/
    inline constexpr Variable<T_Variable, T_Expression> *reference(void) const {
        return const_cast<Variable<T_Variable, T_Expression> *>(this);
    }

    /*************************************************************************/
    inline constexpr Expression<T_Variable, T_Expression> operator+(
        void) const {
        return this->to_expression();
    }

    /*************************************************************************/
    inline constexpr Expression<T_Variable, T_Expression> operator-(
        void) const {
        return -this->to_expression();
    }

    /*************************************************************************/
    inline constexpr Variable<T_Variable, T_Expression> &operator=(
        const T_Variable a_VALUE) {
        if (m_value != a_VALUE && m_is_fixed) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "A fixed variable was attempted to be changed."));
        }
        m_value = a_VALUE;
        this->update_margin();
        return *this;
    }
};
using IPVariable = Variable<int, double>;
}  // namespace model
}  // namespace printemps
#endif
/*****************************************************************************/
// END
/*****************************************************************************/
