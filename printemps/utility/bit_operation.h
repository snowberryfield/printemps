/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_UTILITY_BIT_OPERATION_H__
#define PRINTEMPS_UTILITY_BIT_OPERATION_H__

#if defined(_MSC_VER)
#include <intrin.h>
#endif

namespace printemps::utility {
/******************************************************************************/
inline int countr_zero(const std::uint64_t a_VALUE) noexcept {
    if (a_VALUE == 0) {
        return 64;
    }
#if defined(__GNUC__) || defined(__clang__)
    return __builtin_ctzll(a_VALUE);
#elif defined(_MSC_VER)
    unsigned long index;
    _BitScanForward64(&index, a_VALUE);
    return static_cast<int>(index);
#else
    // Fallback
    std::uint64_t val = a_VALUE;
    int count = 0;
    while ((val & 1ULL) == 0) {
        val >>= 1;
        count++;
    }
    return count;
#endif
}

/******************************************************************************/
inline int popcount(const std::uint64_t a_VALUE) noexcept {
#if defined(__GNUC__) || defined(__clang__)
    return __builtin_popcountll(a_VALUE);
#elif defined(_MSC_VER)
    return static_cast<int>(__popcnt64(a_VALUE));
#else
    std::uint64_t val = a_VALUE;
    int count = 0;
    while (val != 0) {
        val &= (val - 1);
        count++;
    }
    return count;
#endif
}

}  // namespace printemps::utility

/******************************************************************************/
// END
/******************************************************************************/
#endif
