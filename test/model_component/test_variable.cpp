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
class TestVariable : public ::testing::Test {
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
TEST_F(TestVariable, initialize) {
    auto variable =
        printemps::model_component::Variable<int, double>::create_instance();

    /// Check the initial values of the base class members.
    EXPECT_EQ(0, variable.proxy_index());
    EXPECT_EQ(0, variable.flat_index());
    EXPECT_EQ(0, variable.multi_dimensional_index()[0]);
    EXPECT_EQ("", variable.name());

    /// Check the initial values of the derived class members.
    EXPECT_FALSE(variable.is_fixed());
    EXPECT_EQ(0, variable.value());
    EXPECT_EQ(printemps::constant::INT_HALF_MIN, variable.lower_bound());
    EXPECT_EQ(printemps::constant::INT_HALF_MAX, variable.upper_bound());
    EXPECT_FALSE(variable.has_bounds());
    EXPECT_FALSE(variable.is_objective_improvable());
    EXPECT_FALSE(variable.is_feasibility_improvable());
    EXPECT_TRUE(variable.has_lower_bound_margin());
    EXPECT_TRUE(variable.has_upper_bound_margin());
    EXPECT_FALSE(variable.has_uniform_sensitivity());
    EXPECT_EQ(0.0, variable.uniform_sensitivity());
    EXPECT_EQ(printemps::model_component::VariableSense::Integer,
              variable.sense());
    EXPECT_EQ(nullptr, variable.selection_ptr());
    EXPECT_TRUE(variable.related_constraint_ptrs().empty());
    EXPECT_TRUE(
        variable.related_zero_one_coefficient_constraint_ptrs().empty());
    EXPECT_EQ(nullptr, variable.dependent_constraint_ptr());
    EXPECT_TRUE(variable.constraint_sensitivities().empty());
    EXPECT_EQ(0.0, variable.objective_sensitivity());
}

/*****************************************************************************/
TEST_F(TestVariable, set_value_force) {
    auto variable =
        printemps::model_component::Variable<int, double>::create_instance();

    auto value_0 = random_integer();
    variable     = value_0;
    EXPECT_EQ(value_0, variable.value());

    auto value_1 = random_integer();
    variable.fix();
    variable.set_value_force(value_1);
    EXPECT_EQ(value_1, variable.value());
}

/*****************************************************************************/
TEST_F(TestVariable, set_value_if_mutable) {
    auto variable =
        printemps::model_component::Variable<int, double>::create_instance();

    auto value_0 = random_integer();
    variable     = value_0;
    EXPECT_EQ(value_0, variable.value());

    variable.fix();

    auto value_1 = random_integer();
    variable.set_value_if_mutable(value_1);
    EXPECT_EQ(value_0, variable.value());
}

/*****************************************************************************/
TEST_F(TestVariable, value) {
    auto variable =
        printemps::model_component::Variable<int, double>::create_instance();

    auto value = random_integer();
    variable   = value;
    EXPECT_EQ(value, variable.value());
}

/*****************************************************************************/
TEST_F(TestVariable, set_value) {
    auto variable =
        printemps::model_component::Variable<int, double>::create_instance();

    auto value = random_integer();
    variable.set_value(value);
    EXPECT_EQ(value, variable.value());

    variable.fix();
    EXPECT_EQ(value, (variable = value).value());
    ASSERT_THROW(variable = value + 1, std::logic_error);
}

/*****************************************************************************/
TEST_F(TestVariable, evaluate_arg_void) {
    auto variable =
        printemps::model_component::Variable<int, double>::create_instance();

    auto value = random_integer();
    variable   = value;
    EXPECT_EQ(value, variable.evaluate());
}

/*****************************************************************************/
TEST_F(TestVariable, evaluate_arg_move) {
    auto variable_0 =
        printemps::model_component::Variable<int, double>::create_instance();
    auto variable_1 =
        printemps::model_component::Variable<int, double>::create_instance();
    auto value_0_before = random_integer();
    auto value_1_before = random_integer();
    auto value_0_after  = random_integer();
    auto value_1_after  = random_integer();

    variable_0 = value_0_before;
    variable_1 = value_1_before;

    printemps::neighborhood::Move<int, double> move_0;
    printemps::neighborhood::Move<int, double> move_1;
    move_0.alterations.emplace_back(&variable_0, value_0_after);
    move_1.alterations.emplace_back(&variable_1, value_1_after);

    EXPECT_EQ(value_0_after, variable_0.evaluate(move_0));
    EXPECT_EQ(value_0_before, variable_0.evaluate(move_1));
    EXPECT_EQ(value_1_after, variable_1.evaluate(move_1));
    EXPECT_EQ(value_1_before, variable_1.evaluate(move_0));
}

/*****************************************************************************/
TEST_F(TestVariable, fix) {
    auto variable =
        printemps::model_component::Variable<int, double>::create_instance();
    variable.fix();
    EXPECT_TRUE(variable.is_fixed());
    variable.unfix();
    EXPECT_FALSE(variable.is_fixed());
}

/*****************************************************************************/
TEST_F(TestVariable, is_fixed) {
    /// This method is tested in fix().
}

/*****************************************************************************/
TEST_F(TestVariable, unfix) {
    /// This method is tested in fix().
}

/*****************************************************************************/
TEST_F(TestVariable, fix_by) {
    auto variable =
        printemps::model_component::Variable<int, double>::create_instance();

    auto value = random_integer();
    variable.fix_by(value);
    EXPECT_TRUE(variable.is_fixed());
    EXPECT_EQ(value, variable.value());
}

/*****************************************************************************/
TEST_F(TestVariable, sense) {
    auto variable =
        printemps::model_component::Variable<int, double>::create_instance();
    EXPECT_EQ(printemps::model_component::VariableSense::Integer,
              variable.sense());
    variable.set_bound(0, 1);
    EXPECT_EQ(printemps::model_component::VariableSense::Binary,
              variable.sense());
    variable.set_bound(0, 10);
    EXPECT_EQ(printemps::model_component::VariableSense::Integer,
              variable.sense());
}

/*****************************************************************************/
TEST_F(TestVariable, setup_sense_binary_or_integer) {
    auto variable =
        printemps::model_component::Variable<int, double>::create_instance();
    variable.set_bound(0, 1);
    printemps::model_component::Selection<int, double> selection;
    variable.set_selection_ptr(&selection);
    EXPECT_EQ(printemps::model_component::VariableSense::Selection,
              variable.sense());
    variable.setup_sense_binary_or_integer();
    EXPECT_EQ(printemps::model_component::VariableSense::Binary,
              variable.sense());
}

/*****************************************************************************/
TEST_F(TestVariable, set_bound) {
    auto variable =
        printemps::model_component::Variable<int, double>::create_instance();

    auto lower_bound = random_integer();
    auto upper_bound = lower_bound + random_positive_integer();

    variable.set_bound(lower_bound, upper_bound);
    EXPECT_EQ(lower_bound, variable.lower_bound());
    EXPECT_EQ(upper_bound, variable.upper_bound());
    EXPECT_TRUE(variable.has_bounds());

    ASSERT_THROW(variable.set_bound(upper_bound, lower_bound),
                 std::logic_error);

    variable.reset_bound();
    EXPECT_EQ(printemps::constant::INT_HALF_MIN, variable.lower_bound());
    EXPECT_EQ(printemps::constant::INT_HALF_MAX, variable.upper_bound());
    EXPECT_FALSE(variable.has_bounds());
}

/*****************************************************************************/
TEST_F(TestVariable, reset_bound) {
    /// This method is tested in set_bound().
}

/*****************************************************************************/
TEST_F(TestVariable, lower_bound) {
    /// This method is tested in set_bound().
}

/*****************************************************************************/
TEST_F(TestVariable, upper_bound) {
    /// This method is tested in set_bound().
}

/*****************************************************************************/
TEST_F(TestVariable, has_bounds) {
    /// This method is tested in set_bound().
}

/*****************************************************************************/
TEST_F(TestVariable, set_is_objective_improvable) {
    auto variable =
        printemps::model_component::Variable<int, double>::create_instance();
    EXPECT_FALSE(variable.is_objective_improvable());
    variable.set_is_objective_improvable(true);
    EXPECT_TRUE(variable.is_objective_improvable());
    variable.set_is_objective_improvable(false);
    EXPECT_FALSE(variable.is_objective_improvable());
}

/*****************************************************************************/
TEST_F(TestVariable, is_objective_improvable) {
    /// This method is tested in set_is_objective_improvable().
}

/*****************************************************************************/
TEST_F(TestVariable, set_is_feasibility_improvable) {
    auto variable =
        printemps::model_component::Variable<int, double>::create_instance();
    EXPECT_FALSE(variable.is_feasibility_improvable());
    variable.set_is_feasibility_improvable(true);
    EXPECT_TRUE(variable.is_feasibility_improvable());
    variable.set_is_feasibility_improvable(false);
    EXPECT_FALSE(variable.is_feasibility_improvable());
}

/*****************************************************************************/
TEST_F(TestVariable, is_feasibility_improvable) {
    /// This method is tested in set_is_feasibility_improvable().
}

/*****************************************************************************/
TEST_F(TestVariable, set_selection_ptr) {
    auto variable_0 =
        printemps::model_component::Variable<int, double>::create_instance();
    auto variable_1 =
        printemps::model_component::Variable<int, double>::create_instance();
    variable_0.set_bound(0, 1);
    variable_1.set_bound(0, 1);
    printemps::model_component::Selection<int, double> selection;
    variable_0.set_selection_ptr(&selection);
    variable_1.set_selection_ptr(&selection);

    variable_0.select();
    EXPECT_EQ(&variable_0, variable_0.selection_ptr()->selected_variable_ptr);
    EXPECT_EQ(&variable_0, variable_1.selection_ptr()->selected_variable_ptr);

    variable_1.select();
    EXPECT_EQ(&variable_1, variable_0.selection_ptr()->selected_variable_ptr);
    EXPECT_EQ(&variable_1, variable_1.selection_ptr()->selected_variable_ptr);
}

/*****************************************************************************/
TEST_F(TestVariable, selection_ptr) {
    /// This method is tested in set_selection_ptr().
}

/*****************************************************************************/
TEST_F(TestVariable, select) {
    /// This method is tested in set_selection_ptr().
}

/*****************************************************************************/
TEST_F(TestVariable, update_as_intermediate_variable) {
    {
        auto variable =
            printemps::model_component::Variable<int,
                                                 double>::create_instance();
        auto constraint =
            printemps::model_component::Constraint<int,
                                                   double>::create_instance();

        variable.set_dependent_constraint_ptr(&constraint);
        constraint = (variable <= 10);
        variable   = 0;
        constraint.update();
        variable.update_as_intermediate_variable();
        EXPECT_EQ(10, variable.value());
    }
    {
        auto variable =
            printemps::model_component::Variable<int,
                                                 double>::create_instance();
        auto constraint =
            printemps::model_component::Constraint<int,
                                                   double>::create_instance();

        variable.set_dependent_constraint_ptr(&constraint);
        constraint = (-variable <= 10);
        variable   = 0;
        constraint.update();
        variable.update_as_intermediate_variable();
        EXPECT_EQ(-10, variable.value());
    }
    {
        auto variable =
            printemps::model_component::Variable<int,
                                                 double>::create_instance();
        auto constraint =
            printemps::model_component::Constraint<int,
                                                   double>::create_instance();

        variable.set_dependent_constraint_ptr(&constraint);
        constraint = (variable >= 20);
        variable   = 0;
        constraint.update();
        variable.update_as_intermediate_variable();
        EXPECT_EQ(20, variable.value());
    }
    {
        auto variable =
            printemps::model_component::Variable<int,
                                                 double>::create_instance();
        auto constraint =
            printemps::model_component::Constraint<int,
                                                   double>::create_instance();

        variable.set_dependent_constraint_ptr(&constraint);
        constraint = (-variable >= 20);
        variable   = 0;
        constraint.update();
        variable.update_as_intermediate_variable();
        EXPECT_EQ(-20, variable.value());
    }
    {
        auto variable =
            printemps::model_component::Variable<int,
                                                 double>::create_instance();
        auto constraint =
            printemps::model_component::Constraint<int,
                                                   double>::create_instance();

        variable.set_dependent_constraint_ptr(&constraint);
        constraint = (variable == 30);
        variable   = 0;
        constraint.update();
        variable.update_as_intermediate_variable();
        EXPECT_EQ(30, variable.value());
    }
    {
        auto variable =
            printemps::model_component::Variable<int,
                                                 double>::create_instance();
        auto constraint =
            printemps::model_component::Constraint<int,
                                                   double>::create_instance();

        variable.set_dependent_constraint_ptr(&constraint);
        constraint = (-variable == 30);
        variable   = 50;
        constraint.update();
        variable.update_as_intermediate_variable();
        EXPECT_EQ(-30, variable.value());
    }
}

/*****************************************************************************/
TEST_F(TestVariable, register_related_constraint_ptr) {
    auto variable =
        printemps::model_component::Variable<int, double>::create_instance();
    auto constraint_0 =
        printemps::model_component::Constraint<int, double>::create_instance();
    auto constraint_1 =
        printemps::model_component::Constraint<int, double>::create_instance();

    EXPECT_TRUE(variable.related_constraint_ptrs().empty());
    EXPECT_FALSE(variable.related_constraint_ptrs().find(&constraint_0) !=
                 variable.related_constraint_ptrs().end());
    EXPECT_FALSE(variable.related_constraint_ptrs().find(&constraint_1) !=
                 variable.related_constraint_ptrs().end());

    variable.register_related_constraint_ptr(&constraint_0);
    EXPECT_EQ(1, static_cast<int>(variable.related_constraint_ptrs().size()));
    EXPECT_TRUE(variable.related_constraint_ptrs().find(&constraint_0) !=
                variable.related_constraint_ptrs().end());
    EXPECT_FALSE(variable.related_constraint_ptrs().find(&constraint_1) !=
                 variable.related_constraint_ptrs().end());

    variable.register_related_constraint_ptr(&constraint_1);
    EXPECT_EQ(2, static_cast<int>(variable.related_constraint_ptrs().size()));
    EXPECT_TRUE(variable.related_constraint_ptrs().find(&constraint_0) !=
                variable.related_constraint_ptrs().end());
    EXPECT_TRUE(variable.related_constraint_ptrs().find(&constraint_1) !=
                variable.related_constraint_ptrs().end());

    variable.register_related_constraint_ptr(&constraint_1);
    EXPECT_EQ(2, static_cast<int>(variable.related_constraint_ptrs().size()));
    EXPECT_TRUE(variable.related_constraint_ptrs().find(&constraint_0) !=
                variable.related_constraint_ptrs().end());
    EXPECT_TRUE(variable.related_constraint_ptrs().find(&constraint_1) !=
                variable.related_constraint_ptrs().end());

    variable.reset_related_constraint_ptrs();
    EXPECT_TRUE(variable.related_constraint_ptrs().empty());
    EXPECT_FALSE(variable.related_constraint_ptrs().find(&constraint_0) !=
                 variable.related_constraint_ptrs().end());
    EXPECT_FALSE(variable.related_constraint_ptrs().find(&constraint_1) !=
                 variable.related_constraint_ptrs().end());
}

/*****************************************************************************/
TEST_F(TestVariable, reset_related_constraint_ptrs) {
    /// This method is tested in register_related_constraint_ptr().
}

/*****************************************************************************/
TEST_F(TestVariable, related_constraint_ptrs) {
    /// This method is tested in register_related_constraint_ptr().
}

/*****************************************************************************/
TEST_F(TestVariable, setup_related_zero_one_coefficient_constraint_ptrs) {
    /// This method is tested in
    /// Model.setup_variable_related_zero_one_coefficient_constraint_ptrs().
}

/*****************************************************************************/
TEST_F(TestVariable, reset_setup_zero_one_coefficient_constraint_ptrs) {
    /// This method is tested in
    /// Model.setup_variable_related_zero_one_coefficient_constraint_ptrs().
}

/*****************************************************************************/
TEST_F(TestVariable, related_zero_one_coefficient_constraint_ptrs) {
    /// This method is tested in
    /// Model.setup_variable_related_zero_one_coefficient_constraint_ptrs().
}

/*****************************************************************************/
TEST_F(TestVariable, register_constraint_sensitivity) {
    auto variable =
        printemps::model_component::Variable<int, double>::create_instance();
    auto constraint_0 =
        printemps::model_component::Constraint<int, double>::create_instance();
    auto constraint_1 =
        printemps::model_component::Constraint<int, double>::create_instance();

    EXPECT_TRUE(variable.constraint_sensitivities().empty());

    variable.register_constraint_sensitivity(&constraint_0, 10);
    EXPECT_EQ(1, static_cast<int>(variable.constraint_sensitivities().size()));
    EXPECT_EQ(10, variable.constraint_sensitivities().at(&constraint_0));

    variable.register_constraint_sensitivity(&constraint_1, 20);
    EXPECT_EQ(2, static_cast<int>(variable.constraint_sensitivities().size()));
    EXPECT_EQ(20, variable.constraint_sensitivities().at(&constraint_1));

    variable.reset_constraint_sensitivities();
    EXPECT_TRUE(variable.constraint_sensitivities().empty());
}

/*****************************************************************************/
TEST_F(TestVariable, reset_constraint_sensitivities) {
    /// This method is tested in register_constraint_sensitivity().
}

/*****************************************************************************/
TEST_F(TestVariable, constraint_sensitivities) {
    /// This method is tested in register_constraint_sensitivity().
}

/*****************************************************************************/
TEST_F(TestVariable, setup_uniform_sensitivity) {
    {
        auto variable =
            printemps::model_component::Variable<int,
                                                 double>::create_instance();
        auto constraint_0 =
            printemps::model_component::Constraint<int,
                                                   double>::create_instance();
        auto constraint_1 =
            printemps::model_component::Constraint<int,
                                                   double>::create_instance();

        variable.register_constraint_sensitivity(&constraint_0, 10);
        variable.register_constraint_sensitivity(&constraint_1, 20);
        variable.setup_uniform_sensitivity();
        EXPECT_FALSE(variable.has_uniform_sensitivity());
    }

    {
        auto variable =
            printemps::model_component::Variable<int,
                                                 double>::create_instance();
        auto constraint_0 =
            printemps::model_component::Constraint<int,
                                                   double>::create_instance();
        auto constraint_1 =
            printemps::model_component::Constraint<int,
                                                   double>::create_instance();

        variable.register_constraint_sensitivity(&constraint_0, 10);
        variable.register_constraint_sensitivity(&constraint_1, 10);
        variable.setup_uniform_sensitivity();
        EXPECT_TRUE(variable.has_uniform_sensitivity());
        EXPECT_EQ(10.0, variable.uniform_sensitivity());
        variable.reset_constraint_sensitivities();
        EXPECT_FALSE(variable.has_uniform_sensitivity());
        EXPECT_EQ(0.0, variable.uniform_sensitivity());
    }
}

/*****************************************************************************/
TEST_F(TestVariable, has_uniform_sensitivity) {
    /// This method is tested in setup_uniform_sensitivity().
}

/*****************************************************************************/
TEST_F(TestVariable, uniform_sensitivity) {
    /// This method is tested in setup_uniform_sensitivity().
}

/*****************************************************************************/
TEST_F(TestVariable, set_dependent_constraint_ptr) {
    auto variable =
        printemps::model_component::Variable<int, double>::create_instance();
    auto constraint =
        printemps::model_component::Constraint<int, double>::create_instance();

    EXPECT_EQ(printemps::model_component::VariableSense::Integer,
              variable.sense());
    EXPECT_EQ(nullptr, variable.dependent_constraint_ptr());

    variable.set_dependent_constraint_ptr(&constraint);

    EXPECT_EQ(printemps::model_component::VariableSense::Intermediate,
              variable.sense());
    EXPECT_EQ(&constraint, variable.dependent_constraint_ptr());

    variable.reset_dependent_constraint_ptr();
    EXPECT_EQ(printemps::model_component::VariableSense::Integer,
              variable.sense());
    EXPECT_EQ(nullptr, variable.dependent_constraint_ptr());
}

/*****************************************************************************/
TEST_F(TestVariable, reset_dependent_constraint_ptr) {
    /// This method is tested in setup_uniform_sensitivity().
}

/*****************************************************************************/
TEST_F(TestVariable, dependent_constraint_ptr) {
    /// This method is tested in setup_uniform_sensitivity().
}

/*****************************************************************************/
TEST_F(TestVariable, set_objective_sensitivity) {
    auto variable =
        printemps::model_component::Variable<int, double>::create_instance();
    EXPECT_EQ(0.0, variable.objective_sensitivity());
    variable.set_objective_sensitivity(100.0);
    EXPECT_EQ(100.0, variable.objective_sensitivity());
}

/*****************************************************************************/
TEST_F(TestVariable, objective_sensitivity) {
    /// This method is tested in set_objective_sensitivity().
}

/*****************************************************************************/
TEST_F(TestVariable, update_margin) {
    auto variable =
        printemps::model_component::Variable<int, double>::create_instance();
    variable.set_bound(-10, 10);
    variable.set_value(-10);  /// includes update_margin()
    EXPECT_FALSE(variable.has_lower_bound_margin());
    EXPECT_TRUE(variable.has_upper_bound_margin());

    variable.set_value_if_mutable(10);  /// includes update_margin()
    EXPECT_TRUE(variable.has_lower_bound_margin());
    EXPECT_FALSE(variable.has_upper_bound_margin());

    variable.set_bound(-100, 100);  /// includes update_margin()
    EXPECT_TRUE(variable.has_lower_bound_margin());
    EXPECT_TRUE(variable.has_upper_bound_margin());

    variable = -100;  /// includes update_margin()
    EXPECT_FALSE(variable.has_lower_bound_margin());
    EXPECT_TRUE(variable.has_upper_bound_margin());

    variable.fix_by(100);  /// includes update_margin()
    EXPECT_TRUE(variable.has_lower_bound_margin());
    EXPECT_FALSE(variable.has_upper_bound_margin());
}

/*****************************************************************************/
TEST_F(TestVariable, has_lower_bound_margin) {
    /// This method is tested in update_margin().
}

/*****************************************************************************/
TEST_F(TestVariable, has_upper_bound_margin) {
    /// This method is tested in update_margin().
}

/*****************************************************************************/
TEST_F(TestVariable, to_expression) {
    auto variable =
        printemps::model_component::Variable<int, double>::create_instance();

    EXPECT_EQ(1, variable.to_expression().sensitivities().at(&variable));
}

/*****************************************************************************/
TEST_F(TestVariable, reference) {
    auto variable =
        printemps::model_component::Variable<int, double>::create_instance();

    EXPECT_EQ(&variable, variable.reference());
}

/*****************************************************************************/
TEST_F(TestVariable, operator_plus) {
    auto variable =
        printemps::model_component::Variable<int, double>::create_instance();

    EXPECT_EQ(1, (+variable).sensitivities().at(&variable));
}

/*****************************************************************************/
TEST_F(TestVariable, operator_minus) {
    auto variable =
        printemps::model_component::Variable<int, double>::create_instance();

    EXPECT_EQ(-1, (-variable).sensitivities().at(&variable));
}

/*****************************************************************************/
TEST_F(TestVariable, operator_equal_arg_t_variable) {
    auto variable =
        printemps::model_component::Variable<int, double>::create_instance();

    auto value = random_integer();
    variable   = value;
    EXPECT_EQ(value, variable.value());

    variable.fix();
    EXPECT_EQ(value, (variable = value).value());
    ASSERT_THROW(variable = value + 1, std::logic_error);
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/