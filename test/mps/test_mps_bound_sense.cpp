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
class TestMPSBoundSense : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestMPSBoundSense, bound_sense_map) {
    using namespace printemps::mps;
    EXPECT_EQ(MPSBoundSense::BV, bound_sense_map("BV"));
    EXPECT_EQ(MPSBoundSense::FR, bound_sense_map("FR"));
    EXPECT_EQ(MPSBoundSense::MI, bound_sense_map("MI"));
    EXPECT_EQ(MPSBoundSense::PL, bound_sense_map("PL"));
    EXPECT_EQ(MPSBoundSense::LO, bound_sense_map("LO"));
    EXPECT_EQ(MPSBoundSense::LI, bound_sense_map("LI"));
    EXPECT_EQ(MPSBoundSense::UP, bound_sense_map("UP"));
    EXPECT_EQ(MPSBoundSense::UI, bound_sense_map("UI"));
    EXPECT_EQ(MPSBoundSense::FX, bound_sense_map("FX"));
    EXPECT_EQ(MPSBoundSense::UNKNOWN, bound_sense_map("XX"));
}

}  // namespace
/*****************************************************************************/
// END