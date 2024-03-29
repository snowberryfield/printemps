/*****************************************************************************/
// Copyright (c) 2020-2023 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_UTILITY_UNORDERED_MAP_UTILITY_H__
#define PRINTEMPS_UTILITY_UNORDERED_MAP_UTILITY_H__

namespace printemps::utility {
/*****************************************************************************/
template <class T1, class T2>
inline std::pair<std::vector<T1>, std::vector<T2>> to_vector_pair(
    const std::unordered_map<T1, T2> &a_UNORDERED_MAP) {
    std::vector<T1> result_T1;
    std::vector<T2> result_T2;
    for (const auto &item : a_UNORDERED_MAP) {
        result_T1.push_back(item.first);
        result_T2.push_back(item.second);
    }
    return std::make_pair(result_T1, result_T2);
}

/*****************************************************************************/
template <class T1, class T2>
inline T2 distance_l1(const std::unordered_map<T1, T2> &a_FIRST,
                      const std::unordered_map<T1, T2> &a_SECOND) {
    T2 result = static_cast<T2>(0);

    if (a_FIRST.size() < a_SECOND.size()) {
        for (const auto &item : a_SECOND) {
            result += std::abs(item.second);
        }

        for (const auto &item : a_FIRST) {
            const auto SHORTER_VALUE = item.second;
            if (a_SECOND.find(item.first) != a_SECOND.end()) {
                const auto LONGER_VALUE = a_SECOND.at(item.first);
                result -= std::abs(LONGER_VALUE);
                result += std::abs(LONGER_VALUE - SHORTER_VALUE);
            } else {
                result += std::abs(SHORTER_VALUE);
            }
        }
        return result;
    } else {
        for (const auto &item : a_FIRST) {
            result += std::abs(item.second);
        }

        for (const auto &item : a_SECOND) {
            const auto SHORTER_VALUE = item.second;
            if (a_FIRST.find(item.first) != a_FIRST.end()) {
                const auto LONGER_VALUE = a_FIRST.at(item.first);
                result -= std::abs(LONGER_VALUE);
                result += std::abs(LONGER_VALUE - SHORTER_VALUE);
            } else {
                result += std::abs(SHORTER_VALUE);
            }
        }
        return result;
    }
}

/*****************************************************************************/
template <class T1, class T2>
inline int distance_l0(const std::unordered_map<T1, T2> &a_FIRST,
                       const std::unordered_map<T1, T2> &a_SECOND) {
    int result = 0;

    if (a_FIRST.size() < a_SECOND.size()) {
        result += a_SECOND.size();
        for (const auto &item : a_FIRST) {
            if (a_SECOND.find(item.first) == a_SECOND.end()) {
                result++;
                continue;
            }
            if (item.second == a_SECOND.at(item.first)) {
                result--;
            }
        }
        return result;
    } else {
        result += a_FIRST.size();
        for (const auto &item : a_SECOND) {
            if (a_FIRST.find(item.first) == a_FIRST.end()) {
                result++;
                continue;
            }
            if (item.second == a_FIRST.at(item.first)) {
                result--;
            }
        }
        return result;
    }
}

}  // namespace printemps::utility

#endif
/*****************************************************************************/
// END
/*****************************************************************************/