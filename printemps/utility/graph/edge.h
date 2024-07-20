/*****************************************************************************/
// Copyright (c) 2020-2024 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/

namespace printemps::utility::graph {
/*****************************************************************************/
template <class T>
class Graph;

/*****************************************************************************/
template <class T>
class Edge {
    friend Graph<T>;

   private:
    double          m_weight;
    std::pair<T, T> m_node_keys;

   public:
    /*************************************************************************/
    Edge(void) {
        this->initialize();
    }

    /*************************************************************************/
    Edge(const T a_NODE_KEY_FIRST, const T a_NODE_KEY_SECOND) {
        this->setup(a_NODE_KEY_FIRST, a_NODE_KEY_SECOND);
    }

    /*************************************************************************/
    Edge(const T a_NODE_KEY_FIRST, const T a_NODE_KEY_SECOND,
         const double a_WEIGHT) {
        this->setup(a_NODE_KEY_FIRST, a_NODE_KEY_SECOND, a_WEIGHT);
    }

    /*************************************************************************/
    inline void initialize(void) {
        m_weight = 0.0;
    }

    /*************************************************************************/
    inline void setup(const T a_NODE_KEY_FIRST,
                      const T a_NODE_KEY_SECOND) noexcept {
        this->initialize();
        m_node_keys.first  = a_NODE_KEY_FIRST;
        m_node_keys.second = a_NODE_KEY_SECOND;
    }

    /*************************************************************************/
    inline void setup(const T a_NODE_KEY_FIRST, const T a_NODE_KEY_SECOND,
                      const double a_WEIGHT) noexcept {
        this->initialize();
        m_node_keys.first  = a_NODE_KEY_FIRST;
        m_node_keys.second = a_NODE_KEY_SECOND;
        m_weight           = a_WEIGHT;
    }

    /*************************************************************************/
    inline double weight(void) const noexcept {
        return m_weight;
    }

    /*************************************************************************/
    inline std::pair<T, T> const &node_keys(void) const noexcept {
        return m_node_keys;
    }
};
}  // namespace printemps::utility::graph
/*****************************************************************************/
// END
/*****************************************************************************/