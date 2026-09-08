/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <string>

#include <printemps.h>

namespace {
using namespace printemps;
/*****************************************************************************/
class TestInteger : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestInteger, is_integer) {
    EXPECT_TRUE(utility::is_integer(0.0));
    EXPECT_TRUE(utility::is_integer(0));
    EXPECT_TRUE(utility::is_integer(1.0E6));
    EXPECT_FALSE(utility::is_integer(1.1));
}

/*****************************************************************************/
TEST_F(TestInteger, decimal_places) {
    EXPECT_EQ(0, utility::decimal_places(0.0));
    EXPECT_EQ(0, utility::decimal_places(2.0));
    EXPECT_EQ(1, utility::decimal_places(0.5));
    EXPECT_EQ(2, utility::decimal_places(0.25));
    EXPECT_EQ(3, utility::decimal_places(0.125));
    EXPECT_EQ(6, utility::decimal_places(0.333333333333));
}

/*****************************************************************************/
TEST_F(TestInteger, luby_value) {
    std::vector<int> expected = {1, 1, 2, 1, 1, 2, 4, 1, 1,
                                 2, 1, 1, 2, 4, 8, 1, 1, 2};
    for (size_t i = 0; i < expected.size(); i++) {
        EXPECT_EQ(expected[i], utility::luby_value(i + 1));
    }

    // Boundary/edge conditions
    EXPECT_EQ(1, utility::luby_value(0));
    EXPECT_EQ(1, utility::luby_value(-1));
}
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/