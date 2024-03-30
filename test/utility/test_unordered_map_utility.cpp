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
class TestUnorderedMapUtility : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestUnorderedMapUtility, to_vector_pair) {
    std::unordered_map<int, double>         umap_int;
    std::unordered_map<std::string, double> umap_str;

    umap_int[0]   = 1.1;
    umap_int[1]   = 2.2;
    umap_str["a"] = 3.3;
    umap_str["b"] = 4.4;
    umap_str["c"] = 5.5;

    auto vector_pair_int = utility::to_vector_pair(umap_int);
    auto vector_pair_str = utility::to_vector_pair(umap_str);

    EXPECT_EQ(2, static_cast<int>(vector_pair_int.first.size()));
    EXPECT_EQ(2, static_cast<int>(vector_pair_int.second.size()));

    EXPECT_EQ(3, static_cast<int>(vector_pair_str.first.size()));
    EXPECT_EQ(3, static_cast<int>(vector_pair_str.second.size()));
}

/*****************************************************************************/
TEST_F(TestUnorderedMapUtility, distance_l1) {
    {
        std::unordered_map<std::string, int> first;
        std::unordered_map<std::string, int> second;

        first["a"]  = 10;
        first["b"]  = -20;
        second["b"] = 30;
        second["c"] = -40;
        second["d"] = 50;

        EXPECT_EQ(10 + 50 + 40 + 50, utility::distance_l1(first, second));
        EXPECT_EQ(10 + 50 + 40 + 50, utility::distance_l1(second, first));
    }

    {
        std::unordered_map<std::string, int> first;
        std::unordered_map<std::string, int> second;

        first["a"]  = 10;
        first["b"]  = -20;
        second["b"] = -20;
        second["c"] = -40;
        second["d"] = 50;

        EXPECT_EQ(10 + 0 + 40 + 50, utility::distance_l1(first, second));
        EXPECT_EQ(10 + 0 + 40 + 50, utility::distance_l1(second, first));
    }
}

/*****************************************************************************/
TEST_F(TestUnorderedMapUtility, distance_l0) {
    {
        std::unordered_map<std::string, int> first;
        std::unordered_map<std::string, int> second;

        first["a"]  = 10;
        first["b"]  = -20;
        second["b"] = 30;
        second["c"] = -40;
        second["d"] = 50;

        EXPECT_EQ(4, utility::distance_l0(first, second));
        EXPECT_EQ(4, utility::distance_l0(second, first));
    }

    {
        std::unordered_map<std::string, int> first;
        std::unordered_map<std::string, int> second;

        first["a"]  = 10;
        first["b"]  = -20;
        second["b"] = -20;
        second["c"] = -40;
        second["d"] = 50;

        EXPECT_EQ(3, utility::distance_l0(first, second));
        EXPECT_EQ(3, utility::distance_l0(second, first));
    }

    {
        std::unordered_map<std::string, int> first;
        std::unordered_map<std::string, int> second;

        first["a"]  = 10;
        first["b"]  = 20;
        first["c"]  = 30;
        second["a"] = 10;
        second["b"] = 20;
        second["c"] = 30;

        EXPECT_EQ(0, utility::distance_l0(first, second));
        EXPECT_EQ(0, utility::distance_l0(second, first));
    }

    {
        std::unordered_map<std::string, int> first;
        std::unordered_map<std::string, int> second;

        first["a"]  = 10;
        first["b"]  = 20;
        first["c"]  = 30;
        second["a"] = 20;
        second["b"] = 30;
        second["c"] = 40;

        EXPECT_EQ(3, utility::distance_l0(first, second));
        EXPECT_EQ(3, utility::distance_l0(second, first));
    }

    {
        std::unordered_map<std::string, int> first;
        std::unordered_map<std::string, int> second;

        first["a"]  = 10;
        first["b"]  = 20;
        first["c"]  = 30;
        second["d"] = 10;
        second["e"] = 20;
        second["f"] = 30;

        EXPECT_EQ(6, utility::distance_l0(first, second));
        EXPECT_EQ(6, utility::distance_l0(second, first));
    }
}
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/