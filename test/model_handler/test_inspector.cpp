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
class TestInspector : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown(void) {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestInspector, initialize_and_setup) {
    model::Model<int, double>            model;
    model_handler::Inspector<int, double> inspector;

    EXPECT_FALSE(inspector.compute_is_all_binary_variables());
    EXPECT_FALSE(inspector.compute_is_all_integer_coefficients());
    EXPECT_FALSE(inspector.compute_is_monotone());

    inspector.setup(&model);
    model.builder().update_derived_components();
    EXPECT_TRUE(inspector.compute_is_all_binary_variables());
    EXPECT_TRUE(inspector.compute_is_all_integer_coefficients());
    EXPECT_TRUE(inspector.compute_is_monotone());
}

/*****************************************************************************/
TEST_F(TestInspector, compute_is_all_binary_variables) {
    /// Only binary variables
    {
        model::Model<int, double> model;
        model.create_variables("x", 2, 0, 1);

        EXPECT_TRUE(model.inspector().compute_is_all_binary_variables());
    }

    /// Including selection and dependent binary variables
    {
        model::Model<int, double> model;
        auto& x = model.create_variables("x", 2, 0, 1);
        x(0).set_type(model_component::VariableType::Selection);
        x(1).set_type(model_component::VariableType::DependentBinary);

        EXPECT_TRUE(model.inspector().compute_is_all_binary_variables());
    }

    /// Including general integer variable
    {
        model::Model<int, double> model;
        model.create_variables("x", 2, 0, 1);
        model.create_variable("y", 0, 10);

        EXPECT_FALSE(model.inspector().compute_is_all_binary_variables());
    }
}

/*****************************************************************************/
TEST_F(TestInspector, compute_is_all_integer_coefficients) {
    /// All integer coefficients
    {
        model::Model<int, double> model;
        auto& x = model.create_variables("x", 2, 0, 1);
        auto& g = model.create_constraint("g", x(0) + 2 * x(1) <= 3);

        g(0).update_structure();
        EXPECT_TRUE(model.inspector().compute_is_all_integer_coefficients());
    }

    /// Including non-integer coefficient
    {
        model::Model<int, double> model;
        auto& x = model.create_variables("x", 2, 0, 1);
        auto& g = model.create_constraint("g", 1.5 * x(0) + x(1) <= 3);

        g(0).update_structure();
        EXPECT_FALSE(model.inspector().compute_is_all_integer_coefficients());
    }

    /// Non-integer coefficient in disabled constraint
    {
        model::Model<int, double> model;
        auto& x = model.create_variables("x", 2, 0, 1);
        auto& g1 = model.create_constraint("g1", x(0) + x(1) <= 1);
        auto& g2 = model.create_constraint("g2", 1.5 * x(0) + x(1) <= 3);
        g2.disable();

        g1(0).update_structure();
        g2(0).update_structure();
        EXPECT_TRUE(model.inspector().compute_is_all_integer_coefficients());
    }
}

/*****************************************************************************/
TEST_F(TestInspector, compute_is_monotone) {
    /// No variables / No constraints
    {
        model::Model<int, double> model;
        model.builder().update_derived_components();
        EXPECT_TRUE(model.inspector().compute_is_monotone());
    }

    /// All greater-or-equal monotone
    {
        model::Model<int, double> model;
        auto& x0 = model.create_variable("x0", 0, 1);
        auto& x1 = model.create_variable("x1", 0, 1);
        auto& x2 = model.create_variable("x2", 0, 1);

        model.create_constraint("g1", 2 * x0 + 3 * x1 >= 1);
        model.create_constraint("g2", x1 + 4 * x2 >= 2);

        model.builder().update_derived_components();
        EXPECT_TRUE(model.inspector().compute_is_monotone());
    }

    /// Mixed senses monotone (<= and >=)
    {
        model::Model<int, double> model;
        auto& x0 = model.create_variable("x0", 0, 1);
        auto& x1 = model.create_variable("x1", 0, 1);

        model.create_constraint("g1", x0 - 2 * x1 >= 0);
        model.create_constraint("g2", -3 * x0 + x1 <= 0);

        model.builder().update_derived_components();
        EXPECT_TRUE(model.inspector().compute_is_monotone());
    }

    /// General integer variables monotone
    {
        model::Model<int, double> model;
        auto& y = model.create_variable("y", -100, 100);

        model.create_constraint("g1", y >= 1);
        model.create_constraint("g2", 3 * y >= 10);

        model.builder().update_derived_components();
        EXPECT_TRUE(model.inspector().compute_is_monotone());
    }

    /// Conflicting signs not monotone
    {
        model::Model<int, double> model;
        auto& x0 = model.create_variable("x0", 0, 1);

        model.create_constraint("g1", x0 >= 1);
        model.create_constraint("g2", x0 <= 0);

        model.builder().update_derived_components();
        EXPECT_FALSE(model.inspector().compute_is_monotone());
    }

    /// Equality constraint not monotone
    {
        model::Model<int, double> model;
        auto& x0 = model.create_variable("x0", 0, 1);
        auto& x1 = model.create_variable("x1", 0, 1);

        model.create_constraint("g1", x0 + x1 == 1);

        model.builder().update_derived_components();
        EXPECT_FALSE(model.inspector().compute_is_monotone());
    }

    /// Disabled constraint ignored
    {
        model::Model<int, double> model;
        auto& x0 = model.create_variable("x0", 0, 1);

        model.create_constraint("g1", x0 >= 1);
        auto& g2 = model.create_constraint("g2", x0 <= 0);
        g2.disable();

        model.builder().update_derived_components();
        EXPECT_TRUE(model.inspector().compute_is_monotone());
    }
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/
