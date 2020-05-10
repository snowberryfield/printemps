/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <cppmh.h>

/*****************************************************************************/
namespace {
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
    EXPECT_EQ("hoge hoge", cppmh::utility::trim(" hoge hoge "));
}

/*****************************************************************************/
TEST_F(TestStringUtility, to_string) {
    EXPECT_EQ(" 100", cppmh::utility::to_string(100, "%4d"));
    EXPECT_EQ("0100", cppmh::utility::to_string(100, "%04d"));
    EXPECT_EQ("100.00000", cppmh::utility::to_string(100.0, "%.5f"));
    EXPECT_EQ("1.000e+02", cppmh::utility::to_string(100.0, "%.3e"));
}

/*****************************************************************************/
TEST_F(TestStringUtility, format_error_location) {
    /// The test for this function is omitted.
}

/*****************************************************************************/
TEST_F(TestStringUtility, delete_crlf) {
    EXPECT_EQ("hogehoge", cppmh::utility::delete_crlf("hogehoge"));
    EXPECT_EQ("hogehoge", cppmh::utility::delete_crlf("hogehoge\r"));
    EXPECT_EQ("hogehoge", cppmh::utility::delete_crlf("hogehoge\n"));
    EXPECT_EQ("hogehoge", cppmh::utility::delete_crlf("hogehoge\r\n"));
    EXPECT_EQ("hogehoge", cppmh::utility::delete_crlf("hogehoge\n\r"));
    EXPECT_EQ("hogehoge", cppmh::utility::delete_crlf("\r\nhogehoge"));
    EXPECT_EQ("hogehoge", cppmh::utility::delete_crlf("hogehoge\n\r\r\n"));
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/