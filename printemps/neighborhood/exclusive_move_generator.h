/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_NEIGHBORHOOD_EXCLUSIVE_MOVE_MOVE_GENERATOR_H__
#define PRINTEMPS_NEIGHBORHOOD_EXCLUSIVE_MOVE_MOVE_GENERATOR_H__

#include "abstract_move_generator.h"

namespace printemps {
namespace model {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Variable;

/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Constraint;

}  // namespace model
}  // namespace printemps

namespace printemps {
namespace neighborhood {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Move;

/*****************************************************************************/
template <class T_Variable, class T_Expression>
class ExclusiveMoveGenerator
    : public AbstractMoveGenerator<T_Variable, T_Expression> {
   private:
   public:
    /*************************************************************************/
    ExclusiveMoveGenerator(void) {
        /// nothing to do
    }

    /*************************************************************************/
    virtual ~ExclusiveMoveGenerator(void) {
        /// nothing to do
    }

    /*************************************************************************/
    constexpr void setup(
        const std::vector<model::Constraint<T_Variable, T_Expression> *>
            &a_SET_PARTITIONING_PTRS,
        const std::vector<model::Constraint<T_Variable, T_Expression> *>
            &a_SET_PACKING_PTRS) {
        int SET_PARTITIONINGS_SIZE = a_SET_PARTITIONING_PTRS.size();
        int SET_PACKINGS_SIZE      = a_SET_PACKING_PTRS.size();

        std::unordered_map<
            model::Variable<T_Variable, T_Expression> *,
            std::unordered_set<model::Variable<T_Variable, T_Expression> *>>
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
                             model::VariableSense::Selection) ||
                            (variable_ptr_second->sense() ==
                             model::VariableSense::Selection)) {
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
                             model::VariableSense::Selection) ||
                            (variable_ptr_second->sense() ==
                             model::VariableSense::Selection)) {
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
        this->m_moves.resize(VARIABLES_SIZE);
        this->m_flags.resize(VARIABLES_SIZE);

        std::vector<model::Variable<T_Variable, T_Expression> *> variable_ptrs(
            VARIABLES_SIZE);
        std::vector<
            std::unordered_set<model::Variable<T_Variable, T_Expression> *>>
            associated_variables_ptrs(VARIABLES_SIZE);

        int move_index = 0;
        for (auto &&association : associations) {
            auto  variable_ptr             = association.first;
            auto &associated_variable_ptrs = association.second;

            variable_ptrs[move_index]             = variable_ptr;
            associated_variables_ptrs[move_index] = associated_variable_ptrs;

            this->m_moves[move_index].sense = MoveSense::Exclusive;
            this->m_moves[move_index].alterations.emplace_back(variable_ptr, 1);

            this->m_moves[move_index].is_special_neighborhood_move = true;
            this->m_moves[move_index].is_available                 = true;
            this->m_moves[move_index].overlap_rate                 = 0.0;

            this->m_moves[move_index].related_constraint_ptrs.insert(
                variable_ptr->related_constraint_ptrs().begin(),
                variable_ptr->related_constraint_ptrs().end());

            for (auto &&associated_variable_ptr : associated_variable_ptrs) {
                this->m_moves[move_index].alterations.emplace_back(
                    associated_variable_ptr, 0);
                this->m_moves[move_index].related_constraint_ptrs.insert(
                    associated_variable_ptr->related_constraint_ptrs().begin(),
                    associated_variable_ptr->related_constraint_ptrs().end());
            }

            move_index++;
        }

        auto move_updater =                                     //
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
                    if (neighborhood::has_fixed_variables((*a_moves)[i])) {
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
                            neighborhood::has_objective_improvable_variable(
                                (*a_moves)[i])) {
                            continue;
                        }

                        if (a_ACCEPT_FEASIBILITY_IMPROVABLE &&
                            neighborhood::has_feasibility_improvable_variable(
                                (*a_moves)[i])) {
                            continue;
                        }
                        (*a_flags)[i] = 0;
                    }
                }
            };
        this->m_move_updater = move_updater;
    }
};
}  // namespace neighborhood
}  // namespace printemps
#endif
/*****************************************************************************/
// END
/*****************************************************************************/