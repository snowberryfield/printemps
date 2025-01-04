/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
using namespace printemps;
/*****************************************************************************/
class TestGeneralOption : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestGeneralOption, initialize) {
    using namespace printemps::option;
    GeneralOption option;
    EXPECT_EQ(                                         //
        GeneralOptionConstant::DEFAULT_ITERATION_MAX,  //
        option.iteration_max);

    EXPECT_FLOAT_EQ(                              //
        GeneralOptionConstant::DEFAULT_TIME_MAX,  //
        option.time_max);

    EXPECT_FLOAT_EQ(                                 //
        GeneralOptionConstant::DEFAULT_TIME_OFFSET,  //
        option.time_offset);

    EXPECT_FLOAT_EQ(                                            //
        GeneralOptionConstant::DEFAULT_TARGET_OBJECTIVE_VALUE,  //
        option.target_objective_value);

    EXPECT_EQ(                                //
        GeneralOptionConstant::DEFAULT_SEED,  //
        option.seed);

    EXPECT_EQ(                                                      //
        GeneralOptionConstant::DEFAULT_IS_ENABLED_FAST_EVALUATION,  //
        option.is_enabled_fast_evaluation);
}
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/