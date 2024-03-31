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
class TestNode : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestNode, constructor_without_arg) {
    utility::graph::Node<int> node;

    EXPECT_FLOAT_EQ(0.0, node.weight());
    EXPECT_TRUE(node.edge_indices().empty());
}

/*****************************************************************************/
TEST_F(TestNode, constructor_with_arg_1) {
    utility::graph::Node<int> node(10);

    EXPECT_EQ(10, node.key());
    EXPECT_FLOAT_EQ(0.0, node.weight());
    EXPECT_TRUE(node.edge_indices().empty());
}

/*****************************************************************************/
TEST_F(TestNode, constructor_with_arg_2) {
    utility::graph::Node<int> node(10, 3.14);

    EXPECT_EQ(10, node.key());
    EXPECT_FLOAT_EQ(3.14, node.weight());
    EXPECT_TRUE(node.edge_indices().empty());
}

/*****************************************************************************/
TEST_F(TestNode, initialize) {
    utility::graph::Node<int> node(10, 3.14);
    node.initialize();

    EXPECT_FLOAT_EQ(0.0, node.weight());
    EXPECT_TRUE(node.edge_indices().empty());
}

/*****************************************************************************/
TEST_F(TestNode, setup_with_arg_1) {
    /// This method is tested in constructor_with_arg_1()
}

/*****************************************************************************/
TEST_F(TestNode, setup_with_arg_2) {
    /// This method is tested in constructor_with_arg_2()
}
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/