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
class TestModelStateInspector : public ::testing::Test {
   protected:
    utility::UniformRandom<std::uniform_int_distribution<>, int>
        m_random_integer;
    utility::UniformRandom<std::uniform_int_distribution<>, int>
        m_random_positive_integer;

    virtual void SetUp(void) {
        m_random_integer.setup(-1000, 1000, 0);
        m_random_positive_integer.setup(1, 1000, 0);
    }
    virtual void TearDown() {
        /// nothing to do
    }
    int random_integer(void) {
        return m_random_integer.generate_random();
    }

    int random_positive_integer(void) {
        return m_random_positive_integer.generate_random();
    }
};

/*****************************************************************************/
TEST_F(TestModelStateInspector, generate_variable_parameter_proxies) {
    model::Model<int, double> model;

    auto& x = model.create_variable("x");
    auto& y = model.create_variables("y", 10);
    auto& z = model.create_variables("z", {10, 10});

    int fill_value = random_integer();
    model.builder().setup_unique_names();

    auto parameter_proxies =
        model.state_inspector().generate_variable_parameter_proxies(fill_value);
    EXPECT_EQ(x.index(), parameter_proxies[0].index());
    EXPECT_EQ(1, parameter_proxies[0].number_of_dimensions());
    EXPECT_EQ(1, parameter_proxies[0].number_of_elements());
    EXPECT_EQ("x", parameter_proxies[0].flat_indexed_names(0));

    EXPECT_EQ(y.index(), parameter_proxies[1].index());
    EXPECT_EQ(1, parameter_proxies[1].number_of_dimensions());
    EXPECT_EQ(10, parameter_proxies[1].number_of_elements());
    EXPECT_EQ("y[ 0]", parameter_proxies[1].flat_indexed_names(0));
    EXPECT_EQ("y[ 9]", parameter_proxies[1].flat_indexed_names(10 - 1));

    EXPECT_EQ(z.index(), parameter_proxies[2].index());
    EXPECT_EQ(2, parameter_proxies[2].number_of_dimensions());
    EXPECT_EQ(100, parameter_proxies[2].number_of_elements());
    EXPECT_EQ("z[ 0,  0]", parameter_proxies[2].flat_indexed_names(0));
    EXPECT_EQ("z[ 9,  9]", parameter_proxies[2].flat_indexed_names(100 - 1));

    for (auto&& value : parameter_proxies[0].flat_indexed_values()) {
        EXPECT_EQ(fill_value, value);
    }
    for (auto&& value : parameter_proxies[1].flat_indexed_values()) {
        EXPECT_EQ(fill_value, value);
    }
    for (auto&& value : parameter_proxies[2].flat_indexed_values()) {
        EXPECT_EQ(fill_value, value);
    }
}

/*****************************************************************************/
TEST_F(TestModelStateInspector, generate_expression_parameter_proxies) {
    model::Model<int, double> model;

    auto& p = model.create_expression("p");
    auto& q = model.create_expressions("q", 10);
    auto& r = model.create_expressions("r", {10, 10});

    int fill_value = random_integer();
    model.builder().setup_unique_names();

    auto parameter_proxies =
        model.state_inspector().generate_expression_parameter_proxies(
            fill_value);
    EXPECT_EQ(p.index(), parameter_proxies[0].index());
    EXPECT_EQ(1, parameter_proxies[0].number_of_dimensions());
    EXPECT_EQ(1, parameter_proxies[0].number_of_elements());
    EXPECT_EQ("p", parameter_proxies[0].flat_indexed_names(0));

    EXPECT_EQ(q.index(), parameter_proxies[1].index());
    EXPECT_EQ(1, parameter_proxies[1].number_of_dimensions());
    EXPECT_EQ(10, parameter_proxies[1].number_of_elements());
    EXPECT_EQ("q[ 0]", parameter_proxies[1].flat_indexed_names(0));
    EXPECT_EQ("q[ 9]", parameter_proxies[1].flat_indexed_names(10 - 1));

    EXPECT_EQ(r.index(), parameter_proxies[2].index());
    EXPECT_EQ(2, parameter_proxies[2].number_of_dimensions());
    EXPECT_EQ(100, parameter_proxies[2].number_of_elements());
    EXPECT_EQ("r[ 0,  0]", parameter_proxies[2].flat_indexed_names(0));
    EXPECT_EQ("r[ 9,  9]", parameter_proxies[2].flat_indexed_names(100 - 1));

    for (auto&& value : parameter_proxies[0].flat_indexed_values()) {
        EXPECT_EQ(fill_value, value);
    }
    for (auto&& value : parameter_proxies[1].flat_indexed_values()) {
        EXPECT_EQ(fill_value, value);
    }
    for (auto&& value : parameter_proxies[2].flat_indexed_values()) {
        EXPECT_EQ(fill_value, value);
    }
}

/*****************************************************************************/
TEST_F(TestModelStateInspector, generate_constraint_parameter_proxies) {
    model::Model<int, double> model;

    auto& g = model.create_constraint("g");
    auto& h = model.create_constraints("h", 10);
    auto& v = model.create_constraints("v", {10, 10});

    int fill_value = random_integer();
    model.builder().setup_unique_names();

    auto parameter_proxies =
        model.state_inspector().generate_constraint_parameter_proxies(
            fill_value);
    EXPECT_EQ(g.index(), parameter_proxies[0].index());
    EXPECT_EQ(1, parameter_proxies[0].number_of_dimensions());
    EXPECT_EQ(1, parameter_proxies[0].number_of_elements());
    EXPECT_EQ("g", parameter_proxies[0].flat_indexed_names(0));

    EXPECT_EQ(h.index(), parameter_proxies[1].index());
    EXPECT_EQ(1, parameter_proxies[1].number_of_dimensions());
    EXPECT_EQ(10, parameter_proxies[1].number_of_elements());
    EXPECT_EQ("h[ 0]", parameter_proxies[1].flat_indexed_names(0));
    EXPECT_EQ("h[ 9]", parameter_proxies[1].flat_indexed_names(10 - 1));

    EXPECT_EQ(v.index(), parameter_proxies[2].index());
    EXPECT_EQ(2, parameter_proxies[2].number_of_dimensions());
    EXPECT_EQ(100, parameter_proxies[2].number_of_elements());
    EXPECT_EQ("v[ 0,  0]", parameter_proxies[2].flat_indexed_names(0));
    EXPECT_EQ("v[ 9,  9]", parameter_proxies[2].flat_indexed_names(100 - 1));

    for (auto&& value : parameter_proxies[0].flat_indexed_values()) {
        EXPECT_EQ(fill_value, value);
    }
    for (auto&& value : parameter_proxies[1].flat_indexed_values()) {
        EXPECT_EQ(fill_value, value);
    }
    for (auto&& value : parameter_proxies[2].flat_indexed_values()) {
        EXPECT_EQ(fill_value, value);
    }
}

/*****************************************************************************/
TEST_F(TestModelStateInspector, export_local_penalty_coefficient_proxies) {
    model::Model<int, double> model;

    auto& g = model.create_constraint("g");
    auto& h = model.create_constraints("h", 10);
    auto& v = model.create_constraints("v", {10, 10});

    model.builder().setup_unique_names();

    g(0).local_penalty_coefficient_less()    = 1.0;
    g(0).local_penalty_coefficient_greater() = 10.0;

    for (auto i = 0; i < 10; i++) {
        h(i).local_penalty_coefficient_less()    = 20.0;
        h(i).local_penalty_coefficient_greater() = 2.0;
    }

    for (auto i = 0; i < 10; i++) {
        for (auto j = 0; j < 10; j++) {
            v(i, j).local_penalty_coefficient_less()    = 3.0;
            v(i, j).local_penalty_coefficient_greater() = 30.0;
        }
    }

    auto local_penalty_coefficient_proxies =
        model.state_inspector().export_local_penalty_coefficient_proxies();

    EXPECT_EQ(g.index(), local_penalty_coefficient_proxies[0].index());
    EXPECT_EQ(1, local_penalty_coefficient_proxies[0].number_of_dimensions());
    EXPECT_EQ(1, local_penalty_coefficient_proxies[0].number_of_elements());
    EXPECT_EQ("g", local_penalty_coefficient_proxies[0].flat_indexed_names(0));

    EXPECT_EQ(h.index(), local_penalty_coefficient_proxies[1].index());
    EXPECT_EQ(1, local_penalty_coefficient_proxies[1].number_of_dimensions());
    EXPECT_EQ(10, local_penalty_coefficient_proxies[1].number_of_elements());
    EXPECT_EQ("h[ 0]",
              local_penalty_coefficient_proxies[1].flat_indexed_names(0));
    EXPECT_EQ("h[ 9]",
              local_penalty_coefficient_proxies[1].flat_indexed_names(10 - 1));

    EXPECT_EQ(v.index(), local_penalty_coefficient_proxies[2].index());
    EXPECT_EQ(2, local_penalty_coefficient_proxies[2].number_of_dimensions());
    EXPECT_EQ(100, local_penalty_coefficient_proxies[2].number_of_elements());
    EXPECT_EQ("v[ 0,  0]",
              local_penalty_coefficient_proxies[2].flat_indexed_names(0));
    EXPECT_EQ("v[ 9,  9]",
              local_penalty_coefficient_proxies[2].flat_indexed_names(100 - 1));

    for (auto&& value :
         local_penalty_coefficient_proxies[0].flat_indexed_values()) {
        EXPECT_FLOAT_EQ(10.0, value);
    }
    for (auto&& value :
         local_penalty_coefficient_proxies[1].flat_indexed_values()) {
        EXPECT_FLOAT_EQ(20.0, value);
    }
    for (auto&& value :
         local_penalty_coefficient_proxies[2].flat_indexed_values()) {
        EXPECT_FLOAT_EQ(30.0, value);
    }
}

/*****************************************************************************/
TEST_F(TestModelStateInspector, export_update_count_proxies) {
    model::Model<int, double> model;

    auto& x = model.create_variable("x");
    auto& y = model.create_variables("y", 10);
    auto& z = model.create_variables("z", {10, 10});

    model.builder().setup_unique_names();
    x(0).increment_update_count();

    for (auto i = 0; i < 10; i++) {
        y(i).increment_update_count();
        y(i).increment_update_count();
    }

    for (auto i = 0; i < 10; i++) {
        for (auto j = 0; j < 10; j++) {
            z(i, j).increment_update_count();
            z(i, j).increment_update_count();
            z(i, j).increment_update_count();
        }
    }

    auto update_count_proxies =
        model.state_inspector().export_update_count_proxies();

    EXPECT_EQ(x.index(), update_count_proxies[0].index());
    EXPECT_EQ(1, update_count_proxies[0].number_of_dimensions());
    EXPECT_EQ(1, update_count_proxies[0].number_of_elements());
    EXPECT_EQ("x", update_count_proxies[0].flat_indexed_names(0));

    EXPECT_EQ(y.index(), update_count_proxies[1].index());
    EXPECT_EQ(1, update_count_proxies[1].number_of_dimensions());
    EXPECT_EQ(10, update_count_proxies[1].number_of_elements());
    EXPECT_EQ("y[ 0]", update_count_proxies[1].flat_indexed_names(0));
    EXPECT_EQ("y[ 9]", update_count_proxies[1].flat_indexed_names(10 - 1));

    EXPECT_EQ(z.index(), update_count_proxies[2].index());
    EXPECT_EQ(2, update_count_proxies[2].number_of_dimensions());
    EXPECT_EQ(100, update_count_proxies[2].number_of_elements());
    EXPECT_EQ("z[ 0,  0]", update_count_proxies[2].flat_indexed_names(0));
    EXPECT_EQ("z[ 9,  9]", update_count_proxies[2].flat_indexed_names(100 - 1));

    for (auto&& value : update_count_proxies[0].flat_indexed_values()) {
        EXPECT_EQ(1, value);
    }
    for (auto&& value : update_count_proxies[1].flat_indexed_values()) {
        EXPECT_EQ(2, value);
    }
    for (auto&& value : update_count_proxies[2].flat_indexed_values()) {
        EXPECT_EQ(3, value);
    }
}

/*****************************************************************************/
TEST_F(TestModelStateInspector, export_violation_count_proxies) {
    model::Model<int, double> model;

    auto& g = model.create_constraint("g");
    auto& h = model.create_constraints("h", 10);
    auto& v = model.create_constraints("v", {10, 10});

    model.builder().setup_unique_names();

    g(0).increment_violation_count();

    for (auto i = 0; i < 10; i++) {
        h(i).increment_violation_count();
        h(i).increment_violation_count();
    }

    for (auto i = 0; i < 10; i++) {
        for (auto j = 0; j < 10; j++) {
            v(i, j).increment_violation_count();
            v(i, j).increment_violation_count();
            v(i, j).increment_violation_count();
        }
    }

    auto violation_count_proxies =
        model.state_inspector().export_violation_count_proxies();

    EXPECT_EQ(g.index(), violation_count_proxies[0].index());
    EXPECT_EQ(1, violation_count_proxies[0].number_of_dimensions());
    EXPECT_EQ(1, violation_count_proxies[0].number_of_elements());
    EXPECT_EQ("g", violation_count_proxies[0].flat_indexed_names(0));

    EXPECT_EQ(h.index(), violation_count_proxies[1].index());
    EXPECT_EQ(1, violation_count_proxies[1].number_of_dimensions());
    EXPECT_EQ(10, violation_count_proxies[1].number_of_elements());
    EXPECT_EQ("h[ 0]", violation_count_proxies[1].flat_indexed_names(0));
    EXPECT_EQ("h[ 9]", violation_count_proxies[1].flat_indexed_names(10 - 1));

    EXPECT_EQ(v.index(), violation_count_proxies[2].index());
    EXPECT_EQ(2, violation_count_proxies[2].number_of_dimensions());
    EXPECT_EQ(100, violation_count_proxies[2].number_of_elements());
    EXPECT_EQ("v[ 0,  0]", violation_count_proxies[2].flat_indexed_names(0));
    EXPECT_EQ("v[ 9,  9]",
              violation_count_proxies[2].flat_indexed_names(100 - 1));

    for (auto&& value : violation_count_proxies[0].flat_indexed_values()) {
        EXPECT_EQ(1, value);
    }
    for (auto&& value : violation_count_proxies[1].flat_indexed_values()) {
        EXPECT_EQ(2, value);
    }
    for (auto&& value : violation_count_proxies[2].flat_indexed_values()) {
        EXPECT_EQ(3, value);
    }
}

/*****************************************************************************/
TEST_F(TestModelStateInspector, export_dense_solution) {
    model::Model<int, double> model;

    auto& x = model.create_variable("x");
    auto& y = model.create_variables("y", 10);
    auto& z = model.create_variables("z", {20, 30});

    auto& p = model.create_expression("p");
    auto& q = model.create_expressions("q", 10);
    auto& r = model.create_expressions("r", {20, 30});

    auto& g = model.create_constraint("g");
    auto& h = model.create_constraints("h", 10);
    auto& v = model.create_constraints("v", {20, 30});

    p = random_integer() * x;
    for (auto i = 0; i < 10; i++) {
        q(i) = random_integer() * y(i);
    }

    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            r(i, j) = random_integer() * z(i, j) + random_integer();
            v(i, j) = r(i, j) == random_integer();
        }
    }
    model.minimize(random_integer() * p + random_integer() * q.sum() +
                   random_integer() * r.sum());

    x = random_integer();
    for (auto i = 0; i < 10; i++) {
        y(i) = random_integer();
    }

    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            z(i, j) = random_integer();
        }
    }

    model.updater().update();

    auto solution = model.state_inspector().export_dense_solution();
    EXPECT_EQ(3, static_cast<int>(solution.variable_value_proxies.size()));
    EXPECT_EQ(3, static_cast<int>(solution.expression_value_proxies.size()));
    EXPECT_EQ(3, static_cast<int>(solution.constraint_value_proxies.size()));
    EXPECT_EQ(3, static_cast<int>(solution.violation_value_proxies.size()));

    EXPECT_EQ(model.objective().value(), solution.objective);
    EXPECT_EQ(model.is_feasible(), solution.is_feasible);

    EXPECT_EQ(x.index(), solution.variable_value_proxies[0].index());
    EXPECT_EQ(x.value(), solution.variable_value_proxies[0].value());

    EXPECT_EQ(y.index(), solution.variable_value_proxies[1].index());
    for (auto i = 0; i < 10; i++) {
        EXPECT_EQ(y(i).value(), solution.variable_value_proxies[1](i));
    }

    EXPECT_EQ(z.index(), solution.variable_value_proxies[2].index());
    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            EXPECT_EQ(z(i, j).value(),
                      solution.variable_value_proxies[2](i, j));
        }
    }

    EXPECT_EQ(p.index(), solution.expression_value_proxies[0].index());
    EXPECT_EQ(p.value(), solution.expression_value_proxies[0].value());

    EXPECT_EQ(q.index(), solution.expression_value_proxies[1].index());
    for (auto i = 0; i < 10; i++) {
        EXPECT_EQ(q(i).value(), solution.expression_value_proxies[1](i));
    }

    EXPECT_EQ(r.index(), solution.expression_value_proxies[2].index());
    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            EXPECT_EQ(r(i, j).value(),
                      solution.expression_value_proxies[2](i, j));
        }
    }

    EXPECT_EQ(g.index(), solution.constraint_value_proxies[0].index());
    EXPECT_EQ(g.constraint_value(),
              solution.constraint_value_proxies[0].value());

    EXPECT_EQ(h.index(), solution.constraint_value_proxies[1].index());
    for (auto i = 0; i < 10; i++) {
        EXPECT_EQ(h(i).constraint_value(),
                  solution.constraint_value_proxies[1](i));
    }

    EXPECT_EQ(v.index(), solution.constraint_value_proxies[2].index());
    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            EXPECT_EQ(v(i, j).constraint_value(),
                      solution.constraint_value_proxies[2](i, j));
        }
    }

    EXPECT_EQ(g.index(), solution.violation_value_proxies[0].index());
    EXPECT_EQ(g.violation_value(), solution.violation_value_proxies[0].value());

    EXPECT_EQ(h.index(), solution.constraint_value_proxies[1].index());
    for (auto i = 0; i < 10; i++) {
        EXPECT_EQ(h(i).violation_value(),
                  solution.violation_value_proxies[1](i));
    }

    EXPECT_EQ(v.index(), solution.violation_value_proxies[2].index());
    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            EXPECT_EQ(v(i, j).violation_value(),
                      solution.violation_value_proxies[2](i, j));
        }
    }
}

/*****************************************************************************/
TEST_F(TestModelStateInspector, export_sparse_solution) {
    model::Model<int, double> model;

    auto& x = model.create_variable("x");
    auto& y = model.create_variables("y", 10);
    auto& z = model.create_variables("z", {20, 30});

    model.minimize(random_integer() * x.sum() + random_integer() * y.sum() +
                   random_integer() * z.sum());
    model.builder().setup_unique_names();

    x = 10;
    for (auto i = 0; i < 10; i++) {
        y(i) = random_integer();
    }

    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            z(i, j) = random_integer();
        }
    }

    model.updater().update();

    auto sparse_solution = model.state_inspector().export_sparse_solution();
    EXPECT_EQ(model.objective().value(), sparse_solution.objective);
    EXPECT_EQ(model.is_feasible(), sparse_solution.is_feasible);

    EXPECT_EQ(10, sparse_solution.variables[x[0].name()]);
}

/*****************************************************************************/
TEST_F(TestModelStateInspector, export_named_solution) {
    model::Model<int, double> model;

    auto& x = model.create_variable("x");
    auto& y = model.create_variables("y", 10);
    auto& z = model.create_variables("z", {20, 30});

    auto& p = model.create_expression("p");
    auto& q = model.create_expressions("q", 10);
    auto& r = model.create_expressions("r", {20, 30});

    auto& g = model.create_constraint("g");
    auto& h = model.create_constraints("h", 10);
    auto& v = model.create_constraints("v", {20, 30});

    p = random_integer() * x;
    for (auto i = 0; i < 10; i++) {
        q(i) = random_integer() * y(i);
    }

    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            r(i, j) = random_integer() * z(i, j) + random_integer();
            v(i, j) = r(i, j) == random_integer();
        }
    }
    model.minimize(random_integer() * p + random_integer() * q.sum() +
                   random_integer() * r.sum());

    x = random_integer();
    for (auto i = 0; i < 10; i++) {
        y(i) = random_integer();
    }

    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            z(i, j) = random_integer();
        }
    }

    model.builder().setup_structure();
    model.updater().update();

    auto named_solution = model.state_inspector().export_named_solution();

    EXPECT_EQ(3, static_cast<int>(named_solution.variables().size()));
    EXPECT_EQ(3, static_cast<int>(named_solution.expressions().size()));
    EXPECT_EQ(3, static_cast<int>(named_solution.constraints().size()));
    EXPECT_EQ(3, static_cast<int>(named_solution.violations().size()));

    EXPECT_EQ(model.objective().value(), named_solution.objective());
    EXPECT_EQ(model.is_feasible(), named_solution.is_feasible());

    EXPECT_EQ(x.index(), named_solution.variables("x").index());
    EXPECT_EQ(x.index(), named_solution.variables().at("x").index());
    EXPECT_EQ(x.value(), named_solution.variables("x").value());
    EXPECT_EQ(x.value(), named_solution.variables().at("x").value());

    EXPECT_EQ(y.index(), named_solution.variables("y").index());
    EXPECT_EQ(y.index(), named_solution.variables().at("y").index());
    for (auto i = 0; i < 10; i++) {
        EXPECT_EQ(y(i).value(), named_solution.variables("y")(i));
        EXPECT_EQ(y(i).value(), named_solution.variables().at("y")(i));
    }

    EXPECT_EQ(z.index(), named_solution.variables("z").index());
    EXPECT_EQ(z.index(), named_solution.variables().at("z").index());
    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            EXPECT_EQ(z(i, j).value(), named_solution.variables("z")(i, j));
            EXPECT_EQ(z(i, j).value(),
                      named_solution.variables().at("z")(i, j));
        }
    }

    EXPECT_EQ(p.index(), named_solution.expressions("p").index());
    EXPECT_EQ(p.index(), named_solution.expressions().at("p").index());
    EXPECT_EQ(p.value(), named_solution.expressions("p").value());
    EXPECT_EQ(p.value(), named_solution.expressions().at("p").value());

    EXPECT_EQ(q.index(), named_solution.expressions("q").index());
    EXPECT_EQ(q.index(), named_solution.expressions().at("q").index());
    for (auto i = 0; i < 10; i++) {
        EXPECT_EQ(q(i).value(), named_solution.expressions("q")(i));
        EXPECT_EQ(q(i).value(), named_solution.expressions().at("q")(i));
    }

    EXPECT_EQ(r.index(), named_solution.expressions("r").index());
    EXPECT_EQ(r.index(), named_solution.expressions().at("r").index());
    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            EXPECT_EQ(r(i, j).value(), named_solution.expressions("r")(i, j));
            EXPECT_EQ(r(i, j).value(),
                      named_solution.expressions().at("r")(i, j));
        }
    }

    EXPECT_EQ(g.index(), named_solution.constraints("g").index());
    EXPECT_EQ(g.index(), named_solution.constraints().at("g").index());
    EXPECT_EQ(g.constraint_value(), named_solution.constraints("g").value());
    EXPECT_EQ(g.constraint_value(),
              named_solution.constraints().at("g").value());

    EXPECT_EQ(h.index(), named_solution.constraints("h").index());
    EXPECT_EQ(h.index(), named_solution.constraints().at("h").index());
    for (auto i = 0; i < 10; i++) {
        EXPECT_EQ(h(i).constraint_value(), named_solution.constraints("h")(i));
        EXPECT_EQ(h(i).constraint_value(),
                  named_solution.constraints().at("h")(i));
    }

    EXPECT_EQ(v.index(), named_solution.constraints("v").index());
    EXPECT_EQ(v.index(), named_solution.constraints().at("v").index());
    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            EXPECT_EQ(v(i, j).constraint_value(),
                      named_solution.constraints("v")(i, j));
            EXPECT_EQ(v(i, j).constraint_value(),
                      named_solution.constraints().at("v")(i, j));
        }
    }

    EXPECT_EQ(g.index(), named_solution.violations("g").index());
    EXPECT_EQ(g.index(), named_solution.violations().at("g").index());
    EXPECT_EQ(g.violation_value(), named_solution.violations("g").value());
    EXPECT_EQ(g.violation_value(), named_solution.violations().at("g").value());

    EXPECT_EQ(h.index(), named_solution.violations("h").index());
    EXPECT_EQ(h.index(), named_solution.violations().at("h").index());
    for (auto i = 0; i < 10; i++) {
        EXPECT_EQ(h(i).violation_value(), named_solution.violations("h")(i));
        EXPECT_EQ(h(i).violation_value(),
                  named_solution.violations().at("h")(i));
    }

    EXPECT_EQ(v.index(), named_solution.violations("v").index());
    EXPECT_EQ(v.index(), named_solution.violations().at("v").index());
    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            EXPECT_EQ(v(i, j).violation_value(),
                      named_solution.violations("v")(i, j));
            EXPECT_EQ(v(i, j).violation_value(),
                      named_solution.violations().at("v")(i, j));
        }
    }
}

/*****************************************************************************/
TEST_F(TestModelStateInspector, export_named_penalty_coefficients) {
    model::Model<int, double> model;

    auto& g = model.create_constraint("g");
    auto& h = model.create_constraints("h", 10);
    auto& v = model.create_constraints("v", {10, 10});

    model.builder().setup_unique_names();

    g(0).local_penalty_coefficient_less()    = 1.0;
    g(0).local_penalty_coefficient_greater() = 10.0;

    for (auto i = 0; i < 10; i++) {
        h(i).local_penalty_coefficient_less()    = 20.0;
        h(i).local_penalty_coefficient_greater() = 2.0;
    }

    for (auto i = 0; i < 10; i++) {
        for (auto j = 0; j < 10; j++) {
            v(i, j).local_penalty_coefficient_less()    = 3.0;
            v(i, j).local_penalty_coefficient_greater() = 30.0;
        }
    }

    auto named_penalty_coefficients =
        model.state_inspector().export_named_penalty_coefficients();

    EXPECT_FLOAT_EQ(10.0, named_penalty_coefficients["g"](0));
    EXPECT_FLOAT_EQ(20.0, named_penalty_coefficients["h"](0));
    EXPECT_FLOAT_EQ(20.0, named_penalty_coefficients["h"](9));
    EXPECT_FLOAT_EQ(30.0, named_penalty_coefficients["v"](0, 0));
    EXPECT_FLOAT_EQ(30.0, named_penalty_coefficients["v"](9, 9));
}

/*****************************************************************************/
TEST_F(TestModelStateInspector, export_named_update_counts) {
    model::Model<int, double> model;

    auto& x = model.create_variable("x");
    auto& y = model.create_variables("y", 10);
    auto& z = model.create_variables("z", {10, 10});

    model.builder().setup_unique_names();
    x(0).increment_update_count();

    for (auto i = 0; i < 10; i++) {
        y(i).increment_update_count();
        y(i).increment_update_count();
    }

    for (auto i = 0; i < 10; i++) {
        for (auto j = 0; j < 10; j++) {
            z(i, j).increment_update_count();
            z(i, j).increment_update_count();
            z(i, j).increment_update_count();
        }
    }

    auto named_update_counts =
        model.state_inspector().export_named_update_counts();

    EXPECT_EQ(1, named_update_counts["x"](0));
    EXPECT_EQ(2, named_update_counts["y"](0));
    EXPECT_EQ(2, named_update_counts["y"](9));
    EXPECT_EQ(3, named_update_counts["z"](0, 0));
    EXPECT_EQ(3, named_update_counts["z"](9, 9));
}

/*****************************************************************************/
TEST_F(TestModelStateInspector, export_named_violation_counts) {
    model::Model<int, double> model;

    auto& g = model.create_constraint("g");
    auto& h = model.create_constraints("h", 10);
    auto& v = model.create_constraints("v", {10, 10});

    model.builder().setup_unique_names();

    g(0).increment_violation_count();

    for (auto i = 0; i < 10; i++) {
        h(i).increment_violation_count();
        h(i).increment_violation_count();
    }

    for (auto i = 0; i < 10; i++) {
        for (auto j = 0; j < 10; j++) {
            v(i, j).increment_violation_count();
            v(i, j).increment_violation_count();
            v(i, j).increment_violation_count();
        }
    }

    auto violation_counts =
        model.state_inspector().export_named_violation_counts();

    EXPECT_EQ(1, violation_counts["g"](0));
    EXPECT_EQ(2, violation_counts["h"](0));
    EXPECT_EQ(2, violation_counts["h"](9));
    EXPECT_EQ(3, violation_counts["v"](0, 0));
    EXPECT_EQ(3, violation_counts["v"](9, 9));
}
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/