/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_NEIGHBORHOOD_CHAIN_MOVE_GENERATOR_H__
#define PRINTEMPS_NEIGHBORHOOD_CHAIN_MOVE_GENERATOR_H__

#include "abstract_move_generator.h"

namespace printemps {
namespace neighborhood {
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
    virtual ~ChainMoveGenerator(void) {
        /// nothing to do
    }

    /*************************************************************************/
    void setup(void) {
        auto move_updater =                                     //
            [this](auto *     a_moves,                          //
                   auto *     a_flags,                          //
                   const bool a_ACCEPT_ALL,                     //
                   const bool a_ACCEPT_OBJECTIVE_IMPROVABLE,    //
                   const bool a_ACCEPT_FEASIBILITY_IMPROVABLE,  //
                   [[maybe_unused]] const bool a_IS_ENABLED_PARALLEL) {
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
                    if (neighborhood::has_fixed_variable((*a_moves)[i])) {
                        (*a_flags)[i] = 0;
                        continue;
                    }
                    for (const auto &alteration : (*a_moves)[i].alterations) {
                        if (alteration.first->value() == alteration.second) {
                            (*a_flags)[i] = 0;
                            break;
                        }
                    }
                    if ((*a_flags)[i] == 0) {
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

    /*************************************************************************/
    inline constexpr void register_move(
        const Move<T_Variable, T_Expression> &a_MOVE) {
        this->m_moves.push_back(a_MOVE);
        this->m_flags.resize(this->m_moves.size());
    }

    /*************************************************************************/
    inline constexpr void clear_moves() {
        this->m_moves.clear();
        this->m_flags.clear();
    }

    /*************************************************************************/
    constexpr void deduplicate_moves() {
        this->m_moves.erase(std::unique(this->m_moves.begin(),  //
                                        this->m_moves.end()),
                            this->m_moves.end());
        this->m_flags.resize(this->m_moves.size());
    }

    /*************************************************************************/
    inline constexpr void sort_moves(void) {
        std::sort(this->m_moves.begin(), this->m_moves.end(),
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

                      for (auto i = 0; i < ALTERATIONS_SIZE; i++) {
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

                      for (auto i = 0; i < ALTERATIONS_SIZE; i++) {
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
    inline constexpr void shuffle_moves(std::mt19937 *a_rand) {
        std::shuffle(this->m_moves.begin(), this->m_moves.end(), *a_rand);
    }

    /*************************************************************************/
    inline constexpr void reduce_moves(const int a_NUMBER_OF_MOVES) {
        if (static_cast<int>(this->m_moves.size()) <= a_NUMBER_OF_MOVES) {
            return;
        }

        this->m_moves.resize(a_NUMBER_OF_MOVES);
        this->m_flags.resize(a_NUMBER_OF_MOVES);
    }
};
}  // namespace neighborhood
}  // namespace printemps
#endif
/*****************************************************************************/
// END
/*****************************************************************************/