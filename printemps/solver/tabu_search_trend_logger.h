/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_TABU_SEARCH_TREND_LOGGER_H__
#define PRINTEMPS_SOLVER_TABU_SEARCH_TREND_LOGGER_H__

namespace printemps {
namespace solver {
/*****************************************************************************/
class TabuSearchTrendLogger {
   private:
    std::string   m_file_name;
    std::ofstream m_ofstream;

    /*************************************************************************/
    TabuSearchTrendLogger(const TabuSearchTrendLogger &);
    TabuSearchTrendLogger &operator=(const TabuSearchTrendLogger &);

   public:
    /*************************************************************************/
    TabuSearchTrendLogger(void) {
        this->initialize();
    }
    /*************************************************************************/
    TabuSearchTrendLogger(const std::string &a_FILE_NAME) {
        this->setup(a_FILE_NAME);
    }

    /*************************************************************************/
    void initialize(void) {
        if (m_ofstream.is_open()) {
            m_ofstream.close();
        }
    }

    /*************************************************************************/
    void setup(const std::string &a_FILE_NAME) {
        this->initialize();
        m_file_name = a_FILE_NAME;
        m_ofstream.open(a_FILE_NAME);
    }

    /*************************************************************************/
    void write_instance_info(const std::string &a_NAME,
                             const int          a_NUMBER_OF_VARIABLES,
                             const int          a_NUMBER_OF_CONTSTRAINTS) {
        m_ofstream << "#instance_name: " << a_NAME << std::endl;
        m_ofstream << "#number_of_variables: " << a_NUMBER_OF_VARIABLES
                   << std::endl;
        m_ofstream << "#number_of_Constraints: " << a_NUMBER_OF_CONTSTRAINTS
                   << std::endl;
    }

    /*************************************************************************/
    void write_header(void) {
        m_ofstream                                           //
            << "#iteration "                                 // 0
            << "elapsed_time "                               // 1
            << "local_objective "                            // 2
            << "local_violation "                            // 3
            << "global_objective "                           // 4
            << "global_violation "                           // 5
            << "intensity "                                  // 6
            << "update_status "                              // 7
            << "employing_local_augmented_solution_flag "    // 8
            << "employing_global_augmented_solution_flag "   // 9
            << "employing_previous_solution_flag "           // 10
            << "is_enabled_penalty_coefficient_relaxing "    // 11
            << "is_enabled_penalty_coefficient_tightening "  // 12
            << "penalty_coefficient_reset_flag"              // 13
            << "penalty_coefficient_relaxing_rate "          // 14
            << "penalty_coefficient_tightening_rate "        // 15
            << "is_enabled_forcibly_initial_modification "   // 16
            << "number_of_initial_modification "             // 17
            << "initial_tabu_tenure" << std::endl;           // 18
    }

    /*************************************************************************/
    void write_log(const int    a_ITERATION,                                // 0
                   const double a_ELAPSED_TIME,                             // 1
                   const double a_LOCAL_OBJECTIVE,                          // 2
                   const double a_LOCAL_VIOLATION,                          // 3
                   const double a_GLOBAL_OBJECTIVE,                         // 4
                   const double a_GLOBAL_VIOLATION,                         // 5
                   const double a_INTENSITY,                                // 6
                   const int    a_UPDATE_STATUS,                            // 7
                   const bool   a_EMPLOYING_LOCAL_AUGMENTED_SOLUTION_FLAG,  // 8
                   const bool a_EMPLOYING_GLOBAL_AUGMENTED_SOLUTION_FLAG,   // 9
                   const bool a_EMPLOYING_PREVIOUS_SOLUTION_FLAG,          // 10
                   const bool a_IS_ENABLED_PENALTY_COEFFICIENT_RELAXING,   // 11
                   const bool a_IS_ENABLED_PENALTY_COEFFICIENT_TIGHTING,   // 12
                   const bool a_PENALTY_COEFFICIENT_RESET_FLAG,            // 13
                   const double a_PENALTY_COEFFICIENT_RELAXING_RATE,       // 14
                   const double a_PENALTY_COEFFICIENT_TIGHTENING_RATE,     // 15
                   const bool a_IS_ENABLED_FORCIBLY_INITIAL_MODIFICATION,  // 16
                   const int  a_NUMBER_OF_INITIAL_MODIFICATION,            // 17
                   const int  a_INITIAL_TABU_TENURE                        // 18
    ) {
        if (!m_ofstream.is_open()) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "This log file " + m_file_name + " is not opened."));
        } else {
            m_ofstream                                                //
                << a_ITERATION << " "                                 // 0
                << a_ELAPSED_TIME << " "                              // 1
                << a_LOCAL_OBJECTIVE << " "                           // 2
                << a_LOCAL_VIOLATION << " "                           // 3
                << a_GLOBAL_OBJECTIVE << " "                          // 4
                << a_GLOBAL_VIOLATION << " "                          // 5
                << a_INTENSITY << " "                                 // 6
                << a_UPDATE_STATUS << " "                             // 7
                << a_EMPLOYING_LOCAL_AUGMENTED_SOLUTION_FLAG << " "   // 8
                << a_EMPLOYING_GLOBAL_AUGMENTED_SOLUTION_FLAG << " "  // 9
                << a_EMPLOYING_PREVIOUS_SOLUTION_FLAG << " "          // 10
                << a_IS_ENABLED_PENALTY_COEFFICIENT_RELAXING << " "   // 11
                << a_IS_ENABLED_PENALTY_COEFFICIENT_TIGHTING << " "   // 12
                << a_PENALTY_COEFFICIENT_RESET_FLAG << " "            // 13
                << a_PENALTY_COEFFICIENT_RELAXING_RATE << " "         // 14
                << a_PENALTY_COEFFICIENT_TIGHTENING_RATE << " "       // 15
                << a_IS_ENABLED_FORCIBLY_INITIAL_MODIFICATION << " "  // 16
                << a_NUMBER_OF_INITIAL_MODIFICATION << " "            // 17
                << a_INITIAL_TABU_TENURE                              // 18
                << std::endl;
        }
    }
};
}  // namespace solver
}  // namespace printemps

#endif
/*****************************************************************************/
// END
/*****************************************************************************/