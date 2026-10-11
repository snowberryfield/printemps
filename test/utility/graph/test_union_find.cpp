/*****************************************************************************/
// Copyright (c) 2020-2026 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
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
    std::vector<int>               elements = {0, 1, 2, 3, 4, 5, 6};
    utility::graph::UnionFind<int> union_find;
    union_find.setup(elements);

    EXPECT_EQ(7, static_cast<int>(union_find.parents().size()));
    EXPECT_EQ(7, static_cast<int>(union_find.sizes().size()));
    EXPECT_EQ(7, static_cast<int>(union_find.parities().size()));
    EXPECT_EQ(0, union_find.parents().at(0));
    EXPECT_EQ(6, union_find.parents().at(6));
    EXPECT_EQ(1, union_find.sizes().at(0));
    EXPECT_EQ(1, union_find.sizes().at(6));
    EXPECT_EQ(0, union_find.parities().at(0));
    EXPECT_EQ(0, union_find.parities().at(6));

    union_find.initialize();
    EXPECT_TRUE(union_find.parents().empty());
    EXPECT_TRUE(union_find.sizes().empty());
    EXPECT_TRUE(union_find.parities().empty());
}

/*****************************************************************************/
TEST_F(TestUnionFind, setup) {
    /// This test is covered by initialize();
}

/*****************************************************************************/
TEST_F(TestUnionFind, constructor) {
    std::vector<int>               elements = {10, 20, 30};
    utility::graph::UnionFind<int> union_find(elements);

    EXPECT_EQ(3, static_cast<int>(union_find.parents().size()));
    EXPECT_EQ(3, static_cast<int>(union_find.sizes().size()));
    EXPECT_EQ(3, static_cast<int>(union_find.parities().size()));
    EXPECT_EQ(10, union_find.root(10));
    EXPECT_EQ(20, union_find.root(20));
    EXPECT_EQ(30, union_find.root(30));
}

/*****************************************************************************/
TEST_F(TestUnionFind, unite) {
    utility::graph::UnionFind<int> union_find;

    EXPECT_TRUE(union_find.unite(1, 2, 0));
    EXPECT_TRUE(union_find.has_same_root(1, 2));
    EXPECT_EQ(0, union_find.parity_between(1, 2));

    EXPECT_TRUE(union_find.unite(2, 3, 1));
    EXPECT_TRUE(union_find.has_same_root(1, 3));
    EXPECT_EQ(1, union_find.parity_between(1, 3));

    EXPECT_FALSE(union_find.unite(1, 3, 0));

    utility::graph::UnionFind<int> union_find_reversed;
    union_find_reversed.unite(2, 1, 0);
    EXPECT_EQ(union_find.parity_between(1, 2),
              union_find_reversed.parity_between(1, 2));
}

/*****************************************************************************/
TEST_F(TestUnionFind, has_same_root) {
    utility::graph::UnionFind<int> union_find;
    EXPECT_THROW(union_find.has_same_root(1, 2), std::runtime_error);

    union_find.unite(1, 2, 0);
    EXPECT_TRUE(union_find.has_same_root(1, 2));

    union_find.unite(3, 4, 0);
    EXPECT_FALSE(union_find.has_same_root(1, 3));
}

/*****************************************************************************/
TEST_F(TestUnionFind, parity_between) {
    utility::graph::UnionFind<int> union_find;

    union_find.unite(1, 2, 1);
    union_find.unite(2, 3, 1);

    EXPECT_EQ(1, union_find.parity_between(1, 2));
    EXPECT_EQ(1, union_find.parity_between(2, 3));
    EXPECT_EQ(0, union_find.parity_between(1, 3));

    EXPECT_THROW(union_find.parity_between(1, 4), std::runtime_error);
}

/*****************************************************************************/
TEST_F(TestUnionFind, root_with_parity) {
    utility::graph::UnionFind<int> union_find;

    union_find.unite(1, 2, 1);
    union_find.unite(2, 3, 0);

    const auto ROOT_1 = union_find.root_with_parity(1);
    const auto ROOT_2 = union_find.root_with_parity(2);
    const auto ROOT_3 = union_find.root_with_parity(3);

    EXPECT_EQ(ROOT_1.first, ROOT_2.first);
    EXPECT_EQ(ROOT_2.first, ROOT_3.first);

    EXPECT_EQ(1, ROOT_1.second ^ ROOT_2.second);
    EXPECT_EQ(0, ROOT_2.second ^ ROOT_3.second);
}

/*****************************************************************************/
TEST_F(TestUnionFind, root) {
    utility::graph::UnionFind<int> union_find;

    union_find.unite(1, 2, 0);
    union_find.unite(2, 3, 0);

    EXPECT_EQ(union_find.root(1), union_find.root(2));
    EXPECT_EQ(union_find.root(2), union_find.root(3));
}

/*****************************************************************************/
TEST_F(TestUnionFind, groups) {
    utility::graph::UnionFind<int> union_find;

    union_find.unite(1, 2, 1);
    union_find.unite(2, 3, 0);
    union_find.unite(4, 5, 0);

    const auto GROUPS = union_find.groups();
    EXPECT_EQ(2, static_cast<int>(GROUPS.size()));

    int group_with_3_members = -1;
    int group_with_2_members = -1;
    for (int i = 0; i < static_cast<int>(GROUPS.size()); i++) {
        if (GROUPS[i].members.size() == 3) {
            group_with_3_members = i;
        } else if (GROUPS[i].members.size() == 2) {
            group_with_2_members = i;
        }
    }

    EXPECT_NE(-1, group_with_3_members);
    EXPECT_NE(-1, group_with_2_members);
}

/*****************************************************************************/
TEST_F(TestUnionFind, parents) {
    /// This test is covered by initialize();
}

/*****************************************************************************/
TEST_F(TestUnionFind, sizes) {
    /// This test is covered by initialize();
}

/*****************************************************************************/
TEST_F(TestUnionFind, parities) {
    /// This test is covered by initialize();
}
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/
