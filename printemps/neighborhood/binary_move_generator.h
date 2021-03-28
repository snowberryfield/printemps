/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_NEIGHBORHOOD_BINARY_MOVE_GENERATOR_H__
#define PRINTEMPS_NEIGHBORHOOD_BINARY_MOVE_GENERATOR_H__

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
class BinaryMoveGenerator
    : public AbstractMoveGenerator<T_Variable, T_Expression> {
   private:
   public:
    /*************************************************************************/
    BinaryMoveGenerator(void) {
        /// nothing to do
    }

    /*************************************************************************/
    virtual ~BinaryMoveGenerator(void) {
        /// nothing to do
    }

    /*************************************************************************/
    void setup(const std::vector<model::Variable<T_Variable, T_Expression> *>
                   &a_RAW_VARIABLE_PTRS) {
        /**
         * "Flip" move for binary variables:
         * e.g) binary variable x \in {0, 1}
         *  move: {(x = 1)} (if x = 0)
         *        {(x = 0)} (if x = 1)
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
        this->m_moves.resize(VARIABLES_SIZE);
        this->m_flags.resize(VARIABLES_SIZE);

        for (auto i = 0; i < VARIABLES_SIZE; i++) {
            this->m_moves[i].sense = MoveSense::Binary;
            this->m_moves[i].related_constraint_ptrs =
                mutable_variable_ptrs[i]->related_constraint_ptrs();
            this->m_moves[i].alterations.emplace_back(mutable_variable_ptrs[i],
                                                      0);
            this->m_moves[i].is_special_neighborhood_move = false;
            this->m_moves[i].is_available                 = true;
            this->m_moves[i].overlap_rate                 = 0.0;
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
#ifdef _OPENMP
#pragma omp parallel for if (a_IS_ENABLED_PARALLEL) schedule(static)
#endif
                for (auto i = 0; i < VARIABLES_SIZE; i++) {
                    if (a_ACCEPT_ALL ||
                        (a_ACCEPT_OBJECTIVE_IMPROVABLE &&
                         mutable_variable_ptrs[i]->is_objective_improvable()) ||
                        (a_ACCEPT_FEASIBILITY_IMPROVABLE &&
                         mutable_variable_ptrs[i]
                             ->is_feasibility_improvable())) {
                        (*a_moves)[i].alterations.front().second =
                            1 - mutable_variable_ptrs[i]->value();
                        (*a_flags)[i] = 1;
                    } else {
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