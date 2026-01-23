/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
using namespace printemps;
/*****************************************************************************/
class TestModelMPSHandler : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestModelMPSHandler, import) {
    mps::MPS mps;
    mps.read_mps("./dat/mps/test_00.mps");

    model::Model<int, double> model;
    model.mps_handler().import(mps, true);
    model.builder().setup_structure();

    EXPECT_EQ(60, model.reference().number_of_variables());
    EXPECT_EQ(3, model.reference().number_of_constraints());
    EXPECT_TRUE(model.is_minimization());

    auto &variable_name_map   = model.reference().variable_name_map;
    auto &constraint_name_map = model.reference().constraint_name_map;

    {
        auto &c_1        = constraint_name_map.at("_C1");
        auto &expression = c_1->expression();

        auto &x_0 = variable_name_map.at("x_0");
        auto &x_9 = variable_name_map.at("x_9");
        EXPECT_FLOAT_EQ(1.0, expression.sensitivities().at(x_0));
        EXPECT_FLOAT_EQ(1.0, expression.sensitivities().at(x_9));
        EXPECT_EQ(model_component::ConstraintSense::Equal, c_1->sense());
        EXPECT_FLOAT_EQ(-1.0, expression.constant_value());
    }

    {
        auto &c_2        = constraint_name_map.at("_C2");
        auto &expression = c_2->expression();

        auto &y_0  = variable_name_map.at("y_0");
        auto &y_19 = variable_name_map.at("y_19");
        EXPECT_FLOAT_EQ(1.0, expression.sensitivities().at(y_0));
        EXPECT_FLOAT_EQ(20.0, expression.sensitivities().at(y_19));
        EXPECT_EQ(model_component::ConstraintSense::Less, c_2->sense());
        EXPECT_FLOAT_EQ(-2.0e7, expression.constant_value());
    }

    {
        auto &c_3        = constraint_name_map.at("_C3");
        auto &expression = c_3->expression();

        auto &z_0  = variable_name_map.at("z_0");
        auto &z_29 = variable_name_map.at("z_29");
        EXPECT_FLOAT_EQ(1.0, expression.sensitivities().at(z_0));
        EXPECT_FLOAT_EQ(30.0, expression.sensitivities().at(z_29));
        EXPECT_EQ(model_component::ConstraintSense::Greater, c_3->sense());
        EXPECT_FLOAT_EQ(-300.0, expression.constant_value());
    }

    EXPECT_FLOAT_EQ(1.0, variable_name_map.at("x_0")->objective_sensitivity());
    EXPECT_FLOAT_EQ(10.0, variable_name_map.at("x_9")->objective_sensitivity());

    EXPECT_FLOAT_EQ(0.0, variable_name_map.at("x_0")->lower_bound());
    EXPECT_FLOAT_EQ(1.0, variable_name_map.at("x_0")->upper_bound());

    EXPECT_FLOAT_EQ(constant::INT_HALF_MIN,
                    variable_name_map.at("y_0")->lower_bound());
    EXPECT_FLOAT_EQ(constant::INT_HALF_MAX,
                    variable_name_map.at("y_0")->upper_bound());

    EXPECT_FLOAT_EQ(-100.0, variable_name_map.at("z_0")->lower_bound());
    EXPECT_FLOAT_EQ(100.0, variable_name_map.at("z_0")->upper_bound());
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/