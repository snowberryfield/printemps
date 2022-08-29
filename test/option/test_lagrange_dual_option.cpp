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
class TestLagrangeDualOption : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestLagrangeDualOption, initialize) {
    using namespace printemps::option;
    LagrangeDualOption option;
    EXPECT_EQ(                                           //
        LagrangeDualOptionConstant::DEFAULT_IS_ENABLED,  //
        option.is_enabled);

    EXPECT_EQ(                                              //
        LagrangeDualOptionConstant::DEFAULT_ITERATION_MAX,  //
        option.iteration_max);

    EXPECT_FLOAT_EQ(                                   //
        LagrangeDualOptionConstant::DEFAULT_TIME_MAX,  //
        option.time_max);

    EXPECT_FLOAT_EQ(                                      //
        LagrangeDualOptionConstant::DEFAULT_TIME_OFFSET,  //
        option.time_offset);

    EXPECT_FLOAT_EQ(  //
        LagrangeDualOptionConstant::DEFAULT_STEP_SIZE_EXTEND_RATE,
        option.step_size_extend_rate);

    EXPECT_FLOAT_EQ(  //
        LagrangeDualOptionConstant::DEFAULT_STEP_SIZE_REDUCE_RATE,
        option.step_size_reduce_rate);

    EXPECT_FLOAT_EQ(                                    //
        LagrangeDualOptionConstant::DEFAULT_TOLERANCE,  //
        option.tolerance);

    EXPECT_FLOAT_EQ(                                     //
        LagrangeDualOptionConstant::DEFAULT_QUEUE_SIZE,  //
        option.queue_size);

    EXPECT_EQ(                                             //
        LagrangeDualOptionConstant::DEFAULT_LOG_INTERVAL,  //
        option.log_interval);
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/