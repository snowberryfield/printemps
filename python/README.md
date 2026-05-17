# printemps Python bindings

Python bindings for [printemps](../README.md), a C++ header-only integer
linear programming solver, built with [pybind11](https://pybind11.readthedocs.io/).

## Build & install

From the repository root:

```bash
pip install ./python
```

Or, for editable development installs:

```bash
pip install -e ./python
```

The build uses `scikit-build-core` and CMake. A C++17 compiler and CMake
≥ 3.15 are required.

## Quick start

The Python API mirrors the C++ API closely. The main difference is that
constraint and expression slot assignment uses `proxy[key] = ...` instead
of the C++ `proxy(key) = ...` (Python cannot overload `=` on a function
call).

```python
import printemps

model = printemps.IPModel()
x = model.create_variables("x", 2, -100, 100)
g = model.create_constraints("g", 2)

g[0] = 66 * x(0) + 14 * x(1) >= 1430
g[1] = -82 * x(0) + 28 * x(1) >= 1306
model.minimize(x(0) + 10 * x(1))

result = printemps.solve(model)
print("objective =", result.solution.objective())
print("x(0) =", result.solution.variables("x")(0))
print("x(1) =", result.solution.variables("x")(1))
```

Translated versions of all C++ examples live under
`printemps/examples/`:

```bash
python -m printemps.examples.simple_1
python -m printemps.examples.simple_2
python -m printemps.examples.knapsack
python -m printemps.examples.bin_packing
python -m printemps.examples.sudoku
```

## Standalone CLI

Installing the package also registers a `printemps` console script that
wraps `printemps::standalone::Standalone(argc, argv).run()` — the same
binary entry point as the C++ executable in `application/standalone/`.

```bash
printemps path/to/instance.mps -t 60 -v 2
```

See `application/standalone/main.cpp` and `printemps/standalone/argparser.h`
for the full set of supported flags.

## Releasing to PyPI

The printemps C++ headers live in the sibling `printemps/` directory of
this repository. For development builds CMake picks them up directly. For
distribution, the headers are vendored into `python/_vendor/printemps/`
and bundled into the source tarball so end users do not need a checkout
of the printemps repository to install from PyPI.

```bash
cd python
python tools/vendor.py        # copy ../printemps/ -> _vendor/printemps/
python -m build               # produces dist/printemps-X.Y.Z.tar.gz and the wheel
twine upload dist/*           # publish
```

`tools/vendor.py` is idempotent and can be re-run before each release.
`_vendor/` is gitignored, so day-to-day development is unaffected.

## Translation cheat sheet

| C++ idiom                                   | Python idiom                                  |
|---------------------------------------------|-----------------------------------------------|
| `auto& x = model.create_variables(...)`     | `x = model.create_variables(...)`             |
| `g(0) = expr >= 1430`                       | `g[0] = expr >= 1430`                         |
| `f(0) = expr`                               | `f[0] = expr`                                 |
| `objective += term`                         | `objective += term`                           |
| `expression_block(i, j, k) += x(n, m, k)`   | `expression_block[i, j, k] += x(n, m, k)`     |
| `x.selection({n, Range::All, k})`           | `x.selection([n, printemps.ALL, k])`          |
| `option.output.verbose = verbose::Full`     | `option.output.verbose = printemps.Verbose.Full` |
| `result.solution.variables("x").values(n,m)`| `result.solution.variables("x")(n, m)`        |
