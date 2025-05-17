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
class TestTerm : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestTerm, initialize) {
    opb::OPBTerm term;
    EXPECT_EQ(0, term.coefficient);
    EXPECT_TRUE(term.variable_names.empty());
}

/*****************************************************************************/
TEST_F(TestTerm, concated_variable_name) {
    opb::OPBTerm term;
    term.variable_names.push_back("x0");
    EXPECT_EQ("x0", term.concated_variable_name());

    term.variable_names.push_back("x1");
    EXPECT_EQ("x0x1", term.concated_variable_name());
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/