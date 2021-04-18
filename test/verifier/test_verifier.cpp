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
class TestVerifier : public ::testing::Test {
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
TEST_F(TestVerifier, verify_problem) {
    /// No decision variables.
    {
        printemps::model::Model<int, double> model;
        ASSERT_THROW(printemps::verifier::verify_problem(&model, false),
                     std::logic_error);
    }

    /// No constraint functions.
    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variable("x");
        model.minimize(x);
        printemps::verifier::verify_problem(&model, false);
    }

    /// No objective function.
    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variable("x");
        model.create_constraint("g", x == 1);
        printemps::verifier::verify_problem(&model, false);
    }

    /// No constraint functions and no objective function
    {
        printemps::model::Model<int, double> model;

        [[maybe_unused]] auto& x = model.create_variable("x");
        ASSERT_THROW(printemps::verifier::verify_problem(&model, false),
                     std::logic_error);
    }
}

/*****************************************************************************/
TEST_F(TestVerifier, verify_and_correct_selection_variables_initial_values) {
    /// There is a fixed variable with an invalid initial value.
    /// correction: true
    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variables("x", 10, 0, 1);
        model.create_constraint("g", x.selection());
        x(0).fix_by(2);

        model.categorize_variables();
        model.categorize_constraints();
        printemps::presolver::extract_selections_by_defined_order(&model,
                                                                  false);

        ASSERT_THROW(
            printemps::verifier::
                verify_and_correct_selection_variables_initial_values(  //
                    &model, true, false),
            std::logic_error);
    }

    /// There is a fixed variable with an invalid initial value.
    /// correction: false
    {
        printemps::model::Model<int, double> model;
        auto& x = model.create_variables("x", 10, 0, 1);
        model.create_constraint("g", x.selection());
        x(0).fix_by(2);

        model.categorize_variables();
        model.categorize_constraints();
        printemps::presolver::extract_selections_by_defined_order(&model,
                                                                  false);

        ASSERT_THROW(
            printemps::verifier::
                verify_and_correct_selection_variables_initial_values(  //
                    &model, false, false),
            std::logic_error);
    }

    /// There is one fixed selected variable.
    /// correction: true
    {
        printemps::model::Model<int, double> model;
        auto& x = model.create_variables("x", 10, 0, 1);
        model.create_constraint("g", x.selection());
        x(0).fix_by(1);

        model.categorize_variables();
        model.categorize_constraints();
        printemps::presolver::extract_selections_by_defined_order(&model,
                                                                  false);

        printemps::verifier::
            verify_and_correct_selection_variables_initial_values(  //
                &model, true, false);
        EXPECT_EQ(1, x(0).value());
    }

    /// There is one fixed selected variable.
    /// correction: false
    {
        printemps::model::Model<int, double> model;
        auto& x = model.create_variables("x", 10, 0, 1);
        model.create_constraint("g", x.selection());
        x(0).fix_by(1);

        model.categorize_variables();
        model.categorize_constraints();
        printemps::presolver::extract_selections_by_defined_order(&model,
                                                                  false);

        printemps::verifier::
            verify_and_correct_selection_variables_initial_values(  //
                &model, false, false);
        EXPECT_EQ(1, x(0).value());
    }

    /// There are two fixed selected variables.
    /// correction: true
    {
        printemps::model::Model<int, double> model;
        auto& x = model.create_variables("x", 10, 0, 1);
        model.create_constraint("g", x.selection());
        x(0).fix_by(1);
        x(1).fix_by(1);

        model.categorize_variables();
        model.categorize_constraints();
        printemps::presolver::extract_selections_by_defined_order(&model,
                                                                  false);

        ASSERT_THROW(
            printemps::verifier::
                verify_and_correct_selection_variables_initial_values(  //
                    &model, true, false),
            std::logic_error);
    }

    /// There are two fixed selected variables.
    /// correction: true
    {
        printemps::model::Model<int, double> model;
        auto& x = model.create_variables("x", 10, 0, 1);
        model.create_constraint("g", x.selection());
        x(0).fix_by(1);
        x(1).fix_by(1);

        model.categorize_variables();
        model.categorize_constraints();
        printemps::presolver::extract_selections_by_defined_order(&model,
                                                                  false);

        ASSERT_THROW(
            printemps::verifier::
                verify_and_correct_selection_variables_initial_values(  //
                    &model, false, false),
            std::logic_error);
    }

    /// There are two variables with invalid initial values.
    /// correction: true
    {
        printemps::model::Model<int, double> model;
        auto& x = model.create_variables("x", 10, 0, 1);
        model.create_constraint("g", x.selection());
        x(0) = 2;
        x(1) = 3;
        model.categorize_variables();
        model.categorize_constraints();
        printemps::presolver::extract_selections_by_defined_order(&model,
                                                                  false);

        printemps::verifier::
            verify_and_correct_selection_variables_initial_values(  //
                &model, true, false);

        EXPECT_EQ(0, x(0).value());
        EXPECT_EQ(0, x(1).value());
    }

    /// There are two variables with invalid initial values.
    /// correction: false
    {
        printemps::model::Model<int, double> model;
        auto& x = model.create_variables("x", 10, 0, 1);
        model.create_constraint("g", x.selection());
        x(0) = 2;
        x(1) = 3;

        model.categorize_variables();
        model.categorize_constraints();
        printemps::presolver::extract_selections_by_defined_order(&model,
                                                                  false);

        ASSERT_THROW(
            printemps::verifier::
                verify_and_correct_selection_variables_initial_values(  //
                    &model, false, false),
            std::logic_error);
    }

    /// There is no selected variable.
    /// correction: true
    {
        printemps::model::Model<int, double> model;
        auto& x = model.create_variables("x", 10, 0, 1);
        model.create_constraint("g", x.selection());

        model.categorize_variables();
        model.categorize_constraints();
        printemps::presolver::extract_selections_by_defined_order(&model,
                                                                  false);

        printemps::verifier::
            verify_and_correct_selection_variables_initial_values(  //
                &model, true, false);

        /// selected_variable_ptr is not always &x(0).
        auto selected_variable_ptr =
            model.selections().front().variable_ptrs.front();
        EXPECT_EQ(1, selected_variable_ptr->value());
    }

    /// There is no selected variable.
    /// correction: false
    {
        printemps::model::Model<int, double> model;
        auto& x = model.create_variables("x", 10, 0, 1);
        model.create_constraint("g", x.selection());

        model.categorize_variables();
        model.categorize_constraints();
        printemps::presolver::extract_selections_by_defined_order(&model,
                                                                  false);

        ASSERT_THROW(
            printemps::verifier::
                verify_and_correct_selection_variables_initial_values(  //
                    &model, false, false),
            std::logic_error);
    }

    /// There is one selected variable.
    /// correction: true
    {
        printemps::model::Model<int, double> model;
        auto& x = model.create_variables("x", 10, 0, 1);
        model.create_constraint("g", x.selection());
        x(0) = 1;

        model.categorize_variables();
        model.categorize_constraints();
        printemps::presolver::extract_selections_by_defined_order(&model,
                                                                  false);

        printemps::verifier::
            verify_and_correct_selection_variables_initial_values(  //
                &model, true, false);

        EXPECT_EQ(1, x(0).value());
    }

    /// There is one selected variable.
    /// correction: false
    {
        printemps::model::Model<int, double> model;
        auto& x = model.create_variables("x", 10, 0, 1);
        model.create_constraint("g", x.selection());
        x(0) = 1;

        model.categorize_variables();
        model.categorize_constraints();
        printemps::presolver::extract_selections_by_defined_order(&model,
                                                                  false);

        printemps::verifier::
            verify_and_correct_selection_variables_initial_values(  //
                &model, false, false);

        EXPECT_EQ(1, x(0).value());
    }

    /// There are two unfixed selected variable.
    /// correction: true
    {
        printemps::model::Model<int, double> model;
        auto& x = model.create_variables("x", 10, 0, 1);
        model.create_constraint("g", x.selection());
        x(0) = 1;
        x(1) = 1;

        model.categorize_variables();
        model.categorize_constraints();
        printemps::presolver::extract_selections_by_defined_order(&model,
                                                                  false);

        printemps::verifier::
            verify_and_correct_selection_variables_initial_values(  //
                &model, true, false);

        /// selected_variable is not always x(0).
        EXPECT_EQ(1, x(0).value() + x(1).value());
    }

    /// There are two unfixed selected variable.
    /// correction: false
    {
        printemps::model::Model<int, double> model;
        auto& x = model.create_variables("x", 10, 0, 1);
        model.create_constraint("g", x.selection());
        x(0) = 1;
        x(1) = 1;

        model.categorize_variables();
        model.categorize_constraints();
        printemps::presolver::extract_selections_by_defined_order(&model,
                                                                  false);

        ASSERT_THROW(
            printemps::verifier::
                verify_and_correct_selection_variables_initial_values(  //
                    &model, false, false),
            std::logic_error);
    }

    /// There are 1 fixed and 1 unfixed selected variable.
    /// correction: true
    {
        printemps::model::Model<int, double> model;
        auto& x = model.create_variables("x", 10, 0, 1);
        model.create_constraint("g", x.selection());
        x(0) = 1;
        x(1).fix_by(1);

        model.categorize_variables();
        model.categorize_constraints();
        printemps::presolver::extract_selections_by_defined_order(&model,
                                                                  false);
        printemps::verifier::
            verify_and_correct_selection_variables_initial_values(  //
                &model, true, false);

        EXPECT_EQ(0, x(0).value());
        EXPECT_EQ(1, x(1).value());
    }

    /// There are 1 fixed and 1 unfixed selected variable.
    /// correction: false
    {
        printemps::model::Model<int, double> model;
        auto& x = model.create_variables("x", 10, 0, 1);
        model.create_constraint("g", x.selection());
        x(0) = 1;
        x(1).fix_by(1);

        model.categorize_variables();
        model.categorize_constraints();
        printemps::presolver::extract_selections_by_defined_order(&model,
                                                                  false);

        ASSERT_THROW(
            printemps::verifier::
                verify_and_correct_selection_variables_initial_values(  //
                    &model, false, false),
            std::logic_error);
    }
}

/*****************************************************************************/
TEST_F(TestVerifier, verify_and_correct_binary_variables_initial_values) {
    /// There is a fixed variable with an invalid initial value.
    /// correction: true
    {
        printemps::model::Model<int, double> model;
        auto& x = model.create_variables("x", 10, 0, 1);
        x(0).fix_by(2);

        model.categorize_variables();
        model.categorize_constraints();

        ASSERT_THROW(  //
            printemps::verifier::
                verify_and_correct_binary_variables_initial_values(  //
                    &model, true, false),
            std::logic_error);
    }

    /// There is a fixed variable with an invalid initial value.
    /// correction: false
    {
        printemps::model::Model<int, double> model;
        auto& x = model.create_variables("x", 10, 0, 1);
        x(0).fix_by(-1);

        model.categorize_variables();
        model.categorize_constraints();

        ASSERT_THROW(  //
            printemps::verifier::
                verify_and_correct_binary_variables_initial_values(  //
                    &model, false, false),
            std::logic_error);
    }

    /// There is a variable with an invalid initial value.
    /// correction: true
    {
        printemps::model::Model<int, double> model;
        auto& x = model.create_variables("x", 10, 0, 1);
        x(0)    = 2;
        x(1)    = -1;

        model.categorize_variables();
        model.categorize_constraints();

        printemps::verifier::
            verify_and_correct_binary_variables_initial_values(  //
                &model, true, false);
        EXPECT_EQ(1, x(0).value());
        EXPECT_EQ(0, x(1).value());
    }

    /// There is a variable with an invalid initial value.
    /// correction: false
    {
        printemps::model::Model<int, double> model;
        auto& x = model.create_variables("x", 10, 0, 1);
        x(0)    = 2;
        x(1)    = -1;

        model.categorize_variables();
        model.categorize_constraints();

        ASSERT_THROW(  //
            printemps::verifier::
                verify_and_correct_binary_variables_initial_values(  //
                    &model, false, false),
            std::logic_error);
    }
}

/*****************************************************************************/
TEST_F(TestVerifier, verify_and_correct_integer_variables_initial_values) {
    /// There is a fixed variable with an invalid initial value.
    /// correction: true
    {
        printemps::model::Model<int, double> model;
        auto& x = model.create_variables("x", 10, -10, 10);
        x(0).fix_by(11);

        model.categorize_variables();
        model.categorize_constraints();

        ASSERT_THROW(  //
            printemps::verifier::
                verify_and_correct_integer_variables_initial_values(  //
                    &model, true, false),
            std::logic_error);
    }

    /// There is a fixed variable with an invalid initial value.
    /// correction: false
    {
        printemps::model::Model<int, double> model;
        auto& x = model.create_variables("x", 10, -10, 10);
        x(0).fix_by(-11);

        model.categorize_variables();
        model.categorize_constraints();

        ASSERT_THROW(  //
            printemps::verifier::
                verify_and_correct_integer_variables_initial_values(  //
                    &model, false, false),
            std::logic_error);
    }

    /// There is a variable with an invalid initial value.
    /// correction: true
    {
        printemps::model::Model<int, double> model;
        auto& x = model.create_variables("x", 10, -10, 10);
        x(0)    = 11;
        x(1)    = -11;

        model.categorize_variables();
        model.categorize_constraints();

        printemps::verifier::
            verify_and_correct_integer_variables_initial_values(  //
                &model, true, false);
        EXPECT_EQ(10, x(0).value());
        EXPECT_EQ(-10, x(1).value());
    }

    /// There is a variable with an invalid initial value.
    /// correction: false
    {
        printemps::model::Model<int, double> model;
        auto& x = model.create_variables("x", 10, -10, 10);
        x(0)    = 11;
        x(1)    = -11;

        model.categorize_variables();
        model.categorize_constraints();

        ASSERT_THROW(  //
            printemps::verifier::
                verify_and_correct_integer_variables_initial_values(  //
                    &model, false, false),
            std::logic_error);
    }
}
/*****************************************************************************/
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/