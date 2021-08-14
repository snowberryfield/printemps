/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/

#ifndef PRINTEMPS_UTILITY_TIME_KEEPER_H__
#define PRINTEMPS_UTILITY_TIME_KEEPER_H__

namespace printemps {
namespace utility {
/*****************************************************************************/
class TimeKeeper {
   private:
    double m_start_time;
    double m_elapsed_time;

    /*************************************************************************/
    inline double get_time(void) {
#ifdef _OPENMP
        return omp_get_wtime();
#else
        clock_t now = std::clock();
        return static_cast<double>(now) / CLOCKS_PER_SEC;
#endif
    }

   public:
    /*************************************************************************/
    TimeKeeper(void) {
        this->initialize();
        m_start_time = get_time();
    }

    /*************************************************************************/
    virtual ~TimeKeeper(void) {
        /// nothing to do
    }

    /*************************************************************************/
    inline constexpr void initialize(void) {
        m_start_time   = 0.0;
        m_elapsed_time = 0.0;
    }

    /*************************************************************************/
    inline double clock(void) {
        double now     = get_time();
        m_elapsed_time = now - m_start_time;
        return m_elapsed_time;
    }

    /*************************************************************************/
    inline constexpr double start_time(void) const {
        return m_start_time;
    }

    /*************************************************************************/
    inline constexpr double elapsed_time(void) const {
        return m_elapsed_time;
    }
};
}  // namespace utility
}  // namespace printemps

#endif
/*****************************************************************************/
// END
/*****************************************************************************/