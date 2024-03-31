/*****************************************************************************/
// Copyright (c) 2020-2024 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
using namespace printemps;
/*****************************************************************************/
class TestGraph : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestGraph, constructor) {
    utility::graph::Graph<int> graph;
    EXPECT_TRUE(graph.nodes().empty());
    EXPECT_TRUE(graph.edges().empty());
}

/*****************************************************************************/
TEST_F(TestGraph, initialize) {
    utility::graph::Graph<int> graph;
    graph.add_edge(1, 2);
    graph.add_edge(1, 3);
    graph.initialize();

    EXPECT_TRUE(graph.nodes().empty());
    EXPECT_TRUE(graph.edges().empty());
}

/*****************************************************************************/
TEST_F(TestGraph, add_edge_1) {
    utility::graph::Graph<int> graph;
    graph.add_edge(1, 2);
    graph.add_edge(1, 3);

    EXPECT_TRUE(graph.nodes().find(1) != graph.nodes().end());
    EXPECT_TRUE(graph.nodes().find(2) != graph.nodes().end());
    EXPECT_TRUE(graph.nodes().find(3) != graph.nodes().end());
    EXPECT_EQ(3, static_cast<int>(graph.nodes().size()));

    EXPECT_EQ(1, graph.edges()[0].node_keys().first);
    EXPECT_EQ(2, graph.edges()[0].node_keys().second);
    EXPECT_EQ(1, graph.edges()[1].node_keys().first);
    EXPECT_EQ(3, graph.edges()[1].node_keys().second);
    EXPECT_EQ(2, static_cast<int>(graph.edges().size()));
}

/*****************************************************************************/
TEST_F(TestGraph, add_edge_2) {
    utility::graph::Graph<int> graph;
    graph.add_edge(1, 2, 3.14);
    graph.add_edge(1, 3, 6.28);

    EXPECT_TRUE(graph.nodes().find(1) != graph.nodes().end());
    EXPECT_TRUE(graph.nodes().find(2) != graph.nodes().end());
    EXPECT_TRUE(graph.nodes().find(3) != graph.nodes().end());
    EXPECT_EQ(3, static_cast<int>(graph.nodes().size()));

    EXPECT_EQ(1, graph.edges()[0].node_keys().first);
    EXPECT_EQ(2, graph.edges()[0].node_keys().second);
    EXPECT_FLOAT_EQ(3.14, graph.edges()[0].weight());

    EXPECT_EQ(1, graph.edges()[1].node_keys().first);
    EXPECT_EQ(3, graph.edges()[1].node_keys().second);
    EXPECT_FLOAT_EQ(6.28, graph.edges()[1].weight());
    EXPECT_EQ(2, static_cast<int>(graph.edges().size()));
}

/*****************************************************************************/
TEST_F(TestGraph, add_edge_3) {
    utility::graph::Graph<int> graph;
    graph.add_edge(utility::graph::Edge<int>(1, 2, 3.14));
    graph.add_edge(utility::graph::Edge<int>(1, 3, 6.28));

    EXPECT_TRUE(graph.nodes().find(1) != graph.nodes().end());
    EXPECT_TRUE(graph.nodes().find(2) != graph.nodes().end());
    EXPECT_TRUE(graph.nodes().find(3) != graph.nodes().end());
    EXPECT_EQ(3, static_cast<int>(graph.nodes().size()));

    EXPECT_EQ(1, graph.edges()[0].node_keys().first);
    EXPECT_EQ(2, graph.edges()[0].node_keys().second);
    EXPECT_FLOAT_EQ(3.14, graph.edges()[0].weight());

    EXPECT_EQ(1, graph.edges()[1].node_keys().first);
    EXPECT_EQ(3, graph.edges()[1].node_keys().second);
    EXPECT_FLOAT_EQ(6.28, graph.edges()[1].weight());
    EXPECT_EQ(2, static_cast<int>(graph.edges().size()));
}

/*****************************************************************************/
TEST_F(TestGraph, add_node_1) {
    utility::graph::Graph<int> graph;
    graph.add_node(1);
    graph.add_node(2);
    graph.add_node(2);
    EXPECT_TRUE(graph.nodes().find(1) != graph.nodes().end());
    EXPECT_TRUE(graph.nodes().find(2) != graph.nodes().end());
    EXPECT_EQ(2, static_cast<int>(graph.nodes().size()));
}

/*****************************************************************************/
TEST_F(TestGraph, add_node_2) {
    utility::graph::Graph<int> graph;
    graph.add_node(1, 3.14);
    graph.add_node(2, 3.14);
    graph.add_node(2, 6.28);

    EXPECT_TRUE(graph.nodes().find(1) != graph.nodes().end());
    EXPECT_TRUE(graph.nodes().find(2) != graph.nodes().end());
    EXPECT_EQ(2, static_cast<int>(graph.nodes().size()));

    EXPECT_FLOAT_EQ(3.14, graph.nodes().at(1).weight());
    EXPECT_FLOAT_EQ(6.28, graph.nodes().at(2).weight());
}

/*****************************************************************************/
TEST_F(TestGraph, add_node_3) {
    utility::graph::Graph<int> graph;
    graph.add_node(utility::graph::Node<int>(1, 3.14));
    graph.add_node(utility::graph::Node<int>(2, 3.14));
    graph.add_node(utility::graph::Node<int>(2, 6.28));

    EXPECT_TRUE(graph.nodes().find(1) != graph.nodes().end());
    EXPECT_TRUE(graph.nodes().find(2) != graph.nodes().end());
    EXPECT_EQ(2, static_cast<int>(graph.nodes().size()));

    EXPECT_FLOAT_EQ(3.14, graph.nodes().at(1).weight());
    EXPECT_FLOAT_EQ(6.28, graph.nodes().at(2).weight());
}

/*****************************************************************************/
TEST_F(TestGraph, minimum_spanning_tree) {
    {
        utility::graph::Graph<int> graph;
        graph.add_edge(1, 2, 10.0);
        graph.add_edge(2, 3, 10.0);
        graph.add_edge(3, 4, 10.0);
        graph.add_edge(4, 1, 10.0);
        graph.add_edge(1, 5, 1.0);
        graph.add_edge(2, 5, 1.0);
        graph.add_edge(3, 5, 1.0);
        graph.add_edge(4, 5, 1.0);

        auto mst = graph.minimum_spanning_tree();

        EXPECT_EQ(1, static_cast<int>(mst.nodes().at(1).edge_indices().size()));
        EXPECT_EQ(1, static_cast<int>(mst.nodes().at(2).edge_indices().size()));
        EXPECT_EQ(1, static_cast<int>(mst.nodes().at(3).edge_indices().size()));
        EXPECT_EQ(1, static_cast<int>(mst.nodes().at(4).edge_indices().size()));
        EXPECT_EQ(4, static_cast<int>(mst.nodes().at(5).edge_indices().size()));
    }

    {
        utility::graph::Graph<int> graph;
        graph.add_edge(1, 2, 1.0);
        graph.add_edge(3, 4, 1.0);

        auto mst = graph.minimum_spanning_tree();

        EXPECT_EQ(1, static_cast<int>(mst.nodes().at(1).edge_indices().size()));
        EXPECT_EQ(1, static_cast<int>(mst.nodes().at(2).edge_indices().size()));
        EXPECT_EQ(1, static_cast<int>(mst.nodes().at(3).edge_indices().size()));
        EXPECT_EQ(1, static_cast<int>(mst.nodes().at(4).edge_indices().size()));
    }
}

/*****************************************************************************/
TEST_F(TestGraph, nodes) {
    /// This method is tested in other cases.
}

/*****************************************************************************/
TEST_F(TestGraph, edges) {
    /// This method is tested in other cases.
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/