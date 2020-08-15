/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef CPPMH_SOLVER_HISTORY_H__
#define CPPMH_SOLVER_HISTORY_H__

namespace cppmh {
namespace solver {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct History {
    model::ModelSummary model_summary;
    std::vector<model::PlainSolution<T_Variable, T_Expression>>  //
        feasible_solutions;

    /*************************************************************************/
    History(void) {
        this->initialize();
    }

    /*************************************************************************/
    virtual ~History(void) {
        /// nothing to do
    }

    /*************************************************************************/
    inline void constexpr initialize(void) {
        this->model_summary.initialize();
        this->feasible_solutions.clear();
    }

    /*************************************************************************/
    inline void write_feasible_solutions_json(
        const std::string& a_FILE_NAME) const {
        int indent_level = 0;

        std::ofstream ofs(a_FILE_NAME.c_str());
        ofs << utility::indent_spaces(indent_level) << "{" << std::endl;
        indent_level++;

        /// Summary
        ofs << utility::indent_spaces(indent_level) << "\"name\" : "
            << "\"" << model_summary.name << "\"," << std::endl;

        ofs << utility::indent_spaces(indent_level)
            << "\"number_of_variables\" : " << model_summary.number_of_variables
            << "," << std::endl;

        ofs << utility::indent_spaces(indent_level)
            << "\"number_of_constraints\" : "
            << model_summary.number_of_constraints << "," << std::endl;

        /// Solutions
        ofs << utility::indent_spaces(indent_level) << "\"solutions\": ["
            << std::endl;

        indent_level++;
        auto& feasible_solutions      = this->feasible_solutions;
        int   feasible_solutions_size = feasible_solutions.size();
        for (auto i = 0; i < feasible_solutions_size; i++) {
            feasible_solutions[i].write(&ofs, indent_level);
            if (i != feasible_solutions_size - 1) {
                ofs << "," << std::endl;
            } else {
                ofs << std::endl;
            }
        }
        indent_level--;
        ofs << utility::indent_spaces(indent_level) << "]" << std::endl;
        indent_level--;
        ofs << utility::indent_spaces(indent_level) << "}" << std::endl;
        ofs.close();
    }
};
}  // namespace solver
}  // namespace cppmh

#endif
/*****************************************************************************/
// END
/*****************************************************************************/