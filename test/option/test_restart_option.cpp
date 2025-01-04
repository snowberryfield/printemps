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
class TestRestartOption : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestRestartOption, initialize) {
    using namespace printemps::option;
    RestartOption option;
    EXPECT_EQ(                                        //
        RestartOptionConstant::DEFAULT_RESTART_MODE,  //
        option.restart_mode);
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/