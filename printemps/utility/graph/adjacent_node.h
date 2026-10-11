/*****************************************************************************/
// Copyright (c) 2020-2026 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_UTILITY_GRAPH_ADJACENT_NODE_H__
#define PRINTEMPS_UTILITY_GRAPH_ADJACENT_NODE_H__

namespace printemps::utility::graph {
/*****************************************************************************/
struct AdjacentNode {
    int    node_index;
    double weight;
    int    edge_index;

    /*************************************************************************/
    AdjacentNode(void) : node_index(0), weight(0.0), edge_index(0) {
    }

    /*************************************************************************/
    AdjacentNode(const int    a_NODE_INDEX,
                 const double a_WEIGHT     = 0.0,
                 const int    a_EDGE_INDEX = 0)
        : node_index(a_NODE_INDEX),
          weight(a_WEIGHT),
          edge_index(a_EDGE_INDEX) {
    }
};
}  // namespace printemps::utility::graph

#endif
/*****************************************************************************/
// END
/*****************************************************************************/
