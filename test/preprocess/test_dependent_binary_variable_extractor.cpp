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
class TestDependentBinaryVariableExtractor : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestDependentBinaryVariableExtractor, run) {
    /// case 01
    {
        model::Model<int, double> model;

        auto& x = model.create_variables("x", 5, 0, 1);
        auto& y = model.create_variable("y", 0, 1);

        auto& f = model.create_constraint("f", 5 * y == x.sum());
        model.minimize(x.sum());
        model.builder().setup_unique_names();
        model.builder().update_derived_components();

        EXPECT_TRUE(
            f(0).is_type(model_component::ConstraintType::AllOrNothing));

        preprocess::DependentBinaryVariableExtractor<int, double>
            dependent_binary_variable_extractor(&model);
        preprocess::DependentVariableEliminator<int, double>
            dependent_variable_eliminator(&model);

        /// Extracting
        {
            option::Option option;
            option.preprocess.is_enabled_extract_dependent_all_or_nothing =
                true;
            dependent_binary_variable_extractor.run(option, true);
            model.builder().update_derived_components();

            EXPECT_EQ(model_component::VariableType::Binary, x(0).type());
            EXPECT_EQ(model_component::VariableType::DependentBinary,
                      x(1).type());
            EXPECT_EQ(model_component::VariableType::DependentBinary,
                      x(2).type());
            EXPECT_EQ(model_component::VariableType::DependentBinary,
                      x(3).type());
            EXPECT_EQ(model_component::VariableType::DependentBinary,
                      x(4).type());
            EXPECT_EQ(model_component::VariableType::DependentBinary,
                      y(0).type());
            EXPECT_FALSE(f.is_enabled());
        }

        /// Eliminating
        {
            dependent_variable_eliminator.run(false);

            model.builder().update_derived_components();

            auto& sensitivities_objective =
                model.objective().expression().sensitivities();

            EXPECT_EQ(5, sensitivities_objective.at(&x(0)));
        }
    }

    // case 02 (Infeasible)
    {
        model::Model<int, double> model;

        auto& x = model.create_variables("x", 5, 0, 1);
        auto& f = model.create_constraints("f", 5);

        f(0) = x(0) == x(1);
        f(1) = x(1) == x(2);
        f(2) = x(2) == x(3);
        f(3) = x(3) == x(4);
        f(4) = x(4) == 1 - x(0);

        model.builder().setup_unique_names();
        model.builder().update_derived_components();

        preprocess::DependentBinaryVariableExtractor<int, double>
            dependent_binary_variable_extractor(&model);

        /// Extracting
        {
            option::Option option;
            option.preprocess.is_enabled_extract_dependent_all_or_nothing =
                true;
            EXPECT_THROW(dependent_binary_variable_extractor.run(option, false),
                         error_handler::InfeasibleError);
        }
    }
}
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/