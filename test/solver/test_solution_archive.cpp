/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <random>

#include <printemps.h>

namespace {
/*****************************************************************************/
class TestSolutionArchive : public ::testing::Test {
   protected:
    printemps::utility::IntegerUniformRandom m_random_integer;
    printemps::utility::IntegerUniformRandom m_random_positive_integer;

    virtual void SetUp(void) {
        m_random_integer.setup(-1000, 1000, 0);
        m_random_positive_integer.setup(1, 1000, 0);
    }
    virtual void TearDown() {
        /// nothing to do
    }
    int random_integer(void) {
        return m_random_integer.generate_random();
    }

    int random_positive_integer(void) {
        return m_random_positive_integer.generate_random();
    }
};

/*****************************************************************************/
TEST_F(TestSolutionArchive, initialize) {
    printemps::solver::SolutionArchive<int, double> archive;

    EXPECT_EQ(0, archive.size());
    EXPECT_EQ(0, archive.max_size());
    EXPECT_EQ(true, archive.is_ascending());
    EXPECT_EQ(true, archive.solutions().empty());
}

/*****************************************************************************/
TEST_F(TestSolutionArchive, setup) {
    {
        printemps::solver::SolutionArchive<int, double> archive(100, true);

        EXPECT_EQ(0, archive.size());
        EXPECT_EQ(100, archive.max_size());
        EXPECT_EQ(true, archive.is_ascending());
        EXPECT_EQ(true, archive.solutions().empty());
    }
    {
        printemps::solver::SolutionArchive<int, double> archive(200, false);

        EXPECT_EQ(0, archive.size());
        EXPECT_EQ(200, archive.max_size());
        EXPECT_EQ(false, archive.is_ascending());
        EXPECT_EQ(true, archive.solutions().empty());
    }
}

/*****************************************************************************/
TEST_F(TestSolutionArchive, push) {
    printemps::model::PlainSolution<int, double> solution_0;
    solution_0.objective = 10;
    solution_0.variables = {
        0,
        0,
        0,
        0,
    };
    printemps::model::PlainSolution<int, double> solution_1;
    solution_1.objective = 10;
    solution_1.variables = {
        0,
        0,
        0,
        0,
    };
    printemps::model::PlainSolution<int, double> solution_2;
    solution_2.objective = 5;
    solution_2.variables = {
        0,
        0,
        1,
        1,
    };

    printemps::model::PlainSolution<int, double> solution_3;
    solution_3.objective = 1;
    solution_3.variables = {
        1,
        1,
        1,
        1,
    };

    std::vector<printemps::model::PlainSolution<int, double>> solutions = {
        solution_0, solution_1, solution_2, solution_3};

    {
        printemps::solver::SolutionArchive<int, double> archive(2, true);

        archive.push(solution_0);
        EXPECT_EQ(1, archive.size());

        archive.push(solution_1);
        EXPECT_EQ(1, archive.size());

        archive.push(solution_2);
        EXPECT_EQ(2, archive.size());

        archive.push(solution_3);
        EXPECT_EQ(2, archive.size());

        EXPECT_EQ(1, archive.solutions()[0].objective);
        EXPECT_EQ(5, archive.solutions()[1].objective);
    }
    {
        printemps::solver::SolutionArchive<int, double> archive(2, false);

        archive.push(solutions);
        EXPECT_EQ(2, archive.size());
        EXPECT_EQ(10, archive.solutions()[0].objective);
        EXPECT_EQ(5, archive.solutions()[1].objective);
    }
}

/*****************************************************************************/
TEST_F(TestSolutionArchive, size) {
    /// This method is tested in push();
}

/*****************************************************************************/
TEST_F(TestSolutionArchive, max_size) {
    /// This method is tested in setup().
}

/*****************************************************************************/
TEST_F(TestSolutionArchive, is_ascending) {
    /// This method is tested in setup().
}

/*****************************************************************************/
TEST_F(TestSolutionArchive, solutions) {
    /// This method is tested in push().
}

/*****************************************************************************/
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/