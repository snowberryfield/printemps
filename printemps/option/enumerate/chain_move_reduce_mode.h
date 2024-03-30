/*****************************************************************************/
// Copyright (c) 2020-2024 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_OPTION_CHAIN_MOVE_REDUCE_MODE_H__
#define PRINTEMPS_OPTION_CHAIN_MOVE_REDUCE_MODE_H__

namespace printemps::option::chain_move_reduce_mode {
/*****************************************************************************/
enum ChainMoveReduceMode : int {  //
    OverlapRate,
    Shuffle
};

/*****************************************************************************/
static std::unordered_map<std::string, ChainMoveReduceMode>
    ChainMoveReduceModeMap = {  //
        {"OverlapRate", ChainMoveReduceMode::OverlapRate},
        {"Shuffle", ChainMoveReduceMode::Shuffle}};

/*****************************************************************************/
static std::unordered_map<ChainMoveReduceMode, std::string>
    ChainMoveReduceModeInverseMap = {  //
        {ChainMoveReduceMode::OverlapRate, "OverlapRate"},
        {ChainMoveReduceMode::Shuffle, "Shuffle"}};

}  // namespace printemps::option::chain_move_reduce_mode
#endif
/*****************************************************************************/
// END
/*****************************************************************************/