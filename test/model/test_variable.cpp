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
class TestVariable : public ::testing::Test {
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
TEST_F(TestVariable, initialize) {
    auto variable = cppmh::model::Variable<int, double>::create_instance();

    /// Check the initial values of the base class members.
    EXPECT_EQ(0, variable.id());
    EXPECT_EQ(0, variable.flat_index());
    EXPECT_EQ(0, variable.multi_dimensional_index()[0]);
    EXPECT_EQ("", variable.name());

    /// Check the initial values of the derived class members.
    EXPECT_EQ(false, variable.is_fixed());
    EXPECT_EQ(0, variable.value());
    EXPECT_EQ(std::numeric_limits<int>::min() + 1, variable.lower_bound());
    EXPECT_EQ(std::numeric_limits<int>::max() - 1, variable.upper_bound());
    EXPECT_EQ(false, variable.has_bounds());
    EXPECT_EQ(cppmh::model::VariableSense::Integer, variable.sense());
    EXPECT_EQ(nullptr, variable.selection_ptr());
    EXPECT_EQ(0, static_cast<int>(variable.related_constraint_ptrs().size()));
}

/*****************************************************************************/
TEST_F(TestVariable, set_value_force) {
    auto variable = cppmh::model::Variable<int, double>::create_instance();

    auto value_0 = random_integer();
    variable     = value_0;
    EXPECT_EQ(value_0, variable.value());

    auto value_1 = random_integer();
    variable.fix();
    variable.set_value_force(value_1);
    EXPECT_EQ(value_1, variable.value());
}

/*****************************************************************************/
TEST_F(TestVariable, set_value_if_not_fixed) {
    auto variable = cppmh::model::Variable<int, double>::create_instance();

    auto value_0 = random_integer();
    variable     = value_0;
    EXPECT_EQ(value_0, variable.value());

    variable.fix();

    auto value_1 = random_integer();
    variable.set_value_if_not_fixed(value_1);
    EXPECT_EQ(value_0, variable.value());
}

/*****************************************************************************/
TEST_F(TestVariable, value) {
    auto variable = cppmh::model::Variable<int, double>::create_instance();

    auto value = random_integer();
    variable   = value;
    EXPECT_EQ(value, variable.value());
}

/*****************************************************************************/
TEST_F(TestVariable, set_value) {
    auto variable = cppmh::model::Variable<int, double>::create_instance();

    auto value = random_integer();
    variable.set_value(value);
    EXPECT_EQ(value, variable.value());

    variable.fix();
    ASSERT_THROW(variable = value, std::logic_error);
}

/*****************************************************************************/
TEST_F(TestVariable, evaluate_arg_void) {
    auto variable = cppmh::model::Variable<int, double>::create_instance();

    auto value = random_integer();
    variable   = value;
    EXPECT_EQ(value, variable.evaluate());
}

/*****************************************************************************/
TEST_F(TestVariable, evaluate_arg_move) {
    auto variable_0 = cppmh::model::Variable<int, double>::create_instance();
    auto variable_1 = cppmh::model::Variable<int, double>::create_instance();
    auto value_0_before = random_integer();
    auto value_1_before = random_integer();
    auto value_0_after  = random_integer();
    auto value_1_after  = random_integer();

    variable_0 = value_0_before;
    variable_1 = value_1_before;

    cppmh::model::Move<int, double> move_0;
    cppmh::model::Move<int, double> move_1;
    move_0.alterations.emplace_back(&variable_0, value_0_after);
    move_1.alterations.emplace_back(&variable_1, value_1_after);

    EXPECT_EQ(value_0_after, variable_0.evaluate(move_0));
    EXPECT_EQ(value_0_before, variable_0.evaluate(move_1));
    EXPECT_EQ(value_1_after, variable_1.evaluate(move_1));
    EXPECT_EQ(value_1_before, variable_1.evaluate(move_0));
}

/*****************************************************************************/
TEST_F(TestVariable, fix) {
    auto variable = cppmh::model::Variable<int, double>::create_instance();
    variable.fix();
    EXPECT_EQ(true, variable.is_fixed());
    variable.unfix();
    EXPECT_EQ(false, variable.is_fixed());
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
    auto variable = cppmh::model::Variable<int, double>::create_instance();

    auto value = random_integer();
    variable.fix_by(value);
    EXPECT_EQ(true, variable.is_fixed());
    EXPECT_EQ(value, variable.value());
}

/*****************************************************************************/
TEST_F(TestVariable, sense) {
    auto variable = cppmh::model::Variable<int, double>::create_instance();
    EXPECT_EQ(cppmh::model::VariableSense::Integer, variable.sense());
    variable.set_bound(0, 1);
    EXPECT_EQ(cppmh::model::VariableSense::Binary, variable.sense());
    variable.set_bound(0, 10);
    EXPECT_EQ(cppmh::model::VariableSense::Integer, variable.sense());
}

/*****************************************************************************/
TEST_F(TestVariable, setup_sense) {
    auto variable = cppmh::model::Variable<int, double>::create_instance();
    variable.set_bound(0, 1);
    cppmh::model::Selection<int, double> selection;
    variable.set_selection_ptr(&selection);
    EXPECT_EQ(cppmh::model::VariableSense::Selection, variable.sense());
    variable.setup_sense();
    EXPECT_EQ(cppmh::model::VariableSense::Binary, variable.sense());
}

/*****************************************************************************/
TEST_F(TestVariable, set_bound) {
    auto variable = cppmh::model::Variable<int, double>::create_instance();

    auto lower_bound = random_integer();
    auto upper_bound = lower_bound + random_positive_integer();

    variable.set_bound(lower_bound, upper_bound);
    EXPECT_EQ(lower_bound, variable.lower_bound());
    EXPECT_EQ(upper_bound, variable.upper_bound());
    EXPECT_EQ(true, variable.has_bounds());

    ASSERT_THROW(variable.set_bound(upper_bound, lower_bound),
                 std::logic_error);

    variable.reset_bound();
    EXPECT_EQ(std::numeric_limits<int>::min() + 1, variable.lower_bound());
    EXPECT_EQ(std::numeric_limits<int>::max() - 1, variable.upper_bound());
    EXPECT_EQ(false, variable.has_bounds());
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
TEST_F(TestVariable, set_selection_ptr) {
    auto variable_0 = cppmh::model::Variable<int, double>::create_instance();
    auto variable_1 = cppmh::model::Variable<int, double>::create_instance();
    variable_0.set_bound(0, 1);
    variable_1.set_bound(0, 1);
    cppmh::model::Selection<int, double> selection;
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
TEST_F(TestVariable, register_related_constraint_ptr) {
    auto variable = cppmh::model::Variable<int, double>::create_instance();
    auto constraint_0 =
        cppmh::model::Constraint<int, double>::create_instance();
    auto constraint_1 =
        cppmh::model::Constraint<int, double>::create_instance();

    EXPECT_EQ(0, static_cast<int>(variable.related_constraint_ptrs().size()));
    EXPECT_EQ(false, variable.related_constraint_ptrs().find(&constraint_0) !=
                         variable.related_constraint_ptrs().end());
    EXPECT_EQ(false, variable.related_constraint_ptrs().find(&constraint_1) !=
                         variable.related_constraint_ptrs().end());

    variable.register_related_constraint_ptr(&constraint_0);
    EXPECT_EQ(1, static_cast<int>(variable.related_constraint_ptrs().size()));
    EXPECT_EQ(true, variable.related_constraint_ptrs().find(&constraint_0) !=
                        variable.related_constraint_ptrs().end());
    EXPECT_EQ(false, variable.related_constraint_ptrs().find(&constraint_1) !=
                         variable.related_constraint_ptrs().end());

    variable.register_related_constraint_ptr(&constraint_1);
    EXPECT_EQ(2, static_cast<int>(variable.related_constraint_ptrs().size()));
    EXPECT_EQ(true, variable.related_constraint_ptrs().find(&constraint_0) !=
                        variable.related_constraint_ptrs().end());
    EXPECT_EQ(true, variable.related_constraint_ptrs().find(&constraint_1) !=
                        variable.related_constraint_ptrs().end());

    variable.register_related_constraint_ptr(&constraint_1);
    EXPECT_EQ(2, static_cast<int>(variable.related_constraint_ptrs().size()));
    EXPECT_EQ(true, variable.related_constraint_ptrs().find(&constraint_0) !=
                        variable.related_constraint_ptrs().end());
    EXPECT_EQ(true, variable.related_constraint_ptrs().find(&constraint_1) !=
                        variable.related_constraint_ptrs().end());

    variable.reset_related_constraint_ptrs();
    EXPECT_EQ(0, static_cast<int>(variable.related_constraint_ptrs().size()));
    EXPECT_EQ(false, variable.related_constraint_ptrs().find(&constraint_0) !=
                         variable.related_constraint_ptrs().end());
    EXPECT_EQ(false, variable.related_constraint_ptrs().find(&constraint_1) !=
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
TEST_F(TestVariable, to_expression) {
    auto variable = cppmh::model::Variable<int, double>::create_instance();

    EXPECT_EQ(1, variable.to_expression().sensitivities().at(&variable));
}

/*****************************************************************************/
TEST_F(TestVariable, reference) {
    auto variable = cppmh::model::Variable<int, double>::create_instance();

    EXPECT_EQ(&variable, variable.reference());
}

/*****************************************************************************/
TEST_F(TestVariable, operator_plus) {
    auto variable = cppmh::model::Variable<int, double>::create_instance();

    EXPECT_EQ(1, (+variable).sensitivities().at(&variable));
}

/*****************************************************************************/
TEST_F(TestVariable, operator_minus) {
    auto variable = cppmh::model::Variable<int, double>::create_instance();

    EXPECT_EQ(-1, (-variable).sensitivities().at(&variable));
}

/*****************************************************************************/
TEST_F(TestVariable, operator_equal_arg_t_variable) {
    auto variable = cppmh::model::Variable<int, double>::create_instance();

    auto value = random_integer();
    variable   = value;
    EXPECT_EQ(value, variable.value());

    variable.fix();
    ASSERT_THROW(variable = value, std::logic_error);
}

/*****************************************************************************/
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/