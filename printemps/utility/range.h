/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/

#ifndef PRINTEMPS_UTILITY_RANGE_H__
#define PRINTEMPS_UTILITY_RANGE_H__

namespace printemps {
namespace utility {
/*****************************************************************************/
template <class T>
class Range {
   private:
    T m_min;
    T m_max;

   public:
    /*************************************************************************/
    Range(void) {
        this->initialize();
    }

    /*************************************************************************/
    virtual ~Range(void) {
        /// nothing to do
    }

    /*************************************************************************/
    inline void update(const T a_VALUE) {
        m_min = std::min(m_min, a_VALUE);
        m_max = std::max(m_max, a_VALUE);
    }

    /*************************************************************************/
    inline constexpr void initialize(void) {
        m_min = std::numeric_limits<T>::max();
        m_max = std::numeric_limits<T>::lowest();
    }

    /*************************************************************************/
    inline constexpr T min(void) const noexcept {
        return m_min;
    }

    /*************************************************************************/
    inline constexpr T max(void) const noexcept {
        return m_max;
    }

    /*************************************************************************/
    inline constexpr T max_abs(void) const noexcept {
        return std::max(fabs(m_max), fabs(m_min));
    }

    /*************************************************************************/
    inline constexpr T range(void) const noexcept {
        return m_max - m_min;
    }
};
}  // namespace utility
}  // namespace printemps

#endif
/*****************************************************************************/
// END
/*****************************************************************************/