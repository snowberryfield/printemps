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
class TestModelBuilder : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestModelBuilder, build) {
    /// This test is covered by the following submethods.
}

/*****************************************************************************/
TEST_F(TestModelBuilder, setup_structure) {
    /// This test is covered by the following submethods.
}

/*****************************************************************************/
TEST_F(TestModelBuilder, setup_unique_names) {
    model::Model<int, double> model;

    auto& x = model.create_variable("x");
    auto& p = model.create_variables("p", {10});
    auto& g = model.create_variables("g", {20, 30});

    x.set_name("_x");
    p(0).set_name("_p_0");
    p(9).set_name("_p_9");
    g(0, 0).set_name("_g_0_0");
    g(19, 29).set_name("_g_19_29");
    model.builder().setup_unique_names();

    EXPECT_EQ("_x", x.name());
    EXPECT_EQ("_p_0", p(0).name());
    EXPECT_EQ("p[ 1]", p(1).name());
    EXPECT_EQ("p[ 8]", p(8).name());
    EXPECT_EQ("_p_9", p(9).name());
    EXPECT_EQ("_g_0_0", g(0, 0).name());
    EXPECT_EQ("g[ 0,  1]", g(0, 1).name());
    EXPECT_EQ("g[19, 28]", g(19, 28).name());
    EXPECT_EQ("_g_19_29", g(19, 29).name());
}

/*****************************************************************************/
TEST_F(TestModelBuilder, setup_is_integer) {
    {
        model::Model<int, double> model;

        auto& x = model.create_variables("x", 2);
        auto& g = model.create_constraint("g");
        g       = x(0) + x(1) <= 1;

        model.builder().setup_is_integer();
        EXPECT_TRUE(model.is_integer());
    }

    {
        model::Model<int, double> model;

        auto& x = model.create_variables("x", 2);
        auto& g = model.create_constraint("g");
        g       = 1.1 * x(0) + x(1) <= 1;

        model.builder().setup_is_integer();
        EXPECT_FALSE(model.is_integer());
    }
}

/*****************************************************************************/
TEST_F(TestModelBuilder, setup_variable_related_constraints) {
    model::Model<int, double> model;

    auto& x = model.create_variables("x", 10, 0, 1);
    auto& y = model.create_variables("y", {20, 30}, 0, 1);

    auto& g = model.create_constraints("g", 3);
    g(0)    = x.selection();
    g(1)    = y.selection();
    g(2)    = x(0) + 2 * y.sum({0, model_component::Range::All}) >= 1;

    model.reference().update_constraint_reference();
    model.builder().setup_variable_constraint_sensitivities();
    model.builder().setup_variable_related_constraint_ptrs();
    model.builder().setup_variable_related_binary_coefficient_constraint_ptrs();

    for (auto i = 0; i < 10; i++) {
        EXPECT_TRUE(std::find(x(i).related_constraint_ptrs().begin(),
                              x(i).related_constraint_ptrs().end(),
                              &g(0)) != x(i).related_constraint_ptrs().end());
        EXPECT_FALSE(std::find(x(i).related_constraint_ptrs().begin(),
                               x(i).related_constraint_ptrs().end(),
                               &g(1)) != x(i).related_constraint_ptrs().end());
        /// Only x(0) is related to g(2).
        EXPECT_EQ(i == 0,
                  std::find(x(i).related_constraint_ptrs().begin(),
                            x(i).related_constraint_ptrs().end(),
                            &g(2)) != x(i).related_constraint_ptrs().end());
    }

    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            EXPECT_FALSE(std::find(y(i, j).related_constraint_ptrs().begin(),
                                   y(i, j).related_constraint_ptrs().end(),
                                   &g(0)) !=
                         y(i, j).related_constraint_ptrs().end());
            EXPECT_TRUE(std::find(y(i, j).related_constraint_ptrs().begin(),
                                  y(i, j).related_constraint_ptrs().end(),
                                  &g(1)) !=
                        y(i, j).related_constraint_ptrs().end());
            /// Only y(0,*) is related to g(2).
            EXPECT_EQ(
                i == 0,
                std::find(y(i, j).related_constraint_ptrs().begin(),
                          y(i, j).related_constraint_ptrs().end(),
                          &g(2)) != y(i, j).related_constraint_ptrs().end());
        }
    }

    for (auto i = 0; i < 10; i++) {
        EXPECT_TRUE(
            std::find(x(i).related_binary_coefficient_constraint_ptrs().begin(),
                      x(i).related_binary_coefficient_constraint_ptrs().end(),
                      &g(0)) !=
            x(i).related_binary_coefficient_constraint_ptrs().end());
        EXPECT_FALSE(
            std::find(x(i).related_binary_coefficient_constraint_ptrs().begin(),
                      x(i).related_binary_coefficient_constraint_ptrs().end(),
                      &g(1)) !=
            x(i).related_binary_coefficient_constraint_ptrs().end());
    }

    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            EXPECT_FALSE(
                std::find(
                    y(i, j)
                        .related_binary_coefficient_constraint_ptrs()
                        .begin(),
                    y(i, j).related_binary_coefficient_constraint_ptrs().end(),
                    &g(0)) !=
                y(i, j).related_binary_coefficient_constraint_ptrs().end());
            EXPECT_TRUE(
                std::find(
                    y(i, j)
                        .related_binary_coefficient_constraint_ptrs()
                        .begin(),
                    y(i, j).related_binary_coefficient_constraint_ptrs().end(),
                    &g(1)) !=
                y(i, j).related_binary_coefficient_constraint_ptrs().end());
            EXPECT_FALSE(
                std::find(
                    y(i, j)
                        .related_binary_coefficient_constraint_ptrs()
                        .begin(),
                    y(i, j).related_binary_coefficient_constraint_ptrs().end(),
                    &g(2)) !=
                y(i, j).related_binary_coefficient_constraint_ptrs().end());
        }
    }
    EXPECT_FALSE(
        std::find(x(0).related_binary_coefficient_constraint_ptrs().begin(),
                  x(0).related_binary_coefficient_constraint_ptrs().end(),
                  &g(2)) !=
        x(0).related_binary_coefficient_constraint_ptrs().end());
}

/*****************************************************************************/
TEST_F(TestModelBuilder, setup_variable_objective_sensitivities) {
    model::Model<int, double> model;

    auto& x = model.create_variables("x", 10, 0, 1);
    auto& y = model.create_variables("y", {20, 30}, 0, 1);

    model.minimize(2 * x.sum() + 5 * y.sum());
    model.builder().setup_variable_objective_sensitivities();

    for (auto i = 0; i < 10; i++) {
        EXPECT_EQ(2, x(i).objective_sensitivity());
    }

    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            EXPECT_EQ(5, y(i, j).objective_sensitivity());
        }
    }
}

/*****************************************************************************/
TEST_F(TestModelBuilder, setup_variable_constraint_sensitivities) {
    model::Model<int, double> model;

    auto& x = model.create_variables("x", 10, 0, 1);
    auto& y = model.create_variables("y", {20, 30}, 0, 1);
    auto& g = model.create_constraints("g", 2);
    auto& p = model.create_expressions("p", 2);
    for (auto i = 0; i < 10; i++) {
        p(0) += (i + 1) * x(i);
    }

    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            p(1) += (i + j + 1) * y(i, j);
        }
    }
    g(0) = p(0) <= 10000;
    g(1) = p(1) <= 10000;

    model.builder().setup_variable_constraint_sensitivities();

    for (auto i = 0; i < 10; i++) {
        for (auto&& sensitivity : x(i).constraint_sensitivities()) {
            if (sensitivity.first == &g(0)) {
                EXPECT_EQ(i + 1, sensitivity.second);
            }
        }
    }

    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            for (auto&& sensitivity : y(i, j).constraint_sensitivities()) {
                if (sensitivity.first == &g(1)) {
                    EXPECT_EQ(i + j + 1, sensitivity.second);
                }
            }
        }
    }
}

/*****************************************************************************/
TEST_F(TestModelBuilder, setup_related_selection_constraint_ptr_index) {
    model::Model<int, double> model;

    auto& x = model.create_variables("x", 4, 0, 1);

    model.create_constraint("selection", x.selection());

    /**
     * To verify the sort process, constraints are defined in the order of c_1
     * -> c_2 -> c_0, which will be sorted in the order of c_0 - >c_1 -> c2.
     */
    auto& c_1 = model.create_constraint("c_1", x(1) + x(2) <= 1);
    auto& c_2 = model.create_constraint("c_2", x(2) + x(3) <= 1);
    auto& c_0 = model.create_constraint("c_0", x(0) + x(1) <= 1);

    model.builder().setup_unique_names();
    model.builder().setup_structure();
    preprocess::SelectionExtractor<int, double> selection_extractor(&model);
    selection_extractor.extract_by_defined_order(false);
    model.builder().setup_structure();
    model.builder().setup_variable_related_selection_constraint_ptr_index();

    EXPECT_EQ(1, static_cast<int>(model.selections().size()));

    EXPECT_EQ(3, static_cast<int>(
                     model.selections()[0].related_constraint_ptrs.size()));
    EXPECT_EQ(&c_0[0], model.selections()[0].related_constraint_ptrs[0]);
    EXPECT_EQ(&c_1[0], model.selections()[0].related_constraint_ptrs[1]);
    EXPECT_EQ(&c_2[0], model.selections()[0].related_constraint_ptrs[2]);

    EXPECT_EQ(0, x(0).related_selection_constraint_ptr_index_min());
    EXPECT_EQ(0, x(0).related_selection_constraint_ptr_index_max());

    EXPECT_EQ(0, x(1).related_selection_constraint_ptr_index_min());
    EXPECT_EQ(1, x(1).related_selection_constraint_ptr_index_max());

    EXPECT_EQ(1, x(2).related_selection_constraint_ptr_index_min());
    EXPECT_EQ(2, x(2).related_selection_constraint_ptr_index_max());

    EXPECT_EQ(2, x(3).related_selection_constraint_ptr_index_min());
    EXPECT_EQ(2, x(3).related_selection_constraint_ptr_index_max());
}

/*****************************************************************************/
TEST_F(TestModelBuilder,
       setup_variable_related_binary_coefficient_constraints) {
    /// This test is covered by setup_variable_related_constraint_ptrs().
}

/*****************************************************************************/
TEST_F(TestModelBuilder, setup_neighborhood) {
    /// This test is covered by test_neighborhood.h
}

/*****************************************************************************/
TEST_F(TestModelBuilder, setup_fixed_sensitivities) {
    /// This test is covered by test_expression.h
}

/*****************************************************************************/
TEST_F(TestModelBuilder,
       setup_positive_and_negative_coefficient_mutable_variable_ptrs) {
    /// This test is covered by test_expression.h
}

/*****************************************************************************/
TEST_F(TestModelBuilder, shrink_global_penalty_coefficient) {
    model::Model<int, double> model;

    auto& x = model.create_variables("x", 2, -1, 1);

    model.minimize(2 * x.sum() + 1);
    model.set_global_penalty_coefficient(100.0);
    model.builder().shrink_global_penalty_coefficient(false);

    const auto EXPECTED_UPPER_BOUND = 2.0 * (1 + 1) + 1;   // 5
    const auto EXPECTED_LOWER_BOUND = 2.0 * (-1 - 1) + 1;  // -3

    EXPECT_FLOAT_EQ(EXPECTED_UPPER_BOUND - EXPECTED_LOWER_BOUND + 1,
                    model.global_penalty_coefficient());  // 5 -(-3)+ 1 = 9
}

/*****************************************************************************/
TEST_F(TestModelBuilder, setup_selections) {
    model::Model<int, double> model;

    auto& x = model.create_variables("x", 10, 0, 1);
    auto& y = model.create_variables("y", 5, 0, 1);

    model.create_constraint("c_0", x.selection());
    model.create_constraint("c_1", y.selection());

    model.builder().setup_unique_names();
    model.builder().setup_structure();
    preprocess::SelectionExtractor<int, double> selection_extractor(&model);
    selection_extractor.extract_by_defined_order(false);
    auto& selections = selection_extractor.selections();
    model.builder().setup_structure();

    model.builder().setup_selections(selections);

    for (auto i = 0; i < 10; i++) {
        EXPECT_EQ(&(model.selections()[0]), x(i).selection_ptr());
    }

    for (auto i = 0; i < 5; i++) {
        EXPECT_EQ(&(model.selections()[1]), y(i).selection_ptr());
    }
}

/*****************************************************************************/
TEST_F(TestModelBuilder, setup_user_defined_selection_constraints) {
    model::Model<int, double> model;

    auto& x = model.create_variables("x", 3, 0, 1);
    auto& g = model.create_constraints("g", 2);

    g(0) = x[0] + x[1] == 1;
    g(1) = x[1] + x[2] == 1;

    model.builder().setup_unique_names();
    std::unordered_set<std::string> constraint_names;
    constraint_names.insert("g[0]");

    model.builder().setup_user_defined_selection_constraints(constraint_names);
    EXPECT_TRUE(g(0).is_user_defined_selection());
    EXPECT_FALSE(g(1).is_user_defined_selection());
}

/*****************************************************************************/
TEST_F(TestModelBuilder, setup_flippable_variable_ptr_pairs) {
    model::Model<int, double> model;

    auto& x = model.create_variables("x", 3, 0, 1);
    model.builder().setup_unique_names();

    std::vector<std::pair<std::string, std::string>>
        flippable_variable_name_pairs;
    flippable_variable_name_pairs.push_back(std::make_pair("x[0]", "x[1]"));
    flippable_variable_name_pairs.push_back(std::make_pair("x[1]", "x[2]"));

    model.builder().setup_flippable_variable_ptr_pairs(
        flippable_variable_name_pairs);

    EXPECT_EQ(2, static_cast<int>(model.flippable_variable_ptr_pairs().size()));
    EXPECT_EQ(&x(0), model.flippable_variable_ptr_pairs()[0].first);
    EXPECT_EQ(&x(1), model.flippable_variable_ptr_pairs()[0].second);
    EXPECT_EQ(&x(1), model.flippable_variable_ptr_pairs()[1].first);
    EXPECT_EQ(&x(2), model.flippable_variable_ptr_pairs()[1].second);
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/