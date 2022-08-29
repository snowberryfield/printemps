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
class TestTabuSearchOption : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestTabuSearchOption, initialize) {
    using namespace printemps::option;
    TabuSearchOption option;
    EXPECT_EQ(                                            //
        TabuSearchOptionConstant::DEFAULT_ITERATION_MAX,  //
        option.iteration_max);

    EXPECT_FLOAT_EQ(                                 //
        TabuSearchOptionConstant::DEFAULT_TIME_MAX,  //
        option.time_max);

    EXPECT_FLOAT_EQ(                                    //
        TabuSearchOptionConstant::DEFAULT_TIME_OFFSET,  //
        option.time_offset);

    EXPECT_EQ(                                           //
        TabuSearchOptionConstant::DEFAULT_LOG_INTERVAL,  //
        option.log_interval);

    EXPECT_EQ(                                                  //
        TabuSearchOptionConstant::DEFAULT_INITIAL_TABU_TENURE,  //
        option.initial_tabu_tenure);

    EXPECT_FLOAT_EQ(                                                   //
        TabuSearchOptionConstant::DEFAULT_TABU_TENURE_RANDOMIZE_RATE,  //
        option.tabu_tenure_randomize_rate);

    EXPECT_FLOAT_EQ(                                                        //
        TabuSearchOptionConstant::DEFAULT_INITIAL_MODIFICATION_FIXED_RATE,  //
        option.initial_modification_fixed_rate);

    EXPECT_FLOAT_EQ(  //
        TabuSearchOptionConstant::
            DEFAULT_INITIAL_MODIFICATION_RANDOMIZE_RATE,  //
        option.initial_modification_randomize_rate);

    EXPECT_EQ(                                        //
        TabuSearchOptionConstant::DEFAULT_TABU_MODE,  //
        option.tabu_mode);

    EXPECT_FLOAT_EQ(                                           //
        TabuSearchOptionConstant::DEFAULT_MOVE_PRESERVE_RATE,  //
        option.move_preserve_rate);

    EXPECT_FLOAT_EQ(                                                      //
        TabuSearchOptionConstant::DEFAULT_FREQUENCY_PENALTY_COEFFICIENT,  //
        option.frequency_penalty_coefficient);

    EXPECT_FLOAT_EQ(                                                       //
        TabuSearchOptionConstant::DEFAULT_LAGRANGIAN_PENALTY_COEFFICIENT,  //
        option.lagrangian_penalty_coefficient);

    EXPECT_FLOAT_EQ(                                               //
        TabuSearchOptionConstant::DEFAULT_PRUNING_RATE_THRESHOLD,  //
        option.pruning_rate_threshold);

    EXPECT_EQ(                                                 //
        TabuSearchOptionConstant::DEFAULT_IS_ENABLED_SHUFFLE,  //
        option.is_enabled_shuffle);

    EXPECT_EQ(                                                      //
        TabuSearchOptionConstant::DEFAULT_IS_ENABLED_MOVE_CURTAIL,  //
        option.is_enabled_move_curtail);

    EXPECT_EQ(                                                         //
        TabuSearchOptionConstant::DEFAULT_IS_ENABLED_AUTOMATIC_BREAK,  //
        option.is_enabled_automatic_break);

    EXPECT_EQ(  //
        TabuSearchOptionConstant::
            DEFAULT_IS_ENABLED_AUTOMATIC_TABU_TENURE_ADJUSTMENT,  //
        option.is_enabled_automatic_tabu_tenure_adjustment);

    EXPECT_EQ(  //
        TabuSearchOptionConstant::
            DEFAULT_IS_ENABLED_AUTOMATIC_ITERATION_ADJUSTMENT,  //
        option.is_enabled_automatic_iteration_adjustment);

    EXPECT_EQ(                                                              //
        TabuSearchOptionConstant::DEFAULT_IS_ENABLED_INITIAL_MODIFICATION,  //
        option.is_enabled_initial_modification);

    EXPECT_EQ(  //
        TabuSearchOptionConstant::
            DEFAULT_INTENSITY_INCREASE_COUNT_THRESHOLD,  //
        option.intensity_increase_count_threshold);

    EXPECT_EQ(  //
        TabuSearchOptionConstant::
            DEFAULT_INTENSITY_DECREASE_COUNT_THRESHOLD,  //
        option.intensity_decrease_count_threshold);

    EXPECT_FLOAT_EQ(                                                //
        TabuSearchOptionConstant::DEFAULT_ITERATION_INCREASE_RATE,  //
        option.iteration_increase_rate);

    EXPECT_FLOAT_EQ(                                                //
        TabuSearchOptionConstant::DEFAULT_ITERATION_DECREASE_RATE,  //
        option.iteration_decrease_rate);

    EXPECT_EQ(                                                              //
        TabuSearchOptionConstant::DEFAULT_IGNORE_TABU_IF_GLOBAL_INCUMBENT,  //
        option.ignore_tabu_if_global_incumbent);

    EXPECT_EQ(                                                             //
        TabuSearchOptionConstant::DEFAULT_NUMBER_OF_INITIAL_MODIFICATION,  //
        option.number_of_initial_modification);

    EXPECT_EQ(                                   //
        TabuSearchOptionConstant::DEFAULT_SEED,  //
        option.seed);
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/