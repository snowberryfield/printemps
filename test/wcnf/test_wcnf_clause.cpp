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
class TestWCNFClause : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestWCNFClause, initialize) {
    wcnf::WCNFClause clause;
    EXPECT_TRUE(clause.literals.empty());
    EXPECT_EQ(0u, clause.weight);
    EXPECT_FALSE(clause.is_hard);
    EXPECT_EQ("", clause.name);
}

/*****************************************************************************/
TEST_F(TestWCNFClause, number_of_negative_literals) {
    wcnf::WCNFClause clause;
    clause.literals = {1, -2, 3, -4, -5};
    EXPECT_EQ(3, clause.number_of_negative_literals());

    clause.literals = {1, 2, 3};
    EXPECT_EQ(0, clause.number_of_negative_literals());

    clause.literals = {-1, -2};
    EXPECT_EQ(2, clause.number_of_negative_literals());
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/
