/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_NEIGHBORHOOD_MOVE_SENSE_H__
#define PRINTEMPS_NEIGHBORHOOD_MOVE_SENSE_H__

namespace printemps::neighborhood {
/*****************************************************************************/
enum class MoveSense {
    Binary,         //
    Integer,        //
    Selection,      //
    Aggregation,    //
    Precedence,     //
    VariableBound,  //
    SoftSelection,  //
    Chain,          //
    TwoFlip,        //
    UserDefined,    //
    General
};
}  // namespace printemps::neighborhood
#endif
/*****************************************************************************/
// END
/*****************************************************************************/