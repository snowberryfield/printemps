/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_UTILITY_INTEGER_UTILITY_H__
#define PRINTEMPS_UTILITY_INTEGER_UTILITY_H__

namespace printemps::utility {
/******************************************************************************/
template <class T>
inline bool is_integer(const T a_VALUE) {
    return std::abs(a_VALUE - floor(a_VALUE)) < constant::EPSILON_10;
};
}  // namespace printemps::utility

/******************************************************************************/
// END
/******************************************************************************/
#endif