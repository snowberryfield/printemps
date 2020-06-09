/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef CPPMH_MODEL_VARIABLE_H__
#define CPPMH_MODEL_VARIABLE_H__

#include <unordered_map>
#include <unordered_set>
#include "abstract_multi_array_element.h"

namespace cppmh {
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
enum class VariableSense {
    Selection,  //
    Binary,     //
    Integer     //
};

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
    VariableSense m_sense;

    Selection<T_Variable, T_Expression> *m_selection_ptr;
    std::unordered_set<Constraint<T_Variable, T_Expression> *>
        m_related_constraint_ptrs;

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
    inline constexpr void initialize(void) {
        AbstractMultiArrayElement::initialize();
        m_is_fixed = false;
        m_value    = 0;

        /**
         * The default lower and upper bounds of a decision variable are set by
         * sufficiently large negative and positive integer, respectively. The
         * default bounds have margin to avoid overflows in calculating moves.
         */
        m_lower_bound   = std::numeric_limits<int>::min() + 1;
        m_upper_bound   = std::numeric_limits<int>::max() - 1;
        m_has_bounds    = false;
        m_sense         = VariableSense::Integer;
        m_selection_ptr = nullptr;
        m_related_constraint_ptrs.clear();
    }

    /*************************************************************************/
    inline constexpr void set_value_force(const T_Variable a_VALUE) {
        m_value = a_VALUE;
    }

    /*************************************************************************/
    inline constexpr void set_value_if_not_fixed(const T_Variable a_VALUE) {
        if (!m_is_fixed) {
            m_value = a_VALUE;
        }
    }

    /*************************************************************************/
    inline constexpr T_Variable value(void) const noexcept {
        return m_value;
    }

    /*************************************************************************/
    inline constexpr void set_value(T_Variable a_VALUE) {
        if (m_is_fixed) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "A fixed variable was attempted to be changed."));
        }
        m_value = a_VALUE;
    }

    /*************************************************************************/
    inline constexpr T_Expression evaluate(void) const noexcept {
        return this->value();
    }

    /*************************************************************************/
    inline constexpr T_Expression evaluate(
        const Move<T_Variable, T_Expression> &a_MOVE) const noexcept {
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
    }

    /*************************************************************************/
    inline constexpr void reset_bound(void) {
        m_lower_bound   = std::numeric_limits<int>::min() + 1;
        m_upper_bound   = std::numeric_limits<int>::max() - 1;
        m_has_bounds    = false;
        m_sense         = VariableSense::Integer;
        m_selection_ptr = nullptr;
    }

    /*************************************************************************/
    inline constexpr T_Variable lower_bound(void) const {
        return m_lower_bound;
    }

    /*************************************************************************/
    inline constexpr T_Variable upper_bound(void) const {
        return m_upper_bound;
    }

    /*************************************************************************/
    inline constexpr bool has_bounds(void) const {
        return m_has_bounds;
    }

    /*************************************************************************/
    inline constexpr VariableSense sense(void) const {
        return m_sense;
    }

    /*************************************************************************/
    inline constexpr void setup_sense(void) {
        if (m_lower_bound == 0 && m_upper_bound == 1) {
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
        Constraint<T_Variable, T_Expression> *a_related_constraint_ptr) {
        m_related_constraint_ptrs.insert(a_related_constraint_ptr);
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
        if (m_is_fixed) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "A fixed variable was attempted to be changed."));
        }
        m_value = a_VALUE;
        return *this;
    }
};
using IPVariable = Variable<int, double>;
}  // namespace model
}  // namespace cppmh
#endif
/*****************************************************************************/
// END
/*****************************************************************************/
