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
}

/*****************************************************************************/
TEST_F(TestMPS, parse_value) {
    mps::MPS mps;
    EXPECT_FLOAT_EQ(0.0, mps.parse_value("0"));
    EXPECT_FLOAT_EQ(1.0, mps.parse_value("1"));
    EXPECT_FLOAT_EQ(-1.0, mps.parse_value("-1"));
    EXPECT_FLOAT_EQ(100.0, mps.parse_value("100"));
    EXPECT_FLOAT_EQ(-100.0, mps.parse_value("-100"));
    EXPECT_FLOAT_EQ(0.1, mps.parse_value("0.1"));
    EXPECT_FLOAT_EQ(-0.1, mps.parse_value("-0.1"));
    EXPECT_FLOAT_EQ(1E10, mps.parse_value("1E10"));
    EXPECT_FLOAT_EQ(-1E10, mps.parse_value("-1E10"));
    EXPECT_FLOAT_EQ(1E-10, mps.parse_value("1E-10"));
    EXPECT_FLOAT_EQ(-1E-10, mps.parse_value("-1E-10"));
    EXPECT_FLOAT_EQ(3.14159, mps.parse_value("3.14159"));
    EXPECT_FLOAT_EQ(-3.14159, mps.parse_value("-3.14159"));
    ASSERT_THROW(mps.parse_value("hoge"), std::runtime_error);
}

/*****************************************************************************/
TEST_F(TestMPS, parse_name) {
    {
        mps::MPS                      mps;
        std::vector<std::string_view> items = {"NANE", "problem"};
        mps.parse_name(items);
        EXPECT_EQ("problem", mps.name);
    }

    {
        mps::MPS                      mps;
        std::vector<std::string_view> items = {"problem"};
        mps.parse_name(items);
        EXPECT_EQ("problem", mps.name);
    }

    {
        mps::MPS                      mps;
        std::vector<std::string_view> items = {"NANE", "problem", "extra"};
        ASSERT_THROW(mps.parse_name(items), std::runtime_error);
    }
}

/*****************************************************************************/
TEST_F(TestMPS, parse_objsense) {
    {
        mps::MPS                      mps;
        std::vector<std::string_view> items = {"OBJSENSE", "minimization"};
        mps.parse_objsense(items);
        EXPECT_TRUE(mps.objective.is_minimization);
    }

    {
        mps::MPS                      mps;
        std::vector<std::string_view> items = {"OBJSENSE", "minimize"};
        mps.parse_objsense(items);
        EXPECT_TRUE(mps.objective.is_minimization);
    }

    {
        mps::MPS                      mps;
        std::vector<std::string_view> items = {"OBJSENSE", "min"};
        mps.parse_objsense(items);
        EXPECT_TRUE(mps.objective.is_minimization);
    }

    {
        mps::MPS                      mps;
        std::vector<std::string_view> items = {"minimization"};
        mps.parse_objsense(items);
        EXPECT_TRUE(mps.objective.is_minimization);
    }

    {
        mps::MPS                      mps;
        std::vector<std::string_view> items = {"minimize"};
        mps.parse_objsense(items);
        EXPECT_TRUE(mps.objective.is_minimization);
    }

    {
        mps::MPS                      mps;
        std::vector<std::string_view> items = {"min"};
        mps.parse_objsense(items);
        EXPECT_TRUE(mps.objective.is_minimization);
    }

    {
        mps::MPS                      mps;
        std::vector<std::string_view> items = {"OBJSENSE", "maximization"};
        mps.parse_objsense(items);
        EXPECT_FALSE(mps.objective.is_minimization);
    }

    {
        mps::MPS                      mps;
        std::vector<std::string_view> items = {"OBJSENSE", "maximize"};
        mps.parse_objsense(items);
        EXPECT_FALSE(mps.objective.is_minimization);
    }

    {
        mps::MPS                      mps;
        std::vector<std::string_view> items = {"OBJSENSE", "max"};
        mps.parse_objsense(items);
        EXPECT_FALSE(mps.objective.is_minimization);
    }

    {
        mps::MPS                      mps;
        std::vector<std::string_view> items = {"maximization"};
        mps.parse_objsense(items);
        EXPECT_FALSE(mps.objective.is_minimization);
    }

    {
        mps::MPS                      mps;
        std::vector<std::string_view> items = {"maximize"};
        mps.parse_objsense(items);
        EXPECT_FALSE(mps.objective.is_minimization);
    }

    {
        mps::MPS                      mps;
        std::vector<std::string_view> items = {"max"};
        mps.parse_objsense(items);
        EXPECT_FALSE(mps.objective.is_minimization);
    }

    {
        mps::MPS                      mps;
        std::vector<std::string_view> items = {"OBJSENSE", "minimization",
                                               "extra"};
        ASSERT_THROW(mps.parse_name(items), std::runtime_error);
    }
}

/*****************************************************************************/
TEST_F(TestMPS, parse_objname) {
    {
        mps::MPS                      mps;
        std::vector<std::string_view> items = {"OBJNAME", "obj"};
        mps.parse_objname(items);
        EXPECT_EQ("obj", mps.objective.name);
    }

    {
        mps::MPS                      mps;
        std::vector<std::string_view> items = {"obj"};
        mps.parse_objname(items);
        EXPECT_EQ("obj", mps.objective.name);
    }

    {
        mps::MPS                      mps;
        std::vector<std::string_view> items = {"OBJNAME", "obj", "extra"};
        ASSERT_THROW(mps.parse_objname(items), std::runtime_error);
    }
}

/*****************************************************************************/
TEST_F(TestMPS, parse_rows) {
    {
        mps::MPS                      mps;
        std::vector<std::string_view> items = {"E", "c1"};
        mps.parse_rows(items);
        EXPECT_EQ("c1", mps.constraints["c1"].name);
        EXPECT_EQ(mps::MPSConstraintSense::Equal, mps.constraints["c1"].sense);
        EXPECT_TRUE(mps.objective.name.empty());
    }
    {
        mps::MPS                      mps;
        std::vector<std::string_view> items = {"L", "c2"};
        mps.parse_rows(items);
        EXPECT_EQ("c2", mps.constraints["c2"].name);
        EXPECT_EQ(mps::MPSConstraintSense::Less, mps.constraints["c2"].sense);
        EXPECT_TRUE(mps.objective.name.empty());
    }

    {
        mps::MPS                      mps;
        std::vector<std::string_view> items = {"G", "c3"};
        mps.parse_rows(items);
        EXPECT_EQ("c3", mps.constraints["c3"].name);
        EXPECT_EQ(mps::MPSConstraintSense::Greater,
                  mps.constraints["c3"].sense);
        EXPECT_TRUE(mps.objective.name.empty());
    }

    {
        mps::MPS                      mps;
        std::vector<std::string_view> items = {"N", "obj"};
        mps.parse_rows(items);
        EXPECT_EQ("obj", mps.objective.name);
        EXPECT_TRUE(mps.constraints.find("obj") == mps.constraints.end());
    }

    {
        mps::MPS                      mps;
        std::vector<std::string_view> items = {"N", "obj", "extra"};
        ASSERT_THROW(mps.parse_rows(items), std::runtime_error);
    }

    {
        mps::MPS                      mps;
        std::vector<std::string_view> items = {"X", "c4"};
        ASSERT_THROW(mps.parse_rows(items), std::runtime_error);
    }

    {
        mps::MPS                      mps;
        std::vector<std::string_view> items = {"E"};
        ASSERT_THROW(mps.parse_rows(items), std::runtime_error);
    }

    {
        mps::MPS                      mps;
        std::vector<std::string_view> items = {"E", "c1", "extra"};
        ASSERT_THROW(mps.parse_rows(items), std::runtime_error);
    }
}

/*****************************************************************************/
TEST_F(TestMPS, parse_columns) {
    {
        mps::MPS mps;

        mps::MPSVariableSense variable_sense =
            mps::MPSVariableSense::Continuous;

        std::vector<std::string_view> items;
        items = {"E", "c1"};
        mps.parse_rows(items);

        items = {"E", "c2"};
        mps.parse_rows(items);

        items = {"x1", "c1", "1.0", "c2", "2.0"};
        mps.parse_columns(items, &variable_sense);

        EXPECT_EQ("x1", mps.variables["x1"].name);
        EXPECT_EQ(mps::MPSVariableSense::Continuous, mps.variables["x1"].sense);
        EXPECT_FLOAT_EQ(1.0, mps.constraints["c1"].sensitivities["x1"]);
        EXPECT_FLOAT_EQ(2.0, mps.constraints["c2"].sensitivities["x1"]);
    }

    {
        mps::MPS mps;

        mps::MPSVariableSense variable_sense =
            mps::MPSVariableSense::Continuous;

        std::vector<std::string_view> items;
        items = {"E", "c1"};
        mps.parse_rows(items);

        items = {"E", "c2"};
        mps.parse_rows(items);

        items = {"MARK", "'MARKER'", "'INTORG'"};
        mps.parse_columns(items, &variable_sense);

        items = {"x1", "c1", "1.0", "c2", "2.0"};
        mps.parse_columns(items, &variable_sense);

        items = {"MARK", "'MARKER'", "'INTEND'"};
        mps.parse_columns(items, &variable_sense);

        items = {"x2", "c1", "1.0", "c2", "2.0"};
        mps.parse_columns(items, &variable_sense);

        EXPECT_EQ("x1", mps.variables["x1"].name);
        EXPECT_EQ(mps::MPSVariableSense::Integer, mps.variables["x1"].sense);
        EXPECT_FLOAT_EQ(1.0, mps.constraints["c1"].sensitivities["x1"]);
        EXPECT_FLOAT_EQ(2.0, mps.constraints["c2"].sensitivities["x1"]);

        EXPECT_EQ("x2", mps.variables["x2"].name);
        EXPECT_EQ(mps::MPSVariableSense::Continuous, mps.variables["x2"].sense);
        EXPECT_FLOAT_EQ(1.0, mps.constraints["c1"].sensitivities["x2"]);
        EXPECT_FLOAT_EQ(2.0, mps.constraints["c2"].sensitivities["x2"]);
    }

    {
        mps::MPS              mps;
        mps::MPSVariableSense variable_sense =
            mps::MPSVariableSense::Continuous;

        std::vector<std::string_view> items;
        items = {"E", "c1"};
        mps.parse_rows(items);

        items = {"E", "c2"};
        mps.parse_rows(items);

        items = {"x1", "c1", "1.0", "c2"};
        ASSERT_THROW(mps.parse_columns(items, &variable_sense),
                     std::runtime_error);
    }

    {
        mps::MPS              mps;
        mps::MPSVariableSense variable_sense =
            mps::MPSVariableSense::Continuous;

        std::vector<std::string_view> items;
        items = {"E", "c1"};
        mps.parse_rows(items);

        items = {"E", "c2"};
        mps.parse_rows(items);

        items = {"x1", "c1", "one", "c2", "2.0"};
        ASSERT_THROW(mps.parse_columns(items, &variable_sense),
                     std::runtime_error);
    }

    {
        mps::MPS              mps;
        mps::MPSVariableSense variable_sense =
            mps::MPSVariableSense::Continuous;

        std::vector<std::string_view> items;
        items = {"E", "c1"};
        mps.parse_rows(items);

        items = {"E", "c2"};
        mps.parse_rows(items);

        items = {"x1", "c3", "1.0", "c4", "2.0"};
        ASSERT_THROW(mps.parse_columns(items, &variable_sense),
                     std::runtime_error);
    }
}

/*****************************************************************************/
TEST_F(TestMPS, parse_rhs) {
    {
        mps::MPS                      mps;
        std::vector<std::string_view> items = {"rhs1", "_C1", "10", "_C2",
                                               "20"};
        mps.parse_rhs(items);
        EXPECT_EQ(10, mps.constraints["_C1"].rhs);
        EXPECT_EQ(20, mps.constraints["_C2"].rhs);
    }

    {
        mps::MPS                      mps;
        std::vector<std::string_view> items = {"rhs1", "_C1", "10", "_C2"};
        ASSERT_THROW(mps.parse_rhs(items), std::runtime_error);
    }

    {
        mps::MPS                      mps;
        std::vector<std::string_view> items = {"rhs1", "_C1", "ten", "_C2",
                                               "20"};
        ASSERT_THROW(mps.parse_rhs(items), std::runtime_error);
    }
}

/*****************************************************************************/
TEST_F(TestMPS, parse_range) {
    /// This function is tested in read_mps_01 and read_mps_02 below.
}

/*****************************************************************************/
TEST_F(TestMPS, parse_bounds) {
    {
        mps::MPS                      mps;
        std::vector<std::string_view> items = {"FR", "bnd", "x_0"};
        mps.parse_bounds(items);
        auto x_0 = mps.variables["x_0"];
        EXPECT_EQ("x_0", x_0.name);
        EXPECT_TRUE(x_0.is_bound_defined);
        EXPECT_FALSE(x_0.is_fixed);
        EXPECT_EQ(mps::MPSVariableSense::Continuous, x_0.sense);
        EXPECT_EQ(constant::INT_HALF_MIN, x_0.integer_lower_bound);
        EXPECT_EQ(constant::INT_HALF_MAX, x_0.integer_upper_bound);
        EXPECT_FLOAT_EQ(-HUGE_VAL, x_0.continuous_lower_bound);
        EXPECT_FLOAT_EQ(HUGE_VAL, x_0.continuous_upper_bound);
    }

    {
        mps::MPS                      mps;
        std::vector<std::string_view> items = {"BV", "bnd", "x"};
        mps.parse_bounds(items);
        auto x = mps.variables["x"];
        EXPECT_EQ("x", x.name);
        EXPECT_TRUE(x.is_bound_defined);
        EXPECT_FALSE(x.is_fixed);
        EXPECT_EQ(mps::MPSVariableSense::Integer, x.sense);
        EXPECT_EQ(0, x.integer_lower_bound);
        EXPECT_EQ(1, x.integer_upper_bound);
        EXPECT_FLOAT_EQ(0, x.continuous_lower_bound);
        EXPECT_FLOAT_EQ(1, x.continuous_upper_bound);
    }

    {
        mps::MPS                      mps;
        std::vector<std::string_view> items = {"MI", "bnd", "x"};
        mps.parse_bounds(items);
        auto x = mps.variables["x"];
        EXPECT_EQ("x", x.name);
        EXPECT_TRUE(x.is_bound_defined);
        EXPECT_FALSE(x.is_fixed);
        EXPECT_EQ(mps::MPSVariableSense::Continuous, x.sense);
        EXPECT_EQ(constant::INT_HALF_MIN, x.integer_lower_bound);
        EXPECT_EQ(0, x.integer_upper_bound);
        EXPECT_FLOAT_EQ(-HUGE_VAL, x.continuous_lower_bound);
        EXPECT_FLOAT_EQ(0, x.continuous_upper_bound);
    }

    {
        mps::MPS                      mps;
        std::vector<std::string_view> items = {"PL", "bnd", "x"};
        mps.parse_bounds(items);
        auto x = mps.variables["x"];
        EXPECT_EQ("x", x.name);
        EXPECT_TRUE(x.is_bound_defined);
        EXPECT_FALSE(x.is_fixed);
        EXPECT_EQ(mps::MPSVariableSense::Continuous, x.sense);
        EXPECT_EQ(0, x.integer_lower_bound);
        EXPECT_EQ(constant::INT_HALF_MAX, x.integer_upper_bound);
        EXPECT_FLOAT_EQ(0, x.continuous_lower_bound);
        EXPECT_FLOAT_EQ(HUGE_VAL, x.continuous_upper_bound);
    }

    {
        mps::MPS                      mps;
        std::vector<std::string_view> items = {"LO", "bnd", "x", "0"};
        mps.parse_bounds(items);
        auto x = mps.variables["x"];
        EXPECT_EQ("x", x.name);
        EXPECT_TRUE(x.is_bound_defined);
        EXPECT_FALSE(x.is_fixed);
        EXPECT_EQ(mps::MPSVariableSense::Continuous, x.sense);
        EXPECT_EQ(0, x.integer_lower_bound);
        EXPECT_EQ(constant::INT_HALF_MAX, x.integer_upper_bound);
        EXPECT_FLOAT_EQ(0, x.continuous_lower_bound);
        EXPECT_FLOAT_EQ(HUGE_VAL, x.continuous_upper_bound);
    }

    {
        mps::MPS                      mps;
        std::vector<std::string_view> items = {"LI", "bnd", "x", "0"};
        mps.parse_bounds(items);
        auto x = mps.variables["x"];
        EXPECT_EQ("x", x.name);
        EXPECT_TRUE(x.is_bound_defined);
        EXPECT_FALSE(x.is_fixed);
        EXPECT_EQ(mps::MPSVariableSense::Integer, x.sense);
        EXPECT_EQ(0, x.integer_lower_bound);
        EXPECT_EQ(constant::INT_HALF_MAX, x.integer_upper_bound);
        EXPECT_FLOAT_EQ(0, x.continuous_lower_bound);
        EXPECT_FLOAT_EQ(HUGE_VAL, x.continuous_upper_bound);
    }

    {
        mps::MPS                      mps;
        std::vector<std::string_view> items = {"UP", "bnd", "x", "100"};
        mps.parse_bounds(items);
        auto x = mps.variables["x"];
        EXPECT_EQ("x", x.name);
        EXPECT_TRUE(x.is_bound_defined);
        EXPECT_FALSE(x.is_fixed);
        EXPECT_EQ(mps::MPSVariableSense::Continuous, x.sense);
        EXPECT_EQ(0, x.integer_lower_bound);
        EXPECT_EQ(100, x.integer_upper_bound);
        EXPECT_FLOAT_EQ(0.0, x.continuous_lower_bound);
        EXPECT_FLOAT_EQ(100, x.continuous_upper_bound);
    }

    {
        mps::MPS                      mps;
        std::vector<std::string_view> items = {"UI", "bnd", "x", "100"};
        mps.parse_bounds(items);
        auto x = mps.variables["x"];
        EXPECT_EQ("x", x.name);
        EXPECT_TRUE(x.is_bound_defined);
        EXPECT_FALSE(x.is_fixed);
        EXPECT_EQ(mps::MPSVariableSense::Integer, x.sense);
        EXPECT_EQ(0, x.integer_lower_bound);
        EXPECT_EQ(100, x.integer_upper_bound);
        EXPECT_FLOAT_EQ(0.0, x.continuous_lower_bound);
        EXPECT_FLOAT_EQ(100, x.continuous_upper_bound);
    }

    {
        mps::MPS                      mps;
        std::vector<std::string_view> items = {"FX", "bnd", "x", "10"};
        mps.parse_bounds(items);
        auto x = mps.variables["x"];
        EXPECT_EQ("x", x.name);
        EXPECT_TRUE(x.is_bound_defined);
        EXPECT_TRUE(x.is_fixed);
        EXPECT_EQ(mps::MPSVariableSense::Continuous, x.sense);
        EXPECT_EQ(10, x.integer_lower_bound);
        EXPECT_EQ(10, x.integer_upper_bound);
        EXPECT_FLOAT_EQ(10, x.continuous_lower_bound);
        EXPECT_FLOAT_EQ(10, x.continuous_upper_bound);
    }

    {
        mps::MPS                      mps;
        std::vector<std::string_view> items = {"XX", "bnd", "x", "10"};
        ASSERT_THROW(mps.parse_bounds(items), std::runtime_error);
    }

    {
        mps::MPS                      mps;
        std::vector<std::string_view> items = {"LO", "bnd", "x"};
        ASSERT_THROW(mps.parse_bounds(items), std::runtime_error);
    }

    {
        mps::MPS                      mps;
        std::vector<std::string_view> items = {"LO", "bnd", "x", "zero"};
        ASSERT_THROW(mps.parse_bounds(items), std::runtime_error);
    }
}

/*****************************************************************************/
TEST_F(TestMPS, read_mps_00) {
    // Default case
    mps::MPS mps;
    mps.read_mps("./test/dat/mps/test_00.mps");
    EXPECT_EQ("problem", mps.name);

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
}

/*****************************************************************************/
TEST_F(TestMPS, read_mps_01) {
    // Parse a MPS file including the RANGE section with positive range
    // values.
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
    // Parse a MPS file including the RANGE section with negative range
    // values.
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
    // Parse a MPS file including variables not appear in COLUMNS section
    // but in the BOUNDS section.
    mps::MPS mps("./test/dat/mps/test_03.mps");
    EXPECT_EQ("problem", mps.name);

    auto x = mps.variables["x"];
    EXPECT_EQ("x", x.name);
    EXPECT_EQ(mps::MPSVariableSense::Continuous, x.sense);
}

/*****************************************************************************/
TEST_F(TestMPS, read_mps_04) {
    // Parse a MPS file without problem name.
    {
        mps::MPS mps("./test/dat/mps/test_04a.mps");
        EXPECT_EQ("", mps.name);
    }

    {
        // Parse a MPS file with problem name in another line.
        mps::MPS mps("./test/dat/mps/test_04b.mps");
        EXPECT_EQ("problem", mps.name);
    }
}

/*****************************************************************************/
TEST_F(TestMPS, read_mps_05) {
    // Parse a MPS file including the OBJNAME section.
    {
        mps::MPS mps("./test/dat/mps/test_05a.mps");
        EXPECT_EQ("foo", mps.objective.name);
    }

    // NG case
    {
        mps::MPS mps;
        ASSERT_THROW(mps.read_mps("./test/dat/mps/test_05b.mps"),
                     std::runtime_error);
    }

    // Parse a MPS file including the OBJNAME section in one-line format.
    {
        mps::MPS mps("./test/dat/mps/test_05c.mps");
        EXPECT_EQ("foo", mps.objective.name);
    }
}

/*****************************************************************************/
TEST_F(TestMPS, read_mps_06a) {
    // Parse a MPS file including the OBJSENSE section for minimization.
    {
        mps::MPS mps("./test/dat/mps/test_06a.mps");
        EXPECT_TRUE(mps.objective.is_minimization);
    }

    {
        mps::MPS mps("./test/dat/mps/test_06b.mps");
        EXPECT_TRUE(mps.objective.is_minimization);
    }

    {
        mps::MPS mps("./test/dat/mps/test_06c.mps");
        EXPECT_TRUE(mps.objective.is_minimization);
    }

    {
        mps::MPS mps("./test/dat/mps/test_06d.mps");
        EXPECT_TRUE(mps.objective.is_minimization);
    }

    {
        mps::MPS mps("./test/dat/mps/test_06e.mps");
        EXPECT_TRUE(mps.objective.is_minimization);
    }

    {
        mps::MPS mps("./test/dat/mps/test_06f.mps");
        EXPECT_TRUE(mps.objective.is_minimization);
    }

    // Parse a MPS file including the OBJSENSE section in one-line format.
    {
        mps::MPS mps("./test/dat/mps/test_06g.mps");
        EXPECT_TRUE(mps.objective.is_minimization);
    }
}

/*****************************************************************************/
TEST_F(TestMPS, read_mps_07a) {
    // Parse a MPS file including the OBJSENSE section for maximization.
    {
        mps::MPS mps("./test/dat/mps/test_07a.mps");
        EXPECT_FALSE(mps.objective.is_minimization);
    }

    {
        mps::MPS mps("./test/dat/mps/test_07b.mps");
        EXPECT_FALSE(mps.objective.is_minimization);
    }

    {
        mps::MPS mps("./test/dat/mps/test_07c.mps");
        EXPECT_FALSE(mps.objective.is_minimization);
    }

    {
        mps::MPS mps("./test/dat/mps/test_07d.mps");
        EXPECT_FALSE(mps.objective.is_minimization);
    }

    {
        mps::MPS mps("./test/dat/mps/test_07e.mps");
        EXPECT_FALSE(mps.objective.is_minimization);
    }

    {
        mps::MPS mps("./test/dat/mps/test_07f.mps");
        EXPECT_FALSE(mps.objective.is_minimization);
    }

    // Parse a MPS file including the OBJSENSE section in one-line format.
    {
        mps::MPS mps("./test/dat/mps/test_07g.mps");
        EXPECT_FALSE(mps.objective.is_minimization);
    }
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/