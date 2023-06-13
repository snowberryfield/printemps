/*****************************************************************************/
// Copyright (c) 2020-2023 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/

#ifndef PRINTEMPS_UTILITY_TIME_KEEPER_H__
#define PRINTEMPS_UTILITY_TIME_KEEPER_H__

namespace printemps::utility {
/*****************************************************************************/
class TimeKeeper {
   private:
    double m_start_time;
    double m_elapsed_time;
    bool   m_is_started;

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
    }

    /*************************************************************************/
    virtual ~TimeKeeper(void) {
        /// nothing to do
    }

    /*************************************************************************/
    inline void set_start_time(void) {
        m_start_time = get_time();
        m_is_started = true;
    }

    /*************************************************************************/
    inline constexpr void initialize(void) {
        m_start_time   = 0.0;
        m_elapsed_time = 0.0;
        m_is_started   = false;
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

    /*************************************************************************/
    inline constexpr bool is_started(void) const {
        return m_is_started;
    }
};
}  // namespace printemps::utility

#endif
/*****************************************************************************/
// END
/*****************************************************************************/