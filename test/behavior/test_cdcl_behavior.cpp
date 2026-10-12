/*****************************************************************************/
// Copyright (c) 2020-2026 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
using namespace printemps;
using namespace printemps::solver::cdcl::core;

/*****************************************************************************/
class TestCDCLBehavior : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown(void) {
        /// nothing to do
    }
};

/*****************************************************************************/
// Helper: MatrixModel から CDCL を実行して TerminationStatus を返す
static CDCLCoreTerminationStatus solve_from_matrix_model(
    matrix_model::MatrixModel& a_matrix_model) {
    option::Option option;
    option.cdcl.time_max      = 10.0;
    option.cdcl.iteration_max = 10000;
    option.output.verbose     = option::verbose::Off;

    CDCLCore solver(&a_matrix_model, std::nullopt, option);
    solver.run();
    return solver.result().termination_status;
}

/*****************************************************************************/
// Helper: 単純な sum x_i >= rhs インスタンスを作成
static matrix_model::MatrixModel make_greater_equal_instance(
    const int    a_NUMBER_OF_VARIABLES,  //
    const double a_RIGHT_HAND_SIDE,      //
    const bool   a_IS_EQUALITY = false) {
    matrix_model::MatrixModel matrix_model;
    matrix_model.number_of_variables   = a_NUMBER_OF_VARIABLES;
    matrix_model.number_of_constraints = 1;

    matrix_model.constraint_coefficients.row_ptrs.resize(2);
    matrix_model.constraint_coefficients.row_ptrs[0] = 0;
    matrix_model.constraint_coefficients.row_ptrs[1] = a_NUMBER_OF_VARIABLES;
    for (int i = 0; i < a_NUMBER_OF_VARIABLES; i++) {
        matrix_model.constraint_coefficients.column_indices.push_back(i);
        matrix_model.constraint_coefficients.values.push_back(1.0);
    }

    matrix_model.constraint_lower_bounds.values.push_back(a_RIGHT_HAND_SIDE);
    matrix_model.constraint_upper_bounds.values.push_back(
        static_cast<double>(a_NUMBER_OF_VARIABLES));

    if (a_IS_EQUALITY) {
        matrix_model.equal_constraint_index_range   = {0, 1};
        matrix_model.less_constraint_index_range    = {0, 0};
        matrix_model.greater_constraint_index_range = {1, 1};
    } else {
        matrix_model.less_constraint_index_range    = {0, 0};
        matrix_model.equal_constraint_index_range   = {0, 0};
        matrix_model.greater_constraint_index_range = {0, 1};
    }

    for (int i = 0; i < a_NUMBER_OF_VARIABLES; i++) {
        matrix_model.variable_lower_bounds.values.push_back(0.0);
        matrix_model.variable_upper_bounds.values.push_back(1.0);
    }
    matrix_model.is_minimization = true;
    return matrix_model;
}

/*****************************************************************************/
TEST_F(TestCDCLBehavior, float_constraint_normalization) {
    // Test 0.5 x0 + 0.25 x1 <= 0.75 -> scaled to 2 x0 + 1 x1 <= 3
    matrix_model::MatrixModel matrix_model;
    matrix_model.number_of_constraints                  = 1;
    matrix_model.number_of_variables                    = 2;
    matrix_model.constraint_coefficients.row_ptrs       = {0, 2};
    matrix_model.constraint_coefficients.column_indices = {0, 1};
    matrix_model.constraint_coefficients.values         = {0.5, 0.25};
    matrix_model.constraint_lower_bounds.values         = {
        std::numeric_limits<double>::lowest()};
    matrix_model.constraint_upper_bounds.values = {0.75};
    matrix_model.less_constraint_index_range    = {0, 1};
    matrix_model.equal_constraint_index_range   = {0, 0};
    matrix_model.greater_constraint_index_range = {0, 0};
    for (int i = 0; i < 2; i++) {
        matrix_model.variable_lower_bounds.values.push_back(0.0);
        matrix_model.variable_upper_bounds.values.push_back(1.0);
    }
    matrix_model.is_minimization = true;

    EXPECT_EQ(solve_from_matrix_model(matrix_model),
              CDCLCoreTerminationStatus::FEASIBLE);
}

/*****************************************************************************/
TEST_F(TestCDCLBehavior, pseudo_boolean_constraint) {
    // 2*x0 + 3*x1 + x2 >= 4: feasible
    matrix_model::MatrixModel matrix_model;
    matrix_model.number_of_variables   = 3;
    matrix_model.number_of_constraints = 1;

    matrix_model.constraint_coefficients.row_ptrs       = {0, 3};
    matrix_model.constraint_coefficients.column_indices = {0, 1, 2};
    matrix_model.constraint_coefficients.values         = {2.0, 3.0, 1.0};
    matrix_model.constraint_lower_bounds.values.push_back(4.0);
    matrix_model.constraint_upper_bounds.values.push_back(6.0);

    matrix_model.less_constraint_index_range    = {0, 0};
    matrix_model.equal_constraint_index_range   = {0, 0};
    matrix_model.greater_constraint_index_range = {0, 1};

    for (int i = 0; i < 3; i++) {
        matrix_model.variable_lower_bounds.values.push_back(0.0);
        matrix_model.variable_upper_bounds.values.push_back(1.0);
    }
    matrix_model.is_minimization = true;

    option::Option option;
    option.cdcl.time_max      = 10.0;
    option.cdcl.iteration_max = 100000;
    option.output.verbose     = option::verbose::Off;

    CDCLCore solver(&matrix_model, std::nullopt, option);
    solver.run();

    ASSERT_EQ(solver.result().termination_status,
              CDCLCoreTerminationStatus::FEASIBLE);
    ASSERT_EQ(solver.result().solution.size(), 3u);

    // 2*x0 + 3*x1 + x2 >= 4 must hold
    const std::vector<int64_t> COEFFICIENTS   = {2, 3, 1};
    int64_t                    left_hand_side = 0;
    for (int i = 0; i < 3; i++) {
        left_hand_side += COEFFICIENTS[i] * solver.result().solution[i];
    }
    EXPECT_GE(left_hand_side, 4);
}

/*****************************************************************************/
TEST_F(TestCDCLBehavior, pseudo_boolean_infeasible) {
    // x0 + x1 >= 3: infeasible (only 2 variables)
    auto matrix_model = make_greater_equal_instance(2, 3.0);
    EXPECT_EQ(solve_from_matrix_model(matrix_model),
              CDCLCoreTerminationStatus::INFEASIBLE);
}

/*****************************************************************************/
TEST_F(TestCDCLBehavior, solve_feasible_problem) {
    // x0 + x1 + x2 >= 1: feasible
    auto matrix_model = make_greater_equal_instance(3, 1.0);
    EXPECT_EQ(solve_from_matrix_model(matrix_model),
              CDCLCoreTerminationStatus::FEASIBLE);
}

/*****************************************************************************/
TEST_F(TestCDCLBehavior, solve_infeasible_problem) {
    // x0 + x1 >= 3: infeasible
    auto matrix_model = make_greater_equal_instance(2, 3.0);
    EXPECT_EQ(solve_from_matrix_model(matrix_model),
              CDCLCoreTerminationStatus::INFEASIBLE);
}

/*****************************************************************************/
TEST_F(TestCDCLBehavior, solve_statistics) {
    // Problem requiring decisions and propagations
    matrix_model::MatrixModel matrix_model =
        make_greater_equal_instance(5, 3.0);
    option::Option option;
    option.cdcl.time_max      = 10.0;
    option.cdcl.iteration_max = 10000;
    option.output.verbose     = option::verbose::Inner;

    CDCLCore solver(&matrix_model, std::nullopt, option);
    solver.run();

    const auto& RESULT = solver.result();
    EXPECT_EQ(RESULT.termination_status, CDCLCoreTerminationStatus::FEASIBLE);
    EXPECT_GT(RESULT.number_of_decisions + RESULT.number_of_propagations, 0);
}

/*****************************************************************************/
TEST_F(TestCDCLBehavior, solve_mps_instance) {
    std::vector<std::string> CANDIDATES = {
        "./air05.mps",
        "air05.mps",
        "../printemps_benchmark/instance/MIPLIB2017_Benchmark/bnatt400.mps",
        "/Volumes/Data/mps/bnatt400.mps",
        "../printemps_benchmark/instance/MIPLIB2017_Benchmark/ex10.mps",
        "/Volumes/Data/mps/ex10.mps",
        "scratch/ex9.mps",
        "/Volumes/Data/mps/ex9.mps"};

    std::string mps_path = "";
    for (const auto& path : CANDIDATES) {
        std::ifstream file_stream(path.c_str());
        if (file_stream.good()) {
            mps_path = path;
            break;
        }
    }

    if (mps_path.empty()) {
        GTEST_SKIP() << "No suitable MPS benchmark file found";
    }

    mps::MPS mps_data;
    mps_data.read_mps(mps_path);

    model::Model<int, double> model;
    model.mps_handler().import(mps_data, false);

    option::Option option;
    option.preprocess.is_enabled_presolve = true;
    option.output.verbose                 = option::verbose::Off;
    model.builder().build(option, false);

    auto matrix_model = model.matrix_model_handler().export_matrix_model();

    option::Option cdcl_option;
    cdcl_option.cdcl.time_max  = 15.0;
    cdcl_option.output.verbose = option::verbose::Off;

    CDCLCore solver(&matrix_model, std::nullopt, cdcl_option);
    solver.run();

    EXPECT_EQ(solver.result().termination_status,
              CDCLCoreTerminationStatus::FEASIBLE);
}

/*****************************************************************************/
TEST_F(TestCDCLBehavior, controller) {
    model::Model<int, double> model;
    auto&                     x = model.create_variables("x", 3, 0, 1);
    model.create_constraint("c1", x(0) + x(1) - x(2) == 0);
    model.create_constraint("c2", x(0) + x(2) == 1);

    option::Option option;
    option.preprocess.is_enabled_presolve = false;
    option.pdlp.is_enabled                = false;
    option.lagrange_dual.is_enabled       = false;
    option.local_search.is_enabled        = false;
    option.tabu_search.iteration_max      = 0;
    option.cdcl.is_enabled                = true;
    option.output.verbose                 = option::verbose::Off;

    solver::Solver<int, double> solver(&model, option);
    auto                        result = solver.solve();

    EXPECT_TRUE(result.solution.is_feasible());
    EXPECT_EQ(solver.cdcl_controller().result().core.termination_status,
              CDCLCoreTerminationStatus::FEASIBLE);
}

/*****************************************************************************/
TEST_F(TestCDCLBehavior, controller_parallel_portfolio) {
    model::Model<int, double> model;
    auto&                     x = model.create_variables("x", 4, 0, 1);
    model.create_constraint("c1", x(0) + x(1) + x(2) >= 2);
    model.create_constraint("c2", x(1) + x(2) + x(3) <= 1);
    model.create_constraint("c3", x(0) + x(3) == 1);

    option::Option option;
    option.preprocess.is_enabled_presolve           = false;
    option.pdlp.is_enabled                          = false;
    option.lagrange_dual.is_enabled                 = false;
    option.local_search.is_enabled                  = false;
    option.tabu_search.iteration_max                = 0;
    option.cdcl.is_enabled                          = true;
    option.parallel.is_enabled_cdcl_parallelization = true;
    option.parallel.number_of_threads_cdcl          = 4;
    option.output.verbose                           = option::verbose::Off;

    solver::Solver<int, double> solver(&model, option);
    auto                        result = solver.solve();

    EXPECT_TRUE(result.solution.is_feasible());
    EXPECT_EQ(solver.cdcl_controller().result().core.termination_status,
              CDCLCoreTerminationStatus::FEASIBLE);
}

/*****************************************************************************/
TEST_F(TestCDCLBehavior, best_intermediate_solution) {
    model::Model<int, double> model;
    auto&                     x = model.create_variables("x", 2, 0, 1);
    // Incompatible fractional constraints:
    // c1: 0.2*x0 + 0.3*x1 <= 0.1
    // c2: 0.5*x0 + 0.5*x1 >= 0.8
    model.create_constraint("c1", 0.2 * x(0) + 0.3 * x(1) <= 0.1);
    model.create_constraint("c2", 0.5 * x(0) + 0.5 * x(1) >= 0.8);

    option::Option option;
    option.preprocess.is_enabled_presolve          = false;
    option.pdlp.is_enabled                         = false;
    option.lagrange_dual.is_enabled                = false;
    option.local_search.is_enabled                 = false;
    option.tabu_search.iteration_max               = 0;
    option.cdcl.is_enabled                         = true;
    option.cdcl.is_enabled_evaluate_full_solutions = true;
    option.output.verbose                          = option::verbose::Off;

    solver::Solver<int, double> solver(&model, option);
    auto                        result = solver.solve();

    const auto& cdcl_result = solver.cdcl_controller().result().core;
    EXPECT_EQ(cdcl_result.termination_status,
              CDCLCoreTerminationStatus::INFEASIBLE);

    // best_solution must be tracked
    EXPECT_EQ(2u, cdcl_result.best_solution.size());
    EXPECT_TRUE(std::isfinite(cdcl_result.best_total_violation));

    // Verify evaluation conforms to original Model's floating-point scale:
    // For x=(0,0): c1 viol = 0, c2 viol = 0.8 - 0 = 0.8 (total = 0.8).
    // For x=(1,1): c1 viol = 0.5 - 0.1 = 0.4, c2 viol = 0 (total = 0.4).
    // For x=(1,0): c1 viol = 0.2 - 0.1 = 0.1, c2 viol = 0.8 - 0.5 = 0.3 (total
    // = 0.4). For x=(0,1): c1 viol = 0.3 - 0.1 = 0.2, c2 viol = 0.8 - 0.5 = 0.3
    // (total = 0.5). Best violation is 0.4 (NOT integerized 4).
    EXPECT_NEAR(0.4, cdcl_result.best_total_violation, 1e-6);

    // Incumbent holder must be updated with the best intermediate solution
    const auto& global_state = solver.global_state();
    const auto& augmented_score =
        global_state.incumbent_holder.global_augmented_incumbent_score();
    EXPECT_NEAR(0.4, augmented_score.total_violation, 1e-6);
}

/*****************************************************************************/
TEST_F(TestCDCLBehavior, full_solution_evaluation_disabled_by_default) {
    model::Model<int, double> model;
    auto&                     x = model.create_variables("x", 2, 0, 1);
    model.create_constraint("c1", 0.2 * x(0) + 0.3 * x(1) <= 0.1);
    model.create_constraint("c2", 0.5 * x(0) + 0.5 * x(1) >= 0.8);

    option::Option option;
    option.preprocess.is_enabled_presolve = false;
    option.pdlp.is_enabled                = false;
    option.lagrange_dual.is_enabled       = false;
    option.local_search.is_enabled        = false;
    option.tabu_search.iteration_max      = 0;
    option.cdcl.is_enabled                = true;
    // cdcl.is_enabled_evaluate_full_solutions is false by default
    option.output.verbose = option::verbose::Off;

    solver::Solver<int, double> solver(&model, option);
    auto                        result = solver.solve();

    const auto& cdcl_result = solver.cdcl_controller().result().core;
    EXPECT_EQ(cdcl_result.termination_status,
              CDCLCoreTerminationStatus::INFEASIBLE);

    // best_solution must NOT be tracked when disabled
    EXPECT_TRUE(cdcl_result.best_solution.empty());
    EXPECT_TRUE(std::isinf(cdcl_result.best_total_violation));

    // Incumbent holder violation should remain at the initial solution's
    // violation (x=(0,0) -> viol=0.8)
    const auto& global_state = solver.global_state();
    const auto& augmented_score =
        global_state.incumbent_holder.global_augmented_incumbent_score();
    EXPECT_NEAR(0.8, augmented_score.total_violation, 1e-6);
}

/*****************************************************************************/
TEST_F(TestCDCLBehavior, selection_constraint_satisfaction) {
    model::Model<int, double> model;
    auto&                     x = model.create_variables("x", 4, 0, 1);
    auto&                     y = model.create_variables("y", 3, 0, 1);

    // Two selection constraints
    model.create_constraint("sel_x", x.selection());
    model.create_constraint("sel_y", y.selection());

    // Conflicting constraint to ensure intermediate solutions are tested
    // 3*x(0) + 3*y(0) <= 2 and 3*x(0) + 3*y(0) >= 4 (infeasible)
    model.create_constraint("c1", 3 * x(0) + 3 * y(0) <= 2);
    model.create_constraint("c2", 3 * x(0) + 3 * y(0) >= 4);

    option::Option option;
    option.preprocess.is_enabled_presolve          = false;
    option.pdlp.is_enabled                         = false;
    option.lagrange_dual.is_enabled                = false;
    option.local_search.is_enabled                 = false;
    option.tabu_search.iteration_max               = 0;
    option.cdcl.is_enabled                         = true;
    option.cdcl.is_enabled_evaluate_full_solutions = true;
    option.output.verbose                          = option::verbose::Off;

    solver::Solver<int, double> solver(&model, option);
    auto                        result = solver.solve();

    const auto& cdcl_result = solver.cdcl_controller().result().core;
    EXPECT_EQ(cdcl_result.termination_status,
              CDCLCoreTerminationStatus::INFEASIBLE);

    // Best intermediate solution must be tracked
    EXPECT_EQ(7u, cdcl_result.best_solution.size());

    // Selection constraints must be strictly satisfied in best_solution:
    // sum(x_i) == 1
    int x_sum = cdcl_result.best_solution[0] + cdcl_result.best_solution[1] +
                cdcl_result.best_solution[2] + cdcl_result.best_solution[3];
    EXPECT_EQ(x_sum, 1);

    // sum(y_i) == 1
    int y_sum = cdcl_result.best_solution[4] + cdcl_result.best_solution[5] +
                cdcl_result.best_solution[6];
    EXPECT_EQ(y_sum, 1);

    // Verification must succeed without throw (a_IS_ENABLED_CORRECTION = false)
    EXPECT_NO_THROW(model.verifier().verify_and_correct(false, false));

    // Verify selected_variable_ptr is properly set for each selection
    for (const auto& selection : model.selections()) {
        EXPECT_NE(selection.selected_variable_ptr, nullptr);
        EXPECT_EQ(selection.selected_variable_ptr->value(), 1);
    }
}

/*****************************************************************************/
TEST_F(TestCDCLBehavior, simple_feasible) {
    // Simple 2-variable problem: x0 + x1 >= 1
    matrix_model::MatrixModel matrix_model;
    matrix_model.number_of_variables   = 2;
    matrix_model.number_of_constraints = 1;

    matrix_model.constraint_coefficients.row_ptrs       = {0, 2};
    matrix_model.constraint_coefficients.column_indices = {0, 1};
    matrix_model.constraint_coefficients.values         = {1.0, 1.0};
    matrix_model.constraint_lower_bounds.values.push_back(1.0);
    matrix_model.constraint_upper_bounds.values.push_back(2.0);

    matrix_model.less_constraint_index_range    = {0, 0};
    matrix_model.equal_constraint_index_range   = {0, 0};
    matrix_model.greater_constraint_index_range = {0, 1};

    for (int i = 0; i < 2; i++) {
        matrix_model.variable_lower_bounds.values.push_back(0.0);
        matrix_model.variable_upper_bounds.values.push_back(1.0);
    }
    matrix_model.is_minimization = true;

    option::Option option;
    option.cdcl.time_max      = 10.0;
    option.cdcl.iteration_max = 10000;
    option.output.verbose     = option::verbose::Off;

    CDCLCore solver(&matrix_model, std::nullopt, option);
    solver.run();

    EXPECT_EQ(solver.result().termination_status,
              CDCLCoreTerminationStatus::FEASIBLE);
    EXPECT_EQ(solver.result().solution.size(), 2u);
    EXPECT_GE(solver.result().solution[0] + solver.result().solution[1], 1);
}

/*****************************************************************************/
TEST_F(TestCDCLBehavior, variable_activity_feedback) {
    /**
     * Problem with conflicts to trigger conflict analysis and variable activity
     * feedback: x0 + x1 + x2 + x3 >= 3 x0 + x1 <= 1 x2 + x3 <= 1
     *
     * Infeasible problem that requires branching and learning.
     */
    matrix_model::MatrixModel matrix_model;
    matrix_model.number_of_variables   = 4;
    matrix_model.number_of_constraints = 3;

    // Row 0: x0 + x1 + x2 + x3 >= 3
    // Row 1: x0 + x1 <= 1
    // Row 2: x2 + x3 <= 1
    matrix_model.constraint_coefficients.row_ptrs       = {0, 4, 6, 8};
    matrix_model.constraint_coefficients.column_indices = {0, 1, 2, 3,
                                                           0, 1, 2, 3};
    matrix_model.constraint_coefficients.values         = {1.0, 1.0, 1.0, 1.0,
                                                           1.0, 1.0, 1.0, 1.0};

    matrix_model.constraint_lower_bounds.values = {3.0, 0.0, 0.0};
    matrix_model.constraint_upper_bounds.values = {4.0, 1.0, 1.0};

    matrix_model.less_constraint_index_range    = {1, 3};
    matrix_model.equal_constraint_index_range   = {0, 0};
    matrix_model.greater_constraint_index_range = {0, 1};

    for (int i = 0; i < 4; i++) {
        matrix_model.variable_lower_bounds.values.push_back(0.0);
        matrix_model.variable_upper_bounds.values.push_back(1.0);
    }
    matrix_model.is_minimization = true;

    // Test with variable activity feedback (rate = 0.8)
    {
        option::Option option;
        option.cdcl.time_max                        = 10.0;
        option.cdcl.iteration_max                   = 1000;
        option.cdcl.variable_activity_feedback_rate = 0.8;
        option.output.verbose                       = option::verbose::Off;

        CDCLCore solver(&matrix_model, std::nullopt, option);
        solver.run();

        EXPECT_EQ(solver.result().termination_status,
                  CDCLCoreTerminationStatus::INFEASIBLE);
        EXPECT_GT(solver.result().number_of_conflicts, 0);
    }

    // Test with no feedback (rate = 0.0)
    {
        option::Option option;
        option.cdcl.time_max                        = 10.0;
        option.cdcl.iteration_max                   = 1000;
        option.cdcl.variable_activity_feedback_rate = 0.0;
        option.output.verbose                       = option::verbose::Off;

        CDCLCore solver(&matrix_model, std::nullopt, option);
        solver.run();

        EXPECT_EQ(solver.result().termination_status,
                  CDCLCoreTerminationStatus::INFEASIBLE);
        EXPECT_GT(solver.result().number_of_conflicts, 0);
    }

    // Test with custom feedback rate (0.5)
    {
        option::Option option;
        option.cdcl.time_max                        = 10.0;
        option.cdcl.iteration_max                   = 1000;
        option.cdcl.variable_activity_feedback_rate = 0.5;
        option.output.verbose                       = option::verbose::Off;

        CDCLCore solver(&matrix_model, std::nullopt, option);
        solver.run();

        EXPECT_EQ(solver.result().termination_status,
                  CDCLCoreTerminationStatus::INFEASIBLE);
        EXPECT_GT(solver.result().number_of_conflicts, 0);
    }
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/