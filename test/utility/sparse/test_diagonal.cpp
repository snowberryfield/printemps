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
class TestSparseDiagonal : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestSparseDiagonal, constructor_without_arg) {
    utility::sparse::Diagonal diagonal;

    EXPECT_TRUE(diagonal.diagonal_vector.values.empty());
}

/*****************************************************************************/
TEST_F(TestSparseDiagonal, constructor_with_arg) {
    utility::sparse::Diagonal diagonal(5);

    EXPECT_EQ(5, static_cast<int>(diagonal.size()));
    EXPECT_FLOAT_EQ(1, diagonal[0]);
    EXPECT_FLOAT_EQ(1, diagonal[4]);
}

/*****************************************************************************/
TEST_F(TestSparseDiagonal, initialize) {
    utility::sparse::Diagonal diagonal(5);
    diagonal.initialize();
    EXPECT_TRUE(diagonal.diagonal_vector.values.empty());
}

/*****************************************************************************/
TEST_F(TestSparseDiagonal, setup) {
    utility::sparse::Diagonal diagonal;
    diagonal.setup(5);

    EXPECT_EQ(5, static_cast<int>(diagonal.size()));
    EXPECT_FLOAT_EQ(1, diagonal[0]);
    EXPECT_FLOAT_EQ(1, diagonal[4]);
}

/*****************************************************************************/
TEST_F(TestSparseDiagonal, inverse) {
    utility::sparse::Diagonal diagonal(5);
    diagonal *= 2.0;

    auto inverse = diagonal.inverse();
    EXPECT_EQ(5, static_cast<int>(inverse.size()));
    EXPECT_FLOAT_EQ(0.5, inverse[0]);
    EXPECT_FLOAT_EQ(0.5, inverse[4]);
}

/*****************************************************************************/
TEST_F(TestSparseDiagonal, size) {
    /// This method is tested in constructor_with_arg().
}

/*****************************************************************************/
TEST_F(TestSparseDiagonal, operator_plus_equal_scaler) {
    utility::sparse::Diagonal diagonal(5);
    diagonal *= 2.0;

    EXPECT_FLOAT_EQ(2.0, diagonal[0]);
    EXPECT_FLOAT_EQ(2.0, diagonal[2]);
}

/*****************************************************************************/
TEST_F(TestSparseDiagonal, operator_plus_equal_diagonal) {
    utility::sparse::Diagonal diagonal_0(5);
    utility::sparse::Diagonal diagonal_1(5);
    diagonal_0 *= 2.0;

    diagonal_1[0] = 1.0;
    diagonal_1[1] = 2.0;
    diagonal_1[2] = 3.0;
    diagonal_1[3] = 4.0;
    diagonal_1[4] = 5.0;

    diagonal_0 *= diagonal_1;

    EXPECT_FLOAT_EQ(2.0, diagonal_0[0]);
    EXPECT_FLOAT_EQ(4.0, diagonal_0[1]);
    EXPECT_FLOAT_EQ(6.0, diagonal_0[2]);
    EXPECT_FLOAT_EQ(8.0, diagonal_0[3]);
    EXPECT_FLOAT_EQ(10.0, diagonal_0[4]);
}

/*****************************************************************************/
TEST_F(TestSparseDiagonal, dot_with_buffer) {
    utility::sparse::Diagonal diagonal(5);
    utility::sparse::Vector   vector(5);
    utility::sparse::Vector   result(5);
    diagonal *= 2.0;

    vector[0] = 1.0;
    vector[1] = 2.0;
    vector[2] = 3.0;
    vector[3] = 4.0;
    vector[4] = 5.0;

    diagonal.dot(&result, vector);

    EXPECT_FLOAT_EQ(2.0, result[0]);
    EXPECT_FLOAT_EQ(4.0, result[1]);
    EXPECT_FLOAT_EQ(6.0, result[2]);
    EXPECT_FLOAT_EQ(8.0, result[3]);
    EXPECT_FLOAT_EQ(10.0, result[4]);
}

/*****************************************************************************/
TEST_F(TestSparseDiagonal, dot_without_buffer) {
    utility::sparse::Diagonal diagonal(5);
    utility::sparse::Vector   vector(5);
    utility::sparse::Vector   result(5);
    diagonal *= 2.0;

    vector[0] = 1.0;
    vector[1] = 2.0;
    vector[2] = 3.0;
    vector[3] = 4.0;
    vector[4] = 5.0;

    result = diagonal.dot(vector);

    EXPECT_FLOAT_EQ(2.0, result[0]);
    EXPECT_FLOAT_EQ(4.0, result[1]);
    EXPECT_FLOAT_EQ(6.0, result[2]);
    EXPECT_FLOAT_EQ(8.0, result[3]);
    EXPECT_FLOAT_EQ(10.0, result[4]);
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/