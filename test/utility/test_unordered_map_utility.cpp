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

    auto vector_pair_int = printemps::utility::to_vector_pair(umap_int);
    auto vector_pair_str = printemps::utility::to_vector_pair(umap_str);

    EXPECT_EQ(2, static_cast<int>(vector_pair_int.first.size()));
    EXPECT_EQ(2, static_cast<int>(vector_pair_int.second.size()));

    EXPECT_EQ(3, static_cast<int>(vector_pair_str.first.size()));
    EXPECT_EQ(3, static_cast<int>(vector_pair_str.second.size()));
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/