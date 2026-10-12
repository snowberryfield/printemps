/*****************************************************************************/
// Copyright (c) 2020-2026 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_NEIGHBORHOOD_TWO_FLIP_MOVE_GENERATOR_H__
#define PRINTEMPS_NEIGHBORHOOD_TWO_FLIP_MOVE_GENERATOR_H__

#include "abstract_move_generator.h"

namespace printemps::neighborhood {
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
    void setup(const std::vector<
               std::pair<model_component::Variable<T_Variable, T_Expression> *,
                         model_component::Variable<T_Variable, T_Expression> *>>
                   &a_FLIPPABLE_VARIABLE_PTR_PAIRS) {
        this->initialize();
        /**
         * Setup move objects.
         */
        const int PAIRS_SIZE = a_FLIPPABLE_VARIABLE_PTR_PAIRS.size();

        this->m_moves.clear();
        this->m_flags.clear();

        this->m_moves.resize(2 * PAIRS_SIZE);
        this->m_flags.resize(2 * PAIRS_SIZE);

        for (int i = 0; i < PAIRS_SIZE; i++) {
            auto &move = this->m_moves[2 * i];

            move.associated_constraint_ptr = nullptr;
            move.type                      = MoveType::TwoFlip;
            move.alterations.emplace_back(
                a_FLIPPABLE_VARIABLE_PTR_PAIRS[i].first, 1);
            move.alterations.emplace_back(
                a_FLIPPABLE_VARIABLE_PTR_PAIRS[i].second, 0);
            move.is_univariable_move          = false;
            move.is_selection_move            = false;
            move.is_special_neighborhood_move = true;
            move.is_available                 = true;
            move.overlap_rate                 = 0.0;
            move.setup_related_constraint_ptrs();

            this->m_moves[2 * i + 1] = move;

            this->m_moves[2 * i + 1].alterations[0].second = 0;
            this->m_moves[2 * i + 1].alterations[1].second = 1;
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
                const int MOVES_SIZE      = a_moves_ptr->size();
                const int NUMBER_OF_WORDS = (MOVES_SIZE + 63) / 64;

#ifdef _OPENMP
#pragma omp parallel for if (a_IS_ENABLED_PARALLEL) schedule(static) \
    num_threads(a_NUMBER_OF_THREADS)
#endif
                for (int w = 0; w < NUMBER_OF_WORDS; w++) {
                    const int     START = w * 64;
                    const int     END   = std::min(START + 64, MOVES_SIZE);
                    std::uint64_t word  = 0;

                    for (int i = START; i < END; i++) {
                        if (!(*a_moves_ptr)[i].is_available) {
                            continue;
                        }

                        if ((*a_moves_ptr)[i].has_selection_variable()) {
                            continue;
                        }

                        if ((*a_moves_ptr)[i].has_fixed_variable()) {
                            continue;
                        }

                        bool has_same_value = false;
                        for (const auto &alteration :
                             (*a_moves_ptr)[i].alterations) {
                            if (alteration.first->value() ==
                                alteration.second) {
                                has_same_value = true;
                                break;
                            }
                        }
                        if (has_same_value) {
                            continue;
                        }

                        if (a_ACCEPT_ALL) {
                            BitArray::set_bit_to_word(&word, i - START);
                        } else {
                            if (a_ACCEPT_OBJECTIVE_IMPROVABLE &&
                                (*a_moves_ptr)[i]
                                    .has_objective_improvable_variable()) {
                                BitArray::set_bit_to_word(&word, i - START);
                                continue;
                            }

                            if (a_ACCEPT_FEASIBILITY_IMPROVABLE &&
                                (*a_moves_ptr)[i]
                                    .has_feasibility_improvable_variable()) {
                                BitArray::set_bit_to_word(&word, i - START);
                                continue;
                            }
                        }
                    }
                    a_flags->set_word(w, word);
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