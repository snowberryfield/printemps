/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MODEL_MOVE_SENSE_H__
#define PRINTEMPS_MODEL_MOVE_SENSE_H__

#include <vector>
#include <unordered_set>

namespace printemps {
namespace model {
/*****************************************************************************/
enum class MoveSense {
    Binary,         //
    Integer,        //
    Selection,      //
    Precedence,     //
    Aggregation,    //
    VariableBound,  //
    Exclusive,      //
    Chain,          //
    UserDefined
};
}  // namespace model
}  // namespace printemps
#endif
/*****************************************************************************/
// END
/*****************************************************************************/