/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_NEIGHBORHOOD_MOVE_TYPE_H__
#define PRINTEMPS_NEIGHBORHOOD_MOVE_TYPE_H__

namespace printemps::neighborhood {
/*****************************************************************************/
enum class MoveType {
    Binary,                      //
    Integer,                     //
    Selection,                   //
    ExclusiveOR,                 //
    ExclusiveNOR,                //
    InvertedIntegers,            //
    BalancedIntegers,            //
    ConstantSumIntegers,         //
    ConstantDifferenceIntegers,  //
    ConstantRatioIntegers,       //
    Aggregation,                 //
    Precedence,                  //
    VariableBound,               //
    SoftSelection,               //
    TrinomialExclusiveNOR,       //
    Chain,                       //
    TwoFlip,                     //
    UserDefined,                 //
    General,
};

/*****************************************************************************/
static std::unordered_map<std::string, MoveType> MoveTypeMap = {
    {"Binary", MoveType::Binary},
    {"Integer", MoveType::Integer},
    {"Selection", MoveType::Selection},
    {"Exclusive OR", MoveType::ExclusiveOR},
    {"Exclusive NOR", MoveType::ExclusiveNOR},
    {"Inverted Integers", MoveType::InvertedIntegers},
    {"Balanced Integers", MoveType::BalancedIntegers},
    {"Constant Sum Integers", MoveType::ConstantSumIntegers},
    {"Constant Difference Integers", MoveType::ConstantDifferenceIntegers},
    {"Constant Ratio Integers", MoveType::ConstantRatioIntegers},
    {"Aggregation", MoveType::Aggregation},
    {"Precedence", MoveType::Precedence},
    {"Variable Bound", MoveType::VariableBound},
    {"Soft Selection", MoveType::SoftSelection},
    {"Trinomial Exclusive NOR", MoveType::TrinomialExclusiveNOR},
    {"Chain", MoveType::Chain},
    {"Two Flip", MoveType::TwoFlip},
    {"User Defined", MoveType::UserDefined},
    {"General", MoveType::General}};

/*****************************************************************************/
static std::unordered_map<MoveType, std::string> MoveTypeInverseMap = {
    {MoveType::Binary, "Binary"},
    {MoveType::Integer, "Integer"},
    {MoveType::Selection, "Selection"},
    {MoveType::ExclusiveOR, "Exclusive OR"},
    {MoveType::ExclusiveNOR, "Exclusive NOR"},
    {MoveType::InvertedIntegers, "Inverted Integers"},
    {MoveType::BalancedIntegers, "Balanced Integers"},
    {MoveType::ConstantSumIntegers, "Constant Sum Integers"},
    {MoveType::ConstantDifferenceIntegers, "Constant Difference Integers"},
    {MoveType::ConstantRatioIntegers, "Constant Ratio Integers"},
    {MoveType::Aggregation, "Aggregation"},
    {MoveType::Precedence, "Precedence"},
    {MoveType::VariableBound, "Variable Bound"},
    {MoveType::SoftSelection, "Soft Selection"},
    {MoveType::TrinomialExclusiveNOR, "Trinomial Exclusive NOR"},
    {MoveType::Chain, "Chain"},
    {MoveType::TwoFlip, "Two Flip"},
    {MoveType::UserDefined, "User Defined"},
    {MoveType::General, "General"}};
}  // namespace printemps::neighborhood
#endif
/*****************************************************************************/
// END
/*****************************************************************************/