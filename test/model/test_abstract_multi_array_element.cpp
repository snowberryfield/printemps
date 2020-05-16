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
class TestAbstractMultiArrayElement : public ::testing::Test {
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
TEST_F(TestAbstractMultiArrayElement, initialize) {
    /**
     * The Variable class inherits the TestAbstractMultiArrayElement class.
     */
    auto element = cppmh::model::Variable<int, double>::create_instance();

    EXPECT_EQ(0, element.id());
    EXPECT_EQ(0, element.flat_index());
    EXPECT_EQ(0, element.multi_dimensional_index()[0]);
    EXPECT_EQ("", element.name());
}

/*****************************************************************************/
TEST_F(TestAbstractMultiArrayElement, set_id) {
    auto element = cppmh::model::Variable<int, double>::create_instance();

    auto id = random_integer();
    element.set_id(id);
    EXPECT_EQ(id, element.id());
}

/*****************************************************************************/
TEST_F(TestAbstractMultiArrayElement, id) {
    /// This method is tested in set_id().
}

/*****************************************************************************/
TEST_F(TestAbstractMultiArrayElement, set_flat_index) {
    auto element = cppmh::model::Variable<int, double>::create_instance();

    auto flat_index = random_integer();
    element.set_flat_index(flat_index);
    EXPECT_EQ(flat_index, element.flat_index());
}

/*****************************************************************************/
TEST_F(TestAbstractMultiArrayElement, flat_index) {
    /// This method is tested in set_flat_index().
}

/*****************************************************************************/
TEST_F(TestAbstractMultiArrayElement, set_multi_dimensional_index) {
    auto element = cppmh::model::Variable<int, double>::create_instance();

    auto multi_dimensional_index_0 = random_integer();
    auto multi_dimensional_index_1 = random_integer();

    element.set_multi_dimensional_index(
        {multi_dimensional_index_0, multi_dimensional_index_1});

    EXPECT_EQ(multi_dimensional_index_0, element.multi_dimensional_index()[0]);
    EXPECT_EQ(multi_dimensional_index_1, element.multi_dimensional_index()[1]);
}

/*****************************************************************************/
TEST_F(TestAbstractMultiArrayElement, set_name) {
    auto element = cppmh::model::Variable<int, double>::create_instance();

    std::string name = "name";
    element.set_name(name);

    EXPECT_EQ(name, element.name());
}

/*****************************************************************************/
TEST_F(TestAbstractMultiArrayElement, name) {
    /// This method is tested in set_name().
}

/*****************************************************************************/
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/