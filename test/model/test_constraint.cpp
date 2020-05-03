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
class TestConstraint : public ::testing::Test {
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
TEST_F(TestConstraint, initialize) {
    auto constraint = cppmh::model::Constraint<int, double>::create_instance();

    EXPECT_EQ(0, constraint.evaluate_constraint());
    EXPECT_EQ(0, constraint.evaluate_constraint({}));
    EXPECT_EQ(0, constraint.evaluate_violation());
    EXPECT_EQ(0, constraint.evaluate_violation({}));
    EXPECT_EQ(0, constraint.flat_index());
    EXPECT_EQ(0, constraint.multi_dimensional_index()[0]);
    EXPECT_EQ(true, constraint.expression().sensitivities().empty());
    EXPECT_EQ(0, constraint.expression().constant_value());
    EXPECT_EQ(cppmh::model::ConstraintSense::Lower, constraint.sense());
    EXPECT_EQ(0, constraint.constraint_value());
    EXPECT_EQ(0, constraint.violation_value());
    EXPECT_EQ(true, constraint.is_linear());
    EXPECT_EQ(true, constraint.is_enabled());
}

/*****************************************************************************/
TEST_F(TestConstraint, set_flat_index) {
    auto variable = cppmh::model::Variable<int, double>::create_instance();

    auto flat_index = random_integer();
    variable.set_flat_index(flat_index);
    EXPECT_EQ(flat_index, variable.flat_index());
}

/*****************************************************************************/
TEST_F(TestConstraint, flat_index) {
    /// This method is tested in set_flat_index().
}

/*****************************************************************************/
TEST_F(TestConstraint, set_multi_dimensional_index) {
    auto variable = cppmh::model::Variable<int, double>::create_instance();

    auto multi_dimensional_index_1 = random_integer();
    auto multi_dimensional_index_2 = random_integer();

    variable.set_multi_dimensional_index(
        {multi_dimensional_index_1, multi_dimensional_index_2});

    EXPECT_EQ(multi_dimensional_index_1, variable.multi_dimensional_index()[0]);
    EXPECT_EQ(multi_dimensional_index_2, variable.multi_dimensional_index()[1]);
}

/*****************************************************************************/
TEST_F(TestConstraint, multi_dimensional_index) {
    /// This method is tested in set_multi_dimensional_index().
}

/*****************************************************************************/
TEST_F(TestConstraint, constructor_arg_function) {
    auto expression = cppmh::model::Expression<int, double>::create_instance();
    auto variable   = cppmh::model::Variable<int, double>::create_instance();

    auto sensitivity = random_integer();
    auto constant    = random_integer();
    auto target      = random_integer();

    expression = sensitivity * variable + constant;
    std::function<double(const cppmh::model::Move<int, double> &)> f =
        [&expression](const cppmh::model::Move<int, double> &a_MOVE) {
            return expression.evaluate(a_MOVE);
        };

    /// Lower
    {
        cppmh::model::Constraint<int, double> constraint(f <= target);

        EXPECT_EQ(true, constraint.expression().sensitivities().empty());
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(cppmh::model::ConstraintSense::Lower, constraint.sense());
        EXPECT_EQ(0, constraint.constraint_value());
        EXPECT_EQ(0, constraint.violation_value());
        EXPECT_EQ(false, constraint.is_linear());
        EXPECT_EQ(true, constraint.is_enabled());
    }

    /// Equal
    {
        cppmh::model::Constraint<int, double> constraint(f == target);

        EXPECT_EQ(true, constraint.expression().sensitivities().empty());
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(cppmh::model::ConstraintSense::Equal, constraint.sense());
        EXPECT_EQ(0, constraint.constraint_value());
        EXPECT_EQ(0, constraint.violation_value());
        EXPECT_EQ(false, constraint.is_linear());
        EXPECT_EQ(true, constraint.is_enabled());
    }

    /// Upper
    {
        cppmh::model::Constraint<int, double> constraint(f >= target);

        EXPECT_EQ(true, constraint.expression().sensitivities().empty());
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(cppmh::model::ConstraintSense::Upper, constraint.sense());
        EXPECT_EQ(0, constraint.constraint_value());
        EXPECT_EQ(0, constraint.violation_value());
        EXPECT_EQ(false, constraint.is_linear());
        EXPECT_EQ(true, constraint.is_enabled());
    }
}

/*****************************************************************************/
TEST_F(TestConstraint, constructor_arg_expression) {
    auto expression = cppmh::model::Expression<int, double>::create_instance();
    auto variable   = cppmh::model::Variable<int, double>::create_instance();

    auto sensitivity = random_integer();
    auto constant    = random_integer();
    auto target      = random_integer();

    expression = sensitivity * variable + constant;

    /// Lower
    {
        cppmh::model::Constraint<int, double> constraint(expression <= target);

        EXPECT_EQ(sensitivity,
                  constraint.expression().sensitivities().at(&variable));

        EXPECT_EQ(constant - target, constraint.expression().constant_value());
        EXPECT_EQ(cppmh::model::ConstraintSense::Lower, constraint.sense());
        EXPECT_EQ(0, constraint.constraint_value());
        EXPECT_EQ(0, constraint.violation_value());
        EXPECT_EQ(true, constraint.is_linear());
        EXPECT_EQ(true, constraint.is_enabled());
    }

    /// Equal
    {
        cppmh::model::Constraint<int, double> constraint(expression == target);

        EXPECT_EQ(sensitivity,
                  constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(constant - target, constraint.expression().constant_value());
        EXPECT_EQ(cppmh::model::ConstraintSense::Equal, constraint.sense());
        EXPECT_EQ(0, constraint.constraint_value());
        EXPECT_EQ(0, constraint.violation_value());
        EXPECT_EQ(true, constraint.is_linear());
        EXPECT_EQ(true, constraint.is_enabled());
    }

    /// Upper
    {
        cppmh::model::Constraint<int, double> constraint(expression >= target);

        EXPECT_EQ(sensitivity,
                  constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(constant - target, constraint.expression().constant_value());
        EXPECT_EQ(cppmh::model::ConstraintSense::Upper, constraint.sense());
        EXPECT_EQ(0, constraint.constraint_value());
        EXPECT_EQ(0, constraint.violation_value());
        EXPECT_EQ(true, constraint.is_linear());
        EXPECT_EQ(true, constraint.is_enabled());
    }
}

/*****************************************************************************/
TEST_F(TestConstraint, setup_arg_function) {
    auto expression = cppmh::model::Expression<int, double>::create_instance();
    auto variable   = cppmh::model::Variable<int, double>::create_instance();

    auto sensitivity = random_integer();
    auto constant    = random_integer();
    auto target      = random_integer();

    expression = sensitivity * variable + constant;
    std::function<double(const cppmh::model::Move<int, double> &)> f =
        [&expression, target](const cppmh::model::Move<int, double> &a_MOVE) {
            return expression.evaluate(a_MOVE) - target;
        };

    /// Lower
    {
        auto constraint =
            cppmh::model::Constraint<int, double>::create_instance();

        constraint.setup(f, cppmh::model::ConstraintSense::Lower);

        EXPECT_EQ(true, constraint.expression().sensitivities().empty());
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(cppmh::model::ConstraintSense::Lower, constraint.sense());
        EXPECT_EQ(0, constraint.constraint_value());
        EXPECT_EQ(0, constraint.violation_value());
        EXPECT_EQ(false, constraint.is_linear());
        EXPECT_EQ(true, constraint.is_enabled());
    }

    /// Equal
    {
        auto constraint =
            cppmh::model::Constraint<int, double>::create_instance();
        constraint.setup(f, cppmh::model::ConstraintSense::Equal);

        EXPECT_EQ(true, constraint.expression().sensitivities().empty());
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(cppmh::model::ConstraintSense::Equal, constraint.sense());
        EXPECT_EQ(0, constraint.constraint_value());
        EXPECT_EQ(0, constraint.violation_value());
        EXPECT_EQ(false, constraint.is_linear());
        EXPECT_EQ(true, constraint.is_enabled());
    }

    /// Upper
    {
        auto constraint =
            cppmh::model::Constraint<int, double>::create_instance();
        ;
        constraint.setup(f, cppmh::model::ConstraintSense::Upper);

        EXPECT_EQ(true, constraint.expression().sensitivities().empty());
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(cppmh::model::ConstraintSense::Upper, constraint.sense());
        EXPECT_EQ(0, constraint.constraint_value());
        EXPECT_EQ(0, constraint.violation_value());
        EXPECT_EQ(false, constraint.is_linear());
        EXPECT_EQ(true, constraint.is_enabled());
    }
}

/*****************************************************************************/
TEST_F(TestConstraint, setup_arg_expression) {
    auto expression = cppmh::model::Expression<int, double>::create_instance();
    auto variable   = cppmh::model::Variable<int, double>::create_instance();

    auto sensitivity = random_integer();
    auto constant    = random_integer();
    auto target      = random_integer();

    expression = sensitivity * variable + constant;

    /// Lower
    {
        auto constraint =
            cppmh::model::Constraint<int, double>::create_instance();
        constraint.setup(expression - target,
                         cppmh::model::ConstraintSense::Lower);

        EXPECT_EQ(sensitivity,
                  constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(constant - target, constraint.expression().constant_value());
        EXPECT_EQ(cppmh::model::ConstraintSense::Lower, constraint.sense());
        EXPECT_EQ(0, constraint.constraint_value());
        EXPECT_EQ(0, constraint.violation_value());
        EXPECT_EQ(true, constraint.is_linear());
        EXPECT_EQ(true, constraint.is_enabled());
    }

    /// Equal
    {
        auto constraint =
            cppmh::model::Constraint<int, double>::create_instance();
        ;
        constraint.setup(expression - target,
                         cppmh::model::ConstraintSense::Equal);

        EXPECT_EQ(sensitivity,
                  constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(constant - target, constraint.expression().constant_value());
        EXPECT_EQ(cppmh::model::ConstraintSense::Equal, constraint.sense());
        EXPECT_EQ(0, constraint.constraint_value());
        EXPECT_EQ(0, constraint.violation_value());
        EXPECT_EQ(true, constraint.is_linear());
        EXPECT_EQ(true, constraint.is_enabled());
    }

    /// Upper
    {
        auto constraint =
            cppmh::model::Constraint<int, double>::create_instance();
        constraint.setup(expression - target,
                         cppmh::model::ConstraintSense::Upper);

        EXPECT_EQ(sensitivity,
                  constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(constant - target, constraint.expression().constant_value());
        EXPECT_EQ(cppmh::model::ConstraintSense::Upper, constraint.sense());
        EXPECT_EQ(0, constraint.constraint_value());
        EXPECT_EQ(0, constraint.violation_value());
        EXPECT_EQ(true, constraint.is_linear());
        EXPECT_EQ(true, constraint.is_enabled());
    }
}

/*****************************************************************************/
TEST_F(TestConstraint, evaluate_function_arg_void) {
    auto expression = cppmh::model::Expression<int, double>::create_instance();
    auto variable   = cppmh::model::Variable<int, double>::create_instance();

    auto sensitivity = random_integer();
    auto constant    = random_integer();
    auto target      = random_integer();

    expression = sensitivity * variable + constant;
    std::function<double(const cppmh::model::Move<int, double> &)> f =
        [&expression, target](const cppmh::model::Move<int, double> &a_MOVE) {
            return expression.evaluate(a_MOVE) - target;
        };

    auto constraint = cppmh::model::Constraint<int, double>::create_instance();

    constraint.setup(f, cppmh::model::ConstraintSense::Lower);

    auto value = random_integer();
    variable   = value;

    auto expected_value = sensitivity * value + constant - target;
    EXPECT_EQ(expected_value, constraint.evaluate_constraint());
    constraint.update();
    EXPECT_EQ(expected_value, constraint.constraint_value());
}

/*****************************************************************************/
TEST_F(TestConstraint, evaluate_expression_arg_void) {
    auto expression = cppmh::model::Expression<int, double>::create_instance();
    auto variable   = cppmh::model::Variable<int, double>::create_instance();

    auto sensitivity = random_integer();
    auto constant    = random_integer();
    auto target      = random_integer();

    expression = sensitivity * variable + constant;

    auto constraint = cppmh::model::Constraint<int, double>::create_instance();
    constraint.setup(expression - target, cppmh::model::ConstraintSense::Lower);

    auto value = random_integer();
    variable   = value;

    auto expected_value = sensitivity * value + constant - target;
    EXPECT_EQ(expected_value, constraint.evaluate_constraint());
    constraint.update();
    EXPECT_EQ(expected_value, constraint.constraint_value());
}

/*****************************************************************************/
TEST_F(TestConstraint, evaluate_function_arg_move) {
    auto expression = cppmh::model::Expression<int, double>::create_instance();
    auto variable   = cppmh::model::Variable<int, double>::create_instance();

    auto sensitivity = random_integer();
    auto constant    = random_integer();
    auto target      = random_integer();

    expression = sensitivity * variable + constant;
    std::function<double(const cppmh::model::Move<int, double> &)> f =
        [&expression, target](const cppmh::model::Move<int, double> &a_MOVE) {
            return expression.evaluate(a_MOVE) - target;
        };

    auto constraint = cppmh::model::Constraint<int, double>::create_instance();
    constraint.setup(f, cppmh::model::ConstraintSense::Lower);

    /// initial
    {
        auto value = random_integer();
        variable   = value;

        auto expected_value = sensitivity * value + constant - target;
        EXPECT_EQ(expected_value, constraint.evaluate_constraint());
        constraint.update();
        EXPECT_EQ(expected_value, constraint.constraint_value());

        /// expression.update() must be called after constraint.update();
        expression.update();
    }
    /// after move
    {
        auto value = random_integer();

        cppmh::model::Move<int, double> move;
        move.alterations.emplace_back(&variable, value);

        auto expected_value = sensitivity * value + constant - target;
        EXPECT_EQ(expected_value, constraint.evaluate_constraint(move));
        constraint.update(move);
        EXPECT_EQ(expected_value, constraint.constraint_value());
    }
}

/*****************************************************************************/
TEST_F(TestConstraint, evaluate_expression_arg_move) {
    auto expression = cppmh::model::Expression<int, double>::create_instance();
    auto variable   = cppmh::model::Variable<int, double>::create_instance();

    auto sensitivity = random_integer();
    auto constant    = random_integer();
    auto target      = random_integer();

    expression = sensitivity * variable + constant;

    auto constraint = cppmh::model::Constraint<int, double>::create_instance();
    constraint.setup(expression - target, cppmh::model::ConstraintSense::Lower);

    /// initial
    {
        auto value = random_integer();
        variable   = value;
        constraint.update();

        auto expected_value = sensitivity * value + constant - target;
        EXPECT_EQ(expected_value, constraint.evaluate_constraint());
        constraint.update();
        EXPECT_EQ(expected_value, constraint.constraint_value());
    }

    /// after move
    {
        auto value = random_integer();

        cppmh::model::Move<int, double> move;
        move.alterations.emplace_back(&variable, value);

        auto expected_value = sensitivity * value + constant - target;
        EXPECT_EQ(expected_value, constraint.evaluate_constraint(move));
        constraint.update(move);
        EXPECT_EQ(expected_value, constraint.constraint_value());
    }
}

/*****************************************************************************/
TEST_F(TestConstraint, evaluate_violation_function_arg_void) {
    auto expression = cppmh::model::Expression<int, double>::create_instance();
    auto variable   = cppmh::model::Variable<int, double>::create_instance();

    auto sensitivity = random_integer();
    auto constant    = random_integer();
    auto target      = random_integer();

    expression = sensitivity * variable + constant;
    std::function<double(const cppmh::model::Move<int, double> &)> f =
        [&expression, target](const cppmh::model::Move<int, double> &a_MOVE) {
            return expression.evaluate(a_MOVE) - target;
        };

    /// Lower
    {
        auto constraint =
            cppmh::model::Constraint<int, double>::create_instance();
        constraint.setup(f, cppmh::model::ConstraintSense::Lower);

        auto value = random_integer();
        variable   = value;

        auto expected_value =
            std::max(sensitivity * value + constant - target, 0);
        EXPECT_EQ(expected_value, constraint.evaluate_violation());
        constraint.update();
        EXPECT_EQ(expected_value, constraint.violation_value());
    }

    /// Equal
    {
        auto constraint =
            cppmh::model::Constraint<int, double>::create_instance();
        constraint.setup(f, cppmh::model::ConstraintSense::Equal);

        auto value = random_integer();
        variable   = value;

        auto expected_value = std::abs(sensitivity * value + constant - target);
        EXPECT_EQ(expected_value, constraint.evaluate_violation());
        constraint.update();
        EXPECT_EQ(expected_value, constraint.violation_value());
    }

    /// Upper
    {
        auto constraint =
            cppmh::model::Constraint<int, double>::create_instance();
        constraint.setup(f, cppmh::model::ConstraintSense::Upper);

        auto value = random_integer();
        variable   = value;

        auto expected_value =
            std::max(-(sensitivity * value + constant - target), 0);
        EXPECT_EQ(expected_value, constraint.evaluate_violation());
        constraint.update();
        EXPECT_EQ(expected_value, constraint.violation_value());
    }
}

/*****************************************************************************/
TEST_F(TestConstraint, evaluate_violation_expression_arg_void) {
    auto expression = cppmh::model::Expression<int, double>::create_instance();
    auto variable   = cppmh::model::Variable<int, double>::create_instance();

    auto sensitivity = random_integer();
    auto constant    = random_integer();
    auto target      = random_integer();

    expression = sensitivity * variable + constant;

    /// Lower
    {
        auto constraint =
            cppmh::model::Constraint<int, double>::create_instance();
        constraint.setup(expression - target,
                         cppmh::model::ConstraintSense::Lower);

        auto value = random_integer();
        variable   = value;

        auto expected_value =
            std::max(sensitivity * value + constant - target, 0);
        EXPECT_EQ(expected_value, constraint.evaluate_violation());
        constraint.update();
        EXPECT_EQ(expected_value, constraint.violation_value());
    }

    /// Equal
    {
        auto constraint =
            cppmh::model::Constraint<int, double>::create_instance();
        constraint.setup(expression - target,
                         cppmh::model::ConstraintSense::Equal);

        auto value = random_integer();
        variable   = value;

        auto expected_value = std::abs(sensitivity * value + constant - target);
        EXPECT_EQ(expected_value, constraint.evaluate_violation());
        constraint.update();
        EXPECT_EQ(expected_value, constraint.violation_value());
    }

    /// Upper
    {
        auto constraint =
            cppmh::model::Constraint<int, double>::create_instance();
        constraint.setup(expression - target,
                         cppmh::model::ConstraintSense::Upper);

        auto value = random_integer();
        variable   = value;

        auto expected_value =
            std::max(-(sensitivity * value + constant - target), 0);
        EXPECT_EQ(expected_value, constraint.evaluate_violation());
        constraint.update();
        EXPECT_EQ(expected_value, constraint.violation_value());
    }
}

/*****************************************************************************/
TEST_F(TestConstraint, evaluate_violation_function_arg_move) {
    auto expression = cppmh::model::Expression<int, double>::create_instance();
    auto variable   = cppmh::model::Variable<int, double>::create_instance();

    auto sensitivity = random_integer();
    auto constant    = random_integer();
    auto target      = random_integer();

    expression = sensitivity * variable + constant;
    std::function<double(const cppmh::model::Move<int, double> &)> f =
        [&expression, target](const cppmh::model::Move<int, double> &a_MOVE) {
            return expression.evaluate(a_MOVE) - target;
        };

    /// Lower
    {
        auto constraint =
            cppmh::model::Constraint<int, double>::create_instance();
        constraint.setup(f, cppmh::model::ConstraintSense::Lower);

        /// initial
        {
            auto value = random_integer();
            variable   = value;

            auto expected_value =
                std::max(sensitivity * value + constant - target, 0);
            EXPECT_EQ(expected_value, constraint.evaluate_violation());
            constraint.update();
            EXPECT_EQ(expected_value, constraint.violation_value());

            /// expression.update() must be called after constraint.update();
            expression.update();
        }

        /// after move
        {
            auto value = random_integer();

            cppmh::model::Move<int, double> move;
            move.alterations.emplace_back(&variable, value);

            auto expected_value =
                std::max(sensitivity * value + constant - target, 0);
            EXPECT_EQ(expected_value, constraint.evaluate_violation(move));
            constraint.update(move);
            EXPECT_EQ(expected_value, constraint.violation_value());
        }
    }

    /// Equal
    {
        auto constraint =
            cppmh::model::Constraint<int, double>::create_instance();
        constraint.setup(f, cppmh::model::ConstraintSense::Equal);

        /// initial
        {
            auto value = random_integer();
            variable   = value;

            auto expected_value =
                std::abs(sensitivity * value + constant - target);
            EXPECT_EQ(expected_value, constraint.evaluate_violation());
            constraint.update();
            EXPECT_EQ(expected_value, constraint.violation_value());

            /// expression.update() must be called after constraint.update();
            expression.update();
        }

        /// after move
        {
            auto value = random_integer();

            cppmh::model::Move<int, double> move;
            move.alterations.emplace_back(&variable, value);

            auto expected_value =
                std::abs(sensitivity * value + constant - target);
            EXPECT_EQ(expected_value, constraint.evaluate_violation(move));
            constraint.update(move);
            EXPECT_EQ(expected_value, constraint.violation_value());
        }
    }

    /// Upper
    {
        auto constraint =
            cppmh::model::Constraint<int, double>::create_instance();
        constraint.setup(f, cppmh::model::ConstraintSense::Upper);

        /// initial
        {
            auto value = random_integer();
            variable   = value;

            auto expected_value =
                std::max(-(sensitivity * value + constant - target), 0);
            EXPECT_EQ(expected_value, constraint.evaluate_violation());
            constraint.update();
            EXPECT_EQ(expected_value, constraint.violation_value());

            /// expression.update() must be called after constraint.update();
            expression.update();
        }

        /// after move
        {
            auto value = random_integer();

            cppmh::model::Move<int, double> move;
            move.alterations.emplace_back(&variable, value);

            auto expected_value =
                std::max(-(sensitivity * value + constant - target), 0);
            EXPECT_EQ(expected_value, constraint.evaluate_violation(move));
            constraint.update(move);
            EXPECT_EQ(expected_value, constraint.violation_value());
        }
    }
}

/*****************************************************************************/
TEST_F(TestConstraint, evaluate_violation_expression_arg_move) {
    auto expression = cppmh::model::Expression<int, double>::create_instance();
    auto variable   = cppmh::model::Variable<int, double>::create_instance();

    auto sensitivity = random_integer();
    auto constant    = random_integer();
    auto target      = random_integer();

    expression = sensitivity * variable + constant;

    /// Lower
    {
        auto constraint =
            cppmh::model::Constraint<int, double>::create_instance();
        constraint.setup(expression - target,
                         cppmh::model::ConstraintSense::Lower);

        /// initial
        {
            auto value = random_integer();
            variable   = value;
            constraint.update();

            auto expected_value =
                std::max(sensitivity * value + constant - target, 0);
            EXPECT_EQ(expected_value, constraint.evaluate_violation());
            constraint.update();
            EXPECT_EQ(expected_value, constraint.violation_value());
        }

        /// after move
        {
            auto value = random_integer();

            cppmh::model::Move<int, double> move;
            move.alterations.emplace_back(&variable, value);

            auto expected_value =
                std::max(sensitivity * value + constant - target, 0);
            EXPECT_EQ(expected_value, constraint.evaluate_violation(move));
            constraint.update(move);
            EXPECT_EQ(expected_value, constraint.violation_value());
        }
    }

    /// Equal
    {
        auto constraint =
            cppmh::model::Constraint<int, double>::create_instance();
        constraint.setup(expression - target,
                         cppmh::model::ConstraintSense::Equal);

        /// initial
        {
            auto value = random_integer();
            variable   = value;
            constraint.update();

            auto expected_value =
                std::abs(sensitivity * value + constant - target);
            EXPECT_EQ(expected_value, constraint.evaluate_violation());
            constraint.update();
            EXPECT_EQ(expected_value, constraint.violation_value());
        }

        /// after move
        {
            auto value = random_integer();

            cppmh::model::Move<int, double> move;
            move.alterations.emplace_back(&variable, value);

            auto expected_value =
                std::abs(sensitivity * value + constant - target);
            EXPECT_EQ(expected_value, constraint.evaluate_violation(move));
            constraint.update(move);
            EXPECT_EQ(expected_value, constraint.violation_value());
        }
    }

    /// Upper
    {
        auto constraint =
            cppmh::model::Constraint<int, double>::create_instance();
        constraint.setup(expression - target,
                         cppmh::model::ConstraintSense::Upper);

        /// initial
        {
            auto value = random_integer();
            variable   = value;
            constraint.update();

            auto expected_value =
                std::max(-(sensitivity * value + constant - target), 0);
            EXPECT_EQ(expected_value, constraint.evaluate_violation());
            constraint.update();
            EXPECT_EQ(expected_value, constraint.violation_value());
        }

        /// after move
        {
            auto                            value = random_integer();
            cppmh::model::Move<int, double> move;
            move.alterations.emplace_back(&variable, value);

            auto expected_value =
                std::max(-(sensitivity * value + constant - target), 0);
            EXPECT_EQ(expected_value, constraint.evaluate_violation(move));
            constraint.update(move);
            EXPECT_EQ(expected_value, constraint.violation_value());
        }
    }
}

/*****************************************************************************/
TEST_F(TestConstraint, update_arg_void) {
    /// This method is tested in following tests:
    /// - evaluate_function_arg_void()
    /// - evaluate_expression_arg_void()
    /// - evaluate_violation_function_arg_void()
    /// - evaluate_violation_expression_arg_void()
}

/*****************************************************************************/
TEST_F(TestConstraint, update_arg_move) {
    /// This method is tested in following tests:
    /// - evaluate_function_arg_move()
    /// - evaluate_expression_arg_move()
    /// - evaluate_violation_function_arg_move()
    /// - evaluate_violation_expression_arg_move()
}

/*****************************************************************************/
TEST_F(TestConstraint, expression) {
    /// This method is tested in constructor_arg_expression().
}

/*****************************************************************************/
TEST_F(TestConstraint, value) {
    /// This method is tested in tested in other cases
}

/*****************************************************************************/
TEST_F(TestConstraint, is_linear) {
    /// This method is tested in following tests:
    /// - constructor_arg_function
    /// - constructor_arg_expression
}

/*****************************************************************************/
TEST_F(TestConstraint, is_enabled) {
    auto constraint = cppmh::model::Constraint<int, double>::create_instance();
    constraint.disable();
    EXPECT_EQ(false, constraint.is_enabled());

    constraint.enable();
    EXPECT_EQ(true, constraint.is_enabled());

    constraint.disable();
    EXPECT_EQ(false, constraint.is_enabled());
}

/*****************************************************************************/
TEST_F(TestConstraint, enable) {
    /// This method is tested in is_enabled().
}

/*****************************************************************************/
TEST_F(TestConstraint, disable) {
    /// This method is tested in is_enabled().
}

/*****************************************************************************/
TEST_F(TestConstraint, operator_equal_function) {
    auto expression = cppmh::model::Expression<int, double>::create_instance();
    auto variable   = cppmh::model::Variable<int, double>::create_instance();

    auto sensitivity = random_integer();
    auto constant    = random_integer();
    auto target      = random_integer();

    expression = sensitivity * variable + constant;
    std::function<double(const cppmh::model::Move<int, double> &)> f =
        [&expression, target](const cppmh::model::Move<int, double> &a_MOVE) {
            return expression.evaluate(a_MOVE) - target;
        };

    /// Lower
    {
        auto constraint_source =
            cppmh::model::Constraint<int, double>::create_instance();
        constraint_source.setup(f, cppmh::model::ConstraintSense::Lower);

        auto constraint_result =
            cppmh::model::Constraint<int, double>::create_instance();

        EXPECT_EQ(false, (constraint_result = constraint_source).is_linear());

        EXPECT_EQ(true, constraint_result.expression().sensitivities().empty());
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(cppmh::model::ConstraintSense::Lower,
                  constraint_result.sense());
        EXPECT_EQ(0, constraint_result.constraint_value());
        EXPECT_EQ(0, constraint_result.violation_value());
        EXPECT_EQ(false, constraint_result.is_linear());

        auto value = random_integer();
        variable   = value;

        auto expected_value = sensitivity * value + constant - target;
        EXPECT_EQ(expected_value, constraint_result.evaluate_constraint());
        constraint_result.update();
        EXPECT_EQ(expected_value, constraint_result.constraint_value());
    }

    /// Equal
    {
        auto constraint_source =
            cppmh::model::Constraint<int, double>::create_instance();
        constraint_source.setup(f, cppmh::model::ConstraintSense::Equal);
        auto constraint_result =
            cppmh::model::Constraint<int, double>::create_instance();

        EXPECT_EQ(false, (constraint_result = constraint_source).is_linear());

        EXPECT_EQ(true, constraint_result.expression().sensitivities().empty());
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(cppmh::model::ConstraintSense::Equal,
                  constraint_result.sense());
        EXPECT_EQ(0, constraint_result.constraint_value());
        EXPECT_EQ(0, constraint_result.violation_value());
        EXPECT_EQ(false, constraint_result.is_linear());

        auto value = random_integer();
        variable   = value;

        auto expected_value = sensitivity * value + constant - target;
        EXPECT_EQ(expected_value, constraint_result.evaluate_constraint());
        constraint_result.update();
        EXPECT_EQ(expected_value, constraint_result.constraint_value());
    }

    /// Upper
    {
        auto constraint_source =
            cppmh::model::Constraint<int, double>::create_instance();
        constraint_source.setup(f, cppmh::model::ConstraintSense::Upper);
        auto constraint_result =
            cppmh::model::Constraint<int, double>::create_instance();

        EXPECT_EQ(false, (constraint_result = constraint_source).is_linear());

        EXPECT_EQ(true, constraint_result.expression().sensitivities().empty());
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(cppmh::model::ConstraintSense::Upper,
                  constraint_result.sense());
        EXPECT_EQ(0, constraint_result.constraint_value());
        EXPECT_EQ(0, constraint_result.violation_value());
        EXPECT_EQ(false, constraint_result.is_linear());

        auto value = random_integer();
        variable   = value;

        auto expected_value = sensitivity * value + constant - target;
        EXPECT_EQ(expected_value, constraint_result.evaluate_constraint());
        constraint_result.update();
        EXPECT_EQ(expected_value, constraint_result.constraint_value());
    }
}

/*****************************************************************************/
TEST_F(TestConstraint, operator_equal_expression) {
    auto expression = cppmh::model::Expression<int, double>::create_instance();
    auto variable   = cppmh::model::Variable<int, double>::create_instance();

    auto sensitivity = random_integer();
    auto constant    = random_integer();
    auto target      = random_integer();

    expression = sensitivity * variable + constant;

    /// Lower
    {
        auto constraint_source =
            cppmh::model::Constraint<int, double>::create_instance();
        constraint_source.setup(expression - target,
                                cppmh::model::ConstraintSense::Lower);
        auto constraint_result =
            cppmh::model::Constraint<int, double>::create_instance();

        EXPECT_EQ(true, (constraint_result = constraint_source).is_linear());

        EXPECT_EQ(sensitivity,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(constant - target,
                  constraint_result.expression().constant_value());
        EXPECT_EQ(cppmh::model::ConstraintSense::Lower,
                  constraint_result.sense());
        EXPECT_EQ(0, constraint_result.constraint_value());
        EXPECT_EQ(0, constraint_result.violation_value());
        EXPECT_EQ(true, constraint_result.is_linear());

        auto value = random_integer();
        variable   = value;

        auto expected_value = sensitivity * value + constant - target;
        EXPECT_EQ(expected_value, constraint_result.evaluate_constraint());
        constraint_result.update();
        EXPECT_EQ(expected_value, constraint_result.constraint_value());
    }

    /// Equal
    {
        auto constraint_source =
            cppmh::model::Constraint<int, double>::create_instance();
        constraint_source.setup(expression - target,
                                cppmh::model::ConstraintSense::Equal);
        auto constraint_result =
            cppmh::model::Constraint<int, double>::create_instance();

        EXPECT_EQ(true, (constraint_result = constraint_source).is_linear());

        EXPECT_EQ(sensitivity,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(constant - target,
                  constraint_result.expression().constant_value());
        EXPECT_EQ(cppmh::model::ConstraintSense::Equal,
                  constraint_result.sense());
        EXPECT_EQ(0, constraint_result.constraint_value());
        EXPECT_EQ(0, constraint_result.violation_value());
        EXPECT_EQ(true, constraint_result.is_linear());

        auto value = random_integer();
        variable   = value;

        auto expected_value = sensitivity * value + constant - target;
        EXPECT_EQ(expected_value, constraint_result.evaluate_constraint());
        constraint_result.update();
        EXPECT_EQ(expected_value, constraint_result.constraint_value());
    }

    /// Upper
    {
        auto constraint_source =
            cppmh::model::Constraint<int, double>::create_instance();
        constraint_source.setup(expression - target,
                                cppmh::model::ConstraintSense::Upper);
        auto constraint_result =
            cppmh::model::Constraint<int, double>::create_instance();

        EXPECT_EQ(true, (constraint_result = constraint_source).is_linear());

        EXPECT_EQ(sensitivity,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(constant - target,
                  constraint_result.expression().constant_value());
        EXPECT_EQ(cppmh::model::ConstraintSense::Upper,
                  constraint_result.sense());
        EXPECT_EQ(0, constraint_result.constraint_value());
        EXPECT_EQ(0, constraint_result.violation_value());
        EXPECT_EQ(true, constraint_result.is_linear());

        auto value = random_integer();
        variable   = value;

        auto expected_value = sensitivity * value + constant - target;
        EXPECT_EQ(expected_value, constraint_result.evaluate_constraint());
        constraint_result.update();
        EXPECT_EQ(expected_value, constraint_result.constraint_value());
    }
}

/*****************************************************************************/
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/