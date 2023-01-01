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
class TestMPSObjective : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestMPSObjective, initialize) {
    mps::MPSObjective objective;
    EXPECT_EQ(mps::MPSObjectiveSense::Minimize, objective.sense);
    EXPECT_EQ("", objective.name);
    EXPECT_TRUE(objective.sensitivities.empty());
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/