/*****************************************************************************/
// Copyright (c) 2020-2024 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <string>

#include <printemps.h>

namespace {
using namespace printemps;
/*****************************************************************************/
class TestBinaryMatrix : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestBinaryMatrix, setup) {
    utility::BinaryMatrix binary_matrix(10, 20);

    EXPECT_EQ(10, binary_matrix.number_of_rows());
    EXPECT_EQ(20, binary_matrix.number_of_columns());
}

/*****************************************************************************/
TEST_F(TestBinaryMatrix, inverse_and_rank) {
    {
        utility::BinaryMatrix binary_matrix(3, 3);

        for (auto i = 0; i < 3; i++) {
            binary_matrix[i][i] = 1;
        }
        auto result = binary_matrix.inverse_and_rank();
        EXPECT_EQ(1, result.first[0][0]);
        EXPECT_EQ(1, result.first[1][1]);
        EXPECT_EQ(1, result.first[2][2]);
        EXPECT_EQ(3, result.second);
    }

    {
        utility::BinaryMatrix binary_matrix(3, 3);

        for (auto i = 0; i < 3; i++) {
            binary_matrix[i][i] = 1;
        }
        binary_matrix[0][1] = 1;

        auto result = binary_matrix.inverse_and_rank();
        EXPECT_EQ(1, result.first[0][0]);
        EXPECT_EQ(1, result.first[1][1]);
        EXPECT_EQ(1, result.first[0][1]);
        EXPECT_EQ(1, result.first[2][2]);
        EXPECT_EQ(3, result.second);
    }

    {
        utility::BinaryMatrix binary_matrix(3, 3);

        for (auto i = 0; i < 3; i++) {
            binary_matrix[i][i] = 1;
        }
        binary_matrix[2][2] = 0;

        auto result = binary_matrix.inverse_and_rank();
        EXPECT_EQ(2, result.second);
    }
}

/*****************************************************************************/
TEST_F(TestBinaryMatrix, reachability) {
    auto binary_matrix  = utility::BinaryMatrix::identity(4);
    binary_matrix[0][1] = 1;
    binary_matrix[1][2] = 1;
    binary_matrix[2][3] = 1;
    binary_matrix[3][0] = 1;
    auto reachability   = binary_matrix.reachability();

    for (auto i = 0; i < 4; i++) {
        for (auto j = 0; j < 4; j++) {
            EXPECT_EQ(1, reachability[i][j]);
        }
    }
}
/*****************************************************************************/
TEST_F(TestBinaryMatrix, identity) {
    {
        auto identity = utility::BinaryMatrix::identity(2);
        EXPECT_EQ(2, identity.number_of_columns());
        EXPECT_EQ(2, identity.number_of_rows());
        EXPECT_EQ(1, identity[0][0]);
        EXPECT_EQ(0, identity[0][1]);
        EXPECT_EQ(0, identity[1][0]);
        EXPECT_EQ(1, identity[1][1]);
    }

    {
        auto identity = utility::BinaryMatrix::identity(3);
        EXPECT_EQ(3, identity.number_of_columns());
        EXPECT_EQ(3, identity.number_of_rows());
        EXPECT_EQ(1, identity[0][0]);
        EXPECT_EQ(0, identity[0][1]);
        EXPECT_EQ(0, identity[0][2]);
        EXPECT_EQ(0, identity[1][0]);
        EXPECT_EQ(1, identity[1][1]);
        EXPECT_EQ(0, identity[1][2]);
        EXPECT_EQ(0, identity[2][0]);
        EXPECT_EQ(0, identity[2][1]);
        EXPECT_EQ(1, identity[2][2]);
    }
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/