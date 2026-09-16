/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_NEIGHBORHOOD_INTEGER_MOVE_GENERATOR_H__
#define PRINTEMPS_NEIGHBORHOOD_INTEGER_MOVE_GENERATOR_H__

#include "abstract_move_generator.h"

namespace printemps::neighborhood {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class IntegerMoveGenerator
    : public AbstractMoveGenerator<T_Variable, T_Expression> {
   private:
   public:
    /*************************************************************************/
    IntegerMoveGenerator(void) {
        /// nothing to do
    }

    /*************************************************************************/
    void setup(
        const std::vector<model_component::Variable<T_Variable, T_Expression> *>
            &a_RAW_VARIABLE_PTRS) {
        this->initialize();
        /**
         *  "Shift" move for integer variables:
         *  e.g) integer variable 0 <= x <= 10 (x \in Z)
         *  move: {(x = 5), (x = 7), (x = 3), (x = 8)} (if x = 6)
         *        {(x = 1), (x = 5)} (if x = 0)
         *        {(x = 9), (x = 5)} (if x = 10)
         */

        /**
         * Extract mutable variables.
         */
        auto mutable_variable_ptrs =
            extract_mutable_variable_ptrs(a_RAW_VARIABLE_PTRS);

        /**
         * Setup move objects.
         */
        const int VARIABLES_SIZE = mutable_variable_ptrs.size();

        this->m_moves.clear();
        this->m_flags.clear();

        this->m_moves.resize(4 * VARIABLES_SIZE);
        this->m_flags.resize(4 * VARIABLES_SIZE);

        for (int i = 0; i < VARIABLES_SIZE; i++) {
            auto &move = this->m_moves[4 * i];

            move.associated_constraint_ptr = nullptr;
            move.type                      = MoveType::Integer;
            move.alterations.emplace_back(mutable_variable_ptrs[i], 0);
            move.is_univariable_move          = true;
            move.is_selection_move            = false;
            move.is_special_neighborhood_move = false;
            move.is_available                 = true;
            move.overlap_rate                 = 0.0;
            move.setup_related_constraint_ptrs();

            this->m_moves[4 * i + 1] = move;
            this->m_moves[4 * i + 2] = move;
            this->m_moves[4 * i + 3] = move;
        }

        /**
         * Setup move updater.
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
                const int DELTA_MAX = 10000;
                const int MOVES_SIZE = 4 * VARIABLES_SIZE;
                const int NUM_WORDS  = (MOVES_SIZE + 63) / 64;

#ifdef _OPENMP
#pragma omp parallel for if (a_IS_ENABLED_PARALLEL) schedule(static) \
    num_threads(a_NUMBER_OF_THREADS)
#endif
                for (int w = 0; w < NUM_WORDS; w++) {
                    const int     start_var = w * 16;
                    const int     end_var   = std::min(start_var + 16, VARIABLES_SIZE);
                    std::uint64_t word      = 0;

                    for (int i = start_var; i < end_var; i++) {
                        const int  bit_offset  = (i - start_var) * 4;
                        const auto value       = mutable_variable_ptrs[i]->value();
                        const auto lower_bound =
                            mutable_variable_ptrs[i]->lower_bound();
                        const auto upper_bound =
                            mutable_variable_ptrs[i]->upper_bound();

                        if (a_ACCEPT_ALL ||
                            (a_ACCEPT_OBJECTIVE_IMPROVABLE &&
                             mutable_variable_ptrs[i]->is_objective_improvable()) ||
                            (a_ACCEPT_FEASIBILITY_IMPROVABLE &&
                             mutable_variable_ptrs[i]
                                 ->is_feasibility_improvable())) {
                            if (value != upper_bound) {
                                (*a_moves_ptr)[4 * i].alterations.front().second =
                                    value + 1;
                                word |= (1ULL << bit_offset);
                            }

                            if (value != lower_bound) {
                                (*a_moves_ptr)[4 * i + 1]
                                    .alterations.front()
                                    .second = value - 1;
                                word |= (1ULL << (bit_offset + 1));
                            }

                            if (value < upper_bound - 4 &&
                                upper_bound != constant::INT_HALF_MAX) {
                                const auto DELTA =
                                    std::min(DELTA_MAX, (upper_bound - value) / 2);
                                (*a_moves_ptr)[4 * i + 2]
                                    .alterations.front()
                                    .second = value + DELTA;
                                word |= (1ULL << (bit_offset + 2));
                            }

                            if (value > lower_bound + 4 &&
                                lower_bound != constant::INT_HALF_MIN) {
                                const auto DELTA =
                                    std::max(-DELTA_MAX, (lower_bound - value) / 2);
                                (*a_moves_ptr)[4 * i + 3]
                                    .alterations.front()
                                    .second = value + DELTA;
                                word |= (1ULL << (bit_offset + 3));
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