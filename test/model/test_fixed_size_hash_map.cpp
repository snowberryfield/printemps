/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <random>
#include <cppmh.h>

/*****************************************************************************/
namespace {
class TestFixedSizeHashMap : public ::testing::Test {
   protected:
    cppmh::utility::IntegerUniformRandom m_random_integer;
    cppmh::utility::IntegerUniformRandom m_random_positive_integer;

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
    cppmh::model::FixedSizeHashMap<cppmh::model::Variable<int, double>*, double>
        fixed_size_hash_map;

    /// static const variable seems not to be allowed in EXPECT_EQ().
    int default_bucket_size =
        cppmh::model::FixedSizeHashMapConstant::DEFAULT_BUCKET_SIZE;

    EXPECT_EQ(static_cast<unsigned int>(0), fixed_size_hash_map.shift_size());
    EXPECT_EQ(default_bucket_size, fixed_size_hash_map.bucket_size());
    EXPECT_EQ(default_bucket_size, fixed_size_hash_map.keys().size());
    EXPECT_EQ(default_bucket_size, fixed_size_hash_map.values().size());
    EXPECT_EQ(default_bucket_size, fixed_size_hash_map.is_occupied().size());

    EXPECT_EQ(0, fixed_size_hash_map.keys()[0]);
    EXPECT_EQ(0, fixed_size_hash_map.values()[0]);
    EXPECT_EQ(0, fixed_size_hash_map.is_occupied()[0]);
}

/*****************************************************************************/
TEST_F(TestFixedSizeHashMap, setup) {
    cppmh::model::FixedSizeHashMap<cppmh::model::Variable<int, double>*, double>
        fixed_size_hash_map;

    cppmh::model::Model<int, double> model;

    auto& x = model.create_variables("x", {10, 20});
    auto& y = model.create_variables("y", {20, 30, 40});

    std::unordered_map<cppmh::model::Variable<int, double>*, double>
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
    fixed_size_hash_map.setup(unordered_map,
                              sizeof(cppmh::model::Variable<int, double>));

    EXPECT_EQ((unsigned int)log2(sizeof(cppmh::model::Variable<int, double>)),
              fixed_size_hash_map.shift_size());

    /// 10 * 20 + 20 * 30 * 40 = 200 + 24000
    /// 4194304 < 24000 * 100(LOAD_MARGIN) < 4194304
    int expected_bucket_size = 4194304;
    EXPECT_EQ(expected_bucket_size, fixed_size_hash_map.bucket_size());
    EXPECT_EQ(expected_bucket_size, fixed_size_hash_map.keys().size());
    EXPECT_EQ(expected_bucket_size, fixed_size_hash_map.values().size());
    EXPECT_EQ(expected_bucket_size, fixed_size_hash_map.is_occupied().size());

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

/*****************************************************************************/
TEST_F(TestFixedSizeHashMap, keys) {
    /// This method is tested in initialize() and setup().
}

/*****************************************************************************/
TEST_F(TestFixedSizeHashMap, values) {
    /// This method is tested in initialize() and setup().
}

/*****************************************************************************/
TEST_F(TestFixedSizeHashMap, is_occupied) {
    /// This method is tested in initialize() and setup().
}
/*****************************************************************************/
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/