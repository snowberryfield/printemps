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
class TestWCNF : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestWCNF, initialize) {
    wcnf::WCNF instance;
    EXPECT_EQ("", instance.name);
    EXPECT_TRUE(instance.hard_clauses.empty());
    EXPECT_TRUE(instance.soft_clauses.empty());
    EXPECT_EQ(0, instance.number_of_variables);
    EXPECT_EQ(0, instance.metadata.number_of_clauses);
    EXPECT_EQ(0, instance.metadata.number_of_hard_clauses);
    EXPECT_EQ(0, instance.metadata.number_of_soft_clauses);
    EXPECT_EQ(0u, instance.metadata.weight_sum);
    EXPECT_EQ(0u, instance.metadata.max_weight);
}

/*****************************************************************************/
TEST_F(TestWCNF, parse_clause_hard) {
    const auto CLAUSE = wcnf::WCNF::parse_clause("h 1 -2 3 0", 0);
    EXPECT_TRUE(CLAUSE.is_hard);
    EXPECT_EQ("hard_clause_0", CLAUSE.name);
    ASSERT_EQ(3u, CLAUSE.literals.size());
    EXPECT_EQ(1, CLAUSE.literals[0]);
    EXPECT_EQ(-2, CLAUSE.literals[1]);
    EXPECT_EQ(3, CLAUSE.literals[2]);
    EXPECT_EQ(1, CLAUSE.number_of_negative_literals());
}

/*****************************************************************************/
TEST_F(TestWCNF, parse_clause_soft) {
    const auto CLAUSE = wcnf::WCNF::parse_clause("7 -1 -2 0", 5);
    EXPECT_FALSE(CLAUSE.is_hard);
    EXPECT_EQ("soft_clause_5", CLAUSE.name);
    EXPECT_EQ(7u, CLAUSE.weight);
    ASSERT_EQ(2u, CLAUSE.literals.size());
    EXPECT_EQ(-1, CLAUSE.literals[0]);
    EXPECT_EQ(-2, CLAUSE.literals[1]);
    EXPECT_EQ(2, CLAUSE.number_of_negative_literals());
}

/*****************************************************************************/
TEST_F(TestWCNF, parse_clause_large_weight) {
    const std::string LINE =
        std::to_string((1ULL << 60)) + " 1 -2 0";
    const auto CLAUSE = wcnf::WCNF::parse_clause(LINE, 0);
    EXPECT_FALSE(CLAUSE.is_hard);
    EXPECT_EQ((1ULL << 60), CLAUSE.weight);
}

/*****************************************************************************/
TEST_F(TestWCNF, parse_clause_missing_terminator) {
    EXPECT_THROW(
        wcnf::WCNF::parse_clause("h 1 -2 3", 0),
        std::runtime_error);
}

/*****************************************************************************/
TEST_F(TestWCNF, parse_clause_zero_weight_rejected) {
    EXPECT_THROW(wcnf::WCNF::parse_clause("0 1 0", 0), std::runtime_error);
}

/*****************************************************************************/
TEST_F(TestWCNF, parse_clause_negative_weight_rejected) {
    EXPECT_THROW(wcnf::WCNF::parse_clause("-3 1 0", 0), std::runtime_error);
}

/*****************************************************************************/
TEST_F(TestWCNF, parse_clause_trailing_token_rejected) {
    EXPECT_THROW(wcnf::WCNF::parse_clause("1 -2 3 0 4", 0), std::runtime_error);
}

/*****************************************************************************/
TEST_F(TestWCNF, parse_clause_malformed_weight) {
    EXPECT_THROW(wcnf::WCNF::parse_clause("abc 1 0", 0), std::runtime_error);
}

/*****************************************************************************/
TEST_F(TestWCNF, read_wcnf_test_00a) {
    wcnf::WCNF instance;
    instance.read_wcnf("./dat/wcnf/test_00a.wcnf");

    EXPECT_EQ(7, instance.number_of_variables);
    ASSERT_EQ(1u, instance.hard_clauses.size());
    ASSERT_EQ(3u, instance.soft_clauses.size());

    EXPECT_EQ(4u, instance.hard_clauses[0].literals.size());
    EXPECT_TRUE(instance.hard_clauses[0].is_hard);

    EXPECT_EQ(1u, instance.soft_clauses[0].weight);
    EXPECT_EQ(6u, instance.soft_clauses[1].weight);
    EXPECT_EQ(4u, instance.soft_clauses[2].weight);

    EXPECT_EQ(1u + 6u + 4u, instance.metadata.weight_sum);
    EXPECT_EQ(6u, instance.metadata.max_weight);
    EXPECT_EQ(4, instance.metadata.number_of_clauses);
    EXPECT_EQ(1, instance.metadata.number_of_hard_clauses);
    EXPECT_EQ(3, instance.metadata.number_of_soft_clauses);
}

/*****************************************************************************/
TEST_F(TestWCNF, read_wcnf_test_00b) {
    wcnf::WCNF instance;
    instance.read_wcnf("./dat/wcnf/test_00b.wcnf");

    EXPECT_EQ(1, instance.number_of_variables);
    EXPECT_EQ(1, instance.metadata.number_of_hard_clauses);
    EXPECT_EQ(1, instance.metadata.number_of_soft_clauses);
}

/*****************************************************************************/
TEST_F(TestWCNF, read_wcnf_test_00c_tautology) {
    wcnf::WCNF instance;
    instance.read_wcnf("./dat/wcnf/test_00c.wcnf");

    EXPECT_EQ(1, instance.number_of_variables);
    ASSERT_EQ(1u, instance.hard_clauses.size());
    EXPECT_EQ(2u, instance.hard_clauses[0].literals.size());
    ASSERT_EQ(1u, instance.soft_clauses.size());
    EXPECT_EQ(3u, instance.soft_clauses[0].literals.size());
}

/*****************************************************************************/
TEST_F(TestWCNF, read_wcnf_test_00d_with_comments_and_blanks) {
    wcnf::WCNF instance;
    instance.read_wcnf("./dat/wcnf/test_00d.wcnf");

    EXPECT_EQ(5, instance.number_of_variables);
    EXPECT_EQ(1, instance.metadata.number_of_hard_clauses);
    EXPECT_EQ(2, instance.metadata.number_of_soft_clauses);
    EXPECT_EQ(2u + 3u, instance.metadata.weight_sum);
}

/*****************************************************************************/
TEST_F(TestWCNF, read_wcnf_missing_file) {
    wcnf::WCNF instance;
    EXPECT_THROW(instance.read_wcnf("does_not_exist.wcnf"),
                 std::runtime_error);
}

/*****************************************************************************/
TEST_F(TestWCNF, import_wcnf_minimization) {
    wcnf::WCNF instance;
    instance.read_wcnf("./dat/wcnf/test_00a.wcnf");

    model::IPModel model;
    model.wcnf_handler().import(instance);

    EXPECT_TRUE(model.is_minimization());
}

/*****************************************************************************/
TEST_F(TestWCNF, import_wcnf_proxies) {
    wcnf::WCNF instance;
    instance.read_wcnf("./dat/wcnf/test_00a.wcnf");

    model::IPModel model;
    model.wcnf_handler().import(instance);

    /**
     * Two variable proxies are expected: "variables" (size = number of WCNF
     * variables) and "soft_slacks" (size = number of soft clauses).
     */
    const auto &VARIABLE_PROXIES = model.variable_proxies();
    ASSERT_EQ(2u, VARIABLE_PROXIES.size());
    EXPECT_EQ(instance.number_of_variables,
              VARIABLE_PROXIES[0].number_of_elements());
    EXPECT_EQ(instance.metadata.number_of_soft_clauses,
              VARIABLE_PROXIES[1].number_of_elements());

    /**
     * Two constraint proxies are expected: "hard_clauses" (size = number of
     * hard clauses) and "soft_clauses" (size = number of soft clauses).
     */
    const auto &CONSTRAINT_PROXIES = model.constraint_proxies();
    ASSERT_EQ(2u, CONSTRAINT_PROXIES.size());
    EXPECT_EQ(instance.metadata.number_of_hard_clauses,
              CONSTRAINT_PROXIES[0].number_of_elements());
    EXPECT_EQ(instance.metadata.number_of_soft_clauses,
              CONSTRAINT_PROXIES[1].number_of_elements());
}

/*****************************************************************************/
TEST_F(TestWCNF, import_wcnf_hard_only) {
    wcnf::WCNF instance;
    instance.read_wcnf("./dat/wcnf/test_00a.wcnf");
    /// Drop soft clauses to simulate the all-hard case.
    instance.soft_clauses.clear();
    instance.metadata.number_of_soft_clauses = 0;
    instance.metadata.weight_sum             = 0;
    instance.metadata.max_weight             = 0;

    model::IPModel model;
    model.wcnf_handler().import(instance);

    /// Even with zero soft clauses, the soft_slacks proxy is still created.
    const auto &VARIABLE_PROXIES = model.variable_proxies();
    ASSERT_EQ(2u, VARIABLE_PROXIES.size());
    EXPECT_EQ(0, VARIABLE_PROXIES[1].number_of_elements());
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/
