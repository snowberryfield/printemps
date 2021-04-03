/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_NEIGHBORHOOD_INTEGER_MOVE_GENERATOR_H__
#define PRINTEMPS_NEIGHBORHOOD_INTEGER_MOVE_GENERATOR_H__

#include "abstract_move_generator.h"

namespace printemps {
namespace neighborhood {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct Move;

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
    virtual ~IntegerMoveGenerator(void) {
        /// nothing to do
    }

    /*************************************************************************/
    void setup(const std::vector<model::Variable<T_Variable, T_Expression> *>
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
        this->m_moves.resize(4 * VARIABLES_SIZE);
        this->m_flags.resize(4 * VARIABLES_SIZE);

        for (auto i = 0; i < VARIABLES_SIZE; i++) {
            for (auto j = 0; j < 4; j++) {
                this->m_moves[4 * i + j].sense = MoveSense::Integer;
                this->m_moves[4 * i + j].related_constraint_ptrs =
                    mutable_variable_ptrs[i]->related_constraint_ptrs();
                this->m_moves[4 * i + j].alterations.emplace_back(
                    mutable_variable_ptrs[i], 0);
                this->m_moves[4 * i + j].is_special_neighborhood_move = false;
                this->m_moves[4 * i + j].is_available                 = true;
                this->m_moves[4 * i + j].overlap_rate                 = 0.0;
            }
        }

        /**
         * Setup move updater.
         */
        auto move_updater =  //
            [this, mutable_variable_ptrs, VARIABLES_SIZE](
                auto *                      a_moves,                          //
                auto *                      a_flags,                          //
                const bool                  a_ACCEPT_ALL,                     //
                const bool                  a_ACCEPT_OBJECTIVE_IMPROVABLE,    //
                const bool                  a_ACCEPT_FEASIBILITY_IMPROVABLE,  //
                [[maybe_unused]] const bool a_IS_ENABLED_PARALLEL) {
                const int DELTA_MAX = 10000;
#ifdef _OPENMP
#pragma omp parallel for if (a_IS_ENABLED_PARALLEL) schedule(static)
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
                            (*a_moves)[4 * i].alterations.front().second =
                                value + 1;
                            (*a_flags)[4 * i] = 1;
                        }

                        if (value == lower_bound) {
                            (*a_flags)[4 * i + 1] = 0;
                        } else {
                            (*a_moves)[4 * i + 1].alterations.front().second =
                                value - 1;
                            (*a_flags)[4 * i + 1] = 1;
                        }

                        if (value == upper_bound ||
                            upper_bound == constant::INT_HALF_MAX) {
                            (*a_flags)[4 * i + 2] = 0;
                        } else {
                            const auto delta =
                                std::min(DELTA_MAX, (upper_bound - value) / 2);
                            (*a_moves)[4 * i + 2].alterations.front().second =
                                value + delta;
                            (*a_flags)[4 * i + 2] = 1;
                        }

                        if (value == lower_bound ||
                            lower_bound == constant::INT_HALF_MIN) {
                            (*a_flags)[4 * i + 3] = 0;
                        } else {
                            const auto delta =
                                std::max(-DELTA_MAX, (lower_bound - value) / 2);
                            (*a_moves)[4 * i + 3].alterations.front().second =
                                value + delta;
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
}  // namespace neighborhood
}  // namespace printemps
#endif
/*****************************************************************************/
// END
/*****************************************************************************/