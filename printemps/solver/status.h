/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_STATUS_H__
#define PRINTEMPS_SOLVER_STATUS_H__

namespace printemps::solver {
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
    long        number_of_tabu_search_iterations;
    int         number_of_tabu_search_loops;
    long        number_of_evaluated_moves;
    double      averaged_inner_iteration_speed;
    double      averaged_move_evaluation_speed;

    std::unordered_map<std::string, multi_array::ValueProxy<double>>
        penalty_coefficients;

    std::unordered_map<std::string, multi_array::ValueProxy<int>> update_counts;
    std::unordered_map<std::string, multi_array::ValueProxy<int>>
        violation_counts;

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
        const long         a_NUMBER_OF_TABU_SEARCH_ITERATIONS,    //
        const int          a_NUMBER_OF_TABU_SEARCH_LOOPS,         //
        const long         a_NUMBER_OF_EVALUATED_MOVES,           //
        const double       a_AVERAGED_INNER_ITERATION_SPEED,      //
        const double       a_AVERAGED_MOVE_EVALUATION_SPEED,      //
        const std::unordered_map<std::string, multi_array::ValueProxy<double>>&
            a_PENALTY_COEFFICIENTS,  //
        const std::unordered_map<std::string, multi_array::ValueProxy<int>>&
            a_UPDATE_COUNTS,
        const std::unordered_map<std::string, multi_array::ValueProxy<int>>&
            a_VIOLATION_COUNTS)
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
          number_of_evaluated_moves(a_NUMBER_OF_EVALUATED_MOVES),
          averaged_inner_iteration_speed(a_AVERAGED_INNER_ITERATION_SPEED),
          averaged_move_evaluation_speed(a_AVERAGED_MOVE_EVALUATION_SPEED),
          penalty_coefficients(a_PENALTY_COEFFICIENTS),
          update_counts(a_UPDATE_COUNTS),
          violation_counts(a_VIOLATION_COUNTS) {
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
        this->number_of_evaluated_moves          = 0;
        this->averaged_inner_iteration_speed     = 0.0;
        this->averaged_move_evaluation_speed     = 0.0;
        this->penalty_coefficients.clear();
        this->update_counts.clear();
        this->violation_counts.clear();
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
    inline void print_violation_counts(void) const {
        multi_array::print_values(this->violation_counts, "violation_counts");
    }

    /*************************************************************************/
    void write_json_by_name(const std::string& a_FILE_NAME) const {
        utility::json::JsonObject object;

        // Summary
        object.emplace_back("version", constant::VERSION);
        object.emplace_back("name", this->name);
        object.emplace_back("number_of_variables", this->number_of_variables);
        object.emplace_back("number_of_constraints",
                            this->number_of_constraints);
        object.emplace_back("is_found_feasible_solution",
                            this->is_found_feasible_solution);

        object.emplace_back("start_date_time", this->start_date_time);
        object.emplace_back("finish_date_time", this->finish_date_time);
        object.emplace_back("elapsed_time", this->elapsed_time);

        object.emplace_back("number_of_lagrange_dual_iterations",
                            this->number_of_lagrange_dual_iterations);
        object.emplace_back("number_of_local_search_iterations",
                            this->number_of_local_search_iterations);
        object.emplace_back("number_of_tabu_search_iterations",
                            this->number_of_tabu_search_iterations);
        object.emplace_back("number_of_tabu_search_loops",
                            this->number_of_tabu_search_loops);
        object.emplace_back("number_of_evaluated_moves",
                            this->number_of_evaluated_moves);
        object.emplace_back("averaged_inner_iteration_speed",
                            this->averaged_inner_iteration_speed);
        object.emplace_back("averaged_move_evaluation_speed",
                            this->averaged_move_evaluation_speed);

        /// Penalty coefficients
        object.emplace_back(         //
            "penalty_coefficients",  //
            multi_array::create_json_object(penalty_coefficients));

        /// Update counts
        object.emplace_back(  //
            "update_counts",  //
            multi_array::create_json_object(update_counts));

        /// Violation counts
        object.emplace_back(     //
            "violation_counts",  //
            multi_array::create_json_object(violation_counts));

        utility::json::write_json_object(object, a_FILE_NAME);
    }

    /*************************************************************************/
    void write_json_by_array(const std::string& a_FILE_NAME) const {
        utility::json::JsonObject object;

        /// Summary
        object.emplace_back("version", constant::VERSION);
        object.emplace_back("name", this->name);
        object.emplace_back("number_of_variables", this->number_of_variables);
        object.emplace_back("number_of_constraints",
                            this->number_of_constraints);
        object.emplace_back("is_found_feasible_solution",
                            this->is_found_feasible_solution);

        object.emplace_back("start_date_time", this->start_date_time);
        object.emplace_back("finish_date_time", this->finish_date_time);
        object.emplace_back("elapsed_time", this->elapsed_time);

        object.emplace_back("number_of_lagrange_dual_iterations",
                            this->number_of_lagrange_dual_iterations);
        object.emplace_back("number_of_local_search_iterations",
                            this->number_of_local_search_iterations);
        object.emplace_back("number_of_tabu_search_iterations",
                            this->number_of_tabu_search_iterations);
        object.emplace_back("number_of_tabu_search_loops",
                            this->number_of_tabu_search_loops);
        object.emplace_back("number_of_evaluated_moves",
                            this->number_of_evaluated_moves);
        object.emplace_back("averaged_inner_iteration_speed",
                            this->averaged_inner_iteration_speed);
        object.emplace_back("averaged_move_evaluation_speed",
                            this->averaged_move_evaluation_speed);

        /// Penalty coefficients
        object.emplace_back(         //
            "penalty_coefficients",  //
            multi_array::create_json_array(penalty_coefficients));

        /// Update counts
        object.emplace_back(  //
            "update_counts",  //
            multi_array::create_json_array(update_counts));

        /// Violation counts
        object.emplace_back(     //
            "violation_counts",  //
            multi_array::create_json_array(violation_counts));

        utility::json::write_json_object(object, a_FILE_NAME);
    }
};
}  // namespace printemps::solver

#endif
/*****************************************************************************/
// END
/*****************************************************************************/