/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_NEIGHBORHOOD_CONSTANT_SUM_INTEGERS_MOVE_GENERATOR_H__
#define PRINTEMPS_NEIGHBORHOOD_CONSTANT_SUM_INTEGERS_MOVE_GENERATOR_H__

#include "abstract_move_generator.h"

namespace printemps::neighborhood {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class ConstantSumIntegersMoveGenerator
    : public AbstractMoveGenerator<T_Variable, T_Expression> {
   private:
   public:
    /*************************************************************************/
    ConstantSumIntegersMoveGenerator(void) {
        /// nothing to do
    }

    /*************************************************************************/
    void setup(const std::vector<model_component::Constraint<
                   T_Variable, T_Expression> *> &a_RAW_CONSTRAINT_PTRS) {
        this->initialize();
        /**
         * Exclude constraints which contain fixed variables or selection
         * variables.
         */
        auto constraint_ptrs =
            extract_effective_constraint_ptrs(a_RAW_CONSTRAINT_PTRS);

        /**
         * Convert constraint objects to BinomialConstraint objects.
         */
        auto binomials =
            convert_to_binomial_constraints(constraint_ptrs, false);

        /**
         * Setup move objects.
         */
        const int BINOMIALS_SIZE = binomials.size();

        this->m_moves.clear();
        this->m_flags.clear();

        this->m_moves.resize(2 * BINOMIALS_SIZE);
        this->m_flags.resize(2 * BINOMIALS_SIZE);

        std::vector<double> constant_values(BINOMIALS_SIZE);

        for (int i = 0; i < BINOMIALS_SIZE; i++) {
            auto &move = this->m_moves[2 * i];

            move.associated_constraint_ptr = constraint_ptrs[i];
            move.type                      = MoveType::ConstantSumIntegers;
            move.alterations.emplace_back(binomials[i].variable_ptrs[0], 0);
            move.alterations.emplace_back(binomials[i].variable_ptrs[1], 0);
            move.is_univariable_move          = false;
            move.is_selection_move            = false;
            move.is_special_neighborhood_move = true;
            move.is_available                 = true;
            move.overlap_rate                 = 0.0;
            move.setup_related_constraint_ptrs();

            constant_values[i] =
                constraint_ptrs[i]->expression().constant_value();

            this->m_moves[2 * i + 1] = move;
        }

        /**
         * Setup move updater.
         */
        auto move_updater =  //
            [binomials, constant_values, BINOMIALS_SIZE](
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
                for (int i = 0; i < BINOMIALS_SIZE; i++) {
                    {
                        const int INDEX   = 2 * i;
                        auto &alterations = (*a_moves_ptr)[INDEX].alterations;

                        alterations[0].second =
                            binomials[i].variable_ptrs[0]->value() + 1;
                        alterations[1].second =
                            -constant_values[i] - alterations[0].second;
                    }
                    {
                        const int INDEX   = 2 * i + 1;
                        auto &alterations = (*a_moves_ptr)[INDEX].alterations;

                        alterations[0].second =
                            binomials[i].variable_ptrs[0]->value() - 1;
                        alterations[1].second =
                            -constant_values[i] - alterations[0].second;
                    }
                }
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
                        if (!(*a_moves_ptr)[i]
                                 .associated_constraint_ptr->is_feasible()) {
                            continue;
                        }

                        if (!(*a_moves_ptr)[i].is_available) {
                            continue;
                        }

                        if ((*a_moves_ptr)[i].has_fixed_variable()) {
                            continue;
                        }

                        if ((*a_moves_ptr)[i].has_bound_violation()) {
                            continue;
                        }

                        if (a_ACCEPT_ALL) {
                            word |= (1ULL << (i - START));
                        } else {
                            if (a_ACCEPT_OBJECTIVE_IMPROVABLE &&
                                (*a_moves_ptr)[i]
                                    .has_objective_improvable_variable()) {
                                word |= (1ULL << (i - START));
                                continue;
                            }

                            if (a_ACCEPT_FEASIBILITY_IMPROVABLE &&
                                (*a_moves_ptr)[i]
                                    .has_feasibility_improvable_variable()) {
                                word |= (1ULL << (i - START));
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