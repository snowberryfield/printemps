/*****************************************************************************/
// Copyright (c) 2020-2026 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_UTILITY_GRAPH_ABSTRACT_GRAPH_H__
#define PRINTEMPS_UTILITY_GRAPH_ABSTRACT_GRAPH_H__

#include "edge.h"

namespace printemps::utility::graph {
/*****************************************************************************/
template <class T>
class AbstractBase {
   protected:
    std::vector<T>             m_nodes;
    std::unordered_map<T, int> m_node_to_index;
    std::vector<double>        m_node_weights;
    std::vector<Edge<T>>       m_edges;

    /*************************************************************************/
    inline std::pair<int, bool> register_node(const T     &a_NODE,
                                              const double a_WEIGHT = 0.0) {
        auto it = m_node_to_index.find(a_NODE);
        if (it != m_node_to_index.end()) {
            return {it->second, false};
        }
        const int INDEX         = static_cast<int>(m_nodes.size());
        m_node_to_index[a_NODE] = INDEX;
        m_nodes.push_back(a_NODE);
        m_node_weights.push_back(a_WEIGHT);
        return {INDEX, true};
    }

   public:
    /*************************************************************************/
    AbstractBase(void) {
        this->initialize();
    }

    /*************************************************************************/
    virtual ~AbstractBase(void) = default;

    /*************************************************************************/
    virtual void initialize(void) {
        m_nodes.clear();
        m_node_to_index.clear();
        m_node_weights.clear();
        m_edges.clear();
    }

    /*************************************************************************/
    inline int number_of_nodes(void) const noexcept {
        return static_cast<int>(m_nodes.size());
    }

    /*************************************************************************/
    inline int number_of_edges(void) const noexcept {
        return static_cast<int>(m_edges.size());
    }

    /*************************************************************************/
    inline int size(void) const noexcept {
        return this->number_of_nodes();
    }

    /*************************************************************************/
    inline bool empty(void) const noexcept {
        return m_nodes.empty();
    }

    /*************************************************************************/
    inline bool has_node(const T &a_NODE) const {
        return m_node_to_index.find(a_NODE) != m_node_to_index.end();
    }

    /*************************************************************************/
    inline int node_index(const T &a_NODE) const {
        auto it = m_node_to_index.find(a_NODE);
        if (it == m_node_to_index.end()) {
            return -1;
        }
        return it->second;
    }

    /*************************************************************************/
    inline double node_weight(const T &a_NODE) const {
        auto it = m_node_to_index.find(a_NODE);
        if (it == m_node_to_index.end()) {
            return 0.0;
        }
        return m_node_weights[it->second];
    }

    /*************************************************************************/
    inline void set_node_weight(const T &a_NODE, const double a_WEIGHT) {
        auto it = m_node_to_index.find(a_NODE);
        if (it != m_node_to_index.end()) {
            m_node_weights[it->second] = a_WEIGHT;
        }
    }

    /*************************************************************************/
    inline const std::vector<T> &nodes(void) const noexcept {
        return m_nodes;
    }

    /*************************************************************************/
    inline const std::vector<Edge<T>> &edges(void) const noexcept {
        return m_edges;
    }

    /*************************************************************************/
    virtual int add_node(const T &a_NODE, const double a_WEIGHT = 0.0) = 0;

    /*************************************************************************/
    inline void add_nodes(const std::vector<T> &a_NODES) {
        for (const auto &node : a_NODES) {
            this->add_node(node);
        }
    }
};
}  // namespace printemps::utility::graph

#endif
/*****************************************************************************/
// END
/*****************************************************************************/
