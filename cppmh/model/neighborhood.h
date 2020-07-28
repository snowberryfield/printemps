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
template <class T_Variable, class T_Expression>
class Neighborhood {
   private:
    std::function<void(std::vector<Move<T_Variable, T_Expression>> *)>
        m_binary_move_updater;
    std::function<void(std::vector<Move<T_Variable, T_Expression>> *)>
        m_integer_move_updater;
    std::function<void(std::vector<Move<T_Variable, T_Expression>> *)>
        m_precedence_move_updater;
    std::function<void(std::vector<Move<T_Variable, T_Expression>> *)>
        m_aggregation_move_updater;
    std::function<void(std::vector<Move<T_Variable, T_Expression>> *)>
        m_variable_bound_move_updater;
    std::function<void(std::vector<Move<T_Variable, T_Expression>> *)>
        m_user_defined_move_updater;
    std::function<void(std::vector<Move<T_Variable, T_Expression>> *)>
        m_selection_move_updater;

    std::vector<Move<T_Variable, T_Expression>> m_binary_moves;
    std::vector<Move<T_Variable, T_Expression>> m_integer_moves;
    std::vector<Move<T_Variable, T_Expression>> m_precedence_moves;
    std::vector<Move<T_Variable, T_Expression>> m_aggregation_moves;
    std::vector<Move<T_Variable, T_Expression>> m_variable_bound_moves;
    std::vector<Move<T_Variable, T_Expression>> m_user_defined_moves;
    std::vector<Move<T_Variable, T_Expression>> m_selection_moves;

    std::vector<Move<T_Variable, T_Expression> *> m_move_ptrs;

    bool m_has_fixed_variables;
    bool m_has_selection_variables;

    bool m_is_enabled_binary_move;
    bool m_is_enabled_integer_move;
    bool m_is_enabled_precedence_move;
    bool m_is_enabled_aggregation_move;
    bool m_is_enabled_variable_bound_move;
    bool m_is_enabled_user_defined_move;
    bool m_is_enabled_selection_move;

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
        m_binary_move_updater =
            [](std::vector<Move<T_Variable, T_Expression>> *) {};
        m_integer_move_updater =
            [](std::vector<Move<T_Variable, T_Expression>> *) {};
        m_precedence_move_updater =
            [](std::vector<Move<T_Variable, T_Expression>> *) {};
        m_aggregation_move_updater =
            [](std::vector<Move<T_Variable, T_Expression>> *) {};
        m_variable_bound_move_updater =
            [](std::vector<Move<T_Variable, T_Expression>> *) {};
        m_user_defined_move_updater =
            [](std::vector<Move<T_Variable, T_Expression>> *) {};
        m_selection_move_updater =
            [](std::vector<Move<T_Variable, T_Expression>> *) {};

        m_binary_moves.clear();
        m_integer_moves.clear();
        m_precedence_moves.clear();
        m_aggregation_moves.clear();
        m_variable_bound_moves.clear();
        m_user_defined_moves.clear();
        m_selection_moves.clear();

        m_move_ptrs.clear();

        m_has_fixed_variables     = false;
        m_has_selection_variables = false;

        m_is_enabled_binary_move         = false;
        m_is_enabled_integer_move        = false;
        m_is_enabled_precedence_move     = false;
        m_is_enabled_aggregation_move    = false;
        m_is_enabled_variable_bound_move = false;
        m_is_enabled_user_defined_move   = false;
        m_is_enabled_selection_move      = false;
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
    inline constexpr void setup_aggregation_move_updater(
        std::vector<Constraint<T_Variable, T_Expression> *> &a_CONSTRAINT_PTRS,
        const bool a_IS_ENABLED_PARALLEL) {
        int raw_constraints_size = a_CONSTRAINT_PTRS.size();

        std::vector<std::vector<Variable<T_Variable, T_Expression> *>>
                                               variable_ptr_pairs;
        std::vector<std::vector<T_Expression>> sensitivity_pairs;
        std::vector<T_Expression>              constants;

        for (auto i = 0; i < raw_constraints_size; i++) {
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
                variable_ptr_pairs.push_back(variable_ptr_pair);
                sensitivity_pairs.push_back(sensitivity_pair);
                constants.push_back(expression.constant_value());
            }
        }

        int constraints_size = variable_ptr_pairs.size();
        m_aggregation_moves.resize(4 * constraints_size);

        for (auto i = 0; i < constraints_size; i++) {
            m_aggregation_moves[4 * i].sense = MoveSense::Aggregation;
            m_aggregation_moves[4 * i].related_constraint_ptrs.insert(
                variable_ptr_pairs[i][0]->related_constraint_ptrs().begin(),
                variable_ptr_pairs[i][0]->related_constraint_ptrs().end());
            m_aggregation_moves[4 * i].related_constraint_ptrs.insert(
                variable_ptr_pairs[i][1]->related_constraint_ptrs().begin(),
                variable_ptr_pairs[i][1]->related_constraint_ptrs().end());

            m_aggregation_moves[4 * i + 1] = m_aggregation_moves[4 * i];
            m_aggregation_moves[4 * i + 2] = m_aggregation_moves[4 * i];
            m_aggregation_moves[4 * i + 3] = m_aggregation_moves[4 * i];
        }

        auto aggregation_move_updater = [this, variable_ptr_pairs,
                                         sensitivity_pairs, constants,
                                         constraints_size,
                                         a_IS_ENABLED_PARALLEL](auto *a_moves) {

#ifdef _OPENMP
#pragma omp parallel for if (a_IS_ENABLED_PARALLEL) schedule(static)
#endif
            for (auto i = 0; i < constraints_size; i++) {
                T_Variable value_pair[2] = {variable_ptr_pairs[i][0]->value(),
                                            variable_ptr_pairs[i][1]->value()};

                for (auto j = 0; j < 2; j++) {
                    auto index = 4 * i + 2 * j;

                    (*a_moves)[index].alterations.clear();
                    (*a_moves)[index].alterations.emplace_back(
                        variable_ptr_pairs[i][j], value_pair[j] + 1);
                    (*a_moves)[index].alterations.emplace_back(
                        variable_ptr_pairs[i][1 - j],
                        static_cast<T_Variable>(
                            (-constants[i] -
                             sensitivity_pairs[i][j] * (value_pair[j] + 1)) /
                            sensitivity_pairs[i][1 - j]));

                    index = 4 * i + 2 * j + 1;
                    (*a_moves)[index].alterations.clear();
                    (*a_moves)[index].alterations.emplace_back(
                        variable_ptr_pairs[i][j], value_pair[1 - j] - 1);
                    (*a_moves)[index].alterations.emplace_back(
                        variable_ptr_pairs[i][1 - j],
                        static_cast<T_Variable>(
                            (-constants[i] -
                             sensitivity_pairs[i][j] * (value_pair[j] - 1)) /
                            sensitivity_pairs[i][1 - j]));
                }
            }
        };
        m_aggregation_move_updater = aggregation_move_updater;
    }

    /*************************************************************************/
    inline constexpr void setup_precedence_move_updater(
        std::vector<Constraint<T_Variable, T_Expression> *> &a_CONSTRAINT_PTRS,
        const bool a_IS_ENABLED_PARALLEL) {
        int raw_constraints_size = a_CONSTRAINT_PTRS.size();

        std::vector<std::vector<Variable<T_Variable, T_Expression> *>>
            variable_ptr_pairs;

        for (auto i = 0; i < raw_constraints_size; i++) {
            if (a_CONSTRAINT_PTRS[i]->is_enabled()) {
                auto &sensitivities =
                    a_CONSTRAINT_PTRS[i]->expression().sensitivities();
                std::vector<Variable<T_Variable, T_Expression> *>
                    variable_ptr_pair;
                for (auto &&sensitivity : sensitivities) {
                    variable_ptr_pair.push_back(sensitivity.first);
                }
                variable_ptr_pairs.push_back(variable_ptr_pair);
            }
        }

        int constraints_size = variable_ptr_pairs.size();
        m_precedence_moves.resize(2 * constraints_size);

        for (auto i = 0; i < constraints_size; i++) {
            m_precedence_moves[2 * i].sense = MoveSense::Precedence;
            m_precedence_moves[2 * i].related_constraint_ptrs.insert(
                variable_ptr_pairs[i][0]->related_constraint_ptrs().begin(),
                variable_ptr_pairs[i][0]->related_constraint_ptrs().end());
            m_precedence_moves[2 * i].related_constraint_ptrs.insert(
                variable_ptr_pairs[i][1]->related_constraint_ptrs().begin(),
                variable_ptr_pairs[i][1]->related_constraint_ptrs().end());

            m_precedence_moves[2 * i + 1] = m_precedence_moves[2 * i];
        }

        auto precedence_move_updater = [this, variable_ptr_pairs,
                                        constraints_size,
                                        a_IS_ENABLED_PARALLEL](auto *a_moves) {

#ifdef _OPENMP
#pragma omp parallel for if (a_IS_ENABLED_PARALLEL) schedule(static)
#endif
            for (auto i = 0; i < constraints_size; i++) {
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
        };
        m_precedence_move_updater = precedence_move_updater;
    }

    /*************************************************************************/
    inline constexpr void setup_variable_bound_move_updater(
        std::vector<Constraint<T_Variable, T_Expression> *> &a_CONSTRAINT_PTRS,
        const bool a_IS_ENABLED_PARALLEL) {
        int raw_constraints_size = a_CONSTRAINT_PTRS.size();

        std::vector<std::vector<Variable<T_Variable, T_Expression> *>>
                                               variable_ptr_pairs;
        std::vector<std::vector<T_Expression>> sensitivity_pairs;
        std::vector<T_Expression>              constants;
        std::vector<ConstraintSense>           senses;

        for (auto i = 0; i < raw_constraints_size; i++) {
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

                variable_ptr_pairs.push_back(variable_ptr_pair);
                sensitivity_pairs.push_back(sensitivity_pair);
                constants.push_back(expression.constant_value());
                senses.push_back(a_CONSTRAINT_PTRS[i]->sense());
            }
        }

        int constraints_size = variable_ptr_pairs.size();
        m_variable_bound_moves.resize(4 * constraints_size);

        for (auto i = 0; i < constraints_size; i++) {
            m_variable_bound_moves[4 * i].sense = MoveSense::VariableBound;
            m_variable_bound_moves[4 * i].related_constraint_ptrs.insert(
                variable_ptr_pairs[i][0]->related_constraint_ptrs().begin(),
                variable_ptr_pairs[i][0]->related_constraint_ptrs().end());
            m_variable_bound_moves[4 * i].related_constraint_ptrs.insert(
                variable_ptr_pairs[i][1]->related_constraint_ptrs().begin(),
                variable_ptr_pairs[i][1]->related_constraint_ptrs().end());

            m_variable_bound_moves[4 * i + 1] = m_variable_bound_moves[4 * i];
            m_variable_bound_moves[4 * i + 2] = m_variable_bound_moves[4 * i];
            m_variable_bound_moves[4 * i + 3] = m_variable_bound_moves[4 * i];
        }

        auto variable_bound_move_updater =
            [this, variable_ptr_pairs, sensitivity_pairs, constants, senses,
             constraints_size, a_IS_ENABLED_PARALLEL](auto *a_moves) {
#ifdef _OPENMP
#pragma omp parallel for if (a_IS_ENABLED_PARALLEL) schedule(static)
#endif
                for (auto i = 0; i < constraints_size; i++) {
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
            };
        m_variable_bound_move_updater = variable_bound_move_updater;
    }

    /*************************************************************************/
    inline constexpr void set_user_defined_move_updater(
        const std::function<void(std::vector<Move<T_Variable, T_Expression>> *)>
            &a_FUNCTION) {
        m_user_defined_move_updater = a_FUNCTION;
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
    inline constexpr void update_moves(void) noexcept {
        if (m_binary_moves.size() > 0 &&  //
            m_is_enabled_binary_move) {
            m_binary_move_updater(&m_binary_moves);
        }

        if (m_integer_moves.size() > 0 &&  //
            m_is_enabled_integer_move) {
            m_integer_move_updater(&m_integer_moves);
        }

        if (m_aggregation_moves.size() > 0 &&  //
            m_is_enabled_aggregation_move) {
            m_aggregation_move_updater(&m_aggregation_moves);
        }

        if (m_precedence_moves.size() > 0 &&  //
            m_is_enabled_precedence_move) {
            m_precedence_move_updater(&m_precedence_moves);
        }

        if (m_variable_bound_moves.size() > 0 &&  //
            m_is_enabled_variable_bound_move) {
            m_variable_bound_move_updater(&m_variable_bound_moves);
        }

        if (m_is_enabled_user_defined_move) {
            m_user_defined_move_updater(&m_user_defined_moves);
        }

        if (m_selection_moves.size() > 0 &&  //
            m_is_enabled_selection_move) {
            m_selection_move_updater(&m_selection_moves);
        }

        auto number_of_candidate_moves = m_binary_moves.size()            //
                                         + m_integer_moves.size()         //
                                         + m_precedence_moves.size()      //
                                         + m_aggregation_moves.size()     //
                                         + m_variable_bound_moves.size()  //
                                         + m_user_defined_moves.size()    //
                                         + m_selection_moves.size();

        m_move_ptrs.resize(number_of_candidate_moves);

        auto has_fixed_variables = [this](const auto &a_MOVE) {
            if (!m_has_fixed_variables) {
                return false;
            }
            for (const auto &alteration : a_MOVE.alterations) {
                if (alteration.first->is_fixed()) {
                    return true;
                }
            }
            return false;
        };

        auto has_selection_variables = [this](const auto &a_MOVE) {
            if (!m_has_selection_variables) {
                return false;
            }
            for (const auto &alteration : a_MOVE.alterations) {
                if (alteration.first->sense() == VariableSense::Selection) {
                    return true;
                }
            }
            return false;
        };

        auto has_bound_violation = [this](const auto &a_MOVE) {
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

        auto index = 0;

        if (m_is_enabled_binary_move) {
            for (auto &&move : m_binary_moves) {
                if (has_fixed_variables(move)) {
                    continue;
                }
                if (has_selection_variables(move)) {
                    continue;
                }

                m_move_ptrs[index++] = &move;
            }
        }

        if (m_is_enabled_integer_move) {
            for (auto &&move : m_integer_moves) {
                if (has_fixed_variables(move)) {
                    continue;
                }
                if (has_selection_variables(move)) {
                    continue;
                }
                if (has_bound_violation(move)) {
                    continue;
                }

                m_move_ptrs[index++] = &move;
            }
        }

        if (m_is_enabled_aggregation_move) {
            for (auto &&move : m_aggregation_moves) {
                if (has_fixed_variables(move)) {
                    continue;
                }
                if (has_selection_variables(move)) {
                    continue;
                }
                if (has_bound_violation(move)) {
                    continue;
                }
                m_move_ptrs[index++] = &move;
            }
        }

        if (m_is_enabled_precedence_move) {
            for (auto &&move : m_precedence_moves) {
                if (has_fixed_variables(move)) {
                    continue;
                }
                if (has_selection_variables(move)) {
                    continue;
                }
                if (has_bound_violation(move)) {
                    continue;
                }
                m_move_ptrs[index++] = &move;
            }
        }

        if (m_is_enabled_variable_bound_move) {
            for (auto &&move : m_variable_bound_moves) {
                if (has_fixed_variables(move)) {
                    continue;
                }
                if (has_selection_variables(move)) {
                    continue;
                }
                if (has_bound_violation(move)) {
                    continue;
                }
                m_move_ptrs[index++] = &move;
            }
        }

        if (m_is_enabled_user_defined_move) {
            for (auto &&move : m_user_defined_moves) {
                if (has_fixed_variables(move)) {
                    continue;
                }
                if (has_selection_variables(move)) {
                    continue;
                }
                if (has_bound_violation(move)) {
                    continue;
                }
                m_move_ptrs[index++] = &move;
            }
        }

        if (m_is_enabled_selection_move) {
            for (auto &&move : m_selection_moves) {
                if (has_fixed_variables(move)) {
                    continue;
                }

                if (move.alterations[0].first == move.alterations[1].first) {
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
        &aggregation_moves(void) {
        return m_aggregation_moves;
    }

    /*************************************************************************/
    inline constexpr const std::vector<Move<T_Variable, T_Expression>>
        &precedence_moves(void) {
        return m_precedence_moves;
    }

    /*************************************************************************/
    inline constexpr const std::vector<Move<T_Variable, T_Expression>>
        &variable_bound_moves(void) {
        return m_variable_bound_moves;
    }

    /*************************************************************************/
    inline constexpr const std::vector<Move<T_Variable, T_Expression>>
        &user_defined_moves(void) {
        return m_user_defined_moves;
    }

    /*************************************************************************/
    inline constexpr const std::vector<Move<T_Variable, T_Expression>>
        &selection_moves(void) {
        return m_selection_moves;
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
};  // namespace model
}  // namespace model
}  // namespace cppmh
#endif
/*****************************************************************************/
// END
/*****************************************************************************/