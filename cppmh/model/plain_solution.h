/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef CPPMH_MODEL_PLAIN_SOLUTION_H__
#define CPPMH_MODEL_PLAIN_SOLUTION_H__

#include <vector>

namespace cppmh {
namespace model {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Model;

/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct PlainSolution {
    friend class Model<T_Variable, T_Expression>;

    T_Expression            objective;
    bool                    is_feasible;
    std::vector<T_Variable> variables;

    /*************************************************************************/
    PlainSolution(void) : objective(0), is_feasible(false) {
        this->initialize();
    }

    /*************************************************************************/
    virtual ~PlainSolution(void) {
        /// nothing to do
    }

    /*************************************************************************/
    inline void initialize(void) {
        this->variables.clear();

        this->objective   = 0;
        this->is_feasible = false;
    }

    /*************************************************************************/
    inline void write(std::ofstream* a_ofs, const int a_INDENT_LEVEL) const {
        int indent_level = a_INDENT_LEVEL;
        *a_ofs << utility::indent_spaces(indent_level) << "{" << std::endl;
        indent_level++;
        *a_ofs << utility::indent_spaces(indent_level) << "\"is_feasible\" : "
               << (this->is_feasible ? "true," : "false,") << std::endl;
        *a_ofs << utility::indent_spaces(indent_level)
               << "\"objective\" : " << this->objective << "," << std::endl;
        *a_ofs << utility::indent_spaces(indent_level) << "\"variables\" : ";
        *a_ofs << "[";
        auto& variables      = this->variables;
        int   variables_size = variables.size();
        for (auto i = 0; i < variables_size; i++) {
            if (i != variables_size - 1) {
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
}  // namespace cppmh
#endif
/*****************************************************************************/
// END
/*****************************************************************************/