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
class TestModel : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestModel, initialize) {
    model::Model<int, double> model;

    /// These declarations are for googletest constraint.
    /// http://opencv.jp/googletestdocs/FAQ.html#faq-the-compiler-complains-about-undefined-reference
    auto max_number_of_variable_proxies = model_handler::
        ModelComponentCreatorConstant::MAX_NUMBER_OF_VARIABLE_PROXIES;
    auto max_number_of_expression_proxies = model_handler::
        ModelComponentCreatorConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES;
    auto max_number_of_constraint_proxies = model_handler::
        ModelComponentCreatorConstant::MAX_NUMBER_OF_CONSTRAINT_PROXIES;

    EXPECT_EQ("", model.name());

    EXPECT_EQ(max_number_of_variable_proxies,
              static_cast<int>(model.variable_proxies().capacity()));
    EXPECT_EQ(max_number_of_expression_proxies,
              static_cast<int>(model.expression_proxies().capacity()));
    EXPECT_EQ(max_number_of_constraint_proxies,
              static_cast<int>(model.constraint_proxies().capacity()));

    EXPECT_TRUE(model.variable_names().empty());
    EXPECT_TRUE(model.expression_names().empty());
    EXPECT_TRUE(model.constraint_names().empty());

    EXPECT_FALSE(model.is_defined_objective());
    EXPECT_TRUE(model.is_minimization());
    EXPECT_FALSE(model.is_solved());
    EXPECT_FALSE(model.is_integer());
    EXPECT_FALSE(model.is_feasible());
    EXPECT_FALSE(model.current_is_feasible());
    EXPECT_FALSE(model.previous_is_feasible());
    EXPECT_EQ(0.0, model.global_penalty_coefficient());
    EXPECT_FLOAT_EQ(1.0, model.sign());

    EXPECT_TRUE(model.selections().empty());
    EXPECT_TRUE(model.current_violative_constraint_ptrs().empty());
    EXPECT_TRUE(model.previous_violative_constraint_ptrs().empty());

    EXPECT_TRUE(model.flippable_variable_ptr_pairs().empty());
}

/*****************************************************************************/
TEST_F(TestModel, constructor_arg_name) {
    model::Model<int, double> model("name");
    EXPECT_EQ("name", model.name());
}

/*****************************************************************************/
TEST_F(TestModel, build) {
    /// This test is covered by test_model_builder.cpp.
}

/*****************************************************************************/
TEST_F(TestModel, set_name) {
    model::Model<int, double> model;
    EXPECT_EQ("", model.name());
    model.set_name("name");
    EXPECT_EQ("name", model.name());
}

/*****************************************************************************/
TEST_F(TestModel, name) {
    /// This test is covered by set_name().
}

/*****************************************************************************/
TEST_F(TestModel, create_variable) {
    /// This test is covered by test_model_component_creator.cpp.
}

/*****************************************************************************/
TEST_F(TestModel, create_variables) {
    /// This test is covered by test_model_component_creator.cpp.
}

/*****************************************************************************/
TEST_F(TestModel, create_expression) {
    /// This test is covered by test_model_component_creator.cpp.
}

/*****************************************************************************/
TEST_F(TestModel, create_expressions) {
    /// This test is covered by test_model_component_creator.cpp.
}

/*****************************************************************************/
TEST_F(TestModel, create_constraint) {
    /// This test is covered by test_model_component_creator.cpp.
}

/*****************************************************************************/
TEST_F(TestModel, create_constraints) {
    /// This test is covered by test_model_component_creator.cpp.
}

/*****************************************************************************/
TEST_F(TestModel, minimize) {
    /// This test is covered by test_model_component_creator.cpp.
}

/*****************************************************************************/
TEST_F(TestModel, maximize) {
    /// This test is covered by test_model_component_creator.cpp.
}

/*****************************************************************************/
TEST_F(TestModel, variable_proxies) {
    /// This test is covered by test_model_component_creator.cpp.
}

/*****************************************************************************/
TEST_F(TestModel, expression_proxies) {
    /// This test is covered by test_model_component_creator.cpp.
}

/*****************************************************************************/
TEST_F(TestModel, constraint_proxies) {
    /// This test is covered by test_model_component_creator.cpp.
}

/*****************************************************************************/
TEST_F(TestModel, objective) {
    /// This test is covered by test_model_component_creator.cpp.
}

/*****************************************************************************/
TEST_F(TestModel, dependent_expression_map) {
    /// This test is covered by initialize().
}

/*****************************************************************************/
TEST_F(TestModel, variable_names) {
    /// This test is covered by initialize().
}

/*****************************************************************************/
TEST_F(TestModel, expression_names) {
    /// This test is covered by initialize().
}

/*****************************************************************************/
TEST_F(TestModel, constraint_names) {
    /// This test is covered by initialize().
}

/*****************************************************************************/
TEST_F(TestModel, is_defined_objective) {
    /// This test is covered by initialize().
}

/*****************************************************************************/
TEST_F(TestModel, set_is_minimization) {
    model::Model<int, double> model;
    EXPECT_TRUE(model.is_minimization());
    model.set_is_minimization(false);
    EXPECT_FALSE(model.is_minimization());
    model.set_is_minimization(true);
    EXPECT_TRUE(model.is_minimization());
}

/*****************************************************************************/
TEST_F(TestModel, is_minimization) {
    /// This test is covered by set_is_minimization().
}

/*****************************************************************************/
TEST_F(TestModel, set_is_solved) {
    model::Model<int, double> model;
    EXPECT_FALSE(model.is_solved());
    model.set_is_solved(true);
    EXPECT_TRUE(model.is_solved());
    model.set_is_solved(false);
    EXPECT_FALSE(model.is_solved());
}

/*****************************************************************************/
TEST_F(TestModel, is_solved) {
    /// This test is covered by set_is_solved().
}

/*****************************************************************************/
TEST_F(TestModel, is_integer) {
    /// This test is covered by test_model_builder.cpp.
}

/*****************************************************************************/
TEST_F(TestModel, is_feasible) {
    /// This test is covered by initialize().
}

/*****************************************************************************/
TEST_F(TestModel, current_is_feasible) {
    /// This test is covered by initialize().
}

/*****************************************************************************/
TEST_F(TestModel, previous_is_feasible) {
    /// This test is covered by initialize().
}

/*****************************************************************************/
TEST_F(TestModel, set_global_penalty_coefficient) {
    model::Model<int, double> model;
    model.set_global_penalty_coefficient(1E7);
    EXPECT_FLOAT_EQ(1E7, model.global_penalty_coefficient());
}

/*****************************************************************************/
TEST_F(TestModel, global_penalty_coefficient) {
    /// This test is covered by set_global_penalty_coefficient().
}

/*****************************************************************************/
TEST_F(TestModel, sign) {
    /// This test is covered by initialize().
}

/*****************************************************************************/
TEST_F(TestModel, selections) {
    /// This test is covered by initialize().
}

/*****************************************************************************/
TEST_F(TestModel, current_violative_constraint_ptrs) {
    /// This test is covered by initialize().
}

/*****************************************************************************/
TEST_F(TestModel, previous_violative_constraint_ptrs) {
    /// This test is covered by initialize().
}

/*****************************************************************************/
TEST_F(TestModel, flippable_variable_ptr_pairs) {
    /// This test is covered by initialize().
}

/*****************************************************************************/
TEST_F(TestModel, neighborhood) {
    /// This test is skipped because it is just a getter.
}

/*****************************************************************************/
TEST_F(TestModel, problem_size_reducer) {
    /// This test is skipped because it is just a getter.
}

/*****************************************************************************/
TEST_F(TestModel, verifier) {
    /// This test is skipped because it is just a getter.
}

/*****************************************************************************/
TEST_F(TestModel, reference) {
    /// This test is skipped because it is just a getter.
}

/*****************************************************************************/
TEST_F(TestModel, reference_original) {
    /// This test is skipped because it is just a getter.
}

/*****************************************************************************/
TEST_F(TestModel, builder) {
    /// This test is skipped because it is just a getter.
}

/*****************************************************************************/
TEST_F(TestModel, component_creator) {
    /// This test is skipped because it is just a getter.
}

/*****************************************************************************/
TEST_F(TestModel, evaluator) {
    /// This test is skipped because it is just a getter.
}

/*****************************************************************************/
TEST_F(TestModel, updater) {
    /// This test is skipped because it is just a getter.
}

/*****************************************************************************/
TEST_F(TestModel, printer) {
    /// This test is skipped because it is just a getter.
}

/*****************************************************************************/
TEST_F(TestModel, mps_handler) {
    /// This test is skipped because it is just a getter.
}

/*****************************************************************************/
TEST_F(TestModel, opb_handler) {
    /// This test is skipped because it is just a getter.
}

/*****************************************************************************/
TEST_F(TestModel, json_handler) {
    /// This test is skipped because it is just a getter.
}

/*****************************************************************************/
TEST_F(TestModel, linear_programming_handler) {
    /// This test is skipped because it is just a getter.
}

/*****************************************************************************/
TEST_F(TestModel, initial_solution_handler) {
    /// This test is skipped because it is just a getter.
}

/*****************************************************************************/
TEST_F(TestModel, state_inspector) {
    /// This test is skipped because it is just a getter.
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/