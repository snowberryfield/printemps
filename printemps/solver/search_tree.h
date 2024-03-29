/*****************************************************************************/
// Copyright (c) 2020-2023 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_SEARCH_TREE_H__
#define PRINTEMPS_SOLVER_SEARCH_TREE_H__

namespace printemps::solver {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class SearchTree {
   private:
    utility::graph::Graph<int> m_tree;
    std::vector<
        std::pair<solution::SparseSolution<T_Variable, T_Expression>, int>>
        m_frontier_solutions;
    std::vector<
        std::pair<solution::SparseSolution<T_Variable, T_Expression>, int>>
        m_locally_optimal_solutions;

   public:
    /*************************************************************************/
    SearchTree(void) {
        this->initialize();
    }

    /*************************************************************************/
    SearchTree(const solution::SolutionArchive<T_Variable, T_Expression>&
                   a_SOLUTION_ARCHIVE,
               const solution::SparseSolution<T_Variable, T_Expression>&
                   a_INCUMBENT_SOLUTION) {
        this->setup(a_SOLUTION_ARCHIVE, a_INCUMBENT_SOLUTION);
    }

    /*************************************************************************/
    inline void initialize(void) {
        m_tree.initialize();
        m_frontier_solutions.clear();
        m_locally_optimal_solutions.clear();
    }

    /*************************************************************************/
    inline void update(const solution::SolutionArchive<
                           T_Variable, T_Expression>& a_SOLUTION_ARCHIVE,
                       const solution::SparseSolution<T_Variable, T_Expression>&
                           a_INCUMBENT_SOLUTION) {
        this->setup(a_SOLUTION_ARCHIVE, a_INCUMBENT_SOLUTION);
    }

    /*************************************************************************/
    inline void setup(const solution::SolutionArchive<T_Variable, T_Expression>&
                          a_SOLUTION_ARCHIVE,
                      const solution::SparseSolution<T_Variable, T_Expression>&
                          a_INCUMBENT_SOLUTION) {
        m_frontier_solutions.clear();
        m_locally_optimal_solutions.clear();

        auto&     solutions          = a_SOLUTION_ARCHIVE.solutions();
        const int NEW_SOLUTIONS_SIZE = solutions.size();
        const int OLD_SOLUTION_SIZE  = m_tree.nodes().size();

        for (auto i = OLD_SOLUTION_SIZE; i < NEW_SOLUTIONS_SIZE; i++) {
            m_tree.add_node(i, solutions[i].global_augmented_objective);
        }

        for (auto i = 0; i < OLD_SOLUTION_SIZE; i++) {
            for (auto j = OLD_SOLUTION_SIZE; j < NEW_SOLUTIONS_SIZE; j++) {
                m_tree.add_edge(i, j, solutions[i].distance(solutions[j]));
            }
        }

        for (auto i = OLD_SOLUTION_SIZE; i < NEW_SOLUTIONS_SIZE; i++) {
            for (auto j = i + 1; j < NEW_SOLUTIONS_SIZE; j++) {
                m_tree.add_edge(i, j, solutions[i].distance(solutions[j]));
            }
        }

        const auto  MINIMUM_SPANNING_TREE = m_tree.minimum_spanning_tree();
        const auto& MST_NODES             = MINIMUM_SPANNING_TREE.nodes();
        const auto& MST_EDGES             = MINIMUM_SPANNING_TREE.edges();

        for (auto i = 0; i < NEW_SOLUTIONS_SIZE; i++) {
            const double DISTANCE = solutions[i].distance(a_INCUMBENT_SOLUTION);
            if (MST_NODES.at(i).edge_indices().size() == 1) {
                m_frontier_solutions.emplace_back(solutions[i], DISTANCE);
            }

            bool  is_local_optimal = true;
            auto& edge_indices     = MST_NODES.at(i).edge_indices();

            for (auto&& edge_index : edge_indices) {
                const int NODE_INDEX_FIRST =
                    MST_EDGES[edge_index].node_keys().first;
                const int NODE_INDEX_SECOND =
                    MST_EDGES[edge_index].node_keys().second;

                if (NODE_INDEX_FIRST != i &&
                    solutions[i].global_augmented_objective >
                        solutions[NODE_INDEX_FIRST]
                            .global_augmented_objective) {
                    is_local_optimal = false;
                    break;
                }

                if (NODE_INDEX_SECOND != i &&
                    solutions[i].global_augmented_objective >
                        solutions[NODE_INDEX_SECOND]
                            .global_augmented_objective) {
                    is_local_optimal = false;
                    break;
                }
            }

            if (is_local_optimal) {
                m_locally_optimal_solutions.emplace_back(solutions[i],
                                                         DISTANCE);
            }
        }
    }

    /*************************************************************************/
    inline constexpr const std::vector<
        std::pair<solution::SparseSolution<T_Variable, T_Expression>, int>>&
    frontier_solutions(void) const {
        return m_frontier_solutions;
    }

    /*************************************************************************/
    inline constexpr const std::vector<
        std::pair<solution::SparseSolution<T_Variable, T_Expression>, int>>&
    locally_optimal_solutions(void) const {
        return m_locally_optimal_solutions;
    }
};
}  // namespace printemps::solver

#endif
/*****************************************************************************/
// END
/*****************************************************************************/