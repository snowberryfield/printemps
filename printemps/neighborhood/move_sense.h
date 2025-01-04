/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_NEIGHBORHOOD_MOVE_SENSE_H__
#define PRINTEMPS_NEIGHBORHOOD_MOVE_SENSE_H__

namespace printemps::neighborhood {
/*****************************************************************************/
enum class MoveSense {
    Binary,                      //
    Integer,                     //
    Selection,                   //
    ExclusiveOr,                 //
    ExclusiveNor,                //
    InvertedIntegers,            //
    BalancedIntegers,            //
    ConstantSumIntegers,         //
    ConstantDifferenceIntegers,  //
    ConstantRatioIntegers,       //
    Aggregation,                 //
    Precedence,                  //
    VariableBound,               //
    SoftSelection,               //
    TrinomialExclusiveNor,       //
    Chain,                       //
    TwoFlip,                     //
    UserDefined,                 //
    General
};
}  // namespace printemps::neighborhood
#endif
/*****************************************************************************/
// END
/*****************************************************************************/