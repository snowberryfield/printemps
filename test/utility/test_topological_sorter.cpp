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
class TestTopologicalSorter : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestTopologicalSorter, initialize) {
    utility::TopologicalSorter<int> sorter;
    sorter.add_edge(1, 2);
    EXPECT_EQ(2, sorter.size());
    EXPECT_FALSE(sorter.empty());

    sorter.initialize();
    EXPECT_EQ(0, sorter.size());
    EXPECT_TRUE(sorter.empty());
    EXPECT_FALSE(sorter.has_node(1));
    EXPECT_FALSE(sorter.has_cycle());
}

/*****************************************************************************/
TEST_F(TestTopologicalSorter, constructor) {
    std::vector<int>                nodes = {10, 20, 30};
    utility::TopologicalSorter<int> sorter(nodes);

    EXPECT_EQ(3, sorter.size());
    EXPECT_TRUE(sorter.has_node(10));
    EXPECT_TRUE(sorter.has_node(20));
    EXPECT_TRUE(sorter.has_node(30));
    EXPECT_FALSE(sorter.has_node(40));
}

/*****************************************************************************/
TEST_F(TestTopologicalSorter, add_node_and_has_node) {
    utility::TopologicalSorter<std::string> sorter;
    EXPECT_EQ(0, sorter.add_node("a"));
    EXPECT_EQ(1, sorter.add_node("b"));
    // Duplicate node addition should return existing ID
    EXPECT_EQ(0, sorter.add_node("a"));
    EXPECT_EQ(2, sorter.size());

    EXPECT_TRUE(sorter.has_node("a"));
    EXPECT_TRUE(sorter.has_node("b"));
    EXPECT_FALSE(sorter.has_node("c"));
}

/*****************************************************************************/
TEST_F(TestTopologicalSorter, simple_chain) {
    utility::TopologicalSorter<int> sorter;
    // 0 -> 1 -> 2
    sorter.add_edge(0, 1);
    sorter.add_edge(1, 2);

    auto order = sorter.sort();
    EXPECT_FALSE(sorter.has_cycle());
    EXPECT_EQ(3, static_cast<int>(order.size()));

    EXPECT_EQ(0, order[0]);
    EXPECT_EQ(1, order[1]);
    EXPECT_EQ(2, order[2]);
}

/*****************************************************************************/
TEST_F(TestTopologicalSorter, diamond_dag) {
    utility::TopologicalSorter<int> sorter;
    // 0 -> 1, 0 -> 2, 1 -> 3, 2 -> 3
    sorter.add_edge(0, 1);
    sorter.add_edge(0, 2);
    sorter.add_edge(1, 3);
    sorter.add_edge(2, 3);

    auto order = sorter.sort();
    EXPECT_FALSE(sorter.has_cycle());
    EXPECT_EQ(4, static_cast<int>(order.size()));

    // Verify topological ordering constraints:
    // index(0) < index(1), index(0) < index(2)
    // index(1) < index(3), index(2) < index(3)
    auto pos_0 = std::find(order.begin(), order.end(), 0);
    auto pos_1 = std::find(order.begin(), order.end(), 1);
    auto pos_2 = std::find(order.begin(), order.end(), 2);
    auto pos_3 = std::find(order.begin(), order.end(), 3);

    EXPECT_TRUE(pos_0 < pos_1);
    EXPECT_TRUE(pos_0 < pos_2);
    EXPECT_TRUE(pos_1 < pos_3);
    EXPECT_TRUE(pos_2 < pos_3);
}

/*****************************************************************************/
TEST_F(TestTopologicalSorter, disconnected_dag) {
    utility::TopologicalSorter<int> sorter;
    // Disconnected components: 0 -> 1, 2 -> 3, and isolated node 4
    sorter.add_edge(0, 1);
    sorter.add_edge(2, 3);
    sorter.add_node(4);

    auto order = sorter.sort();
    EXPECT_FALSE(sorter.has_cycle());
    EXPECT_EQ(5, static_cast<int>(order.size()));

    auto pos_0 = std::find(order.begin(), order.end(), 0);
    auto pos_1 = std::find(order.begin(), order.end(), 1);
    auto pos_2 = std::find(order.begin(), order.end(), 2);
    auto pos_3 = std::find(order.begin(), order.end(), 3);

    EXPECT_TRUE(pos_0 < pos_1);
    EXPECT_TRUE(pos_2 < pos_3);
}

/*****************************************************************************/
TEST_F(TestTopologicalSorter, cycle_detection) {
    {
        utility::TopologicalSorter<int> sorter;
        // Cycle: 0 -> 1 -> 2 -> 0
        sorter.add_edge(0, 1);
        sorter.add_edge(1, 2);
        sorter.add_edge(2, 0);

        auto order = sorter.sort();
        EXPECT_TRUE(sorter.has_cycle());
        EXPECT_NE(3, static_cast<int>(order.size()));
    }
    {
        utility::TopologicalSorter<int> sorter;
        // Self-loop: 0 -> 0
        sorter.add_edge(0, 0);

        auto order = sorter.sort();
        EXPECT_TRUE(sorter.has_cycle());
        EXPECT_NE(1, static_cast<int>(order.size()));
    }
}

/*****************************************************************************/
TEST_F(TestTopologicalSorter, successors) {
    utility::TopologicalSorter<int> sorter;
    sorter.add_edge(0, 1);
    sorter.add_edge(0, 2);
    sorter.add_edge(1, 3);

    auto succ_0 = sorter.successors(0);
    EXPECT_EQ(2, static_cast<int>(succ_0.size()));
    EXPECT_TRUE(std::find(succ_0.begin(), succ_0.end(), 1) != succ_0.end());
    EXPECT_TRUE(std::find(succ_0.begin(), succ_0.end(), 2) != succ_0.end());

    auto succ_1 = sorter.successors(1);
    EXPECT_EQ(1, static_cast<int>(succ_1.size()));
    EXPECT_EQ(3, succ_1[0]);

    auto succ_3 = sorter.successors(3);
    EXPECT_TRUE(succ_3.empty());

    // Non-existent node returns empty
    auto succ_unknown = sorter.successors(999);
    EXPECT_TRUE(succ_unknown.empty());
}

/*****************************************************************************/
TEST_F(TestTopologicalSorter, pointer_type) {
    int a = 10, b = 20, c = 30;
    utility::TopologicalSorter<int *> sorter;
    sorter.add_edge(&a, &b);
    sorter.add_edge(&b, &c);

    auto order = sorter.sort();
    EXPECT_FALSE(sorter.has_cycle());
    EXPECT_EQ(3, static_cast<int>(order.size()));
    EXPECT_EQ(&a, order[0]);
    EXPECT_EQ(&b, order[1]);
    EXPECT_EQ(&c, order[2]);

    auto succ_a = sorter.successors(&a);
    EXPECT_EQ(1, static_cast<int>(succ_a.size()));
    EXPECT_EQ(&b, succ_a[0]);
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/
