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
class TestSparseSparseMatrix : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestSparseSparseMatrix, constructor_without_arg) {
    utility::sparse::SparseMatrix sparse_matrix;

    EXPECT_TRUE(sparse_matrix.values.empty());
    EXPECT_TRUE(sparse_matrix.row_indices.empty());
    EXPECT_TRUE(sparse_matrix.column_indices.empty());
    EXPECT_EQ(0, sparse_matrix.number_of_rows);
    EXPECT_EQ(0, sparse_matrix.number_of_columns);
    EXPECT_FLOAT_EQ(0.0, sparse_matrix.norm_one);
    EXPECT_FLOAT_EQ(0.0, sparse_matrix.norm_infty);
}

/*****************************************************************************/
TEST_F(TestSparseSparseMatrix, constructor_with_arg) {
    std::vector<double> values         = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
    std::vector<int>    row_indices    = {0, 0, 1, 1, 2, 2};
    std::vector<int>    column_indices = {0, 2, 1, 3, 0, 2};

    utility::sparse::SparseMatrix sparse_matrix(values, row_indices,
                                                column_indices, 3, 4);

    EXPECT_EQ(6, static_cast<int>(sparse_matrix.values.size()));
    EXPECT_EQ(6, static_cast<int>(sparse_matrix.row_indices.size()));
    EXPECT_EQ(6, static_cast<int>(sparse_matrix.column_indices.size()));
    EXPECT_EQ(3, sparse_matrix.number_of_rows);
    EXPECT_EQ(4, sparse_matrix.number_of_columns);
    EXPECT_FLOAT_EQ(8.0, sparse_matrix.norm_one);
    EXPECT_FLOAT_EQ(6.0, sparse_matrix.norm_infty);
}

/*****************************************************************************/
TEST_F(TestSparseSparseMatrix, initialize) {
    std::vector<double> values         = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
    std::vector<int>    row_indices    = {0, 0, 1, 1, 2, 2};
    std::vector<int>    column_indices = {0, 2, 1, 3, 0, 2};

    utility::sparse::SparseMatrix sparse_matrix(values, row_indices,
                                                column_indices, 3, 4);

    sparse_matrix.initialize();

    EXPECT_TRUE(sparse_matrix.values.empty());
    EXPECT_TRUE(sparse_matrix.row_indices.empty());
    EXPECT_TRUE(sparse_matrix.column_indices.empty());
    EXPECT_EQ(0, sparse_matrix.number_of_rows);
    EXPECT_EQ(0, sparse_matrix.number_of_columns);
    EXPECT_FLOAT_EQ(0.0, sparse_matrix.norm_one);
    EXPECT_FLOAT_EQ(0.0, sparse_matrix.norm_infty);
}

/*****************************************************************************/
TEST_F(TestSparseSparseMatrix, setup) {
    utility::sparse::SparseMatrix sparse_matrix;

    std::vector<double> values         = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
    std::vector<int>    row_indices    = {0, 0, 1, 1, 2, 2};
    std::vector<int>    column_indices = {0, 2, 1, 3, 0, 2};

    sparse_matrix.setup(values, row_indices, column_indices, 3, 4);

    EXPECT_EQ(6, static_cast<int>(sparse_matrix.values.size()));
    EXPECT_EQ(6, static_cast<int>(sparse_matrix.row_indices.size()));
    EXPECT_EQ(6, static_cast<int>(sparse_matrix.column_indices.size()));
    EXPECT_EQ(3, sparse_matrix.number_of_rows);
    EXPECT_EQ(4, sparse_matrix.number_of_columns);
    EXPECT_FLOAT_EQ(8.0, sparse_matrix.norm_one);
    EXPECT_FLOAT_EQ(6.0, sparse_matrix.norm_infty);
}

/*****************************************************************************/
TEST_F(TestSparseSparseMatrix, pock_chambolle_scaling) {
    std::vector<double> values         = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
    std::vector<int>    row_indices    = {0, 0, 1, 1, 2, 2};
    std::vector<int>    column_indices = {0, 2, 1, 3, 0, 2};

    utility::sparse::SparseMatrix sparse_matrix(values, row_indices,
                                                column_indices, 3, 4);

    sparse_matrix.pock_chambolle_scaling();
    // Checked by printing as dense form.
    // sparse_matrix.print_as_dense();

    for (auto i = 0; i < static_cast<int>(sparse_matrix.values.size()); i++) {
        auto invert_scaled =
            sparse_matrix.values[i] *
            sparse_matrix.row_scaler[sparse_matrix.row_indices[i]] *
            sparse_matrix.column_scaler[sparse_matrix.column_indices[i]];
        EXPECT_FLOAT_EQ(values[i], invert_scaled);
    }
}

/*****************************************************************************/
TEST_F(TestSparseSparseMatrix, ruiz_scaling) {
    std::vector<double> values         = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
    std::vector<int>    row_indices    = {0, 0, 1, 1, 2, 2};
    std::vector<int>    column_indices = {0, 2, 1, 3, 0, 2};

    utility::sparse::SparseMatrix sparse_matrix(values, row_indices,
                                                column_indices, 3, 4);

    sparse_matrix.ruiz_scaling(10);
    // Checked by printing as dense form.
    // sparse_matrix.print_as_dense();

    for (auto i = 0; i < static_cast<int>(sparse_matrix.values.size()); i++) {
        auto invert_scaled =
            sparse_matrix.values[i] *
            sparse_matrix.row_scaler[sparse_matrix.row_indices[i]] *
            sparse_matrix.column_scaler[sparse_matrix.column_indices[i]];
        EXPECT_FLOAT_EQ(values[i], invert_scaled);
    }
}

/*****************************************************************************/
TEST_F(TestSparseSparseMatrix, transpose) {
    std::vector<double> values         = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
    std::vector<int>    row_indices    = {0, 0, 1, 1, 2, 2};
    std::vector<int>    column_indices = {0, 2, 1, 3, 0, 2};

    utility::sparse::SparseMatrix sparse_matrix(values, row_indices,
                                                column_indices, 3, 4);

    auto transposed = sparse_matrix.transpose();
    // Checked by printing as dense form.
    // transposed.print_as_dense();
    EXPECT_FLOAT_EQ(11.0, transposed.norm_one);
    EXPECT_FLOAT_EQ(6.0, transposed.norm_infty);
}

/*****************************************************************************/
TEST_F(TestSparseSparseMatrix, dot_with_buffer) {
    std::vector<double> values         = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
    std::vector<int>    row_indices    = {0, 0, 1, 1, 2, 2};
    std::vector<int>    column_indices = {0, 2, 1, 3, 0, 2};

    utility::sparse::SparseMatrix sparse_matrix(values, row_indices,
                                                column_indices, 3, 4);

    printemps::utility::sparse::Vector x;
    printemps::utility::sparse::Vector y(3);
    x.values = std::vector<double>{1, 2, 3, 4};

    sparse_matrix.dot(&y, x);
    EXPECT_EQ(7, y[0]);
    EXPECT_EQ(22, y[1]);
    EXPECT_EQ(23, y[2]);
}

/*****************************************************************************/
TEST_F(TestSparseSparseMatrix, dot_without_buffer) {
    std::vector<double> values         = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
    std::vector<int>    row_indices    = {0, 0, 1, 1, 2, 2};
    std::vector<int>    column_indices = {0, 2, 1, 3, 0, 2};

    utility::sparse::SparseMatrix sparse_matrix(values, row_indices,
                                                column_indices, 3, 4);

    printemps::utility::sparse::Vector x;
    printemps::utility::sparse::Vector y;
    x.values = std::vector<double>{1, 2, 3, 4};

    y = sparse_matrix.dot(x);
    EXPECT_EQ(7, y[0]);
    EXPECT_EQ(22, y[1]);
    EXPECT_EQ(23, y[2]);
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/