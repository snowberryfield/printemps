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
class TestMPSReadMode : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestMPSReadMode, MPSReadModeMap) {
    using namespace printemps::mps;
    EXPECT_EQ(MPSReadMode::Initial, MPSReadModeMap.at("INITIAL"));
    EXPECT_EQ(MPSReadMode::Name, MPSReadModeMap.at("NAME"));
    EXPECT_EQ(MPSReadMode::Objsense, MPSReadModeMap.at("OBJSENSE"));
    EXPECT_EQ(MPSReadMode::Objname, MPSReadModeMap.at("OBJNAME"));
    EXPECT_EQ(MPSReadMode::Rows, MPSReadModeMap.at("ROWS"));
    EXPECT_EQ(MPSReadMode::Columns, MPSReadModeMap.at("COLUMNS"));
    EXPECT_EQ(MPSReadMode::Rhs, MPSReadModeMap.at("RHS"));
    EXPECT_EQ(MPSReadMode::Ranges, MPSReadModeMap.at("RANGES"));
    EXPECT_EQ(MPSReadMode::Bounds, MPSReadModeMap.at("BOUNDS"));
    EXPECT_EQ(MPSReadMode::Endata, MPSReadModeMap.at("ENDATA"));
}

/*****************************************************************************/
TEST_F(TestMPSReadMode, MPSReadModeInverseMap) {
    using namespace printemps::mps;
    EXPECT_EQ("INITIAL", MPSReadModeInverseMap.at(MPSReadMode::Initial));
    EXPECT_EQ("NAME", MPSReadModeInverseMap.at(MPSReadMode::Name));
    EXPECT_EQ("OBJSENSE", MPSReadModeInverseMap.at(MPSReadMode::Objsense));
    EXPECT_EQ("OBJNAME", MPSReadModeInverseMap.at(MPSReadMode::Objname));
    EXPECT_EQ("ROWS", MPSReadModeInverseMap.at(MPSReadMode::Rows));
    EXPECT_EQ("COLUMNS", MPSReadModeInverseMap.at(MPSReadMode::Columns));
    EXPECT_EQ("RHS", MPSReadModeInverseMap.at(MPSReadMode::Rhs));
    EXPECT_EQ("RANGES", MPSReadModeInverseMap.at(MPSReadMode::Ranges));
    EXPECT_EQ("BOUNDS", MPSReadModeInverseMap.at(MPSReadMode::Bounds));
    EXPECT_EQ("ENDATA", MPSReadModeInverseMap.at(MPSReadMode::Endata));
}

}  // namespace
/*****************************************************************************/
// END