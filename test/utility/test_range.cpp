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
class TestRange : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestRange, initialize) {
    {
        utility::Range<int> range;
        EXPECT_EQ(std::numeric_limits<int>::max(), range.min());
        EXPECT_EQ(std::numeric_limits<int>::lowest(), range.max());
    }

    {
        utility::Range<double> range;
        EXPECT_EQ(std::numeric_limits<double>::max(), range.min());
        EXPECT_EQ(std::numeric_limits<double>::lowest(), range.max());
    }
}

/*****************************************************************************/
TEST_F(TestRange, update) {
    {
        utility::Range<int> range;
        range.update(10);
        range.update(-20);
        EXPECT_EQ(-20, range.min());
        EXPECT_EQ(10, range.max());
        EXPECT_EQ(20, range.max_abs());
        EXPECT_EQ(30, range.range());
    }

    {
        utility::Range<double> range;
        range.update(10.0);
        range.update(-20.0);
        EXPECT_FLOAT_EQ(-20.0, range.min());
        EXPECT_FLOAT_EQ(10.0, range.max());
        EXPECT_FLOAT_EQ(20, range.max_abs());
        EXPECT_FLOAT_EQ(30.0, range.range());
    }
}

/*****************************************************************************/
TEST_F(TestRange, min) {
    /// This test is covered by update().
}

/*****************************************************************************/
TEST_F(TestRange, max) {
    /// This test is covered by update().
}

/*****************************************************************************/
TEST_F(TestRange, max_abs) {
    /// This test is covered by update().
}

/*****************************************************************************/
TEST_F(TestRange, range) {
    /// This test is covered by update().
}
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/