/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_NEIGHBORHOOD_SELECTION_MOVE_GENERATOR_H__
#define PRINTEMPS_NEIGHBORHOOD_SELECTION_MOVE_GENERATOR_H__

#include "abstract_move_generator.h"

namespace printemps::neighborhood {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class SelectionMoveGenerator
    : public AbstractMoveGenerator<T_Variable, T_Expression> {
   private:
   public:
    /*************************************************************************/
    SelectionMoveGenerator(void) {
        /// nothing to do
    }

    /*************************************************************************/
    inline void setup(
        const std::vector<model_component::Variable<T_Variable, T_Expression> *>
            &a_RAW_VARIABLE_PTRS) {
        /**
         *  "Swap" move for binary variables in selection
         * constraints: e.g.) selection constraint x + y + z = 1 (x,
         * y, z \in {0, 1}) move: {(x = 0, y = 1), (x = 0, z = 1)}
         * (if x = 1, y = 0, z = 0)
         */

        /**
         * Setup move objects.
         */
        auto mutable_variable_ptrs =
            extract_mutable_variable_ptrs(a_RAW_VARIABLE_PTRS);

        /**
         * Setup move objects.
         */
        const int VARIABLES_SIZE = mutable_variable_ptrs.size();

        this->m_moves.clear();
        this->m_flags.clear();

        this->m_moves.resize(VARIABLES_SIZE);
        this->m_flags.resize(VARIABLES_SIZE);

        for (auto i = 0; i < VARIABLES_SIZE; i++) {
            auto &move                        = this->m_moves[i];
            move.type                         = MoveType::Selection;
            move.is_univariable_move          = false;
            move.is_selection_move            = true;
            move.is_special_neighborhood_move = false;
            move.is_available                 = true;
            move.overlap_rate                 = 0.0;
            move.alterations.resize(2);
            move.alterations[0].second   = 0;
            move.alterations[1].second   = 1;
            move.related_constraint_ptrs = mutable_variable_ptrs[i]
                                               ->selection_ptr()
                                               ->related_constraint_ptrs;
        }

        /**
         * Setup move updater
         */
        auto move_updater =  //
            [mutable_variable_ptrs, VARIABLES_SIZE](
                auto      *a_moves_ptr,                      //
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
#ifdef _OPENMP
#pragma omp parallel for if (a_IS_ENABLED_PARALLEL) schedule(static) \
    num_threads(a_NUMBER_OF_THREADS)
#endif
                for (auto i = 0; i < VARIABLES_SIZE; i++) {
                    auto &alterations = (*a_moves_ptr)[i].alterations;

                    alterations[0].first = mutable_variable_ptrs[i]
                                               ->selection_ptr()
                                               ->selected_variable_ptr;
                    alterations[1].first = mutable_variable_ptrs[i];
                }

                const int MOVES_SIZE = a_moves_ptr->size();

#ifdef _OPENMP
#pragma omp parallel for if (a_IS_ENABLED_PARALLEL) schedule(static) \
    num_threads(a_NUMBER_OF_THREADS)
#endif
                for (auto i = 0; i < MOVES_SIZE; i++) {
                    (*a_flags)[i] = 1;
                    if ((*a_moves_ptr)[i].alterations[0].first ==
                        (*a_moves_ptr)[i].alterations[1].first) {
                        (*a_flags)[i] = 0;
                        continue;
                    }

                    if (a_ACCEPT_ALL) {
                        /** nothing to do */
                    } else {
                        if (a_ACCEPT_OBJECTIVE_IMPROVABLE &&
                            (*a_moves_ptr)[i]
                                .has_objective_improvable_variable()) {
                            continue;
                        }

                        if (a_ACCEPT_FEASIBILITY_IMPROVABLE &&
                            (*a_moves_ptr)[i]
                                .has_feasibility_improvable_variable()) {
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