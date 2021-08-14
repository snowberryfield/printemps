/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_UTILITY_BIDIRECTIONAL_MAP_H__
#define PRINTEMPS_UTILITY_BIDIRECTIONAL_MAP_H__

namespace printemps {
namespace utility {
/*****************************************************************************/
template <class T1, class T2>
class BidirectionalMap {
   private:
    std::unordered_map<T1, T2> m_forward;
    std::unordered_map<T2, T1> m_reverse;

   public:
    /*************************************************************************/
    BidirectionalMap(void) {
        this->initialize();
    }

    /*************************************************************************/
    inline void initialize(void) {
        m_forward.clear();
        m_reverse.clear();
    }

    /*************************************************************************/
    inline constexpr int size(void) const {
        return m_forward.size();
    }

    /*************************************************************************/
    inline constexpr bool empty(void) const {
        return m_forward.empty() && m_reverse.empty();
    }

    /*************************************************************************/
    inline constexpr void insert(const T1 &a_FIRST, const T2 &a_SECOND) {
        m_forward[a_FIRST]  = a_SECOND;
        m_reverse[a_SECOND] = a_FIRST;
    }

    /*************************************************************************/
    inline constexpr T2 &operator[](const T1 a_KEY) {
        return m_forward[a_KEY];
    }

    /*************************************************************************/
    inline constexpr T1 &operator[](const T2 a_KEY) {
        return m_reverse[a_KEY];
    }
};
}  // namespace utility
}  // namespace printemps

#endif
/*****************************************************************************/
// END
/*****************************************************************************/