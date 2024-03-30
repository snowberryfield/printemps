/*****************************************************************************/
// Copyright (c) 2020-2024 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
using namespace printemps;
/*****************************************************************************/
class TestTabuMode : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestTabuMode, TabuModeMap) {
    using namespace printemps::option::tabu_mode;
    EXPECT_EQ(All, TabuModeMap.at("All"));
    EXPECT_EQ(Any, TabuModeMap.at("Any"));
}

/*****************************************************************************/
TEST_F(TestTabuMode, TabuModeInverseMap) {
    using namespace printemps::option::tabu_mode;
    EXPECT_EQ("All", TabuModeInverseMap.at(All));
    EXPECT_EQ("Any", TabuModeInverseMap.at(Any));
}
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/