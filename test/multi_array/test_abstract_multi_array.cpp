/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
using namespace printemps;
/*****************************************************************************/
class TestAbstractMultiArray : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestAbstractMultiArray, scalar_create_instance) {
    /**
     * The VariableProxy class inherits the TestAbstractMultiArray class.
     */
    model::Model<int, double> model;
    auto&                     variable_proxy = model.create_variable("x");
    EXPECT_EQ(0, variable_proxy.index());
    EXPECT_EQ(1, variable_proxy.shape()[0]);
    EXPECT_EQ(1, variable_proxy.strides()[0]);
    EXPECT_EQ(1, variable_proxy.number_of_dimensions());
    EXPECT_EQ(1, variable_proxy.number_of_elements());
}

/*****************************************************************************/
TEST_F(TestAbstractMultiArray, one_dimensional_create_instance) {
    model::Model<int, double> model;

    auto& variable_proxy = model.create_variables("x", 2);
    EXPECT_EQ(0, variable_proxy.index());
    EXPECT_EQ(2, variable_proxy.shape()[0]);
    EXPECT_EQ(1, variable_proxy.strides()[0]);
    EXPECT_EQ(1, variable_proxy.number_of_dimensions());
    EXPECT_EQ(2, variable_proxy.number_of_elements());
}

/*****************************************************************************/
TEST_F(TestAbstractMultiArray, two_dimensional_create_instance) {
    model::Model<int, double> model;
    auto& variable_proxy = model.create_variables("x", {2, 3});
    EXPECT_EQ(0, variable_proxy.index());
    EXPECT_EQ(2, variable_proxy.shape()[0]);
    EXPECT_EQ(3, variable_proxy.shape()[1]);
    EXPECT_EQ(3, variable_proxy.strides()[0]);
    EXPECT_EQ(1, variable_proxy.strides()[1]);
    EXPECT_EQ(2, variable_proxy.number_of_dimensions());
    EXPECT_EQ(2 * 3, variable_proxy.number_of_elements());
}

/*****************************************************************************/
TEST_F(TestAbstractMultiArray, three_dimensional_create_instance) {
    model::Model<int, double> model;
    auto& variable_proxy = model.create_variables("x", {2, 3, 4});
    EXPECT_EQ(0, variable_proxy.index());
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
    model::Model<int, double> model;
    auto& variable_proxy = model.create_variables("x", {2, 3, 4, 5});
    EXPECT_EQ(0, variable_proxy.index());
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
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/