/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_UTILITY_VECTOR_UTILITY_H__
#define PRINTEMPS_UTILITY_VECTOR_UTILITY_H__

namespace printemps {
namespace utility {
/*****************************************************************************/
template <class T, class A>
constexpr int count_duplicate(const std::vector<T, A> &a_ARRAY) {
    std::set<T> set(a_ARRAY.begin(), a_ARRAY.end());
    return a_ARRAY.size() - set.size();
}

/*****************************************************************************/
template <class T, class A>
constexpr int count(const std::vector<T, A> &a_ARRAY, const T a_VALUE) {
    return std::count(a_ARRAY.begin(), a_ARRAY.end(), a_VALUE);
}

/*****************************************************************************/
template <class T, class A>
constexpr T min(const std::vector<T, A> &a_ARRAY) {
    return *min_element(a_ARRAY.begin(), a_ARRAY.end());
}

/*****************************************************************************/
template <class T, class A>
constexpr T max(const std::vector<T, A> &a_ARRAY) {
    return *max_element(a_ARRAY.begin(), a_ARRAY.end());
}

/*****************************************************************************/
template <class T, class A>
constexpr T max_abs(const std::vector<T, A> &a_ARRAY) {
    return std::max(max(a_ARRAY), -min(a_ARRAY));
}

/*****************************************************************************/
template <class T, class A>
constexpr int argmin(const std::vector<T, A> &a_ARRAY) {
    return (std::distance(a_ARRAY.begin(),
                          min_element(a_ARRAY.begin(), a_ARRAY.end())));
}

/*****************************************************************************/
template <class T, class A>
constexpr int argmax(const std::vector<T, A> &a_ARRAY) {
    return (std::distance(a_ARRAY.begin(),
                          max_element(a_ARRAY.begin(), a_ARRAY.end())));
}

/*****************************************************************************/
inline std::vector<int> sequence(const int a_MIN, const int a_MAX) {
    std::vector<int> result(a_MAX - a_MIN);
    std::iota(result.begin(), result.end(), a_MIN);
    return result;
}

/*****************************************************************************/
inline std::vector<int> sequence(const int a_MAX) {
    std::vector<int> result(a_MAX);
    std::iota(result.begin(), result.end(), 0);
    return result;
}
}  // namespace utility
}  // namespace printemps

#endif
/*****************************************************************************/
// END
/*****************************************************************************/