/*****************************************************************************/
// Copyright (c) 2020-2026 Yuji KOGUMA
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
inline std::vector<
    model_component::ConstraintMultinomial<T_Variable, T_Expression>>
convert_to_binomial_constraints(
    const std::vector<model_component::Constraint<T_Variable, T_Expression> *>
              &a_CONSTRAINT_PTRS,
    const bool a_IS_ENABLED_EXCLUDE_OVERLAPPING) {
    std::vector<
        model_component::ConstraintMultinomial<T_Variable, T_Expression>>
        results_raw;
    for (const auto &constraint_ptr : a_CONSTRAINT_PTRS) {
        results_raw.push_back(constraint_ptr->to_binomial_constraint());
    }

    if (!a_IS_ENABLED_EXCLUDE_OVERLAPPING) {
        return results_raw;
    }

    const int BINOMIALS_RAW_SIZE = static_cast<int>(results_raw.size());
    std::unordered_map<model_component::Variable<T_Variable, T_Expression> *,
                       int>
        variable_count;

    variable_count.reserve(2 * BINOMIALS_RAW_SIZE);

    for (const auto &binomial : results_raw) {
        variable_count[binomial.variable_ptrs[0]]++;
        variable_count[binomial.variable_ptrs[1]]++;
    }

    std::vector<
        model_component::ConstraintMultinomial<T_Variable, T_Expression>>
        results_exlude_overlapping;
    results_exlude_overlapping.reserve(BINOMIALS_RAW_SIZE);
    for (const auto &binomial : results_raw) {
        if (variable_count[binomial.variable_ptrs[0]] > 1 ||
            variable_count[binomial.variable_ptrs[1]] > 1) {
            continue;
        }
        results_exlude_overlapping.push_back(binomial);
    }

    return results_exlude_overlapping;
}

/*****************************************************************************/
template <class T_Variable, class T_Expression>
inline std::vector<
    model_component::ConstraintMultinomial<T_Variable, T_Expression>>
convert_to_trinomial_constraints(
    const std::vector<model_component::Constraint<T_Variable, T_Expression> *>
        &a_CONSTRAINT_PTRS) {
    std::vector<
        model_component::ConstraintMultinomial<T_Variable, T_Expression>>
        results;
    for (const auto &constraint_ptr : a_CONSTRAINT_PTRS) {
        results.push_back(constraint_ptr->to_trinomial_constraint());
    }

    return results;
}

}  // namespace printemps::neighborhood
#endif
/*****************************************************************************/
// END
/*****************************************************************************/