/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/

#ifndef PRINTEMPS_UTILITY_RANGE_H__
#define PRINTEMPS_UTILITY_RANGE_H__

namespace printemps::utility {
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
    inline void update(const T a_VALUE) {
        m_min = std::min(m_min, a_VALUE);
        m_max = std::max(m_max, a_VALUE);
    }

    /*************************************************************************/
    inline void initialize(void) {
        m_min = std::numeric_limits<T>::max();
        m_max = std::numeric_limits<T>::lowest();
    }

    /*************************************************************************/
    inline T min(void) const noexcept {
        return m_min;
    }

    /*************************************************************************/
    inline T max(void) const noexcept {
        return m_max;
    }

    /*************************************************************************/
    inline T max_abs(void) const noexcept {
        return std::max(std::fabs(m_max), std::fabs(m_min));
    }

    /*************************************************************************/
    inline T range(void) const noexcept {
        return m_max - m_min;
    }
};
}  // namespace printemps::utility

#endif
/*****************************************************************************/
// END
/*****************************************************************************/