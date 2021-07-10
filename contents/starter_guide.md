---
title: PRINTEMPS
layout: sub
---

# Starter Guide of PRINTEMPS

- [Introduction](#introduction)
- [Modeling](#modeling)
    - [Basics](#basics)
    - [Decision Variable](#decision-variable)
        - [Creating Decision Variables](#creating-decision-variables)
        - [Setting Lower and Upper Bounds for Decision Variables](#setting-lower-and-upper-bounds-for-decision-variables)
        - [Setting Initial values for Decision Variables](#setting-initial-values-for-decision-variables)
        - [Fixing Decision Variable Values](#fixing-decision-variable-values)
    - [Expression](#expression)
        - [Creating Expression](#creating-expression)
        - [Defining Expression](#defining-expression)
    - [Constraint](#constraint)
        - [Creating Constraint](#creating-constraint)
        - [Defining Constraint](#defining-constraint)
    - [Objective Function](#objective-function)
    - [Special Methods for Practical Modeling](#special-methods-for-practical-modeling)
        - [Sum of Indexed Components](#sum-of-indexed-components)
        - [Innder Product](#innder-product)
        - [Selection Constraint](#selection-constraint)
- [Optimization](#optimization)
- [Accessing the Result](#accessing-the-result)
- [Work-in-Progress Topics](#work-in-progress-topics)
- [References](#references)

## Introduction
PRINTEMPS can compute approximate solutions for __linear and nonlinear integer optimization problems__ formulated as follows:


```
(P):  minimize  f(x)
         x
     subject to g(x) <= 0, 
                h(x)  = 0, 
                l <= x <= u, 
                x \in Z^{N},
```
where `x` denotes N-dimensional integer vector with the lower bound vector `l` and the upper bound vector `u`. 
The function `f:R^{N} -> R` denotes the scalar objective function to be minimized, and `g:R^{N} -> R^{I}`, `h:R^{N} -> R^{J}` respectively denote `I`-dimensional inequality and `J`-dimensional equality constraint functions.

PRINTEMPS employs __Tabu Search__ [1], a metaheuristics technique, to solve problems. 
In solving, it replaces constraints with penalty functions which return violations to the corresponding constraints, and the penalty functions multiplied by positive penalty coefficients are added to the objective function. 
The penalty coefficients are updated iteratively and automatically in a method similar (not exact same) to the one proposed in paper [2]. 
The figure below shows the flow-chart of the algorithm of PRINTEMPS. 

<div align="center">

<img src="../assets/image/flowchart.svg" width="800">

__The flow-chart of the optimization algorithm of PRINTEMPS.__ 

</div>

Let us consider a simple linear integer optimization problem [3]:
```
(P1):  minimize       x_1 + 10 x_2
          x
      subject to   66 x_1 + 14 x_2 >= 1430,
                  -82 x_1 + 28 x_2 >= 1306,
                      x_1 and x_2 are integer.
```

The following code shows an implementation to solve the problem (P) using PRINTEMPS.

```c++
#include <printemps.h>
int main(void) {
    // (1) Modeling
    printemps::model::IPModel model;

    auto& x = model.create_variables("x", 2);
    auto& g = model.create_constraints("g", 2);

    g(0) =  66 * x(0) + 14 * x(1) >= 1430;
    g(1) = -82 * x(0) + 28 * x(1) >= 1306;
    model.minimize(x(0) + 10 * x(1));

    // (2) Running Solver
    auto result = printemps::solver::solve(&model);

    // (3) Accessing the Result
    std::cout << "objective = " << result.solution.objective() << std::endl;
    std::cout << "x(0) = "      << result.solution.variables("x").values(0) << std::endl;
    std::cout << "x(1) = "      << result.solution.variables("x").values(1) << std::endl;

    return 0;
}
```
By compiling the code above and then running the generated executable, we will obtain the following result:

```bash
objective = 707
x(0) = 7
x(1) = 70
```

An optimization using PRINTEMPS consists of three steps; (1) modeling, (2) optimization, and (3) accessing the result. Each step is detailed in the following sections of this document. 

## Modeling
### Basics
Let us consider the following optimization problem:

```
(P2):  minimize  f_1 + f_2
         x,y
      subject to x_1 + x_2 + x_3              >= 2,
                                   y_1 + y_2   = 1,
                 x_1             + y_1         = 1,
                 x_1, x_2, x_3, y_1, y_2 \in {0, 1},

        where    f_1 = 2 x_1 + 7 x_2 + 9 x_3 + 1,
                 f_2 = 5 y_1 + 6 y_2.
```

The following code shows an implementation of the mathematical model of the problem (P2) using PRINTEMPS.

```c++
printemps::model::IPModel model;

// Create binary decision variables.
auto& x_1 = model.create_variable("x_1", 0, 1);
auto& x_2 = model.create_variable("x_2", 0, 1);
auto& x_3 = model.create_variable("x_3", 0, 1);

auto& y_1 = model.create_variable("y_1", 0, 1);
auto& y_2 = model.create_variable("y_2", 0, 1);

// Create expressions.
auto& f_1 = model.create_expression("f_1");
auto& f_2 = model.create_expression("f_2");

// Create constraints.
auto& g_1 = model.create_constraint("g_1");
auto& g_2 = model.create_constraint("g_2");
auto& g_3 = model.create_constraint("g_3");

// Define expressions.
f_1 = 2 * x_1 + 7 * x_2 + 9 * x_3 + 1;
f_2 = 5 * y_1 + 6 * y_2;

// Define constraints.
g_1 = x_1 + x_2 + x_3 >= 1;
g_2 = y_1 + y_2 == 1;
g_3 = x_1 + y_1 == 1;

// Define the objective function to be minimized.
model.minimize(f_1 + f_2);
```

An optimization problem can be described as like arithmetic calculations on decision variables, expressions, constraints and related objects. 
An expression can express a first-order function on decision variables such as `2 x_1 + 7 x_2 + 9 x_3 + 1` and it could be used for simplifying the model. Of course, the statement `model.minimize(f_1 + f_2);` in the code above can be replaced with `model.minimize(2 * x_1 + 7 * x_2 + 9 * x_3 + 5 * y_1 + 6 * y_2 + 1);`. 
Please note that instances of the decision variable, expression and constraint created by the methods `create_variable()`, `create_expression()`, and `create_constraint()` are managed in an `IPModel` object. 
These methods return the references of their instances, so that the symbols describing the problem must be declared as aliases. 

PRINTEMPS supports one or higher-dimensional indexed symbols such as `x(0), x(1), x(0,0), x(0,1), ...` for intuitive modeling.
The problem (P2) can be equivalently modeled as follows:

```c++
printemps::model::IPModel model;

// Create indexed binary decision variables x(0), x(1), x(2), y(0), y(1).
auto& x = model.create_variables("x", 3, 0, 1);
auto& y = model.create_constraints("y", 2);

// Create indexed expressions f(0), f(1).
auto& f = model.create_expressions("f", 2);

// Create indexed constraints g(0), g(1), g(2).
auto& g = model.create_constraints("g", 3);

// Define expressions.
f(0) = 2 * x(0) + 7 * x(1) + 9 * x(2) + 1;
f(1) = 5 * y(0) + 6 * y(1);

// Define constraints.
g(0) = x(0) + x(1) + x(2) >= 1;
g(1) = y(0) + y(1) == 1;
g(2) = x(0) + y(0) == 1;

// Define the objective function to be minimized.
model.minimize(f(0) + f(1));
```

In addition, following special methods are provided to define a problem simply:
- The method `sum()` to express a sum of decision variables or expressions.
- The method `dot()` to express a inner product of coefficients and decision variables.
- The method `selection()` to express a selection constraint that just one component takes value 1 in indexed decision variables.

With these methods, the problem (P2) can be simplified as follows:

```c++
printemps::model::IPModel model;

// Create indexed binary decision variables x(0), x(1), x(2), y(0), y(1).
auto& x = model.create_variables("x", 3, 0, 1);
auto& y = model.create_variables("y", 2);

// Create indexed expressions f(0), f(1).
auto& f = model.create_expressions("f", 2);

// Create indexed constraints g(0), g(1), g(2).
auto& g = model.create_constraints("g", 3);

// Create coefficient vectors.
std::vector<int> c = {2, 7, 9};
std::vector<int> d = {5, 6};

// Define expressions.
f(0) = x.dot(c) + 1;
f(1) = y.dot(d);

// Define constraints.
g(0) = x.sum() >= 2;
g(1) = y.selection();
g(2) = x(0) + y(0) == 1;

// Define the objective function to be minimized.
model.minimize(f.sum());
```

### Decision Variable
#### Creating Decision Variables
Decision variables to describe the problem can be created as follows: 

```c++
printemps::model::IPModel model;

// Create scalar (no-indexed) variable x.
auto& x = model.create_variable("x");

// Create scalar (no-indexed) variable y with the lower bound 0 and the upper
// bound 1.
auto& y = model.create_variable("y", 0, 1);

// Create one-dimensional indexed variables z(0), ..., z(9).
auto& z = model.create_variables("z", 10);

// Create two-dimensional indexed variables w(0, 0), ..., w(19, 29).
auto& w = model.create_variables("w", {20, 30});
```

The methods `create_variable(name)` and `create_variable(name, lb, ub)` create scalar (no-indexed) decision variable named `name`. 
As optional, lower and upper bounds can be set through the arguments `lb` and `ub`, respectively.

The methods `create_variables(name, n)` and `create_variables(name, n, lb, ub)` create `n` decision variables named `name` with one-dimensional index. 
Each decision variable can be accessed by like as `z(0)`.
As optional, lower and upper bounds can be set through the arguments `lb` and `ub`, respectively, and they will be commonly set for all variables. 

The methods `create_variables("name", {n_1, ..., n_m})` and `create_variables("name", {n_1, ..., n_m}, lb, ub)` create `n_1 * ... * n_m` decision variables named `name` with `m`-dimensional index. 
Each variable can be accessed by like as `w(0, 0)`.
As optional, lower and upper bounds can be set through the arguments `lb` and `ub`, respectively, and they will be commonly set for all variables. 

#### Setting Lower and Upper Bounds for Decision Variables
The lower and upper bounds of a decision variable can be specified after creation of the decision variable by the method `set_bounds(lb, ub)`. 
In the following code, the lower and upper bounds of `x(1)` to `x(9)` are commonly set as `-5` and `5`, respectively, while those of `x(0)` are set as `0` and `20`, respectively.

```c++
printemps::model::IPModel model;

// Create decision variables x(0), ..., x(9) with the lower bound -5 and the
// upper bound 5.
auto& x = model.create_variables("x", 10, -5, 5);

// The lower and upper bounds of x(0) are changed to 0 and 20, respectively.
x(0).set_bound(0, 20);
```

#### Setting Initial values for Decision Variables
The default initial value of decision variable is `0`. 
Users can specify initial values by the operator `=`, and the current value of the decision variable can be accessed by the method `value()`. In the following code, the initial value for `x(0)` is set as `1`, while as those of `x(1)` to `x(9)` are still `0`.

```c++
printemps::model::IPModel model;

// Create decision variables x(0), ..., x(9). 
// The initial values of them are set by the default value 0.
auto& x = model.create_variables("x", 10);

// The initial value of x(0) is changed to 1.
x(0) = 1;

// x_value will contain 1.
auto x_value = x(0).value();  
```

#### Fixing Decision Variable Values
Users can also "fix" the values of decision variables by the method `fix_by(value)`. 
The value of fixed variable will not be changed during optimization. 
In the following code, the value for `x(0)` is fixed by 1, while as those of `x(1)` to `x(9)` can vary.

```c++
printemps::model::IPModel model;

// Create decision variables x(0), ..., x(9). 
auto& x = model.create_variables("x", 10);

// The value of x(0) is fixed by 1.
x(0).fix_by(1);
```

There are some other method related to the fixing. 
The method `unfix()` cancels the fixing, and the method `set_value_if_not_fixed(value)` tries to override the value of the decision variable if it is not fixed. 
If the decision variable is fixed, initialization by the operator `=` discussed above will throw an error. 
Users can check whether the decision variable is fixed or not by the method `is_fixed()`, which returns true if and only if the decision variable is fixed. 

### Expression
#### Creating Expression
Expressions to describe the problem can be created as follows: 

```c++
printemps::model::IPModel model;

auto& x = model.create_variable("x");
auto& y = model.create_variable("y");

// Create scalar(no-indexed) expression p.
auto& p = model.create_expression("p");

// Create scalar(no-indexed) expression q by given expression 2 * x + 3 * y.
auto& q = model.create_expression("q", 2 * x + 3 * y);

// Create one-dimensional expressions r(0), ..., r(9).
auto& r = model.create_expressions("r", 10);

// Create two-dimensional expressions r(0,0), ..., r(19, 29).
auto& s = model.create_expressions("s", {20, 30});
```

The methods  `create_expression(name)` and `create_expression(name, expression)` create scalar (no-indexed) expression named `name`. 
As optional, the created expression can be directly defined by  `expression`.

The methods `create_expressions(name, n)` creates `n` expressions named `name` with one-dimensional index. Each expression can be accessed by like as `r(0)`.

The `create_expressions("name", {n_1, ..., n_m})` method creates `n_1 * ... * n_m` expressions named `name` with `m`-dimensional index. Each constraint can be accessed by like as `s(0, 0)`.

#### Defining Expression
An expression can be defined as a first-order functions on decision variables and other expressions (nonlinear functions are modelled in a different way.)
The following code shows examples of expression definitions.

```c++
printemps::model::IPModel model;

// Create decision variables.
auto &x = model.create_variable("x");
auto &y = model.create_variable("y");
auto &z = model.create_variables("z", 2);

// Create and define expressions.

// OK. A first-order function on decision variables can be accepted as an
// expression.
auto &p = model.create_expression("p");
p       = 2 * x + 3 * y + 4;

// OK. A decision variable itself can be accepted as an expression.
auto &q = model.create_expression("q");
p       = x;

// OK. A numerical constant can be accepted as an expression.
auto &r = model.create_expression("r");
r       = 4;

// OK. An expression can include a term of a decision variable divided by a
// constant. An expression can be defined in its creation.
auto &s = model.create_expression("s", 2 * x + y / 3);

// OK. Addition and subtraction of the predefined expression are also possible.
s += 4 * x + 1; // s = 6 * x + y / 3 + 1 
s -= 5 * x;     // s = x + y / 3 + 1

auto &t = model.create_expressions("t", 2);
// OK. Parentheses can be used in the definition of an expression.
t(0) = x + 3 * (y * z(0));
// OK. A first-order function on expressions can be accepted as an expression.
t(1) = 5 * s(0) + 6 * s(1) + 6;

auto &t = model.create_expression("u");
// NG. Multiplication and division which produce nonlinear terms are not
// possible. 
// u = x * x + x / y;

// NG. Nonlinear function is not possible.
// u = sin(x);
```

### Constraint
#### Creating Constraint
Constraints to describe the problem can be created as follows: 
```c++
printemps::model::IPModel model;

// Create decision variables.
auto& x = model.create_variable("x");
auto& y = model.create_variable("y");

// Create scalar (no-indexed) constraint g.
auto& g = model.create_expression("p");

// Create scalar (no-indexed) constraint h by given constraint x + y = 1.
auto& h = model.create_expression("h", x + y == 1);

// Create one-dimensional constraints u(0), ..., r(9).
auto& u = model.create_expressions("u", 10);

// Create two-dimensional constraints v(0,0), ..., v(19, 29).
auto& v = model.create_expressions("v", {20, 30});
```

The methods `create_constraint(name)` and `create_constraint(name, constraint)` create scalar (no-indexed) constraint named `name`. 
As optional, the created constraint can be directly defined by `constraint`.

The methods `create_constraints(name, n)` creates `n` constraints named `name` with one-dimensional index. Each constraint can be accessed by like as `u(0)`.

The `create_constraints("name", {n_1, ..., n_m})` method creates `n_1 * ... * n_m` constraints named `name` with `m`-dimensional index. Each constraint can be accessed by like as `v(0, 0)`.

#### Defining Constraint
A constraint can be defined as an equal or inequality between decision variables, expressions, and constants (numerical values of primitive types such as int, double) with operators `==`, `<=`, and `>=`.  
Note that a constraint whose sides are both constants is not acceptable, namely, either side must contain a decision variable or an expression, 
The following code shows examples of constraint definitions.

```c++
printemps::model::IPModel model;

// Create decision variables.
auto& x = model.create_variable("x");
auto& y = model.create_variable("y");
auto& z = model.create_variables("z", {10, 10});

// Create and define an expression.
auto& p = model.create_expression("p", x + y);

// Create and define constraints.
auto& g = model.create_expression("g");
g       = (2 * x + 3 * y + 5<= 10); // OK

auto& h = model.create_expression("h", p >= 7 + 2); // OK

auto& u = model.create_expression("u");
// NG. Both sides are constants.
// u = 4 <= 5;

auto& v = model.create_expressions("v", 10);
for (auto i = 0; i < 10; i++) {
    // OK. This code is identical to the following code:
    // u(i) = z.sum({i, printemps::model_component::All}) == 1;
    v(i) = z.selection({i, printemps::model_component::All});
}
```

### Objective Function
The objective function and the optimization sense (minimization or maximization) can be specified by the methods `minimize()` and `maximize()` of `IPModel` object. 
An expression or a decision variable can be objective function by specifying them as the argument of the methods `minimize()` or `maximize()`. 
If there is no objective function definition, PRINTEMPS solves the problem as a constraint satisfaction problem, which aims to find a feasible solution. 
If methods `minimize()` or `maximize()` are called more than once, the last definition will be effective.
The following code shows an example of objective function definitions.
```c++
printemps::model::IPModel model;

// Create decision variables.
auto& x = model.create_variable("x");
auto& y = model.create_variable("y");

// Create and define an expression.
auto& p = model.create_expression("p", 2 * x + 3 * y + 4);

// The part of constraint creation and definition is omitted.

// OK. A decision variable can be the objective function.
model.minimize(x);
// OK. An expression can be the objective function.
model.maximize(p);
// OK. A formula to define an expression can also be the objective function.
model.minimize(5 * x - 6 * y + 7);

// NG. The arguments of minimize() and maximize() must not be a constant.
// model.maximize(5);
```

### Special Methods for Practical Modeling
#### Sum of Indexed Components
The method `sum()` gives an expression defined by sum of components of indexed decision variables or expressions. 
The code

```c++
printemps::model::IPModel model;

// Create decision variables.
auto& x = model.create_variables("x", {10, 20});

// Create and define an expression.
auto& p = model.create_expression("p", x.sum());
```

is identical to the following code:

```c++
printemps::model::IPModel model;

// Create decision variables.
auto& x = model.create_variables("x", {10, 20});

// Create and define an expression.
auto& p = model.create_expression("p");

for (auto i = 0; i < 10; i++) {
    for (auto j = 0; j < 20; j++) {
        p += x(i, j);
    }
}
```

Partial sums of components of indexed decision variables or expressions can be obtained by specifying indices for fixed and those for summation. 
Summation is calculated for an index of which value is specified by the constant `printemps::model_component::All`.
The code

```c++
printemps::model::IPModel model;

// Create decision variables.
auto& x = model.create_variables("x", {10, 20});

// Create and define expressions.
auto& p = model.create_expressions("p", 10);
for (auto i = 0; i < 10; i++) {
    p(i) += x.sum({i, printemps::model_component::All});
}
```

is identical to the following code:

```c++
printemps::model::IPModel model;

// Create decision variables.
auto& x = model.create_variables("x", {10, 20});

// Create and define expressions.
auto& p = model.create_expressions("p", 10);
for (auto i = 0; i < 10; i++) {
    for (auto j = 0; j < 20; j++) {
        p(i) += x(i, j);
    }
}
```

#### Innder Product 
The method `dot()` gives an expression of an inner product between indexed variables or expressions and a numerical array with the same number of components.
The code

```c++
std::vector<int> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

printemps::model::IPModel model;

// Create decision variables.
auto& x = model.create_variables("x", 10);

// Create and define expressions.
auto& p = model.create_expression("p", x.dot(a));
```

is identical to the following code:

```c++
std::vector<int> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

printemps::model::IPModel model;

// Create decision variables.
auto& x = model.create_variables("x", 10);

// Create and define expressions.
auto& p = model.create_expression("p");
for (auto i = 0; i < 10; i++) {
    p += a[i] * x(i);
}
```

The method `dot()` is also applicable to two or higher-dimensional indexed decision variables and expressions by specifying indices for fixed and for summation. Note that the number of indices for summation must be one.
The following code shows example of allowable and non-allowable usages.

```c++
std::vector<int> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

printemps::model::IPModel model;

// Create decision variables.
auto& x = model.create_variables("x", {10, 10});

// Create and define expressions.

auto& p = model.create_expressions("p", 10);
// OK. The following part is identical to
// for (auto i=0; i<10; i++){
//    for (auto j=0; j<10; j++){
//        p(i) += a[j] * x(i, j);
//    }
// }
for (auto i = 0; i < 10; i++) {
    p[i] = x.dot(a, {i, printemps::model_component::All});
}

auto& q = model.create_expressions("q", 10);
// OK. The following part is identical to
// for (auto i=0; i<10; i++){
//    for (auto j=0; j<10; j++){
//        q(i) += a[j] * x(j, i);
//    }
// }
for (auto i = 0; i < 10; i++) {
    q[i] = x.dot(a, {printemps::model_component::All, i});
}

auto& r = model.create_expressions("r", 10);
// NG. The number of indices for summation must be one.
// for(auto i=0; i<10; i++){
//     r[i] = x.dot(a, {printemps::model_component::All, printemps::model_component::All});
// }
```

#### Selection Constraint
The method `selection()` gives a constraint that just one component takes value 1 in indexed decision variables. 
Such constraints are frequently used in practice.
The code

```c++
printemps::model::IPModel model;

// Create binary decision variables.
auto& x = model.create_variables("x", {10, 20}, 0, 1);

// Create and define a constraint.
auto& g = model.create_constraint("g", x.selection());
```

is identical to the following code:

```c++
printemps::model::IPModel model;

// Create binary decision variables.
auto& x = model.create_variables("x", {10, 20}, 0, 1);

// Create and define an expression for temporary.
auto& p = model.create_expression("p");
for (auto i = 0; i < 10; i++) {
    for (auto j = 0; j < 20; j++) {
        p += x(i, j);
    }
}
// Create and define a constraint.
auto& g = model.create_constraint("g");
g = (p == 1);
```

Of course, a selection constraint for partial sums of indexed binary decision variables can be obtained by specifying indices for fixed and for summation. 
The code

```c++
printemps::model::IPModel model;

auto& x = model.create_variables("x", {10, 20}, 0, 1);
auto& g = model.create_constraints("g", 10);

for (auto i = 0; i < 10; i++) {
    g(i) += x.selection({i, printemps::model_component::All});
}
```

is identical to the following code:

```c++
printemps::model::IPModel model;

// Create binary decision variables.
auto& x = model.create_variables("x", {10, 20}, 0, 1);

// Create and define an expression for temporary.
auto& p = model.create_expressions("p", 10);
for (auto i = 0; i < 10; i++) {
    for (auto j = 0; j < 20; j++) {
        p(i) += x(i, j);
    }
}

// Create and define constraints.
for (auto i = 0; i < 10; i++) {
    for (auto j = 0; j < 20; j++) {
        g(i) += (p(i) == 1);
    }
}
```

## Optimization
The solver can be run by the `printemps::solver::solve()` function with the pointer to the `IPModel` object as the argument:

```c++
printemps::model::IPModel model;

// The modeling part is omitted.

auto result = printemps::solver::solve(&model);
```

Users can configure the behavior of the solver by setting options. The options include those related to optimization parameters, log verbosity, multi-thread computation enabling, etc. [Solver Option Guide](solver_option_guide.md) gives a detailed description of the options and their default values.

## Accessing the Result
The result of the optimization is stored in the member `solution` in the `result` object, which is returned by the `solve()` function.
If one or more feasible solution was found in the optimization, the best solution among the feasible solutions is to be selected as the final best solution. 
Otherwise, the solution with the minimum augmented objective function value is to be selected. 
The augmented objective function is the sum of the original objective function value and the penalty values for violating the constraints.
The following code shows an example of accessing the optimization result.

```c++
printemps::model::IPModel model;

// Create decision variables.
auto& x = model.create_variable("x");
auto& y = model.create_variables("y", 1000);

// Create expressions.
auto& p = model.create_expression("p");
auto& q = model.create_expressions("q", {5, 5});

// Create constraints.
auto& g = model.create_constraint("g");
auto& h = model.create_constraints("h", {10, 10, 10});

// The formulation part is omitted.

auto result = printemps::solver::solve(&model);

// Print whether a feasible solution was obtained or not and the objective
// function value of the final best solution.
std::cout << result.solution.is_feasible() << std::endl;
std::cout << result.solution.objective() << std::endl;

// Print the value of the decision variables "x" and "y(0)" in the final
// best solution.
std::cout << result.solution.variables("x").value() << std::endl;
std::cout << result.solution.variables("y").values(0) << std::endl;

// Print the value of the expressions "p" and "q(0, 0) "in final best solution.
std::cout << result.solution.expressions("p").value() << std::endl;
std::cout << result.solution.expressions("q").values(0, 0) << std::endl;

// Print the value of the constraints "g" and "h(0, 0, 0)" and the violations
// for them in final best solution.
std::cout << result.solution.constraints("g").value() << std::endl;
std::cout << result.solution.constraints("h").values(0, 0, 0) << std::endl;
std::cout << result.solution.violations("g").value() << std::endl;
std::cout << result.solution.violations("h").values(0, 0, 0) << std::endl;
```

For accessing the result, the following methods are provided by the member `solution` in the `Result` object.

-  __Methods for accessing the optimization status:__ 
    -  The method `is_feasible()` returns `true` if the final best solution is feasible, and returns `false` otherwise.
    -  The method `objective()` returns the objective function value of the final best solution.                                                                            
        
- __Methods for accessing the values of decision variables:__ 
    - The method `variables(name).value()`  returns the value of the scalar (no-indexed) decision variable named `name` in the final best solution. The string `name` must correspond to the name given in the declaration; it also applies to expressions and constraints which are described below.
  
    - The method `variables(name).values(n_1, ..., n_m)` returns the value of the component indexed by `n_1, ..., n_m` of the multi-dimensional indexed decision variable named `name` in the final best solution.

- __Methods for accessing the values of expressions:__
    - The method `expressions(name).value()` returns the value of the scalar expression named `name` in the final solution. 
    - The method `expressions(name).values(n_1, ..., n_m)` returns the value of the component indexed by `n_1, ..., n_m` of the multi-dimensional indexed expression named `name` in the final solution.

- __Methods for accessing the values and violations of constraints:__
    - The methods `constraints(name).value()` returns the value of the scalar constraint named `name` in the final solution. The value obtained by this method is the left side when the constraint is expressed as `g(x) <=(=,>=) 0`.
    - The method `constraints(name).values(n_1, ..., n_m)`  returns the value of the component indexed by `n_1, ..., n_m` of the multi-dimensional indexed constraint named `name` in the final solution.
    - The method `violations(name).value()` method returns the violation for the scalar (no-indexed) constraint named `name` in the final solution. The value of the violation is non-negative.
    - The method `violations(name).values(n_1, ..., n_m)` method returns the violation for the component indexed by `n_1, ..., n_m` of the multi-dimensional indexed constraint named `name` in the final solution.

To make it easier to check the value, the utility functions `print_variable_values()`, `print_expression_values()`, `print_constraint_values()` and `print_violations_values()` are provided.
They respectively print the values of all variables, expressions,  constraints, and violations to the standard output. 
Please refer [example](https://github.com/snowberryfield/printemps/tree/master/example/simple_2.cpp).

## Work-in-Progress Topics
The following topics are to be written:
- User-Defined Neighborhoods
- Integrating Nonlinear Functions

For the user-defined neighborhoods and the non-linear function, please refer [example](https://github.com/snowberryfield/printemps/tree/master/example/quadratic_assignment.cpp).

## References

- [1] F.Glover: [Future Paths for Integer Programming and Links to Artificial Intelligence](http://leeds-faculty.colorado.edu/glover/TS%20-%20Future%20Paths%20for%20Integer%20Programming.pdf), _Computers and Operations Research_, Vol.13 No.5 pp.533-549 (1986).

- [2] K.Nonobe and T.Ibaraki: [An improved tabu search method for the weighted constraint satisfaction problem](https://www.researchgate.net/publication/228737620_An_Improved_Tabu_Search_Method_For_The_Weighted_Constraint_Satisfaction_Problem), _INFOR_ Vol.39, No.2 pp.131–151 (2001).

- [3] R.Fletcher: [Practical Methods of Optimization, Second Edition](https://onlinelibrary.wiley.com/doi/book/10.1002/9781118723203), John Wiley & Sons (2000).
