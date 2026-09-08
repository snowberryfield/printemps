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
class TestCDCLOption : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestCDCLOption, initialize) {
    using namespace printemps::option;
    CDCLOption option;
    EXPECT_EQ(                                   //
        CDCLOptionConstant::DEFAULT_IS_ENABLED,  //
        option.is_enabled);

    EXPECT_EQ(                                      //
        CDCLOptionConstant::DEFAULT_ITERATION_MAX,  //
        option.iteration_max);

    EXPECT_FLOAT_EQ(                           //
        CDCLOptionConstant::DEFAULT_TIME_MAX,  //
        option.time_max);

    EXPECT_FLOAT_EQ(                              //
        CDCLOptionConstant::DEFAULT_TIME_OFFSET,  //
        option.time_offset);

    EXPECT_EQ(                                     //
        CDCLOptionConstant::DEFAULT_LOG_INTERVAL,  //
        option.log_interval);

    EXPECT_EQ(                                     //
        CDCLOptionConstant::DEFAULT_CONFLICT_MAX,  //
        option.conflict_max);

    EXPECT_EQ(                             //
        CDCLOptionConstant::DEFAULT_SEED,  //
        option.seed);

    EXPECT_EQ(                                           //
        CDCLOptionConstant::DEFAULT_IS_ENABLED_RESTART,  //
        option.is_enabled_restart);

    EXPECT_EQ(                                     //
        CDCLOptionConstant::DEFAULT_RESTART_MODE,  //
        option.restart_mode);

    EXPECT_EQ(                                               //
        CDCLOptionConstant::DEFAULT_INITIAL_CONFLICT_LIMIT,  //
        option.initial_conflict_limit);

    EXPECT_FLOAT_EQ(                                                    //
        CDCLOptionConstant::DEFAULT_RESTART_EXPONENTIAL_INCREASE_RATE,  //
        option.restart_exponential_increase_rate);

    EXPECT_EQ(                                                //
        CDCLOptionConstant::DEFAULT_RESTART_ADAPTIVE_WINDOW,  //
        option.restart_adaptive_window);

    EXPECT_FLOAT_EQ(  //
        CDCLOptionConstant::
            DEFAULT_RESTART_ADAPTIVE_LITERAL_BLOCK_DISTANCE_RATIO_THRESHOLD,  //
        option.restart_adaptive_literal_block_distance_ratio_threshold);

    EXPECT_EQ(                                                                //
        CDCLOptionConstant::DEFAULT_IS_ENABLED_LEARNED_CONSTRAINT_REDUCTION,  //
        option.is_enabled_learned_constraint_reduction);

    EXPECT_EQ(                                                    //
        CDCLOptionConstant::DEFAULT_LEARNED_CONSTRAINT_CAPACITY,  //
        option.learned_constraint_capacity);

    EXPECT_FLOAT_EQ(                                                    //
        CDCLOptionConstant::DEFAULT_LEARNED_CONSTRAINT_REDUCTION_RATE,  //
        option.learned_constraint_reduction_rate);

    EXPECT_EQ(                                                //
        CDCLOptionConstant::                                  //
        DEFAULT_LITERAL_BLOCK_DISTANCE_PROTECTION_THRESHOLD,  //
        option.literal_block_distance_protection_threshold);

    EXPECT_FLOAT_EQ(                                               //
        CDCLOptionConstant::DEFAULT_VARIABLE_ACTIVITY_DECAY_RATE,  //
        option.variable_activity_decay_rate);

    EXPECT_EQ(                                                //
        CDCLOptionConstant::DEFAULT_IS_ENABLED_PHASE_SAVING,  //
        option.is_enabled_phase_saving);
}
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/