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
class TestChainMoveReduceMode : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestChainMoveReduceMode, ChainMoveReduceModeMap) {
    using namespace printemps::option::chain_move_reduce_mode;
    EXPECT_EQ(OverlapRate, ChainMoveReduceModeMap.at("OverlapRate"));
    EXPECT_EQ(Shuffle, ChainMoveReduceModeMap.at("Shuffle"));
}

/*****************************************************************************/
TEST_F(TestChainMoveReduceMode, ChainMoveReduceModeInverseMap) {
    using namespace printemps::option::chain_move_reduce_mode;
    EXPECT_EQ("OverlapRate", ChainMoveReduceModeInverseMap.at(OverlapRate));
    EXPECT_EQ("Shuffle", ChainMoveReduceModeInverseMap.at(Shuffle));
}
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/