#include <cppmh.h>

int main(void) {
    /**
     * This program solves the following simple liner integer optimization
     * problem [1]:
     * (P):  minimize       x_1 + 10 x_2
     *          x
     *      subject to   66 x_1 + 14 x_2 >= 1430,
     *                  -82 x_1 + 28 x_2 >= 1306,
     *                      x_1 and x_2 are integer.
     *
     * The problem (P) has an optimal solution (x_1, x_2) = (7, 10) with
     * objective function value 707.
     *
     * [1] R.Fletcher: Practical Methods of Optimization, Second Edition,
     * John Wiley & Sons (2000).
     */
    cppmh::model::IPModel model;

    auto& x = model.create_variables("x", 2, -100, 100);
    auto& g = model.create_constraints("g", 2);

    g(0) = 66 * x(0) + 14 * x(1) >= 1430;
    g(1) = -82 * x(0) + 28 * x(1) >= 1306;
    model.minimize(x(0) + 10 * x(1));

    auto result = cppmh::solver::solve(&model);

    std::cout << "objective = " << result.solution.objective() << std::endl;
    std::cout << "x(0) = " << result.solution.variables("x").values(0)
              << std::endl;
    std::cout << "x(1) = " << result.solution.variables("x").values(1)
              << std::endl;

    return 0;
}