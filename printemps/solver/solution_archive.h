/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_SOLUTION_ARCHIVE_H__
#define PRINTEMPS_SOLVER_SOLUTION_ARCHIVE_H__

namespace printemps {
namespace solver {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class SolutionArchive {
   private:
    int  m_max_size;
    bool m_is_ascending;
    std::vector<model::PlainSolution<T_Variable, T_Expression>>  //
        m_solutions;

   public:
    /*************************************************************************/
    SolutionArchive(void) {
        this->initialize();
    }

    /*************************************************************************/
    virtual ~SolutionArchive(void) {
        /// nothing to do
    }

    /*************************************************************************/
    SolutionArchive(const int a_MAX_SIZE, const bool a_is_ASCENDING) {
        this->setup(a_MAX_SIZE, a_is_ASCENDING);
    }

    /*************************************************************************/
    void initialize(void) {
        m_max_size     = 0;
        m_is_ascending = true;
        m_solutions.clear();
    }

    /*************************************************************************/
    void setup(const int a_MAX_SIZE, const bool a_is_ASCENDING) {
        m_max_size     = a_MAX_SIZE;
        m_is_ascending = a_is_ASCENDING;
        m_solutions.clear();
    }

    /*************************************************************************/
    inline constexpr void push(
        const model::PlainSolution<T_Variable, T_Expression>& a_SOLUTION) {
        std::vector<model::PlainSolution<T_Variable, T_Expression>> solutions =
            {a_SOLUTION};
        this->push(solutions);
    }

    /*************************************************************************/
    constexpr void push(
        const std::vector<model::PlainSolution<T_Variable, T_Expression>>&
            a_SOLUTIONS) {
        auto& solutions = m_solutions;
        solutions.insert(solutions.end(), a_SOLUTIONS.begin(),
                         a_SOLUTIONS.end());

        if (m_is_ascending) {
            std::sort(solutions.begin(), solutions.end(),
                      [](auto const& a_LHS, auto const& a_RHS) {
                          return a_LHS.objective < a_RHS.objective;
                      });
        } else {
            std::sort(solutions.begin(), solutions.end(),
                      [](auto const& a_LHS, auto const& a_RHS) {
                          return a_LHS.objective > a_RHS.objective;
                      });
        }

        solutions.erase(std::unique(solutions.begin(), solutions.end(),
                                    [](auto const& a_LHS, auto const& a_RHS) {
                                        return a_LHS.variables ==
                                               a_RHS.variables;
                                    }),
                        solutions.end());

        if (static_cast<int>(solutions.size()) > m_max_size) {
            solutions.resize(m_max_size);
        }
    }

    /*************************************************************************/
    inline constexpr int size(void) const {
        return m_solutions.size();
    }

    /*************************************************************************/
    inline constexpr int max_size(void) const {
        return m_max_size;
    }

    /*************************************************************************/
    inline constexpr bool is_ascending(void) const {
        return m_is_ascending;
    }

    /*************************************************************************/
    inline constexpr const std::vector<
        model::PlainSolution<T_Variable, T_Expression>>&
    solutions(void) const {
        return m_solutions;
    }

    /*************************************************************************/
    void write_solutions_json(
        const std::string&         a_FILE_NAME,
        const model::ModelSummary& a_MODEL_SUMMARY) const {
        int indent_level = 0;

        std::ofstream ofs(a_FILE_NAME.c_str());
        ofs << utility::indent_spaces(indent_level) << "{" << std::endl;
        indent_level++;

        /// Summary
        ofs << utility::indent_spaces(indent_level) << "\"name\" : "
            << "\"" << a_MODEL_SUMMARY.name << "\"," << std::endl;

        ofs << utility::indent_spaces(indent_level)
            << "\"number_of_variables\" : "
            << a_MODEL_SUMMARY.number_of_variables << "," << std::endl;

        ofs << utility::indent_spaces(indent_level)
            << "\"number_of_constraints\" : "
            << a_MODEL_SUMMARY.number_of_constraints << "," << std::endl;

        /// Solutions
        ofs << utility::indent_spaces(indent_level) << "\"solutions\": ["
            << std::endl;

        indent_level++;
        auto& solutions      = this->m_solutions;
        int   solutions_size = solutions.size();
        for (auto i = 0; i < solutions_size; i++) {
            solutions[i].write(&ofs, indent_level);
            if (i != solutions_size - 1) {
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
}  // namespace printemps

#endif
/*****************************************************************************/
// END
/*****************************************************************************/