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
class TestModelJSONHandler : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestModelJSONHandler, create_json_object) {
    model::Model<int, double> model;

    auto& x = model.create_variables("x", 5, 0, 1);
    auto& y = model.create_variable("y", -5, 5);

    [[maybe_unused]] auto& c_1 = model.create_constraint("c_1", x.selection());
    [[maybe_unused]] auto& c_2 =
        model.create_constraint("c_2", y <= 2 * x.sum() + 5.0);
    auto& c_3 = model.create_constraint("c_3", y >= -3 * x.sum() - 10.0);
    c_3.disable();

    x(0).fix_by(1);

    model.minimize(x.sum() + 2 * y);

    model.builder().setup_unique_names();
    model.builder().setup_structure();
    preprocess::SelectionExtractor<int, double> selection_extractor(&model);
    selection_extractor.extract_by_defined_order(false);
    model.builder().setup_structure();

    auto json_object = model.json_handler().create_json_object();
    EXPECT_EQ("untitled", json_object.get<std::string>("name"));
    EXPECT_TRUE(json_object.get<bool>("is_minimization"));

    auto variables = json_object.get<utility::json::JsonArray>("variables");

    {
        auto v = variables.get<utility::json::JsonObject>(0);

        EXPECT_EQ("x[0]", v.get<std::string>("name"));
        EXPECT_EQ(1, v.get<int>("value"));
        EXPECT_TRUE(v.get<bool>("is_fixed"));
        EXPECT_EQ(0, v.get<int>("lower_bound"));
        EXPECT_EQ(1, v.get<int>("upper_bound"));
        EXPECT_EQ("Binary", v.get<std::string>("sense"));
    }

    {
        auto v = variables.get<utility::json::JsonObject>(5);
        EXPECT_EQ("y", v.get<std::string>("name"));
        EXPECT_EQ(0, v.get<int>("value"));
        EXPECT_FALSE(v.get<bool>("is_fixed"));
        EXPECT_EQ(-5, v.get<int>("lower_bound"));
        EXPECT_EQ(5, v.get<int>("upper_bound"));
        EXPECT_EQ("Integer", v.get<std::string>("sense"));
    }

    auto objective = json_object.get<utility::json::JsonObject>("objective");
    auto sensitivities =
        objective.get<utility::json::JsonArray>("sensitivities");

    {
        auto s = sensitivities.get<utility::json::JsonObject>(0);
        EXPECT_EQ("x[0]", s.get<std::string>("variable"));
        EXPECT_FLOAT_EQ(1.0, s.get<double>("coefficient"));
    }

    {
        auto s = sensitivities.get<utility::json::JsonObject>(5);
        EXPECT_EQ("y", s.get<std::string>("variable"));
        EXPECT_FLOAT_EQ(2.0, s.get<double>("coefficient"));
    }

    auto constraints = json_object.get<utility::json::JsonArray>("constraints");

    {
        auto c = constraints.get<utility::json::JsonObject>(0);
        EXPECT_EQ("c_1", c.get<std::string>("name"));
        EXPECT_TRUE(c.get<bool>("is_enabled"));
        EXPECT_FLOAT_EQ(1.0, c.get<double>("rhs"));
        EXPECT_EQ("=", c.get<std::string>("sense"));

        auto sensitivities = c.get<utility::json::JsonArray>("sensitivities");

        {
            auto s = sensitivities.get<utility::json::JsonObject>(0);
            EXPECT_EQ("x[0]", s.get<std::string>("variable"));
            EXPECT_FLOAT_EQ(1.0, s.get<double>("coefficient"));
        }

        {
            auto s = sensitivities.get<utility::json::JsonObject>(4);
            EXPECT_EQ("x[4]", s.get<std::string>("variable"));
            EXPECT_FLOAT_EQ(1.0, s.get<double>("coefficient"));
        }
    }

    {
        auto c = constraints.get<utility::json::JsonObject>(1);
        EXPECT_EQ("c_2", c.get<std::string>("name"));
        EXPECT_TRUE(c.get<bool>("is_enabled"));
        EXPECT_FLOAT_EQ(5.0, c.get<double>("rhs"));
        EXPECT_EQ("<=", c.get<std::string>("sense"));

        auto sensitivities = c.get<utility::json::JsonArray>("sensitivities");
        {
            auto s = sensitivities.get<utility::json::JsonObject>(0);
            EXPECT_EQ("x[0]", s.get<std::string>("variable"));
            EXPECT_FLOAT_EQ(-2.0, s.get<double>("coefficient"));
        }

        {
            auto s = sensitivities.get<utility::json::JsonObject>(4);
            EXPECT_EQ("x[4]", s.get<std::string>("variable"));
            EXPECT_FLOAT_EQ(-2.0, s.get<double>("coefficient"));
        }
        {
            auto s = sensitivities.get<utility::json::JsonObject>(5);
            EXPECT_EQ("y", s.get<std::string>("variable"));
            EXPECT_FLOAT_EQ(1.0, s.get<double>("coefficient"));
        }
    }

    {
        auto c = constraints.get<utility::json::JsonObject>(2);
        EXPECT_EQ("c_3", c.get<std::string>("name"));
        EXPECT_FALSE(c.get<bool>("is_enabled"));
        EXPECT_FLOAT_EQ(-10.0, c.get<double>("rhs"));
        EXPECT_EQ(">=", c.get<std::string>("sense"));

        auto sensitivities = c.get<utility::json::JsonArray>("sensitivities");
        {
            auto s = sensitivities.get<utility::json::JsonObject>(0);
            EXPECT_EQ("x[0]", s.get<std::string>("variable"));
            EXPECT_FLOAT_EQ(3.0, s.get<double>("coefficient"));
        }

        {
            auto s = sensitivities.get<utility::json::JsonObject>(4);
            EXPECT_EQ("x[4]", s.get<std::string>("variable"));
            EXPECT_FLOAT_EQ(3.0, s.get<double>("coefficient"));
        }
        {
            auto s = sensitivities.get<utility::json::JsonObject>(5);
            EXPECT_EQ("y", s.get<std::string>("variable"));
            EXPECT_FLOAT_EQ(1.0, s.get<double>("coefficient"));
        }
    }
}
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/