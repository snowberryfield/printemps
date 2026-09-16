/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
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
    inline void initialize(void) override {
        AbstractMoveGenerator<T_Variable, T_Expression>::initialize();
        this->m_move_updater_wrapper =
            [](std::vector<Move<T_Variable, T_Expression>> *) {};
    }

    /*************************************************************************/
    inline void set_move_updater(
        const std::function<void(std::vector<Move<T_Variable, T_Expression>> *)>
            &a_MOVE_UPDATER) {
        this->m_move_updater_wrapper = a_MOVE_UPDATER;
    }

    /*************************************************************************/
    void setup(void) {
        auto move_updater =                                     //
            [this](auto      *a_moves_ptr,                      //
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
                m_move_updater_wrapper(a_moves_ptr);
                const int MOVES_SIZE = a_moves_ptr->size();
                const int NUM_WORDS  = (MOVES_SIZE + 63) / 64;
                a_flags->resize(MOVES_SIZE);

#ifdef _OPENMP
#pragma omp parallel for if (a_IS_ENABLED_PARALLEL) schedule(static) \
    num_threads(a_NUMBER_OF_THREADS)
#endif
                for (auto w = 0; w < NUM_WORDS; w++) {
                    const int     start_i = w * 64;
                    const int     end_i   = std::min(start_i + 64, MOVES_SIZE);
                    std::uint64_t word    = 0;

                    for (auto i = start_i; i < end_i; i++) {
                        (*a_moves_ptr)[i].type = MoveType::UserDefined;

                        if ((*a_moves_ptr)[i].has_fixed_variable()) {
                            continue;
                        }

                        if ((*a_moves_ptr)[i].has_selection_variable()) {
                            continue;
                        }

                        if ((*a_moves_ptr)[i].has_bound_violation()) {
                            continue;
                        }

                        if (a_ACCEPT_ALL) {
                            word |= (1ULL << (i - start_i));
                        } else {
                            if (a_ACCEPT_OBJECTIVE_IMPROVABLE &&
                                (*a_moves_ptr)[i]
                                    .has_objective_improvable_variable()) {
                                word |= (1ULL << (i - start_i));
                                continue;
                            }

                            if (a_ACCEPT_FEASIBILITY_IMPROVABLE &&
                                (*a_moves_ptr)[i]
                                    .has_feasibility_improvable_variable()) {
                                word |= (1ULL << (i - start_i));
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