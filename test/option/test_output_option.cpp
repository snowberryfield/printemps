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
class TestOutputOption : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestOutputOption, initialize) {
    using namespace printemps::option;
    OutputOption option;

    EXPECT_EQ(                                  //
        OutputOptionConstant::DEFAULT_VERBOSE,  //
        option.verbose);

    EXPECT_EQ(                                                 //
        OutputOptionConstant::DEFAULT_IS_ENABLED_WRITE_TREND,  //
        option.is_enabled_write_trend);

    EXPECT_EQ(                                                              //
        OutputOptionConstant::DEFAULT_IS_ENABLED_STORE_FEASIBLE_SOLUTIONS,  //
        option.is_enabled_store_feasible_solutions);

    EXPECT_EQ(                                                      //
        OutputOptionConstant::DEFAULT_FEASIBLE_SOLUTIONS_CAPACITY,  //
        option.feasible_solutions_capacity);
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/