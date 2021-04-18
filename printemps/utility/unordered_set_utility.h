/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_UTILITY_UNORDERED_SET_UTILITY_H__
#define PRINTEMPS_UTILITY_UNORDERED_SET_UTILITY_H__

namespace printemps {
namespace utility {
/*****************************************************************************/
template <class T>
constexpr std::unordered_set<T> intersection_set(
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
constexpr void update_intersection_set(std::unordered_set<T> *      a_first,
                                       const std::unordered_set<T> &a_SECOND) {
    for (auto it = a_first->begin(); it != a_first->end();) {
        if (a_SECOND.find(*it) == a_SECOND.end()) {
            it = a_first->erase(it);
        } else {
            ++it;
        }
    }
}

/*****************************************************************************/
template <class T>
constexpr std::unordered_set<T> union_set(
    const std::unordered_set<T> &a_FIRST,
    const std::unordered_set<T> &a_SECOND) {
    std::unordered_set<T> result = a_FIRST;
    result.insert(a_SECOND.begin(), a_SECOND.end());
    return result;
}

/*****************************************************************************/
template <class T>
constexpr void update_union_set(std::unordered_set<T> *      a_first,
                                const std::unordered_set<T> &a_SECOND) {
    a_first->insert(a_SECOND.begin(), a_SECOND.end());
}

/*****************************************************************************/
template <class T>
constexpr std::vector<T> to_vector(
    const std::unordered_set<T> &a_UNORDERED_SET) {
    std::vector<T> result(a_UNORDERED_SET.begin(), a_UNORDERED_SET.end());
    return result;
}

}  // namespace utility
}  // namespace printemps

#endif
/*****************************************************************************/
// END
/*****************************************************************************/