/*****************************************************************************/
// Copyright (c) 2020-2026 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_NEIGHBORHOOD_PARTIAL_FEASIBLE_ENUMERATION_MOVE_GENERATOR_H__
#define PRINTEMPS_NEIGHBORHOOD_PARTIAL_FEASIBLE_ENUMERATION_MOVE_GENERATOR_H__

#include "abstract_move_generator.h"

namespace printemps::neighborhood {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class PartialFeasibleEnumerationMoveGenerator
    : public AbstractMoveGenerator<T_Variable, T_Expression> {
   private:
   public:
    /*************************************************************************/
    PartialFeasibleEnumerationMoveGenerator(void) {
        /// nothing to do
    }

    /*************************************************************************/
    void setup(const std::vector<model_component::ConstraintGroup<
                   T_Variable, T_Expression>> &a_SMALL_CONSTRAINT_GROUPS) {
        /**
         * Setup move objects.
         */
        int moves_size = 0;
        for (const auto &constraint_group : a_SMALL_CONSTRAINT_GROUPS) {
            moves_size += constraint_group.solutions.size();
        }

        this->m_moves.clear();
        this->m_flags.clear();

        this->m_moves.resize(moves_size);
        this->m_flags.resize(moves_size);

        int index = 0;
        for (const auto &constraint_group : a_SMALL_CONSTRAINT_GROUPS) {
            for (const auto &solution : constraint_group.solutions) {
                auto &move = this->m_moves[index++];

                move.associated_constraint_ptr = nullptr;
                move.type                = MoveType::PartialFeasibleEnumeration;
                move.alterations         = solution;
                move.is_univariable_move = false;
                move.is_selection_move   = false;
                move.is_special_neighborhood_move = true;
                move.is_available                 = true;
                move.overlap_rate                 = 0.0;
                move.setup_related_constraint_ptrs();
            }
        }

        /**
         * Setup move objects.
         */
        auto move_updater =                                 //
            [](auto      *a_moves_ptr,                      //
               auto      *a_flags,                          //
               const bool a_ACCEPT_ALL,                     //
               const bool a_ACCEPT_OBJECTIVE_IMPROVABLE,    //
               const bool a_ACCEPT_FEASIBILITY_IMPROVABLE,  //
#ifdef _OPENMP
               const bool a_IS_ENABLED_PARALLEL,  //
               const int  a_NUMBER_OF_THREADS     //
#else
               [[maybe_unused]] const bool a_IS_ENABLED_PARALLEL,  //
               [[maybe_unused]] const int  a_NUMBER_OF_THREADS     //
#endif
            ) {
                const int MOVES_SIZE = a_moves_ptr->size();
#ifdef _OPENMP
#pragma omp parallel for if (a_IS_ENABLED_PARALLEL) schedule(static) \
    num_threads(a_NUMBER_OF_THREADS)
#endif
                for (auto i = 0; i < MOVES_SIZE; i++) {
                    (*a_flags)[i] = 1;

                    if (!(*a_moves_ptr)[i].is_available) {
                        (*a_flags)[i] = 0;
                        continue;
                    }

                    if ((*a_moves_ptr)[i].has_selection_variable()) {
                        (*a_flags)[i] = 0;
                        continue;
                    }

                    if ((*a_moves_ptr)[i].has_fixed_variable()) {
                        (*a_flags)[i] = 0;
                        continue;
                    }

                    if ((*a_flags)[i] == 0) {
                        continue;
                    }

                    if (a_ACCEPT_ALL) {
                        /** nothing to do */
                    } else {
                        if (a_ACCEPT_OBJECTIVE_IMPROVABLE &&
                            (*a_moves_ptr)[i]
                                .has_objective_improvable_variable_with_value_check()) {
                            continue;
                        }

                        if (a_ACCEPT_FEASIBILITY_IMPROVABLE &&
                            (*a_moves_ptr)[i]
                                .has_feasibility_improvable_variable_with_value_check()) {
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