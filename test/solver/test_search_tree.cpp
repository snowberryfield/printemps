/*****************************************************************************/
// Copyright (c) 2020-2026 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
using namespace printemps;
using namespace printemps::solver;

/*****************************************************************************/
class TestSearchTree : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown(void) {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestSearchTree, initialize) {
    SearchTree<int, double> search_tree;

    EXPECT_TRUE(search_tree.frontier_solutions().empty());
    EXPECT_TRUE(search_tree.locally_optimal_solutions().empty());
}

/*****************************************************************************/
TEST_F(TestSearchTree, constructor) {
    solution::SparseSolution<int, double> solution_0;
    solution_0.variables["x_0"]           = 0;
    solution_0.variables["x_1"]           = 0;
    solution_0.global_augmented_objective = 10.0;

    solution::SparseSolution<int, double> solution_1;
    solution_1.variables["x_0"]           = 1;
    solution_1.variables["x_1"]           = 0;
    solution_1.global_augmented_objective = 5.0;

    solution::SparseSolution<int, double> solution_2;
    solution_2.variables["x_0"]           = 1;
    solution_2.variables["x_1"]           = 1;
    solution_2.global_augmented_objective = 20.0;

    solution::SolutionArchive<int, double> archive(
        10, solution::SortMode::Ascending, "archive", 2, 0);
    archive.push(solution_0);
    archive.push(solution_1);
    archive.push(solution_2);

    SearchTree<int, double> search_tree(archive, solution_1);

    EXPECT_FALSE(search_tree.frontier_solutions().empty());
    EXPECT_FALSE(search_tree.locally_optimal_solutions().empty());
}

/*****************************************************************************/
TEST_F(TestSearchTree, frontier_solutions) {
    /// This method is tested in constructor.
}

/*****************************************************************************/
TEST_F(TestSearchTree, locally_optimal_solutions) {
    /// This method is tested in constructor.
}

/*****************************************************************************/
TEST_F(TestSearchTree, update) {
    solution::SparseSolution<int, double> solution_0;
    solution_0.variables["x_0"]           = 0;
    solution_0.variables["x_1"]           = 0;
    solution_0.global_augmented_objective = 10.0;

    solution::SparseSolution<int, double> solution_1;
    solution_1.variables["x_0"]           = 1;
    solution_1.variables["x_1"]           = 0;
    solution_1.global_augmented_objective = 5.0;

    solution::SolutionArchive<int, double> archive(
        10, solution::SortMode::Ascending, "archive", 2, 0);
    archive.push(solution_0);
    archive.push(solution_1);

    SearchTree<int, double> search_tree;
    search_tree.update(archive, solution_1);

    EXPECT_FALSE(search_tree.frontier_solutions().empty());
    EXPECT_FALSE(search_tree.locally_optimal_solutions().empty());
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/