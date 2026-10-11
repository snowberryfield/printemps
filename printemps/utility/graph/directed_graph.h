/*****************************************************************************/
// Copyright (c) 2020-2026 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_UTILITY_GRAPH_DIRECTED_GRAPH_H__
#define PRINTEMPS_UTILITY_GRAPH_DIRECTED_GRAPH_H__

#include "adjacent_node.h"
#include "abstract_graph.h"

namespace printemps::utility::graph {
/*****************************************************************************/
template <class T>
class DirectedGraph : public AbstractBase<T> {
   private:
    std::vector<std::vector<AdjacentNode>> m_successors;
    std::vector<std::vector<AdjacentNode>> m_predecessors;
    std::vector<int>                       m_in_degrees;

    /*************************************************************************/
    void dfs_tarjan(const int a_NODE_INDEX, int &a_current_order,
                    std::vector<int> &a_dfs_order, std::vector<int> &a_lowlink,
                    std::vector<bool> &a_is_on_stack, std::vector<int> &a_stack,
                    std::vector<std::vector<T>> &a_components) const {
        a_dfs_order[a_NODE_INDEX] = a_lowlink[a_NODE_INDEX] = a_current_order++;
        a_stack.push_back(a_NODE_INDEX);
        a_is_on_stack[a_NODE_INDEX] = true;

        for (const auto &adj : m_successors[a_NODE_INDEX]) {
            const int NEXT_INDEX = adj.node_index;
            if (a_dfs_order[NEXT_INDEX] < 0) {
                this->dfs_tarjan(NEXT_INDEX, a_current_order, a_dfs_order,
                                 a_lowlink, a_is_on_stack, a_stack,
                                 a_components);
                a_lowlink[a_NODE_INDEX] =
                    std::min(a_lowlink[a_NODE_INDEX], a_lowlink[NEXT_INDEX]);
            } else if (a_is_on_stack[NEXT_INDEX]) {
                a_lowlink[a_NODE_INDEX] =
                    std::min(a_lowlink[a_NODE_INDEX], a_dfs_order[NEXT_INDEX]);
            }
        }

        if (a_lowlink[a_NODE_INDEX] == a_dfs_order[a_NODE_INDEX]) {
            std::vector<T> component;
            while (true) {
                const int V = a_stack.back();
                a_stack.pop_back();
                a_is_on_stack[V] = false;
                component.push_back(this->m_nodes[V]);
                if (V == a_NODE_INDEX) {
                    break;
                }
            }
            a_components.push_back(std::move(component));
        }
    }

   public:
    /*************************************************************************/
    DirectedGraph(void) : AbstractBase<T>() {
        this->initialize();
    }

    /*************************************************************************/
    explicit DirectedGraph(const std::vector<T> &a_NODES) : AbstractBase<T>() {
        this->initialize();
        this->add_nodes(a_NODES);
    }

    /*************************************************************************/
    virtual ~DirectedGraph(void) override = default;

    /*************************************************************************/
    inline void initialize(void) override {
        AbstractBase<T>::initialize();
        m_successors.clear();
        m_predecessors.clear();
        m_in_degrees.clear();
    }

    /*************************************************************************/
    inline int add_node(const T &a_NODE, const double a_WEIGHT = 0.0) override {
        const auto [INDEX, IS_NEW] = this->register_node(a_NODE, a_WEIGHT);
        if (IS_NEW) {
            m_successors.emplace_back();
            m_predecessors.emplace_back();
            m_in_degrees.push_back(0);
        }
        return INDEX;
    }

    /*************************************************************************/
    inline int add_edge(const T &a_SOURCE, const T &a_TARGET,
                        const double a_WEIGHT = 0.0) {
        const int SOURCE_INDEX = this->add_node(a_SOURCE);
        const int TARGET_INDEX = this->add_node(a_TARGET);

        const int EDGE_INDEX = static_cast<int>(this->m_edges.size());
        this->m_edges.emplace_back(a_SOURCE, a_TARGET, a_WEIGHT);

        m_successors[SOURCE_INDEX].emplace_back(TARGET_INDEX, a_WEIGHT,
                                                EDGE_INDEX);
        m_predecessors[TARGET_INDEX].emplace_back(SOURCE_INDEX, a_WEIGHT,
                                                  EDGE_INDEX);
        m_in_degrees[TARGET_INDEX]++;

        return EDGE_INDEX;
    }

    /*************************************************************************/
    inline bool has_edge(const T &a_SOURCE, const T &a_TARGET) const {
        const int SOURCE_INDEX = this->node_index(a_SOURCE);
        const int TARGET_INDEX = this->node_index(a_TARGET);
        if (SOURCE_INDEX < 0 || TARGET_INDEX < 0) {
            return false;
        }

        for (const auto &adj : m_successors[SOURCE_INDEX]) {
            if (adj.node_index == TARGET_INDEX) {
                return true;
            }
        }
        return false;
    }

    /*************************************************************************/
    inline int in_degree(const T &a_NODE) const {
        const int INDEX = this->node_index(a_NODE);
        if (INDEX < 0) {
            return 0;
        }
        return m_in_degrees[INDEX];
    }

    /*************************************************************************/
    inline int out_degree(const T &a_NODE) const {
        const int INDEX = this->node_index(a_NODE);
        if (INDEX < 0) {
            return 0;
        }
        return static_cast<int>(m_successors[INDEX].size());
    }

    /*************************************************************************/
    inline std::vector<T> successors(const T &a_NODE) const {
        const int INDEX = this->node_index(a_NODE);
        if (INDEX < 0) {
            return {};
        }
        std::vector<T> result;
        result.reserve(m_successors[INDEX].size());
        for (const auto &adj : m_successors[INDEX]) {
            result.push_back(this->m_nodes[adj.node_index]);
        }
        return result;
    }

    /*************************************************************************/
    inline std::vector<T> predecessors(const T &a_NODE) const {
        const int INDEX = this->node_index(a_NODE);
        if (INDEX < 0) {
            return {};
        }
        std::vector<T> result;
        result.reserve(m_predecessors[INDEX].size());
        for (const auto &adj : m_predecessors[INDEX]) {
            result.push_back(this->m_nodes[adj.node_index]);
        }
        return result;
    }

    /*************************************************************************/
    inline const std::vector<AdjacentNode> &successor_nodes(
        const T &a_NODE) const {
        static const std::vector<AdjacentNode> EMPTY_ADJACENT_NODES;
        const int                              INDEX = this->node_index(a_NODE);
        if (INDEX < 0) {
            return EMPTY_ADJACENT_NODES;
        }
        return m_successors[INDEX];
    }

    /*************************************************************************/
    inline const std::vector<AdjacentNode> &predecessor_nodes(
        const T &a_NODE) const {
        static const std::vector<AdjacentNode> EMPTY_ADJACENT_NODES;
        const int                              INDEX = this->node_index(a_NODE);
        if (INDEX < 0) {
            return EMPTY_ADJACENT_NODES;
        }
        return m_predecessors[INDEX];
    }

    /*************************************************************************/
    inline std::vector<T> topological_sort(void) const {
        const int        SIZE       = static_cast<int>(this->m_nodes.size());
        std::vector<int> in_degrees = m_in_degrees;

        std::vector<int> queue;
        queue.reserve(SIZE);
        for (int i = 0; i < SIZE; i++) {
            if (in_degrees[i] == 0) {
                queue.push_back(i);
            }
        }

        std::vector<T> topological_order;
        topological_order.reserve(SIZE);
        int head = 0;
        while (head < static_cast<int>(queue.size())) {
            const int SOURCE_INDEX = queue[head++];
            topological_order.push_back(this->m_nodes[SOURCE_INDEX]);

            for (const auto &adj : m_successors[SOURCE_INDEX]) {
                const int TARGET_INDEX = adj.node_index;
                in_degrees[TARGET_INDEX]--;
                if (in_degrees[TARGET_INDEX] == 0) {
                    queue.push_back(TARGET_INDEX);
                }
            }
        }

        if (static_cast<int>(topological_order.size()) != SIZE) {
            return {};
        }

        return topological_order;
    }

    /*************************************************************************/
    inline bool has_cycle(void) const {
        const int        SIZE       = static_cast<int>(this->m_nodes.size());
        std::vector<int> in_degrees = m_in_degrees;

        std::vector<int> queue;
        queue.reserve(SIZE);
        for (int i = 0; i < SIZE; i++) {
            if (in_degrees[i] == 0) {
                queue.push_back(i);
            }
        }

        int count = 0;
        int head  = 0;
        while (head < static_cast<int>(queue.size())) {
            const int SOURCE_INDEX = queue[head++];
            count++;

            for (const auto &adj : m_successors[SOURCE_INDEX]) {
                const int TARGET_INDEX = adj.node_index;
                in_degrees[TARGET_INDEX]--;
                if (in_degrees[TARGET_INDEX] == 0) {
                    queue.push_back(TARGET_INDEX);
                }
            }
        }
        return count != SIZE;
    }

    /*************************************************************************/
    inline std::vector<std::vector<T>> strongly_connected_components(
        void) const {
        const int         SIZE = static_cast<int>(this->m_nodes.size());
        std::vector<int>  dfs_order(SIZE, -1);
        std::vector<int>  lowlink(SIZE, 0);
        std::vector<bool> is_on_stack(SIZE, false);
        std::vector<int>  stack;
        stack.reserve(SIZE);

        std::vector<std::vector<T>> components;
        int                         current_order = 0;

        for (int i = 0; i < SIZE; i++) {
            if (dfs_order[i] < 0) {
                this->dfs_tarjan(i, current_order, dfs_order, lowlink,
                                 is_on_stack, stack, components);
            }
        }
        return components;
    }
};
}  // namespace printemps::utility::graph

#endif
/*****************************************************************************/
// END
/*****************************************************************************/
