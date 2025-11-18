/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_UTILITY_UNORDERED_SET_UTILITY_H__
#define PRINTEMPS_UTILITY_UNORDERED_SET_UTILITY_H__

namespace printemps::utility {
/*****************************************************************************/
template <class T>
inline std::unordered_set<T> intersection_set(
    const std::unordered_set<T> &a_FIRST,
    const std::unordered_set<T> &a_SECOND) {
    const auto &shorter =
        (a_FIRST.size() < a_SECOND.size()) ? a_FIRST : a_SECOND;
    const auto &longer =
        (a_FIRST.size() < a_SECOND.size()) ? a_SECOND : a_FIRST;

    std::unordered_set<T> result;
    for (const auto &item : shorter) {
        if (longer.find(item) != longer.end()) {
            result.insert(item);
        }
    }
    return result;
}

/*****************************************************************************/
template <class T>
inline void update_intersection_set(std::unordered_set<T>       *a_first,
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
inline std::unordered_set<T> union_set(const std::unordered_set<T> &a_FIRST,
                                       const std::unordered_set<T> &a_SECOND) {
    std::unordered_set<T> result = a_FIRST;
    result.insert(a_SECOND.begin(), a_SECOND.end());
    return result;
}

/*****************************************************************************/
template <class T>
inline void update_union_set(std::unordered_set<T>       *a_first,
                             const std::unordered_set<T> &a_SECOND) {
    a_first->insert(a_SECOND.begin(), a_SECOND.end());
}

/*****************************************************************************/
template <class T>
inline std::vector<T> to_vector(const std::unordered_set<T> &a_UNORDERED_SET) {
    return std::vector<T>(a_UNORDERED_SET.begin(), a_UNORDERED_SET.end());
}

/*****************************************************************************/
template <class T>
inline std::unordered_set<T> from_vector(const std::vector<T> &a_VECTOR) {
    return std::unordered_set<T>(a_VECTOR.begin(), a_VECTOR.end());
}

}  // namespace printemps::utility

#endif
/*****************************************************************************/
// END
/*****************************************************************************/