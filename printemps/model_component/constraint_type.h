/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MODEL_COMPONENT_CONSTRAINT_TYPE_H__
#define PRINTEMPS_MODEL_COMPONENT_CONSTRAINT_TYPE_H__

namespace printemps::model_component {
/*****************************************************************************/
enum class ConstraintType {
    Empty,
    Singleton,
    ExclusiveOR,
    ExclusiveNOR,
    InvertedIntegers,
    BalancedIntegers,
    ConstantSumIntegers,
    ConstantDifferenceIntegers,
    ConstantRatioIntegers,
    Aggregation,
    Precedence,
    VariableBound,
    TrinomialExclusiveNOR,
    SetPartitioning,
    SetPacking,
    SetCovering,
    Cardinality,
    InvariantKnapsack,
    MultipleCovering,
    BinaryFlow,
    IntegerFlow,
    SoftSelection,
    MinMax,
    MaxMin,
    Intermediate,
    EquationKnapsack,
    BinPacking,
    Knapsack,
    IntegerKnapsack,
    GF2,
    GeneralLinear,
    Unknown,
};

/*****************************************************************************/
static std::unordered_map<std::string, ConstraintType> ConstraintTypeMap = {
    {"Empty", ConstraintType::Empty},
    {"Singleton", ConstraintType::Singleton},
    {"Exclusive OR", ConstraintType::ExclusiveOR},
    {"Exclusive NOR", ConstraintType::ExclusiveNOR},
    {"Inverted Integers", ConstraintType::InvertedIntegers},
    {"Balanced Integers", ConstraintType::BalancedIntegers},
    {"Constant Sum Integers", ConstraintType::ConstantSumIntegers},
    {"Constant Difference Integers",
     ConstraintType::ConstantDifferenceIntegers},
    {"Constant Ratio Integers", ConstraintType::ConstantRatioIntegers},
    {"Aggregation", ConstraintType::Aggregation},
    {"Precedence", ConstraintType::Precedence},
    {"Variable Bound", ConstraintType::VariableBound},
    {"Trinomial Exclusive NOR", ConstraintType::TrinomialExclusiveNOR},
    {"Set Partitioning", ConstraintType::SetPartitioning},
    {"Set Packing", ConstraintType::SetPacking},
    {"Set Covering", ConstraintType::SetCovering},
    {"Cardinality", ConstraintType::Cardinality},
    {"Invariant Knapsack", ConstraintType::InvariantKnapsack},
    {"Multiple Covering", ConstraintType::MultipleCovering},
    {"Binary Flow", ConstraintType::BinaryFlow},
    {"Integer Flow", ConstraintType::IntegerFlow},
    {"Soft Selection", ConstraintType::SoftSelection},
    {"Min-Max", ConstraintType::MinMax},
    {"Max-Min", ConstraintType::MaxMin},
    {"Intermediate", ConstraintType::Intermediate},
    {"Equation Knapsack", ConstraintType::EquationKnapsack},
    {"Bin Packing", ConstraintType::BinPacking},
    {"Knapsack", ConstraintType::Knapsack},
    {"Integer Knapsack", ConstraintType::IntegerKnapsack},
    {"GF(2)", ConstraintType::GF2},
    {"General Linear", ConstraintType::GeneralLinear},
    {"Unknown", ConstraintType::Unknown},
};

/*****************************************************************************/
static std::unordered_map<ConstraintType, std::string>
    ConstraintTypeInverseMap = {
        {ConstraintType::Empty, "Empty"},
        {ConstraintType::Singleton, "Singleton"},
        {ConstraintType::ExclusiveOR, "Exclusive OR"},
        {ConstraintType::ExclusiveNOR, "Exclusive NOR"},
        {ConstraintType::InvertedIntegers, "Inverted Integers"},
        {ConstraintType::BalancedIntegers, "Balanced Integers"},
        {ConstraintType::ConstantSumIntegers, "Constant Sum Integers"},
        {ConstraintType::ConstantDifferenceIntegers,
         "Constant Difference Integers"},
        {ConstraintType::ConstantRatioIntegers, "Constant Ratio Integers"},
        {ConstraintType::Aggregation, "Aggregation"},
        {ConstraintType::Precedence, "Precedence"},
        {ConstraintType::VariableBound, "Variable Bound"},
        {ConstraintType::TrinomialExclusiveNOR, "Trinomial Exclusive NOR"},
        {ConstraintType::SetPartitioning, "Set Partitioning"},
        {ConstraintType::SetPacking, "Set Packing"},
        {ConstraintType::SetCovering, "Set Covering"},
        {ConstraintType::Cardinality, "Cardinality"},
        {ConstraintType::InvariantKnapsack, "Invariant Knapsack"},
        {ConstraintType::MultipleCovering, "Multiple Covering"},
        {ConstraintType::BinaryFlow, "Binary Flow"},
        {ConstraintType::IntegerFlow, "Integer Flow"},
        {ConstraintType::SoftSelection, "Soft Selection"},
        {ConstraintType::MinMax, "Min-Max"},
        {ConstraintType::MaxMin, "Max-Min"},
        {ConstraintType::Intermediate, "Intermediate"},
        {ConstraintType::EquationKnapsack, "Equation Knapsack"},
        {ConstraintType::BinPacking, "Bin Packing"},
        {ConstraintType::Knapsack, "Knapsack"},
        {ConstraintType::IntegerKnapsack, "Integer Knapsack"},
        {ConstraintType::GF2, "GF(2)"},
        {ConstraintType::GeneralLinear, "General Linear"},
        {ConstraintType::Unknown, "Unknown"},
};

}  // namespace printemps::model_component
#endif
/*****************************************************************************/
// END
/*****************************************************************************/