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
    EXPECT_EQ(  //
        ParallelOptionConstant::
            DEFAULT_IS_ENABLED_MOVE_UPDATE_PARALLELIZATION,  //
        option.is_enabled_move_update_parallelization);

    EXPECT_EQ(  //
        ParallelOptionConstant::
            DEFAULT_IS_ENABLED_MOVE_EVALUATION_PARALLELIZATION,  //
        option.is_enabled_move_evaluation_parallelization);

    EXPECT_EQ(                                                          //
        ParallelOptionConstant::DEFAULT_NUMBER_OF_THREADS_MOVE_UPDATE,  //
        option.number_of_threads_move_update);

    EXPECT_EQ(                                                              //
        ParallelOptionConstant::DEFAULT_NUMBER_OF_THREADS_MOVE_EVALUATION,  //
        option.number_of_threads_move_evaluation);

    EXPECT_EQ(  //
        ParallelOptionConstant::
            DEFAULT_IS_ENABLED_THREAD_COUNT_OPTIMIZATION,  //
        option.is_enabled_thread_count_optimization);

    EXPECT_EQ(  //
        ParallelOptionConstant::
            DEFAULT_THREAD_COUNT_OPTIMIZATION_DECAY_FACTOR,  //
        option.thread_count_optimization_decay_factor);
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/