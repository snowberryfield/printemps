/*****************************************************************************/
// Copyright (c) 2020-2024 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <string>

#include <printemps.h>

namespace {
using namespace printemps;
/*****************************************************************************/
class TestFixedSizeQueue : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestFixedSizeQueue, initialize) {
    utility::FixedSizeQueue<int> queue;

    EXPECT_EQ(0, queue.size());
    EXPECT_EQ(0, queue.max_size());
}

/*****************************************************************************/
TEST_F(TestFixedSizeQueue, size) {
    utility::FixedSizeQueue<int> queue(100);
    EXPECT_EQ(100, queue.max_size());
    EXPECT_EQ(0, queue.size());
    for (auto i = 0; i < 100; i++) {
        queue.push(i);
        EXPECT_EQ(i + 1, queue.size());
    }
    for (auto i = 0; i < 10; i++) {
        queue.push(i);
        EXPECT_EQ(100, queue.size());
    }
}

/*****************************************************************************/
TEST_F(TestFixedSizeQueue, max_size) {
    /// This method is tested in size().
}

/*****************************************************************************/
TEST_F(TestFixedSizeQueue, min) {
    {
        utility::FixedSizeQueue<int> queue(10);

        double sum = 0.0;

        for (auto i = 0; i < 100; i++) {
            queue.push(i);
            sum += i;
            if (i < 10) {
                EXPECT_EQ(0, queue.min());
                EXPECT_EQ(sum / (i + 1), queue.average());
            }
            EXPECT_EQ(i, queue.max());
        }
    }
    {
        utility::FixedSizeQueue<double> queue(110);

        double sum = 0.0;

        for (auto i = 0; i < 100; i++) {
            queue.push(i * 1.0);
            sum += i * 1.0;
            if (i < 10) {
                EXPECT_EQ(0, queue.min());
                EXPECT_EQ(sum / (i + 1), queue.average());
            }
            EXPECT_EQ(i, queue.max());
        }
    }
}

/*****************************************************************************/
TEST_F(TestFixedSizeQueue, max) {
    /// This method is tested in min().
}

/*****************************************************************************/
TEST_F(TestFixedSizeQueue, average) {
    /// This method is tested in min().
}
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/