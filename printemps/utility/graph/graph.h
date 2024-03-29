/*****************************************************************************/
// Copyright (c) 2020-2023 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include "node.h"
#include "edge.h"

namespace printemps::utility::graph {
/*****************************************************************************/
template <class T>
class Graph {
   private:
    std::unordered_map<T, Node<T>> m_nodes;
    std::vector<Edge<T>>           m_edges;

   public:
    /*************************************************************************/
    Graph(void) {
        this->initialize();
    }

    /*************************************************************************/
    inline void initialize(void) {
        m_nodes.clear();
        m_edges.clear();
    }

    /*************************************************************************/
    inline void add_edge(const T a_NODE_KEY_FIRST, const T a_NODE_KEY_SECOND) {
        if (m_nodes.find(a_NODE_KEY_FIRST) == m_nodes.end()) {
            m_nodes[a_NODE_KEY_FIRST] = Node(a_NODE_KEY_FIRST);
        }

        if (m_nodes.find(a_NODE_KEY_SECOND) == m_nodes.end()) {
            m_nodes[a_NODE_KEY_SECOND] = Node(a_NODE_KEY_SECOND);
        }

        m_nodes[a_NODE_KEY_FIRST].m_edge_indices.insert(m_edges.size());
        m_nodes[a_NODE_KEY_SECOND].m_edge_indices.insert(m_edges.size());
        m_edges.emplace_back(a_NODE_KEY_FIRST, a_NODE_KEY_SECOND);
    }

    /*************************************************************************/
    inline void add_edge(const T a_NODE_KEY_FIRST, const T a_NODE_KEY_SECOND,
                         const double a_WEIGHT) {
        if (m_nodes.find(a_NODE_KEY_FIRST) == m_nodes.end()) {
            m_nodes[a_NODE_KEY_FIRST] = Node(a_NODE_KEY_FIRST);
        }

        if (m_nodes.find(a_NODE_KEY_SECOND) == m_nodes.end()) {
            m_nodes[a_NODE_KEY_SECOND] = Node(a_NODE_KEY_SECOND);
        }

        m_nodes[a_NODE_KEY_FIRST].m_edge_indices.insert(m_edges.size());
        m_nodes[a_NODE_KEY_SECOND].m_edge_indices.insert(m_edges.size());
        m_edges.emplace_back(a_NODE_KEY_FIRST, a_NODE_KEY_SECOND, a_WEIGHT);
    }

    /*************************************************************************/
    inline void add_edge(const Edge<T> &a_EDGE) {
        const auto &NODE_KEY_FIRST  = a_EDGE.node_keys().first;
        const auto &NODE_KEY_SECOND = a_EDGE.node_keys().second;

        if (m_nodes.find(NODE_KEY_FIRST) == m_nodes.end()) {
            m_nodes[NODE_KEY_FIRST] = Node(NODE_KEY_FIRST);
        }

        if (m_nodes.find(NODE_KEY_SECOND) == m_nodes.end()) {
            m_nodes[NODE_KEY_SECOND] = Node(NODE_KEY_SECOND);
        }

        m_nodes[NODE_KEY_FIRST].m_edge_indices.insert(m_edges.size());
        m_nodes[NODE_KEY_SECOND].m_edge_indices.insert(m_edges.size());
        m_edges.push_back(a_EDGE);
    }

    /*************************************************************************/
    inline void add_node(const T a_NODE_KEY) {
        m_nodes[a_NODE_KEY] = Node(a_NODE_KEY);
    }

    /*************************************************************************/
    inline void add_node(const T a_NODE_KEY, const double a_WEIGHT) {
        m_nodes[a_NODE_KEY] = Node(a_NODE_KEY, a_WEIGHT);
    }

    /*************************************************************************/
    inline void add_node(const Node<T> &a_NODE) {
        m_nodes[a_NODE.key()] = a_NODE;
    }

    /*************************************************************************/
    inline Graph<T> minimum_spanning_tree(void) noexcept {
        std::vector<Edge<T> *> edge_ptrs;
        for (auto &&edge : m_edges) {
            edge_ptrs.push_back(&edge);
        }

        std::stable_sort(edge_ptrs.begin(), edge_ptrs.end(),
                         [this](const auto &a_EDGE_PTR_FIRST,
                                const auto &a_EDGE_PTR_SECOND) {
                             return a_EDGE_PTR_FIRST->weight() <
                                    a_EDGE_PTR_SECOND->weight();
                         });

        std::unordered_set<T> total_node_keys;
        for (const auto &node : m_nodes) {
            total_node_keys.insert(node.first);
        }

        UnionFind uf(total_node_keys);

        std::vector<Edge<T> *> added_edge_ptrs;
        for (const auto &edge_ptr : edge_ptrs) {
            auto &node_keys = edge_ptr->node_keys();

            if (uf.is_same(node_keys.first, node_keys.second)) {
                continue;
            } else {
                added_edge_ptrs.push_back(edge_ptr);
                uf.unite(node_keys.first, node_keys.second);
            }

            if (added_edge_ptrs.size() == total_node_keys.size() - 1) {
                break;
            }
        }

        Graph<T> result;
        for (const auto &node : m_nodes) {
            result.add_node(node.first);
        }
        for (const auto &edge_ptr : added_edge_ptrs) {
            result.add_edge(*edge_ptr);
        }
        return result;
    }

    /*************************************************************************/
    inline constexpr std::unordered_map<int, Node<T>> &nodes(void) noexcept {
        return m_nodes;
    }

    /*************************************************************************/
    inline constexpr std::unordered_map<int, Node<T>> const &nodes(void) const
        noexcept {
        return m_nodes;
    }

    /*************************************************************************/
    inline constexpr std::vector<Edge<T>> &edges(void) noexcept {
        return m_edges;
    }

    /*************************************************************************/
    inline constexpr std::vector<Edge<T>> const &edges(void) const noexcept {
        return m_edges;
    }
};
}  // namespace printemps::utility::graph
/*****************************************************************************/
// END
/*****************************************************************************/