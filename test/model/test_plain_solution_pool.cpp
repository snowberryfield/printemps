/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <random>

#include <cppmh.h>

namespace {
/*****************************************************************************/
class TestPlainSolutionPool : public ::testing::Test {
   protected:
    cppmh::utility::IntegerUniformRandom m_random_integer;
    cppmh::utility::IntegerUniformRandom m_random_positive_integer;

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
TEST_F(TestPlainSolutionPool, initialize) {
    cppmh::model::PlainSolutionPool<int, double> pool;

    EXPECT_EQ(0, pool.size());
    EXPECT_EQ(0, pool.max_size());
    EXPECT_EQ(true, pool.is_ascending());
    EXPECT_EQ(true, pool.solutions().empty());
}

/*****************************************************************************/
TEST_F(TestPlainSolutionPool, setup) {
    {
        cppmh::model::PlainSolutionPool<int, double> pool(100, true);

        EXPECT_EQ(0, pool.size());
        EXPECT_EQ(100, pool.max_size());
        EXPECT_EQ(true, pool.is_ascending());
        EXPECT_EQ(true, pool.solutions().empty());
    }
    {
        cppmh::model::PlainSolutionPool<int, double> pool(200, false);

        EXPECT_EQ(0, pool.size());
        EXPECT_EQ(200, pool.max_size());
        EXPECT_EQ(false, pool.is_ascending());
        EXPECT_EQ(true, pool.solutions().empty());
    }
}

/*****************************************************************************/
TEST_F(TestPlainSolutionPool, push) {
    cppmh::model::PlainSolution<int, double> solution_0;
    solution_0.objective = 10;
    solution_0.variables = {
        0,
        0,
        0,
        0,
    };
    cppmh::model::PlainSolution<int, double> solution_1;
    solution_1.objective = 10;
    solution_1.variables = {
        0,
        0,
        0,
        0,
    };
    cppmh::model::PlainSolution<int, double> solution_2;
    solution_2.objective = 5;
    solution_2.variables = {
        0,
        0,
        1,
        1,
    };

    cppmh::model::PlainSolution<int, double> solution_3;
    solution_3.objective = 1;
    solution_3.variables = {
        1,
        1,
        1,
        1,
    };

    std::vector<cppmh::model::PlainSolution<int, double>> solutions = {
        solution_0, solution_1, solution_2, solution_3};

    {
        cppmh::model::PlainSolutionPool<int, double> pool(2, true);

        pool.push(solution_0);
        EXPECT_EQ(1, pool.size());

        pool.push(solution_1);
        EXPECT_EQ(1, pool.size());

        pool.push(solution_2);
        EXPECT_EQ(2, pool.size());

        pool.push(solution_3);
        EXPECT_EQ(2, pool.size());

        EXPECT_EQ(1, pool.solutions()[0].objective);
        EXPECT_EQ(5, pool.solutions()[1].objective);
    }
    {
        cppmh::model::PlainSolutionPool<int, double> pool(2, false);

        pool.push(solutions);
        EXPECT_EQ(2, pool.size());
        EXPECT_EQ(10, pool.solutions()[0].objective);
        EXPECT_EQ(5, pool.solutions()[1].objective);
    }
}

/*****************************************************************************/
TEST_F(TestPlainSolutionPool, size) {
    /// This method is tested in push();
}

/*****************************************************************************/
TEST_F(TestPlainSolutionPool, max_size) {
    /// This method is tested in setup().
}

/*****************************************************************************/
TEST_F(TestPlainSolutionPool, is_ascending) {
    /// This method is tested in setup().
}

/*****************************************************************************/
TEST_F(TestPlainSolutionPool, solutions) {
    /// This method is tested in push().
}

/*****************************************************************************/
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/