/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLUTION_SOLUTION_ARCHIVE_H__
#define PRINTEMPS_SOLUTION_SOLUTION_ARCHIVE_H__

namespace printemps::solution {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class SolutionArchive {
   private:
    int  m_max_size;
    bool m_is_ascending;
    std::vector<SparseSolution<T_Variable, T_Expression>>  //
        m_solutions;

    std::string m_name;
    int         m_number_of_variables;
    int         m_number_of_constraints;

   public:
    /*************************************************************************/
    SolutionArchive(void) {
        this->initialize();
    }

    /*************************************************************************/
    SolutionArchive(const int          a_MAX_SIZE,             //
                    const bool         a_is_ASCENDING,         //
                    const std::string& a_NAME,                 //
                    const int          a_NUMBER_OF_VARIABLES,  //
                    const int          a_NUMBER_OF_CONSTRAINTS) {
        this->setup(a_MAX_SIZE,             //
                    a_is_ASCENDING,         //
                    a_NAME,                 //
                    a_NUMBER_OF_VARIABLES,  //
                    a_NUMBER_OF_CONSTRAINTS);
    }

    /*************************************************************************/
    void initialize(void) {
        m_max_size     = 0;
        m_is_ascending = true;
        m_solutions.clear();
        m_name                  = "";
        m_number_of_variables   = 0;
        m_number_of_constraints = 0;
    }

    /*************************************************************************/
    void setup(const int          a_MAX_SIZE,             //
               const bool         a_is_ASCENDING,         //
               const std::string& a_NAME,                 //
               const int          a_NUMBER_OF_VARIABLES,  //
               const int          a_NUMBER_OF_CONSTRAINTS) {
        m_max_size     = a_MAX_SIZE;
        m_is_ascending = a_is_ASCENDING;
        m_solutions.clear();
        m_name                  = a_NAME;
        m_number_of_variables   = a_NUMBER_OF_VARIABLES;
        m_number_of_constraints = a_NUMBER_OF_CONSTRAINTS;
    }

    /*************************************************************************/
    inline constexpr void push(
        const SparseSolution<T_Variable, T_Expression>& a_SOLUTION) {
        std::vector<SparseSolution<T_Variable, T_Expression>> solutions = {
            a_SOLUTION};
        this->push(solutions);
    }

    /*************************************************************************/
    constexpr void push(
        const std::vector<SparseSolution<T_Variable, T_Expression>>&
            a_SOLUTIONS) {
        auto& solutions = m_solutions;
        solutions.insert(solutions.end(), a_SOLUTIONS.begin(),
                         a_SOLUTIONS.end());

        if (m_is_ascending) {
            std::sort(solutions.begin(), solutions.end(),
                      [](const auto& a_LHS, const auto& a_RHS) {
                          return a_LHS.objective < a_RHS.objective;
                      });
        } else {
            std::sort(solutions.begin(), solutions.end(),
                      [](const auto& a_LHS, const auto& a_RHS) {
                          return a_LHS.objective > a_RHS.objective;
                      });
        }

        solutions.erase(std::unique(solutions.begin(), solutions.end(),
                                    [](const auto& a_LHS, const auto& a_RHS) {
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
    inline constexpr const std::string& name(void) const {
        return m_name;
    }

    /*************************************************************************/
    inline constexpr int number_of_variables(void) const {
        return m_number_of_variables;
    }

    /*************************************************************************/
    inline constexpr int number_of_constraints(void) const {
        return m_number_of_constraints;
    }

    /*************************************************************************/
    inline constexpr const std::vector<
        SparseSolution<T_Variable, T_Expression>>&
    solutions(void) const {
        return m_solutions;
    }

    /*************************************************************************/
    void write_solutions_json(const std::string& a_FILE_NAME) const {
        utility::json::JsonObject object;

        // Summary
        object.emplace_back("version", constant::VERSION);
        object.emplace_back("name", m_name);
        object.emplace_back("number_of_variables", m_number_of_variables);
        object.emplace_back("number_of_constraints", m_number_of_constraints);

        /// Solutions
        utility::json::JsonArray solution_array;

        for (const auto& solution : m_solutions) {
            utility::json::JsonObject solution_object;
            solution_object.emplace_back("is_feasible", solution.is_feasible);
            solution_object.emplace_back("objective", solution.objective);
            solution_object.emplace_back("total_violation",
                                         solution.total_violation);

            utility::json::JsonObject variable_object;

            for (const auto& variable : solution.variables) {
                variable_object.emplace_back(variable.first, variable.second);
            }
            solution_object.emplace_back("variables", variable_object);
            solution_array.emplace_back(solution_object);
        }
        object.emplace_back("solutions", solution_array);
        utility::json::write_json_object(object, a_FILE_NAME);
    }
};
}  // namespace printemps::solution

#endif
/*****************************************************************************/
// END
/*****************************************************************************/