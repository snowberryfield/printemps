/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_UTILITY_UNION_FIND_H__
#define PRINTEMPS_UTILITY_UNION_FIND_H__

namespace printemps::utility {
/*****************************************************************************/
template <class T>
class UnionFind {
   private:
    std::unordered_map<T, T>    m_parents;
    std::unordered_map<T, bool> m_has_parent;
    std::unordered_map<T, int>  m_sizes;

   public:
    /*************************************************************************/
    UnionFind(void) {
        this->initialize();
    }

    /*************************************************************************/
    UnionFind(const std::unordered_set<T> &a_ELEMENTS) {
        this->setup(a_ELEMENTS);
    }

    /*************************************************************************/
    inline void initialize(void) {
        m_parents.clear();
        m_has_parent.clear();
        m_sizes.clear();
    }

    /*************************************************************************/
    inline void setup(const std::unordered_set<T> &a_ELEMENTS) {
        for (const auto &element : a_ELEMENTS) {
            m_parents[element]    = element;
            m_has_parent[element] = false;
            m_sizes[element]      = 1;
        }
    }

    /*************************************************************************/
    inline T root(const T &a_ELEMENT) {
        if (!m_has_parent[a_ELEMENT]) {
            return a_ELEMENT;
        } else {
            return m_parents[a_ELEMENT] = this->root(m_parents[a_ELEMENT]);
        }
    }

    /*************************************************************************/
    inline bool is_same(const T &a_ELEMENT_FIRST, const T &a_ELEMENT_SECOND) {
        return this->root(a_ELEMENT_FIRST) == this->root(a_ELEMENT_SECOND);
    }

    /*************************************************************************/
    inline bool unite(const T &a_ELEMENT_FIRST, const T &a_ELEMENT_SECOND) {
        auto root_first  = this->root(a_ELEMENT_FIRST);
        auto root_second = this->root(a_ELEMENT_SECOND);

        if (root_first == root_second) {
            return false;
        }

        if (m_sizes[a_ELEMENT_FIRST] < m_sizes[a_ELEMENT_SECOND]) {
            std::swap(root_first, root_second);
        }

        m_parents[root_second]    = root_first;
        m_has_parent[root_second] = true;
        m_sizes[a_ELEMENT_FIRST] += m_sizes[a_ELEMENT_SECOND];
        return true;
    }

    /*************************************************************************/
    inline const std::unordered_map<T, T> &parents(void) const {
        return m_parents;
    }

    /*************************************************************************/
    inline const std::unordered_map<T, int> &sizes(void) const {
        return m_sizes;
    }
};
}  // namespace printemps::utility

#endif
/*****************************************************************************/
// END
/*****************************************************************************/