/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
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

    EXPECT_EQ(                                                                //
        PenaltyOptionConstant::DEFAULT_IS_ENABLED_GROUP_PENALTY_COEFFICIENT,  //
        option.is_enabled_group_penalty_coefficient);
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/