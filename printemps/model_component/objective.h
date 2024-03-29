/*****************************************************************************/
// Copyright (c) 2020-2023 Yuji KOGUMA
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
    std::function<T_Expression(
        const neighborhood::Move<T_Variable, T_Expression> &)>
        m_function;

    Expression<T_Variable, T_Expression> m_expression;
    T_Expression                         m_value;
    bool                                 m_is_linear;

    /*************************************************************************/
    /// Default constructor
    Objective(void) {
        this->initialize();
    }

    /*************************************************************************/
    /// Copy constructor
    Objective(const Objective &);

    /*************************************************************************/
    Objective(const std::function<T_Expression(
                  const neighborhood::Move<T_Variable, T_Expression> &)>
                  &a_FUNCTION) {
        this->setup(a_FUNCTION);
    }

    /*************************************************************************/
    Objective(const Expression<T_Variable, T_Expression> &a_EXPRESSION) {
        this->setup(a_EXPRESSION);
    }

   public:
    /*************************************************************************/
    /// Copy assignment
    Objective<T_Variable, T_Expression> &operator=(
        const Objective<T_Variable, T_Expression> &a_OBJECTIVE) {
        if (a_OBJECTIVE.m_is_linear) {
            this->setup(a_OBJECTIVE.m_expression);
        } else {
            this->setup(a_OBJECTIVE.m_function);
        }
        return *this;
    }

    /*************************************************************************/
    /// Move constructor
    Objective(Objective<T_Variable, T_Expression> &&) = default;

    /*************************************************************************/
    /// Move assignment
    Objective<T_Variable, T_Expression> &operator=(
        Objective<T_Variable, T_Expression> &&a_objective) {
        if (a_objective.m_is_linear) {
            this->setup(a_objective.m_expression);
        } else {
            this->setup(a_objective.m_function);
        }
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
        const std::function<
            T_Expression(const neighborhood::Move<T_Variable, T_Expression> &)>
            &a_FUNCTION) {
        /**
         * When instantiation, instead of constructor, create_instance() should
         * be called.
         */
        Objective<T_Variable, T_Expression> objective(a_FUNCTION);
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
        m_function = []([[maybe_unused]] const neighborhood::Move<
                         T_Variable, T_Expression> &a_MOVE) { return 0.0; };
        m_expression.initialize();
        m_value     = 0;
        m_is_linear = true;
    }

    /*************************************************************************/
    void setup(const std::function<T_Expression(
                   const neighborhood::Move<T_Variable, T_Expression> &)>
                   &a_FUNCTION) {
        this->initialize();
        m_is_linear = false;
        m_function  = a_FUNCTION;
    }

    /*************************************************************************/
    void setup(const Expression<T_Variable, T_Expression> &a_EXPRESSION) {
        this->initialize();
        m_is_linear  = true;
        m_expression = a_EXPRESSION;
    }

    /*************************************************************************/
    inline T_Expression evaluate(void) const noexcept {
#ifdef _PRINTEMPS_LINEAR_MINIMIZATION
        return m_expression.evaluate();
#else
        if (m_is_linear) {
            return m_expression.evaluate();
        } else {
            return m_function({});
        }
#endif
    }

    /*************************************************************************/
    inline T_Expression evaluate(
        const neighborhood::Move<T_Variable, T_Expression> &a_MOVE) const
        noexcept {
#ifdef _PRINTEMPS_LINEAR_MINIMIZATION
        return m_expression.evaluate(a_MOVE);
#else
        if (m_is_linear) {
            return m_expression.evaluate(a_MOVE);
        } else {
            return m_function(a_MOVE);
        }
#endif
    }

    /*************************************************************************/
    inline void update(void) {
#ifdef _PRINTEMPS_LINEAR_MINIMIZATION
        m_expression.update();
        m_value = m_expression.value();
#else
        if (m_is_linear) {
            m_expression.update();
            m_value = m_expression.value();
        } else {
            m_value = m_function({});
        }
#endif
    }

    /*************************************************************************/
    inline void update(
        const neighborhood::Move<T_Variable, T_Expression> &a_MOVE) {
#ifdef _PRINTEMPS_LINEAR_MINIMIZATION
        m_expression.update(a_MOVE);
        m_value = m_expression.value();
#else
        if (m_is_linear) {
            m_expression.update(a_MOVE);
            m_value = m_expression.value();
        } else {
            m_value = m_function(a_MOVE);
        }
#endif
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

    /*************************************************************************/
    inline bool is_linear(void) const {
        return m_is_linear;
    }
};
using IPObjective = Objective<int, double>;
}  // namespace printemps::model_component
#endif
/*****************************************************************************/
// END
/*****************************************************************************/