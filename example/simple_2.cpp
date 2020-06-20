#include <cppmh.h>

int main(void) {
    /**
     * This program solves the following simple binary integer optimization
     * problem:
     * (P):  minimize  f_1 + f_2
     *          x,y
     *      subject to x_1 + x_2 + x_3              >= 2,
     *                                   y_1 + y_2   = 1,
     *                 x_1             + y_1         = 1,
     *                 x_1, x_2, x_3, y_1, y_2 \in {0, 1},
     *
     *        where    f_1 = 2 x_1 + 7 x_2 + 9 x_3 + 1,
     *                 f_2 = 5 y_1 + 6 y_2.
     */
    cppmh::model::IPModel model;

    auto& x = model.create_variables("x", 3, 0, 1);
    auto& y = model.create_variables("y", 2, 0, 1);
    auto& g = model.create_constraints("g", 3);
    auto& f = model.create_expressions("f", 2);

    std::vector<int> c = {2, 7, 9};
    std::vector<int> d = {5, 6};

    f(0) = x.dot(c) + 1;
    f(1) = y.dot(d);
    g(0) = x.sum() >= 2;
    g(1) = y.selection();
    g(2) = x(0) + y(0) == 1;

    model.minimize(f.sum());

    auto result = cppmh::solver::solve(&model);

    result.solution.print_variable_values();
    result.solution.print_expression_values();
    result.solution.print_constraint_values();
    result.solution.print_violation_values();

    return 0;
}