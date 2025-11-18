/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_NEIGHBORHOOD_NEIGHBORHOOD_H__
#define PRINTEMPS_NEIGHBORHOOD_NEIGHBORHOOD_H__

#include "move_sense.h"
#include "move.h"
#include "binomial_constraint.h"
#include "trinomial_constraint.h"

#include "binary_move_generator.h"
#include "integer_move_generator.h"
#include "selection_move_generator.h"
#include "exclusive_or_move_generator.h"
#include "exclusive_nor_move_generator.h"
#include "inverted_integers_move_generator.h"
#include "balanced_integers_move_generator.h"
#include "constant_sum_integers_move_generator.h"
#include "constant_difference_integers_move_generator.h"
#include "constant_ratio_integers_move_generator.h"
#include "aggregation_move_generator.h"
#include "precedence_move_generator.h"
#include "variable_bound_move_generator.h"
#include "soft_selection_move_generator.h"
#include "trinomial_exclusive_nor_move_generator.h"
#include "chain_move_generator.h"
#include "two_flip_move_generator.h"
#include "user_defined_move_generator.h"

namespace printemps::neighborhood {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Neighborhood {
   private:
    BinaryMoveGenerator<T_Variable, T_Expression>    m_binary;
    IntegerMoveGenerator<T_Variable, T_Expression>   m_integer;
    SelectionMoveGenerator<T_Variable, T_Expression> m_selection;

    ExclusiveOrMoveGenerator<T_Variable, T_Expression>      m_exclusive_or;
    ExclusiveNorMoveGenerator<T_Variable, T_Expression>     m_exclusive_nor;
    InvertedIntegersMoveGenerator<T_Variable, T_Expression> m_inverted_integers;
    BalancedIntegersMoveGenerator<T_Variable, T_Expression> m_balanced_integers;
    ConstantSumIntegersMoveGenerator<T_Variable, T_Expression>
        m_constant_sum_integers;
    ConstantDifferenceIntegersMoveGenerator<T_Variable, T_Expression>
        m_constant_difference_integers;
    ConstantRatioIntegersMoveGenerator<T_Variable, T_Expression>
        m_constant_ratio_integers;

    AggregationMoveGenerator<T_Variable, T_Expression>   m_aggregation;
    PrecedenceMoveGenerator<T_Variable, T_Expression>    m_precedence;
    VariableBoundMoveGenerator<T_Variable, T_Expression> m_variable_bound;
    SoftSelectionMoveGenerator<T_Variable, T_Expression> m_soft_selection;
    TrinomialExclusiveNorMoveGenerator<T_Variable, T_Expression>
                                                   m_trinomial_exclusive_nor;
    ChainMoveGenerator<T_Variable, T_Expression>   m_chain;
    TwoFlipMoveGenerator<T_Variable, T_Expression> m_two_flip;
    UserDefinedMoveGenerator<T_Variable, T_Expression> m_user_defined;

    std::vector<AbstractMoveGenerator<T_Variable, T_Expression> *>
                                                  m_move_generator_ptrs;
    std::vector<Move<T_Variable, T_Expression> *> m_move_ptrs;

    int m_number_of_updated_moves;

   public:
    /*************************************************************************/
    Neighborhood(void) {
        this->initialize();
    }

    /*************************************************************************/
    void initialize(void) {
        m_binary.initialize();
        m_integer.initialize();
        m_selection.initialize();
        m_precedence.initialize();
        m_exclusive_or.initialize();
        m_exclusive_nor.initialize();
        m_inverted_integers.initialize();
        m_balanced_integers.initialize();
        m_constant_sum_integers.initialize();
        m_constant_difference_integers.initialize();
        m_constant_ratio_integers.initialize();
        m_aggregation.initialize();
        m_variable_bound.initialize();
        m_soft_selection.initialize();
        m_trinomial_exclusive_nor.initialize();
        m_chain.initialize();
        m_two_flip.initialize();
        m_user_defined.initialize();

        m_move_generator_ptrs = {&m_binary,                        //
                                 &m_integer,                       //
                                 &m_selection,                     //
                                 &m_exclusive_or,                  //
                                 &m_exclusive_nor,                 //
                                 &m_inverted_integers,             //
                                 &m_balanced_integers,             //
                                 &m_constant_sum_integers,         //
                                 &m_constant_difference_integers,  //
                                 &m_constant_ratio_integers,       //
                                 &m_aggregation,                   //
                                 &m_precedence,                    //
                                 &m_variable_bound,                //
                                 &m_soft_selection,                //
                                 &m_trinomial_exclusive_nor,       //
                                 &m_chain,                         //
                                 &m_two_flip,                      //
                                 &m_user_defined};

        m_move_ptrs.clear();

        m_number_of_updated_moves = 0;
    }

    /*************************************************************************/
    inline void setup(model::Model<T_Variable, T_Expression> *a_model_ptr,
                      const option::Option                   &a_OPTION) {
        auto &variable_type   = a_model_ptr->reference().variable_type;
        auto &constraint_type = a_model_ptr->reference().constraint_type;

        m_binary.setup(variable_type.binary_variable_ptrs);
        m_integer.setup(variable_type.integer_variable_ptrs);
        m_selection.setup(variable_type.selection_variable_ptrs);

        if (a_OPTION.neighborhood.is_enabled_exclusive_or_move) {
            m_exclusive_or.setup(constraint_type.exclusive_or_ptrs);
        }

        if (a_OPTION.neighborhood.is_enabled_exclusive_nor_move) {
            m_exclusive_nor.setup(constraint_type.exclusive_nor_ptrs);
        }

        if (a_OPTION.neighborhood.is_enabled_inverted_integers_move) {
            m_inverted_integers.setup(constraint_type.inverted_integers_ptrs);
        }

        if (a_OPTION.neighborhood.is_enabled_balanced_integers_move) {
            m_balanced_integers.setup(constraint_type.balanced_integers_ptrs);
        }

        if (a_OPTION.neighborhood.is_enabled_constant_sum_integers_move) {
            m_constant_sum_integers.setup(
                constraint_type.constant_sum_integers_ptrs);
        }

        if (a_OPTION.neighborhood
                .is_enabled_constant_difference_integers_move) {
            m_constant_difference_integers.setup(
                constraint_type.constant_difference_integers_ptrs);
        }

        if (a_OPTION.neighborhood.is_enabled_constant_ratio_integers_move) {
            m_constant_ratio_integers.setup(
                constraint_type.constant_ratio_integers_ptrs);
        }

        if (a_OPTION.neighborhood.is_enabled_aggregation_move) {
            m_aggregation.setup(constraint_type.aggregation_ptrs);
        }

        if (a_OPTION.neighborhood.is_enabled_precedence_move) {
            m_precedence.setup(constraint_type.precedence_ptrs);
        }

        if (a_OPTION.neighborhood.is_enabled_variable_bound_move) {
            m_variable_bound.setup(constraint_type.variable_bound_ptrs);
        }

        if (a_OPTION.neighborhood.is_enabled_trinomial_exclusive_nor_move) {
            m_trinomial_exclusive_nor.setup(
                constraint_type.trinomial_exclusive_nor_ptrs);
        }

        if (a_OPTION.neighborhood.is_enabled_soft_selection_move) {
            m_soft_selection.setup(constraint_type.soft_selection_ptrs);
        }

        if (a_OPTION.neighborhood.is_enabled_chain_move) {
            m_chain.setup();
        }

        if (a_OPTION.neighborhood.is_enabled_two_flip_move &&
            a_model_ptr->flippable_variable_ptr_pairs().size() > 0) {
            m_two_flip.setup(a_model_ptr->flippable_variable_ptr_pairs());
        }

        if (a_OPTION.neighborhood.is_enabled_user_defined_move) {
            m_user_defined.setup();
        }
    }

    /*************************************************************************/
    inline void update_moves(const bool a_ACCEPT_ALL,                     //
                             const bool a_ACCEPT_OBJECTIVE_IMPROVABLE,    //
                             const bool a_ACCEPT_FEASIBILITY_IMPROVABLE,  //
                             const bool a_IS_ENABLED_PARALLEL,            //
                             const int  a_NUMBER_OF_THREADS) {
        auto number_of_candidate_moves = 0;
        for (auto &&move_generator_ptr : m_move_generator_ptrs) {
            if (move_generator_ptr->is_enabled()) {
                move_generator_ptr->update_moves(
                    a_ACCEPT_ALL,                     //
                    a_ACCEPT_OBJECTIVE_IMPROVABLE,    //
                    a_ACCEPT_FEASIBILITY_IMPROVABLE,  //
                    a_IS_ENABLED_PARALLEL,            //
                    a_NUMBER_OF_THREADS);
                auto &flags = move_generator_ptr->flags();
                number_of_candidate_moves +=
                    std::accumulate(flags.begin(), flags.end(), 0);
            }
        }
        auto &move_ptrs = m_move_ptrs;
        move_ptrs.resize(number_of_candidate_moves);
        auto index                = 0;
        m_number_of_updated_moves = 0;
        for (const auto &move_generator_ptr : m_move_generator_ptrs) {
            if (move_generator_ptr->is_enabled()) {
                auto     &moves     = move_generator_ptr->moves();
                auto     &flags     = move_generator_ptr->flags();
                const int MOVE_SIZE = move_generator_ptr->moves().size();

                for (auto i = 0; i < MOVE_SIZE; i++) {
                    if (flags[i]) {
                        move_ptrs[index++] = &moves[i];
                    }
                }
                m_number_of_updated_moves += moves.size();
            }
        }
    }

    /*************************************************************************/
    inline std::vector<Move<T_Variable, T_Expression> *> &move_ptrs(
        void) noexcept {
        return m_move_ptrs;
    }

    /*************************************************************************/
    inline const std::vector<Move<T_Variable, T_Expression> *> &move_ptrs(
        void) const noexcept {
        return m_move_ptrs;
    }

    /*************************************************************************/
    inline void shuffle_moves(std::mt19937 *a_rand) noexcept {
        std::shuffle(m_move_ptrs.begin(), m_move_ptrs.end(), *a_rand);
    }

    /*************************************************************************/
    inline void reset_special_neighborhood_moves_availability(void) noexcept {
        m_exclusive_or.reset_availability();
        m_exclusive_nor.reset_availability();
        m_inverted_integers.reset_availability();
        m_balanced_integers.reset_availability();
        m_constant_sum_integers.reset_availability();
        m_constant_difference_integers.reset_availability();
        m_constant_ratio_integers.reset_availability();
        m_aggregation.reset_availability();
        m_precedence.reset_availability();
        m_variable_bound.reset_availability();
        m_soft_selection.reset_availability();
        m_trinomial_exclusive_nor.reset_availability();
        m_chain.reset_availability();
        m_two_flip.reset_availability();
    }

    /*************************************************************************/
    inline bool is_enabled_special_neighborhood_move(void) const {
        if (m_precedence.is_enabled()) {
            return true;
        }

        if (m_exclusive_or.is_enabled()) {
            return true;
        }

        if (m_exclusive_nor.is_enabled()) {
            return true;
        }

        if (m_inverted_integers.is_enabled()) {
            return true;
        }

        if (m_balanced_integers.is_enabled()) {
            return true;
        }

        if (m_constant_sum_integers.is_enabled()) {
            return true;
        }

        if (m_constant_difference_integers.is_enabled()) {
            return true;
        }

        if (m_constant_ratio_integers.is_enabled()) {
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

        if (m_trinomial_exclusive_nor.is_enabled()) {
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
    inline BinaryMoveGenerator      //
        <T_Variable, T_Expression>  //
            &binary(void) noexcept {
        return m_binary;
    }

    /*************************************************************************/
    inline const BinaryMoveGenerator  //
        <T_Variable, T_Expression>    //
            &binary(void) const noexcept {
        return m_binary;
    }

    /*************************************************************************/
    inline IntegerMoveGenerator     //
        <T_Variable, T_Expression>  //
            &integer(void) noexcept {
        return m_integer;
    }

    /*************************************************************************/
    inline const IntegerMoveGenerator  //
        <T_Variable, T_Expression>     //
            &integer(void) const noexcept {
        return m_integer;
    }

    /*************************************************************************/
    inline SelectionMoveGenerator   //
        <T_Variable, T_Expression>  //
            &selection(void) noexcept {
        return m_selection;
    }

    /*************************************************************************/
    inline const SelectionMoveGenerator  //
        <T_Variable, T_Expression>       //
            &selection(void) const noexcept {
        return m_selection;
    }

    /*************************************************************************/
    inline ExclusiveOrMoveGenerator  //
        <T_Variable, T_Expression>   //
            &exclusive_or(void) noexcept {
        return m_exclusive_or;
    }

    /*************************************************************************/
    inline const ExclusiveOrMoveGenerator  //
        <T_Variable, T_Expression>         //
            &exclusive_or(void) const noexcept {
        return m_exclusive_or;
    }

    /*************************************************************************/
    inline ExclusiveNorMoveGenerator  //
        <T_Variable, T_Expression>    //
            &exclusive_nor(void) noexcept {
        return m_exclusive_nor;
    }

    /*************************************************************************/
    inline const ExclusiveNorMoveGenerator  //
        <T_Variable, T_Expression>          //
            &exclusive_nor(void) const noexcept {
        return m_exclusive_nor;
    }

    /*************************************************************************/
    inline InvertedIntegersMoveGenerator  //
        <T_Variable, T_Expression>        //
            &inverted_integers(void) noexcept {
        return m_inverted_integers;
    }

    /*************************************************************************/
    inline const InvertedIntegersMoveGenerator  //
        <T_Variable, T_Expression>              //
            &inverted_integers(void) const noexcept {
        return m_inverted_integers;
    }

    /*************************************************************************/
    inline BalancedIntegersMoveGenerator  //
        <T_Variable, T_Expression>        //
            &balanced_integers(void) noexcept {
        return m_balanced_integers;
    }

    /*************************************************************************/
    inline const BalancedIntegersMoveGenerator  //
        <T_Variable, T_Expression>              //
            &balanced_integers(void) const noexcept {
        return m_balanced_integers;
    }

    /*************************************************************************/
    inline ConstantSumIntegersMoveGenerator  //
        <T_Variable, T_Expression>           //
            &constant_sum_integers(void) noexcept {
        return m_constant_sum_integers;
    }

    /*************************************************************************/
    inline const ConstantSumIntegersMoveGenerator  //
        <T_Variable, T_Expression>                 //
            &constant_sum_integers(void) const noexcept {
        return m_constant_sum_integers;
    }

    /*************************************************************************/
    inline ConstantDifferenceIntegersMoveGenerator  //
        <T_Variable, T_Expression>                  //
            &constant_difference_integers(void) noexcept {
        return m_constant_difference_integers;
    }

    /*************************************************************************/
    inline const ConstantDifferenceIntegersMoveGenerator  //
        <T_Variable, T_Expression>                        //
            &constant_difference_integers(void) const noexcept {
        return m_constant_difference_integers;
    }

    /*************************************************************************/
    inline ConstantRatioIntegersMoveGenerator  //
        <T_Variable, T_Expression>             //
            &constant_ratio_integers(void) noexcept {
        return m_constant_ratio_integers;
    }

    /*************************************************************************/
    inline const ConstantRatioIntegersMoveGenerator  //
        <T_Variable, T_Expression>                   //
            &constant_ratio_integers(void) const noexcept {
        return m_constant_ratio_integers;
    }

    /*************************************************************************/
    inline AggregationMoveGenerator  //
        <T_Variable, T_Expression>   //
            &aggregation(void) noexcept {
        return m_aggregation;
    }

    /*************************************************************************/
    inline const AggregationMoveGenerator  //
        <T_Variable, T_Expression>         //
            &aggregation(void) const noexcept {
        return m_aggregation;
    }

    /*************************************************************************/
    inline PrecedenceMoveGenerator  //
        <T_Variable, T_Expression>  //
            &precedence(void) noexcept {
        return m_precedence;
    }

    /*************************************************************************/
    inline const PrecedenceMoveGenerator  //
        <T_Variable, T_Expression>        //
            &precedence(void) const noexcept {
        return m_precedence;
    }

    /*************************************************************************/
    inline VariableBoundMoveGenerator  //
        <T_Variable, T_Expression>     //
            &variable_bound(void) noexcept {
        return m_variable_bound;
    }

    /*************************************************************************/
    inline const VariableBoundMoveGenerator  //
        <T_Variable, T_Expression> &
        variable_bound(void) const noexcept {
        return m_variable_bound;
    }

    /*************************************************************************/
    inline SoftSelectionMoveGenerator  //
        <T_Variable, T_Expression>     //
            &soft_selection(void) noexcept {
        return m_soft_selection;
    }

    /*************************************************************************/
    inline const SoftSelectionMoveGenerator  //
        <T_Variable, T_Expression>           //
            &soft_selection(void) const noexcept {
        return m_soft_selection;
    }

    /*************************************************************************/
    inline TrinomialExclusiveNorMoveGenerator  //
        <T_Variable, T_Expression>             //
            &trinomial_exclusive_nor(void) noexcept {
        return m_trinomial_exclusive_nor;
    }

    /*************************************************************************/
    inline const TrinomialExclusiveNorMoveGenerator  //
        <T_Variable, T_Expression>                   //
            &trinomial_exclusive_nor(void) const noexcept {
        return m_trinomial_exclusive_nor;
    }

    /*************************************************************************/
    inline ChainMoveGenerator       //
        <T_Variable, T_Expression>  //
            &chain(void) noexcept {
        return m_chain;
    }

    /*************************************************************************/
    inline const ChainMoveGenerator  //
        <T_Variable, T_Expression>   //
            &chain(void) const noexcept {
        return m_chain;
    }

    /*************************************************************************/
    inline TwoFlipMoveGenerator     //
        <T_Variable, T_Expression>  //
            &two_flip(void) noexcept {
        return m_two_flip;
    }

    /*************************************************************************/
    inline const TwoFlipMoveGenerator  //
        <T_Variable, T_Expression>     //
            &two_flip(void) const noexcept {
        return m_two_flip;
    }

    /*************************************************************************/
    inline UserDefinedMoveGenerator  //
        <T_Variable, T_Expression>   //
            &user_defined(void) noexcept {
        return m_user_defined;
    }

    /*************************************************************************/
    inline const UserDefinedMoveGenerator  //
        <T_Variable, T_Expression> &
        user_defined(void) const noexcept {
        return m_user_defined;
    }

    /*************************************************************************/
    inline int number_of_special_neighborhood_moves(void) const {
        return m_exclusive_or.moves().size()                    //
               + m_exclusive_nor.moves().size()                 //
               + m_inverted_integers.moves().size()             //
               + m_balanced_integers.moves().size()             //
               + m_constant_sum_integers.moves().size()         //
               + m_constant_difference_integers.moves().size()  //
               + m_constant_ratio_integers.moves().size()       //
               + m_aggregation.moves().size()                   //
               + m_precedence.moves().size()                    //
               + m_variable_bound.moves().size()                //
               + m_soft_selection.moves().size()                //
               + m_trinomial_exclusive_nor.moves().size()       //
               + m_chain.moves().size()                         //
               + m_two_flip.moves().size();
    }

    /*************************************************************************/
    inline int number_of_updated_moves(void) const noexcept {
        return m_number_of_updated_moves;
    }
};
}  // namespace printemps::neighborhood
#endif
/*****************************************************************************/
// END
/*****************************************************************************/