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
class TestAuxiliaryFileReader : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestAuxiliaryFileReader, read_names_and_values) {
    auto names_and_values =
        helper::read_names_and_values("./dat/helper/names_and_values.txt");
    EXPECT_EQ(1.0, names_and_values["a"]);
    EXPECT_EQ(2.0, names_and_values["b"]);
    EXPECT_EQ(3.0, names_and_values["c"]);
    EXPECT_EQ(4.0, names_and_values["d"]);
}

/*****************************************************************************/
TEST_F(TestAuxiliaryFileReader, read_names) {
    auto names = helper::read_names("./dat/helper/names.txt");
    EXPECT_TRUE(names.find("a") != names.end());
    EXPECT_TRUE(names.find("b") != names.end());
    EXPECT_TRUE(names.find("c") != names.end());
    EXPECT_TRUE(names.find("d") != names.end());
}

/*****************************************************************************/
TEST_F(TestAuxiliaryFileReader, read_name_pairs) {
    auto name_pairs = helper::read_name_pairs("./dat/helper/name_pairs.txt");
    EXPECT_EQ("a", name_pairs[0].first);
    EXPECT_EQ("b", name_pairs[0].second);
    EXPECT_EQ("c", name_pairs[1].first);
    EXPECT_EQ("d", name_pairs[1].second);
    EXPECT_EQ("e", name_pairs[2].first);
    EXPECT_EQ("f", name_pairs[2].second);
    EXPECT_EQ("g", name_pairs[3].first);
    EXPECT_EQ("h", name_pairs[3].second);
}
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/