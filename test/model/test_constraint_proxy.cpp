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
class TestConstraintProxy : public ::testing::Test {
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
TEST_F(TestConstraintProxy, scalar_create_instance) {
    cppmh::model::Model<int, double> model;
    auto& constraint_proxy = model.create_constraint("c");
    EXPECT_EQ(0, constraint_proxy.id());
    EXPECT_EQ(1, constraint_proxy.shape()[0]);
    EXPECT_EQ(1, constraint_proxy.strides()[0]);
    EXPECT_EQ(1, static_cast<int>(constraint_proxy.number_of_dimensions()));
    EXPECT_EQ(1, static_cast<int>(constraint_proxy.number_of_elements()));
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, scalar_constraints_arg_void) {
    cppmh::model::Model<int, double> model;
    auto& constraint_proxy = model.create_constraint("c");

    auto variable = cppmh::model::Variable<int, double>::create_instance();

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
TEST_F(TestConstraintProxy, scalar_constraints_arg_int) {
    cppmh::model::Model<int, double> model;
    auto& constraint_proxy = model.create_constraint("c");

    auto variable = cppmh::model::Variable<int, double>::create_instance();

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
    cppmh::model::Model<int, double> model;
    auto& variable_proxy   = model.create_variable("x");
    auto& expression_proxy = model.create_expression("e");
    auto& constraint_proxy = model.create_constraint("c");

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
TEST_F(TestConstraintProxy, scalar_export_values) {
    cppmh::model::Model<int, double> model;
    auto& constraint_proxy = model.create_constraint("c");

    auto variable = cppmh::model::Variable<int, double>::create_instance();

    auto sensitivity = random_integer();
    auto target      = random_integer();

    constraint_proxy[0] = (sensitivity * variable == target);

    auto value = random_integer();

    variable = value;

    constraint_proxy[0].update();
    auto expected_value = sensitivity * value - target;

    EXPECT_EQ(expected_value, constraint_proxy.export_values().values(0));
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, scalar_export_violations) {
    cppmh::model::Model<int, double> model;
    auto& constraint_proxy = model.create_constraint("c");

    auto variable = cppmh::model::Variable<int, double>::create_instance();

    auto sensitivity = random_integer();
    auto target      = random_integer();

    constraint_proxy[0] = (sensitivity * variable == target);

    auto value = random_integer();

    variable = value;

    constraint_proxy[0].update();
    auto expected_value = abs(sensitivity * value - target);

    EXPECT_EQ(expected_value, constraint_proxy.export_violations().value());
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, scalar_is_enabled) {
    cppmh::model::Model<int, double> model;
    auto& constraint_proxy = model.create_constraint("e");
    constraint_proxy.disable();
    EXPECT_EQ(false, constraint_proxy.is_enabled());

    constraint_proxy.enable();
    EXPECT_EQ(true, constraint_proxy.is_enabled());

    constraint_proxy.disable();
    EXPECT_EQ(false, constraint_proxy.is_enabled());
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
    cppmh::model::Model<int, double> model;

    auto  variable = cppmh::model::Variable<int, double>::create_instance();
    auto& constraint_proxy = model.create_constraint("c");
    auto  sensitivity      = random_integer();

    constraint_proxy[0] = (sensitivity * variable == 0);

    EXPECT_EQ(sensitivity,
              constraint_proxy[0].expression().sensitivities().at(&variable));
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, scalar_operator_round_bracket) {
    cppmh::model::Model<int, double> model;

    auto  variable = cppmh::model::Variable<int, double>::create_instance();
    auto& constraint_proxy = model.create_constraint("c");
    auto  sensitivity      = random_integer();

    constraint_proxy(0) = (sensitivity * variable == 0);

    EXPECT_EQ(sensitivity,
              constraint_proxy(0).expression().sensitivities().at(&variable));
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, scalar_operator_round_bracket_with_indices) {
    cppmh::model::Model<int, double> model;

    auto  variable = cppmh::model::Variable<int, double>::create_instance();
    auto& constraint_proxy = model.create_constraint("c");
    auto  sensitivity      = random_integer();

    constraint_proxy({0}) = (sensitivity * variable == 0);

    EXPECT_EQ(sensitivity,
              constraint_proxy({0}).expression().sensitivities().at(&variable));
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, scalar_operator_equal_arg_constraint) {
    cppmh::model::Model<int, double> model;
    auto& constraint_proxy = model.create_constraint("c");

    auto variable = cppmh::model::Variable<int, double>::create_instance();

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
    cppmh::model::Model<int, double> model;

    auto& expression_proxy = model.create_expressions("e", 2);
    EXPECT_EQ(0, expression_proxy.id());
    EXPECT_EQ(2, expression_proxy.shape()[0]);
    EXPECT_EQ(1, expression_proxy.strides()[0]);
    EXPECT_EQ(1, static_cast<int>(expression_proxy.number_of_dimensions()));
    EXPECT_EQ(2, static_cast<int>(expression_proxy.number_of_elements()));
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, one_dimensional_constraints_arg_void) {
    cppmh::model::Model<int, double> model;
    auto& constraint_proxy = model.create_constraints("c", 2);

    auto variable = cppmh::model::Variable<int, double>::create_instance();

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
TEST_F(TestConstraintProxy, one_dimensional_constraints_arg_int) {
    cppmh::model::Model<int, double> model;
    auto& constraint_proxy = model.create_constraints("c", 2);

    auto variable = cppmh::model::Variable<int, double>::create_instance();

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
    cppmh::model::Model<int, double> model;
    auto& constraint_proxy = model.create_constraints("c", 2);

    ASSERT_THROW(constraint_proxy.constraint_value(), std::logic_error);
    ASSERT_THROW(constraint_proxy.violation_value(), std::logic_error);
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, one_dimensional_violation_value) {
    /// This method is tested in one_dimensional_constraint_value.
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, one_dimensional_export_values) {
    cppmh::model::Model<int, double> model;
    auto& constraint_proxy = model.create_constraints("c", 2);

    auto variable = cppmh::model::Variable<int, double>::create_instance();

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

    EXPECT_EQ(expected_value_0, constraint_proxy.export_values().values(0));
    EXPECT_EQ(expected_value_1, constraint_proxy.export_values().values(1));
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, one_dimensional_export_violations) {
    cppmh::model::Model<int, double> model;
    auto& constraint_proxy = model.create_constraints("c", 2);

    auto variable = cppmh::model::Variable<int, double>::create_instance();

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

    EXPECT_EQ(expected_value_0, constraint_proxy.export_violations().values(0));
    EXPECT_EQ(expected_value_1, constraint_proxy.export_violations().values(1));
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, one_dimensional_is_enabled) {
    cppmh::model::Model<int, double> model;
    auto& constraint_proxy = model.create_constraints("e", 2);
    constraint_proxy.disable();
    ASSERT_THROW(constraint_proxy.is_enabled(), std::logic_error);
    EXPECT_EQ(false, constraint_proxy[0].is_enabled());
    EXPECT_EQ(false, constraint_proxy[1].is_enabled());

    constraint_proxy.enable();
    ASSERT_THROW(constraint_proxy.is_enabled(), std::logic_error);
    EXPECT_EQ(true, constraint_proxy[0].is_enabled());
    EXPECT_EQ(true, constraint_proxy[1].is_enabled());

    constraint_proxy.disable();
    ASSERT_THROW(constraint_proxy.is_enabled(), std::logic_error);
    EXPECT_EQ(false, constraint_proxy[0].is_enabled());
    EXPECT_EQ(false, constraint_proxy[1].is_enabled());
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
    cppmh::model::Model<int, double> model;

    auto  variable = cppmh::model::Variable<int, double>::create_instance();
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
    cppmh::model::Model<int, double> model;

    auto  variable = cppmh::model::Variable<int, double>::create_instance();
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
    cppmh::model::Model<int, double> model;

    auto  variable = cppmh::model::Variable<int, double>::create_instance();
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
    cppmh::model::Model<int, double> model;
    auto& constraint_proxy = model.create_constraints("c", 2);

    auto variable = cppmh::model::Variable<int, double>::create_instance();

    auto sensitivity = random_integer();
    auto target      = random_integer();
    ASSERT_THROW(constraint_proxy = (sensitivity * variable == target),
                 std::logic_error);
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, two_dimensional_create_instance) {
    cppmh::model::Model<int, double> model;

    auto& expression_proxy = model.create_expressions("e", {2, 3});
    EXPECT_EQ(0, expression_proxy.id());
    EXPECT_EQ(2, expression_proxy.shape()[0]);
    EXPECT_EQ(3, expression_proxy.shape()[1]);
    EXPECT_EQ(3, expression_proxy.strides()[0]);
    EXPECT_EQ(1, expression_proxy.strides()[1]);
    EXPECT_EQ(2, static_cast<int>(expression_proxy.number_of_dimensions()));
    EXPECT_EQ(2 * 3, static_cast<int>(expression_proxy.number_of_elements()));
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, two_dimensional_constraints_arg_void) {
    cppmh::model::Model<int, double> model;
    auto& constraint_proxy = model.create_constraints("c", {2, 3});

    auto variable = cppmh::model::Variable<int, double>::create_instance();

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
TEST_F(TestConstraintProxy, two_dimensional_constraints_arg_int) {
    cppmh::model::Model<int, double> model;
    auto& constraint_proxy = model.create_constraints("c", {2, 3});

    auto variable = cppmh::model::Variable<int, double>::create_instance();

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
    cppmh::model::Model<int, double> model;
    auto& constraint_proxy = model.create_constraints("c", {2, 3});

    ASSERT_THROW(constraint_proxy.constraint_value(), std::logic_error);
    ASSERT_THROW(constraint_proxy.violation_value(), std::logic_error);
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, two_dimensional_violation_value) {
    /// This method is tested in two_dimensional_constraint_value.
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, two_dimensional_export_values) {
    cppmh::model::Model<int, double> model;
    auto& constraint_proxy = model.create_constraints("c", {2, 3});

    auto variable = cppmh::model::Variable<int, double>::create_instance();

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

    EXPECT_EQ(expected_value_0, constraint_proxy.export_values().values(0, 0));
    EXPECT_EQ(expected_value_1, constraint_proxy.export_values().values(1, 2));
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, two_dimensional_export_violations) {
    cppmh::model::Model<int, double> model;
    auto& constraint_proxy = model.create_constraints("c", {2, 3});

    auto variable = cppmh::model::Variable<int, double>::create_instance();

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
              constraint_proxy.export_violations().values(0, 0));
    EXPECT_EQ(expected_value_1,
              constraint_proxy.export_violations().values(1, 2));
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, two_dimensional_is_enabled) {
    cppmh::model::Model<int, double> model;
    auto& constraint_proxy = model.create_constraints("c", {2, 3});
    constraint_proxy.disable();
    ASSERT_THROW(constraint_proxy.is_enabled(), std::logic_error);
    EXPECT_EQ(false, constraint_proxy[0].is_enabled());
    EXPECT_EQ(false, constraint_proxy[2 * 3 - 1].is_enabled());

    constraint_proxy.enable();
    ASSERT_THROW(constraint_proxy.is_enabled(), std::logic_error);
    EXPECT_EQ(true, constraint_proxy[0].is_enabled());
    EXPECT_EQ(true, constraint_proxy[2 * 3 - 1].is_enabled());

    constraint_proxy.disable();
    ASSERT_THROW(constraint_proxy.is_enabled(), std::logic_error);
    EXPECT_EQ(false, constraint_proxy[0].is_enabled());
    EXPECT_EQ(false, constraint_proxy[2 * 3 - 1].is_enabled());
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
    cppmh::model::Model<int, double> model;

    auto  variable = cppmh::model::Variable<int, double>::create_instance();
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
    cppmh::model::Model<int, double> model;

    auto  variable = cppmh::model::Variable<int, double>::create_instance();
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
    cppmh::model::Model<int, double> model;

    auto  variable = cppmh::model::Variable<int, double>::create_instance();
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
    cppmh::model::Model<int, double> model;
    auto& constraint_proxy = model.create_constraints("c", {2, 3});

    auto variable = cppmh::model::Variable<int, double>::create_instance();

    auto sensitivity = random_integer();
    auto target      = random_integer();
    ASSERT_THROW(constraint_proxy = (sensitivity * variable == target),
                 std::logic_error);
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, three_dimensional_create_instance) {
    cppmh::model::Model<int, double> model;

    auto& constraint_proxy = model.create_constraints("c", {2, 3, 4});
    EXPECT_EQ(0, constraint_proxy.id());
    EXPECT_EQ(2, constraint_proxy.shape()[0]);
    EXPECT_EQ(3, constraint_proxy.shape()[1]);
    EXPECT_EQ(4, constraint_proxy.shape()[2]);
    EXPECT_EQ(12, constraint_proxy.strides()[0]);
    EXPECT_EQ(4, constraint_proxy.strides()[1]);
    EXPECT_EQ(1, constraint_proxy.strides()[2]);
    EXPECT_EQ(3, static_cast<int>(constraint_proxy.number_of_dimensions()));
    EXPECT_EQ(2 * 3 * 4,
              static_cast<int>(constraint_proxy.number_of_elements()));
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, three_dimensional_operator_round_bracket) {
    cppmh::model::Model<int, double> model;

    auto  variable = cppmh::model::Variable<int, double>::create_instance();
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
    cppmh::model::Model<int, double> model;

    auto  variable = cppmh::model::Variable<int, double>::create_instance();
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
    cppmh::model::Model<int, double> model;

    auto& constraint_proxy = model.create_constraints("c", {2, 3, 4, 5});
    EXPECT_EQ(0, constraint_proxy.id());
    EXPECT_EQ(2, constraint_proxy.shape()[0]);
    EXPECT_EQ(3, constraint_proxy.shape()[1]);
    EXPECT_EQ(4, constraint_proxy.shape()[2]);
    EXPECT_EQ(5, constraint_proxy.shape()[3]);
    EXPECT_EQ(60, constraint_proxy.strides()[0]);
    EXPECT_EQ(20, constraint_proxy.strides()[1]);
    EXPECT_EQ(5, constraint_proxy.strides()[2]);
    EXPECT_EQ(1, constraint_proxy.strides()[3]);
    EXPECT_EQ(4, static_cast<int>(constraint_proxy.number_of_dimensions()));
    EXPECT_EQ(2 * 3 * 4 * 5,
              static_cast<int>(constraint_proxy.number_of_elements()));
}

/*****************************************************************************/
TEST_F(TestConstraintProxy, four_dimensional_operator_round_bracket) {
    cppmh::model::Model<int, double> model;

    auto  variable = cppmh::model::Variable<int, double>::create_instance();
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
    cppmh::model::Model<int, double> model;

    auto  variable = cppmh::model::Variable<int, double>::create_instance();
    auto& constraint_proxy = model.create_constraints("c", {2, 3, 4, 5});
    auto  sentisivity_0    = random_integer();
    auto  sentisivity_1    = random_integer();

    constraint_proxy({0, 0, 0, 0}) = (sentisivity_0 * variable == 0);
    constraint_proxy({1, 2, 3, 4}) = (sentisivity_1 * variable == 0);
    EXPECT_EQ(sentisivity_0, constraint_proxy({0, 0, 0, 0})
                                 .expression()
                                 .sensitivities()
                                 .at(&variable));
    EXPECT_EQ(sentisivity_1, constraint_proxy({1, 2, 3, 4})
                                 .expression()
                                 .sensitivities()
                                 .at(&variable));
}

/*****************************************************************************/
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/