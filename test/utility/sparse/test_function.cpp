/*****************************************************************************/
// Copyright (c) 2020-2024 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
using namespace printemps;
/*****************************************************************************/
class TestSparseMatrixFunction : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestSparseMatrixFunction, distance) {
    printemps::utility::sparse::Vector x;
    x.values = std::vector<double>{1.0, 2.0, 3.0};
    printemps::utility::sparse::Vector y;
    y.values = std::vector<double>{3.0, 6.0, 9.0};

    EXPECT_FLOAT_EQ(std::sqrt(4.0 + 16.0 + 36.0),
                    printemps::utility::sparse::distance(x, y));
}

/*****************************************************************************/
TEST_F(TestSparseMatrixFunction, sum) {
    printemps::utility::sparse::Vector x;
    x.values = std::vector<double>{1.0, 2.0, 3.0};
    printemps::utility::sparse::Vector y;
    y.values = std::vector<double>{3.0, 6.0, 9.0};

    printemps::utility::sparse::Vector result(3);
    printemps::utility::sparse::sum(&result, x, y);

    EXPECT_FLOAT_EQ(4.0, result[0]);
    EXPECT_FLOAT_EQ(8.0, result[1]);
    EXPECT_FLOAT_EQ(12.0, result[2]);
}

/*****************************************************************************/
TEST_F(TestSparseMatrixFunction, subtract) {
    printemps::utility::sparse::Vector x;
    x.values = std::vector<double>{1.0, 2.0, 3.0};
    printemps::utility::sparse::Vector y;
    y.values = std::vector<double>{3.0, 6.0, 9.0};

    printemps::utility::sparse::Vector result(3);
    printemps::utility::sparse::subtract(&result, x, y);

    EXPECT_FLOAT_EQ(-2.0, result[0]);
    EXPECT_FLOAT_EQ(-4.0, result[1]);
    EXPECT_FLOAT_EQ(-6.0, result[2]);
}

/*****************************************************************************/
TEST_F(TestSparseMatrixFunction, linear_combination_1) {
    printemps::utility::sparse::Vector x;
    x.values = std::vector<double>{1.0, 2.0, 3.0};
    printemps::utility::sparse::Vector y;
    y.values = std::vector<double>{3.0, 6.0, 9.0};

    printemps::utility::sparse::Vector result(3);
    printemps::utility::sparse::linear_combination(  //
        &result, 2.0, x, 3.0, y);

    EXPECT_FLOAT_EQ(11.0, result[0]);
    EXPECT_FLOAT_EQ(22.0, result[1]);
    EXPECT_FLOAT_EQ(33.0, result[2]);
}

/*****************************************************************************/
TEST_F(TestSparseMatrixFunction, linear_combination_2) {
    printemps::utility::sparse::Vector x;
    x.values = std::vector<double>{1.0, 2.0, 3.0};
    printemps::utility::sparse::Vector y;
    y.values = std::vector<double>{3.0, 6.0, 9.0};
    printemps::utility::sparse::Vector z;
    z.values = std::vector<double>{5.0, 10.0, 15.0};

    printemps::utility::sparse::Vector result(3);
    printemps::utility::sparse::linear_combination(  //
        &result, 2.0, x, 3.0, y, 4.0, z);

    EXPECT_FLOAT_EQ(31.0, result[0]);
    EXPECT_FLOAT_EQ(62.0, result[1]);
    EXPECT_FLOAT_EQ(93.0, result[2]);
}

/*****************************************************************************/
TEST_F(TestSparseMatrixFunction, linear_combination_3) {
    printemps::utility::sparse::Vector x;
    x.values = std::vector<double>{1.0, 2.0, 3.0};
    printemps::utility::sparse::Vector y;
    y.values = std::vector<double>{3.0, 6.0, 9.0};
    printemps::utility::sparse::Vector z;
    z.values = std::vector<double>{5.0, 10.0, 15.0};

    printemps::utility::sparse::Vector result(3);

    printemps::utility::sparse::linear_combination(  //
        &result, {2.0, 3.0, 4.0}, {&x, &y, &z});

    EXPECT_FLOAT_EQ(31.0, result[0]);
    EXPECT_FLOAT_EQ(62.0, result[1]);
    EXPECT_FLOAT_EQ(93.0, result[2]);
}

/*****************************************************************************/
TEST_F(TestSparseMatrixFunction, linear_combination_4) {
    printemps::utility::sparse::Vector x;
    x.values = std::vector<double>{1.0, 2.0, 3.0};
    printemps::utility::sparse::Vector y;
    y.values = std::vector<double>{3.0, 6.0, 9.0};
    printemps::utility::sparse::Vector z;
    z.values = std::vector<double>{5.0, 10.0, 15.0};

    std::vector<double> values         = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
    std::vector<int>    row_indices    = {0, 0, 0, 1, 1, 2};
    std::vector<int>    column_indices = {0, 1, 2, 1, 2, 2};

    utility::sparse::SparseMatrix sparse_matrix(  //
        values, row_indices, column_indices, 3, 3);

    printemps::utility::sparse::Vector result(3);

    printemps::utility::sparse::linear_combination(  //
        &result, 2.0, sparse_matrix, x, 3.0, y, 4.0, z);

    EXPECT_FLOAT_EQ(41.0, result[0]);
    EXPECT_FLOAT_EQ(68.0, result[1]);
    EXPECT_FLOAT_EQ(93.0, result[2]);
}

/*****************************************************************************/
TEST_F(TestSparseMatrixFunction, linear_combination_5) {
    printemps::utility::sparse::Vector x;
    x.values = std::vector<double>{1.0, 2.0, 3.0};
    printemps::utility::sparse::Vector y;
    y.values = std::vector<double>{3.0, 6.0, 9.0};
    printemps::utility::sparse::Vector z;
    z.values = std::vector<double>{5.0, 10.0, 15.0};

    std::vector<double> values         = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
    std::vector<int>    row_indices    = {0, 0, 0, 1, 1, 2};
    std::vector<int>    column_indices = {0, 1, 2, 1, 2, 2};

    utility::sparse::SparseMatrix sparse_matrix(  //
        values, row_indices, column_indices, 3, 3);

    printemps::utility::sparse::Vector result(3);

    printemps::utility::sparse::linear_combination(  //
        &result, 2.0, sparse_matrix, x, {3.0, 4.0}, {&y, &z});

    EXPECT_FLOAT_EQ(41.0, result[0]);
    EXPECT_FLOAT_EQ(68.0, result[1]);
    EXPECT_FLOAT_EQ(93.0, result[2]);
}

/*****************************************************************************/
TEST_F(TestSparseMatrixFunction, dot_dot) {
    printemps::utility::sparse::Vector x;
    x.values = std::vector<double>{1.0, 2.0, 3.0, 4.0};
    printemps::utility::sparse::Vector y;
    y.values = std::vector<double>{3.0, 6.0, 9.0};

    std::vector<double> values         = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
    std::vector<int>    row_indices    = {0, 0, 1, 1, 2, 2};
    std::vector<int>    column_indices = {0, 2, 1, 3, 0, 2};

    utility::sparse::SparseMatrix sparse_matrix(values, row_indices,
                                                column_indices, 3, 4);

    printemps::utility::sparse::Vector result(3);

    EXPECT_FLOAT_EQ(360.0, printemps::utility::sparse::dot_dot(
                               &result, y, sparse_matrix, x));
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/