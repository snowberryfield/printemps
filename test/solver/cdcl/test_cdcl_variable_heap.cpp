/*****************************************************************************/
// Copyright (c) 2020-2026 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
using namespace printemps;
using namespace printemps::solver::cdcl::core;

/*****************************************************************************/
class TestCDCLVariableHeap : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown(void) {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestCDCLVariableHeap, initialize) {
    CDCLVariableHeap heap;
    EXPECT_TRUE(heap.empty());
    EXPECT_EQ(heap.size(), 0);

    std::vector<double> activities = {1.0, 2.0};
    heap.setup(2, activities);
    EXPECT_FALSE(heap.empty());

    heap.initialize();
    EXPECT_TRUE(heap.empty());
    EXPECT_EQ(heap.size(), 0);
}

/*****************************************************************************/
TEST_F(TestCDCLVariableHeap, setup) {
    std::vector<double> activities = {10.0, 50.0, 20.0};
    CDCLVariableHeap    heap;
    heap.setup(3, activities);

    EXPECT_EQ(heap.size(), 3);
    EXPECT_TRUE(heap.contains(0));
    EXPECT_TRUE(heap.contains(1));
    EXPECT_TRUE(heap.contains(2));

    // Variable with highest activity (index 1) should be popped first
    EXPECT_EQ(heap.pop(activities), 1);
    EXPECT_EQ(heap.pop(activities), 2);
    EXPECT_EQ(heap.pop(activities), 0);
    EXPECT_TRUE(heap.empty());
}

/*****************************************************************************/
TEST_F(TestCDCLVariableHeap, contains) {
    std::vector<double> activities = {5.0, 15.0};
    CDCLVariableHeap    heap;
    heap.setup(2, activities);

    EXPECT_TRUE(heap.contains(0));
    EXPECT_TRUE(heap.contains(1));
    EXPECT_FALSE(heap.contains(2));
    EXPECT_FALSE(heap.contains(-1));

    EXPECT_NE(heap.position_of(0), CDCLVariableHeap::NOT_IN_HEAP);
    EXPECT_NE(heap.position_of(1), CDCLVariableHeap::NOT_IN_HEAP);
    EXPECT_EQ(heap.position_of(2), CDCLVariableHeap::NOT_IN_HEAP);
}

/*****************************************************************************/
TEST_F(TestCDCLVariableHeap, position_of) {
    /// This method is tested in contains().
}

/*****************************************************************************/
TEST_F(TestCDCLVariableHeap, push) {
    CDCLVariableHeap    heap;
    std::vector<double> activities = {1.0, 5.0, 3.0};
    heap.setup(3, activities);

    // Pop all elements to empty the heap
    EXPECT_EQ(heap.pop(activities), 1);
    EXPECT_EQ(heap.pop(activities), 2);
    EXPECT_EQ(heap.pop(activities), 0);
    EXPECT_TRUE(heap.empty());

    // Push elements back
    heap.push(0, activities);
    heap.push(1, activities);
    heap.push(2, activities);
    EXPECT_EQ(heap.size(), 3);

    // Popping order should be 1 (activity 5.0), 2 (activity 3.0), 0
    // (activity 1.0)
    EXPECT_EQ(heap.pop(activities), 1);
    EXPECT_FALSE(heap.contains(1));
    EXPECT_EQ(heap.pop(activities), 2);
    EXPECT_FALSE(heap.contains(2));
    EXPECT_EQ(heap.pop(activities), 0);
    EXPECT_FALSE(heap.contains(0));
    EXPECT_TRUE(heap.empty());
}

/*****************************************************************************/
TEST_F(TestCDCLVariableHeap, pop) {
    /// This method is tested in push().
}

/*****************************************************************************/
TEST_F(TestCDCLVariableHeap, empty) {
    /// This method is tested in initialize() and push().
}

/*****************************************************************************/
TEST_F(TestCDCLVariableHeap, size) {
    /// This method is tested in initialize() and push().
}

/*****************************************************************************/
TEST_F(TestCDCLVariableHeap, remove) {
    std::vector<double> activities = {10.0, 20.0, 30.0, 40.0};
    CDCLVariableHeap    heap;
    heap.setup(4, activities);

    // Remove middle element (index 1, activity 20.0)
    heap.remove(1, activities);
    EXPECT_FALSE(heap.contains(1));
    EXPECT_EQ(heap.size(), 3);

    // Popping remaining elements
    EXPECT_EQ(heap.pop(activities), 3);  // 40.0
    EXPECT_EQ(heap.pop(activities), 2);  // 30.0
    EXPECT_EQ(heap.pop(activities), 0);  // 10.0
    EXPECT_TRUE(heap.empty());
}

/*****************************************************************************/
TEST_F(TestCDCLVariableHeap, update_after_bump) {
    std::vector<double> activities = {10.0, 20.0, 30.0};
    CDCLVariableHeap    heap;
    heap.setup(3, activities);

    // Bump variable 0's activity to 100.0 (highest)
    activities[0] = 100.0;
    heap.update_after_bump(0, activities);

    // Variable 0 should now be at the top
    EXPECT_EQ(heap.pop(activities), 0);
    EXPECT_EQ(heap.pop(activities), 2);
    EXPECT_EQ(heap.pop(activities), 1);
    EXPECT_TRUE(heap.empty());
}

/*****************************************************************************/
TEST_F(TestCDCLVariableHeap, greater_than) {
    CDCLVariableHeap    heap;
    std::vector<double> activities = {10.0, 20.0, 10.0};

    // activity difference: 1 (20.0) > 0 (10.0)
    EXPECT_TRUE(heap.greater_than(1, 0, activities));
    EXPECT_FALSE(heap.greater_than(0, 1, activities));

    // tie-breaker by index: 0 < 2
    EXPECT_TRUE(heap.greater_than(0, 2, activities));
    EXPECT_FALSE(heap.greater_than(2, 0, activities));
}

/*****************************************************************************/
TEST_F(TestCDCLVariableHeap, heap_array_and_positions) {
    std::vector<double> activities = {10.0, 50.0, 20.0};
    CDCLVariableHeap    heap;
    heap.setup(3, activities);

    EXPECT_EQ(heap.heap_array().size(), 3u);
    EXPECT_EQ(heap.heap_positions().size(), 3u);
    EXPECT_EQ(heap.heap_array().front(), 1);
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/