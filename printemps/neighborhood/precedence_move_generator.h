/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_NEIGHBORHOOD_PRECEDENCE_MOVE_GENERATOR_H__
#define PRINTEMPS_NEIGHBORHOOD_PRECEDENCE_MOVE_GENERATOR_H__

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
class PrecedenceMoveGenerator
    : public AbstractMoveGenerator<T_Variable, T_Expression> {
   private:
   public:
    /*************************************************************************/
    PrecedenceMoveGenerator(void) {
        /// nothing to do
    }

    /*************************************************************************/
    virtual ~PrecedenceMoveGenerator(void) {
        /// nothing to do
    }

    /*************************************************************************/
    constexpr void setup(
        const std::vector<model::Constraint<T_Variable, T_Expression> *>
            &a_CONSTRAINT_PTRS) {
        const int RAW_CONSTRAINTS_SIZE = a_CONSTRAINT_PTRS.size();

        std::vector<std::vector<model::Variable<T_Variable, T_Expression> *>>
            variable_ptr_pairs;

        for (auto i = 0; i < RAW_CONSTRAINTS_SIZE; i++) {
            if (a_CONSTRAINT_PTRS[i]->is_enabled()) {
                auto &sensitivities =
                    a_CONSTRAINT_PTRS[i]->expression().sensitivities();
                std::vector<model::Variable<T_Variable, T_Expression> *>
                    variable_ptr_pair;
                for (auto &&sensitivity : sensitivities) {
                    variable_ptr_pair.push_back(sensitivity.first);
                }
                if (variable_ptr_pair[0]->is_fixed() ||
                    variable_ptr_pair[1]->is_fixed() ||
                    (variable_ptr_pair[0]->sense() ==
                     model::VariableSense::Selection) ||
                    (variable_ptr_pair[1]->sense() ==
                     model::VariableSense::Selection)) {
                    continue;
                } else {
                    variable_ptr_pairs.push_back(variable_ptr_pair);
                }
            }
        }

        const int PAIRS_SIZE = variable_ptr_pairs.size();
        this->m_moves.resize(2 * PAIRS_SIZE);
        this->m_flags.resize(2 * PAIRS_SIZE);

        for (auto i = 0; i < PAIRS_SIZE; i++) {
            this->m_moves[2 * i].sense = MoveSense::Precedence;
            this->m_moves[2 * i].related_constraint_ptrs.insert(
                variable_ptr_pairs[i][0]->related_constraint_ptrs().begin(),
                variable_ptr_pairs[i][0]->related_constraint_ptrs().end());
            this->m_moves[2 * i].related_constraint_ptrs.insert(
                variable_ptr_pairs[i][1]->related_constraint_ptrs().begin(),
                variable_ptr_pairs[i][1]->related_constraint_ptrs().end());

            this->m_moves[2 * i].is_special_neighborhood_move = true;
            this->m_moves[2 * i].is_available                 = true;
            this->m_moves[2 * i].overlap_rate                 = 0.0;

            this->m_moves[2 * i].alterations.emplace_back(
                variable_ptr_pairs[i][0], 0);
            this->m_moves[2 * i].alterations.emplace_back(
                variable_ptr_pairs[i][1], 0);

            this->m_moves[2 * i + 1] = this->m_moves[2 * i];
        }

        auto move_updater =  //
            [this, variable_ptr_pairs, PAIRS_SIZE](
                auto *                      a_moves,                          //
                auto *                      a_flags,                          //
                const bool                  a_ACCEPT_ALL,                     //
                const bool                  a_ACCEPT_OBJECTIVE_IMPROVABLE,    //
                const bool                  a_ACCEPT_FEASIBILITY_IMPROVABLE,  //
                [[maybe_unused]] const bool a_IS_ENABLED_PARALLEL) {

#ifdef _OPENMP
#pragma omp parallel for if (a_IS_ENABLED_PARALLEL) schedule(static)
#endif
                for (auto i = 0; i < PAIRS_SIZE; i++) {
                    (*a_moves)[2 * i].alterations.clear();
                    (*a_moves)[2 * i].alterations.emplace_back(
                        variable_ptr_pairs[i][0],
                        variable_ptr_pairs[i][0]->value() + 1);
                    (*a_moves)[2 * i].alterations.emplace_back(
                        variable_ptr_pairs[i][1],
                        variable_ptr_pairs[i][1]->value() + 1);
                    (*a_moves)[2 * i + 1].alterations.clear();
                    (*a_moves)[2 * i + 1].alterations.emplace_back(
                        variable_ptr_pairs[i][0],
                        variable_ptr_pairs[i][0]->value() - 1);
                    (*a_moves)[2 * i + 1].alterations.emplace_back(
                        variable_ptr_pairs[i][1],
                        variable_ptr_pairs[i][1]->value() - 1);
                }

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
                    if (neighborhood::has_fixed_variables((*a_moves)[i])) {
                        (*a_flags)[i] = 0;
                        continue;
                    }
                    if (neighborhood::has_bound_violation((*a_moves)[i])) {
                        (*a_flags)[i] = 0;
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