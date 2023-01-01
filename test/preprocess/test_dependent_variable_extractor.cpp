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
class TestDependentVariableExtractor : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestDependentVariableExtractor, extract) {
    /// case 01
    {
        model::Model<int, double> model;

        auto& x = model.create_variable("x", 0, 1);
        auto& y = model.create_variable("y", 0, 1);
        auto& z = model.create_variable("z", -10, 10);
        auto& w = model.create_variable("w", -100, 100);
        auto& v = model.create_variable("v", -1000, 1000);

        auto& f = model.create_constraint("f", z == 2 * x + y);
        auto& g = model.create_constraint("g", w == 3 * x + 4 * y + 5 * z);
        auto& h = model.create_constraint("h", v == 6 * z + 7 * w);
        model.minimize(w);
        model.setup_unique_names();
        model.setup_structure();

        EXPECT_TRUE(f(0).is_intermediate());
        EXPECT_TRUE(g(0).is_intermediate());
        EXPECT_TRUE(h(0).is_intermediate());

        preprocess::DependentVariableExtractor<int, double>
            dependent_variable_extractor(&model);

        /// Extracting (Round 1)
        {
            option::Option option;
            dependent_variable_extractor.extract(option, false);

            model.setup_structure();

            EXPECT_EQ(model_component::VariableSense::DependentInteger,
                      z(0).sense());
            EXPECT_FALSE(f.is_enabled());

            EXPECT_EQ(model_component::VariableSense::DependentInteger,
                      w(0).sense());
            EXPECT_FALSE(g.is_enabled());

            EXPECT_EQ(model_component::VariableSense::DependentInteger,
                      v(0).sense());
            EXPECT_FALSE(h.is_enabled());
        }

        /// Eliminating (Round 1-1)
        {
            dependent_variable_extractor.eliminate(false);

            model.setup_structure();

            auto& sensitivities_objective =
                model.objective().expression().sensitivities();

            EXPECT_EQ(3, sensitivities_objective.at(&x(0)));
            EXPECT_EQ(4, sensitivities_objective.at(&y(0)));
            EXPECT_EQ(5, sensitivities_objective.at(&z(0)));
        }

        /// Eliminating (Round 1-2)
        {
            dependent_variable_extractor.eliminate(false);

            model.setup_structure();

            auto& sensitivities_objective =
                model.objective().expression().sensitivities();

            EXPECT_EQ(13, sensitivities_objective.at(&x(0)));
            EXPECT_EQ(9, sensitivities_objective.at(&y(0)));

            auto& sensitivities_w =
                w(0).dependent_expression_ptr()->sensitivities();

            EXPECT_EQ(13, sensitivities_w.at(&x(0)));
            EXPECT_EQ(9, sensitivities_w.at(&y(0)));

            auto& sensitivities_v =
                v(0).dependent_expression_ptr()->sensitivities();

            EXPECT_EQ(103, sensitivities_v.at(&x(0)));
            EXPECT_EQ(69, sensitivities_v.at(&y(0)));
        }
    }

    /// case 02
    {
        model::Model<int, double> model;

        auto& x = model.create_variable("x", 0, 1);
        auto& y = model.create_variable("y", 0, 1);
        auto& z = model.create_variable("z", -100, 100);
        auto& w = model.create_variable("w", -90, 90);

        auto& f = model.create_constraint("f", z == 2 * x + y);
        auto& g = model.create_constraint("g", w == 3 * x + 4 * y + 5 * z);
        model.minimize(w);
        model.setup_unique_names();
        model.setup_structure();

        EXPECT_TRUE(f(0).is_intermediate());
        EXPECT_TRUE(g(0).is_intermediate());

        preprocess::DependentVariableExtractor<int, double>
            dependent_variable_extractor(&model);

        /// Extracting (Round 1)
        {
            option::Option option;
            dependent_variable_extractor.extract(option, false);

            model.setup_structure();

            EXPECT_EQ(model_component::VariableSense::DependentInteger,
                      z(0).sense());
            EXPECT_FALSE(f.is_enabled());

            EXPECT_EQ(model_component::VariableSense::DependentInteger,
                      w(0).sense());
            EXPECT_FALSE(g.is_enabled());

            auto& constraint_proxies = model.constraint_proxies();
            EXPECT_EQ(3, static_cast<int>(constraint_proxies.size()));
            EXPECT_EQ(2, static_cast<int>(constraint_proxies.back()
                                              .flat_indexed_constraints()
                                              .size()));
            {
                auto& additional_sensitivities =
                    constraint_proxies.back()
                        .flat_indexed_constraints(0)
                        .expression()
                        .sensitivities();
                EXPECT_EQ(3, additional_sensitivities.at(&x(0)));
                EXPECT_EQ(4, additional_sensitivities.at(&y(0)));
                EXPECT_EQ(5, additional_sensitivities.at(&z(0)));
            }
            {
                auto& additional_sensitivities =
                    constraint_proxies.back()
                        .flat_indexed_constraints(1)
                        .expression()
                        .sensitivities();
                EXPECT_EQ(3, additional_sensitivities.at(&x(0)));
                EXPECT_EQ(4, additional_sensitivities.at(&y(0)));
                EXPECT_EQ(5, additional_sensitivities.at(&z(0)));
            }
        }

        /// Eliminating (Round 1-1)
        {
            dependent_variable_extractor.eliminate(false);

            model.setup_structure();

            auto& sensitivities_objective =
                model.objective().expression().sensitivities();

            EXPECT_EQ(3, sensitivities_objective.at(&x(0)));
            EXPECT_EQ(4, sensitivities_objective.at(&y(0)));
            EXPECT_EQ(5, sensitivities_objective.at(&z(0)));

            auto& sensitivities_w =
                w(0).dependent_expression_ptr()->sensitivities();

            EXPECT_EQ(13, sensitivities_w.at(&x(0)));
            EXPECT_EQ(9, sensitivities_w.at(&y(0)));

            auto& constraint_proxies = model.constraint_proxies();
            {
                auto& additional_sensitivities =
                    constraint_proxies.back()
                        .flat_indexed_constraints(0)
                        .expression()
                        .sensitivities();
                EXPECT_EQ(13, additional_sensitivities.at(&x(0)));
                EXPECT_EQ(9, additional_sensitivities.at(&y(0)));
            }
            {
                auto& additional_sensitivities =
                    constraint_proxies.back()
                        .flat_indexed_constraints(1)
                        .expression()
                        .sensitivities();
                EXPECT_EQ(13, additional_sensitivities.at(&x(0)));
                EXPECT_EQ(9, additional_sensitivities.at(&y(0)));
            }
        }
    }
}

/*****************************************************************************/
TEST_F(TestDependentVariableExtractor, eliminate) {
    /// This method is tested in extract_integer_variables().
}
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/