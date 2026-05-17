// Python bindings for the printemps C++ integer linear programming solver.
//
// Only the Model<int, double> instantiation (alias IPModel) is exposed,
// matching what the C++ examples and the standalone executable use.

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/operators.h>
#include <pybind11/functional.h>

#include <memory>
#include <string>
#include <vector>

#include <printemps.h>

namespace py = pybind11;
namespace pp = printemps;
namespace mc = printemps::model_component;

using Var       = mc::Variable<int, double>;
using VarProxy  = mc::VariableProxy<int, double>;
using Expr      = mc::Expression<int, double>;
using ExprProxy = mc::ExpressionProxy<int, double>;
using Cons      = mc::Constraint<int, double>;
using ConsProxy = mc::ConstraintProxy<int, double>;
using Model     = pp::model::Model<int, double>;
using Result    = pp::solver::Result<int, double>;
using Status    = pp::solver::Status<int, double>;
using NamedSol  = pp::solution::NamedSolution<int, double>;
using IntValueProxy    = pp::multi_array::ValueProxy<int>;
using DoubleValueProxy = pp::multi_array::ValueProxy<double>;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

// Convert a Python key (int / tuple / list) to std::vector<int>. Used for
// proxy indexing where the C++ side accepts either a flat int or a multi-dim
// std::vector<int>.
static std::vector<int> key_to_index(const py::object &key) {
    std::vector<int> idx;
    if (py::isinstance<py::int_>(key)) {
        idx.push_back(key.cast<int>());
    } else if (py::isinstance<py::tuple>(key) || py::isinstance<py::list>(key)) {
        for (auto k : key) {
            idx.push_back(k.cast<int>());
        }
    } else {
        throw py::type_error(
            "Index must be an int, a tuple of ints, or a list of ints.");
    }
    return idx;
}

// Helper: convert an int / Variable / Expression Python object into an Expr.
// Used for arithmetic on the Python side where the operand types are mixed.
static Expr to_expr(const py::object &obj) {
    if (py::isinstance<py::int_>(obj)) {
        Expr e = Expr::create_instance();
        e = obj.cast<int>();
        return e;
    }
    if (py::isinstance<py::float_>(obj)) {
        Expr e = Expr::create_instance();
        e = obj.cast<double>();
        return e;
    }
    try {
        return obj.cast<const Expr &>().copy();
    } catch (const py::cast_error &) {
    }
    try {
        return obj.cast<const Var &>().to_expression();
    } catch (const py::cast_error &) {
    }
    try {
        return obj.cast<const VarProxy &>().to_expression();
    } catch (const py::cast_error &) {
    }
    try {
        return obj.cast<const ExprProxy &>().to_expression();
    } catch (const py::cast_error &) {
    }
    throw py::type_error(
        "Operand must be an int, Variable, Expression, VariableProxy "
        "(scalar), or ExpressionProxy (scalar).");
}

// ---------------------------------------------------------------------------
// Module
// ---------------------------------------------------------------------------

PYBIND11_MODULE(_printemps, m) {
    m.doc() =
        "Python bindings for printemps "
        "(integer linear programming solver).";

    // ----- Range enum -----------------------------------------------------
    py::enum_<mc::Range>(m, "Range",
                         "Sentinel for partial multi-dimensional indexing. "
                         "Use Range.All to denote 'all elements along this "
                         "axis' in selection/dot/sum.")
        .value("All", mc::Range::All)
        .export_values();

    // ----- Verbosity / mode enums ----------------------------------------
    py::enum_<pp::option::verbose::Verbose>(m, "Verbose")
        .value("Off", pp::option::verbose::Off)
        .value("Warning", pp::option::verbose::Warning)
        .value("Outer", pp::option::verbose::Outer)
        .value("Inner", pp::option::verbose::Inner)
        .value("Full", pp::option::verbose::Full)
        .value("Debug", pp::option::verbose::Debug);

    py::enum_<pp::option::tabu_mode::TabuMode>(m, "TabuMode")
        .value("All", pp::option::tabu_mode::All)
        .value("Any", pp::option::tabu_mode::Any);

    py::enum_<pp::option::selection_mode::SelectionMode>(m, "SelectionMode")
        .value("Off", pp::option::selection_mode::Off)
        .value("Defined", pp::option::selection_mode::Defined)
        .value("Smaller", pp::option::selection_mode::Smaller)
        .value("Larger", pp::option::selection_mode::Larger)
        .value("Independent", pp::option::selection_mode::Independent)
        .value("UserDefined", pp::option::selection_mode::UserDefined);

    // ----- Variable -------------------------------------------------------
    // Variable has deleted copy constructor; pybind11 must never own or copy
    // it. Always returned by reference from a VariableProxy that lives inside
    // the Model.
    py::class_<Var, std::unique_ptr<Var, py::nodelete>>(m, "Variable")
        .def("value", &Var::value)
        .def("set_value", &Var::set_value, py::arg("value"))
        .def("fix", &Var::fix)
        .def("unfix", &Var::unfix)
        .def("is_fixed", &Var::is_fixed)
        .def("fix_by", &Var::fix_by, py::arg("value"))
        .def("lower_bound", &Var::lower_bound)
        .def("upper_bound", &Var::upper_bound)
        .def("set_bound", &Var::set_bound,
             py::arg("lower"), py::arg("upper"))
        .def("to_expression", &Var::to_expression)
        // Unary operators
        .def("__neg__", [](const Var &v) { return -v.to_expression(); })
        .def("__pos__", [](const Var &v) { return v.to_expression(); })
        // Binary arithmetic; rely on to_expr() to coerce the right-hand side.
        .def("__add__",
             [](const Var &v, const py::object &o) {
                 return v.to_expression() + to_expr(o);
             })
        .def("__radd__",
             [](const Var &v, const py::object &o) {
                 return to_expr(o) + v.to_expression();
             })
        .def("__sub__",
             [](const Var &v, const py::object &o) {
                 return v.to_expression() - to_expr(o);
             })
        .def("__rsub__",
             [](const Var &v, const py::object &o) {
                 return to_expr(o) - v.to_expression();
             })
        .def("__mul__",
             [](const Var &v, int c) { return v.to_expression() * c; })
        .def("__mul__",
             [](const Var &v, double c) { return v.to_expression() * c; })
        .def("__rmul__",
             [](const Var &v, int c) { return v.to_expression() * c; })
        .def("__rmul__",
             [](const Var &v, double c) { return v.to_expression() * c; })
        // Comparison: build constraint
        .def("__le__",
             [](const Var &v, const py::object &o) {
                 return v.to_expression() - to_expr(o) <= 0;
             })
        .def("__ge__",
             [](const Var &v, const py::object &o) {
                 return v.to_expression() - to_expr(o) >= 0;
             })
        .def("__eq__",
             [](const Var &v, const py::object &o) {
                 return v.to_expression() - to_expr(o) == 0;
             });

    // ----- Expression -----------------------------------------------------
    // Expression has private default ctor; build via Expr::create_instance.
    py::class_<Expr>(m, "Expression")
        .def(py::init([] { return Expr::create_instance(); }))
        .def("value", &Expr::value)
        .def("evaluate", py::overload_cast<>(&Expr::evaluate, py::const_))
        .def("constant_value", &Expr::constant_value)
        .def("is_enabled", &Expr::is_enabled)
        .def("enable", &Expr::enable)
        .def("disable", &Expr::disable)
        .def("copy", &Expr::copy)
        // Unary
        .def("__neg__", [](const Expr &e) { return -e; })
        .def("__pos__", [](const Expr &e) { return +e; })
        // Binary arithmetic
        .def("__add__",
             [](const Expr &e, const py::object &o) { return e + to_expr(o); })
        .def("__radd__",
             [](const Expr &e, const py::object &o) { return to_expr(o) + e; })
        .def("__sub__",
             [](const Expr &e, const py::object &o) { return e - to_expr(o); })
        .def("__rsub__",
             [](const Expr &e, const py::object &o) { return to_expr(o) - e; })
        .def("__mul__", [](const Expr &e, int c) { return e * c; })
        .def("__mul__", [](const Expr &e, double c) { return e * c; })
        .def("__rmul__", [](const Expr &e, int c) { return e * c; })
        .def("__rmul__", [](const Expr &e, double c) { return e * c; })
        .def("__truediv__", [](const Expr &e, int c) { return e / c; })
        .def("__truediv__", [](const Expr &e, double c) { return e / c; })
        // In-place arithmetic; mutates the underlying Expression. Combined
        // with reference_internal on proxy::__call__ this lets users write
        //     expr_proxy(i, j, k) += x(n, m, k)
        // mirroring the C++ Expression& += Variable usage in sudoku.cpp.
        .def("__iadd__",
             [](Expr &self, const py::object &o) -> Expr & {
                 self += to_expr(o);
                 return self;
             },
             py::return_value_policy::reference)
        .def("__isub__",
             [](Expr &self, const py::object &o) -> Expr & {
                 self -= to_expr(o);
                 return self;
             },
             py::return_value_policy::reference)
        .def("__imul__",
             [](Expr &self, int c) -> Expr & {
                 self *= c;
                 return self;
             },
             py::return_value_policy::reference)
        .def("__imul__",
             [](Expr &self, double c) -> Expr & {
                 self *= c;
                 return self;
             },
             py::return_value_policy::reference)
        // Comparison
        .def("__le__",
             [](const Expr &e, const py::object &o) {
                 return e - to_expr(o) <= 0;
             })
        .def("__ge__",
             [](const Expr &e, const py::object &o) {
                 return e - to_expr(o) >= 0;
             })
        .def("__eq__",
             [](const Expr &e, const py::object &o) {
                 return e - to_expr(o) == 0;
             });

    // ----- VariableProxy --------------------------------------------------
    // VariableProxy is owned by Model; never let pybind11 own it.
    py::class_<VarProxy, std::unique_ptr<VarProxy, py::nodelete>>(
        m, "VariableProxy")
        .def("number_of_elements", &VarProxy::number_of_elements)
        .def("number_of_dimensions", &VarProxy::number_of_dimensions)
        .def("shape", &VarProxy::shape)
        .def("set_bound", &VarProxy::set_bound,
             py::arg("lower"), py::arg("upper"))
        // Multi-dim element access via __call__(*args)
        .def("__call__",
             [](VarProxy &self, py::args args) -> Var & {
                 std::vector<int> idx;
                 idx.reserve(args.size());
                 for (auto a : args) idx.push_back(a.cast<int>());
                 return self(idx);
             },
             py::return_value_policy::reference_internal)
        // Pythonic indexing via __getitem__ (int or tuple)
        .def("__getitem__",
             [](VarProxy &self, const py::object &key) -> Var & {
                 return self(key_to_index(key));
             },
             py::return_value_policy::reference_internal)
        // x[i] = 0 to assign initial value, mirrors C++ x(i) = 0.
        .def("__setitem__",
             [](VarProxy &self, const py::object &key, int value) {
                 self(key_to_index(key)).set_value(value);
             })
        // Aggregations
        .def("sum", py::overload_cast<>(&VarProxy::sum, py::const_))
        .def("sum",
             py::overload_cast<const std::vector<int> &>(&VarProxy::sum,
                                                         py::const_),
             py::arg("indices"))
        .def("dot",
             [](VarProxy &self, const std::vector<double> &coeffs) {
                 return self.dot(coeffs);
             },
             py::arg("coefficients"))
        .def("dot",
             [](VarProxy &self, const std::vector<int> &coeffs) {
                 return self.dot(coeffs);
             },
             py::arg("coefficients"))
        .def("dot",
             [](VarProxy &self, const std::vector<int> &indices,
                const std::vector<double> &coeffs) {
                 return self.dot(indices, coeffs);
             },
             py::arg("indices"), py::arg("coefficients"))
        .def("dot",
             [](VarProxy &self, const std::vector<int> &indices,
                const std::vector<int> &coeffs) {
                 return self.dot(indices, coeffs);
             },
             py::arg("indices"), py::arg("coefficients"))
        .def("selection",
             py::overload_cast<>(&VarProxy::selection, py::const_))
        .def("selection",
             py::overload_cast<const std::vector<int> &>(&VarProxy::selection,
                                                         py::const_),
             py::arg("indices"))
        // Singleton-only helpers (mirror C++): allow x.value(), x.fix_by(...)
        .def("value", &VarProxy::value)
        .def("set_value", &VarProxy::set_value, py::arg("value"))
        .def("fix_by", &VarProxy::fix_by, py::arg("value"))
        .def("to_expression", &VarProxy::to_expression)
        // Singleton arithmetic / comparison: enable usage like
        //     model.create_constraint("c", proxy <= 200)
        // when proxy holds a single element (returned by create_expression).
        // For multi-element proxies these will throw at the C++ layer.
        .def("__neg__",
             [](const VarProxy &self) { return -self.to_expression(); })
        .def("__pos__",
             [](const VarProxy &self) { return self.to_expression(); })
        .def("__add__",
             [](const VarProxy &self, const py::object &o) {
                 return self.to_expression() + to_expr(o);
             })
        .def("__radd__",
             [](const VarProxy &self, const py::object &o) {
                 return to_expr(o) + self.to_expression();
             })
        .def("__sub__",
             [](const VarProxy &self, const py::object &o) {
                 return self.to_expression() - to_expr(o);
             })
        .def("__rsub__",
             [](const VarProxy &self, const py::object &o) {
                 return to_expr(o) - self.to_expression();
             })
        .def("__mul__",
             [](const VarProxy &self, int c) { return self.to_expression() * c; })
        .def("__mul__",
             [](const VarProxy &self, double c) { return self.to_expression() * c; })
        .def("__rmul__",
             [](const VarProxy &self, int c) { return self.to_expression() * c; })
        .def("__rmul__",
             [](const VarProxy &self, double c) { return self.to_expression() * c; })
        .def("__le__",
             [](const VarProxy &self, const py::object &o) {
                 return self.to_expression() - to_expr(o) <= 0;
             })
        .def("__ge__",
             [](const VarProxy &self, const py::object &o) {
                 return self.to_expression() - to_expr(o) >= 0;
             })
        .def("__eq__",
             [](const VarProxy &self, const py::object &o) {
                 return self.to_expression() - to_expr(o) == 0;
             });

    // ----- ExpressionProxy -----------------------------------------------
    py::class_<ExprProxy, std::unique_ptr<ExprProxy, py::nodelete>>(
        m, "ExpressionProxy")
        .def("number_of_elements", &ExprProxy::number_of_elements)
        .def("number_of_dimensions", &ExprProxy::number_of_dimensions)
        .def("shape", &ExprProxy::shape)
        .def("disable", py::overload_cast<>(&ExprProxy::disable))
        .def("enable", py::overload_cast<>(&ExprProxy::enable))
        .def("__call__",
             [](ExprProxy &self, py::args args) -> Expr & {
                 std::vector<int> idx;
                 idx.reserve(args.size());
                 for (auto a : args) idx.push_back(a.cast<int>());
                 return self(idx);
             },
             py::return_value_policy::reference_internal)
        .def("__getitem__",
             [](ExprProxy &self, const py::object &key) -> Expr & {
                 return self(key_to_index(key));
             },
             py::return_value_policy::reference_internal)
        // f[i] = expr  →  proxy(idx) = expr  (mirrors C++ f(i) = expr)
        .def("__setitem__",
             [](ExprProxy &self, const py::object &key,
                const py::object &value) {
                 self(key_to_index(key)) = to_expr(value);
             })
        .def("sum", py::overload_cast<>(&ExprProxy::sum, py::const_))
        // Singleton helpers for `objective += term` style usage.
        .def("to_expression", &ExprProxy::to_expression)
        .def("__iadd__",
             [](ExprProxy &self, const py::object &o) -> ExprProxy & {
                 self += to_expr(o);
                 return self;
             },
             py::return_value_policy::reference)
        .def("__neg__",
             [](const ExprProxy &self) { return -self.to_expression(); })
        .def("__pos__",
             [](const ExprProxy &self) { return self.to_expression(); })
        .def("__add__",
             [](const ExprProxy &self, const py::object &o) {
                 return self.to_expression() + to_expr(o);
             })
        .def("__radd__",
             [](const ExprProxy &self, const py::object &o) {
                 return to_expr(o) + self.to_expression();
             })
        .def("__sub__",
             [](const ExprProxy &self, const py::object &o) {
                 return self.to_expression() - to_expr(o);
             })
        .def("__rsub__",
             [](const ExprProxy &self, const py::object &o) {
                 return to_expr(o) - self.to_expression();
             })
        .def("__mul__",
             [](const ExprProxy &self, int c) { return self.to_expression() * c; })
        .def("__mul__",
             [](const ExprProxy &self, double c) { return self.to_expression() * c; })
        .def("__rmul__",
             [](const ExprProxy &self, int c) { return self.to_expression() * c; })
        .def("__rmul__",
             [](const ExprProxy &self, double c) { return self.to_expression() * c; })
        .def("__le__",
             [](const ExprProxy &self, const py::object &o) {
                 return self.to_expression() - to_expr(o) <= 0;
             })
        .def("__ge__",
             [](const ExprProxy &self, const py::object &o) {
                 return self.to_expression() - to_expr(o) >= 0;
             })
        .def("__eq__",
             [](const ExprProxy &self, const py::object &o) {
                 return self.to_expression() - to_expr(o) == 0;
             });

    // ----- Constraint -----------------------------------------------------
    // Constraint has private default ctor; build via Cons::create_instance.
    py::class_<Cons>(m, "Constraint")
        .def(py::init([] { return Cons::create_instance(); }))
        .def("name", &Cons::name)
        .def("is_enabled", &Cons::is_enabled)
        .def("enable", &Cons::enable)
        .def("disable", &Cons::disable);

    // ----- ConstraintProxy -----------------------------------------------
    py::class_<ConsProxy, std::unique_ptr<ConsProxy, py::nodelete>>(
        m, "ConstraintProxy")
        .def("number_of_elements", &ConsProxy::number_of_elements)
        .def("number_of_dimensions", &ConsProxy::number_of_dimensions)
        .def("shape", &ConsProxy::shape)
        .def("disable", py::overload_cast<>(&ConsProxy::disable))
        .def("enable", py::overload_cast<>(&ConsProxy::enable))
        .def("__call__",
             [](ConsProxy &self, py::args args) -> Cons & {
                 std::vector<int> idx;
                 idx.reserve(args.size());
                 for (auto a : args) idx.push_back(a.cast<int>());
                 return self(idx);
             },
             py::return_value_policy::reference_internal)
        .def("__getitem__",
             [](ConsProxy &self, const py::object &key) -> Cons & {
                 return self(key_to_index(key));
             },
             py::return_value_policy::reference_internal)
        // g[i] = constraint  →  proxy(idx) = constraint
        .def("__setitem__",
             [](ConsProxy &self, const py::object &key, const Cons &c) {
                 self(key_to_index(key)) = c;
             });

    // ----- ValueProxy<int> / <double> ------------------------------------
    // These are returned by NamedSolution::variables(name) etc. They live as
    // long as the NamedSolution that produced them, so reference_internal.
    py::class_<IntValueProxy>(m, "IntValueProxy")
        .def("number_of_elements", &IntValueProxy::number_of_elements)
        .def("number_of_dimensions", &IntValueProxy::number_of_dimensions)
        .def("shape", &IntValueProxy::shape)
        .def("flat_indexed_values",
             py::overload_cast<>(&IntValueProxy::flat_indexed_values,
                                 py::const_))
        .def("__call__",
             [](const IntValueProxy &self, py::args args) {
                 std::vector<int> idx;
                 idx.reserve(args.size());
                 for (auto a : args) idx.push_back(a.cast<int>());
                 return self.values(idx);
             })
        .def("values",
             [](const IntValueProxy &self, const std::vector<int> &idx) {
                 return self.values(idx);
             },
             py::arg("indices"))
        .def("__getitem__",
             [](const IntValueProxy &self, const py::object &key) {
                 return self.values(key_to_index(key));
             });

    py::class_<DoubleValueProxy>(m, "DoubleValueProxy")
        .def("number_of_elements", &DoubleValueProxy::number_of_elements)
        .def("number_of_dimensions", &DoubleValueProxy::number_of_dimensions)
        .def("shape", &DoubleValueProxy::shape)
        .def("flat_indexed_values",
             py::overload_cast<>(&DoubleValueProxy::flat_indexed_values,
                                 py::const_))
        .def("__call__",
             [](const DoubleValueProxy &self, py::args args) {
                 std::vector<int> idx;
                 idx.reserve(args.size());
                 for (auto a : args) idx.push_back(a.cast<int>());
                 return self.values(idx);
             })
        .def("values",
             [](const DoubleValueProxy &self, const std::vector<int> &idx) {
                 return self.values(idx);
             },
             py::arg("indices"))
        .def("__getitem__",
             [](const DoubleValueProxy &self, const py::object &key) {
                 return self.values(key_to_index(key));
             });

    // ----- Model (IPModel) -----------------------------------------------
    py::class_<Model>(m, "IPModel")
        .def(py::init<>())
        .def(py::init<const std::string &>(), py::arg("name"))
        .def("name", &Model::name)
        .def("set_name", &Model::set_name, py::arg("name"))
        // create_variable(name) — scalar variable. Lambdas are used
        // throughout the create_* family to avoid py::overload_cast picking
        // up Model's templated overloads, whose instantiations would
        // collide with the non-templated Expression& / Constraint& versions.
        .def("create_variable",
             [](Model &self, const std::string &name) -> VarProxy & {
                 return self.create_variable(name);
             },
             py::arg("name"),
             py::return_value_policy::reference_internal)
        .def("create_variable",
             [](Model &self, const std::string &name, int lower,
                int upper) -> VarProxy & {
                 return self.create_variable(name, lower, upper);
             },
             py::arg("name"), py::arg("lower"), py::arg("upper"),
             py::return_value_policy::reference_internal)
        .def("create_variables",
             [](Model &self, const std::string &name,
                int number_of_elements) -> VarProxy & {
                 return self.create_variables(name, number_of_elements);
             },
             py::arg("name"), py::arg("number_of_elements"),
             py::return_value_policy::reference_internal)
        .def("create_variables",
             [](Model &self, const std::string &name, int number_of_elements,
                int lower, int upper) -> VarProxy & {
                 return self.create_variables(name, number_of_elements, lower,
                                              upper);
             },
             py::arg("name"), py::arg("number_of_elements"),
             py::arg("lower"), py::arg("upper"),
             py::return_value_policy::reference_internal)
        .def("create_variables",
             [](Model &self, const std::string &name,
                const std::vector<int> &shape) -> VarProxy & {
                 return self.create_variables(name, shape);
             },
             py::arg("name"), py::arg("shape"),
             py::return_value_policy::reference_internal)
        .def("create_variables",
             [](Model &self, const std::string &name,
                const std::vector<int> &shape, int lower,
                int upper) -> VarProxy & {
                 return self.create_variables(name, shape, lower, upper);
             },
             py::arg("name"), py::arg("shape"),
             py::arg("lower"), py::arg("upper"),
             py::return_value_policy::reference_internal)
        .def("create_expression",
             [](Model &self, const std::string &name) -> ExprProxy & {
                 return self.create_expression(name);
             },
             py::arg("name"),
             py::return_value_policy::reference_internal)
        .def("create_expression",
             [](Model &self, const std::string &name,
                const Expr &expr) -> ExprProxy & {
                 return self.create_expression(name, expr);
             },
             py::arg("name"), py::arg("expression"),
             py::return_value_policy::reference_internal)
        .def("create_expressions",
             [](Model &self, const std::string &name,
                int number_of_elements) -> ExprProxy & {
                 return self.create_expressions(name, number_of_elements);
             },
             py::arg("name"), py::arg("number_of_elements"),
             py::return_value_policy::reference_internal)
        .def("create_expressions",
             [](Model &self, const std::string &name,
                const std::vector<int> &shape) -> ExprProxy & {
                 return self.create_expressions(name, shape);
             },
             py::arg("name"), py::arg("shape"),
             py::return_value_policy::reference_internal)
        .def("create_constraint",
             [](Model &self, const std::string &name) -> ConsProxy & {
                 return self.create_constraint(name);
             },
             py::arg("name"),
             py::return_value_policy::reference_internal)
        .def("create_constraint",
             [](Model &self, const std::string &name,
                const Cons &constraint) -> ConsProxy & {
                 return self.create_constraint(name, constraint);
             },
             py::arg("name"), py::arg("constraint"),
             py::return_value_policy::reference_internal)
        .def("create_constraints",
             [](Model &self, const std::string &name,
                int number_of_elements) -> ConsProxy & {
                 return self.create_constraints(name, number_of_elements);
             },
             py::arg("name"), py::arg("number_of_elements"),
             py::return_value_policy::reference_internal)
        .def("create_constraints",
             [](Model &self, const std::string &name,
                const std::vector<int> &shape) -> ConsProxy & {
                 return self.create_constraints(name, shape);
             },
             py::arg("name"), py::arg("shape"),
             py::return_value_policy::reference_internal)
        // Objective
        .def("minimize",
             [](Model &self, const py::object &obj) {
                 self.minimize(to_expr(obj));
             },
             py::arg("expression"))
        .def("maximize",
             [](Model &self, const py::object &obj) {
                 self.maximize(to_expr(obj));
             },
             py::arg("expression"));

    // ----- Option (top-level + nested structs + JSON setup) --------------
    py::class_<pp::option::GeneralOption>(m, "GeneralOption")
        .def_readwrite("iteration_max",
                       &pp::option::GeneralOption::iteration_max)
        .def_readwrite("time_max", &pp::option::GeneralOption::time_max)
        .def_readwrite("time_offset", &pp::option::GeneralOption::time_offset)
        .def_readwrite("target_objective_value",
                       &pp::option::GeneralOption::target_objective_value)
        .def_readwrite("seed", &pp::option::GeneralOption::seed)
        .def_readwrite("is_enabled_fast_evaluation",
                       &pp::option::GeneralOption::is_enabled_fast_evaluation);

    py::class_<pp::option::OutputOption>(m, "OutputOption")
        .def_readwrite("verbose", &pp::option::OutputOption::verbose);

    py::class_<pp::option::NeighborhoodOption>(m, "NeighborhoodOption")
        .def_readwrite("selection_mode",
                       &pp::option::NeighborhoodOption::selection_mode);

    py::class_<pp::option::TabuSearchOption>(m, "TabuSearchOption")
        .def_readwrite("iteration_max",
                       &pp::option::TabuSearchOption::iteration_max)
        .def_readwrite("time_max", &pp::option::TabuSearchOption::time_max)
        .def_readwrite("initial_tabu_tenure",
                       &pp::option::TabuSearchOption::initial_tabu_tenure)
        .def_readwrite("tabu_mode", &pp::option::TabuSearchOption::tabu_mode);

    py::class_<pp::option::Option>(m, "Option")
        .def(py::init<>())
        .def(py::init<const std::string &>(), py::arg("json_path"))
        .def("setup", &pp::option::Option::setup, py::arg("json_path"))
        .def_readwrite("general", &pp::option::Option::general)
        .def_readwrite("output", &pp::option::Option::output)
        .def_readwrite("neighborhood", &pp::option::Option::neighborhood)
        .def_readwrite("tabu_search", &pp::option::Option::tabu_search);

    // ----- NamedSolution / Status / Result -------------------------------
    py::class_<NamedSol>(m, "NamedSolution")
        .def("name", &NamedSol::name)
        .def("number_of_variables", &NamedSol::number_of_variables)
        .def("number_of_constraints", &NamedSol::number_of_constraints)
        .def("objective", &NamedSol::objective)
        .def("total_violation", &NamedSol::total_violation)
        .def("is_feasible", &NamedSol::is_feasible)
        .def("variables",
             py::overload_cast<const std::string &>(&NamedSol::variables,
                                                    py::const_),
             py::arg("name"),
             py::return_value_policy::reference_internal)
        .def("expressions",
             py::overload_cast<const std::string &>(&NamedSol::expressions,
                                                    py::const_),
             py::arg("name"),
             py::return_value_policy::reference_internal)
        .def("constraints",
             py::overload_cast<const std::string &>(&NamedSol::constraints,
                                                    py::const_),
             py::arg("name"),
             py::return_value_policy::reference_internal)
        .def("violations",
             py::overload_cast<const std::string &>(&NamedSol::violations,
                                                    py::const_),
             py::arg("name"),
             py::return_value_policy::reference_internal)
        .def("print_variable_values", &NamedSol::print_variable_values)
        .def("print_expression_values", &NamedSol::print_expression_values)
        .def("print_constraint_values", &NamedSol::print_constraint_values)
        .def("print_violation_values", &NamedSol::print_violation_values)
        .def("write_json_by_name", &NamedSol::write_json_by_name,
             py::arg("path"))
        .def("write_json_by_array", &NamedSol::write_json_by_array,
             py::arg("path"))
        .def("write_solution", &NamedSol::write_solution, py::arg("path"));

    py::class_<Status>(m, "Status")
        .def_readonly("objective", &Status::objective)
        .def_readonly("total_violation", &Status::total_violation)
        .def_readonly("is_found_feasible_solution",
                      &Status::is_found_feasible_solution)
        .def_readonly("elapsed_time", &Status::elapsed_time)
        .def_readonly("number_of_variables", &Status::number_of_variables)
        .def_readonly("number_of_constraints", &Status::number_of_constraints)
        .def_readonly("number_of_lagrange_dual_iterations",
                      &Status::number_of_lagrange_dual_iterations)
        .def_readonly("number_of_local_search_iterations",
                      &Status::number_of_local_search_iterations)
        .def_readonly("number_of_tabu_search_iterations",
                      &Status::number_of_tabu_search_iterations);

    py::class_<Result>(m, "Result")
        .def_readonly("solution", &Result::solution)
        .def_readonly("status", &Result::status);

    // ----- solve() --------------------------------------------------------
    m.def("solve",
          [](Model *model, const pp::option::Option *option) {
              py::gil_scoped_release release;
              if (option == nullptr) {
                  return pp::solver::solve(model);
              }
              return pp::solver::solve(model, *option);
          },
          py::arg("model"), py::arg("option") = nullptr,
          "Solve the given model. Returns a Result containing the named "
          "solution and solver status.");

    // ----- Standalone executable wrapper ---------------------------------
    // A small Python-side wrapper class that owns the argv strings and
    // delegates to pp::standalone::Standalone(argc, argv).run().
    py::class_<pp::standalone::Standalone>(m, "Standalone")
        .def(py::init([](const std::vector<std::string> &argv) {
                 // Build a stable char* array from the strings. Standalone
                 // copies what it needs into m_argparser during setup(), so
                 // these temporary buffers only need to outlive the ctor.
                 std::vector<std::vector<char>> argv_storage;
                 std::vector<char *>            argv_ptrs;
                 argv_storage.reserve(argv.size());
                 argv_ptrs.reserve(argv.size() + 1);
                 for (const auto &s : argv) {
                     argv_storage.emplace_back(s.begin(), s.end());
                     argv_storage.back().push_back('\0');
                     argv_ptrs.push_back(argv_storage.back().data());
                 }
                 argv_ptrs.push_back(nullptr);
                 return std::make_unique<pp::standalone::Standalone>(
                     static_cast<int>(argv.size()),
                     const_cast<const char **>(argv_ptrs.data()));
             }),
             py::arg("argv"))
        .def("run",
             [](pp::standalone::Standalone &self) {
                 py::gil_scoped_release release;
                 self.run();
             });
}
