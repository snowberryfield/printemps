/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_NEIGHBORHOOD_NEIGHBORHOOD_H__
#define PRINTEMPS_NEIGHBORHOOD_NEIGHBORHOOD_H__

#include "move_sense.h"
#include "move.h"
#include "binomial_constraint.h"

#include "binary_move_generator.h"
#include "integer_move_generator.h"
#include "selection_move_generator.h"
#include "aggregation_move_generator.h"
#include "precedence_move_generator.h"
#include "variable_bound_move_generator.h"
#include "soft_selection_move_generator.h"
#include "chain_move_generator.h"
#include "two_flip_move_generator.h"
#include "user_defined_move_generator.h"

namespace printemps::neighborhood {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Neighborhood {
   private:
    BinaryMoveGenerator<T_Variable, T_Expression>        m_binary;
    IntegerMoveGenerator<T_Variable, T_Expression>       m_integer;
    SelectionMoveGenerator<T_Variable, T_Expression>     m_selection;
    AggregationMoveGenerator<T_Variable, T_Expression>   m_aggregation;
    PrecedenceMoveGenerator<T_Variable, T_Expression>    m_precedence;
    VariableBoundMoveGenerator<T_Variable, T_Expression> m_variable_bound;
    SoftSelectionMoveGenerator<T_Variable, T_Expression> m_soft_selection;
    ChainMoveGenerator<T_Variable, T_Expression>         m_chain;
    TwoFlipMoveGenerator<T_Variable, T_Expression>       m_two_flip;
    UserDefinedMoveGenerator<T_Variable, T_Expression>   m_user_defined;

    std::vector<AbstractMoveGenerator<T_Variable, T_Expression> *>
                                                  m_move_generator_ptrs;
    std::vector<Move<T_Variable, T_Expression> *> m_move_ptrs;

   public:
    /*************************************************************************/
    Neighborhood(void) {
        this->initialize();
    }

    /*************************************************************************/
    virtual ~Neighborhood(void) {
        /// nothing to do
    }

    /*************************************************************************/
    void initialize(void) {
        m_binary.initialize();
        m_integer.initialize();
        m_selection.initialize();
        m_precedence.initialize();
        m_aggregation.initialize();
        m_variable_bound.initialize();
        m_soft_selection.initialize();
        m_chain.initialize();
        m_two_flip.initialize();
        m_user_defined.initialize();

        m_move_generator_ptrs = {&m_binary,          //
                                 &m_integer,         //
                                 &m_selection,       //
                                 &m_aggregation,     //
                                 &m_precedence,      //
                                 &m_variable_bound,  //
                                 &m_soft_selection,  //
                                 &m_chain,           //
                                 &m_two_flip,        //
                                 &m_user_defined};

        m_move_ptrs.clear();
    }

    /*************************************************************************/
    constexpr void update_moves(
        const bool                  a_ACCEPT_ALL,                     //
        const bool                  a_ACCEPT_OBJECTIVE_IMPROVABLE,    //
        const bool                  a_ACCEPT_FEASIBILITY_IMPROVABLE,  //
        [[maybe_unused]] const bool a_IS_ENABLED_PARALLEL) {
        auto number_of_candidate_moves = 0;

        for (auto &&move_generator_ptr : m_move_generator_ptrs) {
            if (move_generator_ptr->is_enabled()) {
                move_generator_ptr->update_moves(
                    a_ACCEPT_ALL,                     //
                    a_ACCEPT_OBJECTIVE_IMPROVABLE,    //
                    a_ACCEPT_FEASIBILITY_IMPROVABLE,  //
                    a_IS_ENABLED_PARALLEL);
                auto &flags = move_generator_ptr->flags();
                number_of_candidate_moves +=
                    std::accumulate(flags.begin(), flags.end(), 0);
            }
        }

        auto &move_ptrs = m_move_ptrs;
        move_ptrs.resize(number_of_candidate_moves);
        auto index = 0;

        for (const auto &move_generator_ptr : m_move_generator_ptrs) {
            if (move_generator_ptr->is_enabled()) {
                auto &    moves     = move_generator_ptr->moves();
                auto &    flags     = move_generator_ptr->flags();
                const int MOVE_SIZE = move_generator_ptr->moves().size();

                for (auto i = 0; i < MOVE_SIZE; i++) {
                    if (flags[i]) {
                        move_ptrs[index++] = &moves[i];
                    }
                }
            }
        }
    }

    /*************************************************************************/
    inline constexpr std::vector<Move<T_Variable, T_Expression> *> &move_ptrs(
        void) noexcept {
        return m_move_ptrs;
    }

    /*************************************************************************/
    inline constexpr const std::vector<Move<T_Variable, T_Expression> *>
        &move_ptrs(void) const noexcept {
        return m_move_ptrs;
    }

    /*************************************************************************/
    inline constexpr void shuffle_moves(std::mt19937 *a_rand) noexcept {
        std::shuffle(m_move_ptrs.begin(), m_move_ptrs.end(), *a_rand);
    }

    /*************************************************************************/
    inline constexpr void reset_special_neighborhood_moves_availability(
        void) noexcept {
        m_aggregation.reset_availability();
        m_precedence.reset_availability();
        m_variable_bound.reset_availability();
        m_soft_selection.reset_availability();
        m_chain.reset_availability();
        m_two_flip.reset_availability();
    }

    /*************************************************************************/
    inline constexpr bool is_enabled_special_neighborhood_move(void) const {
        if (m_precedence.is_enabled()) {
            return true;
        }

        if (m_aggregation.is_enabled()) {
            return true;
        }

        if (m_variable_bound.is_enabled()) {
            return true;
        }

        if (m_soft_selection.is_enabled()) {
            return true;
        }

        if (m_chain.is_enabled()) {
            return true;
        }

        if (m_two_flip.is_enabled()) {
            return true;
        }

        return false;
    }

    /*************************************************************************/
    inline constexpr BinaryMoveGenerator<T_Variable, T_Expression>  //
        &binary(void) noexcept {
        return m_binary;
    }

    /*************************************************************************/
    inline constexpr const BinaryMoveGenerator<T_Variable, T_Expression>  //
        &binary(void) const noexcept {
        return m_binary;
    }

    /*************************************************************************/
    inline constexpr IntegerMoveGenerator<T_Variable, T_Expression>  //
        &integer(void) noexcept {
        return m_integer;
    }

    /*************************************************************************/
    inline constexpr const IntegerMoveGenerator<T_Variable, T_Expression>  //
        &integer(void) const noexcept {
        return m_integer;
    }

    /*************************************************************************/
    inline constexpr SelectionMoveGenerator<T_Variable, T_Expression>  //
        &selection(void) noexcept {
        return m_selection;
    }

    /*************************************************************************/
    inline constexpr const SelectionMoveGenerator<T_Variable, T_Expression>  //
        &selection(void) const noexcept {
        return m_selection;
    }

    /*************************************************************************/
    inline constexpr AggregationMoveGenerator<T_Variable, T_Expression>  //
        &aggregation(void) noexcept {
        return m_aggregation;
    }

    /*************************************************************************/
    inline constexpr const AggregationMoveGenerator<T_Variable, T_Expression>
        &aggregation(void) const noexcept {
        return m_aggregation;
    }

    /*************************************************************************/
    inline constexpr PrecedenceMoveGenerator<T_Variable, T_Expression>  //
        &precedence(void) noexcept {
        return m_precedence;
    }

    /*************************************************************************/
    inline constexpr const PrecedenceMoveGenerator<T_Variable, T_Expression>  //
        &precedence(void) const noexcept {
        return m_precedence;
    }

    /*************************************************************************/
    inline constexpr VariableBoundMoveGenerator<T_Variable, T_Expression>  //
        &variable_bound(void) noexcept {
        return m_variable_bound;
    }

    /*************************************************************************/
    inline constexpr const VariableBoundMoveGenerator<T_Variable, T_Expression>
        &variable_bound(void) const noexcept {
        return m_variable_bound;
    }

    /*************************************************************************/
    inline constexpr SoftSelectionMoveGenerator<T_Variable, T_Expression>  //
        &soft_selection(void) noexcept {
        return m_soft_selection;
    }

    /*************************************************************************/
    inline constexpr const SoftSelectionMoveGenerator<T_Variable,
                                                      T_Expression>  //
        &soft_selection(void) const noexcept {
        return m_soft_selection;
    }

    /*************************************************************************/
    inline constexpr ChainMoveGenerator<T_Variable, T_Expression>  //
        &chain(void) noexcept {
        return m_chain;
    }

    /*************************************************************************/
    inline constexpr const ChainMoveGenerator<T_Variable, T_Expression>  //
        &chain(void) const noexcept {
        return m_chain;
    }

    /*************************************************************************/
    inline constexpr TwoFlipMoveGenerator<T_Variable, T_Expression>  //
        &two_flip(void) noexcept {
        return m_two_flip;
    }

    /*************************************************************************/
    inline constexpr const TwoFlipMoveGenerator<T_Variable, T_Expression>  //
        &two_flip(void) const noexcept {
        return m_two_flip;
    }

    /*************************************************************************/
    inline constexpr UserDefinedMoveGenerator<T_Variable, T_Expression>  //
        &user_defined(void) noexcept {
        return m_user_defined;
    }

    /*************************************************************************/
    inline constexpr const UserDefinedMoveGenerator<T_Variable, T_Expression>
        &user_defined(void) const noexcept {
        return m_user_defined;
    }

    /*************************************************************************/
    inline constexpr int number_of_special_neighborhood_moves(void) const {
        return m_aggregation.moves().size() + m_precedence.moves().size() +
               m_variable_bound.moves().size() +
               m_soft_selection.moves().size() + m_chain.moves().size() +
               m_two_flip.moves().size();
    }
};
}  // namespace printemps::neighborhood
#endif
/*****************************************************************************/
// END
/*****************************************************************************/