/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_NEIGHBORHOOD_TWO_FLIP_MOVE_GENERATOR_H__
#define PRINTEMPS_NEIGHBORHOOD_TWO_FLIP_MOVE_GENERATOR_H__

#include "abstract_move_generator.h"

namespace printemps {
namespace neighborhood {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class TwoFlipMoveGenerator
    : public AbstractMoveGenerator<T_Variable, T_Expression> {
   private:
   public:
    /*************************************************************************/
    TwoFlipMoveGenerator(void) {
        /// nothing to do
    }

    /*************************************************************************/
    virtual ~TwoFlipMoveGenerator(void) {
        /// nothing to do
    }

    /*************************************************************************/
    void setup(const std::vector<
               std::pair<model_component::Variable<T_Variable, T_Expression> *,
                         model_component::Variable<T_Variable, T_Expression> *>>
                   &a_FLIPPABLE_VARIABLE_PTR_PAIRS) {
        /**
         * Setup move objects.
         */
        const int PAIRS_SIZE = a_FLIPPABLE_VARIABLE_PTR_PAIRS.size();
        this->m_moves.resize(2 * PAIRS_SIZE);
        this->m_flags.resize(2 * PAIRS_SIZE);

        for (auto i = 0; i < PAIRS_SIZE; i++) {
            this->m_moves[2 * i].sense = MoveSense::TwoFlip;
            this->m_moves[2 * i].alterations.emplace_back(
                a_FLIPPABLE_VARIABLE_PTR_PAIRS[i].first, 1);
            this->m_moves[2 * i].alterations.emplace_back(
                a_FLIPPABLE_VARIABLE_PTR_PAIRS[i].second, 0);
            this->m_moves[2 * i].is_univariable_move = false;

            utility::update_union_set(
                &(this->m_moves[2 * i].related_constraint_ptrs),
                a_FLIPPABLE_VARIABLE_PTR_PAIRS[i]
                    .first->related_constraint_ptrs());

            utility::update_union_set(
                &(this->m_moves[2 * i].related_constraint_ptrs),
                a_FLIPPABLE_VARIABLE_PTR_PAIRS[i]
                    .second->related_constraint_ptrs());

            this->m_moves[2 * i].is_special_neighborhood_move = true;
            this->m_moves[2 * i].is_available                 = true;
            this->m_moves[2 * i].overlap_rate                 = 0.0;

            this->m_moves[2 * i + 1] = this->m_moves[2 * i];

            this->m_moves[2 * i + 1].alterations[0].second = 0;
            this->m_moves[2 * i + 1].alterations[1].second = 1;
        }

        /**
         * Setup move objects.
         */
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
                    if (neighborhood::has_selection_variable((*a_moves)[i])) {
                        (*a_flags)[i] = 0;
                        continue;
                    }
                    if (neighborhood::has_fixed_variable((*a_moves)[i])) {
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