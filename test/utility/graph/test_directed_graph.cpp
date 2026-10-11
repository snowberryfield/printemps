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
class TestDirectedGraph : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestDirectedGraph, initialize) {
    utility::graph::DirectedGraph<int> graph;
    graph.add_edge(1, 2, 3.0);

    EXPECT_EQ(2, graph.number_of_nodes());
    EXPECT_EQ(1, graph.number_of_edges());
    EXPECT_FALSE(graph.empty());

    graph.initialize();
    EXPECT_EQ(0, graph.number_of_nodes());
    EXPECT_EQ(0, graph.number_of_edges());
    EXPECT_TRUE(graph.empty());
    EXPECT_FALSE(graph.has_node(1));
    EXPECT_FALSE(graph.has_cycle());
}

/*****************************************************************************/
TEST_F(TestDirectedGraph, constructor) {
    std::vector<int>                   nodes = {10, 20, 30};
    utility::graph::DirectedGraph<int> graph(nodes);

    EXPECT_EQ(3, graph.number_of_nodes());
    EXPECT_EQ(0, graph.number_of_edges());
    EXPECT_TRUE(graph.has_node(10));
    EXPECT_TRUE(graph.has_node(20));
    EXPECT_TRUE(graph.has_node(30));
    EXPECT_FALSE(graph.has_node(40));
}

/*****************************************************************************/
TEST_F(TestDirectedGraph, number_of_nodes) {
    utility::graph::DirectedGraph<int> graph;
    EXPECT_EQ(0, graph.number_of_nodes());

    graph.add_node(1);
    EXPECT_EQ(1, graph.number_of_nodes());

    graph.add_node(2);
    EXPECT_EQ(2, graph.number_of_nodes());
}

/*****************************************************************************/
TEST_F(TestDirectedGraph, number_of_edges) {
    utility::graph::DirectedGraph<int> graph;
    EXPECT_EQ(0, graph.number_of_edges());

    graph.add_edge(1, 2);
    EXPECT_EQ(1, graph.number_of_edges());

    graph.add_edge(2, 3);
    EXPECT_EQ(2, graph.number_of_edges());
}

/*****************************************************************************/
TEST_F(TestDirectedGraph, size) {
    utility::graph::DirectedGraph<int> graph;
    EXPECT_EQ(0, graph.size());

    graph.add_node(1);
    EXPECT_EQ(1, graph.size());
}

/*****************************************************************************/
TEST_F(TestDirectedGraph, empty) {
    utility::graph::DirectedGraph<int> graph;
    EXPECT_TRUE(graph.empty());

    graph.add_node(1);
    EXPECT_FALSE(graph.empty());
}

/*****************************************************************************/
TEST_F(TestDirectedGraph, has_node) {
    utility::graph::DirectedGraph<std::string> graph;
    EXPECT_FALSE(graph.has_node("a"));

    graph.add_node("a");
    EXPECT_TRUE(graph.has_node("a"));
    EXPECT_FALSE(graph.has_node("b"));
}

/*****************************************************************************/
TEST_F(TestDirectedGraph, add_node) {
    utility::graph::DirectedGraph<int> graph;
    const int INDEX_0 = graph.add_node(100);
    EXPECT_EQ(0, INDEX_0);

    // Duplicate node addition should return existing index
    const int INDEX_1 = graph.add_node(100);
    EXPECT_EQ(0, INDEX_1);
    EXPECT_EQ(1, graph.size());
}

/*****************************************************************************/
TEST_F(TestDirectedGraph, add_nodes) {
    utility::graph::DirectedGraph<int> graph;
    graph.add_nodes({1, 2, 3});

    EXPECT_EQ(3, graph.number_of_nodes());
    EXPECT_TRUE(graph.has_node(1));
    EXPECT_TRUE(graph.has_node(2));
    EXPECT_TRUE(graph.has_node(3));
}

/*****************************************************************************/
TEST_F(TestDirectedGraph, add_edge) {
    utility::graph::DirectedGraph<int> graph;
    const int EDGE_INDEX = graph.add_edge(1, 2, 4.5);

    EXPECT_EQ(0, EDGE_INDEX);
    EXPECT_EQ(2, graph.number_of_nodes());
    EXPECT_EQ(1, graph.number_of_edges());
    EXPECT_TRUE(graph.has_edge(1, 2));
    EXPECT_FALSE(graph.has_edge(2, 1));
}

/*****************************************************************************/
TEST_F(TestDirectedGraph, has_edge) {
    utility::graph::DirectedGraph<int> graph;
    graph.add_edge(1, 2);

    EXPECT_TRUE(graph.has_edge(1, 2));
    EXPECT_FALSE(graph.has_edge(2, 1));
    EXPECT_FALSE(graph.has_edge(1, 3));
    EXPECT_FALSE(graph.has_edge(3, 4));
}

/*****************************************************************************/
TEST_F(TestDirectedGraph, in_degree) {
    utility::graph::DirectedGraph<int> graph;
    graph.add_edge(1, 2);
    graph.add_edge(3, 2);

    EXPECT_EQ(0, graph.in_degree(1));
    EXPECT_EQ(2, graph.in_degree(2));
    EXPECT_EQ(0, graph.in_degree(3));
    EXPECT_EQ(0, graph.in_degree(4));  // Non-existent node
}

/*****************************************************************************/
TEST_F(TestDirectedGraph, out_degree) {
    utility::graph::DirectedGraph<int> graph;
    graph.add_edge(1, 2);
    graph.add_edge(1, 3);

    EXPECT_EQ(2, graph.out_degree(1));
    EXPECT_EQ(0, graph.out_degree(2));
    EXPECT_EQ(0, graph.out_degree(3));
    EXPECT_EQ(0, graph.out_degree(4));  // Non-existent node
}

/*****************************************************************************/
TEST_F(TestDirectedGraph, successors) {
    utility::graph::DirectedGraph<int> graph;
    graph.add_edge(1, 2);
    graph.add_edge(1, 3);

    const auto SUCC = graph.successors(1);
    EXPECT_EQ(2, static_cast<int>(SUCC.size()));
    EXPECT_TRUE(std::find(SUCC.begin(), SUCC.end(), 2) != SUCC.end());
    EXPECT_TRUE(std::find(SUCC.begin(), SUCC.end(), 3) != SUCC.end());

    EXPECT_TRUE(graph.successors(2).empty());
    EXPECT_TRUE(graph.successors(99).empty());  // Non-existent node
}

/*****************************************************************************/
TEST_F(TestDirectedGraph, predecessors) {
    utility::graph::DirectedGraph<int> graph;
    graph.add_edge(1, 3);
    graph.add_edge(2, 3);

    const auto PRED = graph.predecessors(3);
    EXPECT_EQ(2, static_cast<int>(PRED.size()));
    EXPECT_TRUE(std::find(PRED.begin(), PRED.end(), 1) != PRED.end());
    EXPECT_TRUE(std::find(PRED.begin(), PRED.end(), 2) != PRED.end());

    EXPECT_TRUE(graph.predecessors(1).empty());
    EXPECT_TRUE(graph.predecessors(99).empty());  // Non-existent node
}

/*****************************************************************************/
TEST_F(TestDirectedGraph, nodes) {
    utility::graph::DirectedGraph<int> graph;
    graph.add_node(10);
    graph.add_node(20);

    const auto &NODES = graph.nodes();
    EXPECT_EQ(2, static_cast<int>(NODES.size()));
    EXPECT_EQ(10, NODES[0]);
    EXPECT_EQ(20, NODES[1]);
}

/*****************************************************************************/
TEST_F(TestDirectedGraph, edges) {
    utility::graph::DirectedGraph<int> graph;
    graph.add_edge(1, 2, 10.0);

    const auto &EDGES = graph.edges();
    EXPECT_EQ(1, static_cast<int>(EDGES.size()));
    EXPECT_EQ(1, EDGES[0].first);
    EXPECT_EQ(2, EDGES[0].second);
    EXPECT_FLOAT_EQ(10.0, EDGES[0].weight);
}

/*****************************************************************************/
TEST_F(TestDirectedGraph, topological_sort) {
    // Chain: 0 -> 1 -> 2
    {
        utility::graph::DirectedGraph<int> graph;
        graph.add_edge(0, 1);
        graph.add_edge(1, 2);

        const auto ORDER = graph.topological_sort();
        EXPECT_EQ(3, static_cast<int>(ORDER.size()));
        EXPECT_EQ(0, ORDER[0]);
        EXPECT_EQ(1, ORDER[1]);
        EXPECT_EQ(2, ORDER[2]);
    }

    // Diamond DAG: 0 -> 1, 0 -> 2, 1 -> 3, 2 -> 3
    {
        utility::graph::DirectedGraph<int> graph;
        graph.add_edge(0, 1);
        graph.add_edge(0, 2);
        graph.add_edge(1, 3);
        graph.add_edge(2, 3);

        const auto ORDER = graph.topological_sort();
        EXPECT_EQ(4, static_cast<int>(ORDER.size()));

        const auto POS_0 = std::find(ORDER.begin(), ORDER.end(), 0);
        const auto POS_1 = std::find(ORDER.begin(), ORDER.end(), 1);
        const auto POS_2 = std::find(ORDER.begin(), ORDER.end(), 2);
        const auto POS_3 = std::find(ORDER.begin(), ORDER.end(), 3);

        EXPECT_TRUE(POS_0 < POS_1);
        EXPECT_TRUE(POS_0 < POS_2);
        EXPECT_TRUE(POS_1 < POS_3);
        EXPECT_TRUE(POS_2 < POS_3);
    }

    // Graph with cycle: returns empty vector
    {
        utility::graph::DirectedGraph<int> graph;
        graph.add_edge(0, 1);
        graph.add_edge(1, 0);

        const auto ORDER = graph.topological_sort();
        EXPECT_TRUE(ORDER.empty());
    }
}

/*****************************************************************************/
TEST_F(TestDirectedGraph, has_cycle) {
    utility::graph::DirectedGraph<int> dag;
    dag.add_edge(0, 1);
    dag.add_edge(1, 2);
    dag.add_edge(0, 2);
    EXPECT_FALSE(dag.has_cycle());

    utility::graph::DirectedGraph<int> cyclic_graph = dag;
    cyclic_graph.add_edge(2, 0);
    EXPECT_TRUE(cyclic_graph.has_cycle());
}

/*****************************************************************************/
TEST_F(TestDirectedGraph, node_weight_and_set_node_weight) {
    utility::graph::DirectedGraph<int> graph;
    graph.add_node(1, 10.5);

    EXPECT_FLOAT_EQ(10.5, graph.node_weight(1));
    EXPECT_FLOAT_EQ(0.0, graph.node_weight(2));  // Non-existent node

    graph.set_node_weight(1, 20.5);
    EXPECT_FLOAT_EQ(20.5, graph.node_weight(1));
}

/*****************************************************************************/
TEST_F(TestDirectedGraph, node_index) {
    utility::graph::DirectedGraph<std::string> graph;
    graph.add_node("a");
    graph.add_node("b");

    EXPECT_EQ(0, graph.node_index("a"));
    EXPECT_EQ(1, graph.node_index("b"));
    EXPECT_EQ(-1, graph.node_index("c"));  // Non-existent node
}

/*****************************************************************************/
TEST_F(TestDirectedGraph, successor_nodes_and_predecessor_nodes) {
    utility::graph::DirectedGraph<int> graph;
    graph.add_edge(1, 2, 3.5);

    const auto &SUCC_NODES = graph.successor_nodes(1);
    EXPECT_EQ(1, static_cast<int>(SUCC_NODES.size()));
    EXPECT_EQ(graph.node_index(2), SUCC_NODES[0].node_index);
    EXPECT_FLOAT_EQ(3.5, SUCC_NODES[0].weight);

    const auto &PRED_NODES = graph.predecessor_nodes(2);
    EXPECT_EQ(1, static_cast<int>(PRED_NODES.size()));
    EXPECT_EQ(graph.node_index(1), PRED_NODES[0].node_index);
    EXPECT_FLOAT_EQ(3.5, PRED_NODES[0].weight);

    EXPECT_TRUE(graph.successor_nodes(99).empty());
    EXPECT_TRUE(graph.predecessor_nodes(99).empty());
}

/*****************************************************************************/
TEST_F(TestDirectedGraph, strongly_connected_components) {
    // 1. DAG: each node is its own SCC
    {
        utility::graph::DirectedGraph<int> dag;
        dag.add_edge(0, 1);
        dag.add_edge(1, 2);

        const auto COMPONENTS = dag.strongly_connected_components();
        EXPECT_EQ(3, static_cast<int>(COMPONENTS.size()));
        for (const auto &component : COMPONENTS) {
            EXPECT_EQ(1, static_cast<int>(component.size()));
        }
    }

    // 2. Simple cycle: 0 -> 1 -> 2 -> 0 forms a single SCC
    {
        utility::graph::DirectedGraph<int> graph;
        graph.add_edge(0, 1);
        graph.add_edge(1, 2);
        graph.add_edge(2, 0);

        const auto COMPONENTS = graph.strongly_connected_components();
        EXPECT_EQ(1, static_cast<int>(COMPONENTS.size()));
        EXPECT_EQ(3, static_cast<int>(COMPONENTS[0].size()));
    }

    // 3. Two SCCs connected by an edge: (0 <-> 1) -> (2 <-> 3)
    {
        utility::graph::DirectedGraph<int> graph;
        graph.add_edge(0, 1);
        graph.add_edge(1, 0);
        graph.add_edge(1, 2);
        graph.add_edge(2, 3);
        graph.add_edge(3, 2);

        const auto COMPONENTS = graph.strongly_connected_components();
        EXPECT_EQ(2, static_cast<int>(COMPONENTS.size()));
        for (const auto &component : COMPONENTS) {
            EXPECT_EQ(2, static_cast<int>(component.size()));
        }
    }
}
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/
