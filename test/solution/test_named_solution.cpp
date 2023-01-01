/*****************************************************************************/
// Copyright (c) 2020-2023 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
using namespace printemps;
/*****************************************************************************/
class TestNamedSolution : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestNamedSolution, constructor) {
    solution::NamedSolution<int, double> solution;

    EXPECT_EQ("", solution.name());
    EXPECT_EQ(0, solution.number_of_variables());
    EXPECT_EQ(0, solution.number_of_constraints());
    EXPECT_EQ(0.0, solution.objective());
    EXPECT_EQ(0.0, solution.total_violation());
    EXPECT_FALSE(solution.is_feasible());
}

/*****************************************************************************/
TEST_F(TestNamedSolution, initialize) {
    model::Model<int, double> model("name");
    auto&                     x = model.create_variables("x", 2, 0, 1);
    auto&                     p = model.create_expression("p");
    auto&                     g = model.create_constraints("g", 2);

    p = x(0) + 2 * x(1);

    g(0) = x(0) + x(1) <= 1;
    g(1) = x(0) - x(1) == 1;
    model.minimize(x(0) + x(1));
    model.setup_structure();
    x(0) = 1;
    x(1) = 1;

    model.update();
    auto solution = model.export_named_solution();

    EXPECT_EQ("name", solution.name());
    EXPECT_EQ(2, solution.number_of_variables());
    EXPECT_EQ(2, solution.number_of_constraints());
    EXPECT_EQ(2.0, solution.objective());
    EXPECT_EQ(2.0, solution.total_violation());
    EXPECT_FALSE(solution.is_feasible());

    EXPECT_EQ(x(0).value(), solution.variables().at("x")[0]);
    EXPECT_EQ(x(0).value(), solution.variables("x")[0]);

    EXPECT_EQ(p(0).value(), solution.expressions().at("p")[0]);
    EXPECT_EQ(p(0).value(), solution.expressions("p")[0]);

    EXPECT_EQ(g(0).constraint_value(), solution.constraints().at("g")[0]);
    EXPECT_EQ(g(0).constraint_value(), solution.constraints("g")[0]);

    EXPECT_EQ(g(0).violation_value(), solution.constraints().at("g")[0]);
    EXPECT_EQ(g(0).violation_value(), solution.constraints("g")[0]);

    solution.initialize();
    EXPECT_EQ("", solution.name());
    EXPECT_EQ(0, solution.number_of_variables());
    EXPECT_EQ(0, solution.number_of_constraints());
    EXPECT_EQ(0.0, solution.objective());
    EXPECT_EQ(0.0, solution.total_violation());
    EXPECT_FALSE(solution.is_feasible());
}

/*****************************************************************************/
TEST_F(TestNamedSolution, variables_without_arguments) {
    /// This method is tested in initialize().
}

/*****************************************************************************/
TEST_F(TestNamedSolution, variables_with_arguments) {
    /// This method is tested in initialize().
}

/*****************************************************************************/
TEST_F(TestNamedSolution, expressions_without_arguments) {
    /// This method is tested in initialize().
}

/*****************************************************************************/
TEST_F(TestNamedSolution, expressions_with_arguments) {
    /// This method is tested in initialize().
}

/*****************************************************************************/
TEST_F(TestNamedSolution, constraints_without_arguments) {
    /// This method is tested in initialize().
}

/*****************************************************************************/
TEST_F(TestNamedSolution, constraints_with_arguments) {
    /// This method is tested in initialize().
}

/*****************************************************************************/
TEST_F(TestNamedSolution, violations_without_arguments) {
    /// This method is tested in initialize().
}

/*****************************************************************************/
TEST_F(TestNamedSolution, violations_with_arguments) {
    /// This method is tested in initialize().
}

/*****************************************************************************/
TEST_F(TestNamedSolution, name) {
    /// This method is tested in initialize().
}

/*****************************************************************************/
TEST_F(TestNamedSolution, number_of_variables) {
    /// This method is tested in initialize().
}

/*****************************************************************************/
TEST_F(TestNamedSolution, number_of_constraints) {
    /// This method is tested in initialize().
}

/*****************************************************************************/
TEST_F(TestNamedSolution, objective) {
    /// This method is tested in initialize().
}

/*****************************************************************************/
TEST_F(TestNamedSolution, total_violation) {
    /// This method is tested in initialize().
}

/*****************************************************************************/
TEST_F(TestNamedSolution, is_feasible) {
    /// This method is tested in initialize().
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/