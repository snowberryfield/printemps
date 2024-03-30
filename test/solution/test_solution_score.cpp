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
class TestSolutionScore : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestSolutionScore, constructor) {
    solution::SolutionScore score;

    EXPECT_FLOAT_EQ(0.0, score.objective);
    EXPECT_FLOAT_EQ(0.0, score.objective_improvement);
    EXPECT_FLOAT_EQ(0.0, score.total_violation);
    EXPECT_FLOAT_EQ(0.0, score.local_penalty);
    EXPECT_FLOAT_EQ(0.0, score.global_penalty);
    EXPECT_FLOAT_EQ(0.0, score.local_augmented_objective);
    EXPECT_FLOAT_EQ(0.0, score.global_augmented_objective);
    EXPECT_FALSE(score.is_feasible);
    EXPECT_FALSE(score.is_objective_improvable);
    EXPECT_FALSE(score.is_feasibility_improvable);
}

/*****************************************************************************/
TEST_F(TestSolutionScore, argmax_index_objective_improvement) {
    std::vector<solution::SolutionScore> scores(5);
    scores[0].objective_improvement = 2.0;
    scores[1].objective_improvement = 3.0;
    scores[2].objective_improvement = 4.0;
    scores[3].objective_improvement = 5.0;
    scores[4].objective_improvement = 1.0;

    EXPECT_EQ(3, solution::argmax_index_objective_improvement(scores));
}

/*****************************************************************************/
TEST_F(TestSolutionScore, argmin_index_objective) {
    std::vector<solution::SolutionScore> scores(5);
    scores[0].objective = 2.0;
    scores[1].objective = 3.0;
    scores[2].objective = 4.0;
    scores[3].objective = 5.0;
    scores[4].objective = 1.0;

    EXPECT_EQ(4, solution::argmin_index_objective(scores));
}

/*****************************************************************************/
TEST_F(TestSolutionScore, argmin_index_local_penalty) {
    std::vector<solution::SolutionScore> scores(5);
    scores[0].local_penalty = 2.0;
    scores[1].local_penalty = 3.0;
    scores[2].local_penalty = 4.0;
    scores[3].local_penalty = 5.0;
    scores[4].local_penalty = 1.0;

    EXPECT_EQ(4, solution::argmin_index_local_penalty(scores));
}

/*****************************************************************************/
TEST_F(TestSolutionScore, argmin_index_global_penalty) {
    std::vector<solution::SolutionScore> scores(5);
    scores[0].global_penalty = 2.0;
    scores[1].global_penalty = 3.0;
    scores[2].global_penalty = 4.0;
    scores[3].global_penalty = 5.0;
    scores[4].global_penalty = 1.0;

    EXPECT_EQ(4, solution::argmin_index_global_penalty(scores));
}

/*****************************************************************************/
TEST_F(TestSolutionScore, argmin_index_local_augmented_objective) {
    std::vector<solution::SolutionScore> scores(5);
    scores[0].local_augmented_objective = 2.0;
    scores[1].local_augmented_objective = 3.0;
    scores[2].local_augmented_objective = 4.0;
    scores[3].local_augmented_objective = 5.0;
    scores[4].local_augmented_objective = 1.0;

    EXPECT_EQ(4, solution::argmin_index_local_augmented_objective(scores));
}

/*****************************************************************************/
TEST_F(TestSolutionScore, argmin_index_global_augmented_objective) {
    std::vector<solution::SolutionScore> scores(5);
    scores[0].global_augmented_objective = 2.0;
    scores[1].global_augmented_objective = 3.0;
    scores[2].global_augmented_objective = 4.0;
    scores[3].global_augmented_objective = 5.0;
    scores[4].global_augmented_objective = 1.0;

    EXPECT_EQ(4, solution::argmin_index_global_augmented_objective(scores));
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/