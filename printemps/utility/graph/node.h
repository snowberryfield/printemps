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
class Edge;

/*****************************************************************************/
template <class T>
class Node {
    friend Graph<T>;

   private:
    T                       m_key;
    double                  m_weight;
    std::unordered_set<int> m_edge_indices;

   public:
    /*************************************************************************/
    Node(void) {
        this->initialize();
    }

    /*************************************************************************/
    Node(const T a_KEY) {
        this->setup(a_KEY);
    }

    /*************************************************************************/
    Node(const T a_KEY, const double a_WEIGHT) {
        this->setup(a_KEY, a_WEIGHT);
    }

    /*************************************************************************/
    inline void initialize(void) {
        m_weight = 0.0;
        m_edge_indices.clear();
    }

    /*************************************************************************/
    inline void setup(const T a_KEY) {
        this->initialize();
        m_key = a_KEY;
    }

    /*************************************************************************/
    inline void setup(const T a_KEY, const double a_WEIGHT) {
        this->initialize();
        m_key    = a_KEY;
        m_weight = a_WEIGHT;
    }

    /*************************************************************************/
    inline T key(void) const noexcept {
        return m_key;
    }

    /*************************************************************************/
    inline double weight(void) const noexcept {
        return m_weight;
    }

    /*************************************************************************/
    inline std::unordered_set<int> const &edge_indices(void) const noexcept {
        return m_edge_indices;
    }
};
}  // namespace printemps::utility::graph
/*****************************************************************************/
// END
/*****************************************************************************/