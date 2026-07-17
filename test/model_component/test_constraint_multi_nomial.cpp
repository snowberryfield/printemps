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
class TestConstraintMultinomial : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestConstraintMultinomial, initialize) {
    model_component::ConstraintMultinomial<int, double> nomial;

    EXPECT_EQ(nullptr, nomial.constraint_ptr);
    EXPECT_TRUE(nomial.variable_ptrs.empty());
    EXPECT_TRUE(nomial.coefficients.empty());
    EXPECT_EQ(0, nomial.constant_value);
    EXPECT_EQ(model_component::ConstraintSense::Equal, nomial.sense);
}

/*****************************************************************************/
TEST_F(TestConstraintMultinomial, setup) {
    model_component::ConstraintMultinomial<int, double> nomial;

    model::Model<int, double> model;

    auto& x = model.create_variables("x", 2, 0, 1);
    auto& f = model.create_constraint("f", x(0) - x(1) <= 1);

    model.builder().setup_unique_names();
    nomial.setup(&f(0));

    EXPECT_EQ(&f(0), nomial.constraint_ptr);
    EXPECT_EQ(&x(0), nomial.variable_ptrs[0]);
    EXPECT_EQ(&x(1), nomial.variable_ptrs[1]);

    EXPECT_EQ(1, nomial.coefficients[0]);
    EXPECT_EQ(-1, nomial.coefficients[1]);

    EXPECT_EQ(-1, nomial.constant_value);
    EXPECT_EQ(model_component::ConstraintSense::Less, nomial.sense);
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/