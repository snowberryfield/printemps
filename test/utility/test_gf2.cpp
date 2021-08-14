/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <string>

#include <printemps.h>

namespace {
/*****************************************************************************/
class TestGF2Matrix : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestGF2Matrix, setup) {
    printemps::utility::GF2Matrix gf2_matrix(10, 20);

    EXPECT_EQ(10, gf2_matrix.number_of_rows());
    EXPECT_EQ(20, gf2_matrix.number_of_columns());
}

/*****************************************************************************/
TEST_F(TestGF2Matrix, inverse_and_rank) {
    {
        printemps::utility::GF2Matrix gf2_matrix(3, 3);

        for (auto i = 0; i < 3; i++) {
            gf2_matrix[i][i] = 1;
        }
        auto result = gf2_matrix.inverse_and_rank();
        EXPECT_EQ(1, result.first[0][0]);
        EXPECT_EQ(1, result.first[1][1]);
        EXPECT_EQ(1, result.first[2][2]);
        EXPECT_EQ(3, result.second);
    }

    {
        printemps::utility::GF2Matrix gf2_matrix(3, 3);

        for (auto i = 0; i < 3; i++) {
            gf2_matrix[i][i] = 1;
        }
        gf2_matrix[0][1] = 1;

        auto result = gf2_matrix.inverse_and_rank();
        EXPECT_EQ(1, result.first[0][0]);
        EXPECT_EQ(1, result.first[1][1]);
        EXPECT_EQ(1, result.first[0][1]);
        EXPECT_EQ(1, result.first[2][2]);
        EXPECT_EQ(3, result.second);
    }

    {
        printemps::utility::GF2Matrix gf2_matrix(3, 3);

        for (auto i = 0; i < 3; i++) {
            gf2_matrix[i][i] = 1;
        }
        gf2_matrix[2][2] = 0;

        auto result = gf2_matrix.inverse_and_rank();
        EXPECT_EQ(2, result.second);
    }
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/