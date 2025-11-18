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
class TestIncumbentHolder : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestIncumbentHolder, constructor) {
    solution::IncumbentHolder<int, double> incumbent_holder;

    EXPECT_FALSE(incumbent_holder.is_found_feasible_solution());
    EXPECT_EQ(HUGE_VALF,
              incumbent_holder.local_augmented_incumbent_objective());
    EXPECT_EQ(HUGE_VALF,
              incumbent_holder.global_augmented_incumbent_objective());
    EXPECT_EQ(HUGE_VALF, incumbent_holder.feasible_incumbent_objective());
    EXPECT_FLOAT_EQ(0.0, incumbent_holder.dual_bound());
}

/*****************************************************************************/
TEST_F(TestIncumbentHolder, initialize) {
    solution::IncumbentHolder<int, double> incumbent_holder;

    solution::DenseSolution<int, double> solution;
    solution::SolutionScore              score;
    score.is_feasible                = true;
    score.objective                  = 10.0;
    score.local_augmented_objective  = 10.0;
    score.global_augmented_objective = 10.0;
    incumbent_holder.try_update_incumbent(solution, score);
    incumbent_holder.update_dual_bound(10.0);

    EXPECT_TRUE(incumbent_holder.is_found_feasible_solution());
    EXPECT_FLOAT_EQ(10.0,
                    incumbent_holder.local_augmented_incumbent_objective());
    EXPECT_FLOAT_EQ(10.0,
                    incumbent_holder.global_augmented_incumbent_objective());
    EXPECT_FLOAT_EQ(10.0, incumbent_holder.feasible_incumbent_objective());
    EXPECT_FLOAT_EQ(10.0, incumbent_holder.dual_bound());

    incumbent_holder.initialize();

    EXPECT_FALSE(incumbent_holder.is_found_feasible_solution());
    EXPECT_EQ(HUGE_VALF,
              incumbent_holder.local_augmented_incumbent_objective());
    EXPECT_EQ(HUGE_VALF,
              incumbent_holder.global_augmented_incumbent_objective());
    EXPECT_EQ(HUGE_VALF, incumbent_holder.feasible_incumbent_objective());
    EXPECT_FLOAT_EQ(0.0, incumbent_holder.dual_bound());
}

/*****************************************************************************/
TEST_F(TestIncumbentHolder, try_update_incumbent_arg_model_ptr) {
    solution::IncumbentHolder<int, double> incumbent_holder;
    model::Model<int, double>              model;

    auto &x = model.create_variables("x", 2, 0, 1);
    auto &g = model.create_constraints("g", 2);

    g(0) = x(0) + x(1) <= 1;
    g(1) = x(0) - x(1) == 1;
    model.minimize(x(0) + x(1));
    model.builder().setup_structure();

    {
        x(0) = 1;
        x(1) = 1;
        model.updater().update();

        solution::SolutionScore score;
        score.is_feasible                = false;
        score.objective                  = 2.0;
        score.local_augmented_objective  = 10.0;
        score.global_augmented_objective = 20.0;
        auto status = incumbent_holder.try_update_incumbent(&model, score);

        EXPECT_EQ(3, status);
        EXPECT_FLOAT_EQ(  //
            10.0,         //
            incumbent_holder.local_augmented_incumbent_objective());
        EXPECT_FLOAT_EQ(  //
            20.0,         //
            incumbent_holder.global_augmented_incumbent_objective());
        EXPECT_FLOAT_EQ(  //
            HUGE_VALF,    //
            incumbent_holder.feasible_incumbent_objective());

        EXPECT_FLOAT_EQ(  //
            2.0,          //
            incumbent_holder.local_augmented_incumbent_score().objective);
        EXPECT_FLOAT_EQ(  //
            2.0,          //
            incumbent_holder.global_augmented_incumbent_score().objective);
        EXPECT_FLOAT_EQ(  //
            0,            //
            incumbent_holder.feasible_incumbent_score().objective);

        EXPECT_FLOAT_EQ(  //
            2.0,          //
            incumbent_holder.local_augmented_incumbent_solution().objective);
        EXPECT_FLOAT_EQ(  //
            2.0,          //
            incumbent_holder.global_augmented_incumbent_solution().objective);
        EXPECT_FLOAT_EQ(  //
            0,            //
            incumbent_holder.feasible_incumbent_solution().objective);

        EXPECT_FALSE(incumbent_holder.is_found_feasible_solution());
    }

    {
        x(0) = 1;
        x(1) = 0;
        model.updater().update();

        solution::SolutionScore score;
        score.is_feasible                = true;
        score.objective                  = 1.0;
        score.local_augmented_objective  = 1.0;
        score.global_augmented_objective = 1.0;
        auto status = incumbent_holder.try_update_incumbent(&model, score);

        EXPECT_EQ(7, status);
        EXPECT_FLOAT_EQ(  //
            1.0,          //
            incumbent_holder.local_augmented_incumbent_objective());
        EXPECT_FLOAT_EQ(  //
            1.0,          //
            incumbent_holder.global_augmented_incumbent_objective());
        EXPECT_FLOAT_EQ(  //
            1.0,          //
            incumbent_holder.feasible_incumbent_objective());

        EXPECT_FLOAT_EQ(  //
            1.0,          //
            incumbent_holder.local_augmented_incumbent_score().objective);
        EXPECT_FLOAT_EQ(  //
            1.0,          //
            incumbent_holder.global_augmented_incumbent_score().objective);
        EXPECT_FLOAT_EQ(  //
            1.0,          //
            incumbent_holder.feasible_incumbent_score().objective);

        EXPECT_FLOAT_EQ(  //
            1.0,          //
            incumbent_holder.local_augmented_incumbent_solution().objective);
        EXPECT_FLOAT_EQ(  //
            1.0,          //
            incumbent_holder.global_augmented_incumbent_solution().objective);
        EXPECT_FLOAT_EQ(  //
            1.0,          //
            incumbent_holder.feasible_incumbent_solution().objective);

        EXPECT_TRUE(incumbent_holder.is_found_feasible_solution());
    }
}

/*****************************************************************************/
TEST_F(TestIncumbentHolder, try_update_incumbent_arg_solution) {
    solution::IncumbentHolder<int, double> incumbent_holder;

    {
        solution::DenseSolution<int, double> solution;
        solution.is_feasible = false;
        solution.objective   = 100.0;

        solution::SolutionScore score;
        score.is_feasible                = false;
        score.objective                  = 200.0;
        score.local_augmented_objective  = 300.0;
        score.global_augmented_objective = 400.0;
        auto status = incumbent_holder.try_update_incumbent(solution, score);

        EXPECT_EQ(3, status);
        EXPECT_FLOAT_EQ(  //
            300.0,        //
            incumbent_holder.local_augmented_incumbent_objective());
        EXPECT_FLOAT_EQ(  //
            400.0,        //
            incumbent_holder.global_augmented_incumbent_objective());
        EXPECT_FLOAT_EQ(  //
            HUGE_VALF,    //
            incumbent_holder.feasible_incumbent_objective());

        EXPECT_FLOAT_EQ(  //
            200.0,        //
            incumbent_holder.local_augmented_incumbent_score().objective);
        EXPECT_FLOAT_EQ(  //
            200.0,        //
            incumbent_holder.global_augmented_incumbent_score().objective);
        EXPECT_FLOAT_EQ(  //
            0,            //
            incumbent_holder.feasible_incumbent_score().objective);

        EXPECT_FLOAT_EQ(  //
            100.0,        //
            incumbent_holder.local_augmented_incumbent_solution().objective);
        EXPECT_FLOAT_EQ(  //
            100.0,        //
            incumbent_holder.global_augmented_incumbent_solution().objective);
        EXPECT_FLOAT_EQ(  //
            0,            //
            incumbent_holder.feasible_incumbent_solution().objective);

        EXPECT_FALSE(incumbent_holder.is_found_feasible_solution());
    }

    {
        solution::DenseSolution<int, double> solution;
        solution.is_feasible = true;
        solution.objective   = 50.0;

        solution::SolutionScore score;
        score.is_feasible                = true;
        score.objective                  = 100.0;
        score.local_augmented_objective  = 150.0;
        score.global_augmented_objective = 200.0;
        auto status = incumbent_holder.try_update_incumbent(solution, score);

        EXPECT_EQ(7, status);
        EXPECT_FLOAT_EQ(  //
            150.0,        //
            incumbent_holder.local_augmented_incumbent_objective());
        EXPECT_FLOAT_EQ(  //
            200.0,        //
            incumbent_holder.global_augmented_incumbent_objective());
        EXPECT_FLOAT_EQ(  //
            100.0,        //
            incumbent_holder.feasible_incumbent_objective());

        EXPECT_FLOAT_EQ(  //
            100.0,        //
            incumbent_holder.local_augmented_incumbent_score().objective);
        EXPECT_FLOAT_EQ(  //
            100.0,        //
            incumbent_holder.global_augmented_incumbent_score().objective);
        EXPECT_FLOAT_EQ(  //
            100.0,        //
            incumbent_holder.feasible_incumbent_score().objective);

        EXPECT_FLOAT_EQ(  //
            50.0,         //
            incumbent_holder.local_augmented_incumbent_solution().objective);
        EXPECT_FLOAT_EQ(  //
            50.0,         //
            incumbent_holder.global_augmented_incumbent_solution().objective);
        EXPECT_FLOAT_EQ(  //
            50.0,         //
            incumbent_holder.feasible_incumbent_solution().objective);

        EXPECT_TRUE(incumbent_holder.is_found_feasible_solution());
    }
}

/*****************************************************************************/
TEST_F(TestIncumbentHolder, reset_local_augmented_incumbent) {
    solution::IncumbentHolder<int, double> incumbent_holder;

    solution::DenseSolution<int, double> solution;
    solution::SolutionScore              score;
    score.is_feasible                = true;
    score.objective                  = 10.0;
    score.local_augmented_objective  = 10.0;
    score.global_augmented_objective = 10.0;

    incumbent_holder.try_update_incumbent(solution, score);
    EXPECT_FLOAT_EQ(10.0,
                    incumbent_holder.local_augmented_incumbent_objective());

    incumbent_holder.reset_local_augmented_incumbent();
    EXPECT_FLOAT_EQ(HUGE_VALF,
                    incumbent_holder.local_augmented_incumbent_objective());
}

/*****************************************************************************/
TEST_F(TestIncumbentHolder, is_found_feasible_solution) {
    /// This test is covered by try_update_incumbent_arg_solution().
}

/*****************************************************************************/
TEST_F(TestIncumbentHolder, local_augmented_incumbent_solution) {
    /// This test is covered by try_update_incumbent_arg_solution().
}

/*****************************************************************************/
TEST_F(TestIncumbentHolder, global_augmented_incumbent_solution) {
    /// This test is covered by try_update_incumbent_arg_solution().
}

/*****************************************************************************/
TEST_F(TestIncumbentHolder, feasible_incumbent_solution) {
    /// This test is covered by try_update_incumbent_arg_solution().
}

/*****************************************************************************/
TEST_F(TestIncumbentHolder, update_dual_bound) {
    /// This test is covered by initialize().
}

/*****************************************************************************/
TEST_F(TestIncumbentHolder, dual_bound) {
    /// This test is covered by initialize().
}

/*****************************************************************************/
TEST_F(TestIncumbentHolder, local_augmented_incumbent_objective) {
    /// This test is covered by initialize().
}

/*****************************************************************************/
TEST_F(TestIncumbentHolder, global_augmented_incumbent_objective) {
    /// This test is covered by initialize().
}

/*****************************************************************************/
TEST_F(TestIncumbentHolder, feasible_incumbent_objective) {
    /// This test is covered by initialize().
}

/*****************************************************************************/
TEST_F(TestIncumbentHolder, local_augmented_incumbent_score) {
    /// This test is covered by try_update_incumbent_arg_solution().
}

/*****************************************************************************/
TEST_F(TestIncumbentHolder, global_augmented_incumbent_score) {
    /// This test is covered by try_update_incumbent_arg_solution().
}

/*****************************************************************************/
TEST_F(TestIncumbentHolder, feasible_incumbent_score) {
    /// This test is covered by try_update_incumbent_arg_solution().
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/