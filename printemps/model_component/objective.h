/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MODEL_COMPONENT_OBJECTIVE_H__
#define PRINTEMPS_MODEL_COMPONENT_OBJECTIVE_H__

namespace printemps::neighborhood {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct Move;
}  // namespace printemps::neighborhood

namespace printemps::model {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Model;
}  // namespace printemps::model

namespace printemps::model_component {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Expression;

/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Objective {
    /**
     * [Access controls for special member functions]
     *  -- Default constructor : default, private
     *  -- Copy constructor    : default, private
     *  -- Copy assignment     : user-defined, public
     *  -- Move constructor    : default, public
     *  -- Move assignment     : user-defined, public
     */

    /// These friend declarations are required to have Expression object as a
    /// member;
    friend class model::Model<T_Variable, T_Expression>;

   private:
    Expression<T_Variable, T_Expression> m_expression;
    T_Expression                         m_value;

    /*************************************************************************/
    /// Default constructor
    Objective(void) {
        this->initialize();
    }

    /*************************************************************************/
    /// Copy constructor
    Objective(const Objective &);

    /*************************************************************************/
    Objective(const Expression<T_Variable, T_Expression> &a_EXPRESSION) {
        this->setup(a_EXPRESSION);
    }

   public:
    /*************************************************************************/
    /// Copy assignment
    Objective<T_Variable, T_Expression> &operator=(
        const Objective<T_Variable, T_Expression> &a_OBJECTIVE) {
        this->setup(a_OBJECTIVE.m_expression);
        return *this;
    }

    /*************************************************************************/
    /// Move constructor
    Objective(Objective<T_Variable, T_Expression> &&) = default;

    /*************************************************************************/
    /// Move assignment
    Objective<T_Variable, T_Expression> &operator=(
        Objective<T_Variable, T_Expression> &&a_objective) {
        this->setup(a_objective.m_expression);
        return *this;
    }

    /*************************************************************************/
    inline static Objective<T_Variable, T_Expression> create_instance(void) {
        /**
         * When instantiation, instead of constructor, create_instance() should
         * be called.
         */
        Objective<T_Variable, T_Expression> objective;
        return objective;
    }

    /*************************************************************************/
    inline static Objective<T_Variable, T_Expression> create_instance(
        const Expression<T_Variable, T_Expression> &a_EXPRESSION) {
        /**
         * When instantiation, instead of constructor, create_instance() should
         * be called.
         */
        Objective<T_Variable, T_Expression> objective(a_EXPRESSION);
        return objective;
    }

    /*************************************************************************/
    void initialize(void) {
        m_expression.initialize();
        m_value = 0;
    }

    /*************************************************************************/
    void setup(const Expression<T_Variable, T_Expression> &a_EXPRESSION) {
        this->initialize();
        m_expression = a_EXPRESSION;
    }

    /*************************************************************************/
    inline T_Expression evaluate(void) const noexcept {
        return m_expression.evaluate();
    }

    /*************************************************************************/
    inline T_Expression evaluate(
        const neighborhood::Move<T_Variable, T_Expression> &a_MOVE) const
        noexcept {
        return m_expression.evaluate(a_MOVE);
    }

    /*************************************************************************/
    inline void update(void) {
        m_expression.update();
        m_value = m_expression.value();
    }

    /*************************************************************************/
    inline void update(
        const neighborhood::Move<T_Variable, T_Expression> &a_MOVE) {
        m_expression.update(a_MOVE);
        m_value = m_expression.value();
    }

    /*************************************************************************/
    inline Expression<T_Variable, T_Expression> &expression(void) {
        return m_expression;
    }

    /*************************************************************************/
    inline const Expression<T_Variable, T_Expression> &expression(void) const {
        return m_expression;
    }

    /*************************************************************************/
    inline T_Expression value(void) const {
        return m_value;
    }
};
using IPObjective = Objective<int, double>;
}  // namespace printemps::model_component
#endif
/*****************************************************************************/
// END
/*****************************************************************************/