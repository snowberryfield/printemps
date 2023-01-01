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
class TestUCB1Learner : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestUCB1Learner, constructor_without_arg) {
    utility::ucb1::Learner<int> learner;

    EXPECT_EQ(0, static_cast<int>(learner.actions().size()));
    EXPECT_EQ(nullptr, &(learner.best_action()));
    EXPECT_EQ(0, learner.total_number_of_samples());
    EXPECT_FLOAT_EQ(0.0, learner.decay_factor());
}

/*****************************************************************************/
TEST_F(TestUCB1Learner, constructor_with_arg) {
    std::vector<utility::ucb1::Action<int>> actions(2);
    actions[0].body = 0;
    actions[1].body = 1;
    utility::ucb1::Learner<int> learner(actions, 0.5);

    EXPECT_EQ(2, static_cast<int>(learner.actions().size()));
    EXPECT_EQ(0, learner.actions()[0].body);
    EXPECT_EQ(1, learner.actions()[1].body);
    EXPECT_EQ(0, learner.best_action().body);
    EXPECT_EQ(0, learner.total_number_of_samples());
    EXPECT_FLOAT_EQ(0.5, learner.decay_factor());
}

/*****************************************************************************/
TEST_F(TestUCB1Learner, initialize) {
    std::vector<utility::ucb1::Action<int>> actions(2);
    actions[0].body = 0;
    actions[1].body = 1;
    utility::ucb1::Learner<int> learner(actions, 0.5);

    learner.learn(10.0);
    EXPECT_EQ(1, learner.total_number_of_samples());
    EXPECT_FLOAT_EQ(10.0, learner.actions()[0].total_score);

    learner.initialize();

    EXPECT_EQ(0, static_cast<int>(learner.actions().size()));
    EXPECT_EQ(nullptr, &(learner.best_action()));
    EXPECT_EQ(0, learner.total_number_of_samples());
    EXPECT_FLOAT_EQ(0.0, learner.decay_factor());
}

/*****************************************************************************/
TEST_F(TestUCB1Learner, setup) {
    utility::ucb1::Learner<int>             learner;
    std::vector<utility::ucb1::Action<int>> actions(2);
    actions[0].body = 0;
    actions[1].body = 1;
    learner.setup(actions, 0.5);

    EXPECT_EQ(2, static_cast<int>(learner.actions().size()));
    EXPECT_EQ(0, learner.actions()[0].body);
    EXPECT_EQ(1, learner.actions()[1].body);
    EXPECT_EQ(0, learner.best_action().body);
    EXPECT_EQ(0, learner.total_number_of_samples());
    EXPECT_FLOAT_EQ(0.5, learner.decay_factor());
}

/*****************************************************************************/
TEST_F(TestUCB1Learner, learn) {
    std::vector<utility::ucb1::Action<int>> actions(2);
    actions[0].body = 0;
    actions[1].body = 1;
    utility::ucb1::Learner<int> learner(actions, 0.5);

    learner.learn(10.0);
    EXPECT_EQ(10.0, learner.actions()[0].total_score);
    EXPECT_EQ(1, learner.total_number_of_samples());
    EXPECT_EQ(1, learner.best_action().body);

    learner.learn(10.0);
    EXPECT_EQ(10.0, learner.actions()[1].total_score);
    EXPECT_EQ(2, learner.total_number_of_samples());
    EXPECT_EQ(0, learner.best_action().body);
}

/*****************************************************************************/
TEST_F(TestUCB1Learner, actions) {
    /// This method is tested in constructor_with_arg().
}

/*****************************************************************************/
TEST_F(TestUCB1Learner, best_action) {
    /// This method is tested in constructor_with_arg().
}

/*****************************************************************************/
TEST_F(TestUCB1Learner, total_number_of_samples) {
    /// This method is tested in constructor_with_arg().
}

/*****************************************************************************/
TEST_F(TestUCB1Learner, decay_factor) {
    /// This method is tested in constructor_with_arg().
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/