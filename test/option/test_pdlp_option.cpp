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
class TestPDLPOption : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestPDLPOption, initialize) {
    using namespace printemps::option;
    PDLPOption option;
    EXPECT_EQ(                                   //
        PDLPOptionConstant::DEFAULT_IS_ENABLED,  //
        option.is_enabled);

    EXPECT_EQ(                                      //
        PDLPOptionConstant::DEFAULT_ITERATION_MAX,  //
        option.iteration_max);

    EXPECT_FLOAT_EQ(                           //
        PDLPOptionConstant::DEFAULT_TIME_MAX,  //
        option.time_max);

    EXPECT_FLOAT_EQ(                              //
        PDLPOptionConstant::DEFAULT_TIME_OFFSET,  //
        option.time_offset);

    EXPECT_FLOAT_EQ(                            //
        PDLPOptionConstant::DEFAULT_TOLERANCE,  //
        option.tolerance);

    EXPECT_FLOAT_EQ(                                            //
        PDLPOptionConstant::DEFAULT_STEP_SIZE_EXTEND_EXPONENT,  //
        option.step_size_extend_exponent);

    EXPECT_FLOAT_EQ(                                            //
        PDLPOptionConstant::DEFAULT_STEP_SIZE_REDUCE_EXPONENT,  //
        option.step_size_reduce_exponent);

    EXPECT_FLOAT_EQ(                                               //
        PDLPOptionConstant::DEFAULT_RESTART_THRESHOLD_SUFFICIENT,  //
        option.restart_threshold_sufficient);

    EXPECT_FLOAT_EQ(                                              //
        PDLPOptionConstant::DEFAULT_RESTART_THRESHOLD_NECESSARY,  //
        option.restart_threshold_necessary);

    EXPECT_FLOAT_EQ(                                               //
        PDLPOptionConstant::DEFAULT_RESTART_THRESHOLD_ARTIFICIAL,  //
        option.restart_threshold_artificial);

    EXPECT_EQ(                                               //
        PDLPOptionConstant::DEFAULT_RESTART_CHECK_INTERVAL,  //
        option.restart_check_interval);

    EXPECT_EQ(                                                   //
        PDLPOptionConstant::DEFAULT_CONVERGENCE_CHECK_INTERVAL,  //
        option.convergence_check_interval);

    EXPECT_EQ(                                               //
        PDLPOptionConstant::DEFAULT_COUNTS_OF_RUIZ_SCALING,  //
        option.counts_of_ruiz_scaling);

    EXPECT_EQ(                                                          //
        PDLPOptionConstant::DEFAULT_IS_ENABLED_POCK_CHAMBOLLE_SCALING,  //
        option.is_enabled_pock_chambolle_scaling);

    EXPECT_EQ(                                     //
        PDLPOptionConstant::DEFAULT_LOG_INTERVAL,  //
        option.log_interval);
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/