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
class TestOPBTopCost : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestOPBTopCost, initialize) {
    opb::OPBTopCost top_cost;
    EXPECT_FALSE(top_cost.is_defined);
    EXPECT_EQ(std::numeric_limits<int>::max(), top_cost.value);
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/