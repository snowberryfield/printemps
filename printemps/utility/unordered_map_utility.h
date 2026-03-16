/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_UTILITY_UNORDERED_MAP_UTILITY_H__
#define PRINTEMPS_UTILITY_UNORDERED_MAP_UTILITY_H__

namespace printemps::utility {
/*****************************************************************************/
template <class T1, class T2>
inline std::pair<std::vector<T1>, std::vector<T2>> to_vector_pair(
    const std::unordered_map<T1, T2>& a_UNORDERED_MAP) {
    std::vector<T1> result_T1;
    std::vector<T2> result_T2;
    result_T1.reserve(a_UNORDERED_MAP.size());
    result_T2.reserve(a_UNORDERED_MAP.size());

    for (const auto& [key, value] : a_UNORDERED_MAP) {
        result_T1.push_back(key);
        result_T2.push_back(value);
    }
    return {result_T1, result_T2};
}

/*****************************************************************************/
template <class T1, class T2>
inline std::vector<std::pair<T1, T2>> to_pair_vector(
    const std::unordered_map<T1, T2>& a_UNORDERED_MAP) {
    std::vector<std::pair<T1, T2>> pair_vector;
    pair_vector.reserve(a_UNORDERED_MAP.size());
    for (const auto& [key, value] : a_UNORDERED_MAP) {
        pair_vector.emplace_back(key, value);
    }
    return pair_vector;
}

/*****************************************************************************/
template <class T1, class T2>
inline T2 distance_l1(const std::unordered_map<T1, T2>& a_FIRST,
                      const std::unordered_map<T1, T2>& a_SECOND) {
    T2                     result = static_cast<T2>(0);
    std::unordered_set<T1> all_keys;
    for (const auto& [k, _] : a_FIRST) {
        all_keys.insert(k);
    }
    for (const auto& [k, _] : a_SECOND) {
        all_keys.insert(k);
    }

    for (const auto& k : all_keys) {
        T2 v1 = a_FIRST.count(k) ? a_FIRST.at(k) : static_cast<T2>(0);
        T2 v2 = a_SECOND.count(k) ? a_SECOND.at(k) : static_cast<T2>(0);
        result += std::abs(v1 - v2);
    }

    return result;
}

/*****************************************************************************/
template <class T1, class T2>
inline int distance_l0(const std::unordered_map<T1, T2>& a_FIRST,
                       const std::unordered_map<T1, T2>& a_SECOND) {
    int                    result = static_cast<T2>(0);
    std::unordered_set<T1> all_keys;
    for (const auto& [k, _] : a_FIRST) {
        all_keys.insert(k);
    }
    for (const auto& [k, _] : a_SECOND) {
        all_keys.insert(k);
    }

    for (const auto& k : all_keys) {
        T2 v1 = a_FIRST.count(k) ? a_FIRST.at(k) : static_cast<T2>(0);
        T2 v2 = a_SECOND.count(k) ? a_SECOND.at(k) : static_cast<T2>(0);
        if (v1 != v2) {
            result++;
        }
    }

    return result;
}

}  // namespace printemps::utility

#endif
/*****************************************************************************/
// END
/*****************************************************************************/