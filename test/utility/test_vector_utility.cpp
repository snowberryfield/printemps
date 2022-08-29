/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <string>

#include <printemps.h>

namespace {
using namespace printemps;
/*****************************************************************************/
class TestVectorUtility : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestVectorUtility, count_duplicate) {
    std::vector<int>         v_int_0  = {1, 2, 3, 4, 5};
    std::vector<int>         v_int_1  = {1, 1, 3, 4, 5};
    std::vector<double>      v_double = {1.0, 1.0, 3.0, 4.0, 1.0};
    std::vector<std::string> v_string = {"a", "a", "a", "b", "a"};

    EXPECT_EQ(0, utility::count_duplicate(v_int_0));
    EXPECT_EQ(1, utility::count_duplicate(v_int_1));
    EXPECT_EQ(2, utility::count_duplicate(v_double));
    EXPECT_EQ(3, utility::count_duplicate(v_string));
}

/*****************************************************************************/
TEST_F(TestVectorUtility, count) {
    std::vector<int>         v_int_0  = {1, 2, 3, 4, 5};
    std::vector<int>         v_int_1  = {1, 1, 3, 4, 5};
    std::vector<double>      v_double = {1.0, 1.0, 3.0, 4.0, 1.0};
    std::vector<std::string> v_string = {"a", "a", "a", "b", "a"};

    EXPECT_EQ(1, utility::count(v_int_0, 1));
    EXPECT_EQ(2, utility::count(v_int_1, 1));
    EXPECT_EQ(3, utility::count(v_double, 1.0));
    std::string key = "a";
    EXPECT_EQ(4, utility::count(v_string, key));
}

/*****************************************************************************/
TEST_F(TestVectorUtility, min) {
    std::vector<int>    v_int    = {-3, -2, -1, -20, 10, 2, 3, 4, 5};
    std::vector<double> v_double = {-5.0,  -4.0, -3.0, -2.0, -1.0,
                                    -30.0, 20.0, 2.0,  3.0};
    EXPECT_EQ(-20, utility::min(v_int));
    EXPECT_EQ(-30.0, utility::min(v_double));
}

/*****************************************************************************/
TEST_F(TestVectorUtility, max) {
    std::vector<int>    v_int    = {-3, -2, -1, -20, 10, 2, 3, 4, 5};
    std::vector<double> v_double = {-5.0,  -4.0, -3.0, -2.0, -1.0,
                                    -30.0, 20.0, 2.0,  3.0};
    EXPECT_EQ(10, utility::max(v_int));
    EXPECT_EQ(20.0, utility::max(v_double));
}

/*****************************************************************************/
TEST_F(TestVectorUtility, max_abs) {
    std::vector<int>    v_int    = {-3, -2, -1, -10, 20, 2, 3, 4, 5};
    std::vector<double> v_double = {-5.0,  -4.0, -3.0, -2.0, -1.0,
                                    -20.0, 30.0, 2.0,  3.0};
    EXPECT_EQ(20, utility::max_abs(v_int));
    EXPECT_EQ(30.0, utility::max_abs(v_double));
}

/*****************************************************************************/
TEST_F(TestVectorUtility, argmin) {
    std::vector<int>    v_int    = {-3, -2, -1, -20, 10, 2, 3, 4, 5};
    std::vector<double> v_double = {-5.0,  -4.0, -3.0, -2.0, -1.0,
                                    -30.0, 20.0, 2.0,  3.0};
    EXPECT_EQ(3, utility::argmin(v_int));
    EXPECT_EQ(5, utility::argmin(v_double));
}

/*****************************************************************************/
TEST_F(TestVectorUtility, argmax) {
    std::vector<int>    v_int    = {-3, -2, -1, -20, 10, 2, 3, 4, 5};
    std::vector<double> v_double = {-5.0,  -4.0, -3.0, -2.0, -1.0,
                                    -30.0, 20.0, 2.0,  3.0};
    EXPECT_EQ(4, utility::argmax(v_int));
    EXPECT_EQ(6, utility::argmax(v_double));
}

/*****************************************************************************/
TEST_F(TestVectorUtility, sequence) {
    int min = 5;
    int max = 100;

    std::vector<int> seq_1 = utility::sequence(max);
    for (auto i = 0; i < max; i++) {
        EXPECT_EQ(i, seq_1[i]);
    }

    std::vector<int> seq_2 = utility::sequence(min, max);
    for (auto i = min; i < max; i++) {
        EXPECT_EQ(i, seq_1[i]);
    }
}
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/