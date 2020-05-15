/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef CPPMH_MODEL_NEIGHBORHOOD_H__
#define CPPMH_MODEL_NEIGHBORHOOD_H__

#include <vector>
#include "selection.h"
#include <typeinfo>

namespace cppmh {
namespace model {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Variable;

/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Neighborhood {
   private:
    std::vector<Selection<T_Variable, T_Expression>> m_selections;

    std::vector<Variable<T_Variable, T_Expression> *> m_selection_variable_ptrs;
    std::vector<Variable<T_Variable, T_Expression> *> m_binary_variable_ptrs;
    std::vector<Variable<T_Variable, T_Expression> *> m_integer_variable_ptrs;

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
        m_selections.clear();
        m_selection_variable_ptrs.clear();
        m_binary_variable_ptrs.clear();
        m_integer_variable_ptrs.clear();

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
    inline constexpr void setup_default_neighborhood(
        std::vector<VariableProxy<T_Variable, T_Expression>>
            *a_variable_proxies,
        std::vector<ConstraintProxy<T_Variable, T_Expression>>
            *      a_constraint_proxies,
        const bool a_IS_ENABLED_PARALLEL) {
        this->categorize_variables_and_constraints(a_variable_proxies,
                                                   a_constraint_proxies);
        this->setup_selection_move_updater(a_IS_ENABLED_PARALLEL);
        this->setup_binary_move_updater(a_IS_ENABLED_PARALLEL);
        this->setup_integer_move_updater(a_IS_ENABLED_PARALLEL);
    }

    /*************************************************************************/
    inline constexpr void setup_has_fixed_variables(
        const std::vector<VariableProxy<T_Variable, T_Expression>>
            &a_VARIABLE_PROXY) {
        bool has_fixed_variables = false;

        for (const auto &variable_proxy : a_VARIABLE_PROXY) {
            for (const auto &variable :
                 variable_proxy.flat_indexed_variables()) {
                if (variable.is_fixed()) {
                    has_fixed_variables = true;
                    break;
                }
            }
            if (has_fixed_variables) {
                break;
            }
        }
        m_has_fixed_variables = has_fixed_variables;
    }
    /*************************************************************************/
    inline constexpr void categorize_variables_and_constraints(
        std::vector<VariableProxy<T_Variable, T_Expression>>
            *a_variable_proxies,
        std::vector<ConstraintProxy<T_Variable, T_Expression>>
            *a_constraint_proxies) {
        /**
         * pointers to binary variables which are includes in selection
         * constraints.
         */
        std::vector<Variable<T_Variable, T_Expression> *>
            selection_variable_ptrs;

        /**
         * pointers to binary variables which are not includes in any selection
         * constraint.
         */
        std::vector<Variable<T_Variable, T_Expression> *> binary_variable_ptrs;

        /**
         * pointers to integer variables which are not includes in any selection
         * constraint.
         */
        std::vector<Variable<T_Variable, T_Expression> *> integer_variable_ptrs;

        std::vector<Selection<T_Variable, T_Expression>> raw_selections;
        std::vector<Selection<T_Variable, T_Expression>> selections;

        /**
         * STEP 1:
         * Filter Special Ordered Set Type I (selection) constraint without
         * considering conflict.
         */
        for (auto &&proxy : *a_constraint_proxies) {
            for (auto &&constraint : proxy.flat_indexed_constraints()) {
                /**
                 * Selection constraint must be linear.
                 */
                if (!constraint.is_linear()) {
                    continue;
                }

                /**
                 * Selection constraint must be equality.
                 */
                if (constraint.sense() != ConstraintSense::Equal) {
                    continue;
                }

                /**
                 * The constant term value of selection constraint must be -1.
                 */
                if (constraint.expression().constant_value() != -1) {
                    continue;
                }

                /**
                 * The number of variables in selection constraint must be more
                 * than or equal to 2.
                 */
                if (constraint.expression().sensitivities().size() < 2) {
                    continue;
                }

                bool is_selection_constraint = true;

                /**
                 * All included variables in selection constraint must be binary
                 * variable with coefficient 1.
                 */
                for (const auto &sensitivity :
                     constraint.expression().sensitivities()) {
                    if (sensitivity.first->sense() != VariableSense::Binary) {
                        is_selection_constraint = false;
                        break;
                    }

                    if (sensitivity.second != 1) {
                        is_selection_constraint = false;
                        break;
                    }
                }

                /**
                 * Store variables pointers to raw_selections.
                 */
                if (is_selection_constraint) {
                    Selection<T_Variable, T_Expression> selection;
                    selection.constraint_ptr = &constraint;

                    for (const auto &sensitivity :
                         constraint.expression().sensitivities()) {
                        selection.variable_ptrs.push_back(sensitivity.first);
                    }
                    raw_selections.push_back(selection);
                }
            }
        }

        /**
         * STEP 2:
         * Add selection constraints extracted in STEP in dictionary order with
         * excluding used variables.
         */
        for (auto &&selection : raw_selections) {
            /// Check whether the variable is covered by other selection
            /// constraint or not.
            auto original_number_of_covered_variables =
                selection.variable_ptrs.size();

            selection.variable_ptrs.erase(
                std::remove_if(
                    selection.variable_ptrs.begin(),
                    selection.variable_ptrs.end(),
                    [&selection_variable_ptrs](const auto &a_VARIABLE_PTR) {
                        return std::find(selection_variable_ptrs.begin(),
                                         selection_variable_ptrs.end(),
                                         a_VARIABLE_PTR) !=
                               selection_variable_ptrs.end();
                    }),
                selection.variable_ptrs.end());

            if (selection.variable_ptrs.size() >= 2) {
                if (selection.variable_ptrs.size() ==
                    original_number_of_covered_variables) {
                    selection.constraint_ptr->disable();
                }
                selections.push_back(selection);
                for (auto &&variable_ptr : selection.variable_ptrs) {
                    selection_variable_ptrs.push_back(variable_ptr);
                }
            } else {
                /// nothing to do
            }
        }

        /*
         * STEP 3:
         * Categorize integer variables into "Selection", "Binary" and
         * "Integer".
         */
        for (auto &&proxy : *a_variable_proxies) {
            for (auto &&variable : proxy.flat_indexed_variables()) {
                if (std::find(selection_variable_ptrs.begin(),
                              selection_variable_ptrs.end(),
                              &variable) == selection_variable_ptrs.end()) {
                    if (variable.lower_bound() == 0 &&
                        variable.upper_bound() == 1) {
                        /// Binary variables
                        binary_variable_ptrs.push_back(&variable);
                    } else {
                        /// Integer variables
                        integer_variable_ptrs.push_back(&variable);
                    }
                }
            }
        }

        m_selections              = selections;
        m_selection_variable_ptrs = selection_variable_ptrs;
        m_binary_variable_ptrs    = binary_variable_ptrs;
        m_integer_variable_ptrs   = integer_variable_ptrs;

        for (auto &&selection : m_selections) {
            for (auto &&variable_ptr : selection.variable_ptrs) {
                /**
                 * Register the selection object to the variables which is
                 * covered by the corresponding selection constraint, and
                 * categorize the variable into "Selection".
                 */
                variable_ptr->set_selection_ptr(&selection);
            }
        }
    }

    /*************************************************************************/
    inline constexpr void setup_selection_move_updater(
        const bool a_IS_ENABLED_PARALLEL) {
        /**
         *  "Swap" move for binary variables in selection
         * constraints: e.g.) selection constraint x + y + z = 1 (x,
         * y, z \in {0, 1}) move: {(x = 0, y = 1), (x = 0, z = 1)}
         * (if x = 1, y = 0, z = 0)
         */

        int selections_size              = m_selections.size();
        int selection_variable_ptrs_size = m_selection_variable_ptrs.size();
        m_selection_moves.resize(selection_variable_ptrs_size);

        for (auto i = 0; i < selections_size; i++) {
            for (auto &variable_ptr : m_selections[i].variable_ptrs) {
                auto &ptrs = variable_ptr->contributive_constraint_ptrs();
                m_selections[i].contributive_constraint_ptrs.insert(
                    ptrs.begin(), ptrs.end());
            }
        }

        for (auto i = 0; i < selection_variable_ptrs_size; i++) {
            m_selection_moves[i].contributive_constraint_ptrs =
                m_selection_variable_ptrs[i]
                    ->selection_ptr()
                    ->contributive_constraint_ptrs;
        }

        auto selection_move_updater = [this,
                                       a_IS_ENABLED_PARALLEL](auto *a_moves) {
            int selection_variable_ptrs_size = m_selection_variable_ptrs.size();
#ifdef _OPENMP
#pragma omp parallel for if (a_IS_ENABLED_PARALLEL) schedule(static)
#endif
            for (auto i = 0; i < selection_variable_ptrs_size; i++) {
                (*a_moves)[i].alterations.clear();
                (*a_moves)[i].sense = MoveSense::Selection;

                (*a_moves)[i].alterations.emplace_back(
                    m_selection_variable_ptrs[i]
                        ->selection_ptr()
                        ->selected_variable_ptr,
                    0);

                (*a_moves)[i].alterations.emplace_back(
                    m_selection_variable_ptrs[i], 1);
            }
        };
        m_selection_move_updater = selection_move_updater;
    }

    /*************************************************************************/
    inline constexpr void setup_binary_move_updater(
        const bool a_IS_ENABLED_PARALLEL) {
        /**
         * "Flip" move for binary variables:
         * e.g) binary variable x \in {0, 1}
         *  move: {(x = 1)} (if x = 0)
         *        {(x = 0)} (if x = 1)
         */
        int binary_variable_ptrs_size = m_binary_variable_ptrs.size();
        m_binary_moves.resize(binary_variable_ptrs_size);

        for (auto i = 0; i < binary_variable_ptrs_size; i++) {
            m_binary_moves[i].contributive_constraint_ptrs =
                m_binary_variable_ptrs[i]->contributive_constraint_ptrs();
        }

        auto binary_move_updater = [this,
                                    a_IS_ENABLED_PARALLEL](auto *a_moves) {
            int binary_variable_ptrs_size = m_binary_variable_ptrs.size();
#ifdef _OPENMP
#pragma omp parallel for if (a_IS_ENABLED_PARALLEL) schedule(static)
#endif
            for (auto i = 0; i < binary_variable_ptrs_size; i++) {
                (*a_moves)[i].alterations.clear();
                (*a_moves)[i].sense = MoveSense::Binary;
                (*a_moves)[i].alterations.emplace_back(
                    m_binary_variable_ptrs[i],
                    1 - m_binary_variable_ptrs[i]->value());
            }
        };
        m_binary_move_updater = binary_move_updater;
    }

    /*************************************************************************/
    inline constexpr void setup_integer_move_updater(
        const bool a_IS_ENABLED_PARALLEL) {
        /**
         *  "Shift" move for integer variables:
         *  e.g) integer variable 0 <= x <= 10 (x \in Z)
         *  move: {(x = 4), (x = 6)} (if x = 5)
         *        {(x = 1)} (if x = 0)
         *        {(x = 9)} (if x = 10)
         */
        int integer_variable_ptrs_size = m_integer_variable_ptrs.size();
        m_integer_moves.resize(2 * m_integer_variable_ptrs.size());

        for (auto i = 0; i < integer_variable_ptrs_size; i++) {
            m_integer_moves[2 * i].contributive_constraint_ptrs =
                m_integer_variable_ptrs[i]->contributive_constraint_ptrs();
            m_integer_moves[2 * i + 1].contributive_constraint_ptrs =
                m_integer_variable_ptrs[i]->contributive_constraint_ptrs();
        }

        auto integer_move_updater = [this,
                                     a_IS_ENABLED_PARALLEL](auto *a_moves) {
            int integer_variable_ptrs_size = m_integer_variable_ptrs.size();
#ifdef _OPENMP
#pragma omp parallel for if (a_IS_ENABLED_PARALLEL) schedule(static)
#endif
            for (auto i = 0; i < integer_variable_ptrs_size; i++) {
                (*a_moves)[2 * i].alterations.clear();
                (*a_moves)[2 * i].sense = MoveSense::Integer;
                (*a_moves)[2 * i].alterations.emplace_back(
                    m_integer_variable_ptrs[i],
                    m_integer_variable_ptrs[i]->value() + 1);

                (*a_moves)[2 * i + 1].alterations.clear();
                (*a_moves)[2 * i + 1].sense = MoveSense::Integer;
                (*a_moves)[2 * i + 1].alterations.emplace_back(
                    m_integer_variable_ptrs[i],
                    m_integer_variable_ptrs[i]->value() - 1);
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
    inline constexpr const std::vector<Variable<T_Variable, T_Expression> *>
        &selection_variable_ptrs(void) {
        return m_selection_variable_ptrs;
    }

    /*************************************************************************/
    inline constexpr const std::vector<Variable<T_Variable, T_Expression> *>
        &binary_variable_ptrs(void) {
        return m_binary_variable_ptrs;
    }

    /*************************************************************************/
    inline constexpr const std::vector<Variable<T_Variable, T_Expression> *>
        &integer_variable_ptrs(void) {
        return m_integer_variable_ptrs;
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
    inline constexpr std::vector<Selection<T_Variable, T_Expression>>
        &selections(void) {
        return m_selections;
    }

    /*************************************************************************/
    inline constexpr const std::vector<Selection<T_Variable, T_Expression>>
        &selections(void) const {
        return m_selections;
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