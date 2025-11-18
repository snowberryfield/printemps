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
    T2 result = static_cast<T2>(0);

    const auto& shorter =
        (a_FIRST.size() < a_SECOND.size()) ? a_FIRST : a_SECOND;
    const auto& longer =
        (a_FIRST.size() < a_SECOND.size()) ? a_SECOND : a_FIRST;

    for (const auto& [key, val_long] : longer) {
        result += std::abs(val_long);
    }

    for (const auto& [key, val_short] : shorter) {
        if (auto it = longer.find(key); it != longer.end()) {
            const auto val_long = it->second;
            result -= std::abs(val_long);
            result += std::abs(val_long - val_short);
        } else {
            result += std::abs(val_short);
        }
    }

    return result;
}

/*****************************************************************************/
template <class T1, class T2>
inline int distance_l0(const std::unordered_map<T1, T2>& a_FIRST,
                       const std::unordered_map<T1, T2>& a_SECOND) {
    const auto& shorter =
        (a_FIRST.size() < a_SECOND.size()) ? a_FIRST : a_SECOND;
    const auto& longer =
        (a_FIRST.size() < a_SECOND.size()) ? a_SECOND : a_FIRST;

    int result = static_cast<int>(longer.size());

    for (const auto& [key, val_short] : shorter) {
        if (auto it = longer.find(key); it != longer.end()) {
            if (val_short == it->second) {
                result--;  // 一致するキー・値は距離ゼロ
            }
        } else {
            result++;  // 存在しないキーは +1
        }
    }

    return result;
}

}  // namespace printemps::utility

#endif
/*****************************************************************************/
// END
/*****************************************************************************/