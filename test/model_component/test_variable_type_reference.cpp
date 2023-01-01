/*****************************************************************************/
// Copyright (c) 2020-2023 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
using namespace printemps;
/*****************************************************************************/
class TestVariableTypeReference : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestVariableTypeReference, initialize) {
    model_component::VariableTypeReference<int, double> reference;

    EXPECT_TRUE(reference.binary_variable_ptrs.empty());
    EXPECT_TRUE(reference.integer_variable_ptrs.empty());
    EXPECT_TRUE(reference.selection_variable_ptrs.empty());
    EXPECT_TRUE(reference.dependent_binary_variable_ptrs.empty());
    EXPECT_TRUE(reference.dependent_integer_variable_ptrs.empty());
}
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/