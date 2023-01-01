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
class TestSimple2 : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestSimple2, simple_2) {
    /**
     * This program solves the following simple binary integer optimization
     * problem:
     * (P):  minimize  f
     *          x,y
     *      subject to x_1 + x_2 + x_3              >= 2,
     *                                   y_1 + y_2   = 1,
     *                 x_1             + y_1         = 1,
     *                 x_1, x_2, x_3, y_1, y_2 \in {0, 1},
     *
     *        where    f_1 = 2 x_1 + 7 x_2 + 9 x_3 + 1,
     *                 f_2 = 5 y_1 + 6 y_2.
     */
    model::IPModel model;

    auto& x = model.create_variables("x", 3, 0, 1);
    auto& y = model.create_variables("y", 2, 0, 1);
    auto& g = model.create_constraints("g", 3);
    auto& f = model.create_expressions("f", 2);

    std::vector<int> c = {2, 7, 9};
    std::vector<int> d = {5, 6};

    f(0) = x.dot(c) + 1;
    f(1) = y.dot(d);
    g(0) = x.sum() >= 2;
    g(1) = y.selection();
    g(2) = x(0) + y(0) == 1;

    model.minimize(f.sum());

    /// initial solution
    for (auto i = 0; i < 3; i++) {
        x(i) = 0;
    }

    for (auto i = 0; i < 2; i++) {
        y(i) = 0;
    }

    /// solve
    option::Option option;

    auto result = solver::solve(&model, option);
    EXPECT_TRUE(result.solution.is_feasible());
    EXPECT_EQ(1, result.solution.variables("x").values(0));
    EXPECT_EQ(1, result.solution.variables("x").values(1));
    EXPECT_EQ(0, result.solution.variables("x").values(2));
    EXPECT_EQ(0, result.solution.variables("y").values(0));
    EXPECT_EQ(1, result.solution.variables("y").values(1));
    EXPECT_EQ(16, result.solution.objective());

    ASSERT_THROW(solver::solve(&model, option), std::logic_error);
}
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/