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
class TestModelOPBHandler : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestModelOPBHandler, import) {
    opb::OPB opb;
    opb.read_opb("./test/dat/pb/test_00a.pb");

    model::Model<int, double> model;
    model.opb_handler().import(opb);
    model.builder().setup_structure();

    EXPECT_EQ(5, model.reference().number_of_variables());
    EXPECT_EQ(4, model.reference().number_of_constraints());
    EXPECT_TRUE(model.is_minimization());

    auto &variable_name_map   = model.reference().variable_name_map;
    auto &constraint_name_map = model.reference().constraint_name_map;

    auto &x1 = variable_name_map.at("x1");
    auto &x2 = variable_name_map.at("x2");
    auto &x3 = variable_name_map.at("x3");
    auto &x4 = variable_name_map.at("x4");
    auto &x5 = variable_name_map.at("x5");

    {
        auto &c_1        = constraint_name_map.at("hard_constraint_0");
        auto &expression = c_1->expression();

        EXPECT_FLOAT_EQ(1.0, expression.sensitivities().at(x1));
        EXPECT_FLOAT_EQ(4.0, expression.sensitivities().at(x2));
        EXPECT_FLOAT_EQ(-2.0, expression.sensitivities().at(x5));
        EXPECT_EQ(model_component::ConstraintSense::Greater, c_1->sense());
        EXPECT_FLOAT_EQ(-2.0, expression.constant_value());
    }

    {
        auto &c_2        = constraint_name_map.at("hard_constraint_1");
        auto &expression = c_2->expression();

        EXPECT_FLOAT_EQ(-1.0, expression.sensitivities().at(x1));
        EXPECT_FLOAT_EQ(4.0, expression.sensitivities().at(x2));
        EXPECT_FLOAT_EQ(-2.0, expression.sensitivities().at(x5));
        EXPECT_EQ(model_component::ConstraintSense::Greater, c_2->sense());
        EXPECT_FLOAT_EQ(-3.0, expression.constant_value());
    }

    {
        auto &c_3        = constraint_name_map.at("hard_constraint_2");
        auto &expression = c_3->expression();

        EXPECT_FLOAT_EQ(12345.0, expression.sensitivities().at(x4));
        EXPECT_FLOAT_EQ(4.0, expression.sensitivities().at(x3));
        EXPECT_EQ(model_component::ConstraintSense::Greater, c_3->sense());
        EXPECT_FLOAT_EQ(-10.0, expression.constant_value());
    }

    {
        auto &c_4        = constraint_name_map.at("hard_constraint_3");
        auto &expression = c_4->expression();

        EXPECT_FLOAT_EQ(2.0, expression.sensitivities().at(x2));
        EXPECT_FLOAT_EQ(3.0, expression.sensitivities().at(x4));
        EXPECT_FLOAT_EQ(2.0, expression.sensitivities().at(x1));
        EXPECT_FLOAT_EQ(3.0, expression.sensitivities().at(x5));
        EXPECT_EQ(model_component::ConstraintSense::Equal, c_4->sense());
        EXPECT_FLOAT_EQ(-5.0, expression.constant_value());
    }

    EXPECT_FLOAT_EQ(1.0, variable_name_map.at("x2")->objective_sensitivity());
    EXPECT_FLOAT_EQ(-1.0, variable_name_map.at("x3")->objective_sensitivity());

    EXPECT_FLOAT_EQ(0.0, variable_name_map.at("x1")->lower_bound());
    EXPECT_FLOAT_EQ(1.0, variable_name_map.at("x1")->upper_bound());

    EXPECT_FLOAT_EQ(0.0, variable_name_map.at("x2")->lower_bound());
    EXPECT_FLOAT_EQ(1.0, variable_name_map.at("x2")->upper_bound());

    EXPECT_FLOAT_EQ(0.0, variable_name_map.at("x3")->lower_bound());
    EXPECT_FLOAT_EQ(1.0, variable_name_map.at("x3")->upper_bound());

    EXPECT_FLOAT_EQ(0.0, variable_name_map.at("x4")->lower_bound());
    EXPECT_FLOAT_EQ(1.0, variable_name_map.at("x4")->upper_bound());

    EXPECT_FLOAT_EQ(0.0, variable_name_map.at("x5")->lower_bound());
    EXPECT_FLOAT_EQ(1.0, variable_name_map.at("x5")->upper_bound());
}
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/