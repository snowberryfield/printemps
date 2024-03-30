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
    EXPECT_EQ(                                                           //
        ParallelOptionConstant::DEFAULT_IS_ENABLED_PARALLEL_EVALUATION,  //
        option.is_enabled_parallel_evaluation);

    EXPECT_EQ(  //
        ParallelOptionConstant::
            DEFAULT_IS_ENABLED_PARALLEL_NEIGHBORHOOD_UPDATE,  //
        option.is_enabled_parallel_neighborhood_update);

    EXPECT_EQ(  //
        ParallelOptionConstant::
            DEFAULT_IS_ENABLED_AUTOMATIC_EVALUATION_PARALLELIZATION,  //
        option.is_enabled_automatic_evaluation_parallelization);

    EXPECT_EQ(  //
        ParallelOptionConstant::
            DEFAULT_IS_ENABLED_AUTOMATIC_NEIGHBORHOOD_UPDATE_PARALLELIZATION,  //
        option.is_enabled_automatic_neighborhood_update_parallelization);

    EXPECT_EQ(  //
        ParallelOptionConstant::
            DEFAULT_EVALUATION_PARALLELIZATION_DECAY_FACTOR,  //
        option.evaluation_parallelization_decay_factor);

    EXPECT_EQ(  //
        ParallelOptionConstant::
            DEFAULT_NEIGHBORHOOD_UPDATE_PARALLELIZATION_DECAY_FACTOR,  //
        option.neighborhood_update_parallelization_decay_factor);

    EXPECT_EQ(                                                         //
        ParallelOptionConstant::DEFAULT_NUMBER_OF_THREADS_EVALUATION,  //
        option.number_of_threads_evaluation);

    EXPECT_EQ(  //
        ParallelOptionConstant::
            DEFAULT_NUMBER_OF_THREADS_NEIGHBORHOOD_UPDATE,  //
        option.number_of_threads_neighborhood_update);
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/