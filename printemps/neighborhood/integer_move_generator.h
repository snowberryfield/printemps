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

        for (auto i = 0; i < VARIABLES_SIZE; i++) {
            auto &move = this->m_moves[4 * i];

            move.sense = MoveSense::Integer;
            move.alterations.emplace_back(mutable_variable_ptrs[i], 0);
            move.is_univariable_move          = true;
            move.is_selection_move            = false;
            move.is_special_neighborhood_move = false;
            move.is_available                 = true;
            move.overlap_rate                 = 0.0;
            move.related_constraint_ptrs =
                mutable_variable_ptrs[i]->related_constraint_ptrs();

            this->m_moves[4 * i + 1] = move;
            this->m_moves[4 * i + 2] = move;
            this->m_moves[4 * i + 3] = move;
        }

        /**
         * Setup move updater.
         */
        auto move_updater =  //
            [mutable_variable_ptrs, VARIABLES_SIZE](
                auto *     a_moves_ptr,                      //
                auto *     a_flags,                          //
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
#ifdef _OPENMP
#pragma omp parallel for if (a_IS_ENABLED_PARALLEL) schedule(static) \
    num_threads(a_NUMBER_OF_THREADS)
#endif
                for (auto i = 0; i < VARIABLES_SIZE; i++) {
                    const auto value = mutable_variable_ptrs[i]->value();
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
                        if (value == upper_bound) {
                            (*a_flags)[4 * i] = 0;
                        } else {
                            (*a_moves_ptr)[4 * i].alterations.front().second =
                                value + 1;
                            (*a_flags)[4 * i] = 1;
                        }

                        if (value == lower_bound) {
                            (*a_flags)[4 * i + 1] = 0;
                        } else {
                            (*a_moves_ptr)[4 * i + 1]
                                .alterations.front()
                                .second           = value - 1;
                            (*a_flags)[4 * i + 1] = 1;
                        }

                        if (value >= upper_bound - 4 ||
                            upper_bound == constant::INT_HALF_MAX) {
                            (*a_flags)[4 * i + 2] = 0;
                        } else {
                            const auto DELTA =
                                std::min(DELTA_MAX, (upper_bound - value) / 2);
                            (*a_moves_ptr)[4 * i + 2]
                                .alterations.front()
                                .second           = value + DELTA;
                            (*a_flags)[4 * i + 2] = 1;
                        }

                        if (value <= lower_bound + 4 ||
                            lower_bound == constant::INT_HALF_MIN) {
                            (*a_flags)[4 * i + 3] = 0;
                        } else {
                            const auto DELTA =
                                std::max(-DELTA_MAX, (lower_bound - value) / 2);
                            (*a_moves_ptr)[4 * i + 3]
                                .alterations.front()
                                .second           = value + DELTA;
                            (*a_flags)[4 * i + 3] = 1;
                        }
                    } else {
                        for (auto j = 0; j < 4; j++) {
                            (*a_flags)[4 * i]     = 0;
                            (*a_flags)[4 * i + j] = 0;
                        }
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