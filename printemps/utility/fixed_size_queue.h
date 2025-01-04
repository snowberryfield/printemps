/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_UTILITY_FIXED_SIZE_QUEUE_H__
#define PRINTEMPS_UTILITY_FIXED_SIZE_QUEUE_H__

namespace printemps::utility {
/*****************************************************************************/
template <class T>
class FixedSizeQueue {
   private:
    int           m_max_size;
    std::deque<T> m_queue;

   public:
    /*************************************************************************/
    FixedSizeQueue(void) {
        this->initialize();
    }

    /*************************************************************************/
    FixedSizeQueue(const int a_MAX_SIZE) {
        this->setup(a_MAX_SIZE);
    }

    /*************************************************************************/
    inline void initialize(void) {
        m_max_size = 0;
        m_queue.clear();
    }

    /*************************************************************************/
    inline void setup(const int a_MAX_SIZE) {
        m_max_size = a_MAX_SIZE;
        m_queue.clear();
    }

    /*************************************************************************/
    inline void push(const T a_DATA) {
        if (static_cast<int>(m_queue.size()) == m_max_size) {
            m_queue.pop_front();
        }
        m_queue.push_back(a_DATA);
    }

    /*************************************************************************/
    inline int size(void) const {
        return m_queue.size();
    }

    /*************************************************************************/
    inline int max_size(void) const {
        return m_max_size;
    }

    /*************************************************************************/
    inline const std::deque<T> &queue(void) const {
        return m_queue;
    }

    /*************************************************************************/
    inline T min(void) const {
        return *min_element(m_queue.begin(), m_queue.end());
    }

    /*************************************************************************/
    inline T max(void) const {
        return *max_element(m_queue.begin(), m_queue.end());
    }

    /*************************************************************************/
    inline double average(void) const {
        return std::accumulate(m_queue.begin(), m_queue.end(), 0.0) /
               static_cast<double>(m_queue.size());
    }
};
}  // namespace printemps::utility

#endif
/*****************************************************************************/
// END
/*****************************************************************************/