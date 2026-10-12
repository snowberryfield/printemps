/*****************************************************************************/
// Copyright (c) 2020-2026 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
using namespace printemps;

/*****************************************************************************/
class TestInfeasibilityBehavior : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown(void) {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestInfeasibilityBehavior, preprocess_infeasible) {
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
TEST_F(TestInfeasibilityBehavior, pdlp_infeasible) {
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
    option.output.verbose                 = option::verbose::Off;
    option.preprocess.is_enabled_presolve = false;
    option.pdlp.is_enabled                = true;
    option.cdcl.is_enabled                = false;

    solver::IPSolver solver(&model, option);
    const auto       RESULT = solver.solve();

    EXPECT_EQ(RESULT.status.termination_status,
              solver::TerminationStatus::INFEASIBLE);
    EXPECT_TRUE(RESULT.status.is_proven_infeasible());
    EXPECT_FALSE(RESULT.status.is_found_feasible_solution);
}

/*****************************************************************************/
TEST_F(TestInfeasibilityBehavior, cdcl_infeasible) {
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
    option.output.verbose                 = option::verbose::Off;
    option.preprocess.is_enabled_presolve = false;
    option.pdlp.is_enabled                = false;
    option.cdcl.is_enabled                = true;

    solver::IPSolver solver(&model, option);
    const auto       RESULT = solver.solve();

    EXPECT_EQ(RESULT.status.termination_status,
              solver::TerminationStatus::INFEASIBLE);
    EXPECT_TRUE(RESULT.status.is_proven_infeasible());
    EXPECT_FALSE(RESULT.status.is_found_feasible_solution);
}

/*****************************************************************************/
TEST_F(TestInfeasibilityBehavior, feasible_solution_status) {
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
              solver::TerminationStatus::REACH_TARGET);
    EXPECT_FALSE(RESULT.status.is_proven_infeasible());
    EXPECT_TRUE(RESULT.status.is_found_feasible_solution);
    EXPECT_EQ(RESULT.solution.termination_status(), "REACH_TARGET");
}

/*****************************************************************************/
TEST_F(TestInfeasibilityBehavior, json_serialization) {
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

    EXPECT_TRUE(RESULT.solution.is_proven_infeasible());
    EXPECT_EQ(RESULT.solution.termination_status(), "INFEASIBLE");

    const std::string JSON_FILE = "test_named_solution.json";
    RESULT.solution.write_json_by_name(JSON_FILE);
    auto sol_json = utility::json::read_json_object(JSON_FILE);
    EXPECT_EQ(sol_json.get<std::string>("termination_status"), "INFEASIBLE");
    EXPECT_FALSE(sol_json.get<bool>("is_found_feasible_solution"));
    EXPECT_EQ(sol_json.at("objective").type(), typeid(std::nullptr_t));
    EXPECT_EQ(sol_json.at("total_violation").type(), typeid(std::nullptr_t));
    EXPECT_EQ(sol_json.at("variables").type(), typeid(std::nullptr_t));
    EXPECT_EQ(sol_json.at("expressions").type(), typeid(std::nullptr_t));
    EXPECT_EQ(sol_json.at("constraints").type(), typeid(std::nullptr_t));
    EXPECT_EQ(sol_json.at("violations").type(), typeid(std::nullptr_t));
    std::remove(JSON_FILE.c_str());

    const std::string SOL_FILE = "test_named_solution.sol";
    RESULT.solution.write_solution(SOL_FILE);
    std::ifstream sol_ifs(SOL_FILE);
    std::string   first_line;
    std::getline(sol_ifs, first_line);
    EXPECT_EQ(first_line, "=infeasible=");
    std::string second_line;
    EXPECT_FALSE(std::getline(sol_ifs, second_line));
    sol_ifs.close();
    std::remove(SOL_FILE.c_str());
}

/*****************************************************************************/
TEST_F(TestInfeasibilityBehavior, cdcl_seeded_clauses) {
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
    option.output.verbose                 = option::verbose::Off;
    option.preprocess.is_enabled_presolve = false;
    // option.cdcl.is_enabled_seed_clauses_from_partial_feasible_enumeration =
    // true;

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

/*****************************************************************************/
TEST_F(TestInfeasibilityBehavior, preprocess_trivial_constraint_infeasible) {
    /**
     * Trivial unsatisfiable constraint:
     * x0, x1 >= 0, x0 + x1 <= -1 (impossible since min is 0 > -1)
     */
    model::IPModel model;
    auto&          x = model.create_variables("x", 2, 0, 10);
    auto&          c = model.create_constraint("c");
    c                = (x(0) + x(1) <= -1);

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
TEST_F(TestInfeasibilityBehavior, preprocess_trivial_equality_infeasible) {
    /**
     * Trivial unsatisfiable equality constraint:
     * x0 in [0, 5], x0 == 10
     */
    model::IPModel model;
    auto&          x = model.create_variable("x", 0, 5);
    auto&          c = model.create_constraint("c");
    c                = (x == 10);

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
TEST_F(TestInfeasibilityBehavior, standalone_infeasible_solution_output) {
    const std::string OPB_FILE = "test_infeasible_standalone.opb";
    std::ofstream     ofs(OPB_FILE);
    ofs << "* #variable= 1 #constraint= 2\n";
    ofs << "1 x1 >= 2 ;\n";
    ofs << "1 x1 <= 0 ;\n";
    ofs.close();

    std::vector<const char*> argv = {"printemps", OPB_FILE.c_str(), "-v",
                                     "Off"};
    standalone::Standalone   standalone(argv.size(), argv.data());
    standalone.run();

    EXPECT_TRUE(std::ifstream("incumbent.json").good());
    EXPECT_TRUE(std::ifstream("incumbent.sol").good());
    EXPECT_TRUE(std::ifstream("status.json").good());

    // Check incumbent.sol has only "=infeasible="
    std::ifstream sol_ifs("incumbent.sol");
    std::string   sol_line;
    std::getline(sol_ifs, sol_line);
    EXPECT_EQ(sol_line, "=infeasible=");
    std::string sol_next_line;
    EXPECT_FALSE(std::getline(sol_ifs, sol_next_line));
    sol_ifs.close();

    // Check incumbent.json has null fields and INFEASIBLE termination_status
    auto json = utility::json::read_json_object("incumbent.json");
    EXPECT_EQ(json.get<std::string>("termination_status"), "INFEASIBLE");
    EXPECT_FALSE(json.get<bool>("is_found_feasible_solution"));
    EXPECT_EQ(json.at("objective").type(), typeid(std::nullptr_t));
    EXPECT_EQ(json.at("total_violation").type(), typeid(std::nullptr_t));
    EXPECT_EQ(json.at("variables").type(), typeid(std::nullptr_t));
    EXPECT_EQ(json.at("expressions").type(), typeid(std::nullptr_t));
    EXPECT_EQ(json.at("constraints").type(), typeid(std::nullptr_t));
    EXPECT_EQ(json.at("violations").type(), typeid(std::nullptr_t));

    std::remove(OPB_FILE.c_str());
    std::remove("status.json");
    std::remove("incumbent.json");
    std::remove("incumbent.sol");
}

/*****************************************************************************/
TEST_F(TestInfeasibilityBehavior, standalone_feasible_solution_output) {
    const std::string OPB_FILE = "test_feasible_standalone.opb";
    std::ofstream     ofs(OPB_FILE);
    ofs << "* #variable= 1 #constraint= 1\n";
    ofs << "1 x1 >= 0 ;\n";
    ofs.close();

    std::vector<const char*> argv = {"printemps", OPB_FILE.c_str(), "-v",
                                     "Off"};
    standalone::Standalone   standalone(argv.size(), argv.data());
    standalone.run();

    EXPECT_TRUE(std::ifstream("incumbent.json").good());
    EXPECT_TRUE(std::ifstream("incumbent.sol").good());
    EXPECT_TRUE(std::ifstream("status.json").good());

    // Check incumbent.sol starts with =obj=
    std::ifstream sol_ifs("incumbent.sol");
    std::string   sol_line;
    std::getline(sol_ifs, sol_line);
    EXPECT_EQ(sol_line.rfind("=obj=", 0), 0UL);
    sol_ifs.close();

    // Check incumbent.json has OPTIMAL termination_status and valid fields
    auto json = utility::json::read_json_object("incumbent.json");
    EXPECT_EQ(json.get<std::string>("termination_status"), "OPTIMAL");
    EXPECT_TRUE(json.get<bool>("is_found_feasible_solution"));
    EXPECT_NE(json.at("objective").type(), typeid(std::nullptr_t));
    EXPECT_NE(json.at("total_violation").type(), typeid(std::nullptr_t));
    EXPECT_NE(json.at("variables").type(), typeid(std::nullptr_t));

    std::remove(OPB_FILE.c_str());
    std::remove("status.json");
    std::remove("incumbent.json");
    std::remove("incumbent.sol");
}

/*****************************************************************************/
TEST_F(TestInfeasibilityBehavior, reach_target_termination_status) {
    model::IPModel model;
    auto&          x = model.create_variables("x", 2, 0, 1);
    auto&          c = model.create_constraint("c");
    c                = (x(0) + x(1) <= 1);
    model.minimize(x(0) + x(1));

    option::Option option;
    option.output.verbose                 = option::verbose::Off;
    option.general.target_objective_value = 0.0;

    solver::IPSolver solver(&model, option);
    const auto       RESULT = solver.solve();

    EXPECT_EQ(RESULT.status.termination_status,
              solver::TerminationStatus::REACH_TARGET);
    EXPECT_TRUE(RESULT.status.is_found_feasible_solution);
    EXPECT_EQ(RESULT.solution.termination_status(), "REACH_TARGET");
}

/*****************************************************************************/
TEST_F(TestInfeasibilityBehavior, no_feasible_solution_found_iteration_over) {
    model::IPModel model;
    auto&          x  = model.create_variables("x", 2, 0, 1);
    auto&          c1 = model.create_constraint("c1");
    auto&          c2 = model.create_constraint("c2");
    c1                = (x(0) + x(1) >= 2);
    c2                = (x(0) + x(1) <= 1);

    option::Option option;
    option.output.verbose                 = option::verbose::Off;
    option.preprocess.is_enabled_presolve = false;
    option.cdcl.is_enabled                = false;
    option.lagrange_dual.is_enabled       = false;
    option.local_search.is_enabled        = false;
    option.general.iteration_max          = 0;

    solver::IPSolver solver(&model, option);
    const auto       RESULT = solver.solve();

    EXPECT_EQ(RESULT.status.termination_status,
              solver::TerminationStatus::ITERATION_OVER);
    EXPECT_FALSE(RESULT.status.is_found_feasible_solution);
    EXPECT_FALSE(RESULT.status.is_proven_infeasible());
    EXPECT_EQ(RESULT.solution.termination_status(), "ITERATION_OVER");
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/