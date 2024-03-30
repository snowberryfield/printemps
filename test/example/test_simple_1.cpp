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
class TestSimple1 : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestSimple1, simple_1) {
    /**
     * This program solves the following simple liner integer optimization
     * problem [1]:
     * (P):  minimize       x_1 + 10 x_2
     *          x
     *      subject to   66 x_1 + 14 x_2 >= 1430,
     *                  -82 x_1 + 28 x_2 >= 1306,
     *                      x_1 and x_2 are integer.
     *
     * The problem (P) has an optimal solution (x_1, x_2) = (7, 10) with
     * objective function value 707.
     *
     * [1] R.Fletcher: Practical Methods of Optimization, Second Edition,
     * John Wiley & Sons (2000).
     */
    model::IPModel model;

    auto& x = model.create_variables("x", 2, -100, 100);
    auto& g = model.create_constraints("g", 2);

    g(0) = 66 * x(0) + 14 * x(1) >= 1430;
    g(1) = -82 * x(0) + 28 * x(1) >= 1306;
    model.minimize(x(0) + 10 * x(1));

    /// initial solution(consider presolving)
    x(0) = 18;
    x(1) = 50;

    /// solve
    option::Option option;

    auto result = solver::solve(&model, option);
    EXPECT_TRUE(result.solution.is_feasible());
    EXPECT_EQ(7, result.solution.variables("x").values(0));
    EXPECT_EQ(70, result.solution.variables("x").values(1));

    ASSERT_THROW(solver::solve(&model, option), std::logic_error);
}
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/