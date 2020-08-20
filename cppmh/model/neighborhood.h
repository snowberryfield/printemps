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
inline constexpr bool has_fixed_variables(
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
inline constexpr bool has_selection_variables(
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
inline constexpr bool has_bound_violation(
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
inline constexpr bool has_improvable_variable(
    const Move<T_Variable, T_Expression> &a_MOVE) {
    for (const auto &alteration : a_MOVE.alterations) {
        if (alteration.first->is_improvable()) {
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
        m_exclusive_move_updater;
    std::function<void(std::vector<Move<T_Variable, T_Expression>> *)>
        m_user_defined_move_updater;
    std::function<void(std::vector<Move<T_Variable, T_Expression>> *)>
        m_selection_move_updater;

    std::vector<Move<T_Variable, T_Expression>> m_binary_moves;
    std::vector<Move<T_Variable, T_Expression>> m_integer_moves;
    std::vector<Move<T_Variable, T_Expression>> m_precedence_moves;
    std::vector<Move<T_Variable, T_Expression>> m_aggregation_moves;
    std::vector<Move<T_Variable, T_Expression>> m_variable_bound_moves;
    std::vector<Move<T_Variable, T_Expression>> m_exclusive_moves;
    std::vector<Move<T_Variable, T_Expression>> m_user_defined_moves;
    std::vector<Move<T_Variable, T_Expression>> m_selection_moves;

    std::vector<int> m_binary_move_flags;
    std::vector<int> m_integer_move_flags;
    std::vector<int> m_precedence_move_flags;
    std::vector<int> m_aggregation_move_flags;
    std::vector<int> m_variable_bound_move_flags;
    std::vector<int> m_exclusive_move_flags;
    std::vector<int> m_user_defined_move_flags;
    std::vector<int> m_selection_move_flags;

    std::vector<Move<T_Variable, T_Expression> *> m_move_ptrs;

    bool m_has_fixed_variables;
    bool m_has_selection_variables;

    bool m_is_enabled_binary_move;
    bool m_is_enabled_integer_move;
    bool m_is_enabled_precedence_move;
    bool m_is_enabled_aggregation_move;
    bool m_is_enabled_variable_bound_move;
    bool m_is_enabled_exclusive_move;
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
        m_exclusive_move_updater =
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
        m_exclusive_moves.clear();
        m_user_defined_moves.clear();
        m_selection_moves.clear();

        m_binary_move_flags.clear();
        m_integer_move_flags.clear();
        m_precedence_move_flags.clear();
        m_aggregation_move_flags.clear();
        m_variable_bound_move_flags.clear();
        m_exclusive_move_flags.clear();
        m_user_defined_move_flags.clear();
        m_selection_move_flags.clear();

        m_move_ptrs.clear();

        m_has_fixed_variables     = false;
        m_has_selection_variables = false;

        m_is_enabled_binary_move         = false;
        m_is_enabled_integer_move        = false;
        m_is_enabled_precedence_move     = false;
        m_is_enabled_aggregation_move    = false;
        m_is_enabled_variable_bound_move = false;
        m_is_enabled_exclusive_move      = false;
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
        const std::vector<Variable<T_Variable, T_Expression> *>
            &      a_VARIABLE_PTRS,
        const bool a_IS_ENABLED_PARALLEL) {
        /**
         * "Flip" move for binary variables:
         * e.g) binary variable x \in {0, 1}
         *  move: {(x = 1)} (if x = 0)
         *        {(x = 0)} (if x = 1)
         */
        int variables_size = a_VARIABLE_PTRS.size();
        m_binary_moves.resize(variables_size);
        m_binary_move_flags.resize(variables_size);

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
        const std::vector<Variable<T_Variable, T_Expression> *>
            &      a_VARIABLE_PTRS,
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
        m_integer_move_flags.resize(2 * variables_size);

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
        const std::vector<Constraint<T_Variable, T_Expression> *>
            &      a_CONSTRAINT_PTRS,
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
        m_aggregation_move_flags.resize(4 * constraints_size);

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
        const std::vector<Constraint<T_Variable, T_Expression> *>
            &      a_CONSTRAINT_PTRS,
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
        m_precedence_move_flags.resize(2 * constraints_size);

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
    inline void setup_variable_bound_move_updater(
        const std::vector<Constraint<T_Variable, T_Expression> *>
            &      a_CONSTRAINT_PTRS,
        const bool a_IS_ENABLED_PARALLEL) {
        /**
         * NOTE: This method cannot be constexpr by clang for
         * std::vector<ConstraintSense>.
         */
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
        m_variable_bound_move_flags.resize(4 * constraints_size);

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
    inline constexpr void setup_exclusive_move_updater(
        const std::vector<Constraint<T_Variable, T_Expression> *>
            &a_SET_PARTITIONING_PTRS,
        const std::vector<Constraint<T_Variable, T_Expression> *>
            &      a_SET_PACKING_PTRS,
        const bool a_IS_ENABLED_PARALLEL) {
        int set_partitionings_size = a_SET_PARTITIONING_PTRS.size();
        int set_packings_size      = a_SET_PACKING_PTRS.size();

        std::unordered_map<
            Variable<T_Variable, T_Expression> *,
            std::unordered_set<Variable<T_Variable, T_Expression> *>>
            associations;

        for (auto i = 0; i < set_partitionings_size; i++) {
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
                        associations[variable_ptr_first].insert(
                            variable_ptr_second);
                    }
                }
            }
        }

        for (auto i = 0; i < set_packings_size; i++) {
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
                        associations[variable_ptr_first].insert(
                            variable_ptr_second);
                    }
                }
            }
        }

        int variables_size = associations.size();
        m_exclusive_moves.resize(variables_size);
        m_exclusive_move_flags.resize(variables_size);

        std::vector<Variable<T_Variable, T_Expression> *> variable_ptrs(
            variables_size);
        std::vector<std::unordered_set<Variable<T_Variable, T_Expression> *>>
            associated_variables_ptrs(variables_size);

        int move_index = 0;
        for (auto &&association : associations) {
            auto  variable_ptr             = association.first;
            auto &associated_variable_ptrs = association.second;

            variable_ptrs[move_index]             = variable_ptr;
            associated_variables_ptrs[move_index] = associated_variable_ptrs;

            m_exclusive_moves[move_index].sense = MoveSense::Exclusive;
            m_exclusive_moves[move_index].related_constraint_ptrs.insert(
                variable_ptr->related_constraint_ptrs().begin(),
                variable_ptr->related_constraint_ptrs().end());
            for (auto &&associated_variable_ptr : associated_variable_ptrs) {
                m_exclusive_moves[move_index].related_constraint_ptrs.insert(
                    associated_variable_ptr->related_constraint_ptrs().begin(),
                    associated_variable_ptr->related_constraint_ptrs().end());
            }
            move_index++;
        }

        auto exclusive_move_updater =
            [this, variable_ptrs, associated_variables_ptrs, variables_size,
             a_IS_ENABLED_PARALLEL](auto *a_moves) {

#ifdef _OPENMP
#pragma omp parallel for if (a_IS_ENABLED_PARALLEL) schedule(static)
#endif
                for (auto i = 0; i < variables_size; i++) {
                    (*a_moves)[i].alterations.clear();
                    (*a_moves)[i].alterations.emplace_back(variable_ptrs[i], 1);
                    for (auto &&variable_ptr : associated_variables_ptrs[i]) {
                        (*a_moves)[i].alterations.emplace_back(variable_ptr, 0);
                    }
                }
            };
        m_exclusive_move_updater = exclusive_move_updater;
        exclusive_move_updater(&m_exclusive_moves);
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
        m_selection_move_flags.resize(variables_size);

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
    inline constexpr void update_moves(
        const bool                  a_IS_ENABLED_IMPROBABILITY_SCREENING,
        [[maybe_unused]] const bool a_IS_ENABLED_PARALLEL) {
        auto &binary_moves         = m_binary_moves;
        auto &integer_moves        = m_integer_moves;
        auto &precedence_moves     = m_precedence_moves;
        auto &aggregation_moves    = m_aggregation_moves;
        auto &variable_bound_moves = m_variable_bound_moves;
        auto &exclusive_moves      = m_exclusive_moves;
        auto &user_defined_moves   = m_user_defined_moves;
        auto &selection_moves      = m_selection_moves;
        auto &move_ptrs            = m_move_ptrs;

        auto &binary_move_flags         = m_binary_move_flags;
        auto &integer_move_flags        = m_integer_move_flags;
        auto &precedence_move_flags     = m_precedence_move_flags;
        auto &aggregation_move_flags    = m_aggregation_move_flags;
        auto &variable_bound_move_flags = m_variable_bound_move_flags;
        auto &exclusive_move_flags      = m_exclusive_move_flags;
        auto &user_defined_move_flags   = m_user_defined_move_flags;
        auto &selection_move_flags      = m_selection_move_flags;

        int binary_moves_size         = binary_moves.size();
        int integer_moves_size        = integer_moves.size();
        int precedence_moves_size     = precedence_moves.size();
        int aggregation_moves_size    = aggregation_moves.size();
        int variable_bound_moves_size = variable_bound_moves.size();
        int exclusive_moves_size      = exclusive_moves.size();
        int user_defined_moves_size   = 0;
        int selection_moves_size      = selection_moves.size();

        /// Binary
        if (binary_moves_size > 0 &&  //
            m_is_enabled_binary_move) {
            m_binary_move_updater(&binary_moves);
        }

        /// Integer
        if (integer_moves_size > 0 &&  //
            m_is_enabled_integer_move) {
            m_integer_move_updater(&integer_moves);
        }

        /// Aggregation
        if (aggregation_moves_size > 0 &&  //
            m_is_enabled_aggregation_move) {
            m_aggregation_move_updater(&aggregation_moves);
        }

        /// Precedence
        if (precedence_moves_size > 0 &&  //
            m_is_enabled_precedence_move) {
            m_precedence_move_updater(&precedence_moves);
        }

        /// Variable Bound
        if (variable_bound_moves_size > 0 &&  //
            m_is_enabled_variable_bound_move) {
            m_variable_bound_move_updater(&variable_bound_moves);
        }

        /// Exclusive
        /**
         * NOTE: Exclusive moves do not require updates.
         */

        /// User Defined
        if (m_is_enabled_user_defined_move) {
            m_user_defined_move_updater(&user_defined_moves);
            user_defined_moves_size = user_defined_moves.size();
            user_defined_move_flags.resize(user_defined_moves_size);
        }

        /// Selection
        if (selection_moves_size > 0 &&  //
            m_is_enabled_selection_move) {
            m_selection_move_updater(&m_selection_moves);
        }

        /// Binary
        if (m_is_enabled_binary_move) {
#ifdef _OPENMP
#pragma omp parallel for if (a_IS_ENABLED_PARALLEL) schedule(static)
#endif
            for (auto i = 0; i < binary_moves_size; i++) {
                binary_move_flags[i] = 1;
                if (m_has_fixed_variables &&
                    model::has_fixed_variables(binary_moves[i])) {
                    binary_move_flags[i] = 0;
                    continue;
                }
                if (m_has_selection_variables &&
                    model::has_selection_variables(binary_moves[i])) {
                    binary_move_flags[i] = 0;
                    continue;
                }
                if (a_IS_ENABLED_IMPROBABILITY_SCREENING &&
                    !model::has_improvable_variable(binary_moves[i])) {
                    binary_move_flags[i] = 0;
                    continue;
                }
            }
        }

        /// Integer
        if (m_is_enabled_integer_move) {
#ifdef _OPENMP
#pragma omp parallel for if (a_IS_ENABLED_PARALLEL) schedule(static)
#endif
            for (auto i = 0; i < integer_moves_size; i++) {
                integer_move_flags[i] = 1;
                if (m_has_fixed_variables &&
                    model::has_fixed_variables(integer_moves[i])) {
                    integer_move_flags[i] = 0;
                    continue;
                }
                if (m_has_selection_variables &&
                    model::has_selection_variables(integer_moves[i])) {
                    integer_move_flags[i] = 0;
                    continue;
                }
                if (model::has_bound_violation(integer_moves[i])) {
                    integer_move_flags[i] = 0;
                    continue;
                }
                if (a_IS_ENABLED_IMPROBABILITY_SCREENING &&
                    !model::has_improvable_variable(integer_moves[i])) {
                    integer_move_flags[i] = 0;
                    continue;
                }
            }
        }

        /// Aggregation
        if (m_is_enabled_aggregation_move) {
#ifdef _OPENMP
#pragma omp parallel for if (a_IS_ENABLED_PARALLEL) schedule(static)
#endif
            for (auto i = 0; i < aggregation_moves_size; i++) {
                aggregation_move_flags[i] = 1;
                if (m_has_fixed_variables &&
                    model::has_fixed_variables(aggregation_moves[i])) {
                    aggregation_move_flags[i] = 0;
                    continue;
                }
                if (m_has_selection_variables &&
                    model::has_selection_variables(aggregation_moves[i])) {
                    aggregation_move_flags[i] = 0;
                    continue;
                }
                if (model::has_bound_violation(aggregation_moves[i])) {
                    aggregation_move_flags[i] = 0;
                    continue;
                }
                if (a_IS_ENABLED_IMPROBABILITY_SCREENING &&
                    !model::has_improvable_variable(aggregation_moves[i])) {
                    aggregation_move_flags[i] = 0;
                    continue;
                }
            }
        }

        /// Precedence
        if (m_is_enabled_precedence_move) {
#ifdef _OPENMP
#pragma omp parallel for if (a_IS_ENABLED_PARALLEL) schedule(static)
#endif
            for (auto i = 0; i < precedence_moves_size; i++) {
                precedence_move_flags[i] = 1;
                if (m_has_fixed_variables &&
                    model::has_fixed_variables(precedence_moves[i])) {
                    precedence_move_flags[i] = 0;
                    continue;
                }
                if (m_has_selection_variables &&
                    model::has_selection_variables(precedence_moves[i])) {
                    precedence_move_flags[i] = 0;
                    continue;
                }
                if (model::has_bound_violation(precedence_moves[i])) {
                    precedence_move_flags[i] = 0;
                    continue;
                }
                if (a_IS_ENABLED_IMPROBABILITY_SCREENING &&
                    !model::has_improvable_variable(precedence_moves[i])) {
                    precedence_move_flags[i] = 0;
                    continue;
                }
            }
        }

        /// Variable Bound
        if (m_is_enabled_variable_bound_move) {
#ifdef _OPENMP
#pragma omp parallel for if (a_IS_ENABLED_PARALLEL) schedule(static)
#endif
            for (auto i = 0; i < variable_bound_moves_size; i++) {
                variable_bound_move_flags[i] = 1;
                if (m_has_fixed_variables &&
                    model::has_fixed_variables(variable_bound_moves[i])) {
                    variable_bound_move_flags[i] = 0;
                    continue;
                }
                if (m_has_selection_variables &&
                    model::has_selection_variables(variable_bound_moves[i])) {
                    variable_bound_move_flags[i] = 0;
                    continue;
                }
                if (model::has_bound_violation(variable_bound_moves[i])) {
                    variable_bound_move_flags[i] = 0;
                    continue;
                }
                if (a_IS_ENABLED_IMPROBABILITY_SCREENING &&
                    !model::has_improvable_variable(variable_bound_moves[i])) {
                    variable_bound_move_flags[i] = 0;
                    continue;
                }
            }
        }

        /// Exclusive
        if (m_is_enabled_exclusive_move) {
#ifdef _OPENMP
#pragma omp parallel for if (a_IS_ENABLED_PARALLEL) schedule(static)
#endif
            for (auto i = 0; i < exclusive_moves_size; i++) {
                exclusive_move_flags[i] = 1;
                if (m_has_fixed_variables &&
                    model::has_fixed_variables(exclusive_moves[i])) {
                    exclusive_move_flags[i] = 0;
                    continue;
                }
                if (m_has_selection_variables &&
                    model::has_selection_variables(exclusive_moves[i])) {
                    exclusive_move_flags[i] = 0;
                    continue;
                }
                if (exclusive_moves[i].alterations[0].first->value() == 1) {
                    exclusive_move_flags[i] = 0;
                    continue;
                }
                if (a_IS_ENABLED_IMPROBABILITY_SCREENING &&
                    !model::has_improvable_variable(exclusive_moves[i])) {
                    exclusive_move_flags[i] = 0;
                    continue;
                }
            }
        }

        /// User Defined
        if (m_is_enabled_user_defined_move) {
#ifdef _OPENMP
#pragma omp parallel for if (a_IS_ENABLED_PARALLEL) schedule(static)
#endif
            for (auto i = 0; i < user_defined_moves_size; i++) {
                user_defined_move_flags[i] = 1;
                if (m_has_fixed_variables &&
                    model::has_fixed_variables(user_defined_moves[i])) {
                    user_defined_move_flags[i] = 0;
                    continue;
                }
                if (m_has_selection_variables &&
                    model::has_selection_variables(user_defined_moves[i])) {
                    user_defined_move_flags[i] = 0;
                    continue;
                }
                if (model::has_bound_violation(user_defined_moves[i])) {
                    user_defined_move_flags[i] = 0;
                    continue;
                }
                if (a_IS_ENABLED_IMPROBABILITY_SCREENING &&
                    !model::has_improvable_variable(user_defined_moves[i])) {
                    user_defined_move_flags[i] = 0;
                    continue;
                }
            }
        }

        /// Selection
        if (m_is_enabled_selection_move) {
#ifdef _OPENMP
#pragma omp parallel for if (a_IS_ENABLED_PARALLEL) schedule(static)
#endif
            for (auto i = 0; i < selection_moves_size; i++) {
                selection_move_flags[i] = 1;
                if (m_has_fixed_variables &&
                    model::has_fixed_variables(selection_moves[i])) {
                    selection_move_flags[i] = 0;
                }
                if ((selection_moves[i].alterations[0].first ==
                     selection_moves[i].alterations[1].first)) {
                    selection_move_flags[i] = 0;
                }
                if (a_IS_ENABLED_IMPROBABILITY_SCREENING &&
                    !model::has_improvable_variable(selection_moves[i])) {
                    selection_move_flags[i] = 0;
                }
            }
        }

        auto number_of_candidate_moves =           //
            std::count(binary_move_flags.begin(),  //
                       binary_move_flags.end(), 1) +
            std::count(integer_move_flags.begin(),  //
                       integer_move_flags.end(), 1) +
            std::count(precedence_move_flags.begin(),
                       precedence_move_flags.end(), 1) +
            std::count(aggregation_move_flags.begin(),  //
                       aggregation_move_flags.end(), 1) +
            std::count(variable_bound_move_flags.begin(),  //
                       variable_bound_move_flags.end(), 1) +
            std::count(exclusive_move_flags.begin(),  //
                       exclusive_move_flags.end(), 1) +
            std::count(user_defined_move_flags.begin(),  //
                       user_defined_move_flags.end(), 1) +
            std::count(selection_move_flags.begin(),  //
                       selection_move_flags.end(), 1);

        move_ptrs.resize(number_of_candidate_moves);
        auto index = 0;

        for (auto i = 0; i < binary_moves_size; i++) {
            if (binary_move_flags[i] > 0) {
                move_ptrs[index++] = &binary_moves[i];
            }
        }

        for (auto i = 0; i < integer_moves_size; i++) {
            if (integer_move_flags[i]) {
                move_ptrs[index++] = &integer_moves[i];
            }
        }

        for (auto i = 0; i < precedence_moves_size; i++) {
            if (precedence_move_flags[i]) {
                move_ptrs[index++] = &precedence_moves[i];
            }
        }

        for (auto i = 0; i < aggregation_moves_size; i++) {
            if (aggregation_move_flags[i]) {
                move_ptrs[index++] = &aggregation_moves[i];
            }
        }

        for (auto i = 0; i < variable_bound_moves_size; i++) {
            if (variable_bound_move_flags[i]) {
                move_ptrs[index++] = &variable_bound_moves[i];
            }
        }

        for (auto i = 0; i < exclusive_moves_size; i++) {
            if (exclusive_move_flags[i]) {
                move_ptrs[index++] = &exclusive_moves[i];
            }
        }

        for (auto i = 0; i < user_defined_moves_size; i++) {
            if (user_defined_move_flags[i]) {
                move_ptrs[index++] = &user_defined_moves[i];
            }
        }

        for (auto i = 0; i < selection_moves_size; i++) {
            if (selection_move_flags[i]) {
                move_ptrs[index++] = &selection_moves[i];
            }
        }
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
        &exclusive_moves(void) {
        return m_exclusive_moves;
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