/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <vector>

#include <printemps.h>

namespace {
/*****************************************************************************/
class TestRandomUtility : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestRandomUtility, generate_random) {
    printemps::utility::IntegerUniformRandom random_integer_generator;
    printemps::utility::IntegerUniformRandom random_positive_integer_generator;

    random_integer_generator.setup(-100, 100, 0);
    random_positive_integer_generator.setup(1, 100, 0);

    std::vector<int> random_integers;
    std::vector<int> random_positive_integers;

    for (auto i = 0; i < 100000; i++) {
        random_integers.push_back(random_integer_generator.generate_random());
        random_positive_integers.push_back(
            random_positive_integer_generator.generate_random());
    }

    EXPECT_EQ(-100, printemps::utility::min(random_integers));
    EXPECT_EQ(100, printemps::utility::max(random_integers));

    EXPECT_EQ(1, printemps::utility::min(random_positive_integers));
    EXPECT_EQ(100, printemps::utility::max(random_positive_integers));
}

/*****************************************************************************/
TEST_F(TestRandomUtility, seed) {
    printemps::utility::IntegerUniformRandom random_integer_generator_1;
    printemps::utility::IntegerUniformRandom random_integer_generator_2;

    random_integer_generator_1.setup(-100, 100, 0);
    random_integer_generator_2.setup(-100, 100, 0);

    /// seed = 1
    random_integer_generator_1.seed(1);
    random_integer_generator_2.seed(1);

    {
        std::vector<int> random_integers_1;
        std::vector<int> random_integers_2;

        for (auto i = 0; i < 100000; i++) {
            random_integers_1.push_back(
                random_integer_generator_1.generate_random());
            random_integers_2.push_back(
                random_integer_generator_2.generate_random());
        }

        for (auto i = 0; i < 100000; i++) {
            EXPECT_EQ(0, random_integers_1[i] - random_integers_2[i]);
        }
    }

    /// seed = 2
    random_integer_generator_1.seed(2);
    random_integer_generator_2.seed(2);
    {
        std::vector<int> random_integers_1;
        std::vector<int> random_integers_2;

        for (auto i = 0; i < 100000; i++) {
            random_integers_1.push_back(
                random_integer_generator_1.generate_random());
            random_integers_2.push_back(
                random_integer_generator_2.generate_random());
        }

        for (auto i = 0; i < 100000; i++) {
            EXPECT_EQ(0, random_integers_1[i] - random_integers_2[i]);
        }
    }
}
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/