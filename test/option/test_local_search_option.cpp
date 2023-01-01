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
class TestLocalSearchOption : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestLocalSearchOption, initialize) {
    using namespace printemps::option;
    LocalSearchOption option;
    EXPECT_EQ(                                          //
        LocalSearchOptionConstant::DEFAULT_IS_ENABLED,  //
        option.is_enabled);

    EXPECT_EQ(                                             //
        LocalSearchOptionConstant::DEFAULT_ITERATION_MAX,  //
        option.iteration_max);

    EXPECT_FLOAT_EQ(                                  //
        LocalSearchOptionConstant::DEFAULT_TIME_MAX,  //
        option.time_max);

    EXPECT_FLOAT_EQ(                                     //
        LocalSearchOptionConstant::DEFAULT_TIME_OFFSET,  //
        option.time_offset);

    EXPECT_EQ(                                            //
        LocalSearchOptionConstant::DEFAULT_LOG_INTERVAL,  //
        option.log_interval);
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/