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
TEST_F(TestDependentVariableExtractor, create_enable_map) {
    using namespace model_component;
    {
        option::Option option;
        auto&          preprocess = option.preprocess;
        preprocess.is_enabled_extract_dependent_exclusive_or          = false;
        preprocess.is_enabled_extract_dependent_exclusive_nor         = false;
        preprocess.is_enabled_extract_dependent_inverted_integers     = false;
        preprocess.is_enabled_extract_dependent_balanced_integers     = false;
        preprocess.is_enabled_extract_dependent_constant_sum_integers = false;
        preprocess.is_enabled_extract_dependent_constant_difference_integers =
            false;
        preprocess.is_enabled_extract_dependent_constant_ratio_integers = false;
        preprocess.is_enabled_extract_dependent_trinomial_exclusive_nor = false;
        preprocess.is_enabled_extract_dependent_all_or_nothing          = false;
        preprocess.is_enabled_extract_dependent_intermediate            = false;

        auto enable_map = preprocess::DependentVariableExtractor<
            int, double>::create_enable_map(option);

        EXPECT_FALSE(enable_map[ConstraintType::ExclusiveOR]);
        EXPECT_FALSE(enable_map[ConstraintType::ExclusiveNOR]);
        EXPECT_FALSE(enable_map[ConstraintType::InvertedIntegers]);
        EXPECT_FALSE(enable_map[ConstraintType::BalancedIntegers]);
        EXPECT_FALSE(enable_map[ConstraintType::ConstantSumIntegers]);
        EXPECT_FALSE(enable_map[ConstraintType::ConstantDifferenceIntegers]);
        EXPECT_FALSE(enable_map[ConstraintType::ConstantRatioIntegers]);
        EXPECT_FALSE(enable_map[ConstraintType::TrinomialExclusiveNOR]);
        EXPECT_FALSE(enable_map[ConstraintType::AllOrNothing]);
        EXPECT_FALSE(enable_map[ConstraintType::Intermediate]);
    }

    {
        option::Option option;
        auto&          preprocess = option.preprocess;
        preprocess.is_enabled_extract_dependent_exclusive_or          = true;
        preprocess.is_enabled_extract_dependent_exclusive_nor         = true;
        preprocess.is_enabled_extract_dependent_inverted_integers     = true;
        preprocess.is_enabled_extract_dependent_balanced_integers     = true;
        preprocess.is_enabled_extract_dependent_constant_sum_integers = true;
        preprocess.is_enabled_extract_dependent_constant_difference_integers =
            true;
        preprocess.is_enabled_extract_dependent_constant_ratio_integers = true;
        preprocess.is_enabled_extract_dependent_trinomial_exclusive_nor = true;
        preprocess.is_enabled_extract_dependent_all_or_nothing          = true;
        preprocess.is_enabled_extract_dependent_intermediate            = true;

        auto enable_map = preprocess::DependentVariableExtractor<
            int, double>::create_enable_map(option);

        EXPECT_TRUE(enable_map[ConstraintType::ExclusiveOR]);
        EXPECT_TRUE(enable_map[ConstraintType::ExclusiveNOR]);
        EXPECT_TRUE(enable_map[ConstraintType::InvertedIntegers]);
        EXPECT_TRUE(enable_map[ConstraintType::BalancedIntegers]);
        EXPECT_TRUE(enable_map[ConstraintType::ConstantSumIntegers]);
        EXPECT_TRUE(enable_map[ConstraintType::ConstantDifferenceIntegers]);
        EXPECT_TRUE(enable_map[ConstraintType::ConstantRatioIntegers]);
        EXPECT_TRUE(enable_map[ConstraintType::TrinomialExclusiveNOR]);
        EXPECT_TRUE(enable_map[ConstraintType::AllOrNothing]);
        EXPECT_TRUE(enable_map[ConstraintType::Intermediate]);
    }
}

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
        model.builder().setup_unique_names();
        model.builder().setup_structure();

        EXPECT_TRUE(
            f(0).is_type(model_component::ConstraintType::Intermediate));
        EXPECT_TRUE(
            g(0).is_type(model_component::ConstraintType::Intermediate));
        EXPECT_TRUE(
            h(0).is_type(model_component::ConstraintType::Intermediate));

        preprocess::DependentVariableExtractor<int, double>
            dependent_variable_extractor(&model);
        preprocess::DependentVariableEliminator<int, double>
            dependent_variable_eliminator(&model);

        /// Extracting (Round 1)
        {
            option::Option option;
            dependent_variable_extractor.extract(option, false);

            model.builder().setup_structure();

            EXPECT_EQ(model_component::VariableType::DependentInteger,
                      z(0).type());
            EXPECT_FALSE(f.is_enabled());

            EXPECT_EQ(model_component::VariableType::DependentInteger,
                      w(0).type());
            EXPECT_FALSE(g.is_enabled());

            EXPECT_EQ(model_component::VariableType::DependentInteger,
                      v(0).type());
            EXPECT_FALSE(h.is_enabled());
        }

        /// Eliminating (Round 1-1)
        {
            dependent_variable_eliminator.eliminate(false);

            model.builder().setup_structure();

            auto& sensitivities_objective =
                model.objective().expression().sensitivities();

            EXPECT_EQ(3, sensitivities_objective.at(&x(0)));
            EXPECT_EQ(4, sensitivities_objective.at(&y(0)));
            EXPECT_EQ(5, sensitivities_objective.at(&z(0)));
        }

        /// Eliminating (Round 1-2)
        {
            dependent_variable_eliminator.eliminate(false);

            model.builder().setup_structure();

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
        model.builder().setup_unique_names();
        model.builder().setup_structure();

        EXPECT_TRUE(
            f(0).is_type(model_component::ConstraintType::Intermediate));
        EXPECT_TRUE(
            g(0).is_type(model_component::ConstraintType::Intermediate));

        preprocess::DependentVariableExtractor<int, double>
            dependent_variable_extractor(&model);
        preprocess::DependentVariableEliminator<int, double>
            dependent_variable_eliminator(&model);

        /// Extracting (Round 1)
        {
            option::Option option;
            dependent_variable_extractor.extract(option, false);

            model.builder().setup_structure();

            EXPECT_EQ(model_component::VariableType::DependentInteger,
                      z(0).type());
            EXPECT_FALSE(f.is_enabled());

            EXPECT_EQ(model_component::VariableType::DependentInteger,
                      w(0).type());
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
            dependent_variable_eliminator.eliminate(false);

            model.builder().setup_structure();

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

    /// case 03
    {
        model::Model<int, double> model;

        auto& x = model.create_variables("x", 5, 0, 1);
        auto& y = model.create_variable("y", 0, 1);

        auto& f = model.create_constraint("f", 5 * y == x.sum());
        model.minimize(x.sum());
        model.builder().setup_unique_names();
        model.builder().setup_structure();

        EXPECT_TRUE(
            f(0).is_type(model_component::ConstraintType::AllOrNothing));

        preprocess::DependentVariableExtractor<int, double>
            dependent_variable_extractor(&model);
        preprocess::DependentVariableEliminator<int, double>
            dependent_variable_eliminator(&model);

        /// Extracting
        {
            option::Option option;
            option.preprocess.is_enabled_extract_dependent_all_or_nothing =
                true;
            dependent_variable_extractor.extract(option, false);

            model.builder().setup_structure();

            EXPECT_EQ(model_component::VariableType::DependentBinary,
                      x(0).type());
            EXPECT_EQ(model_component::VariableType::DependentBinary,
                      x(1).type());
            EXPECT_EQ(model_component::VariableType::DependentBinary,
                      x(2).type());
            EXPECT_EQ(model_component::VariableType::DependentBinary,
                      x(3).type());
            EXPECT_EQ(model_component::VariableType::DependentBinary,
                      x(4).type());
            EXPECT_FALSE(f.is_enabled());
        }

        /// Eliminating
        {
            dependent_variable_eliminator.eliminate(false);

            model.builder().setup_structure();

            auto& sensitivities_objective =
                model.objective().expression().sensitivities();

            EXPECT_EQ(5, sensitivities_objective.at(&y(0)));
        }
    }
}

/*****************************************************************************/
TEST_F(TestDependentVariableExtractor, eliminate) {
    /// This test is covered by extract_integer_variables().
}
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/