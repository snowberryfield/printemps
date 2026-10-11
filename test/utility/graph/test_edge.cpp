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
TEST_F(TestEdge, default_constructor) {
    utility::graph::Edge<int> edge;

    EXPECT_EQ(0, edge.first);
    EXPECT_EQ(0, edge.second);
    EXPECT_FLOAT_EQ(0.0, edge.weight);
}

/*****************************************************************************/
TEST_F(TestEdge, constructor_with_args) {
    utility::graph::Edge<int> edge_default_weight(1, 2);
    EXPECT_EQ(1, edge_default_weight.first);
    EXPECT_EQ(2, edge_default_weight.second);
    EXPECT_FLOAT_EQ(0.0, edge_default_weight.weight);

    utility::graph::Edge<int> edge_with_weight(1, 2, 3.14);
    EXPECT_EQ(1, edge_with_weight.first);
    EXPECT_EQ(2, edge_with_weight.second);
    EXPECT_FLOAT_EQ(3.14, edge_with_weight.weight);

    utility::graph::Edge<std::string> edge_string("a", "b", 1.5);
    EXPECT_EQ("a", edge_string.first);
    EXPECT_EQ("b", edge_string.second);
    EXPECT_FLOAT_EQ(1.5, edge_string.weight);
}
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/
