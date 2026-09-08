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
TEST_F(TestStringUtility, int_format) {
    // 1. Default width (5)
    // - Not abbreviated when fits in 5 digits
    EXPECT_EQ("    0", utility::int_format(0));
    EXPECT_EQ("   42", utility::int_format(42));
    EXPECT_EQ("  999", utility::int_format(999));
    EXPECT_EQ(" 9999", utility::int_format(9999));
    EXPECT_EQ("99999", utility::int_format(99999));

    // - 100,000 ~ 99,999,999: uses 'k' abbreviation
    EXPECT_EQ(" 100k", utility::int_format(100000));
    EXPECT_EQ(" 125k", utility::int_format(125345));
    EXPECT_EQ("1000k", utility::int_format(1000000));
    EXPECT_EQ("9999k", utility::int_format(9999999));

    // - 10,000,000 ~ : uses 'M' abbreviation when 'k' doesn't fit in 5 digits
    EXPECT_EQ("  10M", utility::int_format(10000000));
    EXPECT_EQ(" 100M", utility::int_format(100000000));
    EXPECT_EQ("9999M", utility::int_format(9999999999LL));

    // 2. Custom width
    // - width = 4
    EXPECT_EQ(" 999", utility::int_format(999, 4));
    EXPECT_EQ("1000", utility::int_format(1000, 4));
    EXPECT_EQ("9999", utility::int_format(9999, 4));
    EXPECT_EQ(" 10k", utility::int_format(10000, 4));
    EXPECT_EQ("100k", utility::int_format(100000, 4));
    EXPECT_EQ("999k", utility::int_format(999999, 4));
    EXPECT_EQ("  1M", utility::int_format(1000000, 4));
    EXPECT_EQ(" 10M", utility::int_format(10000000, 4));
    EXPECT_EQ("999M", utility::int_format(999999999, 4));

    // - width = 6
    EXPECT_EQ(" 99999", utility::int_format(99999, 6));
    EXPECT_EQ("100000", utility::int_format(100000, 6));    // fits without abbreviation!
    EXPECT_EQ("999999", utility::int_format(999999, 6));    // fits without abbreviation!
    EXPECT_EQ(" 1000k", utility::int_format(1000000, 6));   // uses k first!
    EXPECT_EQ("99999k", utility::int_format(99999999, 6));  // uses k first!
    EXPECT_EQ("  100M", utility::int_format(100000000, 6)); // k exceeds 6, uses M!

    // - width = 8 (iteration column)
    EXPECT_EQ("       0", utility::int_format(0, 8));
    EXPECT_EQ("   10000", utility::int_format(10000, 8));
    EXPECT_EQ(" 9999999", utility::int_format(9999999, 8));
    EXPECT_EQ("99999999", utility::int_format(99999999, 8));
    EXPECT_EQ(" 100000k", utility::int_format(100000000, 8));
    EXPECT_EQ("9999999k", utility::int_format(9999999999LL, 8));
    EXPECT_EQ("1000000M", utility::int_format(1000000000000LL, 8));

    // - Negative numbers
    EXPECT_EQ("  -42", utility::int_format(-42, 5));
    EXPECT_EQ(" -10k", utility::int_format(-10000, 5));
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

/*****************************************************************************/
TEST_F(TestStringUtility, is_space_or_tab) {
    EXPECT_TRUE(utility::is_space_or_tab(' '));
    EXPECT_TRUE(utility::is_space_or_tab('\t'));
    EXPECT_FALSE(utility::is_space_or_tab('a'));
    EXPECT_FALSE(utility::is_space_or_tab('1'));
    EXPECT_FALSE(utility::is_space_or_tab('\n'));
    EXPECT_FALSE(utility::is_space_or_tab('\r'));
}

/*****************************************************************************/
TEST_F(TestStringUtility, split_items) {
    std::vector<std::string_view> items;

    utility::split_items("  hoge   hoge  ", &items);
    EXPECT_EQ(2, static_cast<int>(items.size()));
    EXPECT_EQ("hoge", items[0]);
    EXPECT_EQ("hoge", items[1]);

    utility::split_items("  hoge   hoge   hoge  ", &items);
    EXPECT_EQ(3, static_cast<int>(items.size()));
    EXPECT_EQ("hoge", items[0]);
    EXPECT_EQ("hoge", items[1]);
    EXPECT_EQ("hoge", items[2]);

    utility::split_items("  hoge   hoge   hoge   hoge  ", &items);
    EXPECT_EQ(4, static_cast<int>(items.size()));
    EXPECT_EQ("hoge", items[0]);
    EXPECT_EQ("hoge", items[1]);
    EXPECT_EQ("hoge", items[2]);
    EXPECT_EQ("hoge", items[3]);

    utility::split_items("hoge", &items);
    EXPECT_EQ(1, static_cast<int>(items.size()));
    EXPECT_EQ("hoge", items[0]);

    utility::split_items("   ", &items);
    EXPECT_EQ(0, static_cast<int>(items.size()));

    utility::split_items("", &items);
    EXPECT_EQ(0, static_cast<int>(items.size()));
}

/*****************************************************************************/
TEST_F(TestStringUtility, to_uppercase) {
    EXPECT_EQ("HOGEHOGE", utility::to_uppercase(std::string("hogehoge")));
    EXPECT_EQ("HOGEHOGE", utility::to_uppercase(std::string("HOGEHOGE")));
    EXPECT_EQ("HOGEHOGE", utility::to_uppercase(std::string("HoGeHoGe")));
    EXPECT_EQ("1234!@#$", utility::to_uppercase(std::string("1234!@#$")));
}

/*****************************************************************************/
TEST_F(TestStringUtility, to_lowercase) {
    EXPECT_EQ("hogehoge", utility::to_lowercase(std::string("hogehoge")));
    EXPECT_EQ("hogehoge", utility::to_lowercase(std::string("HOGEHOGE")));
    EXPECT_EQ("hogehoge", utility::to_lowercase(std::string("HoGeHoGe")));
    EXPECT_EQ("1234!@#$", utility::to_lowercase(std::string("1234!@#$")));
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/