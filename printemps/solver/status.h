/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_STATUS_H__
#define PRINTEMPS_SOLVER_STATUS_H__

namespace printemps {
namespace solver {
/*****************************************************************************/
struct Status {
    bool is_found_feasible_solution;

    std::string start_date_time;
    std::string finish_date_time;

    std::string name;
    int         number_of_variables;
    int         number_of_constraints;
    double      elapsed_time;
    int         number_of_lagrange_dual_iterations;
    int         number_of_local_search_iterations;
    int         number_of_tabu_search_iterations;
    int         number_of_tabu_search_loops;

    std::unordered_map<std::string, multi_array::ValueProxy<double>>
        penalty_coefficients;

    std::unordered_map<std::string, multi_array::ValueProxy<int>> update_counts;

    /*************************************************************************/
    Status(
        const bool         a_IS_FOUND_FEASIBLE_SOLUTION,          //
        const std::string& a_START_DATE_TIME,                     //
        const std::string& a_FINISH_DATE_TIME,                    //
        const std::string& a_NAME,                                //
        const int          a_NUMBER_OF_VARIABLES,                 //
        const int          a_NUMBER_OF_CONSTRAINTS,               //
        const double       a_ELAPSED_TIME,                        //
        const int          a_NUMBER_OF_LAGRANGE_DUAL_ITERATIONS,  //
        const int          a_NUMBER_OF_LOCAL_SEARCH_ITERATIONS,   //
        const int          a_NUMBER_OF_TABU_SEARCH_ITERATIONS,    //
        const int          a_NUMBER_OF_TABU_SEARCH_LOOPS,         //
        const std::unordered_map<std::string, multi_array::ValueProxy<double>>&
            a_PENALTY_COEFFICIENTS,  //
        const std::unordered_map<std::string, multi_array::ValueProxy<int>>&
            a_UPDATE_COUNTS)
        : is_found_feasible_solution(a_IS_FOUND_FEASIBLE_SOLUTION),
          start_date_time(a_START_DATE_TIME),
          finish_date_time(a_FINISH_DATE_TIME),
          name(a_NAME),
          number_of_variables(a_NUMBER_OF_VARIABLES),
          number_of_constraints(a_NUMBER_OF_CONSTRAINTS),
          elapsed_time(a_ELAPSED_TIME),
          number_of_lagrange_dual_iterations(
              a_NUMBER_OF_LAGRANGE_DUAL_ITERATIONS),
          number_of_local_search_iterations(
              a_NUMBER_OF_LOCAL_SEARCH_ITERATIONS),
          number_of_tabu_search_iterations(a_NUMBER_OF_TABU_SEARCH_ITERATIONS),
          number_of_tabu_search_loops(a_NUMBER_OF_TABU_SEARCH_LOOPS),
          penalty_coefficients(a_PENALTY_COEFFICIENTS),
          update_counts(a_UPDATE_COUNTS) {
        /// nothing to do
    }

    /*************************************************************************/
    Status(void) {
        this->initialize();
    }

    /*************************************************************************/
    void initialize(void) {
        this->is_found_feasible_solution = false;
        this->start_date_time.clear();
        this->finish_date_time.clear();
        this->name                               = "";
        this->number_of_variables                = 0;
        this->number_of_constraints              = 0;
        this->elapsed_time                       = 0.0;
        this->number_of_local_search_iterations  = 0;
        this->number_of_lagrange_dual_iterations = 0;
        this->number_of_tabu_search_iterations   = 0;
        this->number_of_tabu_search_loops        = 0;
        this->penalty_coefficients.clear();
        this->update_counts.clear();
    }

    /*************************************************************************/
    inline void print_penalty_coefficients(void) const {
        multi_array::print_values(this->penalty_coefficients,
                                  "penalty_coefficients");
    }

    /*************************************************************************/
    inline void print_update_counts(void) const {
        multi_array::print_values(this->update_counts, "update_counts");
    }

    /*************************************************************************/
    void write_json_by_name(const std::string& a_FILE_NAME) const {
        int indent_level = 0;

        std::ofstream ofs(a_FILE_NAME.c_str());
        ofs << utility::indent_spaces(indent_level) << "{" << std::endl;
        indent_level++;

        /// Summary
        ofs << utility::indent_spaces(indent_level) << "\"version\" : "
            << "\"" << constant::VERSION << "\"," << std::endl;

        ofs << utility::indent_spaces(indent_level) << "\"name\" : "
            << "\"" << this->name << "\"," << std::endl;

        ofs << utility::indent_spaces(indent_level)
            << "\"number_of_variables\" : " << this->number_of_variables << ","
            << std::endl;

        ofs << utility::indent_spaces(indent_level)
            << "\"number_of_constraints\" : " << this->number_of_constraints
            << "," << std::endl;

        ofs << utility::indent_spaces(indent_level)
            << "\"is_found_feasible_solution\" : "
            << (this->is_found_feasible_solution ? "true," : "false,")
            << std::endl;

        ofs << utility::indent_spaces(indent_level)
            << "\"start_date_time\" : \"" + this->start_date_time + "\""
            << "," << std::endl;

        ofs << utility::indent_spaces(indent_level)
            << "\"finish_date_time\" : \"" + this->finish_date_time + "\""
            << "," << std::endl;

        ofs << utility::indent_spaces(indent_level)
            << "\"elapsed_time\" : " + std::to_string(this->elapsed_time) << ","
            << std::endl;

        ofs << utility::indent_spaces(indent_level)
            << "\"number_of_lagrange_dual_iterations\" : " +
                   std::to_string(this->number_of_lagrange_dual_iterations)
            << "," << std::endl;

        ofs << utility::indent_spaces(indent_level)
            << "\"number_of_local_search_iterations\" : " +
                   std::to_string(this->number_of_local_search_iterations)
            << "," << std::endl;

        ofs << utility::indent_spaces(indent_level)
            << "\"number_of_tabu_search_iterations\" : " +
                   std::to_string(this->number_of_tabu_search_iterations)
            << "," << std::endl;

        ofs << utility::indent_spaces(indent_level)
            << "\"number_of_tabu_search_loops\" : " +
                   std::to_string(this->number_of_tabu_search_loops)
            << "," << std::endl;

        /// Penalty coefficients
        multi_array::write_values_by_name(&ofs,                        //
                                          this->penalty_coefficients,  //
                                          "penalty_coefficients",      //
                                          indent_level,                //
                                          "%.10e", true);

        /// Update counts
        multi_array::write_values_by_name(&ofs,                 //
                                          this->update_counts,  //
                                          "update_counts",      //
                                          indent_level,         //
                                          "%d", false);

        indent_level--;
        ofs << utility::indent_spaces(indent_level) << "}" << std::endl;
        ofs.close();
    }

    /*************************************************************************/
    void write_json_by_array(const std::string& a_FILE_NAME) const {
        int indent_level = 0;

        std::ofstream ofs(a_FILE_NAME.c_str());
        ofs << utility::indent_spaces(indent_level) << "{" << std::endl;
        indent_level++;

        /// Summary
        ofs << utility::indent_spaces(indent_level) << "\"version\" : "
            << "\"" << constant::VERSION << "\"," << std::endl;

        ofs << utility::indent_spaces(indent_level) << "\"name\" : "
            << "\"" << this->name << "\"," << std::endl;

        ofs << utility::indent_spaces(indent_level)
            << "\"number_of_variables\" : " << this->number_of_variables << ","
            << std::endl;

        ofs << utility::indent_spaces(indent_level)
            << "\"number_of_constraints\" : " << this->number_of_constraints
            << "," << std::endl;

        ofs << utility::indent_spaces(indent_level)
            << "\"is_found_feasible_solution\" : "
            << (this->is_found_feasible_solution ? "true," : "false,")
            << std::endl;

        ofs << utility::indent_spaces(indent_level)
            << "\"start_date_time\" : \"" + this->start_date_time + "\""
            << "," << std::endl;

        ofs << utility::indent_spaces(indent_level)
            << "\"finish_date_time\" : \"" + this->finish_date_time + "\""
            << "," << std::endl;

        ofs << utility::indent_spaces(indent_level)
            << "\"elapsed_time\" : " + std::to_string(this->elapsed_time) << ","
            << std::endl;

        ofs << utility::indent_spaces(indent_level)
            << "\"number_of_lagrange_dual_iterations\" : " +
                   std::to_string(this->number_of_lagrange_dual_iterations)
            << "," << std::endl;

        ofs << utility::indent_spaces(indent_level)
            << "\"number_of_local_search_iterations\" : " +
                   std::to_string(this->number_of_local_search_iterations)
            << "," << std::endl;

        ofs << utility::indent_spaces(indent_level)
            << "\"number_of_tabu_search_iterations\" : " +
                   std::to_string(this->number_of_tabu_search_iterations)
            << "," << std::endl;

        ofs << utility::indent_spaces(indent_level)
            << "\"number_of_tabu_search_loops\" : " +
                   std::to_string(this->number_of_tabu_search_loops)
            << "," << std::endl;

        /// Penalty coefficients
        multi_array::write_values_by_array(&ofs,                        //
                                           this->penalty_coefficients,  //
                                           "penalty_coefficients",      //
                                           indent_level,                //
                                           "%.10e",                     //
                                           true);

        /// Update counts
        multi_array::write_values_by_array(&ofs,                 //
                                           this->update_counts,  //
                                           "update_counts",      //
                                           indent_level,         //
                                           "%d",                 //
                                           false);

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