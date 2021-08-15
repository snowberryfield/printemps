/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_UTILITY_UNORDERED_MAP_UTILITY_H__
#define PRINTEMPS_UTILITY_UNORDERED_MAP_UTILITY_H__

namespace printemps {
namespace utility {
/*****************************************************************************/
template <class T1, class T2>
inline std::pair<std::vector<T1>, std::vector<T2>> to_vector_pair(
    const std::unordered_map<T1, T2> &a_UNORDERED_SET) {
    std::vector<T1> result_T1;
    std::vector<T2> result_T2;
    for (const auto &item : a_UNORDERED_SET) {
        result_T1.push_back(item.first);
        result_T2.push_back(item.second);
    }
    return std::make_pair(result_T1, result_T2);
}

}  // namespace utility
}  // namespace printemps

#endif
/*****************************************************************************/
// END
/*****************************************************************************/