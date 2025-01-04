/*****************************************************************************/
// Copyright (c) 2020-2024 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
using namespace printemps;
/*****************************************************************************/
class TestJson : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestJson, add_integer_item) {
    using JsonObject = utility::json::JsonObject;
    using JsonArray  = utility::json::JsonArray;
    JsonObject obj;
    obj.emplace_back("key", 0);

    EXPECT_EQ(0, obj.get<int>("key"));
    EXPECT_EQ(0, std::any_cast<int>(obj["key"]));

    ASSERT_THROW(obj.get<double>("key"), std::runtime_error);
    ASSERT_THROW(obj.get<std::string>("key"), std::runtime_error);
    ASSERT_THROW(obj.get<JsonArray>("key"), std::runtime_error);
    ASSERT_THROW(obj.get<JsonObject>("key"), std::runtime_error);
}

/*****************************************************************************/
TEST_F(TestJson, add_double_item) {
    using JsonObject = utility::json::JsonObject;
    using JsonArray  = utility::json::JsonArray;
    JsonObject obj;
    obj.emplace_back("key", 3.14);

    EXPECT_FLOAT_EQ(3.14, obj.get<double>("key"));
    EXPECT_FLOAT_EQ(3.14, std::any_cast<double>(obj["key"]));

    ASSERT_THROW(obj.get<int>("key"), std::runtime_error);
    ASSERT_THROW(obj.get<std::string>("key"), std::runtime_error);
    ASSERT_THROW(obj.get<JsonArray>("key"), std::runtime_error);
    ASSERT_THROW(obj.get<JsonObject>("key"), std::runtime_error);
}

/*****************************************************************************/
TEST_F(TestJson, add_string_item) {
    using JsonObject = utility::json::JsonObject;
    using JsonArray  = utility::json::JsonArray;
    JsonObject obj;
    obj.emplace_back("key", std::string("value"));

    EXPECT_EQ("value", obj.get<std::string>("key"));
    EXPECT_EQ("value", std::any_cast<std::string>(obj["key"]));

    ASSERT_THROW(obj.get<int>("key"), std::runtime_error);
    ASSERT_THROW(obj.get<double>("key"), std::runtime_error);
    ASSERT_THROW(obj.get<JsonArray>("key"), std::runtime_error);
    ASSERT_THROW(obj.get<JsonObject>("key"), std::runtime_error);
}

/*****************************************************************************/
TEST_F(TestJson, add_json_object_item) {
    using JsonObject = utility::json::JsonObject;
    using JsonArray  = utility::json::JsonArray;
    JsonObject obj;
    JsonObject sub;
    sub.emplace_back("sub_0", 0);
    sub.emplace_back("sub_1", 3.14);
    sub.emplace_back("sub_2", std::string("value"));
    obj.emplace_back("key", sub);

    EXPECT_EQ(0, obj.get<JsonObject>("key").get<int>("sub_0"));
    EXPECT_FLOAT_EQ(3.14, obj.get<JsonObject>("key").get<double>("sub_1"));
    EXPECT_EQ("value", obj.get<JsonObject>("key").get<std::string>("sub_2"));

    ASSERT_THROW(obj.get<int>("key"), std::runtime_error);
    ASSERT_THROW(obj.get<double>("key"), std::runtime_error);
    ASSERT_THROW(obj.get<std::string>("key"), std::runtime_error);
    ASSERT_THROW(obj.get<JsonArray>("key"), std::runtime_error);
}

/*****************************************************************************/
TEST_F(TestJson, add_json_array_item) {
    using JsonObject = utility::json::JsonObject;
    using JsonArray  = utility::json::JsonArray;
    JsonObject obj;
    JsonArray  sub;
    sub.emplace_back(0);
    sub.emplace_back(3.14);
    sub.emplace_back(std::string("value"));
    obj.emplace_back("key", sub);

    EXPECT_EQ(0, obj.get<JsonArray>("key").get<int>(0));
    EXPECT_FLOAT_EQ(3.14, obj.get<JsonArray>("key").get<double>(1));
    EXPECT_EQ("value", obj.get<JsonArray>("key").get<std::string>(2));

    ASSERT_THROW(obj.get<int>("key"), std::runtime_error);
    ASSERT_THROW(obj.get<double>("key"), std::runtime_error);
    ASSERT_THROW(obj.get<std::string>("key"), std::runtime_error);
    ASSERT_THROW(obj.get<JsonObject>("key"), std::runtime_error);
}

/*****************************************************************************/
TEST_F(TestJson, find) {
    using JsonObject = utility::json::JsonObject;
    JsonObject obj;
    obj.emplace_back("key", 0);

    EXPECT_TRUE(obj.find("key"));
    EXPECT_FALSE(obj.find("key_key"));
}

/*****************************************************************************/
TEST_F(TestJson, is_integer) {
    EXPECT_TRUE(utility::json::is_integer("0"));
    EXPECT_TRUE(utility::json::is_integer("-1"));
    EXPECT_TRUE(utility::json::is_integer("1"));
    EXPECT_FALSE(utility::json::is_integer("3.14"));
    EXPECT_FALSE(utility::json::is_integer("string"));
}

/*****************************************************************************/
TEST_F(TestJson, close_count) {
    EXPECT_EQ(0, utility::json::close_count("{{}}", '{', '}'));
    EXPECT_EQ(1, utility::json::close_count("{{{}}", '{', '}'));
    EXPECT_EQ(-1, utility::json::close_count("{{}}}", '{', '}'));
    EXPECT_EQ(0, utility::json::close_count("{{\"{\"}}", '{', '}'));
}

/*****************************************************************************/
TEST_F(TestJson, find_end_position) {
    std::vector<std::string> tokens = {"{", "{", "}", "}"};
    EXPECT_EQ(3, static_cast<int>(
                     utility::json::find_end_position(tokens, 0, "{", "}")));
    EXPECT_EQ(2, static_cast<int>(
                     utility::json::find_end_position(tokens, 1, "{", "}")));
}

/*****************************************************************************/
TEST_F(TestJson, tokenize) {
    std::string str    = "{\"key_0\":\"hoge hoge\",\"key_1\":[1,2,3]}";
    auto        tokens = utility::json::tokenize(str);
    EXPECT_EQ("{", tokens[0]);
    EXPECT_EQ("\"key_0\"", tokens[1]);
    EXPECT_EQ("\"hoge hoge\"", tokens[2]);
    EXPECT_EQ("\"key_1\"", tokens[3]);
    EXPECT_EQ("[", tokens[4]);
    EXPECT_EQ("1", tokens[5]);
    EXPECT_EQ("2", tokens[6]);
    EXPECT_EQ("3", tokens[7]);
    EXPECT_EQ("]", tokens[8]);
    EXPECT_EQ("}", tokens[9]);
}

/*****************************************************************************/
TEST_F(TestJson, parse_json_object) {
    using JsonObject = utility::json::JsonObject;
    using JsonArray  = utility::json::JsonArray;

    std::string str =
        "{\"key_0\":1,\"key_1\":3.14,\"key_2\":\"hoge "
        "hoge\",\"key_3\":{\"key_3_1\":null},\"key_4\":[1,2]}";

    auto obj = utility::json::parse_json_object(str);

    EXPECT_EQ(1, obj.get<int>("key_0"));
    EXPECT_FLOAT_EQ(3.14, obj.get<double>("key_1"));
    EXPECT_EQ("hoge hoge", obj.get<std::string>("key_2"));
    EXPECT_EQ(nullptr,
              obj.get<JsonObject>("key_3").get<std::nullptr_t>("key_3_1"));
    EXPECT_EQ(1, obj.get<JsonArray>("key_4").get<int>(0));
    EXPECT_EQ(2, obj.get<JsonArray>("key_4").get<int>(1));
}

/*****************************************************************************/
TEST_F(TestJson, parse_json_array) {
    using JsonObject = utility::json::JsonObject;
    using JsonArray  = utility::json::JsonArray;

    std::string str =
        "[{\"key_0\":1},{\"key_1\":3.14},{\"key_2\":\"hoge "
        "hoge\"},{\"key_3\":null},{\"key_4\":[1,2]}]";

    auto array = utility::json::parse_json_array(str);

    EXPECT_EQ(1, array.get<JsonObject>(0).get<int>("key_0"));
    EXPECT_EQ(3.14, array.get<JsonObject>(1).get<double>("key_1"));
    EXPECT_EQ("hoge hoge", array.get<JsonObject>(2).get<std::string>("key_2"));
    EXPECT_EQ(nullptr, array.get<JsonObject>(3).get<std::nullptr_t>("key_3"));
    EXPECT_EQ(1, array.get<JsonObject>(4).get<JsonArray>("key_4").get<int>(0));
    EXPECT_EQ(2, array.get<JsonObject>(4).get<JsonArray>("key_4").get<int>(1));
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/