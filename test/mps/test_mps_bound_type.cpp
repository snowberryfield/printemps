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
class TestMPSBoundType : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestMPSBoundType, bound_type_map) {
    using namespace printemps::mps;
    EXPECT_EQ(MPSBoundType::BV, bound_type_map("BV"));
    EXPECT_EQ(MPSBoundType::FR, bound_type_map("FR"));
    EXPECT_EQ(MPSBoundType::MI, bound_type_map("MI"));
    EXPECT_EQ(MPSBoundType::PL, bound_type_map("PL"));
    EXPECT_EQ(MPSBoundType::LO, bound_type_map("LO"));
    EXPECT_EQ(MPSBoundType::LI, bound_type_map("LI"));
    EXPECT_EQ(MPSBoundType::UP, bound_type_map("UP"));
    EXPECT_EQ(MPSBoundType::UI, bound_type_map("UI"));
    EXPECT_EQ(MPSBoundType::FX, bound_type_map("FX"));
    EXPECT_EQ(MPSBoundType::UNKNOWN, bound_type_map("XX"));
}

}  // namespace
/*****************************************************************************/
// END