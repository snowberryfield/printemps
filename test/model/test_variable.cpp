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

    EXPECT_EQ(-1, variable.id());
    EXPECT_EQ(0, variable.flat_index());
    EXPECT_EQ(0, variable.multi_dimensional_index()[0]);
    EXPECT_EQ(false, variable.is_fixed());
    EXPECT_EQ(0, variable.value());
    EXPECT_EQ(std::numeric_limits<int>::min() + 1, variable.lower_bound());
    EXPECT_EQ(std::numeric_limits<int>::max() - 1, variable.upper_bound());
}

/*****************************************************************************/
TEST_F(TestVariable, set_id) {
    auto variable = cppmh::model::Variable<int, double>::create_instance();

    auto id = random_integer();
    variable.set_id(id);
    EXPECT_EQ(id, variable.id());
}

/*****************************************************************************/
TEST_F(TestVariable, id) {
    /// This method is tested in set_id().
}

/*****************************************************************************/
TEST_F(TestVariable, set_flat_index) {
    auto variable = cppmh::model::Variable<int, double>::create_instance();

    auto flat_index = random_integer();
    variable.set_flat_index(flat_index);
    EXPECT_EQ(flat_index, variable.flat_index());
}

/*****************************************************************************/
TEST_F(TestVariable, flat_index) {
    /// This method is tested in set_flat_index().
}

/*****************************************************************************/
TEST_F(TestVariable, set_multi_dimensional_index) {
    auto variable = cppmh::model::Variable<int, double>::create_instance();

    auto multi_dimensional_index_1 = random_integer();
    auto multi_dimensional_index_2 = random_integer();

    variable.set_multi_dimensional_index(
        {multi_dimensional_index_1, multi_dimensional_index_2});

    EXPECT_EQ(multi_dimensional_index_1, variable.multi_dimensional_index()[0]);
    EXPECT_EQ(multi_dimensional_index_2, variable.multi_dimensional_index()[1]);
}

/*****************************************************************************/
TEST_F(TestVariable, multi_dimensional_index) {
    /// This method is tested in set_multi_dimensional_index().
}

/*****************************************************************************/
TEST_F(TestVariable, set_value_force) {
    auto variable = cppmh::model::Variable<int, double>::create_instance();

    auto value_1 = random_integer();
    variable     = value_1;
    EXPECT_EQ(value_1, variable.value());

    auto value_2 = random_integer();
    variable.fix();
    variable.set_value_force(value_2);
    EXPECT_EQ(value_2, variable.value());
}

/*****************************************************************************/
TEST_F(TestVariable, set_value_if_not_fixed) {
    auto variable = cppmh::model::Variable<int, double>::create_instance();

    auto value_1 = random_integer();
    variable     = value_1;
    EXPECT_EQ(value_1, variable.value());

    auto value_2 = random_integer();
    variable.fix();
    variable.set_value_if_not_fixed(value_2);
    EXPECT_EQ(value_1, variable.value());
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
TEST_F(TestVariable, reset_sense) {
    auto variable = cppmh::model::Variable<int, double>::create_instance();
    variable.set_bound(0, 1);
    cppmh::model::Selection<int, double> selection;
    variable.set_selection_ptr(&selection);
    EXPECT_EQ(cppmh::model::VariableSense::Selection, variable.sense());
    variable.reset_sense();
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
    EXPECT_EQ(true, variable.is_defined_bounds());

    ASSERT_THROW(variable.set_bound(upper_bound, lower_bound),
                 std::logic_error);
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
TEST_F(TestVariable, is_defined_bounds) {
    /// This method is tested in set_bound().
}

/*****************************************************************************/
TEST_F(TestVariable, set_selection_ptr) {
    /// This method is tested in reset_sense().
}

/*****************************************************************************/
TEST_F(TestVariable, selection_ptr) {
    /// This method is tested in reset_sense().
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