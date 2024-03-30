/*****************************************************************************/
// Copyright (c) 2020-2024 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
using namespace printemps;
/*****************************************************************************/
class TestPenaltyOption : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestPenaltyOption, initialize) {
    using namespace printemps::option;
    PenaltyOption option;

    EXPECT_FLOAT_EQ(                                                       //
        PenaltyOptionConstant::DEFAULT_PENALTY_COEFFICIENT_RELAXING_RATE,  //
        option.penalty_coefficient_relaxing_rate);

    EXPECT_FLOAT_EQ(                                                         //
        PenaltyOptionConstant::DEFAULT_PENALTY_COEFFICIENT_TIGHTENING_RATE,  //
        option.penalty_coefficient_tightening_rate);

    EXPECT_FLOAT_EQ(                                                          //
        PenaltyOptionConstant::DEFAULT_PENALTY_COEFFICIENT_UPDATING_BALANCE,  //
        option.penalty_coefficient_updating_balance);

    EXPECT_FLOAT_EQ(                                                 //
        PenaltyOptionConstant::DEFAULT_INITIAL_PENALTY_COEFFICIENT,  //
        option.initial_penalty_coefficient);

    EXPECT_FLOAT_EQ(  //
        PenaltyOptionConstant::
            DEFAULT_PENALTY_COEFFICIENT_RELAXING_RATE_MIN,  //
        option.penalty_coefficient_relaxing_rate_min);

    EXPECT_FLOAT_EQ(  //
        PenaltyOptionConstant::
            DEFAULT_PENALTY_COEFFICIENT_RELAXING_RATE_MAX,  //
        option.penalty_coefficient_relaxing_rate_max);

    EXPECT_FLOAT_EQ(  //
        PenaltyOptionConstant::
            DEFAULT_PENALTY_COEFFICIENT_RELAXING_RATE_INCREASE_RATE,  //
        option.penalty_coefficient_relaxing_rate_increase_rate);

    EXPECT_FLOAT_EQ(  //
        PenaltyOptionConstant::
            DEFAULT_PENALTY_COEFFICIENT_RELAXING_RATE_DECREASE_RATE,  //
        option.penalty_coefficient_relaxing_rate_decrease_rate);

    EXPECT_FLOAT_EQ(                                                //
        PenaltyOptionConstant::DEFAULT_INNER_STAGNATION_THRESHOLD,  //
        option.inner_stagnation_threshold);

    EXPECT_EQ(                                                      //
        PenaltyOptionConstant::DEFAULT_OUTER_STAGNATION_THRESHOLD,  //
        option.outer_stagnation_threshold);

    EXPECT_FLOAT_EQ(                                                         //
        PenaltyOptionConstant::DEFAULT_IS_ENABLED_INNER_STAGNATION_BREAKER,  //
        option.is_enabled_inner_stagnation_breaker);

    EXPECT_EQ(                                                               //
        PenaltyOptionConstant::DEFAULT_IS_ENABLED_OUTER_STAGNATION_BREAKER,  //
        option.is_enabled_outer_stagnation_breaker);

    EXPECT_EQ(                                                                //
        PenaltyOptionConstant::DEFAULT_IS_ENABLED_GROUP_PENALTY_COEFFICIENT,  //
        option.is_enabled_group_penalty_coefficient);

    EXPECT_EQ(  //
        PenaltyOptionConstant::
            DEFAULT_IS_ENABLED_SHRINK_PENALTY_COEFFICIENT,  //
        option.is_enabled_shrink_penalty_coefficient);
}
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/