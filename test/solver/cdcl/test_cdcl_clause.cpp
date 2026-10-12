/*****************************************************************************/
// Copyright (c) 2020-2026 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
using namespace printemps;
using namespace printemps::solver::cdcl::core;

/*****************************************************************************/
class TestCDCLClause : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown(void) {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestCDCLClause, make_literal) {
    // Variable index 3, positive polarity (sign = 0)
    const int LITERAL_POSITIVE = make_literal(3, 0);
    EXPECT_EQ(literal_variable(LITERAL_POSITIVE), 3);
    EXPECT_EQ(literal_sign(LITERAL_POSITIVE), 0);

    // Variable index 3, negative polarity (sign = 1)
    const int LITERAL_NEGATIVE = make_literal(3, 1);
    EXPECT_EQ(literal_variable(LITERAL_NEGATIVE), 3);
    EXPECT_EQ(literal_sign(LITERAL_NEGATIVE), 1);

    // Literal negate
    EXPECT_EQ(literal_negate(LITERAL_POSITIVE), LITERAL_NEGATIVE);
    EXPECT_EQ(literal_negate(LITERAL_NEGATIVE), LITERAL_POSITIVE);
}

/*****************************************************************************/
TEST_F(TestCDCLClause, literal_variable) {
    /// This method is tested in make_literal().
}

/*****************************************************************************/
TEST_F(TestCDCLClause, literal_sign) {
    /// This method is tested in make_literal().
}

/*****************************************************************************/
TEST_F(TestCDCLClause, literal_negate) {
    /// This method is tested in make_literal().
}

/*****************************************************************************/
TEST_F(TestCDCLClause, initialize) {
    CDCLClause clause;
    EXPECT_EQ(clause.index, -1);
    EXPECT_EQ(clause.literal_block_distance, 0);
    EXPECT_FALSE(clause.is_deleted);
    EXPECT_FALSE(clause.is_learned);
    EXPECT_TRUE(clause.empty());
    EXPECT_EQ(clause.size(), 0);
}

/*****************************************************************************/
TEST_F(TestCDCLClause, constructor) {
    std::vector<int> literals = {make_literal(0, 0), make_literal(1, 1)};
    CDCLClause       clause(5, 2, true, std::move(literals));

    EXPECT_EQ(clause.index, 5);
    EXPECT_EQ(clause.literal_block_distance, 2);
    EXPECT_TRUE(clause.is_learned);
    EXPECT_FALSE(clause.is_deleted);
    EXPECT_FALSE(clause.empty());
    EXPECT_EQ(clause.size(), 2);
    EXPECT_EQ(clause.literals[0], make_literal(0, 0));
    EXPECT_EQ(clause.literals[1], make_literal(1, 1));
}

/*****************************************************************************/
TEST_F(TestCDCLClause, empty) {
    /// This method is tested in initialize() and constructor().
}

/*****************************************************************************/
TEST_F(TestCDCLClause, size) {
    /// This method is tested in initialize() and constructor().
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/