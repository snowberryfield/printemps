/*****************************************************************************/
// Copyright (c) 2020-2026 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_UTILITY_TOPOLOGICAL_SORTER_H__
#define PRINTEMPS_UTILITY_TOPOLOGICAL_SORTER_H__

namespace printemps::utility {
/*****************************************************************************/
template <class T>
class TopologicalSorter {
   private:
    std::vector<T>                m_nodes;
    std::unordered_map<T, int>    m_node_to_index;
    std::vector<std::vector<int>> m_successors;
    std::vector<int>              m_in_degrees;
    bool                          m_has_cycle;

   public:
    /*************************************************************************/
    TopologicalSorter(void) {
        this->initialize();
    }

    /*************************************************************************/
    explicit TopologicalSorter(const std::vector<T> &a_NODES) {
        this->setup(a_NODES);
    }

    /*************************************************************************/
    inline void initialize(void) {
        m_nodes.clear();
        m_node_to_index.clear();
        m_successors.clear();
        m_in_degrees.clear();
        m_has_cycle = false;
    }

    /*************************************************************************/
    inline void setup(const std::vector<T> &a_NODES) {
        this->initialize();
        this->add_nodes(a_NODES);
    }

    /*************************************************************************/
    inline int add_node(const T &a_NODE) {
        auto it = m_node_to_index.find(a_NODE);
        if (it != m_node_to_index.end()) {
            return it->second;
        }

        const int INDEX         = static_cast<int>(m_nodes.size());
        m_node_to_index[a_NODE] = INDEX;
        m_nodes.push_back(a_NODE);
        m_successors.emplace_back();
        m_in_degrees.push_back(0);
        return INDEX;
    }

    /*************************************************************************/
    inline void add_nodes(const std::vector<T> &a_NODES) {
        for (const auto &node : a_NODES) {
            this->add_node(node);
        }
    }

    /*************************************************************************/
    inline bool has_node(const T &a_NODE) const {
        return m_node_to_index.find(a_NODE) != m_node_to_index.end();
    }

    /*************************************************************************/
    inline void add_edge(const T &a_SOURCE, const T &a_TARGET) {
        const int SOURCE_INDEX = this->add_node(a_SOURCE);
        const int TARGET_INDEX = this->add_node(a_TARGET);
        m_successors[SOURCE_INDEX].push_back(TARGET_INDEX);
        m_in_degrees[TARGET_INDEX]++;
    }

    /*************************************************************************/
    inline std::vector<T> sort(void) {
        const int        SIZE       = static_cast<int>(m_nodes.size());
        std::vector<int> in_degrees = m_in_degrees;

        std::vector<int> queue;
        queue.reserve(SIZE);
        for (int i = 0; i < SIZE; i++) {
            if (in_degrees[i] == 0) {
                queue.push_back(i);
            }
        }

        std::vector<T> topo_order;
        topo_order.reserve(SIZE);
        int head = 0;
        while (head < static_cast<int>(queue.size())) {
            const int SOURCE_INDEX = queue[head++];
            topo_order.push_back(m_nodes[SOURCE_INDEX]);
            for (const int TARGET_INDEX : m_successors[SOURCE_INDEX]) {
                in_degrees[TARGET_INDEX]--;
                if (in_degrees[TARGET_INDEX] == 0) {
                    queue.push_back(TARGET_INDEX);
                }
            }
        }

        m_has_cycle = (static_cast<int>(topo_order.size()) != SIZE);
        return topo_order;
    }

    /*************************************************************************/
    inline bool has_cycle(void) const noexcept {
        return m_has_cycle;
    }

    /*************************************************************************/
    inline std::vector<T> successors(const T &a_NODE) const {
        auto it = m_node_to_index.find(a_NODE);
        if (it == m_node_to_index.end()) {
            return {};
        }
        const auto    &successor_indices = m_successors[it->second];
        std::vector<T> result;
        result.reserve(successor_indices.size());
        for (const int index : successor_indices) {
            result.push_back(m_nodes[index]);
        }
        return result;
    }

    /*************************************************************************/
    inline int size(void) const noexcept {
        return static_cast<int>(m_nodes.size());
    }

    /*************************************************************************/
    inline bool empty(void) const noexcept {
        return m_nodes.empty();
    }

    /*************************************************************************/
    inline const std::vector<T> &nodes(void) const noexcept {
        return m_nodes;
    }
};

}  // namespace printemps::utility
#endif
/*****************************************************************************/
// END
/*****************************************************************************/
