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
class TestOPBMetadata : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestOPBMetadata, initialize) {
    opb::OPBMetadata metadata;
    EXPECT_EQ(0, metadata.number_of_variables);
    EXPECT_EQ(0, metadata.number_of_constraints);
    EXPECT_EQ(0, metadata.number_of_equals);
    EXPECT_EQ(0, metadata.number_of_products);
    EXPECT_EQ(0, metadata.intsize);
    EXPECT_EQ(0, metadata.sizeproduct);
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/