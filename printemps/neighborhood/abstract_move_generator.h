/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_NEIGHBORHOOD_ABSTRACT_MOVE_GENERATOR_H__
#define PRINTEMPS_NEIGHBORHOOD_ABSTRACT_MOVE_GENERATOR_H__

namespace printemps::neighborhood {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class AbstractMoveGenerator {
   protected:
    std::function<void(std::vector<Move<T_Variable, T_Expression>> *,
                       std::vector<short> *, const bool, const bool, const bool,
                       const bool, const int)>
        m_move_updater;

    std::vector<Move<T_Variable, T_Expression>> m_moves;
    std::vector<short>                          m_flags;
    bool                                        m_is_enabled;

   public:
    /*************************************************************************/
    AbstractMoveGenerator(void) {
        this->initialize();
    }

    /*************************************************************************/
    void initialize(void) {
        m_move_updater = [](std::vector<Move<T_Variable, T_Expression>> *,  //
                            std::vector<short> *,                           //
                            const bool,                                     //
                            const bool,                                     //
                            const bool,                                     //
                            const bool,                                     //
                            const int) {};
        m_moves.clear();
        m_flags.clear();
        m_is_enabled = false;
    }

    /*************************************************************************/
    inline void update_moves(const bool a_ACCEPT_ALL,                     //
                             const bool a_ACCEPT_OBJECTIVE_IMPROVABLE,    //
                             const bool a_ACCEPT_FEASIBILITY_IMPROVABLE,  //
                             const bool a_IS_ENABLED_PARALLEL,            //
                             const int  a_NUMBER_OF_THREADS) {
        m_move_updater(&m_moves,                         //
                       &m_flags,                         //
                       a_ACCEPT_ALL,                     //
                       a_ACCEPT_OBJECTIVE_IMPROVABLE,    //
                       a_ACCEPT_FEASIBILITY_IMPROVABLE,  //
                       a_IS_ENABLED_PARALLEL,            //
                       a_NUMBER_OF_THREADS);
    }

    /*************************************************************************/
    inline std::vector<Move<T_Variable, T_Expression>> &moves(void) {
        return m_moves;
    }

    /*************************************************************************/
    inline const std::vector<Move<T_Variable, T_Expression>> &moves(
        void) const {
        return m_moves;
    }

    /*************************************************************************/
    inline void set_moves(
        const std::vector<Move<T_Variable, T_Expression>> &a_MOVES) {
        /**
         * NOTE: This method is for test.
         */
        m_moves = a_MOVES;
    }

    /*************************************************************************/
    inline std::vector<short> &flags(void) {
        return m_flags;
    }

    /*************************************************************************/
    inline const std::vector<short> &flags(void) const {
        return m_flags;
    }

    /*************************************************************************/
    inline void set_flags(const std::vector<short> &a_FLAGS) {
        /**
         * NOTE: This method is for test.
         */
        m_flags = a_FLAGS;
    }

    /*************************************************************************/
    inline bool is_enabled(void) const {
        return m_is_enabled;
    }

    /*************************************************************************/
    inline void enable(void) {
        m_is_enabled = true;
    }

    /*************************************************************************/
    inline void disable(void) {
        m_is_enabled = false;
    }

    /*************************************************************************/
    inline void reset_availability(void) {
        for (auto &&move : m_moves) {
            move.is_available = true;
        }
    }
};
/*****************************************************************************/
template <class T_Variable, class T_Expression>
inline std::vector<model_component::Variable<T_Variable, T_Expression> *>
extract_mutable_variable_ptrs(
    const std::vector<model_component::Variable<T_Variable, T_Expression> *>
        &a_RAW_VARIABLE_PTRS) {
    std::vector<model_component::Variable<T_Variable, T_Expression> *> results;
    for (auto &&variable_ptr : a_RAW_VARIABLE_PTRS) {
        if (!variable_ptr->is_fixed()) {
            results.push_back(variable_ptr);
        }
    }
    return results;
}

/*****************************************************************************/
template <class T_Variable, class T_Expression>
inline std::vector<model_component::Constraint<T_Variable, T_Expression> *>
extract_effective_constraint_ptrs(
    const std::vector<model_component::Constraint<T_Variable, T_Expression> *>
        &a_RAW_CONSTRAINT_PTRS) {
    std::vector<model_component::Constraint<T_Variable, T_Expression> *>
        results;
    for (const auto &constraint_ptr : a_RAW_CONSTRAINT_PTRS) {
        if (!constraint_ptr->is_enabled()) {
            continue;
        }

        bool  has_fixed_or_selection_variables = false;
        auto &expression                       = constraint_ptr->expression();
        for (const auto &sensitivity : expression.sensitivities()) {
            if (sensitivity.first->is_fixed() ||
                sensitivity.first->type() ==
                    model_component::VariableType::Selection) {
                has_fixed_or_selection_variables = true;
                break;
            }
        }

        if (!has_fixed_or_selection_variables) {
            results.push_back(constraint_ptr);
        }
    }
    return results;
}

/*****************************************************************************/
template <class T_Variable, class T_Expression>
inline std::vector<BinomialConstraint<T_Variable, T_Expression>>
convert_to_binomial_constraints(
    const std::vector<model_component::Constraint<T_Variable, T_Expression> *>
        &a_CONSTRAINT_PTRS) {
    std::vector<BinomialConstraint<T_Variable, T_Expression>> results;
    for (const auto &constraint_ptr : a_CONSTRAINT_PTRS) {
        auto &expression = constraint_ptr->expression();

        if (expression.sensitivities().size() != 2) {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The constraint is not binomial."));
        }

        auto vector_pair = utility::to_vector_pair(expression.sensitivities());

        if (vector_pair.first[0] > vector_pair.first[1]) {
            std::swap(vector_pair.first[0], vector_pair.first[1]);
            std::swap(vector_pair.second[0], vector_pair.second[1]);
        }

        BinomialConstraint<T_Variable, T_Expression> binomial;

        binomial.variable_ptr_first  = vector_pair.first[0];
        binomial.variable_ptr_second = vector_pair.first[1];
        binomial.sensitivity_first   = vector_pair.second[0];
        binomial.sensitivity_second  = vector_pair.second[1];
        binomial.constant_value      = expression.constant_value();
        binomial.sense               = constraint_ptr->sense();
        results.push_back(binomial);
    }

    return results;
}

/*****************************************************************************/
template <class T_Variable, class T_Expression>
inline std::vector<TrinomialConstraint<T_Variable, T_Expression>>
convert_to_trinomial_constraints(
    const std::vector<model_component::Constraint<T_Variable, T_Expression> *>
        &a_CONSTRAINT_PTRS) {
    std::vector<TrinomialConstraint<T_Variable, T_Expression>> results;
    for (const auto &constraint_ptr : a_CONSTRAINT_PTRS) {
        auto &expression = constraint_ptr->expression();

        if (expression.sensitivities().size() != 3) {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The constraint is not trinomial."));
        }

        auto vector_pair = utility::to_vector_pair(expression.sensitivities());

        TrinomialConstraint<T_Variable, T_Expression> trinomial;

        trinomial.variable_ptr_first  = vector_pair.first[0];
        trinomial.variable_ptr_second = vector_pair.first[1];
        trinomial.variable_ptr_third  = vector_pair.first[2];
        trinomial.sensitivity_first   = vector_pair.second[0];
        trinomial.sensitivity_second  = vector_pair.second[1];
        trinomial.sensitivity_third   = vector_pair.second[2];
        trinomial.constant_value      = expression.constant_value();
        trinomial.sense               = constraint_ptr->sense();
        results.push_back(trinomial);
    }

    return results;
}

}  // namespace printemps::neighborhood
#endif
/*****************************************************************************/
// END
/*****************************************************************************/