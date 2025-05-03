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
class TestMPS : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestMPS, initialize) {
    mps::MPS mps;
    EXPECT_EQ("", mps.name);
    EXPECT_TRUE(mps.variables.empty());
    EXPECT_TRUE(mps.constraints.empty());
    EXPECT_TRUE(mps.variables.empty());
    EXPECT_TRUE(mps.variable_names.empty());
    EXPECT_TRUE(mps.constraint_names.empty());

    EXPECT_EQ(0, mps.number_of_variables);
    EXPECT_EQ(0, mps.number_of_lesser_constraints);
    EXPECT_EQ(0, mps.number_of_equal_constraints);
    EXPECT_EQ(0, mps.number_of_greater_constraints);
}

/*****************************************************************************/
TEST_F(TestMPS, read_mps_00) {
    // Default case
    mps::MPS mps;
    mps.read_mps("./test/dat/mps/test_00.mps");
    EXPECT_EQ("problem", mps.name);
    EXPECT_EQ(60, mps.number_of_variables);

    {
        auto x_0 = mps.variables["x_0"];
        EXPECT_EQ("x_0", x_0.name);
        EXPECT_EQ(mps::MPSVariableSense::Integer, x_0.sense);
        EXPECT_EQ(0, x_0.integer_lower_bound);
        EXPECT_EQ(1, x_0.integer_upper_bound);
        EXPECT_TRUE(x_0.is_bound_defined);
        EXPECT_FALSE(x_0.is_fixed);
    }

    {
        auto y_0 = mps.variables["y_0"];
        EXPECT_EQ("y_0", y_0.name);
        EXPECT_EQ(mps::MPSVariableSense::Continuous, y_0.sense);
        EXPECT_EQ(constant::INT_HALF_MIN, y_0.integer_lower_bound);
        EXPECT_EQ(constant::INT_HALF_MAX, y_0.integer_upper_bound);
        EXPECT_FLOAT_EQ(-HUGE_VAL, y_0.continuous_lower_bound);
        EXPECT_FLOAT_EQ(HUGE_VAL, y_0.continuous_upper_bound);
        EXPECT_TRUE(y_0.is_bound_defined);
        EXPECT_FALSE(y_0.is_fixed);
    }

    {
        auto z_0 = mps.variables["z_0"];
        EXPECT_EQ("z_0", z_0.name);
        EXPECT_EQ(mps::MPSVariableSense::Integer, z_0.sense);
        EXPECT_EQ(-100, z_0.integer_lower_bound);
        EXPECT_EQ(100, z_0.integer_upper_bound);
        EXPECT_TRUE(z_0.is_bound_defined);
        EXPECT_FALSE(z_0.is_fixed);
    }

    {
        auto c_1 = mps.constraints["_C1"];
        EXPECT_EQ("_C1", c_1.name);
        EXPECT_EQ(mps::MPSConstraintSense::Equal, c_1.sense);
        EXPECT_EQ(1, c_1.sensitivities["x_0"]);
        EXPECT_EQ(1, c_1.sensitivities["x_9"]);
        EXPECT_EQ(1, c_1.rhs);
    }

    {
        auto c_2 = mps.constraints["_C2"];
        EXPECT_EQ("_C2", c_2.name);
        EXPECT_EQ(mps::MPSConstraintSense::Less, c_2.sense);
        EXPECT_EQ(1, c_2.sensitivities["y_0"]);
        EXPECT_EQ(20, c_2.sensitivities["y_19"]);
        EXPECT_FLOAT_EQ(2E7, c_2.rhs);
    }

    {
        auto c_3 = mps.constraints["_C3"];
        EXPECT_EQ("_C3", c_3.name);
        EXPECT_EQ(mps::MPSConstraintSense::Greater, c_3.sense);
        EXPECT_EQ(1, c_3.sensitivities["z_0"]);
        EXPECT_EQ(30, c_3.sensitivities["z_29"]);
        EXPECT_FLOAT_EQ(300, c_3.rhs);
    }

    {
        EXPECT_EQ(1, mps.objective.sensitivities["x_0"]);
        EXPECT_EQ(10, mps.objective.sensitivities["x_9"]);
    }

    EXPECT_EQ(1, mps.number_of_lesser_constraints);
    EXPECT_EQ(1, mps.number_of_equal_constraints);
    EXPECT_EQ(1, mps.number_of_greater_constraints);
}

/*****************************************************************************/
TEST_F(TestMPS, read_mps_01) {
    // Parse a MPS file including the RANGE section with positive range values.
    mps::MPS mps("./test/dat/mps/test_01.mps");

    auto c_1 = mps.constraints["_C1"];
    EXPECT_EQ(mps::MPSConstraintSense::Greater, c_1.sense);
    EXPECT_EQ(1, c_1.rhs);

    auto c_1_range = mps.constraints["_C1_range"];
    EXPECT_EQ(mps::MPSConstraintSense::Less, c_1_range.sense);
    EXPECT_EQ(1 + 10, c_1_range.rhs);

    auto c_2 = mps.constraints["_C2"];
    EXPECT_EQ(mps::MPSConstraintSense::Less, c_2.sense);
    EXPECT_EQ(2, c_2.rhs);

    auto c_2_range = mps.constraints["_C2_range"];
    EXPECT_EQ(mps::MPSConstraintSense::Greater, c_2_range.sense);
    EXPECT_EQ(2 - 10, c_2_range.rhs);

    auto c_3 = mps.constraints["_C3"];
    EXPECT_EQ(mps::MPSConstraintSense::Greater, c_3.sense);
    EXPECT_EQ(3, c_3.rhs);

    auto c_3_range = mps.constraints["_C3_range"];
    EXPECT_EQ(mps::MPSConstraintSense::Less, c_3_range.sense);
    EXPECT_EQ(3 + 10, c_3_range.rhs);
}

/*****************************************************************************/
TEST_F(TestMPS, read_mps_02) {
    // Parse a MPS file including the RANGE section with negative range values.
    mps::MPS mps("./test/dat/mps/test_02.mps");

    auto c_1 = mps.constraints["_C1"];
    EXPECT_EQ(mps::MPSConstraintSense::Less, c_1.sense);
    EXPECT_EQ(1, c_1.rhs);

    auto c_1_range = mps.constraints["_C1_range"];
    EXPECT_EQ(mps::MPSConstraintSense::Greater, c_1_range.sense);
    EXPECT_EQ(1 + (-10), c_1_range.rhs);

    auto c_2 = mps.constraints["_C2"];
    EXPECT_EQ(mps::MPSConstraintSense::Less, c_2.sense);
    EXPECT_EQ(2, c_2.rhs);

    auto c_2_range = mps.constraints["_C2_range"];
    EXPECT_EQ(mps::MPSConstraintSense::Greater, c_2_range.sense);
    EXPECT_EQ(2 - 10, c_2_range.rhs);

    auto c_3 = mps.constraints["_C3"];
    EXPECT_EQ(mps::MPSConstraintSense::Greater, c_3.sense);
    EXPECT_EQ(3, c_3.rhs);

    auto c_3_range = mps.constraints["_C3_range"];
    EXPECT_EQ(mps::MPSConstraintSense::Less, c_3_range.sense);
    EXPECT_EQ(3 + 10, c_3_range.rhs);
}

/*****************************************************************************/
TEST_F(TestMPS, read_mps_03) {
    // Parse a MPS file including variables not appear in COLUMNS section but in
    // the BOUNDS section.
    mps::MPS mps("./test/dat/mps/test_03.mps");
    EXPECT_EQ("problem", mps.name);

    auto x = mps.variables["x"];
    EXPECT_EQ("x", x.name);
    EXPECT_EQ(mps::MPSVariableSense::Continuous, x.sense);
}

/*****************************************************************************/
TEST_F(TestMPS, read_mps_04) {
    // Parse a MPS file without problem name.
    mps::MPS mps("./test/dat/mps/test_04.mps");
    EXPECT_EQ("", mps.name);
}

/*****************************************************************************/
TEST_F(TestMPS, read_mps_05) {
    // Parse a MPS file including the OBJNAME section.
    {
        mps::MPS mps("./test/dat/mps/test_05a.mps");
        EXPECT_EQ("foo", mps.objective.name);
    }
    {
        // NG case
        mps::MPS mps;
        ASSERT_THROW(mps.read_mps("./test/dat/mps/test_05b.mps"),
                     std::runtime_error);
    }
}

/*****************************************************************************/
TEST_F(TestMPS, read_mps_06a) {
    // Parse a MPS file including the OBJSENSE section for minimization.
    {
        mps::MPS mps("./test/dat/mps/test_06a.mps");
        EXPECT_TRUE(mps.is_minimization);
    }

    {
        mps::MPS mps("./test/dat/mps/test_06b.mps");
        EXPECT_TRUE(mps.is_minimization);
    }

    {
        mps::MPS mps("./test/dat/mps/test_06c.mps");
        EXPECT_TRUE(mps.is_minimization);
    }

    {
        mps::MPS mps("./test/dat/mps/test_06d.mps");
        EXPECT_TRUE(mps.is_minimization);
    }

    {
        mps::MPS mps("./test/dat/mps/test_06e.mps");
        EXPECT_TRUE(mps.is_minimization);
    }

    {
        mps::MPS mps("./test/dat/mps/test_06f.mps");
        EXPECT_TRUE(mps.is_minimization);
    }
}

/*****************************************************************************/
TEST_F(TestMPS, read_mps_07a) {
    // Parse a MPS file including the OBJSENSE section for maximization.
    {
        mps::MPS mps("./test/dat/mps/test_07a.mps");
        EXPECT_FALSE(mps.is_minimization);
    }

    {
        mps::MPS mps("./test/dat/mps/test_07b.mps");
        EXPECT_FALSE(mps.is_minimization);
    }

    {
        mps::MPS mps("./test/dat/mps/test_07c.mps");
        EXPECT_FALSE(mps.is_minimization);
    }

    {
        mps::MPS mps("./test/dat/mps/test_07d.mps");
        EXPECT_FALSE(mps.is_minimization);
    }

    {
        mps::MPS mps("./test/dat/mps/test_07e.mps");
        EXPECT_FALSE(mps.is_minimization);
    }

    {
        mps::MPS mps("./test/dat/mps/test_07f.mps");
        EXPECT_FALSE(mps.is_minimization);
    }
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/