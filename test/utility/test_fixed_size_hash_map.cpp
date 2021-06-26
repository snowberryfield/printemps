/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <random>

#include <printemps.h>

namespace {
/*****************************************************************************/
class TestFixedSizeHashMap : public ::testing::Test {
   protected:
    printemps::utility::IntegerUniformRandom m_random_integer;
    printemps::utility::IntegerUniformRandom m_random_positive_integer;

    virtual void SetUp(void) {
        m_random_integer.setup(-1000, 1000, 0);
        m_random_positive_integer.setup(1, 1000, 0);
    }
    virtual void TearDown() {
        /// nothing to do
    }
    int random_integer(void) {
        return m_random_integer.generate_random();
    }

    int random_positive_integer(void) {
        return m_random_positive_integer.generate_random();
    }
};

/*****************************************************************************/
TEST_F(TestFixedSizeHashMap, initialize) {
    printemps::utility::FixedSizeHashMap<
        printemps::model_component::Variable<int, double>*, double>
        fixed_size_hash_map;

    /// static const variable seems not to be allowed in EXPECT_EQ().
    std::size_t default_bucket_size =
        printemps::utility::FixedSizeHashMapConstant::DEFAULT_BUCKET_SIZE;

    EXPECT_EQ(0, static_cast<int>(fixed_size_hash_map.shift_size()));
    EXPECT_EQ(default_bucket_size, fixed_size_hash_map.bucket_size());
}

/*****************************************************************************/
TEST_F(TestFixedSizeHashMap, setup) {
    printemps::utility::FixedSizeHashMap<
        printemps::model_component::Variable<int, double>*, double>
        fixed_size_hash_map;

    printemps::model::Model<int, double> model;

    auto& x = model.create_variables("x", {10, 20});
    auto& y = model.create_variables("y", {20, 30, 40});

    std::unordered_map<printemps::model_component::Variable<int, double>*,
                       double>
        unordered_map;

    for (auto i = 0; i < 10; i++) {
        for (auto j = 0; j < 20; j++) {
            unordered_map[&x(i, j)] = random_integer();
        }
    }

    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            for (auto k = 0; k < 40; k++) {
                unordered_map[&y(i, j, k)] = random_integer();
            }
        }
    }
    fixed_size_hash_map.setup(
        unordered_map,
        sizeof(printemps::model_component::Variable<int, double>));

    EXPECT_EQ((unsigned int)log2(
                  sizeof(printemps::model_component::Variable<int, double>)),
              fixed_size_hash_map.shift_size());

    /// 10 * 20 + 20 * 30 * 40 = 200 + 24000
    /// 262144 < 24000 * 5(LOAD_FACTOR_MULTIPLIER) < 131072
    std::size_t expected_bucket_size = 131072;
    EXPECT_EQ(expected_bucket_size, fixed_size_hash_map.bucket_size());

    for (const auto& element : unordered_map) {
        EXPECT_EQ(element.second, fixed_size_hash_map.at(element.first));
    }
}

/*****************************************************************************/
TEST_F(TestFixedSizeHashMap, at) {
    /// This method is tested in setup().
}

/*****************************************************************************/
TEST_F(TestFixedSizeHashMap, shift_size) {
    /// This method is tested in initialize() and setup().
}

/*****************************************************************************/
TEST_F(TestFixedSizeHashMap, bucket_size) {
    /// This method is tested in initialize() and setup().
}
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/