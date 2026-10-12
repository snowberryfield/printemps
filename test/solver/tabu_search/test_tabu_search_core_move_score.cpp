/*****************************************************************************/
// Copyright (c) 2020-2026 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
using namespace printemps;
using namespace printemps::solver::tabu_search::core;

/*****************************************************************************/
class TestTabuSearchCoreMoveScore : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown(void) {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestTabuSearchCoreMoveScore, initialize) {
    TabuSearchCoreMoveScore score;

    EXPECT_FALSE(score.is_permissible);
    EXPECT_DOUBLE_EQ(score.frequency_penalty, 0.0);
    EXPECT_DOUBLE_EQ(score.lagrangian_penalty, 0.0);
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/