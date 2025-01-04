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
class TestSparseVector : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestSparseVector, constructor_without_arg) {
    utility::sparse::Vector vector;

    EXPECT_TRUE(vector.values.empty());
}

/*****************************************************************************/
TEST_F(TestSparseVector, constructor_with_arg) {
    utility::sparse::Vector vector(5, 1.0);

    EXPECT_EQ(5, static_cast<int>(vector.size()));
    EXPECT_FLOAT_EQ(1, vector[0]);
    EXPECT_FLOAT_EQ(1, vector[4]);
}

/*****************************************************************************/
TEST_F(TestSparseVector, initialize) {
    utility::sparse::Vector vector(5, 1.0);
    vector.initialize();
    EXPECT_TRUE(vector.values.empty());
}

/*****************************************************************************/
TEST_F(TestSparseVector, setup) {
    utility::sparse::Vector vector;
    vector.setup(5, 1.0);

    EXPECT_EQ(5, static_cast<int>(vector.size()));
    EXPECT_FLOAT_EQ(1, vector[0]);
    EXPECT_FLOAT_EQ(1, vector[4]);
}

/*****************************************************************************/
TEST_F(TestSparseVector, operator_square_bracket) {
    /// This method is tested in constructor_with_arg().
}

/*****************************************************************************/
TEST_F(TestSparseVector, max) {
    utility::sparse::Vector vector(5, 0.0);
    vector[0] = 2.0;
    vector[1] = 5.0;
    vector[2] = 4.0;
    vector[3] = 1.0;
    vector[4] = 3.0;
    EXPECT_FLOAT_EQ(5.0, vector.max());
}

/*****************************************************************************/
TEST_F(TestSparseVector, min) {
    utility::sparse::Vector vector(5, 0.0);
    vector[0] = 2.0;
    vector[1] = 5.0;
    vector[2] = 4.0;
    vector[3] = 1.0;
    vector[4] = 3.0;
    EXPECT_FLOAT_EQ(1.0, vector.min());
}

/*****************************************************************************/
TEST_F(TestSparseVector, square_sum) {
    utility::sparse::Vector vector(5, 0.0);
    vector[0] = -2.0;
    vector[1] = 5.0;
    vector[2] = -4.0;
    vector[3] = 1.0;
    vector[4] = -3.0;
    EXPECT_FLOAT_EQ(55.0, vector.square_sum());
}

/*****************************************************************************/
TEST_F(TestSparseVector, norm_without_arg) {
    utility::sparse::Vector vector(5, 0.0);
    vector[0] = -2.0;
    vector[1] = 5.0;
    vector[2] = -4.0;
    vector[3] = 1.0;
    vector[4] = -3.0;
    EXPECT_FLOAT_EQ(sqrt(55.0), vector.norm());
}

/*****************************************************************************/
TEST_F(TestSparseVector, norm_with_arg) {
    utility::sparse::Vector vector(5, 0.0);
    vector[0] = -2.0;
    vector[1] = 5.0;
    vector[2] = -4.0;
    vector[3] = 1.0;
    vector[4] = -3.0;
    EXPECT_FLOAT_EQ(sqrt(55.0), vector.norm(2.0));
    EXPECT_FLOAT_EQ(15.0, vector.norm(1.0));
}

/*****************************************************************************/
TEST_F(TestSparseVector, size) {
    /// This method is tested in constructor_with_arg().
}

/*****************************************************************************/
TEST_F(TestSparseVector, reset) {
    utility::sparse::Vector vector(5, 0.0);
    vector[0] = -2.0;
    vector[1] = 5.0;
    vector[2] = -4.0;
    vector[3] = 1.0;
    vector[4] = -3.0;
    vector.reset();
    EXPECT_FLOAT_EQ(0.0, vector[0]);
    EXPECT_FLOAT_EQ(0.0, vector[1]);
    EXPECT_FLOAT_EQ(0.0, vector[2]);
    EXPECT_FLOAT_EQ(0.0, vector[3]);
    EXPECT_FLOAT_EQ(0.0, vector[4]);
}

/*****************************************************************************/
TEST_F(TestSparseVector, operator_plus) {
    utility::sparse::Vector vector(5, 1.0);

    auto vector_plus = +vector;

    EXPECT_FLOAT_EQ(1.0, vector_plus[0]);
    EXPECT_FLOAT_EQ(1.0, vector_plus[2]);
}

/*****************************************************************************/
TEST_F(TestSparseVector, operator_minus) {
    utility::sparse::Vector vector(5, 1.0);

    auto vector_minus = -vector;

    EXPECT_FLOAT_EQ(-1.0, vector_minus[0]);
    EXPECT_FLOAT_EQ(-1.0, vector_minus[2]);
}

/*****************************************************************************/
TEST_F(TestSparseVector, operator_plus_equal_scaler) {
    utility::sparse::Vector vector(5, 1.0);
    vector += 2.0;

    EXPECT_FLOAT_EQ(3.0, vector[0]);
    EXPECT_FLOAT_EQ(3.0, vector[2]);
}

/*****************************************************************************/
TEST_F(TestSparseVector, operator_plus_equal_vector) {
    utility::sparse::Vector vector(5, 1.0);
    vector += utility::sparse::Vector(5, 2.0);

    EXPECT_FLOAT_EQ(3.0, vector[0]);
    EXPECT_FLOAT_EQ(3.0, vector[2]);
}

/*****************************************************************************/
TEST_F(TestSparseVector, operator_minus_equal_scaler) {
    utility::sparse::Vector vector(5, 1.0);
    vector -= 2.0;

    EXPECT_FLOAT_EQ(-1.0, vector[0]);
    EXPECT_FLOAT_EQ(-1.0, vector[2]);
}

/*****************************************************************************/
TEST_F(TestSparseVector, operator_minus_equal_vector) {
    utility::sparse::Vector vector(5, 1.0);
    vector -= utility::sparse::Vector(5, 2.0);

    EXPECT_FLOAT_EQ(-1.0, vector[0]);
    EXPECT_FLOAT_EQ(-1.0, vector[2]);
}

/*****************************************************************************/
TEST_F(TestSparseVector, operator_product_equal) {
    utility::sparse::Vector vector(5, 1.0);
    vector *= 1.5;

    EXPECT_FLOAT_EQ(1.5, vector[0]);
    EXPECT_FLOAT_EQ(1.5, vector[2]);
}

/*****************************************************************************/
TEST_F(TestSparseVector, operator_divide_equal) {
    utility::sparse::Vector vector(5, 1.0);
    vector /= 2.0;

    EXPECT_FLOAT_EQ(0.5, vector[0]);
    EXPECT_FLOAT_EQ(0.5, vector[2]);
}

/*****************************************************************************/
TEST_F(TestSparseVector, clamp) {
    utility::sparse::Vector lower_bound(5, -1.0);
    utility::sparse::Vector upper_bound(5, 1.0);

    utility::sparse::Vector vector(5, 0.0);
    vector[0] = -1.5;
    vector[1] = -0.5;
    vector[2] = 0.0;
    vector[3] = 0.5;
    vector[4] = 1.5;
    vector.clamp(lower_bound, upper_bound);

    EXPECT_FLOAT_EQ(-1.0, vector[0]);
    EXPECT_FLOAT_EQ(-0.5, vector[1]);
    EXPECT_FLOAT_EQ(0.0, vector[2]);
    EXPECT_FLOAT_EQ(0.5, vector[3]);
    EXPECT_FLOAT_EQ(1.0, vector[4]);
}

/*****************************************************************************/
TEST_F(TestSparseVector, clamp_positive) {
    utility::sparse::Vector vector(5, 0.0);
    vector[0] = -1.5;
    vector[1] = -0.5;
    vector[2] = 0.0;
    vector[3] = 0.5;
    vector[4] = 1.5;
    vector.clamp_positive();

    EXPECT_FLOAT_EQ(0.0, vector[0]);
    EXPECT_FLOAT_EQ(0.0, vector[1]);
    EXPECT_FLOAT_EQ(0.0, vector[2]);
    EXPECT_FLOAT_EQ(0.5, vector[3]);
    EXPECT_FLOAT_EQ(1.5, vector[4]);
}

/*****************************************************************************/
TEST_F(TestSparseVector, clamp_negative) {
    utility::sparse::Vector vector(5, 0.0);
    vector[0] = -1.5;
    vector[1] = -0.5;
    vector[2] = 0.0;
    vector[3] = 0.5;
    vector[4] = 1.5;
    vector.clamp_negative();

    EXPECT_FLOAT_EQ(-1.5, vector[0]);
    EXPECT_FLOAT_EQ(-0.5, vector[1]);
    EXPECT_FLOAT_EQ(0.0, vector[2]);
    EXPECT_FLOAT_EQ(0.0, vector[3]);
    EXPECT_FLOAT_EQ(0.0, vector[4]);
}

/*****************************************************************************/
TEST_F(TestSparseVector, dot) {
    utility::sparse::Vector vector_0(5, 1.0);
    utility::sparse::Vector vector_1(5, 2.0);

    EXPECT_FLOAT_EQ(10.0, vector_0.dot(vector_1));
}

/*****************************************************************************/
TEST_F(TestSparseVector, extend) {
    utility::sparse::Vector vector_0(2, -1.0);
    utility::sparse::Vector vector_1(3, 1.0);

    auto result = vector_0.extend(vector_1);

    EXPECT_EQ(5, static_cast<int>(result.size()));
    EXPECT_FLOAT_EQ(-1.0, result[0]);
    EXPECT_FLOAT_EQ(-1.0, result[1]);
    EXPECT_FLOAT_EQ(1.0, result[2]);
    EXPECT_FLOAT_EQ(1.0, result[3]);
    EXPECT_FLOAT_EQ(1.0, result[4]);
}

/*****************************************************************************/
TEST_F(TestSparseVector, learn) {
    utility::sparse::Vector vector_0(5, 1.0);
    utility::sparse::Vector vector_1(5, 2.0);

    vector_0.learn(vector_1, 0.5);

    EXPECT_FLOAT_EQ(1.5, vector_0[0]);
    EXPECT_FLOAT_EQ(1.5, vector_0[4]);
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/