/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MODEL_PLAIN_SOLUTION_H__
#define PRINTEMPS_MODEL_PLAIN_SOLUTION_H__

#include <vector>

namespace printemps {
namespace model {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Model;

/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct PlainSolution {
    friend class Model<T_Variable, T_Expression>;

    T_Expression            objective;
    T_Expression            total_violation;
    bool                    is_feasible;
    std::vector<T_Variable> variables;

    /*************************************************************************/
    PlainSolution(void) : objective(0), total_violation(0), is_feasible(false) {
        this->initialize();
    }

    /*************************************************************************/
    virtual ~PlainSolution(void) {
        /// nothing to do
    }

    /*************************************************************************/
    void initialize(void) {
        this->variables.clear();

        this->objective       = 0;
        this->total_violation = 0;
        this->is_feasible     = false;
    }

    /*************************************************************************/
    void write(std::ofstream* a_ofs, const int a_INDENT_LEVEL) const {
        int indent_level = a_INDENT_LEVEL;
        *a_ofs << utility::indent_spaces(indent_level) << "{" << std::endl;
        indent_level++;
        *a_ofs << utility::indent_spaces(indent_level) << "\"is_feasible\" : "
               << (this->is_feasible ? "true," : "false,") << std::endl;
        *a_ofs << utility::indent_spaces(indent_level)
               << "\"objective\" : " << this->objective << "," << std::endl;
        *a_ofs << utility::indent_spaces(indent_level)
               << "\"total_violation\" : " << this->total_violation << ","
               << std::endl;
        *a_ofs << utility::indent_spaces(indent_level) << "\"variables\" : ";
        *a_ofs << "[";
        const auto& variables      = this->variables;
        const int   VARIABLES_SIZE = variables.size();
        for (auto i = 0; i < VARIABLES_SIZE; i++) {
            if (i != VARIABLES_SIZE - 1) {
                *a_ofs << variables[i] << ", ";
            } else {
                *a_ofs << variables[i] << "]" << std::endl;
            }
        }
        indent_level--;
        *a_ofs << utility::indent_spaces(indent_level) << "}";
    }
};
using IPPlainSolution = PlainSolution<int, double>;
}  // namespace model
}  // namespace printemps
#endif
/*****************************************************************************/
// END
/*****************************************************************************/