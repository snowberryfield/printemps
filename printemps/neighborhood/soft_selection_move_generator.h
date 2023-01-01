/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_NEIGHBORHOOD_SOFT_SELECTION_MOVE_GENERATOR_H__
#define PRINTEMPS_NEIGHBORHOOD_SOFT_SELECTION_MOVE_GENERATOR_H__

#include "abstract_move_generator.h"

namespace printemps::neighborhood {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class SoftSelectionMoveGenerator
    : public AbstractMoveGenerator<T_Variable, T_Expression> {
   private:
   public:
    /*************************************************************************/
    SoftSelectionMoveGenerator(void) {
        /// nothing to do
    }

    /*************************************************************************/
    virtual ~SoftSelectionMoveGenerator(void) {
        /// nothing to do
    }

    /*************************************************************************/
    constexpr void setup(
        const std::vector<model_component::Constraint<T_Variable, T_Expression>
                              *> &a_RAW_CONSTRAINT_PTRS) {
        /**
         * Exclude constraints which contain fixed variables or selection
         * variables.
         */
        auto constraint_ptrs =
            extract_effective_constraint_ptrs(a_RAW_CONSTRAINT_PTRS);

        for (auto &&constraint_ptr : constraint_ptrs) {
            const auto &sensitivities =
                constraint_ptr->expression().sensitivities();
            const auto key_variable_ptr = constraint_ptr->key_variable_ptr();

            std::vector<Move<T_Variable, T_Expression>> moves;
            moves.reserve(2 * sensitivities.size());
            for (auto &&sensitivity : sensitivities) {
                const auto variable_ptr = sensitivity.first;

                if (variable_ptr == key_variable_ptr) {
                    continue;
                }

                Move<T_Variable, T_Expression> move_first;
                Move<T_Variable, T_Expression> move_second;

                move_first.sense               = MoveSense::SoftSelection;
                move_first.is_univariable_move = false;
                move_first.is_selection_move   = false;
                move_first.is_special_neighborhood_move = true;
                move_first.is_available                 = true;
                move_first.overlap_rate                 = 0.0;

                move_first.related_constraint_ptrs.insert(
                    move_first.related_constraint_ptrs.end(),
                    variable_ptr->related_constraint_ptrs().begin(),
                    variable_ptr->related_constraint_ptrs().end());

                move_first.related_constraint_ptrs.insert(
                    move_first.related_constraint_ptrs.end(),
                    key_variable_ptr->related_constraint_ptrs().begin(),
                    key_variable_ptr->related_constraint_ptrs().end());

                sort_and_unique_related_constraint_ptrs(&move_first);

                move_second = move_first;

                move_first.alterations.emplace_back(variable_ptr, 0);
                move_first.alterations.emplace_back(key_variable_ptr, 0);

                move_second.alterations.emplace_back(variable_ptr, 1);
                move_second.alterations.emplace_back(key_variable_ptr, 1);
                moves.push_back(move_first);
                moves.push_back(move_second);
            }
            this->m_moves.insert(this->m_moves.end(), moves.begin(),
                                 moves.end());
        }
        this->m_flags.resize(this->m_moves.size());

        /**
         * Setup move updater
         */
        auto move_updater =                                                  //
            [](auto *                      a_moves_ptr,                      //
               auto *                      a_flags,                          //
               const bool                  a_ACCEPT_ALL,                     //
               const bool                  a_ACCEPT_OBJECTIVE_IMPROVABLE,    //
               const bool                  a_ACCEPT_FEASIBILITY_IMPROVABLE,  //
               [[maybe_unused]] const bool a_IS_ENABLED_PARALLEL) {
                const int MOVES_SIZE = a_moves_ptr->size();
#ifdef _OPENMP
#pragma omp parallel for if (a_IS_ENABLED_PARALLEL) schedule(static)
#endif
                for (auto i = 0; i < MOVES_SIZE; i++) {
                    (*a_flags)[i] = 1;
                    if (!(*a_moves_ptr)[i].is_available) {
                        (*a_flags)[i] = 0;
                        continue;
                    }
                    if (neighborhood::has_fixed_variable((*a_moves_ptr)[i])) {
                        (*a_flags)[i] = 0;
                        continue;
                    }
                    for (const auto &alteration :
                         (*a_moves_ptr)[i].alterations) {
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
                            neighborhood::has_objective_improvable_variable(
                                (*a_moves_ptr)[i])) {
                            continue;
                        }

                        if (a_ACCEPT_FEASIBILITY_IMPROVABLE &&
                            neighborhood::has_feasibility_improvable_variable(
                                (*a_moves_ptr)[i])) {
                            continue;
                        }
                        (*a_flags)[i] = 0;
                    }
                }
            };
        this->m_move_updater = move_updater;
    }
};
}  // namespace printemps::neighborhood
#endif
/*****************************************************************************/
// END
/*****************************************************************************/