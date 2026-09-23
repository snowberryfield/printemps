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

    const auto TOPOLOGICAL_ORDER = sorter.sort();
    EXPECT_FALSE(sorter.has_cycle());
    EXPECT_EQ(3, static_cast<int>(TOPOLOGICAL_ORDER.size()));

    EXPECT_EQ(0, TOPOLOGICAL_ORDER[0]);
    EXPECT_EQ(1, TOPOLOGICAL_ORDER[1]);
    EXPECT_EQ(2, TOPOLOGICAL_ORDER[2]);
}

/*****************************************************************************/
TEST_F(TestTopologicalSorter, diamond_dag) {
    utility::TopologicalSorter<int> sorter;
    // 0 -> 1, 0 -> 2, 1 -> 3, 2 -> 3
    sorter.add_edge(0, 1);
    sorter.add_edge(0, 2);
    sorter.add_edge(1, 3);
    sorter.add_edge(2, 3);

    const auto TOPOLOGICAL_ORDER = sorter.sort();
    EXPECT_FALSE(sorter.has_cycle());
    EXPECT_EQ(4, static_cast<int>(TOPOLOGICAL_ORDER.size()));

    // Verify topological ordering constraints:
    // index(0) < index(1), index(0) < index(2)
    // index(1) < index(3), index(2) < index(3)
    const auto POSITION_0 =
        std::find(TOPOLOGICAL_ORDER.begin(), TOPOLOGICAL_ORDER.end(), 0);
    const auto POSITION_1 =
        std::find(TOPOLOGICAL_ORDER.begin(), TOPOLOGICAL_ORDER.end(), 1);
    const auto POSITION_2 =
        std::find(TOPOLOGICAL_ORDER.begin(), TOPOLOGICAL_ORDER.end(), 2);
    const auto POSITION_3 =
        std::find(TOPOLOGICAL_ORDER.begin(), TOPOLOGICAL_ORDER.end(), 3);

    EXPECT_TRUE(POSITION_0 < POSITION_1);
    EXPECT_TRUE(POSITION_0 < POSITION_2);
    EXPECT_TRUE(POSITION_1 < POSITION_3);
    EXPECT_TRUE(POSITION_2 < POSITION_3);
}

/*****************************************************************************/
TEST_F(TestTopologicalSorter, disconnected_dag) {
    utility::TopologicalSorter<int> sorter;
    // Disconnected components: 0 -> 1, 2 -> 3, and isolated node 4
    sorter.add_edge(0, 1);
    sorter.add_edge(2, 3);
    sorter.add_node(4);

    const auto TOPOLOGICAL_ORDER = sorter.sort();
    EXPECT_FALSE(sorter.has_cycle());
    EXPECT_EQ(5, static_cast<int>(TOPOLOGICAL_ORDER.size()));

    const auto POSITION_0 =
        std::find(TOPOLOGICAL_ORDER.begin(), TOPOLOGICAL_ORDER.end(), 0);
    const auto POSITION_1 =
        std::find(TOPOLOGICAL_ORDER.begin(), TOPOLOGICAL_ORDER.end(), 1);
    const auto POSITION_2 =
        std::find(TOPOLOGICAL_ORDER.begin(), TOPOLOGICAL_ORDER.end(), 2);
    const auto POSITION_3 =
        std::find(TOPOLOGICAL_ORDER.begin(), TOPOLOGICAL_ORDER.end(), 3);

    EXPECT_TRUE(POSITION_0 < POSITION_1);
    EXPECT_TRUE(POSITION_2 < POSITION_3);
}

/*****************************************************************************/
TEST_F(TestTopologicalSorter, cycle_detection) {
    {
        utility::TopologicalSorter<int> sorter;
        // Cycle: 0 -> 1 -> 2 -> 0
        sorter.add_edge(0, 1);
        sorter.add_edge(1, 2);
        sorter.add_edge(2, 0);

        const auto TOPOLOGICAL_ORDER = sorter.sort();
        EXPECT_TRUE(sorter.has_cycle());
        EXPECT_NE(3, static_cast<int>(TOPOLOGICAL_ORDER.size()));
    }
    {
        utility::TopologicalSorter<int> sorter;
        // Self-loop: 0 -> 0
        sorter.add_edge(0, 0);

        const auto TOPOLOGICAL_ORDER = sorter.sort();
        EXPECT_TRUE(sorter.has_cycle());
        EXPECT_NE(1, static_cast<int>(TOPOLOGICAL_ORDER.size()));
    }
}

/*****************************************************************************/
TEST_F(TestTopologicalSorter, successors) {
    utility::TopologicalSorter<int> sorter;
    sorter.add_edge(0, 1);
    sorter.add_edge(0, 2);
    sorter.add_edge(1, 3);

    const auto SUCCESSORS_0 = sorter.successors(0);
    EXPECT_EQ(2, static_cast<int>(SUCCESSORS_0.size()));
    EXPECT_TRUE(std::find(SUCCESSORS_0.begin(), SUCCESSORS_0.end(), 1) !=
                SUCCESSORS_0.end());
    EXPECT_TRUE(std::find(SUCCESSORS_0.begin(), SUCCESSORS_0.end(), 2) !=
                SUCCESSORS_0.end());

    const auto SUCCESSORS_1 = sorter.successors(1);
    EXPECT_EQ(1, static_cast<int>(SUCCESSORS_1.size()));
    EXPECT_EQ(3, SUCCESSORS_1[0]);

    const auto SUCCESSORS_3 = sorter.successors(3);
    EXPECT_TRUE(SUCCESSORS_3.empty());

    // Non-existent node returns empty
    const auto SUCCESSORS_UNKNOWN = sorter.successors(999);
    EXPECT_TRUE(SUCCESSORS_UNKNOWN.empty());
}

/*****************************************************************************/
TEST_F(TestTopologicalSorter, pointer_type) {
    int a = 10, b = 20, c = 30;
    utility::TopologicalSorter<int *> sorter;
    sorter.add_edge(&a, &b);
    sorter.add_edge(&b, &c);

    const auto TOPOLOGICAL_ORDER = sorter.sort();
    EXPECT_FALSE(sorter.has_cycle());
    EXPECT_EQ(3, static_cast<int>(TOPOLOGICAL_ORDER.size()));
    EXPECT_EQ(&a, TOPOLOGICAL_ORDER[0]);
    EXPECT_EQ(&b, TOPOLOGICAL_ORDER[1]);
    EXPECT_EQ(&c, TOPOLOGICAL_ORDER[2]);

    const auto SUCCESSORS_A = sorter.successors(&a);
    EXPECT_EQ(1, static_cast<int>(SUCCESSORS_A.size()));
    EXPECT_EQ(&b, SUCCESSORS_A[0]);
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/
