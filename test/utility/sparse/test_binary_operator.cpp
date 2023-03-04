/*****************************************************************************/
// Copyright (c) 2020-2023 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
using namespace printemps;
/*****************************************************************************/
class TestSparseMatrixBinaryOperator : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestSparseMatrixBinaryOperator, operator_add_vector_and_double) {
    printemps::utility::sparse::Vector x;
    x.values = std::vector<double>{1.0, 2.0, 3.0};

    auto y = x + 1.1;
    EXPECT_FLOAT_EQ(2.1, y[0]);
    EXPECT_FLOAT_EQ(3.1, y[1]);
    EXPECT_FLOAT_EQ(4.1, y[2]);
}

/*****************************************************************************/
TEST_F(TestSparseMatrixBinaryOperator, operator_add_double_and_vector) {
    printemps::utility::sparse::Vector x;
    x.values = std::vector<double>{1.0, 2.0, 3.0};

    auto y = 1.1 + x;
    EXPECT_FLOAT_EQ(2.1, y[0]);
    EXPECT_FLOAT_EQ(3.1, y[1]);
    EXPECT_FLOAT_EQ(4.1, y[2]);
}

/*****************************************************************************/
TEST_F(TestSparseMatrixBinaryOperator, operator_add_vector_and_vector) {
    printemps::utility::sparse::Vector x;
    printemps::utility::sparse::Vector y;
    x.values = std::vector<double>{1.0, 2.0, 3.0};
    y.values = std::vector<double>{4.1, 5.1, 6.1};

    auto z = x + y;
    EXPECT_FLOAT_EQ(5.1, z[0]);
    EXPECT_FLOAT_EQ(7.1, z[1]);
    EXPECT_FLOAT_EQ(9.1, z[2]);
}

/*****************************************************************************/
TEST_F(TestSparseMatrixBinaryOperator, operator_subtract_vector_and_double) {
    printemps::utility::sparse::Vector x;
    x.values = std::vector<double>{1.0, 2.0, 3.0};

    auto y = x - 1.1;
    EXPECT_FLOAT_EQ(-0.1, y[0]);
    EXPECT_FLOAT_EQ(0.9, y[1]);
    EXPECT_FLOAT_EQ(1.9, y[2]);
}

/*****************************************************************************/
TEST_F(TestSparseMatrixBinaryOperator, operator_subtract_double_and_vector) {
    printemps::utility::sparse::Vector x;
    x.values = std::vector<double>{1, 2, 3};

    auto y = 1.1 - x;
    EXPECT_FLOAT_EQ(0.1, y[0]);
    EXPECT_FLOAT_EQ(-0.9, y[1]);
    EXPECT_FLOAT_EQ(-1.9, y[2]);
}

/*****************************************************************************/
TEST_F(TestSparseMatrixBinaryOperator, operator_subtract_vector_and_vector) {
    printemps::utility::sparse::Vector x;
    printemps::utility::sparse::Vector y;
    x.values = std::vector<double>{1.0, 2.0, 3.0};
    y.values = std::vector<double>{4.1, 5.1, 6.1};

    auto z = x - y;
    EXPECT_FLOAT_EQ(-3.1, z[0]);
    EXPECT_FLOAT_EQ(-3.1, z[1]);
    EXPECT_FLOAT_EQ(-3.1, z[2]);
}

/*****************************************************************************/
TEST_F(TestSparseMatrixBinaryOperator, operator_product_vector_and_double) {
    printemps::utility::sparse::Vector x;
    x.values = std::vector<double>{1, 2, 3};

    auto y = x * 1.1;
    EXPECT_FLOAT_EQ(1.1, y[0]);
    EXPECT_FLOAT_EQ(2.2, y[1]);
    EXPECT_FLOAT_EQ(3.3, y[2]);
}

/*****************************************************************************/
TEST_F(TestSparseMatrixBinaryOperator, operator_product_double_and_vector) {
    printemps::utility::sparse::Vector x;
    x.values = std::vector<double>{1, 2, 3};

    auto y = 1.1 * x;
    EXPECT_FLOAT_EQ(1.1, y[0]);
    EXPECT_FLOAT_EQ(2.2, y[1]);
    EXPECT_FLOAT_EQ(3.3, y[2]);
}

/*****************************************************************************/
TEST_F(TestSparseMatrixBinaryOperator, operator_divide_vector_and_double) {
    printemps::utility::sparse::Vector x;
    x.values = std::vector<double>{1.0, 2.0, 3.0};

    auto y = x / 0.5;
    EXPECT_FLOAT_EQ(2.0, y[0]);
    EXPECT_FLOAT_EQ(4.0, y[1]);
    EXPECT_FLOAT_EQ(6.0, y[2]);
}

/*****************************************************************************/
TEST_F(TestSparseMatrixBinaryOperator,
       operator_product_sparse_matrix_and_vector) {
    std::vector<double> values         = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
    std::vector<int>    row_indices    = {0, 0, 1, 1, 2, 2};
    std::vector<int>    column_indices = {0, 2, 1, 3, 0, 2};

    utility::sparse::SparseMatrix sparse_matrix(values, row_indices,
                                                column_indices, 3, 4);

    printemps::utility::sparse::Vector x;
    x.values = std::vector<double>{1.0, 2.0, 3.0, 4.0};

    auto y = sparse_matrix * x;
    EXPECT_FLOAT_EQ(7, y[0]);
    EXPECT_FLOAT_EQ(22, y[1]);
    EXPECT_FLOAT_EQ(23, y[2]);
}

/*****************************************************************************/
TEST_F(TestSparseMatrixBinaryOperator,
       operator_product_sparse_matrix_and_diagonal) {
    std::vector<double> values         = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
    std::vector<int>    row_indices    = {0, 0, 1, 1, 2, 2};
    std::vector<int>    column_indices = {0, 2, 1, 3, 0, 2};

    utility::sparse::SparseMatrix sparse_matrix(values, row_indices,
                                                column_indices, 3, 4);

    printemps::utility::sparse::Diagonal diagonal;
    diagonal.diagonal_vector.values = std::vector<double>{1.0, 2.0, 3.0, 4.0};

    auto scaled = sparse_matrix * diagonal;
    EXPECT_FLOAT_EQ(1.0, scaled.values[0]);
    EXPECT_FLOAT_EQ(6.0, scaled.values[1]);
    EXPECT_FLOAT_EQ(6.0, scaled.values[2]);
    EXPECT_FLOAT_EQ(16.0, scaled.values[3]);
    EXPECT_FLOAT_EQ(5.0, scaled.values[4]);
    EXPECT_FLOAT_EQ(18.0, scaled.values[5]);
}

/*****************************************************************************/
TEST_F(TestSparseMatrixBinaryOperator,
       operator_product_diagonal_and_sparse_matrix) {
    std::vector<double> values         = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
    std::vector<int>    row_indices    = {0, 0, 1, 1, 2, 2};
    std::vector<int>    column_indices = {0, 2, 1, 3, 0, 2};

    utility::sparse::SparseMatrix sparse_matrix(values, row_indices,
                                                column_indices, 3, 4);

    printemps::utility::sparse::Diagonal diagonal;
    diagonal.diagonal_vector.values = std::vector<double>{1.0, 2.0, 3.0};

    auto scaled = diagonal * sparse_matrix;
    EXPECT_FLOAT_EQ(1.0, scaled.values[0]);
    EXPECT_FLOAT_EQ(2.0, scaled.values[1]);
    EXPECT_FLOAT_EQ(6.0, scaled.values[2]);
    EXPECT_FLOAT_EQ(8.0, scaled.values[3]);
    EXPECT_FLOAT_EQ(15.0, scaled.values[4]);
    EXPECT_FLOAT_EQ(18.0, scaled.values[5]);
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/