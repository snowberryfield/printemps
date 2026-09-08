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
class TestCDCLRestartMode : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestCDCLRestartMode, CDCLRestartModeMap) {
    using namespace printemps::option::cdcl_restart_mode;
    EXPECT_EQ(Geometric, CDCLRestartModeMap.at("Geometric"));
    EXPECT_EQ(Luby, CDCLRestartModeMap.at("Luby"));
    EXPECT_EQ(Adaptive, CDCLRestartModeMap.at("Adaptive"));
}

/*****************************************************************************/
TEST_F(TestCDCLRestartMode, CDCLRestartModeInverseMap) {
    using namespace printemps::option::cdcl_restart_mode;
    EXPECT_EQ("Geometric", CDCLRestartModeInverseMap.at(Geometric));
    EXPECT_EQ("Luby", CDCLRestartModeInverseMap.at(Luby));
    EXPECT_EQ("Adaptive", CDCLRestartModeInverseMap.at(Adaptive));
}
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/
