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
class TestVerifier : public ::testing::Test {
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
TEST_F(TestVerifier, verify_problem) {
    /// No decision variables.
    {
        model::Model<int, double>         model;
        preprocess::Verifier<int, double> verifier(&model);
        ASSERT_THROW(verifier.verify_problem(false), std::runtime_error);
    }

    /// No constraint functions.
    {
        model::Model<int, double> model;

        auto& x = model.create_variable("x");
        model.minimize(x);
        preprocess::Verifier<int, double> verifier(&model);
        verifier.verify_problem(false);
    }

    /// No objective function.
    {
        model::Model<int, double> model;

        auto& x = model.create_variable("x");
        model.create_constraint("g", x == 1);
        preprocess::Verifier<int, double> verifier(&model);
        verifier.verify_problem(false);
    }

    /// No constraint functions and no objective function
    {
        model::Model<int, double> model;

        [[maybe_unused]] auto&            x = model.create_variable("x");
        preprocess::Verifier<int, double> verifier(&model);
        ASSERT_THROW(verifier.verify_problem(false), std::runtime_error);
    }
}

/*****************************************************************************/
TEST_F(TestVerifier, verify_and_correct_selection_variables_initial_values) {
    /// There is a fixed variable with an invalid initial value.
    /// correction: true
    {
        model::Model<int, double> model;

        auto& x = model.create_variables("x", 10, 0, 1);
        model.create_constraint("g", x.selection());
        x(0).fix_by(2);

        model.builder().setup_structure();
        preprocess::SelectionExtractor<int, double> selection_extractor(&model);
        selection_extractor.extract_by_defined_order(false);

        preprocess::Verifier<int, double> verifier(&model);
        ASSERT_THROW(
            verifier.verify_and_correct_selection_variables_initial_values(
                true, false),
            std::runtime_error);
    }

    /// There is a fixed variable with an invalid initial value.
    /// correction: false
    {
        model::Model<int, double> model;
        auto&                     x = model.create_variables("x", 10, 0, 1);
        model.create_constraint("g", x.selection());
        x(0).fix_by(2);

        model.builder().setup_structure();
        preprocess::SelectionExtractor<int, double> selection_extractor(&model);
        selection_extractor.extract_by_defined_order(false);

        preprocess::Verifier<int, double> verifier(&model);
        ASSERT_THROW(
            verifier.verify_and_correct_selection_variables_initial_values(
                false, false),
            std::runtime_error);
    }

    /// There is one fixed selected variable.
    /// correction: true
    {
        model::Model<int, double> model;
        auto&                     x = model.create_variables("x", 10, 0, 1);
        model.create_constraint("g", x.selection());
        x(0).fix_by(1);

        model.builder().setup_structure();
        preprocess::SelectionExtractor<int, double> selection_extractor(&model);
        selection_extractor.extract_by_defined_order(false);

        preprocess::Verifier<int, double> verifier(&model);
        verifier.verify_and_correct_selection_variables_initial_values(true,
                                                                       false);
        EXPECT_EQ(1, x(0).value());
    }

    /// There is one fixed selected variable.
    /// correction: false
    {
        model::Model<int, double> model;
        auto&                     x = model.create_variables("x", 10, 0, 1);
        model.create_constraint("g", x.selection());
        x(0).fix_by(1);

        model.builder().setup_structure();
        preprocess::SelectionExtractor<int, double> selection_extractor(&model);
        selection_extractor.extract_by_defined_order(false);

        preprocess::Verifier<int, double> verifier(&model);
        verifier.verify_and_correct_selection_variables_initial_values(false,
                                                                       false);
        EXPECT_EQ(1, x(0).value());
    }

    /// There are two fixed selected variables.
    /// correction: true
    {
        model::Model<int, double> model;
        auto&                     x = model.create_variables("x", 10, 0, 1);
        model.create_constraint("g", x.selection());
        x(0).fix_by(1);
        x(1).fix_by(1);

        model.builder().setup_structure();
        preprocess::SelectionExtractor<int, double> selection_extractor(&model);
        selection_extractor.extract_by_defined_order(false);

        preprocess::Verifier<int, double> verifier(&model);
        ASSERT_THROW(
            verifier.verify_and_correct_selection_variables_initial_values(
                true, false),
            std::runtime_error);
    }

    /// There are two fixed selected variables.
    /// correction: true
    {
        model::Model<int, double> model;
        auto&                     x = model.create_variables("x", 10, 0, 1);
        model.create_constraint("g", x.selection());
        x(0).fix_by(1);
        x(1).fix_by(1);

        model.builder().setup_structure();
        preprocess::SelectionExtractor<int, double> selection_extractor(&model);
        selection_extractor.extract_by_defined_order(false);

        preprocess::Verifier<int, double> verifier(&model);
        ASSERT_THROW(
            verifier.verify_and_correct_selection_variables_initial_values(
                false, false),
            std::runtime_error);
    }

    /// There are two variables with invalid initial values.
    /// correction: true
    {
        model::Model<int, double> model;
        auto&                     x = model.create_variables("x", 10, 0, 1);
        model.create_constraint("g", x.selection());
        x(0) = 2;
        x(1) = 3;
        model.builder().setup_structure();
        preprocess::SelectionExtractor<int, double> selection_extractor(&model);
        selection_extractor.extract_by_defined_order(false);

        preprocess::Verifier<int, double> verifier(&model);
        verifier.verify_and_correct_selection_variables_initial_values(true,
                                                                       false);

        EXPECT_EQ(0, x(0).value());
        EXPECT_EQ(0, x(1).value());
    }

    /// There are two variables with invalid initial values.
    /// correction: false
    {
        model::Model<int, double> model;
        auto&                     x = model.create_variables("x", 10, 0, 1);
        model.create_constraint("g", x.selection());
        x(0) = 2;
        x(1) = 3;

        model.builder().setup_structure();
        preprocess::SelectionExtractor<int, double> selection_extractor(&model);
        selection_extractor.extract_by_defined_order(false);

        preprocess::Verifier<int, double> verifier(&model);
        ASSERT_THROW(
            verifier.verify_and_correct_selection_variables_initial_values(
                false, false),
            std::runtime_error);
    }

    /// There is no selected variable.
    /// correction: true
    {
        model::Model<int, double> model;
        auto&                     x = model.create_variables("x", 10, 0, 1);
        model.create_constraint("g", x.selection());

        model.builder().setup_structure();
        preprocess::SelectionExtractor<int, double> selection_extractor(&model);
        selection_extractor.extract_by_defined_order(false);

        preprocess::Verifier<int, double> verifier(&model);
        verifier.verify_and_correct_selection_variables_initial_values(true,
                                                                       false);

        /// selected_variable_ptr is not always &x(0).
        auto selected_variable_ptr =
            model.selections().front().variable_ptrs.front();
        EXPECT_EQ(1, selected_variable_ptr->value());
    }

    /// There is no selected variable.
    /// correction: false
    {
        model::Model<int, double> model;
        auto&                     x = model.create_variables("x", 10, 0, 1);
        model.create_constraint("g", x.selection());

        model.builder().setup_structure();
        preprocess::SelectionExtractor<int, double> selection_extractor(&model);
        selection_extractor.extract_by_defined_order(false);

        preprocess::Verifier<int, double> verifier(&model);

        ASSERT_THROW(
            verifier.verify_and_correct_selection_variables_initial_values(
                false, false),
            std::runtime_error);
    }

    /// There is one selected variable.
    /// correction: true
    {
        model::Model<int, double> model;
        auto&                     x = model.create_variables("x", 10, 0, 1);
        model.create_constraint("g", x.selection());
        x(0) = 1;

        model.builder().setup_structure();
        preprocess::SelectionExtractor<int, double> selection_extractor(&model);
        selection_extractor.extract_by_defined_order(false);

        preprocess::Verifier<int, double> verifier(&model);
        verifier.verify_and_correct_selection_variables_initial_values(true,
                                                                       false);

        EXPECT_EQ(1, x(0).value());
    }

    /// There is one selected variable.
    /// correction: false
    {
        model::Model<int, double> model;
        auto&                     x = model.create_variables("x", 10, 0, 1);
        model.create_constraint("g", x.selection());
        x(0) = 1;

        model.builder().setup_structure();
        preprocess::SelectionExtractor<int, double> selection_extractor(&model);
        selection_extractor.extract_by_defined_order(false);

        preprocess::Verifier<int, double> verifier(&model);
        verifier.verify_and_correct_selection_variables_initial_values(false,
                                                                       false);

        EXPECT_EQ(1, x(0).value());
    }

    /// There are two unfixed selected variable.
    /// correction: true
    {
        model::Model<int, double> model;
        auto&                     x = model.create_variables("x", 10, 0, 1);
        model.create_constraint("g", x.selection());
        x(0) = 1;
        x(1) = 1;

        model.builder().setup_structure();
        preprocess::SelectionExtractor<int, double> selection_extractor(&model);
        selection_extractor.extract_by_defined_order(false);

        preprocess::Verifier<int, double> verifier(&model);
        verifier.verify_and_correct_selection_variables_initial_values(true,
                                                                       false);

        /// selected_variable is not always x(0).
        EXPECT_EQ(1, x(0).value() + x(1).value());
    }

    /// There are two unfixed selected variable.
    /// correction: false
    {
        model::Model<int, double> model;
        auto&                     x = model.create_variables("x", 10, 0, 1);
        model.create_constraint("g", x.selection());
        x(0) = 1;
        x(1) = 1;

        model.builder().setup_structure();
        preprocess::SelectionExtractor<int, double> selection_extractor(&model);
        selection_extractor.extract_by_defined_order(false);

        preprocess::Verifier<int, double> verifier(&model);
        ASSERT_THROW(
            verifier.verify_and_correct_selection_variables_initial_values(
                false, false),
            std::runtime_error);
    }

    /// There are 1 fixed and 1 unfixed selected variable.
    /// correction: true
    {
        model::Model<int, double> model;
        auto&                     x = model.create_variables("x", 10, 0, 1);
        model.create_constraint("g", x.selection());
        x(0) = 1;
        x(1).fix_by(1);

        model.builder().setup_structure();
        preprocess::SelectionExtractor<int, double> selection_extractor(&model);
        selection_extractor.extract_by_defined_order(false);

        preprocess::Verifier<int, double> verifier(&model);
        verifier.verify_and_correct_selection_variables_initial_values(true,
                                                                       false);

        EXPECT_EQ(0, x(0).value());
        EXPECT_EQ(1, x(1).value());
    }

    /// There are 1 fixed and 1 unfixed selected variable.
    /// correction: false
    {
        model::Model<int, double> model;
        auto&                     x = model.create_variables("x", 10, 0, 1);
        model.create_constraint("g", x.selection());
        x(0) = 1;
        x(1).fix_by(1);

        model.builder().setup_structure();
        preprocess::SelectionExtractor<int, double> selection_extractor(&model);
        selection_extractor.extract_by_defined_order(false);

        preprocess::Verifier<int, double> verifier(&model);
        ASSERT_THROW(
            verifier.verify_and_correct_selection_variables_initial_values(
                false, false),
            std::runtime_error);
    }
}

/*****************************************************************************/
TEST_F(TestVerifier, verify_and_correct_binary_variables_initial_values) {
    /// There is a fixed variable with an invalid initial value.
    /// correction: true
    {
        model::Model<int, double> model;
        auto&                     x = model.create_variables("x", 10, 0, 1);
        x(0).fix_by(2);

        model.builder().setup_structure();

        preprocess::Verifier<int, double> verifier(&model);
        ASSERT_THROW(
            verifier.verify_and_correct_binary_variables_initial_values(true,
                                                                        false),
            std::runtime_error);
    }

    /// There is a fixed variable with an invalid initial value.
    /// correction: false
    {
        model::Model<int, double> model;
        auto&                     x = model.create_variables("x", 10, 0, 1);
        x(0).fix_by(-1);

        model.builder().setup_structure();

        preprocess::Verifier<int, double> verifier(&model);
        ASSERT_THROW(
            verifier.verify_and_correct_binary_variables_initial_values(false,
                                                                        false),
            std::runtime_error);
    }

    /// There is a variable with an invalid initial value.
    /// correction: true
    {
        model::Model<int, double> model;
        auto&                     x = model.create_variables("x", 10, 0, 1);
        x(0)                        = 2;
        x(1)                        = -1;

        model.builder().setup_structure();

        preprocess::Verifier<int, double> verifier(&model);
        verifier.verify_and_correct_binary_variables_initial_values(true,
                                                                    false);
        EXPECT_EQ(1, x(0).value());
        EXPECT_EQ(0, x(1).value());
    }

    /// There is a variable with an invalid initial value.
    /// correction: false
    {
        model::Model<int, double> model;
        auto&                     x = model.create_variables("x", 10, 0, 1);
        x(0)                        = 2;
        x(1)                        = -1;

        model.builder().setup_structure();

        preprocess::Verifier<int, double> verifier(&model);
        ASSERT_THROW(
            verifier.verify_and_correct_binary_variables_initial_values(false,
                                                                        false),
            std::runtime_error);
    }
}

/*****************************************************************************/
TEST_F(TestVerifier, verify_and_correct_integer_variables_initial_values) {
    /// There is a fixed variable with an invalid initial value.
    /// correction: true
    {
        model::Model<int, double> model;
        auto&                     x = model.create_variables("x", 10, -10, 10);
        x(0).fix_by(11);

        model.builder().setup_structure();

        preprocess::Verifier<int, double> verifier(&model);
        ASSERT_THROW(
            verifier.verify_and_correct_integer_variables_initial_values(true,
                                                                         false),
            std::runtime_error);
    }

    /// There is a fixed variable with an invalid initial value.
    /// correction: false
    {
        model::Model<int, double> model;
        auto&                     x = model.create_variables("x", 10, -10, 10);
        x(0).fix_by(-11);

        model.builder().setup_structure();

        preprocess::Verifier<int, double> verifier(&model);
        ASSERT_THROW(
            verifier.verify_and_correct_integer_variables_initial_values(false,
                                                                         false),
            std::runtime_error);
    }

    /// There is a variable with an invalid initial value.
    /// correction: true
    {
        model::Model<int, double> model;
        auto&                     x = model.create_variables("x", 10, -10, 10);
        x(0)                        = 11;
        x(1)                        = -11;

        model.builder().setup_structure();

        preprocess::Verifier<int, double> verifier(&model);
        verifier.verify_and_correct_integer_variables_initial_values(true,
                                                                     false);

        EXPECT_EQ(10, x(0).value());
        EXPECT_EQ(-10, x(1).value());
    }

    /// There is a variable with an invalid initial value.
    /// correction: false
    {
        model::Model<int, double> model;
        auto&                     x = model.create_variables("x", 10, -10, 10);
        x(0)                        = 11;
        x(1)                        = -11;

        model.builder().setup_structure();

        preprocess::Verifier<int, double> verifier(&model);
        ASSERT_THROW(
            verifier.verify_and_correct_integer_variables_initial_values(false,
                                                                         false),
            std::runtime_error);
    }
}
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/