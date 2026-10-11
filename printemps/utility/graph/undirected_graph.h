/*****************************************************************************/
// Copyright (c) 2020-2026 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_UTILITY_GRAPH_UNDIRECTED_GRAPH_H__
#define PRINTEMPS_UTILITY_GRAPH_UNDIRECTED_GRAPH_H__

#include <vector>
#include <unordered_map>
#include <algorithm>

#include "adjacent_node.h"
#include "graph_base.h"
#include "union_find.h"

namespace printemps::utility::graph {
/*****************************************************************************/
template <class T>
class UndirectedGraph : public GraphBase<T> {
   private:
    std::vector<std::vector<AdjacentNode>> m_adjacency_list;

   public:
    /*************************************************************************/
    UndirectedGraph(void) : GraphBase<T>() {
        this->initialize();
    }

    /*************************************************************************/
    explicit UndirectedGraph(const std::vector<T> &a_NODES) : GraphBase<T>() {
        this->initialize();
        this->add_nodes(a_NODES);
    }

    /*************************************************************************/
    virtual ~UndirectedGraph(void) override = default;

    /*************************************************************************/
    inline void initialize(void) override {
        GraphBase<T>::initialize();
        m_adjacency_list.clear();
    }

    /*************************************************************************/
    inline int add_node(const T     &a_NODE,
                        const double a_WEIGHT = 0.0) override {
        const auto [INDEX, IS_NEW] = this->register_node(a_NODE, a_WEIGHT);
        if (IS_NEW) {
            m_adjacency_list.emplace_back();
        }
        return INDEX;
    }

    /*************************************************************************/
    inline int add_edge(const T &a_FIRST, const T &a_SECOND,
                        const double a_WEIGHT = 0.0) {
        const int FIRST_INDEX  = this->add_node(a_FIRST);
        const int SECOND_INDEX = this->add_node(a_SECOND);

        const int EDGE_INDEX = static_cast<int>(this->m_edges.size());
        this->m_edges.emplace_back(a_FIRST, a_SECOND, a_WEIGHT);

        m_adjacency_list[FIRST_INDEX].emplace_back(SECOND_INDEX, a_WEIGHT,
                                                   EDGE_INDEX);
        m_adjacency_list[SECOND_INDEX].emplace_back(FIRST_INDEX, a_WEIGHT,
                                                    EDGE_INDEX);

        return EDGE_INDEX;
    }

    /*************************************************************************/
    inline bool has_edge(const T &a_FIRST, const T &a_SECOND) const {
        const int FIRST_INDEX  = this->node_index(a_FIRST);
        const int SECOND_INDEX = this->node_index(a_SECOND);
        if (FIRST_INDEX < 0 || SECOND_INDEX < 0) {
            return false;
        }

        for (const auto &adj : m_adjacency_list[FIRST_INDEX]) {
            if (adj.node_index == SECOND_INDEX) {
                return true;
            }
        }
        return false;
    }

    /*************************************************************************/
    inline int degree(const T &a_NODE) const {
        const int INDEX = this->node_index(a_NODE);
        if (INDEX < 0) {
            return 0;
        }
        return static_cast<int>(m_adjacency_list[INDEX].size());
    }

    /*************************************************************************/
    inline std::vector<T> neighbors(const T &a_NODE) const {
        const int INDEX = this->node_index(a_NODE);
        if (INDEX < 0) {
            return {};
        }
        std::vector<T> result;
        result.reserve(m_adjacency_list[INDEX].size());
        for (const auto &adj : m_adjacency_list[INDEX]) {
            result.push_back(this->m_nodes[adj.node_index]);
        }
        return result;
    }

    /*************************************************************************/
    inline const std::vector<AdjacentNode> &adjacent_nodes(
        const T &a_NODE) const {
        static const std::vector<AdjacentNode> EMPTY_ADJACENT_NODES;
        const int                              INDEX = this->node_index(a_NODE);
        if (INDEX < 0) {
            return EMPTY_ADJACENT_NODES;
        }
        return m_adjacency_list[INDEX];
    }

    /*************************************************************************/
    inline UndirectedGraph<T> minimum_spanning_tree(void) const {
        std::vector<const Edge<T> *> edge_ptrs;
        edge_ptrs.reserve(this->m_edges.size());
        for (const auto &edge : this->m_edges) {
            edge_ptrs.push_back(&edge);
        }

        std::stable_sort(
            edge_ptrs.begin(), edge_ptrs.end(),
            [](const auto *a_FIRST, const auto *a_SECOND) {
                return a_FIRST->weight < a_SECOND->weight;
            });

        UnionFind<T>       union_find(this->m_nodes);
        UndirectedGraph<T> mst;
        const int          NODES_SIZE = static_cast<int>(this->m_nodes.size());
        for (int i = 0; i < NODES_SIZE; i++) {
            mst.add_node(this->m_nodes[i], this->m_node_weights[i]);
        }

        int       added_edges_count  = 0;
        const int TARGET_EDGES_COUNT = NODES_SIZE > 0 ? NODES_SIZE - 1 : 0;

        for (const auto *edge_ptr : edge_ptrs) {
            if (union_find.has_same_root(edge_ptr->first, edge_ptr->second)) {
                continue;
            }
            union_find.unite(edge_ptr->first, edge_ptr->second);
            mst.add_edge(edge_ptr->first, edge_ptr->second, edge_ptr->weight);
            added_edges_count++;
            if (added_edges_count == TARGET_EDGES_COUNT) {
                break;
            }
        }
        return mst;
    }

    /*************************************************************************/
    inline std::vector<std::vector<T>> connected_components(void) const {
        UnionFind<T> union_find(this->m_nodes);
        for (const auto &edge : this->m_edges) {
            union_find.unite(edge.first, edge.second);
        }
        const auto                  ROOT_GROUPS = union_find.groups();
        std::vector<std::vector<T>> result;
        result.reserve(ROOT_GROUPS.size());
        for (const auto &group : ROOT_GROUPS) {
            std::vector<T> component;
            component.reserve(group.members.size());
            for (const auto &[member, parity] : group.members) {
                component.push_back(member);
            }
            result.push_back(std::move(component));
        }
        return result;
    }

    /*************************************************************************/
    inline bool has_cycle(void) const {
        UnionFind<T> union_find(this->m_nodes);
        for (const auto &edge : this->m_edges) {
            if (union_find.has_same_root(edge.first, edge.second)) {
                return true;
            }
            union_find.unite(edge.first, edge.second);
        }
        return false;
    }

    /*************************************************************************/
    inline bool is_bipartite(void) const {
        UnionFind<T> union_find(this->m_nodes);
        for (const auto &edge : this->m_edges) {
            if (!union_find.unite(edge.first, edge.second, 1)) {
                return false;
            }
        }
        return true;
    }
};
}  // namespace printemps::utility::graph

#endif
/*****************************************************************************/
// END
/*****************************************************************************/
