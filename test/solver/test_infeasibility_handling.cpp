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
class TestInfeasibilityHandling : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown(void) {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestInfeasibilityHandling, preprocess_infeasible) {
    /**
     * Contradictory cycle on binary variables:
     * x0 == x1 == x2 == x3 == x4 == 1 - x0
     * which implies x0 == 1 - x0 (no binary solution).
     */
    model::IPModel model;
    auto&          x = model.create_variables("x", 5, 0, 1);
    auto&          f = model.create_constraints("f", 5);

    f(0) = (x(0) == x(1));
    f(1) = (x(1) == x(2));
    f(2) = (x(2) == x(3));
    f(3) = (x(3) == x(4));
    f(4) = (x(4) == 1 - x(0));

    option::Option option;
    option.output.verbose = option::verbose::Off;

    solver::IPSolver solver(&model, option);
    const auto       RESULT = solver.solve();

    EXPECT_EQ(RESULT.status.termination_status,
              solver::TerminationStatus::INFEASIBLE);
    EXPECT_TRUE(RESULT.status.is_proven_infeasible());
    EXPECT_FALSE(RESULT.status.is_found_feasible_solution);
}

/*****************************************************************************/
TEST_F(TestInfeasibilityHandling, pdlp_infeasible) {
    /**
     * LP relaxation is infeasible:
     * x >= 0, y >= 0
     * x + y <= 1
     * x + y >= 2
     */
    model::IPModel model;
    auto&          x  = model.create_variables("x", 2, 0, 10);
    auto&          c0 = model.create_constraint("c0");
    auto&          c1 = model.create_constraint("c1");
    c0                = (x(0) + x(1) <= 1);
    c1                = (x(0) + x(1) >= 2);

    option::Option option;
    option.output.verbose                = option::verbose::Off;
    option.preprocess.is_enabled_presolve = false;
    option.pdlp.is_enabled               = true;
    option.cdcl.is_enabled               = false;

    solver::IPSolver solver(&model, option);
    const auto       RESULT = solver.solve();

    EXPECT_EQ(RESULT.status.termination_status,
              solver::TerminationStatus::INFEASIBLE);
    EXPECT_TRUE(RESULT.status.is_proven_infeasible());
    EXPECT_FALSE(RESULT.status.is_found_feasible_solution);
}

/*****************************************************************************/
TEST_F(TestInfeasibilityHandling, cdcl_infeasible) {
    /**
     * Binary infeasible problem:
     * x0 + x1 == 1 and x0 - x1 == 0 (both in {0, 1})
     */
    model::IPModel model;
    auto&          x  = model.create_variables("x", 2, 0, 1);
    auto&          c0 = model.create_constraint("c0");
    auto&          c1 = model.create_constraint("c1");
    c0                = (x(0) + x(1) == 1);
    c1                = (x(0) == x(1));

    option::Option option;
    option.output.verbose                = option::verbose::Off;
    option.preprocess.is_enabled_presolve = false;
    option.pdlp.is_enabled               = false;
    option.cdcl.is_enabled               = true;

    solver::IPSolver solver(&model, option);
    const auto       RESULT = solver.solve();

    EXPECT_EQ(RESULT.status.termination_status,
              solver::TerminationStatus::INFEASIBLE);
    EXPECT_TRUE(RESULT.status.is_proven_infeasible());
    EXPECT_FALSE(RESULT.status.is_found_feasible_solution);
}

/*****************************************************************************/
TEST_F(TestInfeasibilityHandling, feasible_solution_status) {
    /**
     * Trivial feasible problem:
     * x0 + x1 <= 1
     */
    model::IPModel model;
    auto&          x = model.create_variables("x", 2, 0, 1);
    auto&          c = model.create_constraint("c");
    c                = (x(0) + x(1) <= 1);

    option::Option option;
    option.output.verbose = option::verbose::Off;

    solver::IPSolver solver(&model, option);
    const auto       RESULT = solver.solve();

    EXPECT_EQ(RESULT.status.termination_status,
              solver::TerminationStatus::FEASIBLE);
    EXPECT_FALSE(RESULT.status.is_proven_infeasible());
    EXPECT_TRUE(RESULT.status.is_found_feasible_solution);
}

/*****************************************************************************/
TEST_F(TestInfeasibilityHandling, json_serialization) {
    model::IPModel model;
    auto&          x = model.create_variables("x", 5, 0, 1);
    auto&          f = model.create_constraints("f", 5);

    f(0) = (x(0) == x(1));
    f(1) = (x(1) == x(2));
    f(2) = (x(2) == x(3));
    f(3) = (x(3) == x(4));
    f(4) = (x(4) == 1 - x(0));

    option::Option option;
    option.output.verbose = option::verbose::Off;

    solver::IPSolver solver(&model, option);
    const auto       RESULT = solver.solve();

    utility::json::JsonObject json_object;
    RESULT.status.add_summary_json(&json_object);

    EXPECT_EQ(std::any_cast<std::string>(json_object.at("termination_status")),
              "INFEASIBLE");
}

/*****************************************************************************/
TEST_F(TestInfeasibilityHandling, cdcl_seeded_clauses) {
    /**
     * Problem with a small constraint group:
     * x0, x1, x2, x3 in {0, 1}
     * f0: x0 + x1 - x2 - x3 == 0
     * f1: x0 + x1 + x2 + x3 == 2
     * Feasible solutions for (x0, x1, x2, x3):
     * x0+x1 = 1 and x2+x3 = 1
     * Solutions: (1,0,1,0), (1,0,0,1), (0,1,1,0), (0,1,0,1) - 4 solutions.
     * No variable is implicitly fixed because each variable can be 0 or 1.
     * Implications/clauses extracted:
     * x0 + x1 <= 1, x0 + x1 >= 1 (x0 != x1)
     * x2 + x3 <= 1, x2 + x3 >= 1 (x2 != x3)
     */
    model::IPModel model;
    auto&          x = model.create_variables("x", 4, 0, 1);
    auto&          f = model.create_constraints("f", 2);
    f(0)             = (x(0) + x(1) - x(2) - x(3) == 0);
    f(1)             = (x(0) + x(1) + x(2) + x(3) == 2);

    option::Option option;
    option.output.verbose                = option::verbose::Off;
    option.preprocess.is_enabled_presolve = false;
    // option.cdcl.is_enabled_seed_clauses_from_partial_feasible_enumeration = true;

    model.build(option, false);
    auto matrix_model = model.matrix_model_handler().export_matrix_model();

    // Verify CDCL solves it and finds feasible solution
    solver::cdcl::core::CDCLCore cdcl_core(&matrix_model, std::nullopt, option);
    cdcl_core.run();
    EXPECT_EQ(solver::cdcl::core::CDCLCoreTerminationStatus::FEASIBLE,
              cdcl_core.result().termination_status);

    const auto& SOLUTION = cdcl_core.result().solution;
    const int   X0_VALUE =
        SOLUTION[matrix_model.variable_index_map.forward_at(&x(0))];
    const int X1_VALUE =
        SOLUTION[matrix_model.variable_index_map.forward_at(&x(1))];
    const int X2_VALUE =
        SOLUTION[matrix_model.variable_index_map.forward_at(&x(2))];
    const int X3_VALUE =
        SOLUTION[matrix_model.variable_index_map.forward_at(&x(3))];

    EXPECT_EQ(1, X0_VALUE + X1_VALUE);
    EXPECT_EQ(1, X2_VALUE + X3_VALUE);
}

}  // namespace
