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
class TestStringUtility : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestStringUtility, trim) {
    EXPECT_EQ("hoge hoge", utility::trim(" hoge hoge "));
}

/*****************************************************************************/
TEST_F(TestStringUtility, delete_space) {
    EXPECT_EQ("hogehoge", utility::delete_space(" hoge hoge "));
}

/*****************************************************************************/
TEST_F(TestStringUtility, has_space) {
    EXPECT_TRUE(utility::has_space(" hoge hoge "));
    EXPECT_TRUE(utility::has_space("hoge hoge "));
    EXPECT_TRUE(utility::has_space(" hoge hoge"));
    EXPECT_TRUE(utility::has_space("hoge hoge"));
    EXPECT_FALSE(utility::has_space("hogehoge"));
}

/*****************************************************************************/
TEST_F(TestStringUtility, to_string) {
    EXPECT_EQ(" 100", utility::to_string(100, "%4d"));
    EXPECT_EQ("0100", utility::to_string(100, "%04d"));
    EXPECT_EQ("100.00000", utility::to_string(100.0, "%.5f"));
    EXPECT_EQ("1.000e+02", utility::to_string(100.0, "%.3e"));
}

/*****************************************************************************/
TEST_F(TestStringUtility, to_true_or_false) {
    EXPECT_EQ("False", utility::to_true_or_false(false));
    EXPECT_EQ("True", utility::to_true_or_false(true));
}

/*****************************************************************************/
TEST_F(TestStringUtility, format_error_location) {
    /// The test for this function is omitted.
}

/*****************************************************************************/
TEST_F(TestStringUtility, delete_crlf) {
    EXPECT_EQ("hogehoge", utility::delete_crlf("hogehoge"));
    EXPECT_EQ("hogehoge", utility::delete_crlf("hogehoge\r"));
    EXPECT_EQ("hogehoge", utility::delete_crlf("hogehoge\n"));
    EXPECT_EQ("hogehoge", utility::delete_crlf("hogehoge\r\n"));
    EXPECT_EQ("hogehoge", utility::delete_crlf("hogehoge\n\r"));
    EXPECT_EQ("hogehoge", utility::delete_crlf("\r\nhogehoge"));
    EXPECT_EQ("hogehoge", utility::delete_crlf("hogehoge\n\r\r\n"));
}

/*****************************************************************************/
TEST_F(TestStringUtility, remove_extension) {
    EXPECT_EQ("hogehoge", utility::remove_extension("hogehoge"));
    EXPECT_EQ("hogehoge", utility::remove_extension("hogehoge.txt"));
    EXPECT_EQ("hogehoge.txt", utility::remove_extension("hogehoge.txt.txt"));
}

/*****************************************************************************/
TEST_F(TestStringUtility, remove_path) {
    EXPECT_EQ("hogehoge", utility::remove_path("hogehoge"));
    EXPECT_EQ("hogehoge", utility::remove_path("path/to/hogehoge"));
    EXPECT_EQ("hogehoge", utility::remove_path("path\\to\\hogehoge"));
}

/*****************************************************************************/
TEST_F(TestStringUtility, base_name) {
    EXPECT_EQ("hogehoge", utility::base_name("hogehoge"));
    EXPECT_EQ("hogehoge", utility::base_name("hogehoge.txt"));
    EXPECT_EQ("hogehoge", utility::base_name("path/to/hogehoge"));
    EXPECT_EQ("hogehoge", utility::base_name("path/to/hogehoge.txt"));
    EXPECT_EQ("hogehoge", utility::base_name("path\\to\\hogehoge"));
    EXPECT_EQ("hogehoge", utility::base_name("path\\to\\hogehoge.txt"));
}
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/