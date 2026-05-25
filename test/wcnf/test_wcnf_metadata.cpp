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
class TestWCNFMetadata : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestWCNFMetadata, initialize) {
    wcnf::WCNFMetadata metadata;
    EXPECT_EQ(0, metadata.number_of_variables);
    EXPECT_EQ(0, metadata.number_of_clauses);
    EXPECT_EQ(0, metadata.number_of_hard_clauses);
    EXPECT_EQ(0, metadata.number_of_soft_clauses);
    EXPECT_EQ(0u, metadata.weight_sum);
    EXPECT_EQ(0u, metadata.max_weight);
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/
