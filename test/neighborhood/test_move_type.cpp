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
class TestMoveType : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
    }
    virtual void TearDown() {
    }
};

/*****************************************************************************/
TEST_F(TestMoveType, MoveTypeMap) {
    using namespace printemps::neighborhood;

    EXPECT_EQ(MoveType::Binary, MoveTypeMap.at("Binary"));
    EXPECT_EQ(MoveType::Integer, MoveTypeMap.at("Integer"));
    EXPECT_EQ(MoveType::Selection, MoveTypeMap.at("Selection"));
    EXPECT_EQ(MoveType::ExclusiveOR, MoveTypeMap.at("Exclusive OR"));
    EXPECT_EQ(MoveType::ExclusiveNOR, MoveTypeMap.at("Exclusive NOR"));
    EXPECT_EQ(MoveType::InvertedIntegers, MoveTypeMap.at("Inverted Integers"));
    EXPECT_EQ(MoveType::BalancedIntegers, MoveTypeMap.at("Balanced Integers"));
    EXPECT_EQ(MoveType::ConstantSumIntegers,
              MoveTypeMap.at("Constant Sum Integers"));
    EXPECT_EQ(MoveType::ConstantDifferenceIntegers,
              MoveTypeMap.at("Constant Difference Integers"));
    EXPECT_EQ(MoveType::ConstantRatioIntegers,
              MoveTypeMap.at("Constant Ratio Integers"));
    EXPECT_EQ(MoveType::Aggregation, MoveTypeMap.at("Aggregation"));
    EXPECT_EQ(MoveType::Precedence, MoveTypeMap.at("Precedence"));
    EXPECT_EQ(MoveType::VariableBound, MoveTypeMap.at("Variable Bound"));
    EXPECT_EQ(MoveType::SoftSelection, MoveTypeMap.at("Soft Selection"));
    EXPECT_EQ(MoveType::TrinomialExclusiveNOR,
              MoveTypeMap.at("Trinomial Exclusive NOR"));
    EXPECT_EQ(MoveType::Chain, MoveTypeMap.at("Chain"));
    EXPECT_EQ(MoveType::TwoFlip, MoveTypeMap.at("Two Flip"));
    EXPECT_EQ(MoveType::UserDefined, MoveTypeMap.at("User Defined"));
    EXPECT_EQ(MoveType::General, MoveTypeMap.at("General"));
}

/*****************************************************************************/
TEST_F(TestMoveType, MoveTypeInverseMap) {
    using namespace printemps::neighborhood;

    EXPECT_EQ("Binary", MoveTypeInverseMap.at(MoveType::Binary));
    EXPECT_EQ("Integer", MoveTypeInverseMap.at(MoveType::Integer));
    EXPECT_EQ("Selection", MoveTypeInverseMap.at(MoveType::Selection));
    EXPECT_EQ("Exclusive OR", MoveTypeInverseMap.at(MoveType::ExclusiveOR));
    EXPECT_EQ("Exclusive NOR", MoveTypeInverseMap.at(MoveType::ExclusiveNOR));
    EXPECT_EQ("Inverted Integers",
              MoveTypeInverseMap.at(MoveType::InvertedIntegers));
    EXPECT_EQ("Balanced Integers",
              MoveTypeInverseMap.at(MoveType::BalancedIntegers));
    EXPECT_EQ("Constant Sum Integers",
              MoveTypeInverseMap.at(MoveType::ConstantSumIntegers));
    EXPECT_EQ("Constant Difference Integers",
              MoveTypeInverseMap.at(MoveType::ConstantDifferenceIntegers));
    EXPECT_EQ("Constant Ratio Integers",
              MoveTypeInverseMap.at(MoveType::ConstantRatioIntegers));
    EXPECT_EQ("Aggregation", MoveTypeInverseMap.at(MoveType::Aggregation));
    EXPECT_EQ("Precedence", MoveTypeInverseMap.at(MoveType::Precedence));
    EXPECT_EQ("Variable Bound", MoveTypeInverseMap.at(MoveType::VariableBound));
    EXPECT_EQ("Soft Selection", MoveTypeInverseMap.at(MoveType::SoftSelection));
    EXPECT_EQ("Trinomial Exclusive NOR",
              MoveTypeInverseMap.at(MoveType::TrinomialExclusiveNOR));
    EXPECT_EQ("Chain", MoveTypeInverseMap.at(MoveType::Chain));
    EXPECT_EQ("Two Flip", MoveTypeInverseMap.at(MoveType::TwoFlip));
    EXPECT_EQ("User Defined", MoveTypeInverseMap.at(MoveType::UserDefined));
    EXPECT_EQ("General", MoveTypeInverseMap.at(MoveType::General));
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/