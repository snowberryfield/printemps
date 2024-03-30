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
class TestVariable : public ::testing::Test {
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
TEST_F(TestVariable, initialize) {
    auto variable = model_component::Variable<int, double>::create_instance();

    /// Check the initial values of the base class members.
    EXPECT_EQ(0, variable.proxy_index());
    EXPECT_EQ(0, variable.flat_index());
    EXPECT_EQ(0, variable.multi_dimensional_index()[0]);
    EXPECT_EQ("", variable.name());

    /// Check the initial values of the derived class members.
    EXPECT_FALSE(variable.is_fixed());
    EXPECT_EQ(0, variable.value());
    EXPECT_EQ(constant::INT_HALF_MIN, variable.lower_bound());
    EXPECT_EQ(constant::INT_HALF_MAX, variable.upper_bound());
    EXPECT_FALSE(variable.has_bounds());
    EXPECT_EQ(0.0, variable.lagrangian_coefficient());
    EXPECT_FALSE(variable.is_objective_improvable());
    EXPECT_FALSE(variable.is_feasibility_improvable());
    EXPECT_TRUE(variable.has_lower_bound_margin());
    EXPECT_TRUE(variable.has_upper_bound_margin());
    EXPECT_EQ(model_component::VariableSense::Integer, variable.sense());
    EXPECT_EQ(nullptr, variable.selection_ptr());
    EXPECT_TRUE(variable.related_constraint_ptrs().empty());
    EXPECT_TRUE(variable.related_binary_coefficient_constraint_ptrs().empty());
    EXPECT_EQ(nullptr, variable.dependent_expression_ptr());
    EXPECT_TRUE(variable.constraint_sensitivities().empty());
    EXPECT_EQ(0.0, variable.objective_sensitivity());
    EXPECT_EQ(0, variable.hash());
    EXPECT_EQ(-1, variable.related_selection_constraint_ptr_index_min());
    EXPECT_EQ(-1, variable.related_selection_constraint_ptr_index_max());
}

/*****************************************************************************/
TEST_F(TestVariable, set_value_force) {
    auto variable = model_component::Variable<int, double>::create_instance();

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
    auto variable = model_component::Variable<int, double>::create_instance();

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
    auto variable = model_component::Variable<int, double>::create_instance();

    auto value = random_integer();
    variable   = value;
    EXPECT_EQ(value, variable.value());
}

/*****************************************************************************/
TEST_F(TestVariable, set_value) {
    auto variable = model_component::Variable<int, double>::create_instance();

    auto value = random_integer();
    variable.set_value(value);
    EXPECT_EQ(value, variable.value());

    variable.fix();
    EXPECT_EQ(value, (variable = value).value());
    ASSERT_THROW(variable = value + 1, std::logic_error);
}

/*****************************************************************************/
TEST_F(TestVariable, evaluate_arg_void) {
    auto variable = model_component::Variable<int, double>::create_instance();

    auto value = random_integer();
    variable   = value;
    EXPECT_EQ(value, variable.evaluate());
}

/*****************************************************************************/
TEST_F(TestVariable, evaluate_arg_move) {
    auto variable_0 = model_component::Variable<int, double>::create_instance();
    auto variable_1 = model_component::Variable<int, double>::create_instance();
    auto value_0_before = random_integer();
    auto value_1_before = random_integer();
    auto value_0_after  = random_integer();
    auto value_1_after  = random_integer();

    variable_0 = value_0_before;
    variable_1 = value_1_before;

    neighborhood::Move<int, double> move_0;
    neighborhood::Move<int, double> move_1;
    move_0.alterations.emplace_back(&variable_0, value_0_after);
    move_1.alterations.emplace_back(&variable_1, value_1_after);

    EXPECT_EQ(value_0_after, variable_0.evaluate(move_0));
    EXPECT_EQ(value_0_before, variable_0.evaluate(move_1));
    EXPECT_EQ(value_1_after, variable_1.evaluate(move_1));
    EXPECT_EQ(value_1_before, variable_1.evaluate(move_0));
}

/*****************************************************************************/
TEST_F(TestVariable, fix) {
    auto variable = model_component::Variable<int, double>::create_instance();
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
    auto variable = model_component::Variable<int, double>::create_instance();

    auto value = random_integer();
    variable.fix_by(value);
    EXPECT_TRUE(variable.is_fixed());
    EXPECT_EQ(value, variable.value());
}

/*****************************************************************************/
TEST_F(TestVariable, set_bound) {
    auto variable = model_component::Variable<int, double>::create_instance();

    auto lower_bound = random_integer();
    auto upper_bound = lower_bound + random_positive_integer();

    variable.set_bound(lower_bound, upper_bound);
    EXPECT_EQ(lower_bound, variable.lower_bound());
    EXPECT_EQ(upper_bound, variable.upper_bound());
    EXPECT_TRUE(variable.has_bounds());
    EXPECT_EQ(upper_bound - lower_bound, variable.range());

    ASSERT_THROW(variable.set_bound(upper_bound, lower_bound),
                 std::logic_error);

    variable.reset_bound();
    EXPECT_EQ(constant::INT_HALF_MIN, variable.lower_bound());
    EXPECT_EQ(constant::INT_HALF_MAX, variable.upper_bound());
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
TEST_F(TestVariable, range) {
    /// This method is tested in set_bound().
}

/*****************************************************************************/
TEST_F(TestVariable, set_lagrangian_coefficient) {
    auto variable = model_component::Variable<int, double>::create_instance();
    variable.set_lagrangian_coefficient(10.0);
    EXPECT_FALSE(variable.has_bounds());
}
/*****************************************************************************/
TEST_F(TestVariable, lagrangian_coefficient) {
    /// This method is tested in lagrangian_coefficient().
}

/*****************************************************************************/
TEST_F(TestVariable, set_is_objective_improvable) {
    auto variable = model_component::Variable<int, double>::create_instance();
    EXPECT_FALSE(variable.is_objective_improvable());
    variable.set_is_objective_improvable(true);
    EXPECT_TRUE(variable.is_objective_improvable());
    variable.set_is_objective_improvable(false);
    EXPECT_FALSE(variable.is_objective_improvable());
}

/*****************************************************************************/
TEST_F(TestVariable, set_is_objective_improvable_or) {
    auto variable = model_component::Variable<int, double>::create_instance();
    EXPECT_FALSE(variable.is_objective_improvable());
    variable.set_is_objective_improvable_or(true);
    EXPECT_TRUE(variable.is_objective_improvable());
    variable.set_is_objective_improvable_or(false);
    EXPECT_TRUE(variable.is_objective_improvable());
}

/*****************************************************************************/
TEST_F(TestVariable, set_is_objective_improvable_and) {
    auto variable = model_component::Variable<int, double>::create_instance();
    EXPECT_FALSE(variable.is_objective_improvable());
    variable.set_is_objective_improvable(true);
    EXPECT_TRUE(variable.is_objective_improvable());
    variable.set_is_objective_improvable_and(false);
    EXPECT_FALSE(variable.is_objective_improvable());
}

/*****************************************************************************/
TEST_F(TestVariable, is_objective_improvable) {
    /// This method is tested in set_is_objective_improvable().
}

/*****************************************************************************/
TEST_F(TestVariable, set_is_feasibility_improvable) {
    auto variable = model_component::Variable<int, double>::create_instance();
    EXPECT_FALSE(variable.is_feasibility_improvable());
    variable.set_is_feasibility_improvable(true);
    EXPECT_TRUE(variable.is_feasibility_improvable());
    variable.set_is_feasibility_improvable(false);
    EXPECT_FALSE(variable.is_feasibility_improvable());
}

/*****************************************************************************/
TEST_F(TestVariable, set_is_feasibility_improvable_or) {
    auto variable = model_component::Variable<int, double>::create_instance();
    EXPECT_FALSE(variable.is_feasibility_improvable());
    variable.set_is_feasibility_improvable_or(true);
    EXPECT_TRUE(variable.is_feasibility_improvable());
    variable.set_is_feasibility_improvable_or(false);
    EXPECT_TRUE(variable.is_feasibility_improvable());
}

/*****************************************************************************/
TEST_F(TestVariable, set_is_feasibility_improvable_and) {
    auto variable = model_component::Variable<int, double>::create_instance();
    EXPECT_FALSE(variable.is_feasibility_improvable());
    variable.set_is_feasibility_improvable(true);
    EXPECT_TRUE(variable.is_feasibility_improvable());
    variable.set_is_feasibility_improvable_and(false);
    EXPECT_FALSE(variable.is_feasibility_improvable());
}

/*****************************************************************************/
TEST_F(TestVariable, is_feasibility_improvable) {
    /// This method is tested in set_is_feasibility_improvable().
}

/*****************************************************************************/
TEST_F(TestVariable, is_improvable) {
    {
        auto variable =
            model_component::Variable<int, double>::create_instance();
        variable.set_is_objective_improvable(false);
        variable.set_is_feasibility_improvable(false);
        EXPECT_FALSE(variable.is_improvable());
    }

    {
        auto variable =
            model_component::Variable<int, double>::create_instance();
        variable.set_is_objective_improvable(true);
        variable.set_is_feasibility_improvable(false);
        EXPECT_TRUE(variable.is_improvable());
    }

    {
        auto variable =
            model_component::Variable<int, double>::create_instance();
        variable.set_is_objective_improvable(false);
        variable.set_is_feasibility_improvable(true);
        EXPECT_TRUE(variable.is_improvable());
    }

    {
        auto variable =
            model_component::Variable<int, double>::create_instance();
        variable.set_is_objective_improvable(true);
        variable.set_is_feasibility_improvable(true);
        EXPECT_TRUE(variable.is_improvable());
    }
}

/*****************************************************************************/
TEST_F(TestVariable, set_sense) {
    auto variable = model_component::Variable<int, double>::create_instance();
    variable.set_sense(model_component::VariableSense::Binary);
    EXPECT_EQ(model_component::VariableSense::Binary, variable.sense());
    EXPECT_EQ("Binary", variable.sense_label());

    variable.set_sense(model_component::VariableSense::Integer);
    EXPECT_EQ(model_component::VariableSense::Integer, variable.sense());
    EXPECT_EQ("Integer", variable.sense_label());

    variable.set_sense(model_component::VariableSense::Selection);
    EXPECT_EQ(model_component::VariableSense::Selection, variable.sense());
    EXPECT_EQ("Selection", variable.sense_label());

    variable.set_sense(model_component::VariableSense::DependentBinary);
    EXPECT_EQ(model_component::VariableSense::DependentBinary,
              variable.sense());
    EXPECT_EQ("DependentBinary", variable.sense_label());

    variable.set_sense(model_component::VariableSense::DependentInteger);
    EXPECT_EQ(model_component::VariableSense::DependentInteger,
              variable.sense());
    EXPECT_EQ("DependentInteger", variable.sense_label());
}

/*****************************************************************************/
TEST_F(TestVariable, sense) {
    /// This method is tested in set_sense().
}

/*****************************************************************************/
TEST_F(TestVariable, sense_label) {
    /// This method is tested in set_sense().
}

/*****************************************************************************/
TEST_F(TestVariable, setup_sense_binary_or_integer) {
    auto variable = model_component::Variable<int, double>::create_instance();
    variable.set_bound(0, 1);
    model_component::Selection<int, double> selection;
    variable.set_selection_ptr(&selection);
    EXPECT_EQ(model_component::VariableSense::Selection, variable.sense());
    variable.setup_sense_binary_or_integer();
    EXPECT_EQ(model_component::VariableSense::Binary, variable.sense());
}

/*****************************************************************************/
TEST_F(TestVariable, set_selection_ptr) {
    auto variable_0 = model_component::Variable<int, double>::create_instance();
    auto variable_1 = model_component::Variable<int, double>::create_instance();
    variable_0.set_bound(0, 1);
    variable_1.set_bound(0, 1);
    model_component::Selection<int, double> selection;
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
TEST_F(TestVariable, update) {
    auto variable_0 = model_component::Variable<int, double>::create_instance();
    auto variable_1 = model_component::Variable<int, double>::create_instance();

    auto expression = 2 * variable_0 + 1;
    variable_1.set_dependent_expression_ptr(&expression);

    variable_0 = 5;
    expression.update();
    variable_1.update();
    EXPECT_EQ(11, variable_1.value());
}

/*****************************************************************************/
TEST_F(TestVariable, register_related_constraint_ptr) {
    auto variable = model_component::Variable<int, double>::create_instance();
    auto constraint_0 =
        model_component::Constraint<int, double>::create_instance();
    auto constraint_1 =
        model_component::Constraint<int, double>::create_instance();

    EXPECT_TRUE(variable.related_constraint_ptrs().empty());
    EXPECT_FALSE(std::find(variable.related_constraint_ptrs().begin(),
                           variable.related_constraint_ptrs().end(),
                           &constraint_0) !=
                 variable.related_constraint_ptrs().end());
    EXPECT_FALSE(std::find(variable.related_constraint_ptrs().begin(),
                           variable.related_constraint_ptrs().end(),
                           &constraint_1) !=
                 variable.related_constraint_ptrs().end());

    variable.register_related_constraint_ptr(&constraint_0);
    variable.sort_and_unique_related_constraint_ptrs();
    EXPECT_EQ(1, static_cast<int>(variable.related_constraint_ptrs().size()));
    EXPECT_TRUE(std::find(variable.related_constraint_ptrs().begin(),
                          variable.related_constraint_ptrs().end(),
                          &constraint_0) !=
                variable.related_constraint_ptrs().end());
    EXPECT_FALSE(std::find(variable.related_constraint_ptrs().begin(),
                           variable.related_constraint_ptrs().end(),
                           &constraint_1) !=
                 variable.related_constraint_ptrs().end());

    variable.register_related_constraint_ptr(&constraint_1);
    variable.sort_and_unique_related_constraint_ptrs();
    EXPECT_EQ(2, static_cast<int>(variable.related_constraint_ptrs().size()));
    EXPECT_TRUE(std::find(variable.related_constraint_ptrs().begin(),
                          variable.related_constraint_ptrs().end(),
                          &constraint_0) !=
                variable.related_constraint_ptrs().end());
    EXPECT_TRUE(std::find(variable.related_constraint_ptrs().begin(),
                          variable.related_constraint_ptrs().end(),
                          &constraint_1) !=
                variable.related_constraint_ptrs().end());

    variable.register_related_constraint_ptr(&constraint_1);
    variable.sort_and_unique_related_constraint_ptrs();
    EXPECT_EQ(2, static_cast<int>(variable.related_constraint_ptrs().size()));
    EXPECT_TRUE(std::find(variable.related_constraint_ptrs().begin(),
                          variable.related_constraint_ptrs().end(),
                          &constraint_0) !=
                variable.related_constraint_ptrs().end());
    EXPECT_TRUE(std::find(variable.related_constraint_ptrs().begin(),
                          variable.related_constraint_ptrs().end(),
                          &constraint_1) !=
                variable.related_constraint_ptrs().end());

    variable.reset_related_constraint_ptrs();
    EXPECT_TRUE(variable.related_constraint_ptrs().empty());
    EXPECT_FALSE(std::find(variable.related_constraint_ptrs().begin(),
                           variable.related_constraint_ptrs().end(),
                           &constraint_0) !=
                 variable.related_constraint_ptrs().end());
    EXPECT_FALSE(std::find(variable.related_constraint_ptrs().begin(),
                           variable.related_constraint_ptrs().end(),
                           &constraint_1) !=
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
TEST_F(TestVariable, setup_related_binary_coefficient_constraint_ptrs) {
    /// This method is tested in
    /// Model.setup_variable_related_binary_coefficient_constraint_ptrs().
}

/*****************************************************************************/
TEST_F(TestVariable, reset_setup_binary_coefficient_constraint_ptrs) {
    /// This method is tested in
    /// Model.setup_variable_related_binary_coefficient_constraint_ptrs().
}

/*****************************************************************************/
TEST_F(TestVariable, related_binary_coefficient_constraint_ptrs) {
    /// This method is tested in
    /// Model.setup_variable_related_binary_coefficient_constraint_ptrs().
}

/*****************************************************************************/
TEST_F(TestVariable, register_constraint_sensitivity) {
    auto variable = model_component::Variable<int, double>::create_instance();
    auto constraint_0 =
        model_component::Constraint<int, double>::create_instance();
    auto constraint_1 =
        model_component::Constraint<int, double>::create_instance();
    constraint_0.set_name("bb");
    constraint_1.set_name("aa");

    EXPECT_TRUE(variable.constraint_sensitivities().empty());

    variable.register_constraint_sensitivity(&constraint_0, 10);
    EXPECT_EQ(1, static_cast<int>(variable.constraint_sensitivities().size()));
    EXPECT_EQ(10, variable.constraint_sensitivities().back().second);

    variable.register_constraint_sensitivity(&constraint_1, 20);
    EXPECT_EQ(2, static_cast<int>(variable.constraint_sensitivities().size()));
    EXPECT_EQ(20, variable.constraint_sensitivities().back().second);

    variable.sort_constraint_sensitivities();
    EXPECT_EQ("aa", variable.constraint_sensitivities()[0].first->name());
    EXPECT_EQ("bb", variable.constraint_sensitivities()[1].first->name());

    variable.reset_constraint_sensitivities();
    EXPECT_TRUE(variable.constraint_sensitivities().empty());
}

/*****************************************************************************/
TEST_F(TestVariable, reset_constraint_sensitivities) {
    /// This method is tested in register_constraint_sensitivity().
}

/*****************************************************************************/
TEST_F(TestVariable, sort_constraint_sensitivities) {
    /// This method is tested in register_related_constraint_ptr().
}

/*****************************************************************************/
TEST_F(TestVariable, constraint_sensitivities) {
    /// This method is tested in register_constraint_sensitivity().
}

/*****************************************************************************/
TEST_F(TestVariable, setup_hash) {
    model::Model<int, double> model;

    auto& x = model.create_variables("x", 2, 0, 1);
    auto& g = model.create_constraints("g", 2);

    g(0) = x(0) + x(1) == 1;
    g(1) = x(0) == 1;

    model.setup_is_linear();
    model.setup_structure();
    {
        x(0).setup_hash();
        std::uint64_t hash = reinterpret_cast<std::uint64_t>(&g(0)) +
                             reinterpret_cast<std::uint64_t>(&g(1));
        EXPECT_EQ(hash, x(0).hash());
    }

    {
        x(1).setup_hash();
        std::uint64_t hash = reinterpret_cast<std::uint64_t>(&g(0));
        EXPECT_EQ(hash, x(1).hash());
    }
}

/*****************************************************************************/
TEST_F(TestVariable, set_dependent_expression_ptr) {
    auto variable = model_component::Variable<int, double>::create_instance();
    auto expression =
        model_component::Expression<int, double>::create_instance();

    EXPECT_EQ(model_component::VariableSense::Integer, variable.sense());
    EXPECT_EQ(nullptr, variable.dependent_expression_ptr());

    variable.set_dependent_expression_ptr(&expression);

    EXPECT_EQ(model_component::VariableSense::DependentInteger,
              variable.sense());
    EXPECT_EQ(&expression, variable.dependent_expression_ptr());

    variable.reset_dependent_expression_ptr();
    EXPECT_EQ(model_component::VariableSense::Integer, variable.sense());
    EXPECT_EQ(nullptr, variable.dependent_expression_ptr());
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
    auto variable = model_component::Variable<int, double>::create_instance();
    EXPECT_EQ(0.0, variable.objective_sensitivity());
    variable.set_objective_sensitivity(100.0);
    EXPECT_EQ(100.0, variable.objective_sensitivity());
}

/*****************************************************************************/
TEST_F(TestVariable, objective_sensitivity) {
    /// This method is tested in set_objective_sensitivity().
}

/*****************************************************************************/
TEST_F(TestVariable, hash) {
    /// This method is tested in setup_hash().
}

/*****************************************************************************/
TEST_F(TestVariable, reset_related_selection_constraint_ptr_index) {
    auto variable = model_component::Variable<int, double>::create_instance();
    variable.reset_related_selection_constraint_ptr_index();
    EXPECT_EQ(-1, variable.related_selection_constraint_ptr_index_min());
    EXPECT_EQ(-1, variable.related_selection_constraint_ptr_index_max());
}

/*****************************************************************************/
TEST_F(TestVariable, setup_related_selection_constraint_ptr_index) {
    /// This method is tested in
    /// TestModel.setup_related_selection_constraint_ptr_index().
}

/*****************************************************************************/
TEST_F(TestVariable, related_selection_constraint_ptr_index_min) {
    /// This method is tested in
    /// TestModel.setup_related_selection_constraint_ptr_index().
}

/*****************************************************************************/
TEST_F(TestVariable, related_selection_constraint_ptr_index_max) {
    /// This method is tested in
    /// TestModel.setup_related_selection_constraint_ptr_index().
}

/*****************************************************************************/
TEST_F(TestVariable, update_margin) {
    auto variable = model_component::Variable<int, double>::create_instance();
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
TEST_F(TestVariable, set_lower_or_upper_bound) {
    auto variable = model_component::Variable<int, double>::create_instance();
    variable.set_bound(-10, 10);
    variable.set_lower_or_upper_bound(true);
    EXPECT_EQ(-10, variable.value());
    variable.set_lower_or_upper_bound(false);
    EXPECT_EQ(10, variable.value());
}

/*****************************************************************************/
TEST_F(TestVariable, to_expression) {
    auto variable = model_component::Variable<int, double>::create_instance();

    EXPECT_EQ(1, variable.to_expression().sensitivities().at(&variable));
}

/*****************************************************************************/
TEST_F(TestVariable, reference) {
    auto variable = model_component::Variable<int, double>::create_instance();

    EXPECT_EQ(&variable, variable.reference());
}

/*****************************************************************************/
TEST_F(TestVariable, operator_plus) {
    auto variable = model_component::Variable<int, double>::create_instance();

    EXPECT_EQ(1, (+variable).sensitivities().at(&variable));
}

/*****************************************************************************/
TEST_F(TestVariable, operator_minus) {
    auto variable = model_component::Variable<int, double>::create_instance();

    EXPECT_EQ(-1, (-variable).sensitivities().at(&variable));
}

/*****************************************************************************/
TEST_F(TestVariable, operator_equal_arg_t_variable) {
    auto variable = model_component::Variable<int, double>::create_instance();

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