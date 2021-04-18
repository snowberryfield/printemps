/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <string>

#include <printemps.h>

namespace {
/*****************************************************************************/
class TestUnorderedSetUtility : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestUnorderedSetUtility, intersection_set) {
    std::unordered_set<int>         set_int_0    = {1, 2, 3, 4, 5};
    std::unordered_set<int>         set_int_1    = {3, 4, 5, 6, 7};
    std::unordered_set<std::string> set_string_0 = {"a", "b", "c", "d", "e"};
    std::unordered_set<std::string> set_string_1 = {"c", "d", "e", "f", "g"};

    auto intersection_int =
        printemps::utility::intersection_set(set_int_0, set_int_1);

    auto intersection_str =
        printemps::utility::intersection_set(set_string_0, set_string_1);

    EXPECT_FALSE(intersection_int.find(1) != intersection_int.end());
    EXPECT_FALSE(intersection_int.find(2) != intersection_int.end());
    EXPECT_TRUE(intersection_int.find(3) != intersection_int.end());
    EXPECT_TRUE(intersection_int.find(4) != intersection_int.end());
    EXPECT_TRUE(intersection_int.find(5) != intersection_int.end());
    EXPECT_FALSE(intersection_int.find(6) != intersection_int.end());
    EXPECT_FALSE(intersection_int.find(7) != intersection_int.end());

    EXPECT_FALSE(intersection_str.find("a") != intersection_str.end());
    EXPECT_FALSE(intersection_str.find("b") != intersection_str.end());
    EXPECT_TRUE(intersection_str.find("c") != intersection_str.end());
    EXPECT_TRUE(intersection_str.find("d") != intersection_str.end());
    EXPECT_TRUE(intersection_str.find("e") != intersection_str.end());
    EXPECT_FALSE(intersection_str.find("f") != intersection_str.end());
    EXPECT_FALSE(intersection_str.find("g") != intersection_str.end());
}

/*****************************************************************************/
TEST_F(TestUnorderedSetUtility, update_intersection_set) {
    std::unordered_set<int>         set_int_0    = {1, 2, 3, 4, 5};
    std::unordered_set<int>         set_int_1    = {3, 4, 5, 6, 7};
    std::unordered_set<std::string> set_string_0 = {"a", "b", "c", "d", "e"};
    std::unordered_set<std::string> set_string_1 = {"c", "d", "e", "f", "g"};

    printemps::utility::update_intersection_set(&set_int_0, set_int_1);
    printemps::utility::update_intersection_set(&set_string_0, set_string_1);

    EXPECT_FALSE(set_int_0.find(1) != set_int_0.end());
    EXPECT_FALSE(set_int_0.find(2) != set_int_0.end());
    EXPECT_TRUE(set_int_0.find(3) != set_int_0.end());
    EXPECT_TRUE(set_int_0.find(4) != set_int_0.end());
    EXPECT_TRUE(set_int_0.find(5) != set_int_0.end());
    EXPECT_FALSE(set_int_0.find(6) != set_int_0.end());
    EXPECT_FALSE(set_int_0.find(7) != set_int_0.end());

    EXPECT_FALSE(set_string_0.find("a") != set_string_0.end());
    EXPECT_FALSE(set_string_0.find("b") != set_string_0.end());
    EXPECT_TRUE(set_string_0.find("c") != set_string_0.end());
    EXPECT_TRUE(set_string_0.find("d") != set_string_0.end());
    EXPECT_TRUE(set_string_0.find("e") != set_string_0.end());
    EXPECT_FALSE(set_string_0.find("f") != set_string_0.end());
    EXPECT_FALSE(set_string_0.find("g") != set_string_0.end());
}

/*****************************************************************************/
TEST_F(TestUnorderedSetUtility, union_set) {
    std::unordered_set<int>         set_int_0    = {1, 2, 3, 4, 5};
    std::unordered_set<int>         set_int_1    = {3, 4, 5, 6, 7};
    std::unordered_set<std::string> set_string_0 = {"a", "b", "c", "d", "e"};
    std::unordered_set<std::string> set_string_1 = {"c", "d", "e", "f", "g"};

    printemps::utility::update_union_set(&set_int_0, set_int_1);
    printemps::utility::update_union_set(&set_string_0, set_string_1);

    EXPECT_TRUE(set_int_0.find(1) != set_int_0.end());
    EXPECT_TRUE(set_int_0.find(2) != set_int_0.end());
    EXPECT_TRUE(set_int_0.find(3) != set_int_0.end());
    EXPECT_TRUE(set_int_0.find(4) != set_int_0.end());
    EXPECT_TRUE(set_int_0.find(5) != set_int_0.end());
    EXPECT_TRUE(set_int_0.find(6) != set_int_0.end());
    EXPECT_TRUE(set_int_0.find(7) != set_int_0.end());

    EXPECT_TRUE(set_string_0.find("a") != set_string_0.end());
    EXPECT_TRUE(set_string_0.find("b") != set_string_0.end());
    EXPECT_TRUE(set_string_0.find("c") != set_string_0.end());
    EXPECT_TRUE(set_string_0.find("d") != set_string_0.end());
    EXPECT_TRUE(set_string_0.find("e") != set_string_0.end());
    EXPECT_TRUE(set_string_0.find("f") != set_string_0.end());
    EXPECT_TRUE(set_string_0.find("g") != set_string_0.end());
}

/*****************************************************************************/
TEST_F(TestUnorderedSetUtility, update_union_set) {
    std::unordered_set<int>         set_int_0    = {1, 2, 3, 4, 5};
    std::unordered_set<int>         set_int_1    = {3, 4, 5, 6, 7};
    std::unordered_set<std::string> set_string_0 = {"a", "b", "c", "d", "e"};
    std::unordered_set<std::string> set_string_1 = {"c", "d", "e", "f", "g"};

    auto union_int = printemps::utility::union_set(set_int_0, set_int_1);
    auto union_str = printemps::utility::union_set(set_string_0, set_string_1);

    EXPECT_TRUE(union_int.find(1) != union_int.end());
    EXPECT_TRUE(union_int.find(2) != union_int.end());
    EXPECT_TRUE(union_int.find(3) != union_int.end());
    EXPECT_TRUE(union_int.find(4) != union_int.end());
    EXPECT_TRUE(union_int.find(5) != union_int.end());
    EXPECT_TRUE(union_int.find(6) != union_int.end());
    EXPECT_TRUE(union_int.find(7) != union_int.end());

    EXPECT_TRUE(union_str.find("a") != union_str.end());
    EXPECT_TRUE(union_str.find("b") != union_str.end());
    EXPECT_TRUE(union_str.find("c") != union_str.end());
    EXPECT_TRUE(union_str.find("d") != union_str.end());
    EXPECT_TRUE(union_str.find("e") != union_str.end());
    EXPECT_TRUE(union_str.find("f") != union_str.end());
    EXPECT_TRUE(union_str.find("g") != union_str.end());
}

/*****************************************************************************/
TEST_F(TestUnorderedSetUtility, to_vector) {
    std::unordered_set<int>         set_int    = {1, 2, 3, 4, 5};
    std::unordered_set<std::string> set_string = {"a", "b", "c", "d", "e"};

    auto vector_int    = printemps::utility::to_vector(set_int);
    auto vector_string = printemps::utility::to_vector(set_string);

    EXPECT_TRUE(std::find(vector_int.begin(), vector_int.end(), 1) !=
                vector_int.end());
    EXPECT_TRUE(std::find(vector_int.begin(), vector_int.end(), 2) !=
                vector_int.end());
    EXPECT_TRUE(std::find(vector_int.begin(), vector_int.end(), 3) !=
                vector_int.end());
    EXPECT_TRUE(std::find(vector_int.begin(), vector_int.end(), 4) !=
                vector_int.end());
    EXPECT_TRUE(std::find(vector_int.begin(), vector_int.end(), 5) !=
                vector_int.end());

    EXPECT_TRUE(std::find(vector_string.begin(), vector_string.end(), "a") !=
                vector_string.end());
    EXPECT_TRUE(std::find(vector_string.begin(), vector_string.end(), "b") !=
                vector_string.end());
    EXPECT_TRUE(std::find(vector_string.begin(), vector_string.end(), "c") !=
                vector_string.end());
    EXPECT_TRUE(std::find(vector_string.begin(), vector_string.end(), "d") !=
                vector_string.end());
    EXPECT_TRUE(std::find(vector_string.begin(), vector_string.end(), "e") !=
                vector_string.end());
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/