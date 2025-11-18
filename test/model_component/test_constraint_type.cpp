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
class TestConstraintType : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestConstraintType, ConstraintTypeMap) {
    using namespace printemps::model_component;

    EXPECT_EQ(ConstraintType::Singleton, ConstraintTypeMap.at("Singleton"));
    EXPECT_EQ(ConstraintType::ExclusiveOR,
              ConstraintTypeMap.at("Exclusive OR"));
    EXPECT_EQ(ConstraintType::ExclusiveNOR,
              ConstraintTypeMap.at("Exclusive NOR"));
    EXPECT_EQ(ConstraintType::InvertedIntegers,
              ConstraintTypeMap.at("Inverted Integers"));
    EXPECT_EQ(ConstraintType::BalancedIntegers,
              ConstraintTypeMap.at("Balanced Integers"));
    EXPECT_EQ(ConstraintType::ConstantSumIntegers,
              ConstraintTypeMap.at("Constant Sum Integers"));
    EXPECT_EQ(ConstraintType::ConstantDifferenceIntegers,
              ConstraintTypeMap.at("Constant Difference Integers"));
    EXPECT_EQ(ConstraintType::ConstantRatioIntegers,
              ConstraintTypeMap.at("Constant Ratio Integers"));
    EXPECT_EQ(ConstraintType::Aggregation, ConstraintTypeMap.at("Aggregation"));
    EXPECT_EQ(ConstraintType::Precedence, ConstraintTypeMap.at("Precedence"));
    EXPECT_EQ(ConstraintType::VariableBound,
              ConstraintTypeMap.at("Variable Bound"));
    EXPECT_EQ(ConstraintType::TrinomialExclusiveNOR,
              ConstraintTypeMap.at("Trinomial Exclusive NOR"));
    EXPECT_EQ(ConstraintType::SetPartitioning,
              ConstraintTypeMap.at("Set Partitioning"));
    EXPECT_EQ(ConstraintType::SetPacking, ConstraintTypeMap.at("Set Packing"));
    EXPECT_EQ(ConstraintType::SetCovering,
              ConstraintTypeMap.at("Set Covering"));
    EXPECT_EQ(ConstraintType::Cardinality, ConstraintTypeMap.at("Cardinality"));
    EXPECT_EQ(ConstraintType::InvariantKnapsack,
              ConstraintTypeMap.at("Invariant Knapsack"));
    EXPECT_EQ(ConstraintType::MultipleCovering,
              ConstraintTypeMap.at("Multiple Covering"));
    EXPECT_EQ(ConstraintType::BinaryFlow, ConstraintTypeMap.at("Binary Flow"));
    EXPECT_EQ(ConstraintType::IntegerFlow,
              ConstraintTypeMap.at("Integer Flow"));
    EXPECT_EQ(ConstraintType::SoftSelection,
              ConstraintTypeMap.at("Soft Selection"));
    EXPECT_EQ(ConstraintType::MinMax, ConstraintTypeMap.at("Min-Max"));
    EXPECT_EQ(ConstraintType::MaxMin, ConstraintTypeMap.at("Max-Min"));
    EXPECT_EQ(ConstraintType::Intermediate,
              ConstraintTypeMap.at("Intermediate"));
    EXPECT_EQ(ConstraintType::EquationKnapsack,
              ConstraintTypeMap.at("Equation Knapsack"));
    EXPECT_EQ(ConstraintType::BinPacking, ConstraintTypeMap.at("Bin Packing"));
    EXPECT_EQ(ConstraintType::Knapsack, ConstraintTypeMap.at("Knapsack"));
    EXPECT_EQ(ConstraintType::IntegerKnapsack,
              ConstraintTypeMap.at("Integer Knapsack"));
    EXPECT_EQ(ConstraintType::GF2, ConstraintTypeMap.at("GF(2)"));
    EXPECT_EQ(ConstraintType::GeneralLinear,
              ConstraintTypeMap.at("General Linear"));
    EXPECT_EQ(ConstraintType::Unknown, ConstraintTypeMap.at("Unknown"));
}

/*****************************************************************************/
TEST_F(TestConstraintType, ConstraintTypeInverseMap) {
    using namespace printemps::model_component;

    EXPECT_EQ("Singleton",
              ConstraintTypeInverseMap.at(ConstraintType::Singleton));
    EXPECT_EQ("Exclusive OR",
              ConstraintTypeInverseMap.at(ConstraintType::ExclusiveOR));
    EXPECT_EQ("Exclusive NOR",
              ConstraintTypeInverseMap.at(ConstraintType::ExclusiveNOR));
    EXPECT_EQ("Inverted Integers",
              ConstraintTypeInverseMap.at(ConstraintType::InvertedIntegers));
    EXPECT_EQ("Balanced Integers",
              ConstraintTypeInverseMap.at(ConstraintType::BalancedIntegers));
    EXPECT_EQ("Constant Sum Integers",
              ConstraintTypeInverseMap.at(ConstraintType::ConstantSumIntegers));
    EXPECT_EQ("Constant Difference Integers",
              ConstraintTypeInverseMap.at(
                  ConstraintType::ConstantDifferenceIntegers));
    EXPECT_EQ(
        "Constant Ratio Integers",
        ConstraintTypeInverseMap.at(ConstraintType::ConstantRatioIntegers));
    EXPECT_EQ("Aggregation",
              ConstraintTypeInverseMap.at(ConstraintType::Aggregation));
    EXPECT_EQ("Precedence",
              ConstraintTypeInverseMap.at(ConstraintType::Precedence));
    EXPECT_EQ("Variable Bound",
              ConstraintTypeInverseMap.at(ConstraintType::VariableBound));
    EXPECT_EQ(
        "Trinomial Exclusive NOR",
        ConstraintTypeInverseMap.at(ConstraintType::TrinomialExclusiveNOR));
    EXPECT_EQ("Set Partitioning",
              ConstraintTypeInverseMap.at(ConstraintType::SetPartitioning));
    EXPECT_EQ("Set Packing",
              ConstraintTypeInverseMap.at(ConstraintType::SetPacking));
    EXPECT_EQ("Set Covering",
              ConstraintTypeInverseMap.at(ConstraintType::SetCovering));
    EXPECT_EQ("Cardinality",
              ConstraintTypeInverseMap.at(ConstraintType::Cardinality));
    EXPECT_EQ("Invariant Knapsack",
              ConstraintTypeInverseMap.at(ConstraintType::InvariantKnapsack));
    EXPECT_EQ("Multiple Covering",
              ConstraintTypeInverseMap.at(ConstraintType::MultipleCovering));
    EXPECT_EQ("Binary Flow",
              ConstraintTypeInverseMap.at(ConstraintType::BinaryFlow));
    EXPECT_EQ("Integer Flow",
              ConstraintTypeInverseMap.at(ConstraintType::IntegerFlow));
    EXPECT_EQ("Soft Selection",
              ConstraintTypeInverseMap.at(ConstraintType::SoftSelection));
    EXPECT_EQ("Min-Max", ConstraintTypeInverseMap.at(ConstraintType::MinMax));
    EXPECT_EQ("Max-Min", ConstraintTypeInverseMap.at(ConstraintType::MaxMin));
    EXPECT_EQ("Intermediate",
              ConstraintTypeInverseMap.at(ConstraintType::Intermediate));
    EXPECT_EQ("Equation Knapsack",
              ConstraintTypeInverseMap.at(ConstraintType::EquationKnapsack));
    EXPECT_EQ("Bin Packing",
              ConstraintTypeInverseMap.at(ConstraintType::BinPacking));
    EXPECT_EQ("Knapsack",
              ConstraintTypeInverseMap.at(ConstraintType::Knapsack));
    EXPECT_EQ("Integer Knapsack",
              ConstraintTypeInverseMap.at(ConstraintType::IntegerKnapsack));
    EXPECT_EQ("GF(2)", ConstraintTypeInverseMap.at(ConstraintType::GF2));
    EXPECT_EQ("General Linear",
              ConstraintTypeInverseMap.at(ConstraintType::GeneralLinear));
    EXPECT_EQ("Unknown", ConstraintTypeInverseMap.at(ConstraintType::Unknown));
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/
