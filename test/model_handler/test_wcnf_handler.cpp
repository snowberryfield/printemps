/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
// Modified by Antigravity (2026)
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
using namespace printemps;
/*****************************************************************************/
class TestModelWCNFHandler : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestModelWCNFHandler, import) {
    wcnf::WCNF wcnf;
    wcnf.read_wcnf("./dat/wcnf/test_00a.wcnf");

    model::Model<int, double> model;
    model.wcnf_handler().import(wcnf);
    model.builder().update_derived_components();

    EXPECT_EQ(10, model.reference().number_of_variables()); // 7 variables + 3 soft slacks
    EXPECT_EQ(4, model.reference().number_of_constraints()); // 1 hard clause + 3 soft clauses
    EXPECT_TRUE(model.is_minimization());

    auto &variable_name_map   = model.reference().variable_name_map;
    auto &constraint_name_map = model.reference().constraint_name_map;

    EXPECT_NO_THROW(variable_name_map.at("x_1"));
    EXPECT_NO_THROW(variable_name_map.at("x_7"));
    EXPECT_NO_THROW(variable_name_map.at("s_1"));
    EXPECT_NO_THROW(variable_name_map.at("s_3"));

    EXPECT_NO_THROW(constraint_name_map.at("hard_clause_0"));
    EXPECT_NO_THROW(constraint_name_map.at("soft_clause_1"));
    EXPECT_NO_THROW(constraint_name_map.at("soft_clause_3"));
}
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/
