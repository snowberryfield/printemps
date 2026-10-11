/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_UTILITY_GRAPH_EDGE_H__
#define PRINTEMPS_UTILITY_GRAPH_EDGE_H__

namespace printemps::utility::graph {
/*****************************************************************************/
template <class T>
struct Edge {
    T      first;
    T      second;
    double weight;

    /*************************************************************************/
    Edge(void) : first(), second(), weight(0.0) {
    }

    /*************************************************************************/
    Edge(const T &a_FIRST, const T &a_SECOND, const double a_WEIGHT = 0.0)
        : first(a_FIRST), second(a_SECOND), weight(a_WEIGHT) {
    }
};
}  // namespace printemps::utility::graph

#endif
/*****************************************************************************/
// END
/*****************************************************************************/