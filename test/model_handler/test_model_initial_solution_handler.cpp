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
class TestModelInitialSolutionHandler : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestModelInitialSolutionHandler, import_solution_arg_dense_solution) {
    model::Model<int, double> model;

    auto& x = model.create_variable("x");
    auto& y = model.create_variables("y", 10);
    auto& z = model.create_variables("z", {10, 10});

    multi_array::ValueProxy<int> x_value(x.index());
    multi_array::ValueProxy<int> y_value(y.index(), 10);
    multi_array::ValueProxy<int> z_value(z.index(), {10, 10});

    x_value.value() = 1;

    for (auto i = 0; i < 10; i++) {
        y_value(i) = 10 * i;
    }

    for (auto i = 0; i < 10; i++) {
        for (auto j = 0; j < 10; j++) {
            z_value(i, j) = 100 * (i + j);
        }
    }

    solution::DenseSolution<int, double> solution;
    solution.variable_value_proxies.push_back(x_value);
    solution.variable_value_proxies.push_back(y_value);
    solution.variable_value_proxies.push_back(z_value);

    model.initial_solution_handler().import_solution(solution, false);

    EXPECT_EQ(1, x.value());

    for (auto i = 0; i < 10; i++) {
        EXPECT_EQ(10 * i, y(i).value());
    }

    for (auto i = 0; i < 10; i++) {
        for (auto j = 0; j < 10; j++) {
            EXPECT_EQ(100 * (i + j), z(i, j).value());
        }
    }
}

/*****************************************************************************/
TEST_F(TestModelInitialSolutionHandler, import_solution_arg_sparse_solution) {
    model::Model<int, double> model;

    auto& x = model.create_variable("x");
    auto& y = model.create_variables("y", 10);
    auto& z = model.create_variables("z", {20, 30});

    model.builder().setup_unique_names();

    solution::SparseSolution<int, double> solution;
    solution.variables["x"]         = 1;
    solution.variables["y[ 0]"]     = 2;
    solution.variables["y[ 9]"]     = 3;
    solution.variables["z[ 0,  0]"] = 4;
    solution.variables["z[19, 19]"] = 5;

    model.initial_solution_handler().import_solution(solution, false);

    EXPECT_EQ(x.value(), 1);
    EXPECT_EQ(y(0).value(), 2);
    EXPECT_EQ(y(9).value(), 3);
    EXPECT_EQ(z(0, 0).value(), 4);
    EXPECT_EQ(z(19, 19).value(), 5);
}

/*****************************************************************************/
TEST_F(TestModelInitialSolutionHandler, import_solution_arg_unordered_map) {
    model::Model<int, double> model;

    auto& x = model.create_variable("x");
    auto& y = model.create_variables("y", 10);
    auto& z = model.create_variables("z", {20, 30});

    model.builder().setup_unique_names();

    std::unordered_map<std::string, int> variables;
    variables["x"]         = 1;
    variables["y[ 0]"]     = 2;
    variables["y[ 9]"]     = 3;
    variables["z[ 0,  0]"] = 4;
    variables["z[19, 19]"] = 5;

    model.initial_solution_handler().import_solution(variables, false);

    EXPECT_EQ(x.value(), 1);
    EXPECT_EQ(y(0).value(), 2);
    EXPECT_EQ(y(9).value(), 3);
    EXPECT_EQ(z(0, 0).value(), 4);
    EXPECT_EQ(z(19, 19).value(), 5);
}

/*****************************************************************************/
TEST_F(TestModelInitialSolutionHandler, fix_variables) {
    model::Model<int, double> model;

    auto& x = model.create_variable("x");
    auto& y = model.create_variables("y", 10);
    auto& z = model.create_variables("z", {20, 30});

    model.builder().setup_unique_names();

    std::unordered_map<std::string, int> solution;
    solution["x"]         = 1;
    solution["y[ 0]"]     = 2;
    solution["y[ 9]"]     = 3;
    solution["z[ 0,  0]"] = 4;
    solution["z[19, 19]"] = 5;
    model.initial_solution_handler().fix_variables(solution);

    EXPECT_EQ(1, x.value());
    EXPECT_EQ(2, y(0).value());
    EXPECT_EQ(3, y(9).value());
    EXPECT_EQ(4, z(0, 0).value());
    EXPECT_EQ(5, z(19, 19).value());

    EXPECT_TRUE(x.is_fixed());
    EXPECT_TRUE(y(0).is_fixed());
    EXPECT_TRUE(y(9).is_fixed());
    EXPECT_TRUE(z(0, 0).is_fixed());
    EXPECT_TRUE(z(19, 19).is_fixed());
}

/*****************************************************************************/
TEST_F(TestModelInitialSolutionHandler, unfix_variables) {
    model::Model<int, double> model;

    auto& x = model.create_variable("x");
    auto& y = model.create_variables("y", 10);
    auto& z = model.create_variables("z", {20, 30});

    model.builder().setup_unique_names();

    std::unordered_set<std::string> mutable_variable_names;
    mutable_variable_names.insert("x");
    mutable_variable_names.insert("y[ 0]");
    mutable_variable_names.insert("y[ 9]");
    mutable_variable_names.insert("z[ 0,  0]");
    mutable_variable_names.insert("z[19, 19]");
    model.initial_solution_handler().unfix_variables(mutable_variable_names);

    EXPECT_FALSE(x.is_fixed());
    EXPECT_FALSE(y(0).is_fixed());
    EXPECT_TRUE(y(1).is_fixed());
    EXPECT_TRUE(y(8).is_fixed());
    EXPECT_FALSE(y(9).is_fixed());
    EXPECT_FALSE(z(0, 0).is_fixed());
    EXPECT_TRUE(z(0, 1).is_fixed());
    EXPECT_TRUE(z(19, 18).is_fixed());
    EXPECT_FALSE(z(19, 19).is_fixed());
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/