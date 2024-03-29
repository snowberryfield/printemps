/*****************************************************************************/
// Copyright (c) 2020-2023 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <string>

#include <printemps.h>

namespace {
using namespace printemps;
/*****************************************************************************/
class TestUnionFind : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestUnionFind, initialize) {
    std::unordered_set<int> elements = {0, 1, 2, 3, 4, 5, 6};
    utility::UnionFind<int> uf;
    uf.setup(elements);

    EXPECT_EQ(7, static_cast<int>(uf.parents().size()));
    EXPECT_EQ(7, static_cast<int>(uf.sizes().size()));
    EXPECT_EQ(0, uf.parents().at(0));
    EXPECT_EQ(6, uf.parents().at(6));
    EXPECT_EQ(1, uf.sizes().at(0));
    EXPECT_EQ(1, uf.sizes().at(6));

    uf.initialize();
    EXPECT_TRUE(uf.parents().empty());
    EXPECT_TRUE(uf.sizes().empty());
}

/*****************************************************************************/
TEST_F(TestUnionFind, setup) {
    /// This method is tested in initialize();
}

/*****************************************************************************/
TEST_F(TestUnionFind, is_same) {
    std::unordered_set<int> elements = {0, 1, 2, 3, 4, 5, 6};
    utility::UnionFind<int> uf(elements);
    uf.unite(1, 2);
    uf.unite(2, 3);
    uf.unite(5, 6);

    EXPECT_TRUE(uf.is_same(1, 2));
    EXPECT_TRUE(uf.is_same(1, 3));
    EXPECT_TRUE(uf.is_same(2, 3));
    EXPECT_TRUE(uf.is_same(5, 6));

    EXPECT_FALSE(uf.is_same(0, 1));
    EXPECT_FALSE(uf.is_same(1, 5));
    EXPECT_FALSE(uf.is_same(1, 6));
    EXPECT_FALSE(uf.is_same(4, 6));

    uf.unite(1, 6);
    EXPECT_FALSE(uf.is_same(0, 1));
    EXPECT_TRUE(uf.is_same(1, 5));
    EXPECT_TRUE(uf.is_same(1, 6));
    EXPECT_FALSE(uf.is_same(4, 6));

    uf.unite(0, 6);
    EXPECT_TRUE(uf.is_same(0, 1));
    EXPECT_TRUE(uf.is_same(1, 5));
    EXPECT_TRUE(uf.is_same(1, 6));
    EXPECT_FALSE(uf.is_same(4, 6));

    uf.unite(0, 4);
    EXPECT_TRUE(uf.is_same(0, 1));
    EXPECT_TRUE(uf.is_same(1, 5));
    EXPECT_TRUE(uf.is_same(1, 6));
    EXPECT_TRUE(uf.is_same(4, 6));
}

/*****************************************************************************/
TEST_F(TestUnionFind, unite) {
    /// This method is tested in is_same();
}

/*****************************************************************************/
TEST_F(TestUnionFind, sizes) {
    /// This method is tested in is_same();
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/