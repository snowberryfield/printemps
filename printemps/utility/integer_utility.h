/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_UTILITY_INTEGER_UTILITY_H__
#define PRINTEMPS_UTILITY_INTEGER_UTILITY_H__

#include <cmath>

namespace printemps::utility {
/******************************************************************************/
template <class T>
inline bool is_integer(const T a_VALUE) {
    const T floor_val = std::floor(a_VALUE);
    if (a_VALUE == floor_val) {
        return true;
    }
    return std::abs(a_VALUE - floor_val) < constant::EPSILON_10;
};

/******************************************************************************/
inline int decimal_places(const double a_VALUE,
                          const int    a_MAX_DECIMAL_PLACES = 6) {
    if (is_integer(a_VALUE)) {
        return 0;
    }
    static constexpr double POW10[] = {1.0,     10.0,     100.0,    1000.0,
                                       10000.0, 100000.0, 1000000.0};
    const int               limit   = std::min(a_MAX_DECIMAL_PLACES, 6);
    for (int k = 1; k <= limit; k++) {
        const double SCALED_VALUE = a_VALUE * POW10[k];
        if (is_integer(SCALED_VALUE)) {
            return k;
        }
    }
    for (int k = 7; k <= a_MAX_DECIMAL_PLACES; k++) {
        const double SCALED_VALUE = a_VALUE * std::pow(10.0, k);
        if (is_integer(SCALED_VALUE)) {
            return k;
        }
    }
    return a_MAX_DECIMAL_PLACES;
}

/******************************************************************************/
inline int luby_value(const int a_INDEX) noexcept {
    int x    = (a_INDEX > 0) ? (a_INDEX - 1) : 0;
    int size = 1;
    int seq  = 0;
    while (size < x + 1) {
        size = (size << 1) + 1;
        seq++;
    }
    while (size - 1 != x) {
        size = (size - 1) >> 1;
        seq--;
        x = x % size;
    }
    if (seq >= 30) {
        return 1 << 30;
    }
    return 1 << seq;
}
}  // namespace printemps::utility

/******************************************************************************/
// END
/******************************************************************************/
#endif