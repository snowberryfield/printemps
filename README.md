# PRINTEMPS
![GitHub Actions](https://github.com/snowberryfield/printemps/workflows/GitHub%20Actions/badge.svg) [![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

__PRINTEMPS__ is a C++ metaheuristics modeler/solver library for general integer optimization problems. PRINTEMPS emphasizes the following aspects:
- __Portability.__ It is implemented as a header-only library which does not depend on any other proprietary or open-source libraries. Users can integrate it into their own codes by just copying the necessary files. 

- __Intuitiveness in modeling.__ It provides an intuitive modeling environment to define an optimization model as a program code. Users can define constraint and objective functions using arithmetic operations on decision variables.

- __Flexibility to define neighborhood.__ It automatically detects the neighborhood structure of the problem. In addition to this, PRINTEMPS also accepts user-defined neighborhoods.

Please refer to the [project page](https://snowberryfield.github.io/printemps/) for details.

#### Citation
```
@ARTICLE{10418217,
  author={Koguma, Yuji},
  journal={IEEE Access}, 
  title={Tabu Search-Based Heuristic Solver for General Integer Linear Programming Problems}, 
  year={2024},
  volume={12},
  number={},
  pages={19059-19076},
  doi={10.1109/ACCESS.2024.3361323}}
```