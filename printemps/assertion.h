/*****************************************************************************/
// Copyright (c) 2020-2026 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_ASSERTION_H__
#define PRINTEMPS_ASSERTION_H__

/**
 * C++ Standard requirement: C++17 or later
 */
#if __cplusplus < 201703L && (!defined(_MSVC_LANG) || _MSVC_LANG < 201703L)
#error "PRINTEMPS requires C++17 or later."
#endif

/**
 * 64-bit architecture requirement (32-bit is not supported).
 */
static_assert(sizeof(void *) == 8,
              "PRINTEMPS requires a 64-bit architecture (32-bit is not supported).");

/**
 * Fundamental data type size assumptions.
 */
static_assert(sizeof(int) == 4,
              "PRINTEMPS assumes 32-bit integer (sizeof(int) == 4).");
static_assert(sizeof(double) == 8,
              "PRINTEMPS assumes 64-bit double (sizeof(double) == 8).");

#endif
/*****************************************************************************/
// END
/*****************************************************************************/
