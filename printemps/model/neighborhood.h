/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MODEL_NEIGHBORHOOD_H__
#define PRINTEMPS_MODEL_NEIGHBORHOOD_H__

#include <vector>
#include <typeinfo>

namespace printemps {
namespace model {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
constexpr bool has_fixed_variables(
    const Move<T_Variable, T_Expression> &a_MOVE) {
    for (const auto &alteration : a_MOVE.alterations) {
        if (alteration.first->is_fixed()) {
            return true;
        }
    }
    return false;
};

/*****************************************************************************/
template <class T_Variable, class T_Expression>
constexpr bool has_selection_variables(
    const Move<T_Variable, T_Expression> &a_MOVE) {
    for (const auto &alteration : a_MOVE.alterations) {
        if (alteration.first->sense() == VariableSense::Selection) {
            return true;
        }
    }
    return false;
};

/*****************************************************************************/
template <class T_Variable, class T_Expression>
constexpr bool has_bound_violation(
    const Move<T_Variable, T_Expression> &a_MOVE) {
    for (const auto &alteration : a_MOVE.alterations) {
        if (alteration.second < alteration.first->lower_bound()) {
            return true;
        }
        if (alteration.second > alteration.first->upper_bound()) {
            return true;
        }
    }
    return false;
};

/*****************************************************************************/
template <class T_Variable, class T_Expression>
constexpr bool has_objective_improvable_variable(
    const Move<T_Variable, T_Expression> &a_MOVE) {
    for (const auto &alteration : a_MOVE.alterations) {
        if (alteration.first->is_objective_improvable()) {
            return true;
        }
    }
    return false;
};

/*****************************************************************************/
template <class T_Variable, class T_Expression>
constexpr bool has_feasibility_improvable_variable(
    const Move<T_Variable, T_Expression> &a_MOVE) {
    for (const auto &alteration : a_MOVE.alterations) {
        if (alteration.first->is_feasibility_improvable()) {
            return true;
        }
    }
    return false;
};

/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Variable;

/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Neighborhood {
   private:
    std::function<void(std::vector<Move<T_Variable, T_Expression>> *,
                       std::vector<int> *, const bool, const bool, const bool,
                       const bool)>
        m_binary_move_updater;
    std::function<void(std::vector<Move<T_Variable, T_Expression>> *,
                       std::vector<int> *, const bool, const bool, const bool,
                       const bool)>
        m_integer_move_updater;
    std::function<void(std::vector<Move<T_Variable, T_Expression>> *,
                       std::vector<int> *, const bool, const bool, const bool,
                       const bool)>
        m_precedence_move_updater;
    std::function<void(std::vector<Move<T_Variable, T_Expression>> *,
                       std::vector<int> *, const bool, const bool, const bool,
                       const bool)>
        m_aggregation_move_updater;
    std::function<void(std::vector<Move<T_Variable, T_Expression>> *,
                       std::vector<int> *, const bool, const bool, const bool,
                       const bool)>
        m_variable_bound_move_updater;
    std::function<void(std::vector<Move<T_Variable, T_Expression>> *,
                       std::vector<int> *, const bool, const bool, const bool,
                       const bool)>
        m_exclusive_move_updater;
    std::function<void(std::vector<Move<T_Variable, T_Expression>> *,
                       std::vector<int> *, const bool, const bool, const bool,
                       const bool)>
        m_selection_move_updater;
    std::function<void(std::vector<Move<T_Variable, T_Expression>> *,
                       std::vector<int> *, const bool, const bool, const bool,
                       const bool)>
        m_chain_move_updater;

    std::function<void(std::vector<Move<T_Variable, T_Expression>> *)>
        m_user_defined_move_updater;
    std::function<void(std::vector<Move<T_Variable, T_Expression>> *,
                       std::vector<int> *, const bool, const bool, const bool,
                       const bool)>
        m_user_defined_move_updater_wrapper;

    std::vector<Move<T_Variable, T_Expression>> m_binary_moves;
    std::vector<Move<T_Variable, T_Expression>> m_integer_moves;
    std::vector<Move<T_Variable, T_Expression>> m_precedence_moves;
    std::vector<Move<T_Variable, T_Expression>> m_aggregation_moves;
    std::vector<Move<T_Variable, T_Expression>> m_variable_bound_moves;
    std::vector<Move<T_Variable, T_Expression>> m_exclusive_moves;
    std::vector<Move<T_Variable, T_Expression>> m_selection_moves;
    std::vector<Move<T_Variable, T_Expression>> m_chain_moves;
    std::vector<Move<T_Variable, T_Expression>> m_user_defined_moves;

    std::vector<int> m_binary_move_flags;
    std::vector<int> m_integer_move_flags;
    std::vector<int> m_precedence_move_flags;
    std::vector<int> m_aggregation_move_flags;
    std::vector<int> m_variable_bound_move_flags;
    std::vector<int> m_exclusive_move_flags;
    std::vector<int> m_selection_move_flags;
    std::vector<int> m_chain_move_flags;
    std::vector<int> m_user_defined_move_flags;

    std::vector<Move<T_Variable, T_Expression> *> m_move_ptrs;

    bool m_has_fixed_variables;
    bool m_has_selection_variables;

    bool m_is_enabled_binary_move;
    bool m_is_enabled_integer_move;
    bool m_is_enabled_precedence_move;
    bool m_is_enabled_aggregation_move;
    bool m_is_enabled_variable_bound_move;
    bool m_is_enabled_exclusive_move;
    bool m_is_enabled_selection_move;
    bool m_is_enabled_chain_move;
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
    void initialize(void) {
        m_binary_move_updater =
            [](std::vector<Move<T_Variable, T_Expression>> *,
               std::vector<int> *, const bool, const bool, const bool,
               const bool) {};
        m_integer_move_updater =
            [](std::vector<Move<T_Variable, T_Expression>> *,
               std::vector<int> *, const bool, const bool, const bool,
               const bool) {};
        m_precedence_move_updater =
            [](std::vector<Move<T_Variable, T_Expression>> *,
               std::vector<int> *, const bool, const bool, const bool,
               const bool) {};
        m_aggregation_move_updater =
            [](std::vector<Move<T_Variable, T_Expression>> *,
               std::vector<int> *, const bool, const bool, const bool,
               const bool) {};
        m_variable_bound_move_updater =
            [](std::vector<Move<T_Variable, T_Expression>> *,
               std::vector<int> *, const bool, const bool, const bool,
               const bool) {};
        m_exclusive_move_updater =
            [](std::vector<Move<T_Variable, T_Expression>> *,
               std::vector<int> *, const bool, const bool, const bool,
               const bool) {};
        m_selection_move_updater =
            [](std::vector<Move<T_Variable, T_Expression>> *,
               std::vector<int> *, const bool, const bool, const bool,
               const bool) {};
        m_chain_move_updater =  //
            [](std::vector<Move<T_Variable, T_Expression>> *,
               std::vector<int> *, const bool, const bool, const bool,
               const bool) {};
        m_user_defined_move_updater =
            [](std::vector<Move<T_Variable, T_Expression>> *) {};
        m_user_defined_move_updater_wrapper =
            [](std::vector<Move<T_Variable, T_Expression>> *,
               std::vector<int> *, const bool, const bool, const bool,
               const bool) {};

        m_binary_moves.clear();
        m_integer_moves.clear();
        m_precedence_moves.clear();
        m_aggregation_moves.clear();
        m_variable_bound_moves.clear();
        m_exclusive_moves.clear();
        m_selection_moves.clear();
        m_chain_moves.clear();
        m_user_defined_moves.clear();

        m_binary_move_flags.clear();
        m_integer_move_flags.clear();
        m_precedence_move_flags.clear();
        m_aggregation_move_flags.clear();
        m_variable_bound_move_flags.clear();
        m_exclusive_move_flags.clear();
        m_selection_move_flags.clear();
        m_chain_move_flags.clear();
        m_user_defined_move_flags.clear();

        m_move_ptrs.clear();

        m_has_fixed_variables     = false;
        m_has_selection_variables = false;

        m_is_enabled_binary_move         = false;
        m_is_enabled_integer_move        = false;
        m_is_enabled_precedence_move     = false;
        m_is_enabled_aggregation_move    = false;
        m_is_enabled_variable_bound_move = false;
        m_is_enabled_exclusive_move      = false;
        m_is_enabled_selection_move      = false;
        m_is_enabled_chain_move          = false;
        m_is_enabled_user_defined_move   = false;
    }

    /*************************************************************************/
    inline constexpr void set_has_fixed_variables(
        const bool a_HAS_FIXED_VARIABLES) {
        m_has_fixed_variables = a_HAS_FIXED_VARIABLES;
    }

    /*************************************************************************/
    inline constexpr void set_has_selection_variables(
        const bool a_HAS_SELECTION_VARIABLES) {
        m_has_selection_variables = a_HAS_SELECTION_VARIABLES;
    }

    /*************************************************************************/
    constexpr void setup_binary_move_updater(
        const std::vector<Variable<T_Variable, T_Expression> *>
            &a_VARIABLE_PTRS) {
        /**
         * "Flip" move for binary variables:
         * e.g) binary variable x \in {0, 1}
         *  move: {(x = 1)} (if x = 0)
         *        {(x = 0)} (if x = 1)
         */

        std::vector<Variable<T_Variable, T_Expression> *>
            not_fixed_variable_ptrs;
        for (auto &&variable_ptr : a_VARIABLE_PTRS) {
            if (!variable_ptr->is_fixed()) {
                not_fixed_variable_ptrs.push_back(variable_ptr);
            }
        }

        const int VARIABLES_SIZE = not_fixed_variable_ptrs.size();
        m_binary_moves.resize(VARIABLES_SIZE);
        m_binary_move_flags.resize(VARIABLES_SIZE);

        for (auto i = 0; i < VARIABLES_SIZE; i++) {
            m_binary_moves[i].sense = MoveSense::Binary;
            m_binary_moves[i].related_constraint_ptrs =
                not_fixed_variable_ptrs[i]->related_constraint_ptrs();
            m_binary_moves[i].alterations.emplace_back(
                not_fixed_variable_ptrs[i], 0);
            m_binary_moves[i].is_special_neighborhood_move = false;
            m_binary_moves[i].is_available                 = true;
            m_binary_moves[i].overlap_rate                 = 0.0;
        }

        auto binary_move_updater =  //
            [this, not_fixed_variable_ptrs, VARIABLES_SIZE](
                auto *                      a_moves,                          //
                auto *                      a_flags,                          //
                const bool                  a_ACCEPT_ALL,                     //
                const bool                  a_ACCEPT_OBJECTIVE_IMPROVABLE,    //
                const bool                  a_ACCEPT_FEASIBILITY_IMPROVABLE,  //
                [[maybe_unused]] const bool a_IS_ENABLED_PARALLEL) {
#ifdef _OPENMP
#pragma omp parallel for if (a_IS_ENABLED_PARALLEL) schedule(static)
#endif
                for (auto i = 0; i < VARIABLES_SIZE; i++) {
                    if (a_ACCEPT_ALL ||
                        (a_ACCEPT_OBJECTIVE_IMPROVABLE &&
                         not_fixed_variable_ptrs[i]
                             ->is_objective_improvable()) ||
                        (a_ACCEPT_FEASIBILITY_IMPROVABLE &&
                         not_fixed_variable_ptrs[i]
                             ->is_feasibility_improvable())) {
                        (*a_moves)[i].alterations.front().second =
                            1 - not_fixed_variable_ptrs[i]->value();
                        (*a_flags)[i] = 1;
                    } else {
                        (*a_flags)[i] = 0;
                    }
                }
            };
        m_binary_move_updater = binary_move_updater;
    }

    /*************************************************************************/
    constexpr void setup_integer_move_updater(
        const std::vector<Variable<T_Variable, T_Expression> *>
            &a_VARIABLE_PTRS) {
        /**
         *  "Shift" move for integer variables:
         *  e.g) integer variable 0 <= x <= 10 (x \in Z)
         *  move: {(x = 5), (x = 7), (x = 3), (x = 8)} (if x = 6)
         *        {(x = 1), (x = 5)} (if x = 0)
         *        {(x = 9), (x = 5)} (if x = 10)
         */

        std::vector<Variable<T_Variable, T_Expression> *>
            not_fixed_variable_ptrs;
        for (auto &&variable_ptr : a_VARIABLE_PTRS) {
            if (!variable_ptr->is_fixed()) {
                not_fixed_variable_ptrs.push_back(variable_ptr);
            }
        }

        const int VARIABLES_SIZE = not_fixed_variable_ptrs.size();
        m_integer_moves.resize(4 * VARIABLES_SIZE);
        m_integer_move_flags.resize(4 * VARIABLES_SIZE);

        for (auto i = 0; i < VARIABLES_SIZE; i++) {
            for (auto j = 0; j < 4; j++) {
                m_integer_moves[4 * i + j].sense = MoveSense::Integer;
                m_integer_moves[4 * i + j].related_constraint_ptrs =
                    not_fixed_variable_ptrs[i]->related_constraint_ptrs();
                m_integer_moves[4 * i + j].alterations.emplace_back(
                    not_fixed_variable_ptrs[i], 0);
                m_integer_moves[4 * i + j].is_special_neighborhood_move = false;
                m_integer_moves[4 * i + j].is_available                 = true;
                m_integer_moves[4 * i + j].overlap_rate                 = 0.0;
            }
        }

        auto integer_move_updater =  //
            [this, not_fixed_variable_ptrs, VARIABLES_SIZE](
                auto *                      a_moves,                          //
                auto *                      a_flags,                          //
                const bool                  a_ACCEPT_ALL,                     //
                const bool                  a_ACCEPT_OBJECTIVE_IMPROVABLE,    //
                const bool                  a_ACCEPT_FEASIBILITY_IMPROVABLE,  //
                [[maybe_unused]] const bool a_IS_ENABLED_PARALLEL) {
                const int DELTA_MAX = 10000;
#ifdef _OPENMP
#pragma omp parallel for if (a_IS_ENABLED_PARALLEL) schedule(static)
#endif
                for (auto i = 0; i < VARIABLES_SIZE; i++) {
                    const auto value = not_fixed_variable_ptrs[i]->value();
                    const auto lower_bound =
                        not_fixed_variable_ptrs[i]->lower_bound();
                    const auto upper_bound =
                        not_fixed_variable_ptrs[i]->upper_bound();

                    if (a_ACCEPT_ALL ||
                        (a_ACCEPT_OBJECTIVE_IMPROVABLE &&
                         not_fixed_variable_ptrs[i]
                             ->is_objective_improvable()) ||
                        (a_ACCEPT_FEASIBILITY_IMPROVABLE &&
                         not_fixed_variable_ptrs[i]
                             ->is_feasibility_improvable())) {
                        if (value == upper_bound) {
                            (*a_flags)[4 * i] = 0;
                        } else {
                            (*a_moves)[4 * i].alterations.front().second =
                                value + 1;
                            (*a_flags)[4 * i] = 1;
                        }

                        if (value == lower_bound) {
                            (*a_flags)[4 * i + 1] = 0;
                        } else {
                            (*a_moves)[4 * i + 1].alterations.front().second =
                                value - 1;
                            (*a_flags)[4 * i + 1] = 1;
                        }

                        if (value == upper_bound ||
                            upper_bound == constant::INT_HALF_MAX) {
                            (*a_flags)[4 * i + 2] = 0;
                        } else {
                            const auto delta =
                                std::min(DELTA_MAX, (upper_bound - value) / 2);
                            (*a_moves)[4 * i + 2].alterations.front().second =
                                value + delta;
                            (*a_flags)[4 * i + 2] = 1;
                        }

                        if (value == lower_bound ||
                            lower_bound == constant::INT_HALF_MIN) {
                            (*a_flags)[4 * i + 3] = 0;
                        } else {
                            const auto delta =
                                std::max(-DELTA_MAX, (lower_bound - value) / 2);
                            (*a_moves)[4 * i + 3].alterations.front().second =
                                value + delta;
                            (*a_flags)[4 * i + 3] = 1;
                        }
                    } else {
                        for (auto j = 0; j < 4; j++) {
                            (*a_flags)[4 * i]     = 0;
                            (*a_flags)[4 * i + j] = 0;
                        }
                    }
                }
            };
        m_integer_move_updater = integer_move_updater;
    }

    /*************************************************************************/
    constexpr void setup_aggregation_move_updater(
        const std::vector<Constraint<T_Variable, T_Expression> *>
            &a_CONSTRAINT_PTRS) {
        const int RAW_CONSTRAINTS_SIZE = a_CONSTRAINT_PTRS.size();

        std::vector<std::vector<Variable<T_Variable, T_Expression> *>>
                                               variable_ptr_pairs;
        std::vector<std::vector<T_Expression>> sensitivity_pairs;
        std::vector<T_Expression>              constants;

        for (auto i = 0; i < RAW_CONSTRAINTS_SIZE; i++) {
            if (a_CONSTRAINT_PTRS[i]->is_enabled()) {
                auto &expression    = a_CONSTRAINT_PTRS[i]->expression();
                auto &sensitivities = expression.sensitivities();
                std::vector<Variable<T_Variable, T_Expression> *>
                                          variable_ptr_pair;
                std::vector<T_Expression> sensitivity_pair;
                for (auto &&sensitivity : sensitivities) {
                    variable_ptr_pair.push_back(sensitivity.first);
                    sensitivity_pair.push_back(sensitivity.second);
                }
                if (variable_ptr_pair[0]->is_fixed() ||
                    variable_ptr_pair[1]->is_fixed() ||
                    (variable_ptr_pair[0]->sense() ==
                     VariableSense::Selection) ||
                    (variable_ptr_pair[1]->sense() ==
                     VariableSense::Selection)) {
                    continue;
                } else {
                    variable_ptr_pairs.push_back(variable_ptr_pair);
                    sensitivity_pairs.push_back(sensitivity_pair);
                    constants.push_back(expression.constant_value());
                }
            }
        }

        const int PAIRS_SIZE = variable_ptr_pairs.size();
        m_aggregation_moves.resize(4 * PAIRS_SIZE);
        m_aggregation_move_flags.resize(4 * PAIRS_SIZE);

        for (auto i = 0; i < PAIRS_SIZE; i++) {
            m_aggregation_moves[4 * i].sense = MoveSense::Aggregation;
            m_aggregation_moves[4 * i].related_constraint_ptrs.insert(
                variable_ptr_pairs[i][0]->related_constraint_ptrs().begin(),
                variable_ptr_pairs[i][0]->related_constraint_ptrs().end());
            m_aggregation_moves[4 * i].related_constraint_ptrs.insert(
                variable_ptr_pairs[i][1]->related_constraint_ptrs().begin(),
                variable_ptr_pairs[i][1]->related_constraint_ptrs().end());

            m_aggregation_moves[4 * i].is_special_neighborhood_move = true;
            m_aggregation_moves[4 * i].is_available                 = true;
            m_aggregation_moves[4 * i].overlap_rate                 = 0.0;

            m_aggregation_moves[4 * i + 1] = m_aggregation_moves[4 * i];
            m_aggregation_moves[4 * i + 2] = m_aggregation_moves[4 * i];
            m_aggregation_moves[4 * i + 3] = m_aggregation_moves[4 * i];
        }

        auto aggregation_move_updater =  //
            [this, variable_ptr_pairs, sensitivity_pairs, constants,
             PAIRS_SIZE](auto *     a_moves,                          //
                         auto *     a_flags,                          //
                         const bool a_ACCEPT_ALL,                     //
                         const bool a_ACCEPT_OBJECTIVE_IMPROVABLE,    //
                         const bool a_ACCEPT_FEASIBILITY_IMPROVABLE,  //
                         [[maybe_unused]] const bool a_IS_ENABLED_PARALLEL) {
#ifdef _OPENMP
#pragma omp parallel for if (a_IS_ENABLED_PARALLEL) schedule(static)
#endif
                for (auto i = 0; i < PAIRS_SIZE; i++) {
                    T_Variable value_pair[2] = {
                        variable_ptr_pairs[i][0]->value(),
                        variable_ptr_pairs[i][1]->value()};
                    for (auto j = 0; j < 2; j++) {
                        auto index = 4 * i + 2 * j;

                        (*a_moves)[index].alterations.clear();
                        (*a_moves)[index].alterations.emplace_back(
                            variable_ptr_pairs[i][j], value_pair[j] + 1);
                        (*a_moves)[index].alterations.emplace_back(
                            variable_ptr_pairs[i][1 - j],
                            static_cast<T_Variable>(
                                (-constants[i] - sensitivity_pairs[i][j] *
                                                     (value_pair[j] + 1)) /
                                sensitivity_pairs[i][1 - j]));

                        index = 4 * i + 2 * j + 1;
                        (*a_moves)[index].alterations.clear();
                        (*a_moves)[index].alterations.emplace_back(
                            variable_ptr_pairs[i][j], value_pair[1 - j] - 1);
                        (*a_moves)[index].alterations.emplace_back(
                            variable_ptr_pairs[i][1 - j],
                            static_cast<T_Variable>(
                                (-constants[i] - sensitivity_pairs[i][j] *
                                                     (value_pair[j] - 1)) /
                                sensitivity_pairs[i][1 - j]));
                    }
                }
                const int MOVES_SIZE = a_moves->size();

#ifdef _OPENMP
#pragma omp parallel for if (a_IS_ENABLED_PARALLEL) schedule(static)
#endif
                for (auto i = 0; i < MOVES_SIZE; i++) {
                    (*a_flags)[i] = 1;
                    if (!(*a_moves)[i].is_available) {
                        (*a_flags)[i] = 0;
                        continue;
                    }
                    if (model::has_bound_violation((*a_moves)[i])) {
                        (*a_flags)[i] = 0;
                        continue;
                    }
                    if (a_ACCEPT_ALL) {
                        /** nothing to do */
                    } else {
                        if (a_ACCEPT_OBJECTIVE_IMPROVABLE &&
                            model::has_objective_improvable_variable(
                                (*a_moves)[i])) {
                            continue;
                        }

                        if (a_ACCEPT_FEASIBILITY_IMPROVABLE &&
                            model::has_feasibility_improvable_variable(
                                (*a_moves)[i])) {
                            continue;
                        }
                        (*a_flags)[i] = 0;
                    }
                }
            };
        m_aggregation_move_updater = aggregation_move_updater;
    }

    /*************************************************************************/
    constexpr void setup_precedence_move_updater(
        const std::vector<Constraint<T_Variable, T_Expression> *>
            &a_CONSTRAINT_PTRS) {
        const int RAW_CONSTRAINTS_SIZE = a_CONSTRAINT_PTRS.size();

        std::vector<std::vector<Variable<T_Variable, T_Expression> *>>
            variable_ptr_pairs;

        for (auto i = 0; i < RAW_CONSTRAINTS_SIZE; i++) {
            if (a_CONSTRAINT_PTRS[i]->is_enabled()) {
                auto &sensitivities =
                    a_CONSTRAINT_PTRS[i]->expression().sensitivities();
                std::vector<Variable<T_Variable, T_Expression> *>
                    variable_ptr_pair;
                for (auto &&sensitivity : sensitivities) {
                    variable_ptr_pair.push_back(sensitivity.first);
                }
                if (variable_ptr_pair[0]->is_fixed() ||
                    variable_ptr_pair[1]->is_fixed() ||
                    (variable_ptr_pair[0]->sense() ==
                     VariableSense::Selection) ||
                    (variable_ptr_pair[1]->sense() ==
                     VariableSense::Selection)) {
                    continue;
                } else {
                    variable_ptr_pairs.push_back(variable_ptr_pair);
                }
            }
        }

        const int PAIRS_SIZE = variable_ptr_pairs.size();
        m_precedence_moves.resize(2 * PAIRS_SIZE);
        m_precedence_move_flags.resize(2 * PAIRS_SIZE);

        for (auto i = 0; i < PAIRS_SIZE; i++) {
            m_precedence_moves[2 * i].sense = MoveSense::Precedence;
            m_precedence_moves[2 * i].related_constraint_ptrs.insert(
                variable_ptr_pairs[i][0]->related_constraint_ptrs().begin(),
                variable_ptr_pairs[i][0]->related_constraint_ptrs().end());
            m_precedence_moves[2 * i].related_constraint_ptrs.insert(
                variable_ptr_pairs[i][1]->related_constraint_ptrs().begin(),
                variable_ptr_pairs[i][1]->related_constraint_ptrs().end());

            m_precedence_moves[2 * i].is_special_neighborhood_move = true;
            m_precedence_moves[2 * i].is_available                 = true;
            m_precedence_moves[2 * i].overlap_rate                 = 0.0;

            m_precedence_moves[2 * i].alterations.emplace_back(
                variable_ptr_pairs[i][0], 0);
            m_precedence_moves[2 * i].alterations.emplace_back(
                variable_ptr_pairs[i][1], 0);

            m_precedence_moves[2 * i + 1] = m_precedence_moves[2 * i];
        }

        auto precedence_move_updater =  //
            [this, variable_ptr_pairs, PAIRS_SIZE](
                auto *                      a_moves,                          //
                auto *                      a_flags,                          //
                const bool                  a_ACCEPT_ALL,                     //
                const bool                  a_ACCEPT_OBJECTIVE_IMPROVABLE,    //
                const bool                  a_ACCEPT_FEASIBILITY_IMPROVABLE,  //
                [[maybe_unused]] const bool a_IS_ENABLED_PARALLEL) {

#ifdef _OPENMP
#pragma omp parallel for if (a_IS_ENABLED_PARALLEL) schedule(static)
#endif
                for (auto i = 0; i < PAIRS_SIZE; i++) {
                    (*a_moves)[2 * i].alterations.clear();
                    (*a_moves)[2 * i].alterations.emplace_back(
                        variable_ptr_pairs[i][0],
                        variable_ptr_pairs[i][0]->value() + 1);
                    (*a_moves)[2 * i].alterations.emplace_back(
                        variable_ptr_pairs[i][1],
                        variable_ptr_pairs[i][1]->value() + 1);
                    (*a_moves)[2 * i + 1].alterations.clear();
                    (*a_moves)[2 * i + 1].alterations.emplace_back(
                        variable_ptr_pairs[i][0],
                        variable_ptr_pairs[i][0]->value() - 1);
                    (*a_moves)[2 * i + 1].alterations.emplace_back(
                        variable_ptr_pairs[i][1],
                        variable_ptr_pairs[i][1]->value() - 1);
                }

                const int MOVES_SIZE = a_moves->size();

#ifdef _OPENMP
#pragma omp parallel for if (a_IS_ENABLED_PARALLEL) schedule(static)
#endif
                for (auto i = 0; i < MOVES_SIZE; i++) {
                    (*a_flags)[i] = 1;
                    if (!(*a_moves)[i].is_available) {
                        (*a_flags)[i] = 0;
                        continue;
                    }
                    if (model::has_bound_violation((*a_moves)[i])) {
                        (*a_flags)[i] = 0;
                        continue;
                    }
                    if (a_ACCEPT_ALL) {
                        /** nothing to do */
                    } else {
                        if (a_ACCEPT_OBJECTIVE_IMPROVABLE &&
                            model::has_objective_improvable_variable(
                                (*a_moves)[i])) {
                            continue;
                        }

                        if (a_ACCEPT_FEASIBILITY_IMPROVABLE &&
                            model::has_feasibility_improvable_variable(
                                (*a_moves)[i])) {
                            continue;
                        }
                        (*a_flags)[i] = 0;
                    }
                }
            };
        m_precedence_move_updater = precedence_move_updater;
    }

    /*************************************************************************/
    void setup_variable_bound_move_updater(
        const std::vector<Constraint<T_Variable, T_Expression> *>
            &a_CONSTRAINT_PTRS) {
        /**
         * NOTE: This method cannot be constexpr by clang for
         * std::vector<ConstraintSense>.
         */
        const int RAW_CONSTRAINTS_SIZE = a_CONSTRAINT_PTRS.size();

        std::vector<std::vector<Variable<T_Variable, T_Expression> *>>
                                               variable_ptr_pairs;
        std::vector<std::vector<T_Expression>> sensitivity_pairs;
        std::vector<T_Expression>              constants;
        std::vector<ConstraintSense>           senses;

        for (auto i = 0; i < RAW_CONSTRAINTS_SIZE; i++) {
            if (a_CONSTRAINT_PTRS[i]->is_enabled()) {
                auto &expression    = a_CONSTRAINT_PTRS[i]->expression();
                auto &sensitivities = expression.sensitivities();
                std::vector<Variable<T_Variable, T_Expression> *>
                                          variable_ptr_pair;
                std::vector<T_Expression> sensitivity_pair;

                for (auto &&sensitivity : sensitivities) {
                    variable_ptr_pair.push_back(sensitivity.first);
                    sensitivity_pair.push_back(sensitivity.second);
                }

                if (variable_ptr_pair[0]->is_fixed() ||
                    variable_ptr_pair[1]->is_fixed() ||
                    (variable_ptr_pair[0]->sense() ==
                     VariableSense::Selection) ||
                    (variable_ptr_pair[1]->sense() ==
                     VariableSense::Selection)) {
                    continue;
                } else {
                    variable_ptr_pairs.push_back(variable_ptr_pair);
                    sensitivity_pairs.push_back(sensitivity_pair);
                    constants.push_back(expression.constant_value());
                    senses.push_back(a_CONSTRAINT_PTRS[i]->sense());
                }
            }
        }

        const int PAIRS_SIZE = variable_ptr_pairs.size();
        m_variable_bound_moves.resize(4 * PAIRS_SIZE);
        m_variable_bound_move_flags.resize(4 * PAIRS_SIZE);

        for (auto i = 0; i < PAIRS_SIZE; i++) {
            m_variable_bound_moves[4 * i].sense = MoveSense::VariableBound;
            m_variable_bound_moves[4 * i].related_constraint_ptrs.insert(
                variable_ptr_pairs[i][0]->related_constraint_ptrs().begin(),
                variable_ptr_pairs[i][0]->related_constraint_ptrs().end());
            m_variable_bound_moves[4 * i].related_constraint_ptrs.insert(
                variable_ptr_pairs[i][1]->related_constraint_ptrs().begin(),
                variable_ptr_pairs[i][1]->related_constraint_ptrs().end());

            m_variable_bound_moves[4 * i].is_special_neighborhood_move = true;
            m_variable_bound_moves[4 * i].is_available                 = true;
            m_variable_bound_moves[4 * i].overlap_rate                 = 0.0;

            m_variable_bound_moves[4 * i + 1] = m_variable_bound_moves[4 * i];
            m_variable_bound_moves[4 * i + 2] = m_variable_bound_moves[4 * i];
            m_variable_bound_moves[4 * i + 3] = m_variable_bound_moves[4 * i];
        }

        auto variable_bound_move_updater =  //
            [this, variable_ptr_pairs, sensitivity_pairs, constants, senses,
             PAIRS_SIZE](auto *     a_moves,                          //
                         auto *     a_flags,                          //
                         const bool a_ACCEPT_ALL,                     //
                         const bool a_ACCEPT_OBJECTIVE_IMPROVABLE,    //
                         const bool a_ACCEPT_FEASIBILITY_IMPROVABLE,  //
                         [[maybe_unused]] const bool a_IS_ENABLED_PARALLEL) {
#ifdef _OPENMP
#pragma omp parallel for if (a_IS_ENABLED_PARALLEL) schedule(static)
#endif
                for (auto i = 0; i < PAIRS_SIZE; i++) {
                    T_Variable value_pair[2] = {
                        variable_ptr_pairs[i][0]->value(),
                        variable_ptr_pairs[i][1]->value()};

                    for (auto j = 0; j < 2; j++) {
                        auto index = 4 * i + 2 * j;
                        (*a_moves)[index].alterations.clear();
                        (*a_moves)[index].alterations.emplace_back(
                            variable_ptr_pairs[i][j], value_pair[j] + 1);

                        {
                            double target_temp =
                                (-constants[i] - sensitivity_pairs[i][j] *
                                                     (value_pair[j] + 1)) /
                                sensitivity_pairs[i][1 - j];
                            T_Variable target = 0;

                            if ((sensitivity_pairs[i][1 - j] > 0 &&
                                 senses[i] == ConstraintSense::Lower) ||
                                (sensitivity_pairs[i][1 - j] < 0 &&
                                 senses[i] == ConstraintSense::Upper)) {
                                target = std::min(value_pair[1 - j],
                                                  static_cast<T_Variable>(
                                                      std::floor(target_temp)));

                            } else {
                                target = std::max(value_pair[1 - j],
                                                  static_cast<T_Variable>(
                                                      std::ceil(target_temp)));
                            }
                            (*a_moves)[index].alterations.emplace_back(
                                variable_ptr_pairs[i][1 - j], target);
                        }

                        index = 4 * i + 2 * j + 1;
                        (*a_moves)[index].alterations.clear();
                        (*a_moves)[index].alterations.emplace_back(
                            variable_ptr_pairs[i][j], value_pair[j] - 1);

                        {
                            double target_temp =
                                (-constants[i] - sensitivity_pairs[i][j] *
                                                     (value_pair[j] - 1)) /
                                sensitivity_pairs[i][1 - j];
                            T_Variable target = 0;

                            if ((sensitivity_pairs[i][1 - j] > 0 &&
                                 senses[i] == ConstraintSense::Lower) ||
                                (sensitivity_pairs[i][1 - j] < 0 &&
                                 senses[i] == ConstraintSense::Upper)) {
                                target = std::min(value_pair[1 - j],
                                                  static_cast<T_Variable>(
                                                      std::floor(target_temp)));

                            } else {
                                target = std::max(value_pair[1 - j],
                                                  static_cast<T_Variable>(
                                                      std::ceil(target_temp)));
                            }
                            (*a_moves)[index].alterations.emplace_back(
                                variable_ptr_pairs[i][1 - j], target);
                        }
                    }
                }

                const int MOVES_SIZE = a_moves->size();
#ifdef _OPENMP
#pragma omp parallel for if (a_IS_ENABLED_PARALLEL) schedule(static)
#endif
                for (auto i = 0; i < MOVES_SIZE; i++) {
                    (*a_flags)[i] = 1;
                    if (!(*a_moves)[i].is_available) {
                        (*a_flags)[i] = 0;
                        continue;
                    }
                    if (model::has_bound_violation((*a_moves)[i])) {
                        (*a_flags)[i] = 0;
                        continue;
                    }
                    if (a_ACCEPT_ALL) {
                        /** nothing to do */
                    } else {
                        if (a_ACCEPT_OBJECTIVE_IMPROVABLE &&
                            model::has_objective_improvable_variable(
                                (*a_moves)[i])) {
                            continue;
                        }

                        if (a_ACCEPT_FEASIBILITY_IMPROVABLE &&
                            model::has_feasibility_improvable_variable(
                                (*a_moves)[i])) {
                            continue;
                        }
                        (*a_flags)[i] = 0;
                    }
                }
            };
        m_variable_bound_move_updater = variable_bound_move_updater;
    }

    /*************************************************************************/
    constexpr void setup_exclusive_move_updater(
        const std::vector<Constraint<T_Variable, T_Expression> *>
            &a_SET_PARTITIONING_PTRS,
        const std::vector<Constraint<T_Variable, T_Expression> *>
            &a_SET_PACKING_PTRS) {
        int SET_PARTITIONINGS_SIZE = a_SET_PARTITIONING_PTRS.size();
        int SET_PACKINGS_SIZE      = a_SET_PACKING_PTRS.size();

        std::unordered_map<
            Variable<T_Variable, T_Expression> *,
            std::unordered_set<Variable<T_Variable, T_Expression> *>>
            associations;

        for (auto i = 0; i < SET_PARTITIONINGS_SIZE; i++) {
            if (a_SET_PARTITIONING_PTRS[i]->is_enabled()) {
                auto &sensitivities =
                    a_SET_PARTITIONING_PTRS[i]->expression().sensitivities();
                for (auto &&sensitivity_first : sensitivities) {
                    for (auto &&sensitivity_second : sensitivities) {
                        if (sensitivity_first == sensitivity_second) {
                            continue;
                        }
                        auto variable_ptr_first  = sensitivity_first.first;
                        auto variable_ptr_second = sensitivity_second.first;

                        if (variable_ptr_first->is_fixed() ||
                            variable_ptr_second->is_fixed() ||
                            (variable_ptr_first->sense() ==
                             VariableSense::Selection) ||
                            (variable_ptr_second->sense() ==
                             VariableSense::Selection)) {
                            continue;
                        } else {
                            associations[variable_ptr_first].insert(
                                variable_ptr_second);
                        }
                    }
                }
            }
        }

        for (auto i = 0; i < SET_PACKINGS_SIZE; i++) {
            if (a_SET_PACKING_PTRS[i]->is_enabled()) {
                auto &sensitivities =
                    a_SET_PACKING_PTRS[i]->expression().sensitivities();
                for (auto &&sensitivity_first : sensitivities) {
                    for (auto &&sensitivity_second : sensitivities) {
                        if (sensitivity_first == sensitivity_second) {
                            continue;
                        }

                        auto variable_ptr_first  = sensitivity_first.first;
                        auto variable_ptr_second = sensitivity_second.first;

                        if (variable_ptr_first->is_fixed() ||
                            variable_ptr_second->is_fixed() ||
                            (variable_ptr_first->sense() ==
                             VariableSense::Selection) ||
                            (variable_ptr_second->sense() ==
                             VariableSense::Selection)) {
                            continue;
                        } else {
                            associations[variable_ptr_first].insert(
                                variable_ptr_second);
                        }
                    }
                }
            }
        }

        const int VARIABLES_SIZE = associations.size();
        m_exclusive_moves.resize(VARIABLES_SIZE);
        m_exclusive_move_flags.resize(VARIABLES_SIZE);

        std::vector<Variable<T_Variable, T_Expression> *> variable_ptrs(
            VARIABLES_SIZE);
        std::vector<std::unordered_set<Variable<T_Variable, T_Expression> *>>
            associated_variables_ptrs(VARIABLES_SIZE);

        int move_index = 0;
        for (auto &&association : associations) {
            auto  variable_ptr             = association.first;
            auto &associated_variable_ptrs = association.second;

            variable_ptrs[move_index]             = variable_ptr;
            associated_variables_ptrs[move_index] = associated_variable_ptrs;

            m_exclusive_moves[move_index].sense = MoveSense::Exclusive;
            m_exclusive_moves[move_index].alterations.emplace_back(variable_ptr,
                                                                   1);

            m_exclusive_moves[move_index].is_special_neighborhood_move = true;
            m_exclusive_moves[move_index].is_available                 = true;
            m_exclusive_moves[move_index].overlap_rate                 = 0.0;

            m_exclusive_moves[move_index].related_constraint_ptrs.insert(
                variable_ptr->related_constraint_ptrs().begin(),
                variable_ptr->related_constraint_ptrs().end());

            for (auto &&associated_variable_ptr : associated_variable_ptrs) {
                m_exclusive_moves[move_index].alterations.emplace_back(
                    associated_variable_ptr, 0);
                m_exclusive_moves[move_index].related_constraint_ptrs.insert(
                    associated_variable_ptr->related_constraint_ptrs().begin(),
                    associated_variable_ptr->related_constraint_ptrs().end());
            }

            move_index++;
        }

        auto exclusive_move_updater =                           //
            [this](auto *     a_moves,                          //
                   auto *     a_flags,                          //
                   const bool a_ACCEPT_ALL,                     //
                   const bool a_ACCEPT_OBJECTIVE_IMPROVABLE,    //
                   const bool a_ACCEPT_FEASIBILITY_IMPROVABLE,  //
                   [[maybe_unused]] const bool a_IS_ENABLED_PARALLEL) {
                const int MOVES_SIZE = a_moves->size();
#ifdef _OPENMP
#pragma omp parallel for if (a_IS_ENABLED_PARALLEL) schedule(static)
#endif
                for (auto i = 0; i < MOVES_SIZE; i++) {
                    (*a_flags)[i] = 1;
                    if (!(*a_moves)[i].is_available) {
                        (*a_flags)[i] = 0;
                        continue;
                    }
                    if ((*a_moves)[i].alterations[0].first->value() == 1) {
                        (*a_flags)[i] = 0;
                        continue;
                    }
                    if (a_ACCEPT_ALL) {
                        /** nothing to do */
                    } else {
                        if (a_ACCEPT_OBJECTIVE_IMPROVABLE &&
                            model::has_objective_improvable_variable(
                                (*a_moves)[i])) {
                            continue;
                        }

                        if (a_ACCEPT_FEASIBILITY_IMPROVABLE &&
                            model::has_feasibility_improvable_variable(
                                (*a_moves)[i])) {
                            continue;
                        }
                        (*a_flags)[i] = 0;
                    }
                }
            };
        m_exclusive_move_updater = exclusive_move_updater;
    }

    /*************************************************************************/
    constexpr void setup_selection_move_updater(
        std::vector<Variable<T_Variable, T_Expression> *> &a_VARIABLE_PTRS) {
        /**
         *  "Swap" move for binary variables in selection
         * constraints: e.g.) selection constraint x + y + z = 1 (x,
         * y, z \in {0, 1}) move: {(x = 0, y = 1), (x = 0, z = 1)}
         * (if x = 1, y = 0, z = 0)
         */

        const int VARIABLES_SIZE = a_VARIABLE_PTRS.size();
        m_selection_moves.resize(VARIABLES_SIZE);
        m_selection_move_flags.resize(VARIABLES_SIZE);

        for (auto i = 0; i < VARIABLES_SIZE; i++) {
            m_selection_moves[i].sense = MoveSense::Selection;
            m_selection_moves[i].related_constraint_ptrs =
                a_VARIABLE_PTRS[i]->selection_ptr()->related_constraint_ptrs;
            m_selection_moves[i].is_special_neighborhood_move = false;
            m_selection_moves[i].is_available                 = true;
            m_selection_moves[i].overlap_rate                 = 0.0;
        }

        auto selection_move_updater =  //
            [this, a_VARIABLE_PTRS, VARIABLES_SIZE](
                auto *                      a_moves,                          //
                auto *                      a_flags,                          //
                const bool                  a_ACCEPT_ALL,                     //
                const bool                  a_ACCEPT_OBJECTIVE_IMPROVABLE,    //
                const bool                  a_ACCEPT_FEASIBILITY_IMPROVABLE,  //
                [[maybe_unused]] const bool a_IS_ENABLED_PARALLEL) {
#ifdef _OPENMP
#pragma omp parallel for if (a_IS_ENABLED_PARALLEL) schedule(static)
#endif
                for (auto i = 0; i < VARIABLES_SIZE; i++) {
                    (*a_moves)[i].alterations.clear();
                    (*a_moves)[i].alterations.emplace_back(
                        a_VARIABLE_PTRS[i]
                            ->selection_ptr()
                            ->selected_variable_ptr,
                        0);

                    (*a_moves)[i].alterations.emplace_back(a_VARIABLE_PTRS[i],
                                                           1);
                }

                const int MOVES_SIZE = a_moves->size();

#ifdef _OPENMP
#pragma omp parallel for if (a_IS_ENABLED_PARALLEL) schedule(static)
#endif
                for (auto i = 0; i < MOVES_SIZE; i++) {
                    (*a_flags)[i] = 1;
                    if (m_has_fixed_variables &&
                        model::has_fixed_variables((*a_moves)[i])) {
                        (*a_flags)[i] = 0;
                        continue;
                    }
                    if (((*a_moves)[i].alterations[0].first ==
                         (*a_moves)[i].alterations[1].first)) {
                        (*a_flags)[i] = 0;
                        continue;
                    }

                    if (a_ACCEPT_ALL) {
                        /** nothing to do */
                    } else {
                        if (a_ACCEPT_OBJECTIVE_IMPROVABLE &&
                            model::has_objective_improvable_variable(
                                (*a_moves)[i])) {
                            continue;
                        }

                        if (a_ACCEPT_FEASIBILITY_IMPROVABLE &&
                            model::has_feasibility_improvable_variable(
                                (*a_moves)[i])) {
                            continue;
                        }
                        (*a_flags)[i] = 0;
                    }
                }
            };
        m_selection_move_updater = selection_move_updater;
    }

    /*************************************************************************/
    constexpr void setup_chain_move_updater(void) {
        auto chain_move_updater =                               //
            [this](auto *     a_moves,                          //
                   auto *     a_flags,                          //
                   const bool a_ACCEPT_ALL,                     //
                   const bool a_ACCEPT_OBJECTIVE_IMPROVABLE,    //
                   const bool a_ACCEPT_FEASIBILITY_IMPROVABLE,  //
                   [[maybe_unused]] const bool a_IS_ENABLED_PARALLEL) {
                const int MOVES_SIZE = a_moves->size();
#ifdef _OPENMP
#pragma omp parallel for if (a_IS_ENABLED_PARALLEL) schedule(static)
#endif
                for (auto i = 0; i < MOVES_SIZE; i++) {
                    (*a_flags)[i] = 1;
                    if (!(*a_moves)[i].is_available) {
                        (*a_flags)[i] = 0;
                        continue;
                    }
                    if (m_has_fixed_variables &&
                        model::has_fixed_variables((*a_moves)[i])) {
                        (*a_flags)[i] = 0;
                        continue;
                    }
                    for (const auto &alteration : (*a_moves)[i].alterations) {
                        if (alteration.first->value() == alteration.second) {
                            (*a_flags)[i] = 0;
                            break;
                        }
                    }
                    if ((*a_flags)[i] == 0) {
                        continue;
                    }

                    if (a_ACCEPT_ALL) {
                        /** nothing to do */
                    } else {
                        if (a_ACCEPT_OBJECTIVE_IMPROVABLE &&
                            model::has_objective_improvable_variable(
                                (*a_moves)[i])) {
                            continue;
                        }

                        if (a_ACCEPT_FEASIBILITY_IMPROVABLE &&
                            model::has_feasibility_improvable_variable(
                                (*a_moves)[i])) {
                            continue;
                        }
                        (*a_flags)[i] = 0;
                    }
                }
            };
        m_chain_move_updater = chain_move_updater;
    }

    /*************************************************************************/
    inline constexpr void register_chain_move(
        const Move<T_Variable, T_Expression> &a_MOVE) {
        m_chain_moves.push_back(a_MOVE);
        m_chain_move_flags.resize(m_chain_moves.size());
    }

    /*************************************************************************/
    inline constexpr void clear_chain_moves() {
        m_chain_moves.clear();
        m_chain_move_flags.clear();
    }

    /*************************************************************************/
    constexpr void deduplicate_chain_moves() {
        m_chain_moves.erase(std::unique(m_chain_moves.begin(),  //
                                        m_chain_moves.end()),
                            m_chain_moves.end());
        m_chain_move_flags.resize(m_chain_moves.size());
    }

    /*************************************************************************/
    inline constexpr void sort_chain_moves(void) {
        std::sort(m_chain_moves.begin(), m_chain_moves.end(),
                  [](auto const &a_LHS, auto const &a_RHS) {
                      return a_LHS.overlap_rate > a_RHS.overlap_rate;
                  });
        /*
                for (const auto &move : m_chain_moves) {
                    std::cout << &move << " " << move.overlap_rate << std::endl;
                    for (const auto &alteration : move.alterations) {
                        std::cout << alteration.first->name() << ": "
                                  << alteration.second << " ";
                    }
                    std::cout << std::endl;
                }
                */
    }

    /*************************************************************************/
    inline constexpr void shuffle_chain_moves(std::mt19937 *a_rand) {
        std::shuffle(m_chain_moves.begin(), m_chain_moves.end(), *a_rand);
    }

    /*************************************************************************/
    inline constexpr void reduce_chain_moves(const int a_NUMBER_OF_MOVES) {
        if (static_cast<int>(m_chain_moves.size()) <= a_NUMBER_OF_MOVES) {
            return;
        }

        m_chain_moves.resize(a_NUMBER_OF_MOVES);
        m_chain_move_flags.resize(a_NUMBER_OF_MOVES);
    }

    /*************************************************************************/
    inline constexpr void set_user_defined_move_updater(
        const std::function<void(std::vector<Move<T_Variable, T_Expression>> *)>
            &a_FUNCTION) {
        m_user_defined_move_updater = a_FUNCTION;
    }

    /*************************************************************************/
    constexpr void setup_user_defined_move_updater(void) {
        auto user_defined_move_updater_wrapper =                //
            [this](auto *     a_moves,                          //
                   auto *     a_flags,                          //
                   const bool a_ACCEPT_ALL,                     //
                   const bool a_ACCEPT_OBJECTIVE_IMPROVABLE,    //
                   const bool a_ACCEPT_FEASIBILITY_IMPROVABLE,  //
                   [[maybe_unused]] const bool a_IS_ENABLED_PARALLEL) {
                m_user_defined_move_updater(a_moves);
                const int MOVES_SIZE = a_moves->size();
                a_flags->resize(MOVES_SIZE);

#ifdef _OPENMP
#pragma omp parallel for if (a_IS_ENABLED_PARALLEL) schedule(static)
#endif
                for (auto i = 0; i < MOVES_SIZE; i++) {
                    (*a_flags)[i] = 1;
                    if (m_has_fixed_variables &&
                        model::has_fixed_variables((*a_moves)[i])) {
                        (*a_flags)[i] = 0;
                        continue;
                    }
                    if (m_has_selection_variables &&
                        model::has_selection_variables((*a_moves)[i])) {
                        (*a_flags)[i] = 0;
                        continue;
                    }
                    if (model::has_bound_violation((*a_moves)[i])) {
                        (*a_flags)[i] = 0;
                        continue;
                    }
                    if (a_ACCEPT_ALL) {
                        /** nothing to do */
                    } else {
                        if (a_ACCEPT_OBJECTIVE_IMPROVABLE &&
                            model::has_objective_improvable_variable(
                                (*a_moves)[i])) {
                            continue;
                        }

                        if (a_ACCEPT_FEASIBILITY_IMPROVABLE &&
                            model::has_feasibility_improvable_variable(
                                (*a_moves)[i])) {
                            continue;
                        }
                        (*a_flags)[i] = 0;
                    }
                }
            };
        m_user_defined_move_updater_wrapper = user_defined_move_updater_wrapper;
    }

    /*************************************************************************/
    constexpr void update_moves(
        const bool                  a_ACCEPT_ALL,                     //
        const bool                  a_ACCEPT_OBJECTIVE_IMPROVABLE,    //
        const bool                  a_ACCEPT_FEASIBILITY_IMPROVABLE,  //
        [[maybe_unused]] const bool a_IS_ENABLED_PARALLEL) {
        auto &binary_moves         = m_binary_moves;
        auto &integer_moves        = m_integer_moves;
        auto &precedence_moves     = m_precedence_moves;
        auto &aggregation_moves    = m_aggregation_moves;
        auto &variable_bound_moves = m_variable_bound_moves;
        auto &exclusive_moves      = m_exclusive_moves;
        auto &selection_moves      = m_selection_moves;
        auto &chain_moves          = m_chain_moves;
        auto &user_defined_moves   = m_user_defined_moves;

        auto &move_ptrs = m_move_ptrs;

        auto &binary_move_flags         = m_binary_move_flags;
        auto &integer_move_flags        = m_integer_move_flags;
        auto &precedence_move_flags     = m_precedence_move_flags;
        auto &aggregation_move_flags    = m_aggregation_move_flags;
        auto &variable_bound_move_flags = m_variable_bound_move_flags;
        auto &exclusive_move_flags      = m_exclusive_move_flags;
        auto &selection_move_flags      = m_selection_move_flags;
        auto &chain_move_flags          = m_chain_move_flags;
        auto &user_defined_move_flags   = m_user_defined_move_flags;

        const int BINARY_MOVES_SIZE         = binary_moves.size();
        const int INTEGER_MOVES_SIZE        = integer_moves.size();
        const int PRECEDENCE_MOVES_SIZE     = precedence_moves.size();
        const int AGGREGATION_MOVES_SIZE    = aggregation_moves.size();
        const int VARIABLE_BOUND_MOVES_SIZE = variable_bound_moves.size();
        const int EXCLUSIVE_MOVES_SIZE      = exclusive_moves.size();
        const int SELECTION_MOVES_SIZE      = selection_moves.size();
        const int CHAIN_MOVES_SIZE          = chain_moves.size();
        int       user_defined_moves_size   = 0;  /// computed later

        auto number_of_candidate_moves = 0;
        /// Binary
        if (BINARY_MOVES_SIZE > 0 &&  //
            m_is_enabled_binary_move) {
            m_binary_move_updater(&binary_moves,                    //
                                  &binary_move_flags,               //
                                  a_ACCEPT_ALL,                     //
                                  a_ACCEPT_OBJECTIVE_IMPROVABLE,    //
                                  a_ACCEPT_FEASIBILITY_IMPROVABLE,  //
                                  a_IS_ENABLED_PARALLEL);
            number_of_candidate_moves +=
                std::count(binary_move_flags.begin(),  //
                           binary_move_flags.end(), 1);
        }

        /// Integer
        if (INTEGER_MOVES_SIZE > 0 &&  //
            m_is_enabled_integer_move) {
            m_integer_move_updater(&integer_moves,                   //
                                   &integer_move_flags,              //
                                   a_ACCEPT_ALL,                     //
                                   a_ACCEPT_OBJECTIVE_IMPROVABLE,    //
                                   a_ACCEPT_FEASIBILITY_IMPROVABLE,  //
                                   a_IS_ENABLED_PARALLEL);
            number_of_candidate_moves +=
                std::count(integer_move_flags.begin(),  //
                           integer_move_flags.end(), 1);
        }

        /// Aggregation
        if (AGGREGATION_MOVES_SIZE > 0 &&  //
            m_is_enabled_aggregation_move) {
            m_aggregation_move_updater(&aggregation_moves,               //
                                       &aggregation_move_flags,          //
                                       a_ACCEPT_ALL,                     //
                                       a_ACCEPT_OBJECTIVE_IMPROVABLE,    //
                                       a_ACCEPT_FEASIBILITY_IMPROVABLE,  //
                                       a_IS_ENABLED_PARALLEL);
            number_of_candidate_moves +=
                std::count(aggregation_move_flags.begin(),  //
                           aggregation_move_flags.end(), 1);
        }

        /// Precedence
        if (PRECEDENCE_MOVES_SIZE > 0 &&  //
            m_is_enabled_precedence_move) {
            m_precedence_move_updater(&precedence_moves,                //
                                      &precedence_move_flags,           //
                                      a_ACCEPT_ALL,                     //
                                      a_ACCEPT_OBJECTIVE_IMPROVABLE,    //
                                      a_ACCEPT_FEASIBILITY_IMPROVABLE,  //
                                      a_IS_ENABLED_PARALLEL);
            number_of_candidate_moves +=
                std::count(precedence_move_flags.begin(),  //
                           precedence_move_flags.end(), 1);
        }

        /// Variable Bound
        if (VARIABLE_BOUND_MOVES_SIZE > 0 &&  //
            m_is_enabled_variable_bound_move) {
            m_variable_bound_move_updater(&variable_bound_moves,            //
                                          &variable_bound_move_flags,       //
                                          a_ACCEPT_ALL,                     //
                                          a_ACCEPT_OBJECTIVE_IMPROVABLE,    //
                                          a_ACCEPT_FEASIBILITY_IMPROVABLE,  //
                                          a_IS_ENABLED_PARALLEL);
            number_of_candidate_moves +=
                std::count(variable_bound_move_flags.begin(),  //
                           variable_bound_move_flags.end(), 1);
        }

        /// Exclusive
        if (EXCLUSIVE_MOVES_SIZE > 0 &&  //
            m_is_enabled_exclusive_move) {
            m_exclusive_move_updater(&exclusive_moves,                 //
                                     &exclusive_move_flags,            //
                                     a_ACCEPT_ALL,                     //
                                     a_ACCEPT_OBJECTIVE_IMPROVABLE,    //
                                     a_ACCEPT_FEASIBILITY_IMPROVABLE,  //
                                     a_IS_ENABLED_PARALLEL);
            number_of_candidate_moves +=
                std::count(exclusive_move_flags.begin(),  //
                           exclusive_move_flags.end(), 1);
        }

        /// Selection
        if (SELECTION_MOVES_SIZE > 0 &&  //
            m_is_enabled_selection_move) {
            m_selection_move_updater(&selection_moves,                 //
                                     &selection_move_flags,            //
                                     a_ACCEPT_ALL,                     //
                                     a_ACCEPT_OBJECTIVE_IMPROVABLE,    //
                                     a_ACCEPT_FEASIBILITY_IMPROVABLE,  //
                                     a_IS_ENABLED_PARALLEL);
            number_of_candidate_moves +=
                std::count(selection_move_flags.begin(),  //
                           selection_move_flags.end(), 1);
        }

        /// Chain
        if (m_is_enabled_chain_move) {
            m_chain_move_updater(&chain_moves,                     //
                                 &chain_move_flags,                //
                                 a_ACCEPT_ALL,                     //
                                 a_ACCEPT_OBJECTIVE_IMPROVABLE,    //
                                 a_ACCEPT_FEASIBILITY_IMPROVABLE,  //
                                 a_IS_ENABLED_PARALLEL);

            number_of_candidate_moves +=
                std::count(chain_move_flags.begin(),  //
                           chain_move_flags.end(), 1);
        }

        /// User Defined
        if (m_is_enabled_user_defined_move) {
            m_user_defined_move_updater_wrapper(
                &user_defined_moves,              //
                &user_defined_move_flags,         //
                a_ACCEPT_ALL,                     //
                a_ACCEPT_OBJECTIVE_IMPROVABLE,    //
                a_ACCEPT_FEASIBILITY_IMPROVABLE,  //
                a_IS_ENABLED_PARALLEL);
            user_defined_moves_size = user_defined_moves.size();
            number_of_candidate_moves +=
                std::count(user_defined_move_flags.begin(),  //
                           user_defined_move_flags.end(), 1);
        }

        move_ptrs.resize(number_of_candidate_moves);
        auto index = 0;

        if (m_is_enabled_binary_move) {
            for (auto i = 0; i < BINARY_MOVES_SIZE; i++) {
                if (binary_move_flags[i]) {
                    move_ptrs[index++] = &binary_moves[i];
                }
            }
        }

        if (m_is_enabled_integer_move) {
            for (auto i = 0; i < INTEGER_MOVES_SIZE; i++) {
                if (integer_move_flags[i]) {
                    move_ptrs[index++] = &integer_moves[i];
                }
            }
        }

        if (m_is_enabled_precedence_move) {
            for (auto i = 0; i < PRECEDENCE_MOVES_SIZE; i++) {
                if (precedence_move_flags[i]) {
                    move_ptrs[index++] = &precedence_moves[i];
                }
            }
        }

        if (m_is_enabled_aggregation_move) {
            for (auto i = 0; i < AGGREGATION_MOVES_SIZE; i++) {
                if (aggregation_move_flags[i]) {
                    move_ptrs[index++] = &aggregation_moves[i];
                }
            }
        }

        if (m_is_enabled_variable_bound_move) {
            for (auto i = 0; i < VARIABLE_BOUND_MOVES_SIZE; i++) {
                if (variable_bound_move_flags[i]) {
                    move_ptrs[index++] = &variable_bound_moves[i];
                }
            }
        }

        if (m_is_enabled_exclusive_move) {
            for (auto i = 0; i < EXCLUSIVE_MOVES_SIZE; i++) {
                if (exclusive_move_flags[i]) {
                    move_ptrs[index++] = &exclusive_moves[i];
                }
            }
        }

        if (m_is_enabled_selection_move) {
            for (auto i = 0; i < SELECTION_MOVES_SIZE; i++) {
                if (selection_move_flags[i]) {
                    move_ptrs[index++] = &selection_moves[i];
                }
            }
        }

        if (m_is_enabled_chain_move) {
            for (auto i = 0; i < CHAIN_MOVES_SIZE; i++) {
                if (chain_move_flags[i]) {
                    move_ptrs[index++] = &chain_moves[i];
                }
            }
        }

        if (m_is_enabled_user_defined_move) {
            for (auto i = 0; i < user_defined_moves_size; i++) {
                if (user_defined_move_flags[i]) {
                    move_ptrs[index++] = &user_defined_moves[i];
                }
            }
        }
    }

    /*************************************************************************/
    inline constexpr void shuffle_moves(std::mt19937 *a_rand) noexcept {
        std::shuffle(m_move_ptrs.begin(), m_move_ptrs.end(), *a_rand);
    }

    /*************************************************************************/
    inline constexpr const std::vector<Move<T_Variable, T_Expression>>
        &binary_moves(void) const {
        return m_binary_moves;
    }

    /*************************************************************************/
    inline constexpr const std::vector<int> &binary_move_flags(void) const {
        return m_binary_move_flags;
    }

    /*************************************************************************/
    inline constexpr const std::vector<Move<T_Variable, T_Expression>>
        &integer_moves(void) const {
        return m_integer_moves;
    }

    /*************************************************************************/
    inline constexpr const std::vector<int> &integer_move_flags(void) const {
        return m_integer_move_flags;
    }

    /*************************************************************************/
    inline constexpr const std::vector<Move<T_Variable, T_Expression>>
        &aggregation_moves(void) const {
        return m_aggregation_moves;
    }

    /*************************************************************************/
    inline constexpr const std::vector<int> &aggregation_move_flags(
        void) const {
        return m_aggregation_move_flags;
    }

    /*************************************************************************/
    inline constexpr const std::vector<Move<T_Variable, T_Expression>>
        &precedence_moves(void) const {
        return m_precedence_moves;
    }

    /*************************************************************************/
    inline constexpr const std::vector<int> &precedence_move_flags(void) const {
        return m_precedence_move_flags;
    }

    /*************************************************************************/
    inline constexpr const std::vector<Move<T_Variable, T_Expression>>
        &variable_bound_moves(void) const {
        return m_variable_bound_moves;
    }

    /*************************************************************************/
    inline constexpr const std::vector<int> &variable_bound_move_flags(
        void) const {
        return m_variable_bound_move_flags;
    }

    /*************************************************************************/
    inline constexpr const std::vector<Move<T_Variable, T_Expression>>
        &exclusive_moves(void) const {
        return m_exclusive_moves;
    }

    /*************************************************************************/
    inline constexpr const std::vector<int> &exclusive_move_flags(void) const {
        return m_exclusive_move_flags;
    }

    /*************************************************************************/
    inline constexpr const std::vector<Move<T_Variable, T_Expression>>
        &selection_moves(void) const {
        return m_selection_moves;
    }

    /*************************************************************************/
    inline constexpr const std::vector<int> &selection_move_flags(void) const {
        return m_selection_move_flags;
    }

    /*************************************************************************/
    inline constexpr const std::vector<Move<T_Variable, T_Expression>>
        &chain_moves(void) const {
        return m_chain_moves;
    }

    /*************************************************************************/
    inline constexpr const std::vector<int> &chain_move_flags(void) const {
        return m_chain_move_flags;
    }

    /*************************************************************************/
    inline constexpr const std::vector<Move<T_Variable, T_Expression>>
        &user_defined_moves(void) const {
        return m_user_defined_moves;
    }

    /*************************************************************************/
    inline constexpr const std::vector<int> &user_defined_move_flags(
        void) const {
        return m_user_defined_move_flags;
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
    inline constexpr bool has_selection_variables(void) const {
        return m_has_selection_variables;
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
    inline constexpr bool is_enabled_aggregation_move(void) const {
        return m_is_enabled_aggregation_move;
    }

    /*************************************************************************/
    inline constexpr void enable_aggregation_move(void) {
        m_is_enabled_aggregation_move = true;
    }

    /*************************************************************************/
    inline constexpr void disable_aggregation_move(void) {
        m_is_enabled_aggregation_move = false;
    }

    /*************************************************************************/
    inline constexpr bool is_enabled_precedence_move(void) const {
        return m_is_enabled_precedence_move;
    }

    /*************************************************************************/
    inline constexpr void enable_precedence_move(void) {
        m_is_enabled_precedence_move = true;
    }

    /*************************************************************************/
    inline constexpr void disable_precedence_move(void) {
        m_is_enabled_precedence_move = false;
    }

    /*************************************************************************/
    inline constexpr bool is_enabled_variable_bound_move(void) const {
        return m_is_enabled_variable_bound_move;
    }

    /*************************************************************************/
    inline constexpr void enable_variable_bound_move(void) {
        m_is_enabled_variable_bound_move = true;
    }

    /*************************************************************************/
    inline constexpr void disable_variable_bound_move(void) {
        m_is_enabled_variable_bound_move = false;
    }

    /*************************************************************************/
    inline constexpr bool is_enabled_exclusive_move(void) const {
        return m_is_enabled_exclusive_move;
    }

    /*************************************************************************/
    inline constexpr void enable_exclusive_move(void) {
        m_is_enabled_exclusive_move = true;
    }

    /*************************************************************************/
    inline constexpr void disable_exclusive_move(void) {
        m_is_enabled_exclusive_move = false;
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
    inline constexpr bool is_enabled_chain_move(void) const {
        return m_is_enabled_chain_move;
    }

    /*************************************************************************/
    inline constexpr void enable_chain_move(void) {
        m_is_enabled_chain_move = true;
    }

    /*************************************************************************/
    inline constexpr void disable_chain_move(void) {
        m_is_enabled_chain_move = false;
    }

    /*************************************************************************/
    inline constexpr bool is_enabled_user_defined_move(void) const {
        return m_is_enabled_user_defined_move;
    }

    /*************************************************************************/
    inline constexpr void enable_user_defined_move(void) {
        m_is_enabled_user_defined_move = true;
    }

    /*************************************************************************/
    inline constexpr void disable_user_defined_move(void) {
        m_is_enabled_user_defined_move = false;
    }

    /*************************************************************************/
    inline constexpr void reset_special_neighborhood_moves_availability(
        void) noexcept {
        for (auto &move : m_precedence_moves) {
            move.is_available = true;
        }
        for (auto &move : m_aggregation_moves) {
            move.is_available = true;
        }
        for (auto &move : m_variable_bound_moves) {
            move.is_available = true;
        }
        for (auto &move : m_exclusive_moves) {
            move.is_available = true;
        }
        for (auto &move : m_chain_moves) {
            move.is_available = true;
        }
    }

    /*************************************************************************/
    inline constexpr bool is_enabled_special_neighborhood_move(void) const {
        if (m_is_enabled_aggregation_move) {
            return true;
        }

        if (m_is_enabled_precedence_move) {
            return true;
        }

        if (m_is_enabled_variable_bound_move) {
            return true;
        }

        if (m_is_enabled_exclusive_move) {
            return true;
        }

        if (m_is_enabled_chain_move) {
            return true;
        }

        return false;
    }

};  // namespace model
}  // namespace model
}  // namespace printemps
#endif
/*****************************************************************************/
// END
/*****************************************************************************/