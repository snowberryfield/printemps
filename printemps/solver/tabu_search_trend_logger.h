/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
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
        m_ofstream  //
            << "#iteration "
            << "elapsed_time "
            << "local_objective "
            << "local_violation "
            << "global_objective "
            << "global_violation "
            << "intensity "
            << "update_status "
            << "employing_local_augmented_solution_flag "
            << "employing_global_augmented_solution_flag "
            << "employing_previous_solution_flag "
            << "is_enabled_penalty_coefficient_relaxing "
            << "is_enabled_penalty_coefficient_tightening "
            << "is_enabled_forcibly_initial_modification "
            << "penalty_coefficient_relaxing_rate "
            << "penalty_coefficient_tightening_rate "
            << "number_of_initial_modification "
            << "initial_tabu_tenure" << std::endl;
    }

    /*************************************************************************/
    void write_log(const int    a_ITERATION,         //
                   const double a_ELAPSED_TIME,      //
                   const double a_LOCAL_OBJECTIVE,   //
                   const double a_LOCAL_VIOLATION,   //
                   const double a_GLOBAL_OBJECTIVE,  //
                   const double a_GLOBAL_VIOLATION,  //
                   const double a_INTENSITY,         //
                   const int    a_UPDATE_STATUS,
                   const bool   a_EMPLOYING_LOCAL_AUGMENTED_SOLUTION_FLAG,
                   const bool   a_EMPLOYING_GLOBAL_AUGMENTED_SOLUTION_FLAG,
                   const bool   a_EMPLOYING_PREVIOUS_SOLUTION_FLAG,
                   const bool   a_IS_ENABLED_PENALTY_COEFFICIENT_RELAXING,
                   const bool   a_IS_ENABLED_PENALTY_COEFFICIENT_TIGHTING,
                   const bool   a_IS_ENABLED_FORCIBLY_INITIAL_MODIFICATION,
                   const double a_PENALTY_COEFFICIENT_RELAXING_RATE,
                   const double a_PENALTY_COEFFICIENT_TIGHTENING_RATE,
                   const int    a_NUMBER_OF_INITIAL_MODIFICATION,
                   const int    a_INITIAL_TABU_TENURE) {
        if (!m_ofstream.is_open()) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "This log file " + m_file_name + " is not opened."));
        } else {
            m_ofstream                                                //
                << a_ITERATION << " "                                 //
                << a_ELAPSED_TIME << " "                              //
                << a_LOCAL_OBJECTIVE << " "                           //
                << a_LOCAL_VIOLATION << " "                           //
                << a_GLOBAL_OBJECTIVE << " "                          //
                << a_GLOBAL_VIOLATION << " "                          //
                << a_INTENSITY << " "                                 //
                << a_UPDATE_STATUS << " "                             //
                << a_EMPLOYING_LOCAL_AUGMENTED_SOLUTION_FLAG << " "   //
                << a_EMPLOYING_GLOBAL_AUGMENTED_SOLUTION_FLAG << " "  //
                << a_EMPLOYING_PREVIOUS_SOLUTION_FLAG << " "          //
                << a_IS_ENABLED_PENALTY_COEFFICIENT_RELAXING << " "   //
                << a_IS_ENABLED_PENALTY_COEFFICIENT_TIGHTING << " "   //
                << a_IS_ENABLED_FORCIBLY_INITIAL_MODIFICATION << " "  //
                << a_PENALTY_COEFFICIENT_RELAXING_RATE << " "         //
                << a_PENALTY_COEFFICIENT_TIGHTENING_RATE << " "       //
                << a_NUMBER_OF_INITIAL_MODIFICATION << " "            //
                << a_INITIAL_TABU_TENURE << std::endl;
        }
    }
};
}  // namespace solver
}  // namespace printemps

#endif
/*****************************************************************************/
// END
/*****************************************************************************/