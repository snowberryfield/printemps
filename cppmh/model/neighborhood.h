/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef CPPMH_MODEL_NEIGHBORHOOD_H__
#define CPPMH_MODEL_NEIGHBORHOOD_H__

#include <vector>
#include <typeinfo>

namespace cppmh {
namespace model {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Variable;

/*****************************************************************************/
enum SelectionMode : int { None, Defined, Smaller, Larger, Independent };

/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Neighborhood {
   private:
    std::function<void(std::vector<Move<T_Variable, T_Expression>> *)>
        m_selection_move_updater;
    std::function<void(std::vector<Move<T_Variable, T_Expression>> *)>
        m_binary_move_updater;
    std::function<void(std::vector<Move<T_Variable, T_Expression>> *)>
        m_integer_move_updater;
    std::function<void(std::vector<Move<T_Variable, T_Expression>> *)>
        m_user_defined_move_updater;

    std::vector<Move<T_Variable, T_Expression>> m_selection_moves;
    std::vector<Move<T_Variable, T_Expression>> m_binary_moves;
    std::vector<Move<T_Variable, T_Expression>> m_integer_moves;
    std::vector<Move<T_Variable, T_Expression>> m_user_defined_moves;

    std::vector<Move<T_Variable, T_Expression> *> m_move_ptrs;

    bool m_has_fixed_variables;
    bool m_is_enabled_selection_move;
    bool m_is_enabled_binary_move;
    bool m_is_enabled_integer_move;
    bool m_is_enabled_user_defined_move;

   public:
    /*************************************************************************/
    Neighborhood(void) {
        this->initialize();
    }

    /*************************************************************************/
    virtual ~Neighborhood(void) {
        /// nothing to do
    }

    /*************************************************************************/
    inline void initialize(void) {
        m_selection_move_updater =
            [](std::vector<Move<T_Variable, T_Expression>> *) {};
        m_binary_move_updater =
            [](std::vector<Move<T_Variable, T_Expression>> *) {};
        m_integer_move_updater =
            [](std::vector<Move<T_Variable, T_Expression>> *) {};
        m_user_defined_move_updater =
            [](std::vector<Move<T_Variable, T_Expression>> *) {};

        m_selection_moves.clear();
        m_binary_moves.clear();
        m_integer_moves.clear();
        m_user_defined_moves.clear();

        m_move_ptrs.clear();

        m_has_fixed_variables          = false;
        m_is_enabled_selection_move    = true;
        m_is_enabled_binary_move       = true;
        m_is_enabled_integer_move      = true;
        m_is_enabled_user_defined_move = false;
    }

    /*************************************************************************/
    inline constexpr void set_has_fixed_variables(
        const bool a_HAS_FIXED_VARIABLES) {
        m_has_fixed_variables = a_HAS_FIXED_VARIABLES;
    }

    /*************************************************************************/
    inline constexpr void setup_selection_move_updater(
        std::vector<Variable<T_Variable, T_Expression> *> &a_VARIABLE_PTRS,
        const bool a_IS_ENABLED_PARALLEL) {
        /**
         *  "Swap" move for binary variables in selection
         * constraints: e.g.) selection constraint x + y + z = 1 (x,
         * y, z \in {0, 1}) move: {(x = 0, y = 1), (x = 0, z = 1)}
         * (if x = 1, y = 0, z = 0)
         */

        int variables_size = a_VARIABLE_PTRS.size();
        m_selection_moves.resize(variables_size);

        for (auto i = 0; i < variables_size; i++) {
            m_selection_moves[i].sense = MoveSense::Selection;
            m_selection_moves[i].related_constraint_ptrs =
                a_VARIABLE_PTRS[i]->selection_ptr()->related_constraint_ptrs;
        }

        auto selection_move_updater = [this, a_VARIABLE_PTRS, variables_size,
                                       a_IS_ENABLED_PARALLEL](auto *a_moves) {
#ifdef _OPENMP
#pragma omp parallel for if (a_IS_ENABLED_PARALLEL) schedule(static)
#endif
            for (auto i = 0; i < variables_size; i++) {
                (*a_moves)[i].alterations.clear();
                (*a_moves)[i].alterations.emplace_back(
                    a_VARIABLE_PTRS[i]->selection_ptr()->selected_variable_ptr,
                    0);

                (*a_moves)[i].alterations.emplace_back(a_VARIABLE_PTRS[i], 1);
            }
        };
        m_selection_move_updater = selection_move_updater;
    }

    /*************************************************************************/
    inline constexpr void setup_binary_move_updater(
        std::vector<Variable<T_Variable, T_Expression> *> &a_VARIABLE_PTRS,
        const bool a_IS_ENABLED_PARALLEL) {
        /**
         * "Flip" move for binary variables:
         * e.g) binary variable x \in {0, 1}
         *  move: {(x = 1)} (if x = 0)
         *        {(x = 0)} (if x = 1)
         */
        int variables_size = a_VARIABLE_PTRS.size();
        m_binary_moves.resize(variables_size);

        for (auto i = 0; i < variables_size; i++) {
            m_binary_moves[i].sense = MoveSense::Binary;
            m_binary_moves[i].related_constraint_ptrs =
                a_VARIABLE_PTRS[i]->related_constraint_ptrs();
        }

        auto binary_move_updater = [this, a_VARIABLE_PTRS, variables_size,
                                    a_IS_ENABLED_PARALLEL](auto *a_moves) {
#ifdef _OPENMP
#pragma omp parallel for if (a_IS_ENABLED_PARALLEL) schedule(static)
#endif
            for (auto i = 0; i < variables_size; i++) {
                (*a_moves)[i].alterations.clear();
                (*a_moves)[i].alterations.emplace_back(
                    a_VARIABLE_PTRS[i], 1 - a_VARIABLE_PTRS[i]->value());
            }
        };
        m_binary_move_updater = binary_move_updater;
    }

    /*************************************************************************/
    inline constexpr void setup_integer_move_updater(
        std::vector<Variable<T_Variable, T_Expression> *> &a_VARIABLE_PTRS,
        const bool a_IS_ENABLED_PARALLEL) {
        /**
         *  "Shift" move for integer variables:
         *  e.g) integer variable 0 <= x <= 10 (x \in Z)
         *  move: {(x = 4), (x = 6)} (if x = 5)
         *        {(x = 1)} (if x = 0)
         *        {(x = 9)} (if x = 10)
         */
        int variables_size = a_VARIABLE_PTRS.size();
        m_integer_moves.resize(2 * variables_size);

        for (auto i = 0; i < variables_size; i++) {
            m_integer_moves[2 * i].sense = MoveSense::Integer;
            m_integer_moves[2 * i].related_constraint_ptrs =
                a_VARIABLE_PTRS[i]->related_constraint_ptrs();

            m_integer_moves[2 * i + 1].sense = MoveSense::Integer;
            m_integer_moves[2 * i + 1].related_constraint_ptrs =
                a_VARIABLE_PTRS[i]->related_constraint_ptrs();
        }

        auto integer_move_updater = [this, a_VARIABLE_PTRS, variables_size,
                                     a_IS_ENABLED_PARALLEL](auto *a_moves) {
#ifdef _OPENMP
#pragma omp parallel for if (a_IS_ENABLED_PARALLEL) schedule(static)
#endif
            for (auto i = 0; i < variables_size; i++) {
                (*a_moves)[2 * i].alterations.clear();
                (*a_moves)[2 * i].alterations.emplace_back(
                    a_VARIABLE_PTRS[i], a_VARIABLE_PTRS[i]->value() + 1);

                (*a_moves)[2 * i + 1].alterations.clear();
                (*a_moves)[2 * i + 1].alterations.emplace_back(
                    a_VARIABLE_PTRS[i], a_VARIABLE_PTRS[i]->value() - 1);
            }
        };
        m_integer_move_updater = integer_move_updater;
    }

    /*************************************************************************/
    inline constexpr void set_user_defined_move_updater(
        const std::function<void(std::vector<Move<T_Variable, T_Expression>> *)>
            &a_FUNCTION) {
        m_user_defined_move_updater    = a_FUNCTION;
        m_is_enabled_user_defined_move = true;
    }

    /*************************************************************************/
    inline constexpr void update_moves(void) noexcept {
        if (m_selection_moves.size() > 0 && m_is_enabled_selection_move) {
            m_selection_move_updater(&m_selection_moves);
        }

        if (m_binary_moves.size() > 0 && m_is_enabled_binary_move) {
            m_binary_move_updater(&m_binary_moves);
        }

        if (m_integer_moves.size() > 0 && m_is_enabled_integer_move) {
            m_integer_move_updater(&m_integer_moves);
        }

        if (m_is_enabled_user_defined_move) {
            m_user_defined_move_updater(&m_user_defined_moves);
        }

        auto number_of_candidate_moves =
            m_selection_moves.size() + m_binary_moves.size() +
            m_integer_moves.size() + m_user_defined_moves.size();

        m_move_ptrs.resize(number_of_candidate_moves);

        auto has_fixed_value = [this](const auto &a_MOVE) {
            if (!m_has_fixed_variables) {
                return false;
            }
            bool has_fixed_value = false;
            for (auto &&alteration : a_MOVE.alterations) {
                if (alteration.first->is_fixed()) {
                    has_fixed_value = true;
                    break;
                }
            }
            return has_fixed_value;
        };

        auto index = 0;

        if (m_is_enabled_selection_move) {
            for (auto &&move : m_selection_moves) {
                if (move.alterations[0].first == move.alterations[1].first) {
                    continue;
                }

                if (has_fixed_value(move)) {
                    continue;
                }
                m_move_ptrs[index++] = &move;
            }
        }

        if (m_is_enabled_binary_move) {
            for (auto &&move : m_binary_moves) {
                if (has_fixed_value(move)) {
                    continue;
                }
                m_move_ptrs[index++] = &move;
            }
        }

        if (m_is_enabled_integer_move) {
            for (auto &&move : m_integer_moves) {
                if (has_fixed_value(move)) {
                    continue;
                }

                if (move.alterations[0].second <
                    move.alterations[0].first->lower_bound()) {
                    continue;
                } else if (move.alterations[0].second >
                           move.alterations[0].first->upper_bound()) {
                    continue;
                }
                m_move_ptrs[index++] = &move;
            }
        }

        if (m_is_enabled_user_defined_move) {
            for (auto &&move : m_user_defined_moves) {
                if (has_fixed_value(move)) {
                    continue;
                }
                m_move_ptrs[index++] = &move;
            }
        }

        m_move_ptrs.erase(m_move_ptrs.begin() + index, m_move_ptrs.end());
    }

    /*************************************************************************/
    inline constexpr void shuffle_moves(std::mt19937 *a_rand) noexcept {
        std::shuffle(m_move_ptrs.begin(), m_move_ptrs.end(), *a_rand);
    }

    /*************************************************************************/
    inline constexpr const std::vector<Move<T_Variable, T_Expression>>
        &selection_moves(void) {
        return m_selection_moves;
    }

    /*************************************************************************/
    inline constexpr const std::vector<Move<T_Variable, T_Expression>>
        &binary_moves(void) {
        return m_binary_moves;
    }

    /*************************************************************************/
    inline constexpr const std::vector<Move<T_Variable, T_Expression>>
        &integer_moves(void) {
        return m_integer_moves;
    }

    /*************************************************************************/
    inline constexpr const std::vector<Move<T_Variable, T_Expression>>
        &user_defined_moves(void) {
        return m_user_defined_moves;
    }

    /*************************************************************************/
    inline constexpr const std::vector<Move<T_Variable, T_Expression> *>
        &move_ptrs(void) const {
        return m_move_ptrs;
    }

    /*************************************************************************/
    inline constexpr bool has_fixed_variables(void) const {
        return m_has_fixed_variables;
    }

    /*************************************************************************/
    inline constexpr bool is_enabled_selection_move(void) const {
        return m_is_enabled_selection_move;
    }

    /*************************************************************************/
    inline constexpr void enable_selection_move(void) {
        m_is_enabled_selection_move = true;
    }

    /*************************************************************************/
    inline constexpr void disable_selection_move(void) {
        m_is_enabled_selection_move = false;
    }

    /*************************************************************************/
    inline constexpr bool is_enabled_binary_move(void) const {
        return m_is_enabled_binary_move;
    }

    /*************************************************************************/
    inline constexpr void enable_binary_move(void) {
        m_is_enabled_binary_move = true;
    }

    /*************************************************************************/
    inline constexpr void disable_binary_move(void) {
        m_is_enabled_binary_move = false;
    }

    /*************************************************************************/
    inline constexpr bool is_enabled_integer_move(void) const {
        return m_is_enabled_integer_move;
    }

    /*************************************************************************/
    inline constexpr void enable_integer_move(void) {
        m_is_enabled_integer_move = true;
    }

    /*************************************************************************/
    inline constexpr void disable_integer_move(void) {
        m_is_enabled_integer_move = false;
    }

    /*************************************************************************/
    inline constexpr bool is_enabled_user_defined_move(void) const {
        return m_is_enabled_user_defined_move;
    }

    /*************************************************************************/
    inline constexpr void enable_default_move(void) {
        this->enable_selection_move();
        this->enable_binary_move();
        this->enable_integer_move();
    }

    /*************************************************************************/
    inline constexpr void disable_default_move(void) {
        this->disable_selection_move();
        this->disable_binary_move();
        this->disable_integer_move();
    }

    /*************************************************************************/
    inline constexpr void enable_user_defined_move(void) {
        m_is_enabled_user_defined_move = true;
    }

    /*************************************************************************/
    inline constexpr void disable_user_defined_move(void) {
        m_is_enabled_user_defined_move = false;
    }
};
}  // namespace model
}  // namespace cppmh
#endif
/*****************************************************************************/
// END
/*****************************************************************************/