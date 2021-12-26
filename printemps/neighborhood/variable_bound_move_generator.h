/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_NEIGHBORHOOD_VARIABLE_BOUND_MOVE_MOVE_GENERATOR_H__
#define PRINTEMPS_NEIGHBORHOOD_VARIABLE_BOUND_MOVE_MOVE_GENERATOR_H__

#include "abstract_move_generator.h"

namespace printemps {
namespace neighborhood {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class VariableBoundMoveGenerator
    : public AbstractMoveGenerator<T_Variable, T_Expression> {
   private:
   public:
    /*************************************************************************/
    VariableBoundMoveGenerator(void) {
        /// nothing to do
    }

    /*************************************************************************/
    virtual ~VariableBoundMoveGenerator(void) {
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
        this->m_moves.resize(4 * BINOMIALS_SIZE);
        this->m_flags.resize(4 * BINOMIALS_SIZE);

        for (auto i = 0; i < BINOMIALS_SIZE; i++) {
            auto &move = this->m_moves[4 * i];
            move.sense = MoveSense::VariableBound;
            move.alterations.emplace_back(binomials[i].variable_ptr_first, 0);
            move.alterations.emplace_back(binomials[i].variable_ptr_second, 0);
            move.is_univariable_move = false;
            move.is_selection_move   = false;

            utility::update_union_set(
                &(move.related_constraint_ptrs),
                binomials[i].variable_ptr_first->related_constraint_ptrs());

            utility::update_union_set(
                &(move.related_constraint_ptrs),
                binomials[i].variable_ptr_second->related_constraint_ptrs());

            move.is_special_neighborhood_move = true;
            move.is_available                 = true;
            move.overlap_rate                 = 0.0;

            this->m_moves[4 * i + 1] = move;
            this->m_moves[4 * i + 2] = move;
            this->m_moves[4 * i + 3] = move;
        }

        /**
         * Setup move updater.
         */
        auto move_updater =  //
            [this, binomials, BINOMIALS_SIZE](
                auto *                      a_moves_ptr,                      //
                auto *                      a_flags,                          //
                const bool                  a_ACCEPT_ALL,                     //
                const bool                  a_ACCEPT_OBJECTIVE_IMPROVABLE,    //
                const bool                  a_ACCEPT_FEASIBILITY_IMPROVABLE,  //
                [[maybe_unused]] const bool a_IS_ENABLED_PARALLEL) {
#ifdef _OPENMP
#pragma omp parallel for if (a_IS_ENABLED_PARALLEL) schedule(static)
#endif
                for (auto i = 0; i < BINOMIALS_SIZE; i++) {
                    {
                        auto  index       = 4 * i;
                        auto &alterations = (*a_moves_ptr)[index].alterations;

                        T_Variable target = 0;
                        double     target_temp =
                            (-binomials[i].constant_value -
                             binomials[i].sensitivity_first *
                                 (binomials[i].variable_ptr_first->value() +
                                  1)) /
                            binomials[i].sensitivity_second;

                        if ((binomials[i].sensitivity_second > 0 &&
                             binomials[i].sense ==
                                 model_component::ConstraintSense::Less) ||
                            (binomials[i].sensitivity_second < 0 &&
                             binomials[i].sense ==
                                 model_component::ConstraintSense::Greater)) {
                            target = static_cast<T_Variable>(
                                std::floor(target_temp));

                        } else {
                            target =
                                static_cast<T_Variable>(std::ceil(target_temp));
                        }

                        alterations[0].second =
                            binomials[i].variable_ptr_first->value() + 1;
                        alterations[1].second = target;
                    }

                    {
                        auto  index       = 4 * i + 1;
                        auto &alterations = (*a_moves_ptr)[index].alterations;

                        T_Variable target = 0;
                        double     target_temp =
                            (-binomials[i].constant_value -
                             binomials[i].sensitivity_first *
                                 (binomials[i].variable_ptr_first->value() -
                                  1)) /
                            binomials[i].sensitivity_second;

                        if ((binomials[i].sensitivity_second > 0 &&
                             binomials[i].sense ==
                                 model_component::ConstraintSense::Less) ||
                            (binomials[i].sensitivity_second < 0 &&
                             binomials[i].sense ==
                                 model_component::ConstraintSense::Greater)) {
                            target = static_cast<T_Variable>(
                                std::floor(target_temp));

                        } else {
                            target =
                                static_cast<T_Variable>(std::ceil(target_temp));
                        }

                        alterations[0].second =
                            binomials[i].variable_ptr_first->value() - 1;
                        alterations[1].second = target;
                    }

                    {
                        auto  index       = 4 * i + 2;
                        auto &alterations = (*a_moves_ptr)[index].alterations;

                        T_Variable target = 0;
                        double     target_temp =
                            (-binomials[i].constant_value -
                             binomials[i].sensitivity_second *
                                 (binomials[i].variable_ptr_second->value() +
                                  1)) /
                            binomials[i].sensitivity_first;

                        if ((binomials[i].sensitivity_first > 0 &&
                             binomials[i].sense ==
                                 model_component::ConstraintSense::Less) ||
                            (binomials[i].sensitivity_first < 0 &&
                             binomials[i].sense ==
                                 model_component::ConstraintSense::Greater)) {
                            target = static_cast<T_Variable>(
                                std::floor(target_temp));

                        } else {
                            target =
                                static_cast<T_Variable>(std::ceil(target_temp));
                        }

                        alterations[0].second = target;
                        alterations[1].second =
                            binomials[i].variable_ptr_second->value() + 1;
                    }

                    {
                        auto  index       = 4 * i + 3;
                        auto &alterations = (*a_moves_ptr)[index].alterations;

                        T_Variable target = 0;
                        double     target_temp =
                            (-binomials[i].constant_value -
                             binomials[i].sensitivity_second *
                                 (binomials[i].variable_ptr_second->value() -
                                  1)) /
                            binomials[i].sensitivity_first;

                        if ((binomials[i].sensitivity_first > 0 &&
                             binomials[i].sense ==
                                 model_component::ConstraintSense::Less) ||
                            (binomials[i].sensitivity_first < 0 &&
                             binomials[i].sense ==
                                 model_component::ConstraintSense::Greater)) {
                            target = static_cast<T_Variable>(
                                std::floor(target_temp));

                        } else {
                            target =
                                static_cast<T_Variable>(std::ceil(target_temp));
                        }

                        alterations[0].second = target;
                        alterations[1].second =
                            binomials[i].variable_ptr_second->value() - 1;
                    }
                }

                const int MOVES_SIZE = a_moves_ptr->size();
#ifdef _OPENMP
#pragma omp parallel for if (a_IS_ENABLED_PARALLEL) schedule(static)
#endif
                for (auto i = 0; i < MOVES_SIZE; i++) {
                    (*a_flags)[i] = 1;
                    if (!(*a_moves_ptr)[i].is_available) {
                        (*a_flags)[i] = 0;
                        continue;
                    }
                    if (neighborhood::has_fixed_variable((*a_moves_ptr)[i])) {
                        (*a_flags)[i] = 0;
                        continue;
                    }
                    if (neighborhood::has_bound_violation((*a_moves_ptr)[i])) {
                        (*a_flags)[i] = 0;
                        continue;
                    }
                    if (a_ACCEPT_ALL) {
                        /** nothing to do */
                    } else {
                        if (a_ACCEPT_OBJECTIVE_IMPROVABLE &&
                            neighborhood::has_objective_improvable_variable(
                                (*a_moves_ptr)[i])) {
                            continue;
                        }

                        if (a_ACCEPT_FEASIBILITY_IMPROVABLE &&
                            neighborhood::has_feasibility_improvable_variable(
                                (*a_moves_ptr)[i])) {
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