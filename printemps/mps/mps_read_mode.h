/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MPS_MPS_READ_MODE_H__
#define PRINTEMPS_MPS_MPS_READ_MODE_H__

namespace printemps::mps {
/*****************************************************************************/
enum class MPSReadMode {
    Initial,
    Name,
    Objsense,
    Objname,
    Rows,
    Columns,
    Rhs,
    Ranges,
    Bounds,
    Endata
};

/*****************************************************************************/
static inline std::unordered_map<std::string, MPSReadMode> MPSReadModeMap = {
    {"INITIAL", MPSReadMode::Initial},    //
    {"NAME", MPSReadMode::Name},          //
    {"OBJSENSE", MPSReadMode::Objsense},  //
    {"OBJNAME", MPSReadMode::Objname},    //
    {"ROWS", MPSReadMode::Rows},          //
    {"COLUMNS", MPSReadMode::Columns},    //
    {"RHS", MPSReadMode::Rhs},            //
    {"RANGES", MPSReadMode::Ranges},      //
    {"BOUNDS", MPSReadMode::Bounds},      //
    {"ENDATA", MPSReadMode::Endata}};

/*****************************************************************************/
static inline std::unordered_map<MPSReadMode, std::string>
    MPSReadModeInverseMap = {                 //
        {MPSReadMode::Initial, "INITIAL"},    //
        {MPSReadMode::Name, "NAME"},          //
        {MPSReadMode::Objsense, "OBJSENSE"},  //
        {MPSReadMode::Objname, "OBJNAME"},    //
        {MPSReadMode::Rows, "ROWS"},          //
        {MPSReadMode::Columns, "COLUMNS"},    //
        {MPSReadMode::Rhs, "RHS"},            //
        {MPSReadMode::Ranges, "RANGES"},      //
        {MPSReadMode::Bounds, "BOUNDS"},      //
        {MPSReadMode::Endata, "ENDATA"}};
}  // namespace printemps::mps
#endif
/*****************************************************************************/
// END
/*****************************************************************************/