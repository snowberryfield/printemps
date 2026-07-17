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
struct UnionFindRootGroup {
    T                              root;
    std::vector<std::pair<T, int>> members;
};

/*************************************************************************/
template <class T>
class UnionFind {
   private:
    std::vector<int> m_parents;
    std::vector<int> m_sizes;
    std::vector<int> m_parities;

    std::vector<T>             m_elements;
    std::unordered_map<T, int> m_index;

    /*************************************************************************/
    inline int add_node_if_absent(const T& a_ELEMENT) {
        auto it = m_index.find(a_ELEMENT);
        if (it != m_index.end()) {
            return it->second;
        }

        const int ID = static_cast<int>(m_parents.size());

        m_index[a_ELEMENT] = ID;
        m_elements.push_back(a_ELEMENT);

        m_parents.push_back(ID);
        m_sizes.push_back(1);
        m_parities.push_back(0);

        return ID;
    }

    /*************************************************************************/
    inline std::pair<int, int> root_with_id_and_parity(const int a_ID) {
        int current_id = a_ID;
        int parity     = 0;

        // 1st pass: find root and total parity
        while (m_parents[current_id] != current_id) {
            parity ^= m_parities[current_id];
            current_id = m_parents[current_id];
        }

        int root_id = current_id;

        // 2nd pass: path compression
        current_id             = a_ID;
        int accumulated_parity = 0;

        while (m_parents[current_id] != root_id) {
            int parent_id     = m_parents[current_id];
            int parent_parity = m_parities[current_id];

            m_parents[current_id]  = root_id;
            m_parities[current_id] = parity ^ accumulated_parity;

            accumulated_parity ^= parent_parity;
            current_id = parent_id;
        }

        return {root_id, parity};
    }

    /*************************************************************************/
    inline std::pair<int, int> root_with_id_and_parity_const(
        const int a_ID) const {
        int current_id = a_ID;
        int parity     = 0;

        while (m_parents[current_id] != current_id) {
            parity ^= m_parities[current_id];
            current_id = m_parents[current_id];
        }

        return {current_id, parity};
    }

   public:
    /*************************************************************************/
    UnionFind(void) {
        this->initialize();
    }

    /*************************************************************************/
    UnionFind(const std::vector<T>& a_ELEMENTS) {
        this->initialize();
        this->setup(a_ELEMENTS);
    }

    /*************************************************************************/
    inline void initialize(void) {
        m_parents.clear();
        m_sizes.clear();
        m_parities.clear();
        m_elements.clear();
        m_index.clear();
    }

    /*************************************************************************/
    inline void setup(const std::vector<T>& a_ELEMENTS) {
        for (const auto& element : a_ELEMENTS) {
            this->add_node_if_absent(element);
        }
    }

    /*************************************************************************/
    inline std::pair<T, int> root_with_parity(const T& a_ELEMENT) {
        int id                 = this->add_node_if_absent(a_ELEMENT);
        auto [root_id, parity] = this->root_with_id_and_parity(id);
        return {m_elements[root_id], parity};
    }

    /*************************************************************************/
    inline T root(const T& a_ELEMENT) {
        return this->root_with_parity(a_ELEMENT).first;
    }

    /*************************************************************************/
    inline bool unite(const T& a_FIRST, const T& a_SECOND,
                      const int a_PARITY = 0) {
        const int ID_FIRST  = this->add_node_if_absent(a_FIRST);
        const int ID_SECOND = this->add_node_if_absent(a_SECOND);

        const auto [ROOT_FIRST, PARITY_FIRST] =
            this->root_with_id_and_parity(ID_FIRST);

        const auto [ROOT_SECOND, PARITY_SECOND] =
            this->root_with_id_and_parity(ID_SECOND);

        if (ROOT_FIRST == ROOT_SECOND) {
            return ((PARITY_FIRST ^ PARITY_SECOND) == a_PARITY);
        }

        int parent_root   = 0;
        int child_root    = 0;
        int parent_parity = 0;
        int child_parity  = 0;

        if (m_sizes[ROOT_FIRST] > m_sizes[ROOT_SECOND] ||
            (m_sizes[ROOT_FIRST] == m_sizes[ROOT_SECOND] &&
             ROOT_FIRST < ROOT_SECOND)) {
            parent_root   = ROOT_FIRST;
            child_root    = ROOT_SECOND;
            parent_parity = PARITY_FIRST;
            child_parity  = PARITY_SECOND;
        } else {
            parent_root   = ROOT_SECOND;
            child_root    = ROOT_FIRST;
            parent_parity = PARITY_SECOND;
            child_parity  = PARITY_FIRST;
        }

        m_parents[child_root]  = parent_root;
        m_parities[child_root] = parent_parity ^ child_parity ^ a_PARITY;
        m_sizes[parent_root] += m_sizes[child_root];

        return true;
    }

    /*************************************************************************/
    inline bool has_same_root(const T& a_FIRST, const T& a_SECOND) const {
        auto it_first  = m_index.find(a_FIRST);
        auto it_second = m_index.find(a_SECOND);

        if (it_first == m_index.end() || it_second == m_index.end()) {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "Specified element is not included."));
        }

        const int ID_FIRST  = m_index.at(a_FIRST);
        const int ID_SECOND = m_index.at(a_SECOND);

        return this->root_with_id_and_parity_const(ID_FIRST).first ==
               this->root_with_id_and_parity_const(ID_SECOND).first;
    }

    /*************************************************************************/
    inline int parity_between(const T& a_FIRST, const T& a_SECOND) const {
        auto it_first  = m_index.find(a_FIRST);
        auto it_second = m_index.find(a_SECOND);

        if (it_first == m_index.end() || it_second == m_index.end()) {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "Specified element is not included."));
        }

        const int ID_FIRST  = m_index.at(a_FIRST);
        const int ID_SECOND = m_index.at(a_SECOND);

        const auto [ROOT_ID_FIRST, PARITY_FIRST] =
            this->root_with_id_and_parity_const(ID_FIRST);

        auto [ROOT_ID_SECOND, PARITY_SECOND] =
            this->root_with_id_and_parity_const(ID_SECOND);

        if (ROOT_ID_FIRST != ROOT_ID_SECOND) {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__, "Elements are not connected."));
        }

        return PARITY_FIRST ^ PARITY_SECOND;
    }

    /*************************************************************************/
    inline std::vector<UnionFindRootGroup<T>> groups(void) const {
        std::vector<UnionFindRootGroup<T>> result;
        const int        ELEMENTS_SIZE = static_cast<int>(m_elements.size());
        std::vector<int> root_to_group(ELEMENTS_SIZE, -1);
        for (int i = 0; i < ELEMENTS_SIZE; ++i) {
            const auto [ROOT_ID, PARITY] =
                this->root_with_id_and_parity_const(i);

            if (root_to_group[ROOT_ID] == -1) {
                root_to_group[ROOT_ID] = static_cast<int>(result.size());
                UnionFindRootGroup<T> group;
                group.root = m_elements[ROOT_ID];
                result.push_back(group);
            }

            result[root_to_group[ROOT_ID]].members.emplace_back(m_elements[i],
                                                                PARITY);
        }

        return result;
    }

    /*************************************************************************/
    inline const std::vector<int>& parents(void) const noexcept {
        return m_parents;
    };

    /*************************************************************************/
    inline const std::vector<int>& sizes(void) const noexcept {
        return m_sizes;
    };

    /*************************************************************************/
    inline const std::vector<int>& parities(void) const noexcept {
        return m_parities;
    };
};
}  // namespace printemps::utility
/*****************************************************************************/
#endif
/*****************************************************************************/