/*****************************************************************************/
// Copyright (c) 2020-2024 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
using namespace printemps;
/*****************************************************************************/
class TestConstraintProxy : public ::testing::Test {
   protected:
    utility::UniformRandom<std::uniform_int_distribution<>, int>
        m_random_integer;
    utility::UniformRandom<std::uniform_int_distribution<>, int>
        m_random_positive_integer;

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
TEST_F(TestConstraintProxy, scalar_create_instance) {
    model::Model<int, double> model;
    auto&                     constraint_proxy = model.create_constraint("c");

    /// Check the initial values of the base class members.
    EXPECT_EQ(0, constraint_proxy.index());
    EXPECT_EQ(1, constraint_proxy.shape()[0]);
    EXPECT_EQ(1, constraint_proxy.strides()[0]);
    EXPECT_EQ(1, constraint_proxy.number_of_dimensions());
    EXPECT_EQ(1, constraint_proxy.number_of_elements());
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, scalar_set_name) {
    model::Model<int, double> model;

    auto& constraint_proxy = model.create_constraint("c");
    constraint_proxy.set_name("_c");
    EXPECT_EQ("_c", constraint_proxy.name());
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, scalar_name) {
    /// This method is tested in scalar_set_name().
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, scalar_flat_indexed_constraints_arg_void) {
    model::Model<int, double> model;
    auto&                     constraint_proxy = model.create_constraint("c");

    auto variable = model_component::Variable<int, double>::create_instance();

    auto sensitivity = random_integer();
    auto target      = random_integer();

    for (auto&& constraint : constraint_proxy.flat_indexed_constraints()) {
        constraint = (sensitivity * variable == target);
    }

    EXPECT_EQ(sensitivity, constraint_proxy.flat_indexed_constraints()[0]
                               .expression()
                               .sensitivities()
                               .at(&variable));

    EXPECT_EQ(-target, constraint_proxy.flat_indexed_constraints()[0]
                           .expression()
                           .constant_value());
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, scalar_flat_indexed_constraints_arg_int) {
    model::Model<int, double> model;
    auto&                     constraint_proxy = model.create_constraint("c");

    auto variable = model_component::Variable<int, double>::create_instance();

    auto sensitivity = random_integer();
    auto target      = random_integer();

    constraint_proxy[0] = (sensitivity * variable == target);

    EXPECT_EQ(sensitivity, constraint_proxy.flat_indexed_constraints(0)
                               .expression()
                               .sensitivities()
                               .at(&variable));

    EXPECT_EQ(-target, constraint_proxy.flat_indexed_constraints(0)
                           .expression()
                           .constant_value());
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, scalar_constraint_value) {
    model::Model<int, double> model;
    auto&                     variable_proxy   = model.create_variable("x");
    auto&                     expression_proxy = model.create_expression("e");
    auto&                     constraint_proxy = model.create_constraint("c");

    auto sensitivity = random_integer();
    auto constant    = random_integer();
    auto target      = random_integer();

    expression_proxy = sensitivity * variable_proxy + constant;
    constraint_proxy = expression_proxy <= target;

    auto value     = random_integer();
    variable_proxy = value;

    auto constraint_expected_value = sensitivity * value + constant - target;
    auto violation_expected_value =
        std::max(sensitivity * value + constant - target, 0);

    model.update();

    EXPECT_EQ(constraint_expected_value, constraint_proxy.constraint_value());
    EXPECT_EQ(violation_expected_value, constraint_proxy.violation_value());
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, scalar_violation_value) {
    /// This method is tested in scalar_constraint_value.
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, scalar_export_values_and_names) {
    model::Model<int, double> model;
    auto&                     constraint_proxy = model.create_constraint("c");

    auto variable = model_component::Variable<int, double>::create_instance();

    auto sensitivity = random_integer();
    auto target      = random_integer();

    constraint_proxy[0] = (sensitivity * variable == target);

    auto value = random_integer();

    variable = value;

    constraint_proxy[0].update();
    auto expected_value = sensitivity * value - target;

    EXPECT_EQ(expected_value,
              constraint_proxy.export_values_and_names().values(0));
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, scalar_export_violations_and_names) {
    model::Model<int, double> model;
    auto&                     constraint_proxy = model.create_constraint("c");

    auto variable = model_component::Variable<int, double>::create_instance();

    auto sensitivity = random_integer();
    auto target      = random_integer();

    constraint_proxy[0] = (sensitivity * variable == target);

    auto value = random_integer();

    variable = value;

    constraint_proxy[0].update();
    auto expected_value = abs(sensitivity * value - target);

    EXPECT_EQ(expected_value,
              constraint_proxy.export_violations_and_names().value());
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, scalar_is_enabled) {
    model::Model<int, double> model;
    auto&                     constraint_proxy = model.create_constraint("e");
    constraint_proxy.disable();
    EXPECT_FALSE(constraint_proxy.is_enabled());

    constraint_proxy.enable();
    EXPECT_TRUE(constraint_proxy.is_enabled());

    constraint_proxy.disable();
    EXPECT_FALSE(constraint_proxy.is_enabled());
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, scalar_enable) {
    /// This method is tested in scalar_is_enabled().
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, scalar_disable) {
    /// This method is tested in scalar_is_enabled().
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, scalar_operator_square_bracket) {
    model::Model<int, double> model;

    auto  variable = model_component::Variable<int, double>::create_instance();
    auto& constraint_proxy = model.create_constraint("c");
    auto  sensitivity      = random_integer();

    constraint_proxy[0] = (sensitivity * variable == 0);

    EXPECT_EQ(sensitivity,
              constraint_proxy[0].expression().sensitivities().at(&variable));
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, scalar_operator_round_bracket) {
    model::Model<int, double> model;

    auto  variable = model_component::Variable<int, double>::create_instance();
    auto& constraint_proxy = model.create_constraint("c");
    auto  sensitivity      = random_integer();

    constraint_proxy(0) = (sensitivity * variable == 0);

    EXPECT_EQ(sensitivity,
              constraint_proxy(0).expression().sensitivities().at(&variable));
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, scalar_operator_round_bracket_with_indices) {
    model::Model<int, double> model;

    auto  variable = model_component::Variable<int, double>::create_instance();
    auto& constraint_proxy = model.create_constraint("c");
    auto  sensitivity      = random_integer();

    constraint_proxy({0}) = (sensitivity * variable == 0);

    EXPECT_EQ(sensitivity,
              constraint_proxy({0}).expression().sensitivities().at(&variable));
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, scalar_operator_equal_arg_constraint) {
    model::Model<int, double> model;
    auto&                     constraint_proxy = model.create_constraint("c");

    auto variable = model_component::Variable<int, double>::create_instance();

    auto sensitivity = random_integer();
    auto target      = random_integer();
    constraint_proxy = (sensitivity * variable == target);

    EXPECT_EQ(sensitivity, constraint_proxy.flat_indexed_constraints()[0]
                               .expression()
                               .sensitivities()
                               .at(&variable));

    EXPECT_EQ(-target, constraint_proxy.flat_indexed_constraints()[0]
                           .expression()
                           .constant_value());
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, one_dimensional_create_instance) {
    model::Model<int, double> model;

    auto& expression_proxy = model.create_expressions("e", 2);

    /// Check the initial values of the base class members.
    EXPECT_EQ(0, expression_proxy.index());
    EXPECT_EQ(2, expression_proxy.shape()[0]);
    EXPECT_EQ(1, expression_proxy.strides()[0]);
    EXPECT_EQ(1, expression_proxy.number_of_dimensions());
    EXPECT_EQ(2, expression_proxy.number_of_elements());
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, one_dimensional_set_name) {
    model::Model<int, double> model;

    auto& constraint_proxy = model.create_constraints("c", 2);

    ASSERT_THROW(constraint_proxy.set_name("_c"), std::runtime_error);
    ASSERT_THROW(constraint_proxy.name(), std::runtime_error);
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, one_dimensional_name) {
    /// This method is tested in one_dimensional_set_name().
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, one_dimensional_flat_indexed_constraints_arg_void) {
    model::Model<int, double> model;
    auto& constraint_proxy = model.create_constraints("c", 2);

    auto variable = model_component::Variable<int, double>::create_instance();

    auto sensitivity = random_integer();
    auto target      = random_integer();

    int i = 1;
    for (auto&& constraint : constraint_proxy.flat_indexed_constraints()) {
        constraint = (i * sensitivity * variable == i * target);
        i++;
    }

    EXPECT_EQ(sensitivity, constraint_proxy.flat_indexed_constraints()[0]
                               .expression()
                               .sensitivities()
                               .at(&variable));
    EXPECT_EQ(-target, constraint_proxy.flat_indexed_constraints()[0]
                           .expression()
                           .constant_value());

    EXPECT_EQ(sensitivity * 2, constraint_proxy.flat_indexed_constraints()[1]
                                   .expression()
                                   .sensitivities()
                                   .at(&variable));
    EXPECT_EQ(-target * 2, constraint_proxy.flat_indexed_constraints()[1]
                               .expression()
                               .constant_value());
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, one_dimensional_flat_indexed_constraints_arg_int) {
    model::Model<int, double> model;
    auto& constraint_proxy = model.create_constraints("c", 2);

    auto variable = model_component::Variable<int, double>::create_instance();

    auto sensitivity = random_integer();
    auto target      = random_integer();

    for (auto&& constraint : constraint_proxy.flat_indexed_constraints()) {
        constraint = (sensitivity * variable == target);
    }
    constraint_proxy.flat_indexed_constraints(0).expression() *= 1;
    constraint_proxy.flat_indexed_constraints(1).expression() *= 2;

    EXPECT_EQ(sensitivity, constraint_proxy.flat_indexed_constraints(0)
                               .expression()
                               .sensitivities()
                               .at(&variable));
    EXPECT_EQ(-target, constraint_proxy.flat_indexed_constraints(0)
                           .expression()
                           .constant_value());

    EXPECT_EQ(sensitivity * 2, constraint_proxy.flat_indexed_constraints(1)
                                   .expression()
                                   .sensitivities()
                                   .at(&variable));
    EXPECT_EQ(-target * 2, constraint_proxy.flat_indexed_constraints(1)
                               .expression()
                               .constant_value());
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, one_dimensional_constraint_value) {
    model::Model<int, double> model;
    auto& constraint_proxy = model.create_constraints("c", 2);

    ASSERT_THROW(constraint_proxy.constraint_value(), std::runtime_error);
    ASSERT_THROW(constraint_proxy.violation_value(), std::runtime_error);
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, one_dimensional_violation_value) {
    /// This method is tested in one_dimensional_constraint_value.
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, one_dimensional_export_values_and_names) {
    model::Model<int, double> model;
    auto& constraint_proxy = model.create_constraints("c", 2);

    auto variable = model_component::Variable<int, double>::create_instance();

    auto sensitivity = random_integer();
    auto target      = random_integer();

    int i = 1;
    for (auto&& constraint : constraint_proxy.flat_indexed_constraints()) {
        constraint = (i * sensitivity * variable == i * target);
        i++;
    }

    auto value = random_integer();

    variable = value;

    for (auto&& constraint : constraint_proxy.flat_indexed_constraints()) {
        constraint.update();
    }

    auto expected_value_0 = sensitivity * value - target;
    auto expected_value_1 = 2 * (sensitivity * value - target);

    EXPECT_EQ(expected_value_0,
              constraint_proxy.export_values_and_names().values(0));
    EXPECT_EQ(expected_value_1,
              constraint_proxy.export_values_and_names().values(1));
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, one_dimensional_export_violations_and_names) {
    model::Model<int, double> model;
    auto& constraint_proxy = model.create_constraints("c", 2);

    auto variable = model_component::Variable<int, double>::create_instance();

    auto sensitivity = random_integer();
    auto target      = random_integer();

    int i = 1;
    for (auto&& constraint : constraint_proxy.flat_indexed_constraints()) {
        constraint = (i * sensitivity * variable == i * target);
        i++;
    }

    auto value = random_integer();

    variable = value;

    for (auto&& constraint : constraint_proxy.flat_indexed_constraints()) {
        constraint.update();
    }

    auto expected_value_0 = abs(sensitivity * value - target);
    auto expected_value_1 = 2 * abs(sensitivity * value - target);

    EXPECT_EQ(expected_value_0,
              constraint_proxy.export_violations_and_names().values(0));
    EXPECT_EQ(expected_value_1,
              constraint_proxy.export_violations_and_names().values(1));
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, one_dimensional_is_enabled) {
    model::Model<int, double> model;
    auto& constraint_proxy = model.create_constraints("e", 2);
    constraint_proxy.disable();
    ASSERT_THROW(constraint_proxy.is_enabled(), std::runtime_error);
    EXPECT_FALSE(constraint_proxy[0].is_enabled());
    EXPECT_FALSE(constraint_proxy[1].is_enabled());

    constraint_proxy.enable();
    ASSERT_THROW(constraint_proxy.is_enabled(), std::runtime_error);
    EXPECT_TRUE(constraint_proxy[0].is_enabled());
    EXPECT_TRUE(constraint_proxy[1].is_enabled());

    constraint_proxy.disable();
    ASSERT_THROW(constraint_proxy.is_enabled(), std::runtime_error);
    EXPECT_FALSE(constraint_proxy[0].is_enabled());
    EXPECT_FALSE(constraint_proxy[1].is_enabled());
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, one_dimensional_enable) {
    /// This method is tested in one_dimensional_is_enabled().
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, one_dimensional_disable) {
    /// This method is tested in one_dimensional_is_enabled().
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, one_dimensional_operator_square_bracket) {
    model::Model<int, double> model;

    auto  variable = model_component::Variable<int, double>::create_instance();
    auto& constraint_proxy = model.create_constraints("c", 2);
    auto  sensitivity_0    = random_integer();
    auto  sensitivity_1    = random_integer();

    constraint_proxy[0] = (sensitivity_0 * variable == 0);
    constraint_proxy[1] = (sensitivity_1 * variable == 0);
    EXPECT_EQ(sensitivity_0,
              constraint_proxy[0].expression().sensitivities().at(&variable));
    EXPECT_EQ(sensitivity_1,
              constraint_proxy[1].expression().sensitivities().at(&variable));
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, one_dimensional_operator_round_bracket) {
    model::Model<int, double> model;

    auto  variable = model_component::Variable<int, double>::create_instance();
    auto& constraint_proxy = model.create_constraints("c", 2);
    auto  sensitivity_0    = random_integer();
    auto  sensitivity_1    = random_integer();

    constraint_proxy(0) = (sensitivity_0 * variable == 0);
    constraint_proxy(1) = (sensitivity_1 * variable == 0);
    EXPECT_EQ(sensitivity_0,
              constraint_proxy(0).expression().sensitivities().at(&variable));
    EXPECT_EQ(sensitivity_1,
              constraint_proxy(1).expression().sensitivities().at(&variable));
}

/*****************************************************************************/
TEST_F(TestConstraintProxy,
       one_dimensional_operator_round_bracket_with_indices) {
    model::Model<int, double> model;

    auto  variable = model_component::Variable<int, double>::create_instance();
    auto& constraint_proxy = model.create_constraints("c", 2);
    auto  sensitivity_0    = random_integer();
    auto  sensitivity_1    = random_integer();

    constraint_proxy({0}) = (sensitivity_0 * variable == 0);
    constraint_proxy({1}) = (sensitivity_1 * variable == 0);
    EXPECT_EQ(sensitivity_0,
              constraint_proxy({0}).expression().sensitivities().at(&variable));
    EXPECT_EQ(sensitivity_1,
              constraint_proxy({1}).expression().sensitivities().at(&variable));
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, one_dimensional_operator_equal_arg_constraint) {
    model::Model<int, double> model;
    auto& constraint_proxy = model.create_constraints("c", 2);

    auto variable = model_component::Variable<int, double>::create_instance();

    auto sensitivity = random_integer();
    auto target      = random_integer();
    ASSERT_THROW(constraint_proxy = (sensitivity * variable == target),
                 std::runtime_error);
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, two_dimensional_create_instance) {
    model::Model<int, double> model;

    auto& expression_proxy = model.create_expressions("e", {2, 3});

    /// Check the initial values of the base class members.
    EXPECT_EQ(0, expression_proxy.index());
    EXPECT_EQ(2, expression_proxy.shape()[0]);
    EXPECT_EQ(3, expression_proxy.shape()[1]);
    EXPECT_EQ(3, expression_proxy.strides()[0]);
    EXPECT_EQ(1, expression_proxy.strides()[1]);
    EXPECT_EQ(2, expression_proxy.number_of_dimensions());
    EXPECT_EQ(2 * 3, expression_proxy.number_of_elements());
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, two_dimensional_set_name) {
    model::Model<int, double> model;

    auto& constraint_proxy = model.create_constraints("c", {2, 3});

    ASSERT_THROW(constraint_proxy.set_name("_c"), std::runtime_error);
    ASSERT_THROW(constraint_proxy.name(), std::runtime_error);
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, two_dimensional_name) {
    /// This method is tested in two_dimensional_set_name().
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, two_dimensional_flat_indexed_constraints_arg_void) {
    model::Model<int, double> model;
    auto& constraint_proxy = model.create_constraints("c", {2, 3});

    auto variable = model_component::Variable<int, double>::create_instance();

    auto sensitivity = random_integer();
    auto target      = random_integer();

    int i = 1;
    for (auto&& constraint : constraint_proxy.flat_indexed_constraints()) {
        constraint = (i * sensitivity * variable == i * target);
        i++;
    }

    EXPECT_EQ(sensitivity, constraint_proxy.flat_indexed_constraints()[0]
                               .expression()
                               .sensitivities()
                               .at(&variable));
    EXPECT_EQ(-target, constraint_proxy.flat_indexed_constraints()[0]
                           .expression()
                           .constant_value());

    EXPECT_EQ(sensitivity * 2 * 3,
              constraint_proxy.flat_indexed_constraints()[2 * 3 - 1]
                  .expression()
                  .sensitivities()
                  .at(&variable));
    EXPECT_EQ(-target * 2 * 3,
              constraint_proxy.flat_indexed_constraints()[2 * 3 - 1]
                  .expression()
                  .constant_value());
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, two_dimensional_flat_indexed_constraints_arg_int) {
    model::Model<int, double> model;
    auto& constraint_proxy = model.create_constraints("c", {2, 3});

    auto variable = model_component::Variable<int, double>::create_instance();

    auto sensitivity = random_integer();
    auto target      = random_integer();

    for (auto&& constraint : constraint_proxy.flat_indexed_constraints()) {
        constraint = (sensitivity * variable == target);
    }
    constraint_proxy.flat_indexed_constraints(0).expression() *= 1;
    constraint_proxy.flat_indexed_constraints(2 * 3 - 1).expression() *= 2;

    EXPECT_EQ(sensitivity, constraint_proxy.flat_indexed_constraints(0)
                               .expression()
                               .sensitivities()
                               .at(&variable));
    EXPECT_EQ(-target, constraint_proxy.flat_indexed_constraints(0)
                           .expression()
                           .constant_value());

    EXPECT_EQ(sensitivity * 2,
              constraint_proxy.flat_indexed_constraints(2 * 3 - 1)
                  .expression()
                  .sensitivities()
                  .at(&variable));
    EXPECT_EQ(-target * 2, constraint_proxy.flat_indexed_constraints(2 * 3 - 1)
                               .expression()
                               .constant_value());
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, two_dimensional_constraint_value) {
    model::Model<int, double> model;
    auto& constraint_proxy = model.create_constraints("c", {2, 3});

    ASSERT_THROW(constraint_proxy.constraint_value(), std::runtime_error);
    ASSERT_THROW(constraint_proxy.violation_value(), std::runtime_error);
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, two_dimensional_violation_value) {
    /// This method is tested in two_dimensional_constraint_value.
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, two_dimensional_export_values_and_names) {
    model::Model<int, double> model;
    auto& constraint_proxy = model.create_constraints("c", {2, 3});

    auto variable = model_component::Variable<int, double>::create_instance();

    auto sensitivity = random_integer();
    auto target      = random_integer();

    int i = 1;
    for (auto&& constraint : constraint_proxy.flat_indexed_constraints()) {
        constraint = (i * sensitivity * variable == i * target);
        i++;
    }

    auto value = random_integer();

    variable = value;

    for (auto&& constraint : constraint_proxy.flat_indexed_constraints()) {
        constraint.update();
    }

    auto expected_value_0 = sensitivity * value - target;
    auto expected_value_1 = 2 * 3 * (sensitivity * value - target);

    EXPECT_EQ(expected_value_0,
              constraint_proxy.export_values_and_names().values(0, 0));
    EXPECT_EQ(expected_value_1,
              constraint_proxy.export_values_and_names().values(1, 2));
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, two_dimensional_export_violations_and_names) {
    model::Model<int, double> model;
    auto& constraint_proxy = model.create_constraints("c", {2, 3});

    auto variable = model_component::Variable<int, double>::create_instance();

    auto sensitivity = random_integer();
    auto target      = random_integer();

    int i = 1;
    for (auto&& constraint : constraint_proxy.flat_indexed_constraints()) {
        constraint = (i * sensitivity * variable == i * target);
        i++;
    }

    auto value = random_integer();

    variable = value;

    for (auto&& constraint : constraint_proxy.flat_indexed_constraints()) {
        constraint.update();
    }

    auto expected_value_0 = abs(sensitivity * value - target);
    auto expected_value_1 = 2 * 3 * abs(sensitivity * value - target);

    EXPECT_EQ(expected_value_0,
              constraint_proxy.export_violations_and_names().values(0, 0));
    EXPECT_EQ(expected_value_1,
              constraint_proxy.export_violations_and_names().values(1, 2));
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, two_dimensional_is_enabled) {
    model::Model<int, double> model;
    auto& constraint_proxy = model.create_constraints("c", {2, 3});
    constraint_proxy.disable();
    ASSERT_THROW(constraint_proxy.is_enabled(), std::runtime_error);
    EXPECT_FALSE(constraint_proxy[0].is_enabled());
    EXPECT_FALSE(constraint_proxy[2 * 3 - 1].is_enabled());

    constraint_proxy.enable();
    ASSERT_THROW(constraint_proxy.is_enabled(), std::runtime_error);
    EXPECT_TRUE(constraint_proxy[0].is_enabled());
    EXPECT_TRUE(constraint_proxy[2 * 3 - 1].is_enabled());

    constraint_proxy.disable();
    ASSERT_THROW(constraint_proxy.is_enabled(), std::runtime_error);
    EXPECT_FALSE(constraint_proxy[0].is_enabled());
    EXPECT_FALSE(constraint_proxy[2 * 3 - 1].is_enabled());
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, two_dimensional_enable) {
    /// This method is tested in two_dimensional_is_enabled().
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, two_dimensional_disable) {
    /// This method is tested in two_dimensional_is_enabled().
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, two_dimensional_operator_square_bracket) {
    model::Model<int, double> model;

    auto  variable = model_component::Variable<int, double>::create_instance();
    auto& constraint_proxy      = model.create_constraints("c", {2, 3});
    auto  sensitivity_0         = random_integer();
    auto  sensitivity_1         = random_integer();
    constraint_proxy[0]         = (sensitivity_0 * variable == 0);
    constraint_proxy[2 * 3 - 1] = (sensitivity_1 * variable == 0);
    EXPECT_EQ(sensitivity_0,
              constraint_proxy[0].expression().sensitivities().at(&variable));
    EXPECT_EQ(
        sensitivity_1,
        constraint_proxy[2 * 3 - 1].expression().sensitivities().at(&variable));
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, two_dimensional_operator_round_bracket) {
    model::Model<int, double> model;

    auto  variable = model_component::Variable<int, double>::create_instance();
    auto& constraint_proxy = model.create_constraints("c", {2, 3});
    auto  sensitivity_0    = random_integer();
    auto  sensitivity_1    = random_integer();

    constraint_proxy(0, 0) = (sensitivity_0 * variable == 0);
    constraint_proxy(1, 2) = (sensitivity_1 * variable == 0);
    EXPECT_EQ(
        sensitivity_0,
        constraint_proxy(0, 0).expression().sensitivities().at(&variable));
    EXPECT_EQ(
        sensitivity_1,
        constraint_proxy(1, 2).expression().sensitivities().at(&variable));
}

/*****************************************************************************/
TEST_F(TestConstraintProxy,
       two_dimensional_operator_round_bracket_with_indices) {
    model::Model<int, double> model;

    auto  variable = model_component::Variable<int, double>::create_instance();
    auto& constraint_proxy = model.create_constraints("c", {2, 3});
    auto  sensitivity_0    = random_integer();
    auto  sensitivity_1    = random_integer();

    constraint_proxy({0, 0}) = (sensitivity_0 * variable == 0);
    constraint_proxy({1, 2}) = (sensitivity_1 * variable == 0);
    EXPECT_EQ(
        sensitivity_0,
        constraint_proxy({0, 0}).expression().sensitivities().at(&variable));
    EXPECT_EQ(
        sensitivity_1,
        constraint_proxy({1, 2}).expression().sensitivities().at(&variable));
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, two_dimensional_operator_equal_arg_constraint) {
    model::Model<int, double> model;
    auto& constraint_proxy = model.create_constraints("c", {2, 3});

    auto variable = model_component::Variable<int, double>::create_instance();

    auto sensitivity = random_integer();
    auto target      = random_integer();
    ASSERT_THROW(constraint_proxy = (sensitivity * variable == target),
                 std::runtime_error);
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, three_dimensional_create_instance) {
    model::Model<int, double> model;

    auto& constraint_proxy = model.create_constraints("c", {2, 3, 4});

    /// Check the initial values of the base class members.
    EXPECT_EQ(0, constraint_proxy.index());
    EXPECT_EQ(2, constraint_proxy.shape()[0]);
    EXPECT_EQ(3, constraint_proxy.shape()[1]);
    EXPECT_EQ(4, constraint_proxy.shape()[2]);
    EXPECT_EQ(12, constraint_proxy.strides()[0]);
    EXPECT_EQ(4, constraint_proxy.strides()[1]);
    EXPECT_EQ(1, constraint_proxy.strides()[2]);
    EXPECT_EQ(3, constraint_proxy.number_of_dimensions());
    EXPECT_EQ(2 * 3 * 4, constraint_proxy.number_of_elements());
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, three_dimensional_operator_round_bracket) {
    model::Model<int, double> model;

    auto  variable = model_component::Variable<int, double>::create_instance();
    auto& constraint_proxy = model.create_constraints("c", {2, 3, 4});
    auto  sensitivity_0    = random_integer();
    auto  sensitivity_1    = random_integer();

    constraint_proxy(0, 0, 0) = (sensitivity_0 * variable == 0);
    constraint_proxy(1, 2, 3) = (sensitivity_1 * variable == 0);
    EXPECT_EQ(
        sensitivity_0,
        constraint_proxy(0, 0, 0).expression().sensitivities().at(&variable));
    EXPECT_EQ(
        sensitivity_1,
        constraint_proxy(1, 2, 3).expression().sensitivities().at(&variable));
}

/*****************************************************************************/
TEST_F(TestConstraintProxy,
       three_dimensional_operator_round_bracket_with_indices) {
    model::Model<int, double> model;

    auto  variable = model_component::Variable<int, double>::create_instance();
    auto& constraint_proxy = model.create_constraints("c", {2, 3, 4});
    auto  sensitivity_0    = random_integer();
    auto  sensitivity_1    = random_integer();

    constraint_proxy({0, 0, 0}) = (sensitivity_0 * variable == 0);
    constraint_proxy({1, 2, 3}) = (sensitivity_1 * variable == 0);
    EXPECT_EQ(
        sensitivity_0,
        constraint_proxy({0, 0, 0}).expression().sensitivities().at(&variable));
    EXPECT_EQ(
        sensitivity_1,
        constraint_proxy({1, 2, 3}).expression().sensitivities().at(&variable));
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, four_dimensional_create_instance) {
    model::Model<int, double> model;

    auto& constraint_proxy = model.create_constraints("c", {2, 3, 4, 5});

    /// Check the initial values of the base class members.
    EXPECT_EQ(0, constraint_proxy.index());
    EXPECT_EQ(2, constraint_proxy.shape()[0]);
    EXPECT_EQ(3, constraint_proxy.shape()[1]);
    EXPECT_EQ(4, constraint_proxy.shape()[2]);
    EXPECT_EQ(5, constraint_proxy.shape()[3]);
    EXPECT_EQ(60, constraint_proxy.strides()[0]);
    EXPECT_EQ(20, constraint_proxy.strides()[1]);
    EXPECT_EQ(5, constraint_proxy.strides()[2]);
    EXPECT_EQ(1, constraint_proxy.strides()[3]);
    EXPECT_EQ(4, constraint_proxy.number_of_dimensions());
    EXPECT_EQ(2 * 3 * 4 * 5, constraint_proxy.number_of_elements());
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, four_dimensional_operator_round_bracket) {
    model::Model<int, double> model;

    auto  variable = model_component::Variable<int, double>::create_instance();
    auto& constraint_proxy = model.create_constraints("c", {2, 3, 4, 5});
    auto  sensitivity_0    = random_integer();
    auto  sensitivity_1    = random_integer();

    constraint_proxy(0, 0, 0, 0) = (sensitivity_0 * variable == 0);
    constraint_proxy(1, 2, 3, 4) = (sensitivity_1 * variable == 0);
    EXPECT_EQ(sensitivity_0, constraint_proxy(0, 0, 0, 0)
                                 .expression()
                                 .sensitivities()
                                 .at(&variable));
    EXPECT_EQ(sensitivity_1, constraint_proxy(1, 2, 3, 4)
                                 .expression()
                                 .sensitivities()
                                 .at(&variable));
}

/*****************************************************************************/
TEST_F(TestConstraintProxy,
       four_dimensional_operator_round_bracket_with_indices) {
    model::Model<int, double> model;

    auto  variable = model_component::Variable<int, double>::create_instance();
    auto& constraint_proxy = model.create_constraints("c", {2, 3, 4, 5});
    auto  sensitivity_0    = random_integer();
    auto  sensitivity_1    = random_integer();

    constraint_proxy({0, 0, 0, 0}) = (sensitivity_0 * variable == 0);
    constraint_proxy({1, 2, 3, 4}) = (sensitivity_1 * variable == 0);
    EXPECT_EQ(sensitivity_0, constraint_proxy({0, 0, 0, 0})
                                 .expression()
                                 .sensitivities()
                                 .at(&variable));
    EXPECT_EQ(sensitivity_1, constraint_proxy({1, 2, 3, 4})
                                 .expression()
                                 .sensitivities()
                                 .at(&variable));
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/