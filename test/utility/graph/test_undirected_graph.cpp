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
class TestUndirectedGraph : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestUndirectedGraph, initialize) {
    utility::graph::UndirectedGraph<int> graph;
    graph.add_edge(1, 2, 3.0);

    EXPECT_EQ(2, graph.number_of_nodes());
    EXPECT_EQ(1, graph.number_of_edges());
    EXPECT_FALSE(graph.empty());

    graph.initialize();
    EXPECT_EQ(0, graph.number_of_nodes());
    EXPECT_EQ(0, graph.number_of_edges());
    EXPECT_TRUE(graph.empty());
    EXPECT_FALSE(graph.has_node(1));
}

/*****************************************************************************/
TEST_F(TestUndirectedGraph, constructor) {
    std::vector<int>                     nodes = {10, 20, 30};
    utility::graph::UndirectedGraph<int> graph(nodes);

    EXPECT_EQ(3, graph.number_of_nodes());
    EXPECT_EQ(0, graph.number_of_edges());
    EXPECT_TRUE(graph.has_node(10));
    EXPECT_TRUE(graph.has_node(20));
    EXPECT_TRUE(graph.has_node(30));
    EXPECT_FALSE(graph.has_node(40));
}

/*****************************************************************************/
TEST_F(TestUndirectedGraph, number_of_nodes) {
    utility::graph::UndirectedGraph<int> graph;
    EXPECT_EQ(0, graph.number_of_nodes());

    graph.add_node(1);
    EXPECT_EQ(1, graph.number_of_nodes());

    graph.add_node(2);
    EXPECT_EQ(2, graph.number_of_nodes());
}

/*****************************************************************************/
TEST_F(TestUndirectedGraph, number_of_edges) {
    utility::graph::UndirectedGraph<int> graph;
    EXPECT_EQ(0, graph.number_of_edges());

    graph.add_edge(1, 2);
    EXPECT_EQ(1, graph.number_of_edges());

    graph.add_edge(2, 3);
    EXPECT_EQ(2, graph.number_of_edges());
}

/*****************************************************************************/
TEST_F(TestUndirectedGraph, size) {
    utility::graph::UndirectedGraph<int> graph;
    EXPECT_EQ(0, graph.size());

    graph.add_node(1);
    EXPECT_EQ(1, graph.size());
}

/*****************************************************************************/
TEST_F(TestUndirectedGraph, empty) {
    utility::graph::UndirectedGraph<int> graph;
    EXPECT_TRUE(graph.empty());

    graph.add_node(1);
    EXPECT_FALSE(graph.empty());
}

/*****************************************************************************/
TEST_F(TestUndirectedGraph, has_node) {
    utility::graph::UndirectedGraph<std::string> graph;
    EXPECT_FALSE(graph.has_node("a"));

    graph.add_node("a");
    EXPECT_TRUE(graph.has_node("a"));
    EXPECT_FALSE(graph.has_node("b"));
}

/*****************************************************************************/
TEST_F(TestUndirectedGraph, add_node) {
    utility::graph::UndirectedGraph<int> graph;
    const int INDEX_0 = graph.add_node(100, 2.5);
    EXPECT_EQ(0, INDEX_0);
    EXPECT_FLOAT_EQ(2.5, graph.node_weight(100));

    // Re-adding existing node keeps existing weight and returns existing index
    const int INDEX_1 = graph.add_node(100, 5.0);
    EXPECT_EQ(0, INDEX_1);
    EXPECT_FLOAT_EQ(2.5, graph.node_weight(100));

    // set_node_weight updates weight
    graph.set_node_weight(100, 5.0);
    EXPECT_FLOAT_EQ(5.0, graph.node_weight(100));
}

/*****************************************************************************/
TEST_F(TestUndirectedGraph, add_nodes) {
    utility::graph::UndirectedGraph<int> graph;
    graph.add_nodes({1, 2, 3});

    EXPECT_EQ(3, graph.number_of_nodes());
    EXPECT_TRUE(graph.has_node(1));
    EXPECT_TRUE(graph.has_node(2));
    EXPECT_TRUE(graph.has_node(3));
}

/*****************************************************************************/
TEST_F(TestUndirectedGraph, add_edge) {
    utility::graph::UndirectedGraph<int> graph;
    const int EDGE_INDEX = graph.add_edge(1, 2, 4.5);

    EXPECT_EQ(0, EDGE_INDEX);
    EXPECT_EQ(2, graph.number_of_nodes());
    EXPECT_EQ(1, graph.number_of_edges());
    EXPECT_TRUE(graph.has_edge(1, 2));
    EXPECT_TRUE(graph.has_edge(2, 1));
}

/*****************************************************************************/
TEST_F(TestUndirectedGraph, has_edge) {
    utility::graph::UndirectedGraph<int> graph;
    graph.add_edge(1, 2);

    EXPECT_TRUE(graph.has_edge(1, 2));
    EXPECT_TRUE(graph.has_edge(2, 1));
    EXPECT_FALSE(graph.has_edge(1, 3));
    EXPECT_FALSE(graph.has_edge(3, 4));
}

/*****************************************************************************/
TEST_F(TestUndirectedGraph, degree) {
    utility::graph::UndirectedGraph<int> graph;
    graph.add_edge(1, 2);
    graph.add_edge(1, 3);

    EXPECT_EQ(2, graph.degree(1));
    EXPECT_EQ(1, graph.degree(2));
    EXPECT_EQ(1, graph.degree(3));
    EXPECT_EQ(0, graph.degree(4));  // Non-existent node
}

/*****************************************************************************/
TEST_F(TestUndirectedGraph, neighbors) {
    utility::graph::UndirectedGraph<int> graph;
    graph.add_edge(1, 2);
    graph.add_edge(1, 3);

    const auto NEIGHBORS = graph.neighbors(1);
    EXPECT_EQ(2, static_cast<int>(NEIGHBORS.size()));
    EXPECT_TRUE(std::find(NEIGHBORS.begin(), NEIGHBORS.end(), 2) !=
                NEIGHBORS.end());
    EXPECT_TRUE(std::find(NEIGHBORS.begin(), NEIGHBORS.end(), 3) !=
                NEIGHBORS.end());

    EXPECT_TRUE(graph.neighbors(99).empty());  // Non-existent node
}

/*****************************************************************************/
TEST_F(TestUndirectedGraph, node_weight) {
    utility::graph::UndirectedGraph<int> graph;
    graph.add_node(1, 3.14);

    EXPECT_FLOAT_EQ(3.14, graph.node_weight(1));
    EXPECT_FLOAT_EQ(0.0, graph.node_weight(2));  // Non-existent node
}

/*****************************************************************************/
TEST_F(TestUndirectedGraph, nodes) {
    utility::graph::UndirectedGraph<int> graph;
    graph.add_node(10);
    graph.add_node(20);

    const auto &NODES = graph.nodes();
    EXPECT_EQ(2, static_cast<int>(NODES.size()));
    EXPECT_EQ(10, NODES[0]);
    EXPECT_EQ(20, NODES[1]);
}

/*****************************************************************************/
TEST_F(TestUndirectedGraph, edges) {
    utility::graph::UndirectedGraph<int> graph;
    graph.add_edge(1, 2, 10.0);

    const auto &EDGES = graph.edges();
    EXPECT_EQ(1, static_cast<int>(EDGES.size()));
    EXPECT_EQ(1, EDGES[0].first);
    EXPECT_EQ(2, EDGES[0].second);
    EXPECT_FLOAT_EQ(10.0, EDGES[0].weight);
}

/*****************************************************************************/
TEST_F(TestUndirectedGraph, minimum_spanning_tree) {
    utility::graph::UndirectedGraph<int> graph;
    // Triangle: (0, 1, 1.0), (1, 2, 2.0), (0, 2, 3.0)
    graph.add_edge(0, 1, 1.0);
    graph.add_edge(1, 2, 2.0);
    graph.add_edge(0, 2, 3.0);

    const auto MST = graph.minimum_spanning_tree();
    EXPECT_EQ(3, MST.number_of_nodes());
    EXPECT_EQ(2, MST.number_of_edges());
    EXPECT_TRUE(MST.has_edge(0, 1));
    EXPECT_TRUE(MST.has_edge(1, 2));
    EXPECT_FALSE(MST.has_edge(0, 2));

    double total_weight = 0.0;
    for (const auto &edge : MST.edges()) {
        total_weight += edge.weight;
    }
    EXPECT_FLOAT_EQ(3.0, total_weight);
}

/*****************************************************************************/
TEST_F(TestUndirectedGraph, connected_components) {
    utility::graph::UndirectedGraph<int> graph;
    // Component 1: {0, 1, 2}
    graph.add_edge(0, 1);
    graph.add_edge(1, 2);
    // Component 2: {3, 4}
    graph.add_edge(3, 4);
    // Isolated node: {5}
    graph.add_node(5);

    const auto COMPONENTS = graph.connected_components();
    EXPECT_EQ(3, static_cast<int>(COMPONENTS.size()));

    int size_3_count = 0;
    int size_2_count = 0;
    int size_1_count = 0;
    for (const auto &component : COMPONENTS) {
        if (component.size() == 3) {
            size_3_count++;
        } else if (component.size() == 2) {
            size_2_count++;
        } else if (component.size() == 1) {
            size_1_count++;
        }
    }
    EXPECT_EQ(1, size_3_count);
    EXPECT_EQ(1, size_2_count);
    EXPECT_EQ(1, size_1_count);
}

/*****************************************************************************/
TEST_F(TestUndirectedGraph, has_cycle) {
    utility::graph::UndirectedGraph<int> tree_graph;
    tree_graph.add_edge(0, 1);
    tree_graph.add_edge(1, 2);
    tree_graph.add_edge(2, 3);
    EXPECT_FALSE(tree_graph.has_cycle());

    utility::graph::UndirectedGraph<int> cyclic_graph = tree_graph;
    cyclic_graph.add_edge(3, 0);
    EXPECT_TRUE(cyclic_graph.has_cycle());
}

/*****************************************************************************/
TEST_F(TestUndirectedGraph, is_bipartite) {
    // Even cycle: bipartite
    utility::graph::UndirectedGraph<int> c4;
    c4.add_edge(0, 1);
    c4.add_edge(1, 2);
    c4.add_edge(2, 3);
    c4.add_edge(3, 0);
    EXPECT_TRUE(c4.is_bipartite());

    // Odd cycle: not bipartite
    utility::graph::UndirectedGraph<int> c3;
    c3.add_edge(0, 1);
    c3.add_edge(1, 2);
    c3.add_edge(2, 0);
    EXPECT_FALSE(c3.is_bipartite());
}

/*****************************************************************************/
TEST_F(TestUndirectedGraph, set_node_weight) {
    utility::graph::UndirectedGraph<int> graph;
    graph.add_node(1, 5.0);
    EXPECT_FLOAT_EQ(5.0, graph.node_weight(1));

    graph.set_node_weight(1, 15.0);
    EXPECT_FLOAT_EQ(15.0, graph.node_weight(1));
}

/*****************************************************************************/
TEST_F(TestUndirectedGraph, node_index) {
    utility::graph::UndirectedGraph<std::string> graph;
    graph.add_node("a");
    graph.add_node("b");

    EXPECT_EQ(0, graph.node_index("a"));
    EXPECT_EQ(1, graph.node_index("b"));
    EXPECT_EQ(-1, graph.node_index("c"));
}

/*****************************************************************************/
TEST_F(TestUndirectedGraph, adjacent_nodes) {
    utility::graph::UndirectedGraph<int> graph;
    graph.add_edge(1, 2, 4.0);

    const auto &ADJ = graph.adjacent_nodes(1);
    EXPECT_EQ(1, static_cast<int>(ADJ.size()));
    EXPECT_EQ(graph.node_index(2), ADJ[0].node_index);
    EXPECT_FLOAT_EQ(4.0, ADJ[0].weight);

    EXPECT_TRUE(graph.adjacent_nodes(99).empty());
}
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/
