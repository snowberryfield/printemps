/*****************************************************************************/
// Copyright (c) 2020-2026 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_UTILITY_INTENSITY_H__
#define PRINTEMPS_UTILITY_INTENSITY_H__

#include <cstdint>
#include <unordered_map>

namespace printemps::utility {

/*****************************************************************************/
/**
 * Computes intensity (Herfindahl-Hirschman Index / concentration measure).
 *   Intensity = (sum of counts^2) / (total_count)^2
 * Range: [1/M, 1.0], where 1.0 indicates absolute concentration (bias),
 * and 1/M indicates perfectly uniform distribution.
 */
template <class T_Key = int, class T_Hash = std::hash<T_Key>>
class Intensity {
   private:
    std::unordered_map<T_Key, int64_t, T_Hash> m_counts;
    int64_t                                    m_total_count;
    double                                     m_sum_of_squares;
    double                                     m_intensity;

   public:
    /*************************************************************************/
    inline Intensity(void) noexcept {
        this->reset();
    }

    /*************************************************************************/
    inline void reset(void) noexcept {
        m_counts.clear();
        m_total_count    = 0;
        m_sum_of_squares = 0.0;
        m_intensity      = 0.0;
    }

    /*************************************************************************/
    inline void increment(const T_Key& a_KEY) noexcept {
        const int64_t PREVIOUS_COUNT = m_counts[a_KEY];
        m_counts[a_KEY]              = PREVIOUS_COUNT + 1;
        this->update_with_previous_count(PREVIOUS_COUNT);
    }

    /*************************************************************************/
    inline void update_with_previous_count(
        const int64_t a_PREVIOUS_COUNT) noexcept {
        m_total_count++;
        // (c + 1)^2 - c^2 = 2 * c + 1
        m_sum_of_squares += 2.0 * static_cast<double>(a_PREVIOUS_COUNT) + 1.0;
        const double TOTAL = static_cast<double>(m_total_count);
        m_intensity        = m_sum_of_squares / (TOTAL * TOTAL);
    }

    /*************************************************************************/
    inline double value(void) const noexcept {
        return m_intensity;
    }

    /*************************************************************************/
    inline int64_t total_count(void) const noexcept {
        return m_total_count;
    }

    /*************************************************************************/
    inline double total_count_reciprocal(void) const noexcept {
        return (m_total_count > 0) ? (1.0 / static_cast<double>(m_total_count))
                                   : 0.0;
    }

    /*************************************************************************/
    inline int64_t count(const T_Key& a_KEY) const noexcept {
        auto it = m_counts.find(a_KEY);
        return (it != m_counts.end()) ? it->second : 0;
    }

    /*************************************************************************/
    inline const std::unordered_map<T_Key, int64_t, T_Hash>& counts(
        void) const noexcept {
        return m_counts;
    }
};

}  // namespace printemps::utility

#endif
/*****************************************************************************/
// END
/*****************************************************************************/
