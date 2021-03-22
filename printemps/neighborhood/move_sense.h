/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_NEIGHBORHOOD_MOVE_SENSE_H__
#define PRINTEMPS_NEIGHBORHOOD_MOVE_SENSE_H__

namespace printemps {
namespace neighborhood {
/*****************************************************************************/
enum class MoveSense {
    Binary,         //
    Integer,        //
    Selection,      //
    Aggregation,    //
    Precedence,     //
    VariableBound,  //
    Exclusive,      //
    Chain,          //
    UserDefined
};
}  // namespace neighborhood
}  // namespace printemps
#endif
/*****************************************************************************/
// END
/*****************************************************************************/