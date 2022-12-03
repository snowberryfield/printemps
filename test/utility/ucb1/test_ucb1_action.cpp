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
class TestUCB1Action : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestUCB1Action, constructor_without_arg) {
    utility::ucb1::Action<int> action;

    EXPECT_EQ(0, action.number_of_samples);
    EXPECT_FLOAT_EQ(0.0, action.number_of_effective_samples);
    EXPECT_FLOAT_EQ(0.0, action.total_score);
    EXPECT_FLOAT_EQ(0.0, action.mean);
    EXPECT_FLOAT_EQ(0.0, action.variance);
    EXPECT_FLOAT_EQ(0, action.confidence);
}

/*****************************************************************************/
TEST_F(TestUCB1Action, constructor_with_arg) {
    utility::ucb1::Action<int> action(10);

    EXPECT_EQ(10, action.body);
    EXPECT_EQ(0, action.number_of_samples);
    EXPECT_FLOAT_EQ(0.0, action.number_of_effective_samples);
    EXPECT_FLOAT_EQ(0.0, action.total_score);
    EXPECT_FLOAT_EQ(0.0, action.mean);
    EXPECT_FLOAT_EQ(0.0, action.variance);
    EXPECT_FLOAT_EQ(0, action.confidence);
}

/*****************************************************************************/
TEST_F(TestUCB1Action, initialize) {
    utility::ucb1::Action<int> action(10);

    action.number_of_samples           = 1;
    action.number_of_effective_samples = 2.0;
    action.total_score                 = 3.0;
    action.mean                        = 4.0;
    action.variance                    = 5.0;
    action.confidence                  = 6.0;

    action.initialize();

    EXPECT_EQ(10, action.body);
    EXPECT_EQ(0, action.number_of_samples);
    EXPECT_FLOAT_EQ(0.0, action.number_of_effective_samples);
    EXPECT_FLOAT_EQ(0.0, action.total_score);
    EXPECT_FLOAT_EQ(0.0, action.mean);
    EXPECT_FLOAT_EQ(0.0, action.variance);
    EXPECT_FLOAT_EQ(0, action.confidence);
}

/*****************************************************************************/
TEST_F(TestUCB1Action, forget) {
    utility::ucb1::Action<int> action(0);

    action.number_of_effective_samples = 10.0;
    EXPECT_FLOAT_EQ(10.0, action.number_of_effective_samples);

    action.forget(0.5);
    EXPECT_FLOAT_EQ(5.0, action.number_of_effective_samples);

    action.forget(0.5);
    EXPECT_FLOAT_EQ(2.5, action.number_of_effective_samples);
}

/*****************************************************************************/
TEST_F(TestUCB1Action, learn) {
    utility::ucb1::Action<int> action(0);
    action.learn(10.0, 0.5);
    EXPECT_EQ(1, action.number_of_samples);
    EXPECT_FLOAT_EQ(1.0, action.number_of_effective_samples);
    EXPECT_FLOAT_EQ(10.0, action.total_score);
    EXPECT_FLOAT_EQ(10.0, action.mean);
    EXPECT_FLOAT_EQ(0.0, action.variance);

    action.learn(20.0, 0.5);
    EXPECT_EQ(2, action.number_of_samples);
    EXPECT_FLOAT_EQ(1.5, action.number_of_effective_samples);
    EXPECT_FLOAT_EQ(30.0, action.total_score);
    EXPECT_FLOAT_EQ(15.0, action.mean);
    EXPECT_FLOAT_EQ(25.0, action.variance);

    action.learn(30.0, 0.5);
    EXPECT_EQ(3, action.number_of_samples);
    EXPECT_FLOAT_EQ(1.75, action.number_of_effective_samples);
    EXPECT_FLOAT_EQ(60.0, action.total_score);
    EXPECT_FLOAT_EQ(22.5, action.mean);
    EXPECT_FLOAT_EQ(68.75, action.variance);
}

/*****************************************************************************/
TEST_F(TestUCB1Action, update_confidence) {
    /// number_of_samples = 0
    {
        utility::ucb1::Action<int> action(0);

        action.number_of_samples = 0;
        action.confidence        = 10.0;
        action.update_confidence(100.0);
        EXPECT_FLOAT_EQ(10.0, action.confidence);
    }

    /// number_of_samples > 0
    {
        utility::ucb1::Action<int> action(0);
        action.number_of_samples           = 1;
        action.number_of_effective_samples = 100.0;
        action.variance                    = 300.0;
        action.update_confidence(exp(1.0));
        EXPECT_FLOAT_EQ(6.0, action.confidence);
    }
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/