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
class TestOPBObjective : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestOPBObjective, initialize) {
    opb::OPBObjective objective;
    EXPECT_TRUE(objective.is_minimization);
    EXPECT_EQ("", objective.name);
    EXPECT_TRUE(objective.terms.empty());
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/