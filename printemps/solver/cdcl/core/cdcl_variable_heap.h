/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_CDCL_CORE_CDCL_VARIABLE_HEAP_H__
#define PRINTEMPS_SOLVER_CDCL_CORE_CDCL_VARIABLE_HEAP_H__

#include "cdcl_core_constants.h"

namespace printemps::solver::cdcl::core {
/*****************************************************************************/
class CDCLVariableHeap {
   public:
    static constexpr int NOT_IN_HEAP = -1;

   private:
    std::vector<int> m_heap_array;
    std::vector<int> m_positions;

    /*************************************************************************/
    inline void swap_nodes(const int a_POSITION_FIRST,
                           const int a_POSITION_SECOND) noexcept {
        const int VARIABLE_INDEX_FIRST  = m_heap_array[a_POSITION_FIRST];
        const int VARIABLE_INDEX_SECOND = m_heap_array[a_POSITION_SECOND];
        std::swap(m_heap_array[a_POSITION_FIRST],
                  m_heap_array[a_POSITION_SECOND]);
        m_positions[VARIABLE_INDEX_FIRST]  = a_POSITION_SECOND;
        m_positions[VARIABLE_INDEX_SECOND] = a_POSITION_FIRST;
    }

   public:
    /*************************************************************************/
    inline bool greater_than(
        const int a_VARIABLE_INDEX_FIRST, const int a_VARIABLE_INDEX_SECOND,
        const std::vector<double>& a_ACTIVITY) const noexcept {
        if (a_ACTIVITY[a_VARIABLE_INDEX_FIRST] !=
            a_ACTIVITY[a_VARIABLE_INDEX_SECOND]) {
            return a_ACTIVITY[a_VARIABLE_INDEX_FIRST] >
                   a_ACTIVITY[a_VARIABLE_INDEX_SECOND];
        }
        return a_VARIABLE_INDEX_FIRST < a_VARIABLE_INDEX_SECOND;
    }

    /*************************************************************************/
    CDCLVariableHeap(void) {
        initialize();
    }

    /*************************************************************************/
    inline void initialize(void) noexcept {
        m_heap_array.clear();
        m_positions.clear();
    }

    /*************************************************************************/
    inline void setup(const int                  a_NUMBER_OF_VARIABLES,
                      const std::vector<double>& a_ACTIVITY) {
        initialize();
        m_heap_array.resize(a_NUMBER_OF_VARIABLES);
        m_positions.assign(a_NUMBER_OF_VARIABLES, NOT_IN_HEAP);

        for (int i = 0; i < a_NUMBER_OF_VARIABLES; i++) {
            m_heap_array[i] = i;
            m_positions[i]  = i;
        }

        // Heapify (build heap in O(N) via sift-down from last parent).
        for (int i = a_NUMBER_OF_VARIABLES / 2 - 1; i >= 0; i--) {
            sift_down(i, a_ACTIVITY);
        }
    }

    /*************************************************************************/
    inline bool empty(void) const noexcept {
        return m_heap_array.empty();
    }

    /*************************************************************************/
    inline int size(void) const noexcept {
        return static_cast<int>(m_heap_array.size());
    }

    /*************************************************************************/
    inline bool contains(const int a_VARIABLE_INDEX) const noexcept {
        if (a_VARIABLE_INDEX < 0 ||
            a_VARIABLE_INDEX >= static_cast<int>(m_positions.size())) {
            return false;
        }
        return m_positions[a_VARIABLE_INDEX] != NOT_IN_HEAP;
    }

    /*************************************************************************/
    inline int position_of(const int a_VARIABLE_INDEX) const noexcept {
        if (a_VARIABLE_INDEX < 0 ||
            a_VARIABLE_INDEX >= static_cast<int>(m_positions.size())) {
            return CDCLVariableHeap::NOT_IN_HEAP;
        }
        return m_positions[a_VARIABLE_INDEX];
    }

    /*************************************************************************/
    inline const std::vector<int>& heap_array(void) const noexcept {
        return m_heap_array;
    }

    /*************************************************************************/
    inline const std::vector<int>& heap_positions(void) const noexcept {
        return m_positions;
    }

    /*************************************************************************/
    inline void sift_up(const int                  a_POSITION,
                        const std::vector<double>& a_ACTIVITY) noexcept {
        int       position        = a_POSITION;
        const int TARGET_VARIABLE = m_heap_array[position];
        while (position > 0) {
            const int PARENT          = (position - 1) / 2;
            const int PARENT_VARIABLE = m_heap_array[PARENT];
            if (greater_than(TARGET_VARIABLE, PARENT_VARIABLE, a_ACTIVITY)) {
                m_heap_array[position]       = PARENT_VARIABLE;
                m_positions[PARENT_VARIABLE] = position;
                position                     = PARENT;
            } else {
                break;
            }
        }
        m_heap_array[position]       = TARGET_VARIABLE;
        m_positions[TARGET_VARIABLE] = position;
    }

    /*************************************************************************/
    inline void sift_down(const int                  a_POSITION,
                          const std::vector<double>& a_ACTIVITY) noexcept {
        int       position            = a_POSITION;
        const int NUMBER_OF_VARIABLES = static_cast<int>(m_heap_array.size());
        const int TARGET_VARIABLE     = m_heap_array[position];
        while (true) {
            int       largest          = position;
            int       largest_variable = TARGET_VARIABLE;
            const int LEFT             = 2 * position + 1;
            const int RIGHT            = 2 * position + 2;
            if (LEFT < NUMBER_OF_VARIABLES &&
                greater_than(m_heap_array[LEFT], largest_variable,
                             a_ACTIVITY)) {
                largest          = LEFT;
                largest_variable = m_heap_array[LEFT];
            }
            if (RIGHT < NUMBER_OF_VARIABLES &&
                greater_than(m_heap_array[RIGHT], largest_variable,
                             a_ACTIVITY)) {
                largest          = RIGHT;
                largest_variable = m_heap_array[RIGHT];
            }
            if (largest == position) {
                break;
            }
            m_heap_array[position]          = largest_variable;
            m_positions[largest_variable]   = position;
            position                        = largest;
        }
        m_heap_array[position]       = TARGET_VARIABLE;
        m_positions[TARGET_VARIABLE] = position;
    }

    /*************************************************************************/
    inline void push(const int                  a_VARIABLE_INDEX,
                     const std::vector<double>& a_ACTIVITY) noexcept {
        if (a_VARIABLE_INDEX < 0 ||
            a_VARIABLE_INDEX >= static_cast<int>(m_positions.size()) ||
            contains(a_VARIABLE_INDEX)) {
            return;
        }
        const int POSITION = static_cast<int>(m_heap_array.size());
        m_heap_array.push_back(a_VARIABLE_INDEX);
        m_positions[a_VARIABLE_INDEX] = POSITION;
        sift_up(POSITION, a_ACTIVITY);
    }

    /*************************************************************************/
    inline void update_after_bump(
        const int                  a_VARIABLE_INDEX,
        const std::vector<double>& a_ACTIVITY) noexcept {
        const int POSITION = position_of(a_VARIABLE_INDEX);
        if (POSITION != NOT_IN_HEAP) {
            sift_up(POSITION, a_ACTIVITY);
        }
    }

    /*************************************************************************/
    inline int pop(const std::vector<double>& a_ACTIVITY) noexcept {
        if (m_heap_array.empty()) {
            return CDCLCoreConstant::NO_VARIABLE;
        }
        const int VARIABLE_INDEX    = m_heap_array[0];
        m_positions[VARIABLE_INDEX] = NOT_IN_HEAP;
        if (m_heap_array.size() == 1) {
            m_heap_array.pop_back();
            return VARIABLE_INDEX;
        }
        const int LAST_VARIABLE = m_heap_array.back();
        m_heap_array.pop_back();
        m_heap_array[0]            = LAST_VARIABLE;
        m_positions[LAST_VARIABLE] = 0;
        sift_down(0, a_ACTIVITY);
        return VARIABLE_INDEX;
    }

    /*************************************************************************/
    inline void remove(const int                  a_VARIABLE_INDEX,
                       const std::vector<double>& a_ACTIVITY) noexcept {
        const int POSITION = position_of(a_VARIABLE_INDEX);
        if (POSITION == NOT_IN_HEAP) {
            return;
        }
        const int LAST_POSITION = static_cast<int>(m_heap_array.size()) - 1;
        if (POSITION == LAST_POSITION) {
            m_positions[a_VARIABLE_INDEX] = NOT_IN_HEAP;
            m_heap_array.pop_back();
            return;
        }
        const int LAST_VARIABLE = m_heap_array.back();
        m_heap_array.pop_back();
        m_positions[a_VARIABLE_INDEX] = NOT_IN_HEAP;
        m_heap_array[POSITION]        = LAST_VARIABLE;
        m_positions[LAST_VARIABLE]    = POSITION;
        sift_down(POSITION, a_ACTIVITY);
        sift_up(POSITION, a_ACTIVITY);
    }
};

}  // namespace printemps::solver::cdcl::core

#endif
/*****************************************************************************/
// END
/*****************************************************************************/
