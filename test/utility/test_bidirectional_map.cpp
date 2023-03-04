/*****************************************************************************/
// Copyright (c) 2020-2023 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <string>

#include <printemps.h>

namespace {
using namespace printemps;
/*****************************************************************************/
class TestBidirectionalMap : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestBidirectionalMap, initialize) {
    utility::BidirectionalMap<int, std::string> bmap;

    EXPECT_TRUE(bmap.empty());
}

/*****************************************************************************/
TEST_F(TestBidirectionalMap, operator_square_bracket) {
    utility::BidirectionalMap<int, std::string> bmap;

    bmap.insert(0, "zero");
    bmap.insert(1, "one");
    bmap.insert(10, "ten");

    EXPECT_EQ("zero", bmap.forward_at(0));
    EXPECT_EQ("one", bmap.forward_at(1));
    EXPECT_EQ("ten", bmap.forward_at(10));

    EXPECT_EQ(0, bmap.reverse_at("zero"));
    EXPECT_EQ(1, bmap.reverse_at("one"));
    EXPECT_EQ(10, bmap.reverse_at("ten"));

    EXPECT_EQ(3, bmap.size());
}

/*****************************************************************************/
TEST_F(TestBidirectionalMap, empty) {
    /// This method is tested in initialize().
}

/*****************************************************************************/
TEST_F(TestBidirectionalMap, size) {
    /// This method is tested in operator_square_bracket().
}
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/