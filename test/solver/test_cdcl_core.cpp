/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
using namespace printemps;
using namespace printemps::solver::cdcl::core;

/*****************************************************************************/
// Helper: MatrixModel からPB制約を持つCDCLCoreを構築して解く
static CDCLCoreTerminationStatus solve_from_matrix_model(
    matrix_model::MatrixModel& matrix_model, double time_max = 30.0,
    int iteration_max = 1000000) {
    option::Option opt;
    opt.cdcl.time_max      = time_max;
    opt.cdcl.iteration_max = iteration_max;
    opt.cdcl.log_interval  = 1000;
    opt.output.verbose     = option::verbose::Off;

    CDCLCore solver(&matrix_model, std::nullopt, opt);
    solver.run();
    return solver.result().termination_status;
}

/*****************************************************************************/
TEST(TestCDCLCore, float_constraint_normalization_test) {
    // Test 0.5 x0 + 0.25 x1 <= 0.75 -> scaled to 2 x0 + 1 x1 <= 3
    matrix_model::MatrixModel m;
    m.number_of_constraints                  = 1;
    m.number_of_variables                    = 2;
    m.constraint_coefficients.row_ptrs       = {0, 2};
    m.constraint_coefficients.column_indices = {0, 1};
    m.constraint_coefficients.values         = {0.5, 0.25};
    m.constraint_lower_bounds.values = {std::numeric_limits<double>::lowest()};
    m.constraint_upper_bounds.values = {0.75};
    m.less_constraint_index_range    = {0, 1};
    m.equal_constraint_index_range   = {0, 0};
    m.greater_constraint_index_range = {0, 0};
    for (int i = 0; i < 2; i++) {
        m.variable_lower_bounds.values.push_back(0.0);
        m.variable_upper_bounds.values.push_back(1.0);
    }
    m.is_minimization = true;

    EXPECT_EQ(solve_from_matrix_model(m), CDCLCoreTerminationStatus::FEASIBLE);
}

/*****************************************************************************/
// Helper: 単純な sum x_i >= rhs インスタンスを作成
static matrix_model::MatrixModel make_ge_instance(int N, double rhs,
                                                  bool is_equality = false) {
    matrix_model::MatrixModel m;
    m.number_of_variables   = N;
    m.number_of_constraints = 1;

    m.constraint_coefficients.row_ptrs.resize(2);
    m.constraint_coefficients.row_ptrs[0] = 0;
    m.constraint_coefficients.row_ptrs[1] = N;
    for (int i = 0; i < N; i++) {
        m.constraint_coefficients.column_indices.push_back(i);
        m.constraint_coefficients.values.push_back(1.0);
    }

    m.constraint_lower_bounds.values.push_back(rhs);
    m.constraint_upper_bounds.values.push_back(static_cast<double>(N));

    if (is_equality) {
        m.equal_constraint_index_range   = {0, 1};
        m.less_constraint_index_range    = {0, 0};
        m.greater_constraint_index_range = {1, 1};
    } else {
        m.less_constraint_index_range    = {0, 0};
        m.equal_constraint_index_range   = {0, 0};
        m.greater_constraint_index_range = {0, 1};
    }

    for (int i = 0; i < N; i++) {
        m.variable_lower_bounds.values.push_back(0.0);
        m.variable_upper_bounds.values.push_back(1.0);
    }
    m.is_minimization = true;
    return m;
}

/*****************************************************************************/
TEST(TestCDCLCore, literal_test) {
    // x0+x1+x2 >= 2: feasible
    auto m = make_ge_instance(3, 2.0);
    EXPECT_EQ(solve_from_matrix_model(m), CDCLCoreTerminationStatus::FEASIBLE);
}

/*****************************************************************************/
TEST(TestCDCLCore, pb_constraint_test) {
    // 2*x0 + 3*x1 + x2 >= 4: feasible
    matrix_model::MatrixModel m;
    m.number_of_variables   = 3;
    m.number_of_constraints = 1;

    m.constraint_coefficients.row_ptrs       = {0, 3};
    m.constraint_coefficients.column_indices = {0, 1, 2};
    m.constraint_coefficients.values         = {2.0, 3.0, 1.0};
    m.constraint_lower_bounds.values.push_back(4.0);
    m.constraint_upper_bounds.values.push_back(6.0);

    m.less_constraint_index_range    = {0, 0};
    m.equal_constraint_index_range   = {0, 0};
    m.greater_constraint_index_range = {0, 1};

    for (int i = 0; i < 3; i++) {
        m.variable_lower_bounds.values.push_back(0.0);
        m.variable_upper_bounds.values.push_back(1.0);
    }
    m.is_minimization = true;

    option::Option opt;
    opt.cdcl.time_max      = 10.0;
    opt.cdcl.iteration_max = 100000;
    opt.output.verbose     = option::verbose::Off;

    CDCLCore solver(&m, std::nullopt, opt);
    solver.run();

    ASSERT_EQ(solver.result().termination_status,
              CDCLCoreTerminationStatus::FEASIBLE);
    ASSERT_EQ(solver.result().solution.size(), 3u);

    // 2*x0 + 3*x1 + x2 >= 4 must hold
    const std::vector<int64_t> coeffs = {2, 3, 1};
    int64_t                    lhs    = 0;
    for (int i = 0; i < 3; i++)
        lhs += coeffs[i] * solver.result().solution[i];
    EXPECT_GE(lhs, 4);
}

/*****************************************************************************/
TEST(TestCDCLCore, pb_infeasible_test) {
    // x0 + x1 >= 3: infeasible (only 2 variables)
    auto m = make_ge_instance(2, 3.0);
    EXPECT_EQ(solve_from_matrix_model(m),
              CDCLCoreTerminationStatus::INFEASIBLE);
}

/*****************************************************************************/
TEST(TestCDCLCore, cdcl_solve_feasible_problem) {
    // x0 + x1 + x2 >= 1: feasible
    auto m = make_ge_instance(3, 1.0);
    EXPECT_EQ(solve_from_matrix_model(m), CDCLCoreTerminationStatus::FEASIBLE);
}

/*****************************************************************************/
TEST(TestCDCLCore, cdcl_solve_infeasible_problem) {
    // x0 + x1 >= 3: infeasible
    auto m = make_ge_instance(2, 3.0);
    EXPECT_EQ(solve_from_matrix_model(m),
              CDCLCoreTerminationStatus::INFEASIBLE);
}

/*****************************************************************************/
TEST(TestCDCLCore, cdcl_state_and_statistics_test) {
    CDCLCoreState state;
    EXPECT_EQ(state.number_of_assigned_zeros, 0);
    EXPECT_EQ(state.number_of_assigned_ones, 0);
    EXPECT_EQ(state.total_learned_count, 0);
    EXPECT_EQ(state.total_learned_literal_block_distance, 0);
    EXPECT_EQ(state.total_learned_length, 0);
    EXPECT_EQ(state.last_literal_block_distance, 0);
    EXPECT_EQ(state.last_learned_length, 0);
    EXPECT_EQ(state.number_of_restarts, 0);
    EXPECT_EQ(state.number_of_decisions, 0);
    EXPECT_EQ(state.number_of_propagations, 0);

    matrix_model::MatrixModel m = make_ge_instance(4, 2.0);
    option::Option            opt;
    CDCLCoreStateManager      manager(&m, opt);

    EXPECT_EQ(manager.number_of_assigned_zeros(), 0);
    EXPECT_EQ(manager.number_of_assigned_ones(), 0);
    EXPECT_EQ(manager.number_of_assigned_variables(), 0);

    // Test on_variable_assigned
    manager.on_variable_assigned(1);
    EXPECT_EQ(manager.number_of_assigned_ones(), 1);
    EXPECT_EQ(manager.number_of_assigned_zeros(), 0);

    manager.on_variable_assigned(0);
    EXPECT_EQ(manager.number_of_assigned_ones(), 1);
    EXPECT_EQ(manager.number_of_assigned_zeros(), 1);

    // Test on_learned_constraint_added
    CardinalityConstraint c(0, 1, {0, 1}, {2});
    c.literal_block_distance = 2;
    manager.on_learned_constraint_added(c);

    EXPECT_EQ(manager.last_literal_block_distance(), 2);
    EXPECT_EQ(manager.last_learned_length(), 3);
    EXPECT_EQ(manager.total_learned_count(), 1);
    EXPECT_DOUBLE_EQ(manager.average_learned_literal_block_distance(), 2.0);
    EXPECT_DOUBLE_EQ(manager.average_learned_length(), 3.0);

    CardinalityConstraint c2(1, 1, {0}, {3});
    c2.literal_block_distance = 4;
    manager.on_learned_constraint_added(c2);

    EXPECT_EQ(manager.last_literal_block_distance(), 4);
    EXPECT_EQ(manager.last_learned_length(), 2);
    EXPECT_EQ(manager.total_learned_count(), 2);
    EXPECT_DOUBLE_EQ(manager.average_learned_literal_block_distance(), 3.0);
    EXPECT_DOUBLE_EQ(manager.average_learned_length(), 2.5);

    // Test on_restart, on_decision, on_propagation
    manager.on_restart();
    EXPECT_EQ(manager.number_of_restarts(), 1);

    manager.on_decision();
    EXPECT_EQ(manager.number_of_decisions(), 1);

    manager.on_propagation(3);
    EXPECT_EQ(manager.number_of_propagations(), 3);
}

/*****************************************************************************/
TEST(TestCDCLCore, cdcl_solve_statistics_test) {
    // Problem requiring decisions and propagations
    matrix_model::MatrixModel m = make_ge_instance(5, 3.0);
    option::Option            opt;
    opt.cdcl.time_max      = 10.0;
    opt.cdcl.iteration_max = 10000;
    opt.output.verbose     = option::verbose::Inner;

    CDCLCore solver(&m, std::nullopt, opt);
    solver.run();

    const auto& res = solver.result();
    EXPECT_EQ(res.termination_status, CDCLCoreTerminationStatus::FEASIBLE);
    EXPECT_GT(res.number_of_decisions + res.number_of_propagations, 0);
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
TEST(TestCDCLCore, cdcl_solve_mps_instance) {
    std::vector<std::string> CANDIDATES = {
        "./air05.mps",
        "air05.mps",
        "../printemps_benchmark/instance/MIPLIB2017_Benchmark/bnatt400.mps",
        "/Volumes/Data/mps/bnatt400.mps",
        "../printemps_benchmark/instance/MIPLIB2017_Benchmark/ex10.mps",
        "/Volumes/Data/mps/ex10.mps",
        "scratch/ex9.mps",
        "/Volumes/Data/mps/ex9.mps"};

    std::string MPS_PATH = "";
    for (const auto& path : CANDIDATES) {
        std::ifstream f(path.c_str());
        if (f.good()) {
            MPS_PATH = path;
            break;
        }
    }

    if (MPS_PATH.empty()) {
        GTEST_SKIP() << "No suitable MPS benchmark file found";
    }

    mps::MPS mps_data;
    mps_data.read_mps(MPS_PATH);

    model::Model<int, double> model;
    model.mps_handler().import(mps_data, false);

    option::Option opt;
    opt.preprocess.is_enabled_presolve = true;
    opt.output.verbose                 = option::verbose::Off;
    model.builder().build(opt, false);

    auto matrix_model = model.matrix_model_handler().export_matrix_model();

    option::Option cdcl_opt;
    cdcl_opt.cdcl.time_max  = 15.0;
    cdcl_opt.output.verbose = option::verbose::Off;

    CDCLCore solver(&matrix_model, std::nullopt, cdcl_opt);
    solver.run();

    EXPECT_EQ(solver.result().termination_status,
              CDCLCoreTerminationStatus::FEASIBLE);
}

/*****************************************************************************/
TEST(TestCDCLCore, cdcl_controller_test) {
    model::Model<int, double> model;
    auto&                     x = model.create_variables("x", 3, 0, 1);
    model.create_constraint("c1", x(0) + x(1) - x(2) == 0);
    model.create_constraint("c2", x(0) + x(2) == 1);

    option::Option opt;
    opt.preprocess.is_enabled_presolve = false;
    opt.pdlp.is_enabled                = false;
    opt.lagrange_dual.is_enabled       = false;
    opt.local_search.is_enabled        = false;
    opt.tabu_search.iteration_max      = 0;
    opt.cdcl.is_enabled                = true;
    opt.output.verbose                 = option::verbose::Off;

    solver::Solver<int, double> solver(&model, opt);
    auto                        result = solver.solve();

    EXPECT_TRUE(result.solution.is_feasible());
    EXPECT_EQ(solver.cdcl_controller().result().core.termination_status,
              CDCLCoreTerminationStatus::FEASIBLE);
}

/*****************************************************************************/
TEST(TestCDCLCore, cdcl_controller_parallel_portfolio_test) {
    model::Model<int, double> model;
    auto&                     x = model.create_variables("x", 4, 0, 1);
    model.create_constraint("c1", x(0) + x(1) + x(2) >= 2);
    model.create_constraint("c2", x(1) + x(2) + x(3) <= 1);
    model.create_constraint("c3", x(0) + x(3) == 1);

    option::Option opt;
    opt.preprocess.is_enabled_presolve = false;
    opt.pdlp.is_enabled                = false;
    opt.lagrange_dual.is_enabled       = false;
    opt.local_search.is_enabled        = false;
    opt.tabu_search.iteration_max      = 0;
    opt.cdcl.is_enabled                = true;
    opt.parallel.is_enabled_cdcl_parallelization = true;
    opt.parallel.number_of_threads_cdcl          = 4;
    opt.output.verbose                           = option::verbose::Off;

    solver::Solver<int, double> solver(&model, opt);
    auto                        result = solver.solve();

    EXPECT_TRUE(result.solution.is_feasible());
    EXPECT_EQ(solver.cdcl_controller().result().core.termination_status,
              CDCLCoreTerminationStatus::FEASIBLE);
}

/*****************************************************************************/
TEST(TestCDCLCore, cdcl_best_intermediate_solution_test) {
    model::Model<int, double> model;
    auto&                     x = model.create_variables("x", 2, 0, 1);
    // Incompatible fractional constraints:
    // c1: 0.2*x0 + 0.3*x1 <= 0.1
    // c2: 0.5*x0 + 0.5*x1 >= 0.8
    model.create_constraint("c1", 0.2 * x(0) + 0.3 * x(1) <= 0.1);
    model.create_constraint("c2", 0.5 * x(0) + 0.5 * x(1) >= 0.8);

    option::Option opt;
    opt.preprocess.is_enabled_presolve = false;
    opt.pdlp.is_enabled                = false;
    opt.lagrange_dual.is_enabled       = false;
    opt.local_search.is_enabled        = false;
    opt.tabu_search.iteration_max                 = 0;
    opt.cdcl.is_enabled                           = true;
    opt.cdcl.is_enabled_evaluate_full_solutions   = true;
    opt.output.verbose                            = option::verbose::Off;

    solver::Solver<int, double> solver(&model, opt);
    auto                        result = solver.solve();

    const auto& cdcl_res = solver.cdcl_controller().result().core;
    EXPECT_EQ(cdcl_res.termination_status,
              CDCLCoreTerminationStatus::INFEASIBLE);

    // best_solution must be tracked
    EXPECT_EQ(2u, cdcl_res.best_solution.size());
    EXPECT_TRUE(std::isfinite(cdcl_res.best_total_violation));

    // Verify evaluation conforms to original Model's floating-point scale:
    // For x=(0,0): c1 viol = 0, c2 viol = 0.8 - 0 = 0.8 (total = 0.8).
    // For x=(1,1): c1 viol = 0.5 - 0.1 = 0.4, c2 viol = 0 (total = 0.4).
    // For x=(1,0): c1 viol = 0.2 - 0.1 = 0.1, c2 viol = 0.8 - 0.5 = 0.3 (total = 0.4).
    // For x=(0,1): c1 viol = 0.3 - 0.1 = 0.2, c2 viol = 0.8 - 0.5 = 0.3 (total = 0.5).
    // Best violation is 0.4 (NOT integerized 4).
    EXPECT_NEAR(0.4, cdcl_res.best_total_violation, 1e-6);

    // Incumbent holder must be updated with the best intermediate solution
    const auto& global_state = solver.global_state();
    const auto& augmented_score =
        global_state.incumbent_holder.global_augmented_incumbent_score();
    EXPECT_NEAR(0.4, augmented_score.total_violation, 1e-6);
}

/*****************************************************************************/
TEST(TestCDCLCore, cdcl_full_solution_evaluation_disabled_by_default_test) {
    model::Model<int, double> model;
    auto&                     x = model.create_variables("x", 2, 0, 1);
    model.create_constraint("c1", 0.2 * x(0) + 0.3 * x(1) <= 0.1);
    model.create_constraint("c2", 0.5 * x(0) + 0.5 * x(1) >= 0.8);

    option::Option opt;
    opt.preprocess.is_enabled_presolve = false;
    opt.pdlp.is_enabled                = false;
    opt.lagrange_dual.is_enabled       = false;
    opt.local_search.is_enabled        = false;
    opt.tabu_search.iteration_max      = 0;
    opt.cdcl.is_enabled                = true;
    // cdcl.is_enabled_evaluate_full_solutions is false by default
    opt.output.verbose                 = option::verbose::Off;

    solver::Solver<int, double> solver(&model, opt);
    auto                        result = solver.solve();

    const auto& cdcl_res = solver.cdcl_controller().result().core;
    EXPECT_EQ(cdcl_res.termination_status,
              CDCLCoreTerminationStatus::INFEASIBLE);

    // best_solution must NOT be tracked when disabled
    EXPECT_TRUE(cdcl_res.best_solution.empty());
    EXPECT_TRUE(std::isinf(cdcl_res.best_total_violation));

    // Incumbent holder violation should remain at the initial solution's violation (x=(0,0) -> viol=0.8)
    const auto& global_state = solver.global_state();
    const auto& augmented_score =
        global_state.incumbent_holder.global_augmented_incumbent_score();
    EXPECT_NEAR(0.8, augmented_score.total_violation, 1e-6);
}

/*****************************************************************************/
TEST(TestCDCLCore, cdcl_selection_constraint_satisfaction_test) {
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

    option::Option opt;
    opt.preprocess.is_enabled_presolve = false;
    opt.pdlp.is_enabled                = false;
    opt.lagrange_dual.is_enabled       = false;
    opt.local_search.is_enabled        = false;
    opt.tabu_search.iteration_max                 = 0;
    opt.cdcl.is_enabled                           = true;
    opt.cdcl.is_enabled_evaluate_full_solutions   = true;
    opt.output.verbose                            = option::verbose::Off;

    solver::Solver<int, double> solver(&model, opt);
    auto                        result = solver.solve();

    const auto& cdcl_res = solver.cdcl_controller().result().core;
    EXPECT_EQ(cdcl_res.termination_status,
              CDCLCoreTerminationStatus::INFEASIBLE);

    // Best intermediate solution must be tracked
    EXPECT_EQ(7u, cdcl_res.best_solution.size());

    // Selection constraints must be strictly satisfied in best_solution:
    // sum(x_i) == 1
    int x_sum = cdcl_res.best_solution[0] + cdcl_res.best_solution[1] +
                cdcl_res.best_solution[2] + cdcl_res.best_solution[3];
    EXPECT_EQ(x_sum, 1);

    // sum(y_i) == 1
    int y_sum = cdcl_res.best_solution[4] + cdcl_res.best_solution[5] +
                cdcl_res.best_solution[6];
    EXPECT_EQ(y_sum, 1);

    // Verification must succeed without throw (a_IS_ENABLED_CORRECTION = false)
    EXPECT_NO_THROW(model.verifier().verify_and_correct(false, false));

    // Verify selected_variable_ptr is properly set for each selection
    for (const auto& selection : model.selections()) {
        EXPECT_NE(selection.selected_variable_ptr, nullptr);
        EXPECT_EQ(selection.selected_variable_ptr->value(), 1);
    }
}

}  // namespace

