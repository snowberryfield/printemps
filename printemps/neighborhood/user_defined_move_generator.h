/*****************************************************************************/
// Copyright (c) 2020-2023 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_NEIGHBORHOOD_USER_DEFINED_MOVE_GENERATOR_H__
#define PRINTEMPS_NEIGHBORHOOD_USER_DEFINED_MOVE_GENERATOR_H__

#include "abstract_move_generator.h"

namespace printemps::neighborhood {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class UserDefinedMoveGenerator
    : public AbstractMoveGenerator<T_Variable, T_Expression> {
   private:
    std::function<void(std::vector<Move<T_Variable, T_Expression>> *)>
        m_move_updater_wrapper;

   public:
    /*************************************************************************/
    UserDefinedMoveGenerator(void) {
        this->initialize();
    }

    /*************************************************************************/
    virtual ~UserDefinedMoveGenerator(void) {
        /// nothing to do
    }

    /*************************************************************************/
    inline constexpr void initialize(void) {
        this->m_move_updater_wrapper =
            [](std::vector<Move<T_Variable, T_Expression>> *) {};
    }

    /*************************************************************************/
    inline constexpr void set_move_updater(
        const std::function<void(std::vector<Move<T_Variable, T_Expression>> *)>
            &a_MOVE_UPDATER) {
        this->m_move_updater_wrapper = a_MOVE_UPDATER;
    }

    /*************************************************************************/
    void setup(void) {
        auto move_updater =                                     //
            [this](auto *     a_moves_ptr,                      //
                   auto *     a_flags,                          //
                   const bool a_ACCEPT_ALL,                     //
                   const bool a_ACCEPT_OBJECTIVE_IMPROVABLE,    //
                   const bool a_ACCEPT_FEASIBILITY_IMPROVABLE,  //
                   [[maybe_unused]] const bool a_IS_ENABLED_PARALLEL) {
                m_move_updater_wrapper(a_moves_ptr);
                const int MOVES_SIZE = a_moves_ptr->size();
                a_flags->resize(MOVES_SIZE);

#ifdef _OPENMP
#pragma omp parallel for if (a_IS_ENABLED_PARALLEL) schedule(static)
#endif
                for (auto i = 0; i < MOVES_SIZE; i++) {
                    (*a_moves_ptr)[i].sense = MoveSense::UserDefined;
                    (*a_flags)[i]           = 1;
                    if (neighborhood::has_fixed_variable((*a_moves_ptr)[i])) {
                        (*a_flags)[i] = 0;
                        continue;
                    }
                    if (neighborhood::has_selection_variable(
                            (*a_moves_ptr)[i])) {
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
}  // namespace printemps::neighborhood
#endif
/*****************************************************************************/
// END
/*****************************************************************************/