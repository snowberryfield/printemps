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
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/