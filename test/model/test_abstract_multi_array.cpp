/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <random>

#include <cppmh.h>

namespace {
/*****************************************************************************/
class TestAbstractMultiArray : public ::testing::Test {
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
TEST_F(TestAbstractMultiArray, scalar_create_instance) {
    /**
     * The VariableProxy class inherits the TestAbstractMultiArray class.
     */
    cppmh::model::Model<int, double> model;
    auto& variable_proxy = model.create_variable("x");
    EXPECT_EQ(0, variable_proxy.id());
    EXPECT_EQ(1, variable_proxy.shape()[0]);
    EXPECT_EQ(1, variable_proxy.strides()[0]);
    EXPECT_EQ(1, variable_proxy.number_of_dimensions());
    EXPECT_EQ(1, variable_proxy.number_of_elements());
}

/*****************************************************************************/
TEST_F(TestAbstractMultiArray, one_dimensional_create_instance) {
    cppmh::model::Model<int, double> model;

    auto& variable_proxy = model.create_variables("x", 2);
    EXPECT_EQ(0, variable_proxy.id());
    EXPECT_EQ(2, variable_proxy.shape()[0]);
    EXPECT_EQ(1, variable_proxy.strides()[0]);
    EXPECT_EQ(1, variable_proxy.number_of_dimensions());
    EXPECT_EQ(2, variable_proxy.number_of_elements());
}

/*****************************************************************************/
TEST_F(TestAbstractMultiArray, two_dimensional_create_instance) {
    cppmh::model::Model<int, double> model;
    auto& variable_proxy = model.create_variables("x", {2, 3});
    EXPECT_EQ(0, variable_proxy.id());
    EXPECT_EQ(2, variable_proxy.shape()[0]);
    EXPECT_EQ(3, variable_proxy.shape()[1]);
    EXPECT_EQ(3, variable_proxy.strides()[0]);
    EXPECT_EQ(1, variable_proxy.strides()[1]);
    EXPECT_EQ(2, variable_proxy.number_of_dimensions());
    EXPECT_EQ(2 * 3, variable_proxy.number_of_elements());
}

/*****************************************************************************/
TEST_F(TestAbstractMultiArray, three_dimensional_create_instance) {
    cppmh::model::Model<int, double> model;
    auto& variable_proxy = model.create_variables("x", {2, 3, 4});
    EXPECT_EQ(0, variable_proxy.id());
    EXPECT_EQ(2, variable_proxy.shape()[0]);
    EXPECT_EQ(3, variable_proxy.shape()[1]);
    EXPECT_EQ(4, variable_proxy.shape()[2]);
    EXPECT_EQ(12, variable_proxy.strides()[0]);
    EXPECT_EQ(4, variable_proxy.strides()[1]);
    EXPECT_EQ(1, variable_proxy.strides()[2]);
    EXPECT_EQ(3, variable_proxy.number_of_dimensions());
    EXPECT_EQ(2 * 3 * 4, variable_proxy.number_of_elements());
}

/*****************************************************************************/
TEST_F(TestAbstractMultiArray, four_dimensional_create_instance) {
    cppmh::model::Model<int, double> model;
    auto& variable_proxy = model.create_variables("x", {2, 3, 4, 5});
    EXPECT_EQ(0, variable_proxy.id());
    EXPECT_EQ(2, variable_proxy.shape()[0]);
    EXPECT_EQ(3, variable_proxy.shape()[1]);
    EXPECT_EQ(4, variable_proxy.shape()[2]);
    EXPECT_EQ(5, variable_proxy.shape()[3]);
    EXPECT_EQ(60, variable_proxy.strides()[0]);
    EXPECT_EQ(20, variable_proxy.strides()[1]);
    EXPECT_EQ(5, variable_proxy.strides()[2]);
    EXPECT_EQ(1, variable_proxy.strides()[3]);
    EXPECT_EQ(4, variable_proxy.number_of_dimensions());
    EXPECT_EQ(2 * 3 * 4 * 5, variable_proxy.number_of_elements());
}
/*****************************************************************************/
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/