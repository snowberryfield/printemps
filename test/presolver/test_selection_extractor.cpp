/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <random>

#include <printemps.h>

namespace {
/*****************************************************************************/
class TestSelectionExtractor : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestSelectionExtractor, extract_by_defined_order) {
    printemps::model::Model<int, double> model;

    auto& x_0 = model.create_variables("x_0", {10, 10}, 0, 1);
    auto& x_1 = model.create_variables("x_1", {20, 20}, 0, 1);
    auto& x_2 = model.create_variables("x_2", 2, 0, 1);

    /**
     * Selection constraint with 10 decision variables. The priority of this
     * constraint is the first.
     */
    model.create_constraint(
        "c_0", x_0.selection({0, printemps::model_component::Range::All}));

    /**
     * Selection constraint with 31 decision variables. The priority of this
     * constraint is the second.
     */
    model.create_constraint(
        "c_1",
        (x_0.sum({0, printemps::model_component::Range::All}) +
         x_1.sum({1, printemps::model_component::Range::All}) + x_2(0)) == 1);

    /**
     * Selection constraint with 400 decision variables. The priority of this
     * constraint is the 4th.
     */
    model.create_constraint("c_2", x_1.selection());

    /**
     * Selection constraint with 2 decision variables.
     */
    model.create_constraint("c_3", x_2.selection());

    model.setup_structure();
    printemps::presolver::SelectionExtractor<int, double> selection_extractor(
        &model);
    selection_extractor.extract_by_defined_order(false);
    model.setup_structure();

    EXPECT_EQ(3, model.number_of_selection_constraints());
    EXPECT_EQ(3, static_cast<int>(model.selections().size()));

    /**
     * Check the numbers of covered variables and variable pointers.
     */
    {
        /// Constraint c_0
        auto variable_ptrs = model.selections()[0].variable_ptrs;
        EXPECT_EQ(10, static_cast<int>(variable_ptrs.size()));

        EXPECT_TRUE((std::find(variable_ptrs.begin(), variable_ptrs.end(),
                               &x_0(0, 0)) != variable_ptrs.end()));
        EXPECT_TRUE((std::find(variable_ptrs.begin(), variable_ptrs.end(),
                               &x_0(0, 9)) != variable_ptrs.end()));
    }

    {
        /// Constraint c_2
        auto variable_ptrs = model.selections()[1].variable_ptrs;
        EXPECT_EQ(400, static_cast<int>(variable_ptrs.size()));

        EXPECT_TRUE((std::find(variable_ptrs.begin(), variable_ptrs.end(),
                               &x_1(0, 0)) != variable_ptrs.end()));
        EXPECT_TRUE((std::find(variable_ptrs.begin(), variable_ptrs.end(),
                               &x_1(19, 19)) != variable_ptrs.end()));
    }

    {
        /// Constraint c_3
        auto variable_ptrs = model.selections()[2].variable_ptrs;
        EXPECT_EQ(2, static_cast<int>(variable_ptrs.size()));

        EXPECT_TRUE((std::find(variable_ptrs.begin(), variable_ptrs.end(),
                               &x_2(0)) != variable_ptrs.end()));
        EXPECT_TRUE((std::find(variable_ptrs.begin(), variable_ptrs.end(),
                               &x_2(1)) != variable_ptrs.end()));
    }

    /**
     * Check whether the corresponding constraint is enabled or not.
     */

    /// Constraint c_0
    EXPECT_FALSE(model.selections()[0].constraint_ptr->is_enabled());

    /// Constraint c_2
    EXPECT_FALSE(model.selections()[1].constraint_ptr->is_enabled());

    /// Constraint c_2
    EXPECT_FALSE(model.selections()[2].constraint_ptr->is_enabled());

    /**
     * Check the number of covered variables and variable pointers for each
     * category.
     */

    /// Selection
    {
        auto variable_ptrs = model.variable_reference().selection_variable_ptrs;
        EXPECT_EQ(10 + 20 * 20 + 2, model.number_of_selection_variables());
    }

    /// Binary
    {
        auto variable_ptrs = model.variable_reference().binary_variable_ptrs;
        EXPECT_EQ(10 * 10 + 20 * 20 + 2 - (10 + 20 * 20 + 2),
                  model.number_of_binary_variables());
    }
}

/*****************************************************************************/
TEST_F(TestSelectionExtractor, extract_by_number_of_variables_smaller_order) {
    printemps::model::Model<int, double> model;

    auto& x_0 = model.create_variables("x_0", {10, 10}, 0, 1);
    auto& x_1 = model.create_variables("x_1", {20, 20}, 0, 1);
    auto& x_2 = model.create_variables("x_2", 2, 0, 1);

    /**
     * Selection constraint with 10 decision variables. The priority of this
     * constraint is the second.
     */
    model.create_constraint(
        "c_0", x_0.selection({0, printemps::model_component::Range::All}));

    /**
     * Selection constraint with 31 decision variables. The priority of this
     * constraint is the third.
     */
    model.create_constraint(
        "c_1",
        (x_0.sum({1, printemps::model_component::Range::All}) +
         x_1.sum({1, printemps::model_component::Range::All}) + x_2(0)) == 1);

    /**
     * Selection constraint with 400 decision variables. The priority of this
     * constraint is the fourth.
     */
    model.create_constraint("c_2", x_1.selection());

    /**
     * Selection constraint with 2 decision variables. The priority of this
     * constraint is the first.
     */
    model.create_constraint("c_3", x_2.selection());

    model.setup_structure();

    printemps::presolver::SelectionExtractor<int, double> selection_extractor(
        &model);
    selection_extractor.extract_by_number_of_variables_order(true, false);
    model.setup_structure();

    EXPECT_EQ(3, model.number_of_selection_constraints());
    EXPECT_EQ(3, static_cast<int>(model.selections().size()));

    /**
     * Check the numbers of covered variables and variable pointers.
     */
    {
        /// Constraint c_3
        auto variable_ptrs = model.selections()[0].variable_ptrs;
        EXPECT_EQ(2, static_cast<int>(variable_ptrs.size()));

        EXPECT_TRUE((std::find(variable_ptrs.begin(), variable_ptrs.end(),
                               &x_2(0)) != variable_ptrs.end()));
        EXPECT_TRUE((std::find(variable_ptrs.begin(), variable_ptrs.end(),
                               &x_2(1)) != variable_ptrs.end()));
    }

    {
        /// Constraint c_0
        auto variable_ptrs = model.selections()[1].variable_ptrs;
        EXPECT_EQ(10, static_cast<int>(variable_ptrs.size()));

        EXPECT_TRUE((std::find(variable_ptrs.begin(), variable_ptrs.end(),
                               &x_0(0, 0)) != variable_ptrs.end()));
        EXPECT_TRUE((std::find(variable_ptrs.begin(), variable_ptrs.end(),
                               &x_0(0, 9)) != variable_ptrs.end()));
    }

    {
        /// Constraint c_1
        auto variable_ptrs = model.selections()[2].variable_ptrs;
        EXPECT_EQ(400, static_cast<int>(variable_ptrs.size()));

        EXPECT_TRUE((std::find(variable_ptrs.begin(), variable_ptrs.end(),
                               &x_1(0, 0)) != variable_ptrs.end()));
        EXPECT_TRUE((std::find(variable_ptrs.begin(), variable_ptrs.end(),
                               &x_1(19, 19)) != variable_ptrs.end()));
    }

    /**
     * Check whether the corresponding constraint is enabled or not.
     */

    /// Constraint c_3
    EXPECT_FALSE(model.selections()[0].constraint_ptr->is_enabled());

    /// Constraint c_0
    EXPECT_FALSE(model.selections()[1].constraint_ptr->is_enabled());

    /// Constraint c_1
    EXPECT_FALSE(model.selections()[2].constraint_ptr->is_enabled());

    /**
     * Check the number of covered variables and variable pointers for each
     * category.
     */

    /// Selection
    {
        auto variable_ptrs = model.variable_reference().selection_variable_ptrs;
        EXPECT_EQ(2 + 10 + 400, model.number_of_selection_variables());
    }

    /// Binary
    {
        auto variable_ptrs = model.variable_reference().binary_variable_ptrs;
        EXPECT_EQ(10 * 10 + 20 * 20 + 2 - (2 + 10 + 400),
                  model.number_of_binary_variables());
    }
}

/*****************************************************************************/
TEST_F(TestSelectionExtractor,
       extract_selections_by_number_of_variables_larger_order) {
    printemps::model::Model<int, double> model;

    auto& x_0 = model.create_variables("x_0", {10, 10}, 0, 1);
    auto& x_1 = model.create_variables("x_1", {20, 20}, 0, 1);
    auto& x_2 = model.create_variables("x_2", 2, 0, 1);

    /**
     * Selection constraint with 10 decision variables. The priority of this
     * constraint is the third.
     */
    model.create_constraint(
        "c_0", x_0.selection({0, printemps::model_component::Range::All}));

    /**
     * Selection constraint with 31 decision variables. The priority of this
     * constraint is the second. It will NOT be employed as selection constraint
     * because higher-priority constraint c_1 covers x_1.
     */
    model.create_constraint(
        "c_1",
        (x_0.sum({1, printemps::model_component::Range::All}) +
         x_1.sum({1, printemps::model_component::Range::All}) + x_2(0)) == 1);

    /**
     * Selection constraint with 400 decision variables. The priority of this
     * constraint is the first.
     */
    model.create_constraint("c_2", x_1.selection());

    /**
     * Selection constraint with 400 decision variables. The priority of this
     * constraint is the fourth.
     */
    model.create_constraint("c_3", x_2.selection());

    model.setup_structure();
    printemps::presolver::SelectionExtractor<int, double> selection_extractor(
        &model);
    selection_extractor.extract_by_number_of_variables_order(false, false);
    model.setup_structure();

    model.setup_structure();

    EXPECT_EQ(3, model.number_of_selection_constraints());
    EXPECT_EQ(3, static_cast<int>(model.selections().size()));

    /**
     * Check the numbers of covered variables and variable pointers.
     */
    {
        /// Constraint c_2
        auto variable_ptrs = model.selections()[0].variable_ptrs;
        EXPECT_EQ(400, static_cast<int>(variable_ptrs.size()));

        EXPECT_TRUE((std::find(variable_ptrs.begin(), variable_ptrs.end(),
                               &x_1(0, 0)) != variable_ptrs.end()));
        EXPECT_TRUE((std::find(variable_ptrs.begin(), variable_ptrs.end(),
                               &x_1(19, 19)) != variable_ptrs.end()));
    }

    {
        /// Constraint c_0
        auto variable_ptrs = model.selections()[1].variable_ptrs;
        EXPECT_EQ(10, static_cast<int>(variable_ptrs.size()));

        EXPECT_TRUE((std::find(variable_ptrs.begin(), variable_ptrs.end(),
                               &x_0(0, 0)) != variable_ptrs.end()));
        EXPECT_TRUE((std::find(variable_ptrs.begin(), variable_ptrs.end(),
                               &x_0(0, 9)) != variable_ptrs.end()));
    }

    {
        /// Constraint c_3
        auto variable_ptrs = model.selections()[2].variable_ptrs;
        EXPECT_EQ(2, static_cast<int>(variable_ptrs.size()));

        EXPECT_TRUE((std::find(variable_ptrs.begin(), variable_ptrs.end(),
                               &x_2(0)) != variable_ptrs.end()));
        EXPECT_TRUE((std::find(variable_ptrs.begin(), variable_ptrs.end(),
                               &x_2(1)) != variable_ptrs.end()));
    }

    /**
     * Check whether the corresponding constraint is enabled or not.
     */

    /// Constraint c_2
    EXPECT_FALSE(model.selections()[0].constraint_ptr->is_enabled());

    /// Constraint c_0
    EXPECT_FALSE(model.selections()[1].constraint_ptr->is_enabled());

    /// Constraint c_3
    EXPECT_FALSE(model.selections()[2].constraint_ptr->is_enabled());

    /**
     * Check the number of covered variables and variable pointers for each
     * category.
     */

    /// Selection
    {
        auto variable_ptrs = model.variable_reference().selection_variable_ptrs;
        EXPECT_EQ(20 * 20 + 1 * 10 + 2, model.number_of_selection_variables());
    }

    /// Binary
    {
        auto variable_ptrs = model.variable_reference().binary_variable_ptrs;
        EXPECT_EQ(10 * 10 + 20 * 20 + 2 - (20 * 20 + 1 * 10 + 2),
                  model.number_of_binary_variables());
    }
}

/*****************************************************************************/
TEST_F(TestSelectionExtractor, extract_selections_independent) {
    printemps::model::Model<int, double> model;

    auto& x_0 = model.create_variables("x_0", {10, 10}, 0, 1);
    auto& x_1 = model.create_variables("x_1", {20, 20}, 0, 1);
    auto& x_2 = model.create_variables("x_2", 2, 0, 1);

    /**
     * Selection constraint with 10 decision variables (no overlap).
     */
    model.create_constraint(
        "c_0", x_0.selection({0, printemps::model_component::Range::All}));

    /**
     * Selection constraint with 32 decision variables (overlap).
     */
    model.create_constraint(
        "c_1",
        (x_0.sum({1, printemps::model_component::Range::All}) +
         x_1.sum({1, printemps::model_component::Range::All}) + x_2(0)) == 1);

    /**
     * Selection constraint with 400 decision variables (overlap).
     */
    model.create_constraint("c_2", x_1.selection());

    /**
     * Selection constraint with 2 decision variables (overlap).
     */
    model.create_constraint("c_3", x_2.selection());

    model.setup_structure();
    printemps::presolver::SelectionExtractor<int, double> selection_extractor(
        &model);
    selection_extractor.extract_independent(false);
    model.setup_structure();

    EXPECT_EQ(1, model.number_of_selection_constraints());
    EXPECT_EQ(1, static_cast<int>(model.selections().size()));

    /**
     * Check the numbers of covered variables and variable pointers.
     */
    {
        /// Constraint c_0
        auto variable_ptrs = model.selections()[0].variable_ptrs;
        EXPECT_EQ(10, static_cast<int>(variable_ptrs.size()));

        EXPECT_TRUE((std::find(variable_ptrs.begin(), variable_ptrs.end(),
                               &x_0(0, 0)) != variable_ptrs.end()));
        EXPECT_TRUE((std::find(variable_ptrs.begin(), variable_ptrs.end(),
                               &x_0(0, 9)) != variable_ptrs.end()));
    }

    /**
     * Check whether the corresponding constraint is enabled or not.
     */

    /// Constraint c_0
    EXPECT_FALSE(model.selections()[0].constraint_ptr->is_enabled());

    /**
     * Check the number of covered variables and variable pointers for each
     * category.
     */

    /// Selection
    {
        auto variable_ptrs = model.variable_reference().selection_variable_ptrs;
        EXPECT_EQ(10, model.number_of_selection_variables());
    }

    /// Binary
    {
        auto variable_ptrs = model.variable_reference().binary_variable_ptrs;
        EXPECT_EQ(10 * 10 + 20 * 20 + 2 - 10,
                  model.number_of_binary_variables());
    }
}
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/