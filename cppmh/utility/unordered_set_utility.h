/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef CPPMH_UTILITY_UNORDERED_UTILITY_H__
#define CPPMH_UTILITY_UNORDERED_UTILITY_H__

#include <unordered_set>

namespace cppmh {
namespace utility {
/*****************************************************************************/
template <class T>
inline constexpr std::unordered_set<T> intersection_set(
    const std::unordered_set<T> &a_FIRST,
    const std::unordered_set<T> &a_SECOND) {
    std::unordered_set<T> result;
    if (a_FIRST.size() < a_SECOND.size()) {
        for (const auto &item : a_FIRST) {
            if (a_SECOND.find(item) != a_SECOND.end()) {
                result.insert(item);
            }
        }
    } else {
        for (const auto &item : a_SECOND) {
            if (a_FIRST.find(item) != a_FIRST.end()) {
                result.insert(item);
            }
        }
    }
    return result;
}
/*****************************************************************************/
template <class T>
inline constexpr std::unordered_set<T> union_set(
    const std::unordered_set<T> &a_FIRST,
    const std::unordered_set<T> &a_SECOND) {
    std::unordered_set<T> result = a_FIRST;
    result.insert(a_SECOND.begin(), a_SECOND.end());
    return result;
}
}  // namespace utility
}  // namespace cppmh

#endif
/*****************************************************************************/
// END
/*****************************************************************************/