/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <random>

#include <printemps.h>

namespace {
/*****************************************************************************/
class TestSelectionMode : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestSelectionMode, SelectionModeMap) {
    using namespace printemps::option::selection_mode;
    EXPECT_EQ(Off, SelectionModeMap.at("Off"));
    EXPECT_EQ(Defined, SelectionModeMap.at("Defined"));
    EXPECT_EQ(Smaller, SelectionModeMap.at("Smaller"));
    EXPECT_EQ(Larger, SelectionModeMap.at("Larger"));
    EXPECT_EQ(Independent, SelectionModeMap.at("Independent"));
    EXPECT_EQ(UserDefined, SelectionModeMap.at("UserDefined"));
}

/*****************************************************************************/
TEST_F(TestSelectionMode, SelectionModeInverseMap) {
    using namespace printemps::option::selection_mode;
    EXPECT_EQ("Off", SelectionModeInverseMap.at(Off));
    EXPECT_EQ("Defined", SelectionModeInverseMap.at(Defined));
    EXPECT_EQ("Smaller", SelectionModeInverseMap.at(Smaller));
    EXPECT_EQ("Larger", SelectionModeInverseMap.at(Larger));
    EXPECT_EQ("Independent", SelectionModeInverseMap.at(Independent));
    EXPECT_EQ("UserDefined", SelectionModeInverseMap.at(UserDefined));
}
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/