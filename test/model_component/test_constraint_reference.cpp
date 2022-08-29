/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
using namespace printemps;
/*****************************************************************************/
class TestConstraintReference : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestConstraintReference, initialize) {
    model_component::ConstraintReference<int, double> reference;

    EXPECT_TRUE(reference.constraint_ptrs.empty());
    EXPECT_TRUE(reference.enabled_constraint_ptrs.empty());
    EXPECT_TRUE(reference.disabled_constraint_ptrs.empty());
}
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/