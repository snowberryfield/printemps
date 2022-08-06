/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <random>

#include <printemps.h>

namespace {
/*****************************************************************************/
class TestParallelOption : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestParallelOption, initialize) {
    using namespace printemps::option;
    ParallelOption option;
    EXPECT_EQ(                                                           //
        ParallelOptionConstant::DEFAULT_IS_ENABLED_PARALLEL_EVALUATION,  //
        option.is_enabled_parallel_evaluation);

    EXPECT_EQ(  //
        ParallelOptionConstant::
            DEFAULT_IS_ENABLED_PARALLEL_NEIGHBORHOOD_UPDATE,  //
        option.is_enabled_parallel_neighborhood_update);
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/