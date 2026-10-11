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
class TestIntensity : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestIntensity, initialize) {
    utility::Intensity<int> intensity;

    EXPECT_EQ(0, intensity.total_count());
    EXPECT_DOUBLE_EQ(0.0, intensity.total_count_reciprocal());
    EXPECT_DOUBLE_EQ(0.0, intensity.value());
    EXPECT_EQ(0, intensity.count(0));
    EXPECT_TRUE(intensity.counts().empty());
}

/*****************************************************************************/
TEST_F(TestIntensity, reset) {
    utility::Intensity<int> intensity;
    intensity.increment(1);
    intensity.increment(2);

    EXPECT_EQ(2, intensity.total_count());
    EXPECT_FALSE(intensity.counts().empty());

    intensity.reset();

    EXPECT_EQ(0, intensity.total_count());
    EXPECT_DOUBLE_EQ(0.0, intensity.total_count_reciprocal());
    EXPECT_DOUBLE_EQ(0.0, intensity.value());
    EXPECT_EQ(0, intensity.count(1));
    EXPECT_TRUE(intensity.counts().empty());
}

/*****************************************************************************/
TEST_F(TestIntensity, increment_single_key) {
    utility::Intensity<std::string> intensity;

    intensity.increment("A");
    EXPECT_EQ(1, intensity.total_count());
    EXPECT_EQ(1, intensity.count("A"));
    EXPECT_DOUBLE_EQ(1.0, intensity.value());
    EXPECT_DOUBLE_EQ(1.0, intensity.total_count_reciprocal());

    intensity.increment("A");
    EXPECT_EQ(2, intensity.total_count());
    EXPECT_EQ(2, intensity.count("A"));
    EXPECT_DOUBLE_EQ(1.0, intensity.value());
    EXPECT_DOUBLE_EQ(0.5, intensity.total_count_reciprocal());

    intensity.increment("A");
    EXPECT_EQ(3, intensity.total_count());
    EXPECT_EQ(3, intensity.count("A"));
    EXPECT_DOUBLE_EQ(1.0, intensity.value());
}

/*****************************************************************************/
TEST_F(TestIntensity, increment_uniform) {
    utility::Intensity<int> intensity;

    // 2 elements uniform: (1^2 + 1^2) / 2^2 = 2 / 4 = 0.5
    intensity.increment(1);
    intensity.increment(2);
    EXPECT_EQ(2, intensity.total_count());
    EXPECT_DOUBLE_EQ(0.5, intensity.value());

    // 4 elements uniform: (1^2 * 4) / 4^2 = 4 / 16 = 0.25
    intensity.increment(3);
    intensity.increment(4);
    EXPECT_EQ(4, intensity.total_count());
    EXPECT_DOUBLE_EQ(0.25, intensity.value());

    // 4 elements with 2 counts each: (2^2 * 4) / 8^2 = 16 / 64 = 0.25
    intensity.increment(1);
    intensity.increment(2);
    intensity.increment(3);
    intensity.increment(4);
    EXPECT_EQ(8, intensity.total_count());
    EXPECT_DOUBLE_EQ(0.25, intensity.value());
}

/*****************************************************************************/
TEST_F(TestIntensity, increment_skewed) {
    utility::Intensity<std::string> intensity;

    // "A": 3, "B": 1 => sum_sq = 3^2 + 1^2 = 10, total = 4 => 10 / 16 = 0.625
    intensity.increment("A");
    intensity.increment("A");
    intensity.increment("A");
    intensity.increment("B");

    EXPECT_EQ(4, intensity.total_count());
    EXPECT_EQ(3, intensity.count("A"));
    EXPECT_EQ(1, intensity.count("B"));
    EXPECT_EQ(0, intensity.count("C"));
    EXPECT_DOUBLE_EQ(0.625, intensity.value());
    EXPECT_DOUBLE_EQ(0.25, intensity.total_count_reciprocal());
}

/*****************************************************************************/
TEST_F(TestIntensity, update_with_previous_count) {
    utility::Intensity<int> intensity_direct;
    utility::Intensity<int> intensity_sim;

    // Compare direct increment vs update_with_previous_count
    std::vector<int>                 stream = {1, 2, 1, 3, 1, 2, 4};
    std::unordered_map<int, int64_t> manual_counts;

    for (const auto& item : stream) {
        intensity_direct.increment(item);

        const int64_t prev  = manual_counts[item];
        manual_counts[item] = prev + 1;
        intensity_sim.update_with_previous_count(prev);

        EXPECT_EQ(intensity_direct.total_count(), intensity_sim.total_count());
        EXPECT_DOUBLE_EQ(intensity_direct.value(), intensity_sim.value());
    }
}

/*****************************************************************************/
TEST_F(TestIntensity, count_and_counts) {
    utility::Intensity<char> intensity;
    intensity.increment('x');
    intensity.increment('y');
    intensity.increment('x');

    EXPECT_EQ(2, intensity.count('x'));
    EXPECT_EQ(1, intensity.count('y'));
    EXPECT_EQ(0, intensity.count('z'));

    const auto& map = intensity.counts();
    EXPECT_EQ(2u, map.size());
    EXPECT_EQ(2, map.at('x'));
    EXPECT_EQ(1, map.at('y'));
}

}  // namespace
