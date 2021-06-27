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
class TestValueProxy : public ::testing::Test {
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
TEST_F(TestValueProxy, scalar_constructor) {
    const int                               ID = -1;
    printemps::multi_array::ValueProxy<int> value_proxy(ID);

    EXPECT_EQ(ID, value_proxy.index());
    EXPECT_EQ(1, value_proxy.shape()[0]);
    EXPECT_EQ(1, value_proxy.strides()[0]);
    EXPECT_EQ(1, static_cast<int>(value_proxy.number_of_dimensions()));
    EXPECT_EQ(1, static_cast<int>(value_proxy.number_of_elements()));
    EXPECT_EQ(0, value_proxy.value());
}

/*****************************************************************************/
TEST_F(TestValueProxy, scalar_flat_indexed_values_arg_void) {
    const int                               ID = -1;
    printemps::multi_array::ValueProxy<int> value_proxy(ID);

    auto value  = random_integer();
    value_proxy = value;
    EXPECT_EQ(value, value_proxy.flat_indexed_values()[0]);
}

/*****************************************************************************/
TEST_F(TestValueProxy, scalar_flat_indexed_values_arg_int) {
    const int                               ID = -1;
    printemps::multi_array::ValueProxy<int> value_proxy(ID);

    auto value  = random_integer();
    value_proxy = value;
    EXPECT_EQ(value, value_proxy.flat_indexed_values(0));
}

/*****************************************************************************/
TEST_F(TestValueProxy, scalar_value) {
    const int                               ID = -1;
    printemps::multi_array::ValueProxy<int> value_proxy(ID);

    auto value          = random_integer();
    value_proxy.value() = value;
    EXPECT_EQ(value, value_proxy.value());
}

/*****************************************************************************/
TEST_F(TestValueProxy, scalar_values) {
    const int                               ID = -1;
    printemps::multi_array::ValueProxy<int> value_proxy(ID);

    auto value            = random_integer();
    value_proxy.values(0) = value;
    EXPECT_EQ(value, value_proxy.values(0));

    ASSERT_THROW(value_proxy.values(0, 1) = value, std::logic_error);
    ASSERT_THROW(value_proxy.values(0, 1, 2) = value, std::logic_error);
    ASSERT_THROW(value_proxy.values(0, 1, 2, 3) = value, std::logic_error);
}

/*****************************************************************************/
TEST_F(TestValueProxy, scalar_values_with_indices) {
    const int                               ID = -1;
    printemps::multi_array::ValueProxy<int> value_proxy(ID);

    auto value              = random_integer();
    value_proxy.values({0}) = value;
    EXPECT_EQ(value, value_proxy.values({0}));
}

/*****************************************************************************/
TEST_F(TestValueProxy, scalar_fill) {
    const int                               ID = -1;
    printemps::multi_array::ValueProxy<int> value_proxy(ID);

    auto value = random_integer();
    value_proxy.fill(value);
    EXPECT_EQ(value, value_proxy.value());
}

/*****************************************************************************/
TEST_F(TestValueProxy, scalar_operator_square_bracket) {
    const int                               ID = -1;
    printemps::multi_array::ValueProxy<int> value_proxy(ID);

    auto value     = random_integer();
    value_proxy[0] = value;
    EXPECT_EQ(value, value_proxy[0]);
}

/*****************************************************************************/
TEST_F(TestValueProxy, scalar_operator_round_bracket) {
    const int                               ID = -1;
    printemps::multi_array::ValueProxy<int> value_proxy(ID);

    auto value     = random_integer();
    value_proxy(0) = value;
    EXPECT_EQ(value, value_proxy(0));

    ASSERT_THROW(value_proxy(0, 1) = value, std::logic_error);
    ASSERT_THROW(value_proxy(0, 1, 2) = value, std::logic_error);
    ASSERT_THROW(value_proxy(0, 1, 2, 3) = value, std::logic_error);
}

/*****************************************************************************/
TEST_F(TestValueProxy, scalar_operator_equal) {
    const int                               ID = -1;
    printemps::multi_array::ValueProxy<int> value_proxy(ID);

    auto value = random_integer();
    EXPECT_EQ(value, (value_proxy = value)[0]);
    value_proxy = value;
    EXPECT_EQ(value, value_proxy[0]);
}

/*****************************************************************************/
TEST_F(TestValueProxy, one_dimensional_constructor) {
    const int                               ID = -1;
    printemps::multi_array::ValueProxy<int> value_proxy(ID, 2);

    EXPECT_EQ(ID, value_proxy.index());
    EXPECT_EQ(2, value_proxy.shape()[0]);
    EXPECT_EQ(1, value_proxy.strides()[0]);
    EXPECT_EQ(1, static_cast<int>(value_proxy.number_of_dimensions()));
    EXPECT_EQ(2, static_cast<int>(value_proxy.number_of_elements()));
    EXPECT_EQ(0, value_proxy.values(0));
}

/*****************************************************************************/
TEST_F(TestValueProxy, one_dimensional_value) {
    const int                               ID = -1;
    printemps::multi_array::ValueProxy<int> value_proxy(ID, 2);

    auto value = random_integer();
    ASSERT_THROW(value_proxy.value() = value, std::logic_error);
}

/*****************************************************************************/
TEST_F(TestValueProxy, one_dimensional_values) {
    const int                               ID = -1;
    printemps::multi_array::ValueProxy<int> value_proxy(ID, 2);

    auto value_0          = random_integer();
    auto value_1          = random_integer();
    value_proxy.values(0) = value_0;
    value_proxy.values(1) = value_1;
    EXPECT_EQ(value_0, value_proxy.values(0));
    EXPECT_EQ(value_1, value_proxy.values(1));

    ASSERT_THROW(value_proxy.values(0, 1) = value_0, std::logic_error);
    ASSERT_THROW(value_proxy.values(0, 1, 2) = value_0, std::logic_error);
    ASSERT_THROW(value_proxy.values(0, 1, 2, 3) = value_0, std::logic_error);
}

/*****************************************************************************/
TEST_F(TestValueProxy, one_dimensional_values_with_indices) {
    const int                               ID = -1;
    printemps::multi_array::ValueProxy<int> value_proxy(ID, 2);

    auto value_0            = random_integer();
    auto value_1            = random_integer();
    value_proxy.values({0}) = value_0;
    value_proxy.values({1}) = value_1;
    EXPECT_EQ(value_0, value_proxy.values({0}));
    EXPECT_EQ(value_1, value_proxy.values({1}));
}

/*****************************************************************************/
TEST_F(TestValueProxy, one_dimensional_fill) {
    const int                               ID = -1;
    printemps::multi_array::ValueProxy<int> value_proxy(ID, 2);

    auto value = random_integer();
    value_proxy.fill(value);
    EXPECT_EQ(value, value_proxy[0]);
    EXPECT_EQ(value, value_proxy[1]);
}

/*****************************************************************************/
TEST_F(TestValueProxy, one_dimensional_operator_square_bracket) {
    const int                               ID = -1;
    printemps::multi_array::ValueProxy<int> value_proxy(ID, 2);

    auto value_0   = random_integer();
    auto value_1   = random_integer();
    value_proxy[0] = value_0;
    value_proxy[1] = value_1;
    EXPECT_EQ(value_0, value_proxy[0]);
    EXPECT_EQ(value_1, value_proxy[1]);
}

/*****************************************************************************/
TEST_F(TestValueProxy, one_dimensional_operator_round_bracket) {
    const int                               ID = -1;
    printemps::multi_array::ValueProxy<int> value_proxy(ID, 2);

    auto value_0   = random_integer();
    auto value_1   = random_integer();
    value_proxy(0) = value_0;
    value_proxy(1) = value_1;
    EXPECT_EQ(value_0, value_proxy(0));
    EXPECT_EQ(value_1, value_proxy(1));

    ASSERT_THROW(value_proxy(0, 1) = value_0, std::logic_error);
    ASSERT_THROW(value_proxy(0, 1, 2) = value_0, std::logic_error);
    ASSERT_THROW(value_proxy(0, 1, 2, 3) = value_0, std::logic_error);
}

/*****************************************************************************/
TEST_F(TestValueProxy, one_dimensional_operator_equal) {
    const int                               ID = -1;
    printemps::multi_array::ValueProxy<int> value_proxy(ID, 2);

    auto value = random_integer();
    ASSERT_THROW(value_proxy = value, std::logic_error);
}

/*****************************************************************************/
TEST_F(TestValueProxy, two_dimensional_constructor) {
    const int                               ID = -1;
    printemps::multi_array::ValueProxy<int> value_proxy(ID, {2, 3});

    EXPECT_EQ(ID, value_proxy.index());
    EXPECT_EQ(2, value_proxy.shape()[0]);
    EXPECT_EQ(3, value_proxy.shape()[1]);
    EXPECT_EQ(3, value_proxy.strides()[0]);
    EXPECT_EQ(1, value_proxy.strides()[1]);
    EXPECT_EQ(2, static_cast<int>(value_proxy.number_of_dimensions()));
    EXPECT_EQ(2 * 3, static_cast<int>(value_proxy.number_of_elements()));
    EXPECT_EQ(0, value_proxy[0]);
    EXPECT_EQ(0, value_proxy[2 * 3 - 1]);
}

/*****************************************************************************/
TEST_F(TestValueProxy, two_dimensional_value) {
    const int                               ID = -1;
    printemps::multi_array::ValueProxy<int> value_proxy(ID, {2, 3});

    auto value = random_integer();
    ASSERT_THROW(value_proxy.value() = value, std::logic_error);
}

/*****************************************************************************/
TEST_F(TestValueProxy, two_dimensional_values) {
    const int                               ID = -1;
    printemps::multi_array::ValueProxy<int> value_proxy(ID, {2, 3});

    auto value_0             = random_integer();
    auto value_1             = random_integer();
    value_proxy.values(0, 0) = value_0;
    value_proxy.values(1, 2) = value_1;
    EXPECT_EQ(value_0, value_proxy.values(0, 0));
    EXPECT_EQ(value_1, value_proxy.values(1, 2));

    ASSERT_THROW(value_proxy.values(0) = value_0, std::logic_error);
    ASSERT_THROW(value_proxy.values(0, 1, 2) = value_0, std::logic_error);
    ASSERT_THROW(value_proxy.values(0, 1, 2, 3) = value_0, std::logic_error);
}

/*****************************************************************************/
TEST_F(TestValueProxy, two_dimensional_values_with_indices) {
    const int                               ID = -1;
    printemps::multi_array::ValueProxy<int> value_proxy(ID, {2, 3});

    auto value_0               = random_integer();
    auto value_1               = random_integer();
    value_proxy.values({0, 0}) = value_0;
    value_proxy.values({1, 2}) = value_1;
    EXPECT_EQ(value_0, value_proxy.values({0, 0}));
    EXPECT_EQ(value_1, value_proxy.values({1, 2}));
}

/*****************************************************************************/
TEST_F(TestValueProxy, two_dimensional_fill) {
    const int                               ID = -1;
    printemps::multi_array::ValueProxy<int> value_proxy(ID, {2, 3});

    auto value = random_integer();
    value_proxy.fill(value);
    EXPECT_EQ(value, value_proxy[0]);
    EXPECT_EQ(value, value_proxy[2 * 3 - 1]);
}

/*****************************************************************************/
TEST_F(TestValueProxy, two_dimensional_operator_square_bracket) {
    const int                               ID = -1;
    printemps::multi_array::ValueProxy<int> value_proxy(ID, {2, 3});

    auto value_0           = random_integer();
    auto value_1           = random_integer();
    value_proxy[0]         = value_0;
    value_proxy[2 * 3 - 1] = value_1;
    EXPECT_EQ(value_0, value_proxy[0]);
    EXPECT_EQ(value_1, value_proxy[2 * 3 - 1]);
}

/*****************************************************************************/
TEST_F(TestValueProxy, two_dimensional_operator_round_bracket) {
    const int                               ID = -1;
    printemps::multi_array::ValueProxy<int> value_proxy(ID, {2, 3});

    auto value_0      = random_integer();
    auto value_1      = random_integer();
    value_proxy(0, 0) = value_0;
    value_proxy(1, 2) = value_1;
    EXPECT_EQ(value_0, value_proxy(0, 0));
    EXPECT_EQ(value_1, value_proxy(1, 2));

    ASSERT_THROW(value_proxy(0) = value_0, std::logic_error);
    ASSERT_THROW(value_proxy(0, 1, 2) = value_0, std::logic_error);
    ASSERT_THROW(value_proxy(0, 1, 2, 3) = value_0, std::logic_error);
}

/*****************************************************************************/
TEST_F(TestValueProxy, two_dimensional_operator_equal) {
    const int                               ID = -1;
    printemps::multi_array::ValueProxy<int> value_proxy(ID, {2, 3});

    auto value = random_integer();
    ASSERT_THROW(value_proxy = value, std::logic_error);
}

/*****************************************************************************/
TEST_F(TestValueProxy, three_dimensional_constructor) {
    const int                               ID = -1;
    printemps::multi_array::ValueProxy<int> value_proxy(ID, {2, 3, 4});

    EXPECT_EQ(-1, value_proxy.index());
    EXPECT_EQ(2, value_proxy.shape()[0]);
    EXPECT_EQ(3, value_proxy.shape()[1]);
    EXPECT_EQ(4, value_proxy.shape()[2]);
    EXPECT_EQ(12, value_proxy.strides()[0]);
    EXPECT_EQ(4, value_proxy.strides()[1]);
    EXPECT_EQ(1, value_proxy.strides()[2]);
    EXPECT_EQ(3, static_cast<int>(value_proxy.number_of_dimensions()));
    EXPECT_EQ(2 * 3 * 4, static_cast<int>(value_proxy.number_of_elements()));
    EXPECT_EQ(0, value_proxy[0]);
    EXPECT_EQ(0, value_proxy[2 * 3 * 4 - 1]);
}

/*****************************************************************************/
TEST_F(TestValueProxy, three_dimensional_value) {
    const int                               ID = -1;
    printemps::multi_array::ValueProxy<int> value_proxy(ID, {2, 3, 4});

    auto value = random_integer();
    ASSERT_THROW(value_proxy.value() = value, std::logic_error);
}

/*****************************************************************************/
TEST_F(TestValueProxy, three_dimensional_values) {
    const int                               ID = -1;
    printemps::multi_array::ValueProxy<int> value_proxy(ID, {2, 3, 4});

    auto value_0         = random_integer();
    auto value_1         = random_integer();
    value_proxy(0, 0, 0) = value_0;
    value_proxy(1, 2, 3) = value_1;
    EXPECT_EQ(value_0, value_proxy(0, 0, 0));
    EXPECT_EQ(value_1, value_proxy(1, 2, 3));

    ASSERT_THROW(value_proxy.values(0) = value_0, std::logic_error);
    ASSERT_THROW(value_proxy.values(0, 1) = value_0, std::logic_error);
    ASSERT_THROW(value_proxy.values(0, 1, 2, 3) = value_0, std::logic_error);
}

/*****************************************************************************/
TEST_F(TestValueProxy, three_dimensional_operator_round_bracket) {
    const int                               ID = -1;
    printemps::multi_array::ValueProxy<int> value_proxy(ID, {2, 3, 4});

    auto value_0         = random_integer();
    auto value_1         = random_integer();
    value_proxy(0, 0, 0) = value_0;
    value_proxy(1, 2, 3) = value_1;
    EXPECT_EQ(value_0, value_proxy(0, 0, 0));
    EXPECT_EQ(value_1, value_proxy(1, 2, 3));

    ASSERT_THROW(value_proxy(0) = value_0, std::logic_error);
    ASSERT_THROW(value_proxy(0, 1) = value_0, std::logic_error);
    ASSERT_THROW(value_proxy(0, 1, 2, 3) = value_0, std::logic_error);
}

/*****************************************************************************/
TEST_F(TestValueProxy, four_dimensional_constructor) {
    const int                               ID = -1;
    printemps::multi_array::ValueProxy<int> value_proxy(ID, {2, 3, 4, 5});

    EXPECT_EQ(-1, value_proxy.index());
    EXPECT_EQ(2, value_proxy.shape()[0]);
    EXPECT_EQ(3, value_proxy.shape()[1]);
    EXPECT_EQ(4, value_proxy.shape()[2]);
    EXPECT_EQ(5, value_proxy.shape()[3]);
    EXPECT_EQ(60, value_proxy.strides()[0]);
    EXPECT_EQ(20, value_proxy.strides()[1]);
    EXPECT_EQ(5, value_proxy.strides()[2]);
    EXPECT_EQ(1, value_proxy.strides()[3]);
    EXPECT_EQ(4, static_cast<int>(value_proxy.number_of_dimensions()));
    EXPECT_EQ(2 * 3 * 4 * 5,
              static_cast<int>(value_proxy.number_of_elements()));
    EXPECT_EQ(0, value_proxy[0]);
    EXPECT_EQ(0, value_proxy[2 * 3 * 4 * 5 - 1]);
}

/*****************************************************************************/
TEST_F(TestValueProxy, four_dimensional_value) {
    const int                               ID = -1;
    printemps::multi_array::ValueProxy<int> value_proxy(ID, {2, 3, 4, 5});

    auto value = random_integer();
    ASSERT_THROW(value_proxy.value() = value, std::logic_error);
}

/*****************************************************************************/
TEST_F(TestValueProxy, four_dimensional_values) {
    const int                               ID = -1;
    printemps::multi_array::ValueProxy<int> value_proxy(ID, {2, 3, 4, 5});

    auto value_0                   = random_integer();
    auto value_1                   = random_integer();
    value_proxy.values(0, 0, 0, 0) = value_0;
    value_proxy.values(1, 2, 3, 4) = value_1;
    EXPECT_EQ(value_0, value_proxy.values(0, 0, 0, 0));
    EXPECT_EQ(value_1, value_proxy.values(1, 2, 3, 4));

    ASSERT_THROW(value_proxy.values(0) = value_0, std::logic_error);
    ASSERT_THROW(value_proxy.values(0, 1) = value_0, std::logic_error);
    ASSERT_THROW(value_proxy.values(0, 1, 2) = value_0, std::logic_error);
}

/*****************************************************************************/
TEST_F(TestValueProxy, four_dimensional_values_with_indices) {
    const int                               ID = -1;
    printemps::multi_array::ValueProxy<int> value_proxy(ID, {2, 3, 4});

    auto value_0                  = random_integer();
    auto value_1                  = random_integer();
    value_proxy.values({0, 0, 0}) = value_0;
    value_proxy.values({1, 2, 3}) = value_1;
    EXPECT_EQ(value_0, value_proxy.values({0, 0, 0}));
    EXPECT_EQ(value_1, value_proxy.values({1, 2, 3}));
}

/*****************************************************************************/
TEST_F(TestValueProxy, four_dimensional_operator_round_bracket) {
    const int                               ID = -1;
    printemps::multi_array::ValueProxy<int> value_proxy(ID, {2, 3, 4, 5});

    auto value_0            = random_integer();
    auto value_1            = random_integer();
    value_proxy(0, 0, 0, 0) = value_0;
    value_proxy(1, 2, 3, 4) = value_1;
    EXPECT_EQ(value_0, value_proxy(0, 0, 0, 0));
    EXPECT_EQ(value_1, value_proxy(1, 2, 3, 4));

    ASSERT_THROW(value_proxy(0) = value_0, std::logic_error);
    ASSERT_THROW(value_proxy(0, 1) = value_0, std::logic_error);
    ASSERT_THROW(value_proxy(0, 1, 2) = value_0, std::logic_error);
}

}  // namespace

/*****************************************************************************/
// END
/*****************************************************************************/