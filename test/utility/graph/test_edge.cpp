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
class TestEdge : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};
/*****************************************************************************/
TEST_F(TestEdge, constructor_without_arg) {
    utility::graph::Edge<int> edge;

    EXPECT_FLOAT_EQ(0.0, edge.weight());
}

/*****************************************************************************/
TEST_F(TestEdge, constructor_with_arg_1) {
    utility::graph::Edge<int> edge(1, 2);

    EXPECT_FLOAT_EQ(0.0, edge.weight());
    EXPECT_EQ(1, edge.node_keys().first);
    EXPECT_EQ(2, edge.node_keys().second);
}

/*****************************************************************************/
TEST_F(TestEdge, constructor_with_arg_2) {
    utility::graph::Edge<int> edge(1, 2, 3.14);

    EXPECT_FLOAT_EQ(3.14, edge.weight());
    EXPECT_EQ(1, edge.node_keys().first);
    EXPECT_EQ(2, edge.node_keys().second);
}

/*****************************************************************************/
TEST_F(TestEdge, initialize) {
    utility::graph::Node<int> edge(10, 3.14);
    edge.initialize();

    EXPECT_FLOAT_EQ(0.0, edge.weight());
}

/*****************************************************************************/
TEST_F(TestEdge, setup_with_arg_1) {
    /// This method is tested in constructor_with_arg_1()
}

/*****************************************************************************/
TEST_F(TestEdge, setup_with_arg_2) {
    /// This method is tested in constructor_with_arg_2()
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/