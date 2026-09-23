/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_NEIGHBORHOOD_CHAIN_MOVE_GENERATOR_H__
#define PRINTEMPS_NEIGHBORHOOD_CHAIN_MOVE_GENERATOR_H__

#include "abstract_move_generator.h"

namespace printemps::neighborhood {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class ChainMoveGenerator
    : public AbstractMoveGenerator<T_Variable, T_Expression> {
   private:
   public:
    /*************************************************************************/
    ChainMoveGenerator(void) {
        /// nothing to do
    }

    /*************************************************************************/
    void setup(void) {
        this->initialize();
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

    /*************************************************************************/
    inline void register_move(const Move<T_Variable, T_Expression> &a_MOVE) {
        this->m_moves.push_back(a_MOVE);
        this->m_flags.resize(this->m_moves.size());
    }

    /*************************************************************************/
    inline void clear_moves() {
        this->m_moves.clear();
        this->m_flags.clear();
    }

    /*************************************************************************/
    inline void deduplicate_moves() {
        this->m_moves.erase(std::unique(this->m_moves.begin(),  //
                                        this->m_moves.end()),
                            this->m_moves.end());
        this->m_flags.resize(this->m_moves.size());
    }

    /*************************************************************************/
    inline void sort_moves(void) {
        std::stable_sort(
            this->m_moves.begin(), this->m_moves.end(),
            [](const auto &a_LHS, const auto &a_RHS) {
                /**
                 * Firstly, compare the overlap rates.
                 */
                auto overlap_difference =
                    a_LHS.overlap_rate - a_RHS.overlap_rate;
                if (overlap_difference > constant::EPSILON_10) {
                    return true;
                } else if (overlap_difference < -constant::EPSILON_10) {
                    return false;
                }

                /**
                 * Secondly, compare the hashes
                 */
                if (a_LHS.hash > a_LHS.hash) {
                    return true;
                } else if (a_LHS.hash < a_LHS.hash) {
                    return false;
                }

                /**
                 * Thirdly, compare the number of alterations.
                 */
                int alterations_size_difference =
                    a_LHS.alterations.size() - a_RHS.alterations.size();
                if (alterations_size_difference > 0) {
                    return true;
                } else if (alterations_size_difference < 0) {
                    return false;
                }

                /**
                 * Fourthly, compare the number of related constraints.
                 */
                int related_constraints_size_difference =
                    a_LHS.related_constraint_ptrs.size() -
                    a_RHS.related_constraint_ptrs.size();

                if (related_constraints_size_difference > 0) {
                    return true;
                } else if (related_constraints_size_difference < 0) {
                    return false;
                }

                /**
                 * Fifthly, compare the addresses of variables.
                 */
                const int ALTERATIONS_SIZE = a_LHS.alterations.size();

                for (int i = 0; i < ALTERATIONS_SIZE; i++) {
                    int address_difference  //
                        = reinterpret_cast<std::uint_fast64_t>(
                              a_LHS.alterations[i].first) -
                          reinterpret_cast<std::uint_fast64_t>(
                              a_RHS.alterations[i].first);
                    if (address_difference > 0) {
                        return true;
                    } else if (address_difference < 0) {
                        return false;
                    }
                }

                /**
                 * Finally, compare the values of variables.
                 */

                for (int i = 0; i < ALTERATIONS_SIZE; i++) {
                    int value_difference  //
                        = a_LHS.alterations[i].second -
                          a_RHS.alterations[i].second;

                    if (value_difference > 0) {
                        return true;
                    } else if (value_difference < 0) {
                        return false;
                    }
                }
                return false;
            });
    }

    /*************************************************************************/
    inline void shuffle_moves(std::mt19937 *a_rand) {
        std::shuffle(this->m_moves.begin(), this->m_moves.end(), *a_rand);
    }

    /*************************************************************************/
    inline void reduce_moves(const int a_NUMBER_OF_MOVES) {
        if (static_cast<int>(this->m_moves.size()) <= a_NUMBER_OF_MOVES) {
            return;
        }

        this->m_moves.resize(a_NUMBER_OF_MOVES);
        this->m_flags.resize(a_NUMBER_OF_MOVES);
    }

    /*************************************************************************/
    inline void remove_moves_on_fixed_variables(void) {
        this->m_moves.erase(
            std::remove_if(
                this->m_moves.begin(), this->m_moves.end(),
                [](const auto &a_MOVE) { return a_MOVE.has_fixed_variable(); }),
            this->m_moves.end());
        this->m_flags.resize(this->m_moves.size());
    }
};
}  // namespace printemps::neighborhood
#endif
/*****************************************************************************/
// END
/*****************************************************************************/