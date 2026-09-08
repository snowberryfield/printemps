/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
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
    std::vector<int>        elements = {0, 1, 2, 3, 4, 5, 6};
    utility::UnionFind<int> uf;
    uf.setup(elements);

    EXPECT_EQ(7, static_cast<int>(uf.parents().size()));
    EXPECT_EQ(7, static_cast<int>(uf.sizes().size()));
    EXPECT_EQ(7, static_cast<int>(uf.parities().size()));
    EXPECT_EQ(0, uf.parents().at(0));
    EXPECT_EQ(6, uf.parents().at(6));
    EXPECT_EQ(1, uf.sizes().at(0));
    EXPECT_EQ(1, uf.sizes().at(6));
    EXPECT_EQ(0, uf.parities().at(0));
    EXPECT_EQ(0, uf.parities().at(6));

    uf.initialize();
    EXPECT_TRUE(uf.parents().empty());
    EXPECT_TRUE(uf.sizes().empty());
    EXPECT_TRUE(uf.parities().empty());
}

/*****************************************************************************/
TEST_F(TestUnionFind, setup) {
    /// This test is covered by initialize();
}

/*****************************************************************************/
TEST_F(TestUnionFind, unite) {
    utility::UnionFind<int> uf;

    EXPECT_TRUE(uf.unite(1, 2, 0));
    EXPECT_TRUE(uf.has_same_root(1, 2));
    EXPECT_EQ(0, uf.parity_between(1, 2));

    EXPECT_TRUE(uf.unite(2, 3, 1));
    EXPECT_TRUE(uf.has_same_root(1, 3));
    EXPECT_EQ(1, uf.parity_between(1, 3));

    EXPECT_FALSE(uf.unite(1, 3, 0));

    utility::UnionFind<int> uf2;
    uf2.unite(2, 1, 0);
    EXPECT_EQ(uf.parity_between(1, 2), uf2.parity_between(1, 2));
}

/*****************************************************************************/
TEST_F(TestUnionFind, has_same_root) {
    utility::UnionFind<int> uf;
    EXPECT_THROW(uf.has_same_root(1, 2), std::runtime_error);

    uf.unite(1, 2, 0);
    EXPECT_TRUE(uf.has_same_root(1, 2));

    uf.unite(3, 4, 0);
    EXPECT_FALSE(uf.has_same_root(1, 3));
}

/*****************************************************************************/
TEST_F(TestUnionFind, parity_between) {
    utility::UnionFind<int> uf;

    uf.unite(1, 2, 1);
    uf.unite(2, 3, 1);

    EXPECT_EQ(1, uf.parity_between(1, 2));
    EXPECT_EQ(1, uf.parity_between(2, 3));
    EXPECT_EQ(0, uf.parity_between(1, 3));

    EXPECT_THROW(uf.parity_between(1, 4), std::runtime_error);
}

/*****************************************************************************/
TEST_F(TestUnionFind, root_with_parity) {
    utility::UnionFind<int> uf;

    uf.unite(1, 2, 1);
    uf.unite(2, 3, 0);

    auto [r1, p1] = uf.root_with_parity(1);
    auto [r3, p3] = uf.root_with_parity(3);

    EXPECT_EQ(r1, r3);
    EXPECT_EQ(p1 ^ p3, uf.parity_between(1, 3));
}

/*****************************************************************************/
TEST_F(TestUnionFind, groups) {
    utility::UnionFind<int> uf;

    // Group A (parity chain)
    uf.unite(1, 2, 0);
    uf.unite(2, 3, 1);
    uf.unite(3, 4, 0);

    // Group B
    uf.unite(10, 11, 1);
    uf.unite(11, 12, 0);

    // Group C (single node)
    uf.unite(100, 100, 0);

    auto groups = uf.groups();
    EXPECT_EQ(3, static_cast<int>(groups.size()));

    for (const auto& group : groups) {
        for (const auto& member : group.members) {
            EXPECT_TRUE(uf.has_same_root(group.root, member.first));
        }

        for (const auto& member : group.members) {
            if (member.first == group.root) {
                EXPECT_EQ(0, member.second);
            }
        }

        for (size_t i = 0; i < group.members.size(); i++) {
            for (size_t j = 0; j < group.members.size(); j++) {
                const auto& a = group.members[i];
                const auto& b = group.members[j];

                EXPECT_EQ(a.second ^ b.second,
                          uf.parity_between(a.first, b.first));
            }
        }
    }

    for (size_t i = 0; i < groups.size(); i++) {
        for (size_t j = i + 1; j < groups.size(); j++) {
            EXPECT_FALSE(uf.has_same_root(groups[i].root, groups[j].root));
        }
    }
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