---
title: PRINTEMPS
layout: sub
---

# Constraint Types

- [Constraint Types](#constraint-types)
    - [Overview](#overview)
    - [Classification and Description of Constraint Types](#classification-and-description-of-constraint-types)
        - [Singleton](#singleton)
        - [Exclusive OR](#exclusive-or)
        - [Exclusive NOR](#exclusive-nor)
        - [Inverted Integers](#inverted-integers)
        - [Balanced Integers](#balanced-integers)
        - [Constant Sum Integers](#constant-sum-integers)
        - [Constant Difference Integers](#constant-difference-integers)
        - [Constant Ratio Integers](#constant-ratio-integers)
        - [Aggregation](#aggregation)
        - [Precedence](#precedence)
        - [Variable Bound](#variable-bound)
        - [Trinomial Exclusive NOR](#trinomial-exclusive-nor)
        - [All Or Nothing](#all-or-nothing)
        - [Set Partitioning](#set-partitioning)
        - [Set Packing](#set-packing)
        - [Set Covering](#set-covering)
        - [Cardinality](#cardinality)
        - [Invariant Knapsack](#invariant-knapsack)
        - [Multiple Covering](#multiple-covering)
        - [Soft Selection](#soft-selection)
        - [Binary Flow](#binary-flow)
        - [Integer Flow](#integer-flow)
        - [Min-Max](#min-max)
        - [Max-Min](#max-min)
        - [Intermediate](#intermediate)
        - [Equation Knapsack](#equation-knapsack)
        - [Bin Packing](#bin-packing)
        - [Knapsack](#knapsack)
        - [Integer Knapsack](#integer-knapsack)
        - [GF(2)](#gf2)
        - [General Linear](#general-linear)

## Overview
PRINTEMPS automatically classifies constraint types and applies specialized preprocessing and neighborhood move generation according to the detected types. The 31 constraint types described below are considered; some correspond to categories on [MIPLIB 2017](https://miplib.zib.de), while others are subdivided or specialized for effective local search and preprocessing. During constraint classification, if a constraint matches more than one type, priority is assigned according to the order in the list below.

## Classification and Description of Constraint Types

### Singleton
- **Constraint Format:** $ax \le b$, $ax = b$, or $ax \ge b$

This constraint contains only a single variable. The preprocessor eliminates it by tightening the lower or upper bound of the variable, or fixes the variable to a constant value if the bounds become identical.

### Exclusive OR
- **Constraint Format:** $x_{1} + x_{2} = 1 \quad (x_{1}, x_{2} \in \{0, 1\})$

This constraint enforces that exactly one of two binary variables is equal to 1. The preprocessor optionally treats one variable as a dependent variable of the other ($x_{2} = 1 - x_{1}$). In addition, the neighborhood move generator produces swap moves that simultaneously flip both variables to maintain feasibility.

### Exclusive NOR
- **Constraint Format:** $x_{1} - x_{2} = 0 \quad (x_{1}, x_{2} \in \{0, 1\})$

This constraint enforces that two binary variables take identical values ($x_{1} = x_{2}$). The preprocessor extracts one variable as a dependent variable of the other. The neighborhood move generator produces moves that flip both variables simultaneously to maintain equality.

### Inverted Integers
- **Constraint Format:** $y_{1} + y_{2} = 0 \quad (y_{1}, y_{2} \in \mathbb{Z})$

This constraint requires two integer variables to sum to zero. The preprocessor optionally treats $y_{2}$ as a dependent variable of $y_{1}$ ($y_{2} = -y_{1}$). The neighborhood move generator shifts both integer variables with opposite signs ($y_{1} \leftarrow y_{1} + \delta, y_{2} \leftarrow y_{2} - \delta$) to preserve feasibility.

### Balanced Integers
- **Constraint Format:** $y_{1} - y_{2} = 0 \quad (y_{1}, y_{2} \in \mathbb{Z})$

This constraint enforces equality between two integer variables ($y_{1} = y_{2}$). The preprocessor extracts one variable as a dependent variable of the other. The neighborhood move generator shifts both integer variables simultaneously by the same increment ($y_{1} \leftarrow y_{1} + \delta, y_{2} \leftarrow y_{2} + \delta$).

### Constant Sum Integers
- **Constraint Format:** $y_{1} + y_{2} = a \quad (y_{1}, y_{2} \in \mathbb{Z}, a \in \mathbb{Z})$

This constraint requires two integer variables to sum to a constant integer $a$. The preprocessor optionally extracts $y_{2}$ as a dependent variable ($y_{2} = a - y_{1}$). The neighborhood move generator shifts both variables with opposite signs ($y_{1} \leftarrow y_{1} + \delta, y_{2} \leftarrow y_{2} - \delta$) to preserve the constant sum.

### Constant Difference Integers
- **Constraint Format:** $y_{1} - y_{2} = a \quad (y_{1}, y_{2} \in \mathbb{Z}, a \in \mathbb{Z})$

This constraint specifies that the difference between two integer variables is a constant integer $a$. The preprocessor optionally extracts $y_{2}$ as a dependent variable ($y_{2} = y_{1} - a$). The neighborhood move generator shifts both variables by the same increment to preserve their constant difference.

### Constant Ratio Integers
- **Constraint Format:** $y_{1} - a y_{2} = 0 \quad (y_{1}, y_{2} \in \mathbb{Z}, a \in \mathbb{Z})$

This constraint specifies a fixed proportional relationship between two integer variables. The preprocessor optionally extracts $y_{1}$ as a dependent variable ($y_{1} = a y_{2}$). The neighborhood move generator shifts both variables proportionally ($y_{1} \leftarrow y_{1} + a\delta, y_{2} \leftarrow y_{2} + \delta$).

### Aggregation
- **Constraint Format:** $a x_{1} = b x_{2} + c \quad (x_{1}, x_{2} \in \mathbb{Z}, a, b, c \in \mathbb{R})$

This constraint specifies a general linear equality between two integer variables. The preprocessor utilizes it to tighten variable bounds or eliminate dependent variables when integer divisibility holds. The neighborhood move generator coordinates changes to both variables to satisfy the relation.

### Precedence
- **Constraint Format:** $x_{1} \le x_{2} + a \quad (x_{1}, x_{2} \in \mathbb{Z}, a \in \mathbb{R})$

This constraint defines an ordering or precedence relationship between two variables. The specialized precedence move generator adjusts one variable whenever the other changes, maintaining or quickly restoring feasibility without violating the precedence condition.

### Variable Bound
- **Constraint Format:** $a x_{1} \le b x_{2} + c \quad (x_{1}, x_{2} \in \mathbb{Z}, a, b, c \in \mathbb{R})$

This constraint defines a variable upper or lower bound on variable $x_{1}$ that depends on variable $x_{2}$. The preprocessor tightens bounds based on domain implications, and the move generator produces moves that respect the dynamic boundary.

### Trinomial Exclusive NOR
- **Constraint Format:** $x_{1} + x_{2} = 2x' \quad (x_{1}, x_{2}, x' \in \{0, 1\})$

This constraint enforces equivalence among three binary variables ($x_{1} = x_{2} = x'$). The preprocessor eliminates $x_{1}$ and $x_{2}$ as dependent variables of $x'$. The neighborhood move generator flips all three variables simultaneously.

### All Or Nothing
- **Constraint Format:** $\sum_{i=1}^{n} x_{i} = n y \quad (x_{i} \in \{0, 1\}, y \in \{0, 1\})$

This constraint forces all binary variables $x_{i}$ to take the exact same value as an indicator variable $y$ (either all 0s or all 1s). The preprocessor extracts each $x_{i}$ as a dependent variable ($x_{i} = y$). The neighborhood move generator flips all associated variables synchronously.

### Set Partitioning
- **Constraint Format:** $\sum_{j \in \mathcal{J}} x_{j} = 1 \quad (x_{j} \in \{0, 1\})$

This constraint specifies that exactly one binary variable in a set $\mathcal{J}$ must take the value 1. The preprocessor identifies mutually exclusive candidate groups and eliminates redundant variables. The selection move generator generates swap moves that switch the active variable from one choice to another while strictly preserving feasibility.

### Set Packing
- **Constraint Format:** $\sum_{j \in \mathcal{J}} x_{j} \le 1 \quad (x_{j} \in \{0, 1\})$

This constraint specifies that at most one binary variable in a set $\mathcal{J}$ can take the value 1. The preprocessor tightens bounds and removes redundant conditions. The neighborhood move generator produces coordinated moves to avoid simultaneous activations.

### Set Covering
- **Constraint Format:** $\sum_{j \in \mathcal{J}} x_{j} \ge 1 \quad (x_{j} \in \{0, 1\})$

This constraint specifies that at least one binary variable in a set $\mathcal{J}$ must take the value 1. The preprocessor tightens bounds and eliminates dominated variables. The neighborhood move generator prioritizes activating uncovered variables when the constraint is violated.

### Cardinality
- **Constraint Format:** $\sum_{j \in \mathcal{J}} x_{j} = a \quad (x_{j} \in \{0, 1\}, a \in \mathbb{N}, a \ge 2)$

This constraint forces exactly $a$ binary variables in a set $\mathcal{J}$ to take the value 1. The neighborhood move generator performs swap moves between active and inactive variables to maintain the exact count of selected variables.

### Invariant Knapsack
- **Constraint Format:** $\sum_{j \in \mathcal{J}} x_{j} \le a \quad (x_{j} \in \{0, 1\}, a \in \mathbb{N}, a \ge 2)$

This constraint specifies an upper bound $a$ on the number of active binary variables in a set $\mathcal{J}$. The neighborhood move generator prevents exceeding the allowable number of active variables during local search.

### Multiple Covering
- **Constraint Format:** $\sum_{j \in \mathcal{J}} x_{j} \ge a \quad (x_{j} \in \{0, 1\}, a \in \mathbb{N}, a \ge 2)$

This constraint requires at least $a$ binary variables in a set $\mathcal{J}$ to take the value 1. The neighborhood move generator promotes the activation of variables when the active count falls below the required threshold $a$.

### Soft Selection
- **Constraint Format:** $\sum_{j \in \mathcal{J}} x_{j} = x' \quad (x_{j} \in \{0, 1\}, x' \in \{0, 1\})$

This constraint models conditional selection where choosing an alternative is optional. When the indicator variable $x' = 1$, exactly one $x_{j}$ must be 1; when $x' = 0$, all $x_{j}$ must be 0. The specialized soft selection move generator coordinates transitions between selection choices and the activation status of the indicator variable.

### Binary Flow
- **Constraint Format:** $\sum_{j \in \mathcal{J}} x_{j} = \sum_{k \in \mathcal{K}} x_{k} \quad (x_{j}, x_{k} \in \{0, 1\})$

This constraint models flow conservation at a network node where all flow variables are binary. The neighborhood move generator produces coordinated moves along paths or cycles to preserve flow balance.

### Integer Flow
- **Constraint Format:** $\sum_{j \in \mathcal{J}} x_{j} = \sum_{k \in \mathcal{K}} x_{k} \quad (x_{j}, x_{k} \in \mathbb{Z})$

This constraint models flow conservation at a network node where flow variables are general integers. The neighborhood move generator shifts flow values across inbound and outbound arcs to preserve flow balance.

### Min-Max
- **Constraint Format:** $\sum_{j \in \mathcal{J}} a_{j} x_{j} \le y \quad (y \in \mathbb{Z}, a_{j} > 0)$

This constraint enforces an upper bound on a linear expression via an auxiliary integer variable $y$. It frequently arises in minimax and bottleneck formulations, where the move generator dynamically balances the expression terms against the bound variable.

### Max-Min
- **Constraint Format:** $\sum_{j \in \mathcal{J}} a_{j} x_{j} \ge y \quad (y \in \mathbb{Z}, a_{j} > 0)$

This constraint enforces a lower bound on a linear expression via an auxiliary integer variable $y$. It frequently arises in maximin formulations, where the move generator dynamically balances the expression terms against the bound variable.

### Intermediate
- **Constraint Format:** $\sum_{j \in \mathcal{J}} a_{j} x_{j} = y \quad (y \in \mathbb{Z})$

This constraint defines an intermediate integer variable $y$ as a linear combination of other variables. The preprocessor eliminates $y$ as a dependent variable by substituting the expression throughout the model.

### Equation Knapsack
- **Constraint Format:** $\sum_{j \in \mathcal{J}} a_{j} x_{j} = b \quad (x_{j} \in \{0, 1\}, a_{j} > 0, b > 0)$

This constraint represents an equality-constrained 0-1 knapsack condition (subset-sum equality). The preprocessor performs feasibility checks and domain reduction, and the move generator adjusts variable assignments to satisfy the target sum.

### Bin Packing
- **Constraint Format:** $\sum_{j \in \mathcal{J}} a_{j} x_{j} \le b x' \quad (x_{j} \in \{0, 1\}, x' \in \{0, 1\}, a_{j} > 0, b > 0)$

This constraint represents a capacity constraint for a bin, where $x'$ is the binary indicator variable for whether the bin is open. Items can only be packed ($x_{j} = 1$) if the bin is open ($x' = 1$), and the move generator coordinates packing moves with bin opening and closing actions.

### Knapsack
- **Constraint Format:** $\sum_{j \in \mathcal{J}} a_{j} x_{j} \le b \quad (x_{j} \in \{0, 1\}, a_{j} > 0, b > 0)$

This constraint represents a standard 0-1 knapsack constraint with positive coefficients and capacity. The preprocessor detects redundant items and tightens capacity bounds, while the move generator evaluates item additions and removals based on penalty gradients.

### Integer Knapsack
- **Constraint Format:** $\sum_{j \in \mathcal{J}} a_{j} x_{j} \le b \quad (x_{j} \in \mathbb{Z}, a_{j} > 0, b > 0)$

This constraint represents a general integer knapsack constraint with positive integer coefficients and capacity. The preprocessor tightens variable upper bounds according to the knapsack capacity, and the move generator adjusts integer levels to satisfy the capacity limit.

### GF(2)
- **Constraint Format:** $\sum_{j \in \mathcal{J}} x_{j} = 2y + a \quad (x_{j} \in \{0, 1\}, y \in \mathbb{Z}, a \in \{0, 1\})$

This constraint represents a linear system over the Galois Field of two elements (GF(2)), corresponding to a parity or modulo-2 constraint. The preprocessor applies GF(2) Gaussian elimination to detect linear dependencies, eliminate redundant variables, and fix variable values.

### General Linear
- **Constraint Format:** Any other linear equality or inequality constraint.

This category encompasses all general linear constraints that do not match the specialized structures above. The solver evaluates constraint violations using dynamic penalty functions and applies general local search moves to satisfy them.