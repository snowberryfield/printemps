/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_NEIGHBORHOOD_EXCLUSIVE_NOR_MOVE_GENERATOR_H__
#define PRINTEMPS_NEIGHBORHOOD_EXCLUSIVE_NOR_MOVE_GENERATOR_H__

#include "abstract_move_generator.h"

namespace printemps::neighborhood {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class ExclusiveNORMoveGenerator
    : public AbstractMoveGenerator<T_Variable, T_Expression> {
   private:
   public:
    /*************************************************************************/
    ExclusiveNORMoveGenerator(void) {
        /// nothing to do
    }

    /*************************************************************************/
    void setup(const std::vector<model_component::Constraint<
                   T_Variable, T_Expression> *> &a_RAW_CONSTRAINT_PTRS) {
        /**
         * Exclude constraints which contain fixed variables or selection
         * variables.
         */
        auto constraint_ptrs =
            extract_effective_constraint_ptrs(a_RAW_CONSTRAINT_PTRS);

        /**
         * Convert constraint objects to BinomialConstraint objects.
         */
        auto binomials = convert_to_binomial_constraints(constraint_ptrs);

        /**
         * Setup move objects.
         */
        const int BINOMIALS_SIZE = binomials.size();

        this->m_moves.clear();
        this->m_flags.clear();

        this->m_moves.resize(2 * BINOMIALS_SIZE);
        this->m_flags.resize(2 * BINOMIALS_SIZE);

        for (auto i = 0; i < BINOMIALS_SIZE; i++) {
            auto &move = this->m_moves[2 * i];

            move.associated_constraint_ptr = constraint_ptrs[i];
            move.type                      = MoveType::ExclusiveNOR;
            move.alterations.emplace_back(binomials[i].variable_ptr_first, 0);
            move.alterations.emplace_back(binomials[i].variable_ptr_second, 0);
            move.is_univariable_move          = false;
            move.is_selection_move            = false;
            move.is_special_neighborhood_move = true;
            move.is_available                 = true;
            move.overlap_rate                 = 0.0;

            move.related_constraint_ptrs.insert(
                move.related_constraint_ptrs.end(),
                binomials[i]
                    .variable_ptr_first->related_constraint_ptrs()
                    .begin(),
                binomials[i]
                    .variable_ptr_first->related_constraint_ptrs()
                    .end());

            move.related_constraint_ptrs.insert(
                move.related_constraint_ptrs.end(),
                binomials[i]
                    .variable_ptr_second->related_constraint_ptrs()
                    .begin(),
                binomials[i]
                    .variable_ptr_second->related_constraint_ptrs()
                    .end());

            move.sort_and_unique_related_constraint_ptrs();

            this->m_moves[2 * i + 1]                       = move;
            this->m_moves[2 * i + 1].alterations[0].second = 1;
            this->m_moves[2 * i + 1].alterations[1].second = 1;
        }

        /**
         * Setup move updater.
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
                    if (!(*a_moves_ptr)[i]
                             .associated_constraint_ptr->is_feasible()) {
                        (*a_flags)[i] = 0;
                        continue;
                    }

                    (*a_flags)[i] = 1;

                    if (!(*a_moves_ptr)[i].is_available) {
                        (*a_flags)[i] = 0;
                        continue;
                    }

                    if ((*a_moves_ptr)[i].has_fixed_variable()) {
                        (*a_flags)[i] = 0;
                        continue;
                    }

                    if ((*a_moves_ptr)[i].has_bound_violation()) {
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