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
class TestAdjacentNode : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestAdjacentNode, default_constructor) {
    utility::graph::AdjacentNode adjacent_node;

    EXPECT_EQ(0, adjacent_node.node_index);
    EXPECT_FLOAT_EQ(0.0, adjacent_node.weight);
    EXPECT_EQ(0, adjacent_node.edge_index);
}

/*****************************************************************************/
TEST_F(TestAdjacentNode, constructor_with_args) {
    utility::graph::AdjacentNode node_default_args(5);
    EXPECT_EQ(5, node_default_args.node_index);
    EXPECT_FLOAT_EQ(0.0, node_default_args.weight);
    EXPECT_EQ(0, node_default_args.edge_index);

    utility::graph::AdjacentNode node_with_weight(5, 3.14);
    EXPECT_EQ(5, node_with_weight.node_index);
    EXPECT_FLOAT_EQ(3.14, node_with_weight.weight);
    EXPECT_EQ(0, node_with_weight.edge_index);

    utility::graph::AdjacentNode node_full(5, 3.14, 12);
    EXPECT_EQ(5, node_full.node_index);
    EXPECT_FLOAT_EQ(3.14, node_full.weight);
    EXPECT_EQ(12, node_full.edge_index);
}
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/
