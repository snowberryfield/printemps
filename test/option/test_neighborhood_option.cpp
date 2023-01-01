/*****************************************************************************/
// Copyright (c) 2020-2023 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
using namespace printemps;
/*****************************************************************************/
class TestNeighborhoodOption : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestNeighborhoodOption, initialize) {
    using namespace printemps::option;
    NeighborhoodOption option;

    EXPECT_EQ(                                                       //
        NeighborhoodOptionConstant::DEFAULT_IS_ENABLED_BINARY_MOVE,  //
        option.is_enabled_binary_move);

    EXPECT_EQ(                                                        //
        NeighborhoodOptionConstant::DEFAULT_IS_ENABLED_INTEGER_MOVE,  //
        option.is_enabled_integer_move);

    EXPECT_EQ(                                                             //
        NeighborhoodOptionConstant::DEFAULT_IS_ENABLED_EXCLUSIVE_OR_MOVE,  //
        option.is_enabled_exclusive_or_move);

    EXPECT_EQ(                                                              //
        NeighborhoodOptionConstant::DEFAULT_IS_ENABLED_EXCLUSIVE_NOR_MOVE,  //
        option.is_enabled_exclusive_nor_move);

    EXPECT_EQ(  //
        NeighborhoodOptionConstant::
            DEFAULT_IS_ENABLED_INVERTED_INTEGERS_MOVE,  //
        option.is_enabled_inverted_integers_move);

    EXPECT_EQ(  //
        NeighborhoodOptionConstant::
            DEFAULT_IS_ENABLED_BALANCED_INTEGERS_MOVE,  //
        option.is_enabled_balanced_integers_move);

    EXPECT_EQ(  //
        NeighborhoodOptionConstant::
            DEFAULT_IS_ENABLED_CONSTANT_SUM_INTEGERS_MOVE,  //
        option.is_enabled_constant_sum_integers_move);

    EXPECT_EQ(  //
        NeighborhoodOptionConstant::
            DEFAULT_IS_ENABLED_CONSTANT_DIFFERENCE_INTEGERS_MOVE,  //
        option.is_enabled_constant_difference_integers_move);

    EXPECT_EQ(  //
        NeighborhoodOptionConstant::
            DEFAULT_IS_ENABLED_CONSTANT_RATIO_INTEGERS_MOVE,  //
        option.is_enabled_constant_ratio_integers_move);

    EXPECT_EQ(                                                            //
        NeighborhoodOptionConstant::DEFAULT_IS_ENABLED_AGGREGATION_MOVE,  //
        option.is_enabled_aggregation_move);

    EXPECT_EQ(                                                           //
        NeighborhoodOptionConstant::DEFAULT_IS_ENABLED_PRECEDENCE_MOVE,  //
        option.is_enabled_precedence_move);

    EXPECT_EQ(                                                               //
        NeighborhoodOptionConstant::DEFAULT_IS_ENABLED_VARIABLE_BOUND_MOVE,  //
        option.is_enabled_variable_bound_move);

    EXPECT_EQ(  //
        NeighborhoodOptionConstant::
            DEFAULT_IS_ENABLED_TRINOMIAL_EXCLUSIVE_NOR_MOVE,  //
        option.is_enabled_trinomial_exclusive_nor_move);

    EXPECT_EQ(                                                               //
        NeighborhoodOptionConstant::DEFAULT_IS_ENABLED_SOFT_SELECTION_MOVE,  //
        option.is_enabled_soft_selection_move);

    EXPECT_EQ(                                                      //
        NeighborhoodOptionConstant::DEFAULT_IS_ENABLED_CHAIN_MOVE,  //
        option.is_enabled_chain_move);

    EXPECT_EQ(                                                         //
        NeighborhoodOptionConstant::DEFAULT_IS_ENABLED_TWO_FLIP_MOVE,  //
        option.is_enabled_two_flip_move);

    EXPECT_EQ(                                                    //
        NeighborhoodOptionConstant::DEFAULT_CHAIN_MOVE_CAPACITY,  //
        option.chain_move_capacity);

    EXPECT_EQ(                                                       //
        NeighborhoodOptionConstant::DEFAULT_CHAIN_MOVE_REDUCE_MODE,  //
        option.chain_move_reduce_mode);

    EXPECT_FLOAT_EQ(  //
        NeighborhoodOptionConstant::
            DEFAULT_CHAIN_MOVE_OVERLAP_RATE_THRESHOLD,  //
        option.chain_move_overlap_rate_threshold);

    EXPECT_EQ(                                               //
        NeighborhoodOptionConstant::DEFAULT_SELECTION_MODE,  //
        option.selection_mode);

    EXPECT_EQ(                                                             //
        NeighborhoodOptionConstant::DEFAULT_IMPROVABILITY_SCREENING_MODE,  //
        option.improvability_screening_mode);
}
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/