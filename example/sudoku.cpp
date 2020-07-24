/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <cppmh.h>

int main(void) {
    /*************************************************************************/
    /// Problem statement
    /*************************************************************************/
    /**
     * This program solves a "Sudoku" puzzle[1] as a binary integer programming
     * problem. A Sudoku puzzle can be formulated as follows:
     *  (P):    find   {x_{n, m, k}} (n=1,...,N m=1,...,N, k=1,...,N)
     *       such that \sum_{n=1}^{N} x_{n, m, k} = 1 (m=1,...,N, k=1,...,N),
     *                 \sum_{m=1}^{N} x_{n, m, k} = 1 (n=1,...,N, k=1,...,N),
     *                 \sum_{k=1}^{N} x_{n, m, k} = 1 (n=1,...,N, m=1,...,N),
     * 　　　　　        \sum_{n=3(i-1)+1}^{3i} \sum_{m=3(j-1)+1}^{3j} x_{n, m,
     * k} (i=1,...,N/3, j=1,...,N/3, k=1,...,N), x_{n, m, k} \in {0, 1}
     * (n=1,...,N m=1,...,N, k=1,...,N),
     *
     * where N denotes the size of problem.
     *
     * [1] Wikipedia "Sudoku" : https://en.wikipedia.org/wiki/Sudoku
     */

    /// This problem instance is from
    /// https://metro.co.uk/2012/06/28/worlds-hardest-sudoku-everest-created-by-mathematician-arto-inkala-483588/
    std::vector<std::vector<int>> initial  //
        = {{0, 0, 5, 3, 0, 0, 0, 0, 0},    //
           {8, 0, 0, 0, 0, 0, 0, 2, 0},    //
           {0, 7, 0, 0, 1, 0, 5, 0, 0},    //
           {4, 0, 0, 0, 0, 5, 3, 0, 0},    //
           {0, 1, 0, 0, 7, 0, 0, 0, 6},    //
           {0, 0, 3, 2, 0, 0, 0, 8, 0},    //
           {0, 6, 0, 5, 0, 0, 0, 0, 9},    //
           {0, 0, 4, 0, 0, 0, 0, 3, 0},    //
           {0, 0, 0, 0, 0, 9, 7, 0, 0}};
    int N = 9;

    /*************************************************************************/
    /// Model object definition
    /*************************************************************************/
    cppmh::model::IPModel model;

    /*************************************************************************/
    /// Decision variable definitions
    /*************************************************************************/
    auto& x = model.create_variables("x", {N, N, N}, 0, 1);

    /*************************************************************************/
    /// Expression definitions
    /*************************************************************************/
    /// Expressions for the constraints
    /// \sum_{n=3(i-1)+1}^{3i} \sum_{m=3(j-1)+1}^{3j}
    /// x_{n, m, k} (i=1,...,N/3, j=1,...,N/3, k=1,...,N)
    auto& expression_block =
        model.create_expressions("expression_block", {N / 3, N / 3, N});
    for (auto i = 0; i < N / 3; i++) {
        for (auto j = 0; j < N / 3; j++) {
            for (auto k = 0; k < N; k++) {
                for (auto n = i * N / 3; n < (i + 1) * N / 3; n++) {
                    for (auto m = j * N / 3; m < (j + 1) * N / 3; m++) {
                        expression_block(i, j, k) += x(n, m, k);
                    }
                }
            }
        }
    }

    /**
     * All necessary information of expression_block is carried over to the
     * constraints defined in following part, and the it is not used in the
     * optimization calculation, so it is disabled to avoid unnecessary
     * calculations.
     */
    expression_block.disable();

    /*************************************************************************/
    /// Constraint definitions
    /*************************************************************************/
    /// \sum_{n=1}^{N} x_{n, m, k} = 1 (m=1,...,N, k=1,...,N)
    auto& constraint_number =
        model.create_constraints("constraint_number", {N, N});
    for (auto n = 0; n < N; n++) {
        for (auto m = 0; m < N; m++) {
            constraint_number(n, m) = x.selection({n, m, cppmh::model::All});
        }
    }

    /// \sum_{m=1}^{N} x_{n, m, k} = 1 (n=1,...,N, k=1,...,N)
    auto& constraint_row = model.create_constraints("constraint_row", {N, N});
    for (auto m = 0; m < N; m++) {
        for (auto k = 0; k < N; k++) {
            constraint_row(m, k) = x.selection({cppmh::model::All, m, k});
        }
    }

    /// \sum_{k=1}^{N} x_{n, m, k} = 1 (n=1,...,N, m=1,...,N),
    auto& constraint_column =
        model.create_constraints("constraint_column", {N, N});
    for (auto n = 0; n < N; n++) {
        for (auto k = 0; k < N; k++) {
            constraint_column(n, k) = x.selection({n, cppmh::model::All, k});
        }
    }

    /// \sum_{n=3(i-1)+1}^{3i} \sum_{m=3(j-1)+1}^{3j} x_{n, m, k}
    /// (i=1,...,N/3, j=1,...,N/3, k=1,...,N)
    auto& constraint_block =
        model.create_constraints("constraint_block", {N / 3, N / 3, N});
    for (auto i = 0; i < N / 3; i++) {
        for (auto j = 0; j < N / 3; j++) {
            for (auto k = 0; k < N; k++) {
                constraint_block(i, j, k) = expression_block(i, j, k) == 1;
            }
        }
    }
    /*************************************************************************/
    /// Objective function definition
    /*************************************************************************/
    /// There is no objective function.

    /*************************************************************************/
    /// Initial value definitions
    /*************************************************************************/
    for (auto n = 0; n < N; n++) {
        for (auto m = 0; m < N; m++) {
            if (initial[n][m] > 0) {
                x(n, m, initial[n][m] - 1).fix_by(1);
            }
        }
    }

    /*************************************************************************/
    /// Run solver
    /*************************************************************************/
    cppmh::solver::Option option;
    option.selection_mode            = cppmh::model::SelectionMode::Defined;
    option.tabu_search.iteration_max = 1000;
    option.tabu_search.tabu_mode     = cppmh::solver::tabu_search::All;
    option.verbose                   = cppmh::solver::Full;

    auto result = cppmh::solver::solve(&model, option);

    /*************************************************************************/
    /// Print result
    /*************************************************************************/
    std::vector<std::vector<int>> answer(N);
    for (auto n = 0; n < N; n++) {
        answer[n].resize(N);
        for (auto m = 0; m < N; m++) {
            for (auto k = 0; k < N; k++) {
                answer[n][m] +=
                    (k + 1) * result.solution.variables("x").values(n, m, k);
            }
        }
    }

    for (auto n = 0; n < N; n++) {
        for (auto m = 0; m < N; m++) {
            std::cout << answer[n][m] << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}
/*****************************************************************************/
// END
/*****************************************************************************/