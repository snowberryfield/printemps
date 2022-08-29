/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
using namespace printemps;
/*****************************************************************************/
class TestVerbose : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestVerbose, VerboseMap) {
    using namespace printemps::option::verbose;
    EXPECT_EQ(Off, VerboseMap.at("Off"));
    EXPECT_EQ(Warning, VerboseMap.at("Warning"));
    EXPECT_EQ(Outer, VerboseMap.at("Outer"));
    EXPECT_EQ(Full, VerboseMap.at("Full"));
    EXPECT_EQ(Debug, VerboseMap.at("Debug"));
}

/*****************************************************************************/
TEST_F(TestVerbose, VerboseInverseMap) {
    using namespace printemps::option::verbose;
    EXPECT_EQ("Off", VerboseInverseMap.at(Off));
    EXPECT_EQ("Warning", VerboseInverseMap.at(Warning));
    EXPECT_EQ("Outer", VerboseInverseMap.at(Outer));
    EXPECT_EQ("Full", VerboseInverseMap.at(Full));
    EXPECT_EQ("Debug", VerboseInverseMap.at(Debug));
}
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/