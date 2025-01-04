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
class TestVariableReference : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestVariableReference, initialize) {
    model_component::VariableReference<int, double> reference;

    EXPECT_TRUE(reference.variable_ptrs.empty());
    EXPECT_TRUE(reference.mutable_variable_ptrs.empty());
    EXPECT_TRUE(reference.fixed_variable_ptrs.empty());
}
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/