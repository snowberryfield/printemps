/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_STATUS_H__
#define PRINTEMPS_SOLVER_STATUS_H__

namespace printemps::solver {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Solver;

/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct Status {
    model::Model<T_Variable, T_Expression> *model_ptr;
    option::Option                          option;

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
    Status(Solver<T_Variable, T_Expression> *a_solver_ptr) {
        this->setup(a_solver_ptr);
    }

    /*************************************************************************/
    Status(void) {
        this->initialize();
    }

    /*************************************************************************/
    void initialize(void) {
        this->model_ptr = nullptr;
        this->option.initialize();

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
    void setup(Solver<T_Variable, T_Expression> *a_solver_ptr) {
        const auto &LAGRANGE_DUAL_RESULT =
            a_solver_ptr->lagrange_dual_controller().result();
        const auto &LOCAL_SEARCH_RESULT =
            a_solver_ptr->local_search_controller().result();
        const auto &TABU_SEARCH_RESULT =
            a_solver_ptr->tabu_search_controller().result();

        this->model_ptr = a_solver_ptr->model_ptr();
        this->option    = a_solver_ptr->option_original();

        this->is_found_feasible_solution = this->model_ptr->is_feasible();

        this->start_date_time  = a_solver_ptr->start_date_time();
        this->finish_date_time = a_solver_ptr->finish_date_time();

        this->name                  = this->model_ptr->name();
        this->number_of_variables   = this->model_ptr->number_of_variables();
        this->number_of_constraints = this->model_ptr->number_of_constraints();
        this->elapsed_time = a_solver_ptr->time_keeper().elapsed_time();

        this->number_of_local_search_iterations =
            LOCAL_SEARCH_RESULT.number_of_iterations;
        this->number_of_lagrange_dual_iterations =
            LAGRANGE_DUAL_RESULT.number_of_iterations;
        this->number_of_tabu_search_iterations =
            TABU_SEARCH_RESULT.state.total_number_of_inner_iterations;
        this->number_of_tabu_search_loops = TABU_SEARCH_RESULT.state.iteration;
        this->number_of_evaluated_moves =
            TABU_SEARCH_RESULT.state.total_number_of_evaluated_moves;
        this->averaged_inner_iteration_speed =
            TABU_SEARCH_RESULT.state.averaged_inner_iteration_speed;
        this->averaged_move_evaluation_speed =
            TABU_SEARCH_RESULT.state.averaged_move_evaluation_speed;

        this->penalty_coefficients =
            a_solver_ptr->export_named_penalty_coefficients();
        this->update_counts    = a_solver_ptr->export_named_update_counts();
        this->violation_counts = a_solver_ptr->export_named_violation_counts();
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
    inline void add_summary_json(utility::json::JsonObject *a_object) const {
        a_object->emplace_back("version", constant::VERSION);
        a_object->emplace_back("name", this->name);
        a_object->emplace_back("number_of_variables",
                               this->number_of_variables);
        a_object->emplace_back("number_of_constraints",
                               this->number_of_constraints);

        a_object->emplace_back("is_found_feasible_solution",
                               this->is_found_feasible_solution);

        a_object->emplace_back("start_date_time", this->start_date_time);
        a_object->emplace_back("finish_date_time", this->finish_date_time);
        a_object->emplace_back("elapsed_time", this->elapsed_time);

        a_object->emplace_back("number_of_lagrange_dual_iterations",
                               this->number_of_lagrange_dual_iterations);
        a_object->emplace_back("number_of_local_search_iterations",
                               this->number_of_local_search_iterations);
        a_object->emplace_back("number_of_tabu_search_iterations",
                               this->number_of_tabu_search_iterations);
        a_object->emplace_back("number_of_tabu_search_loops",
                               this->number_of_tabu_search_loops);
        a_object->emplace_back("number_of_evaluated_moves",
                               this->number_of_evaluated_moves);
        a_object->emplace_back("averaged_inner_iteration_speed",
                               this->averaged_inner_iteration_speed);
        a_object->emplace_back("averaged_move_evaluation_speed",
                               this->averaged_move_evaluation_speed);
    }

    /*************************************************************************/
    inline void add_option_json(utility::json::JsonObject *a_object) const {
        a_object->emplace_back("option", this->option.to_json());
    }

    /*************************************************************************/
    inline void add_variable_detail(utility::json::JsonObject *a_object) const {
        auto &REFERENCE_ORIGINAL =
            this->model_ptr->variable_reference_original();
        utility::json::JsonObject original;

        auto &REFERENCE_PRESOLVED = this->model_ptr->variable_reference();
        utility::json::JsonObject presolved;

        /// Original
        original.emplace_back(  //
            "all",              //
            REFERENCE_ORIGINAL.variable_ptrs.size());
        original.emplace_back(  //
            "fixed",            ///
            REFERENCE_ORIGINAL.fixed_variable_ptrs.size());
        original.emplace_back(  //
            "mutable",          //
            REFERENCE_ORIGINAL.mutable_variable_ptrs.size());

        /// Presolved
        presolved.emplace_back(  //
            "all",               //
            REFERENCE_PRESOLVED.variable_ptrs.size());
        presolved.emplace_back(  //
            "fixed",             ///
            REFERENCE_PRESOLVED.fixed_variable_ptrs.size());
        presolved.emplace_back(  //
            "mutable",           //
            REFERENCE_PRESOLVED.mutable_variable_ptrs.size());

        utility::json::JsonObject variable_detail;
        variable_detail.emplace_back("original", original);
        variable_detail.emplace_back("presolved", presolved);
        a_object->emplace_back("variable_detail", variable_detail);
    }

    /*************************************************************************/
    inline void add_variable_type_detail(
        utility::json::JsonObject *a_object) const {
        auto &REFERENCE_ORIGINAL =
            this->model_ptr->variable_type_reference_original();
        utility::json::JsonObject original;

        auto &REFERENCE_PRESOLVED = this->model_ptr->variable_type_reference();
        utility::json::JsonObject presolved;

        auto compute_number_of_variables = [](const auto &a_VARIABLE_PTRS) {
            return a_VARIABLE_PTRS.size();
        };

        auto compute_number_of_mutable_variables =
            [](const auto &a_VARIABLE_PTRS) {
                return std::count_if(a_VARIABLE_PTRS.begin(),
                                     a_VARIABLE_PTRS.end(),
                                     [](const auto *a_VARIABLE_PTR) {
                                         return !a_VARIABLE_PTR->is_fixed();
                                     });
            };

        /// Original
        original.emplace_back(  //
            "binary",           //
            compute_number_of_variables(
                REFERENCE_ORIGINAL.binary_variable_ptrs));

        original.emplace_back(  //
            "integer",          //
            compute_number_of_variables(
                REFERENCE_ORIGINAL.integer_variable_ptrs));

        original.emplace_back(  //
            "selection",        //
            compute_number_of_variables(
                REFERENCE_ORIGINAL.selection_variable_ptrs));

        original.emplace_back(   //
            "dependent_binary",  //
            compute_number_of_variables(
                REFERENCE_ORIGINAL.dependent_binary_variable_ptrs));

        original.emplace_back(    //
            "dependent_integer",  //
            compute_number_of_variables(
                REFERENCE_ORIGINAL.dependent_integer_variable_ptrs));

        /// Presolved
        presolved.emplace_back(  //
            "binary",            //
            compute_number_of_mutable_variables(
                REFERENCE_PRESOLVED.binary_variable_ptrs));

        presolved.emplace_back(  //
            "integer",           //
            compute_number_of_mutable_variables(
                REFERENCE_PRESOLVED.integer_variable_ptrs));

        presolved.emplace_back(  //
            "selection",         //
            compute_number_of_mutable_variables(
                REFERENCE_PRESOLVED.selection_variable_ptrs));

        presolved.emplace_back(  //
            "dependent_binary",  //
            compute_number_of_mutable_variables(
                REFERENCE_PRESOLVED.dependent_binary_variable_ptrs));

        presolved.emplace_back(   //
            "dependent_integer",  //
            compute_number_of_mutable_variables(
                REFERENCE_PRESOLVED.dependent_integer_variable_ptrs));

        utility::json::JsonObject variable_type_detail;
        variable_type_detail.emplace_back("original", original);
        variable_type_detail.emplace_back("presolved", presolved);
        a_object->emplace_back("variable_type_detail", variable_type_detail);
    }

    /*************************************************************************/
    inline void add_constraint_detail(
        utility::json::JsonObject *a_object) const {
        auto &REFERENCE_ORIGINAL =
            this->model_ptr->constraint_reference_original();
        utility::json::JsonObject original;

        auto &REFERENCE_PRESOLVED = this->model_ptr->constraint_reference();
        utility::json::JsonObject presolved;

        /// Original
        original.emplace_back(  //
            "all",              //
            REFERENCE_ORIGINAL.constraint_ptrs.size());

        original.emplace_back(  //
            "enabled",          ///
            REFERENCE_ORIGINAL.enabled_constraint_ptrs.size());

        original.emplace_back(  //
            "disabled",         //
            REFERENCE_ORIGINAL.disabled_constraint_ptrs.size());

        /// Presolved
        presolved.emplace_back(  //
            "all",               //
            REFERENCE_PRESOLVED.constraint_ptrs.size());

        presolved.emplace_back(  //
            "enabled",           ///
            REFERENCE_PRESOLVED.enabled_constraint_ptrs.size());

        presolved.emplace_back(  //
            "disabled",          //
            REFERENCE_PRESOLVED.disabled_constraint_ptrs.size());

        utility::json::JsonObject constraint_detail;
        constraint_detail.emplace_back("original", original);
        constraint_detail.emplace_back("presolved", presolved);
        a_object->emplace_back("constraint_detail", constraint_detail);
    }

    /*************************************************************************/
    inline void add_constraint_type_detail(
        utility::json::JsonObject *a_object) const {
        auto &REFERENCE_ORIGINAL =
            this->model_ptr->constraint_type_reference_original();
        utility::json::JsonObject original;

        auto &REFERENCE_PRESOLVED =
            this->model_ptr->constraint_type_reference();
        utility::json::JsonObject presolved;

        auto compute_number_of_constraints = [](const auto &a_CONSTRAINT_PTRS) {
            return a_CONSTRAINT_PTRS.size();
        };

        auto compute_number_of_enabled_constraints =
            [](const auto &a_CONSTRAINT_PTRS) {
                return std::count_if(a_CONSTRAINT_PTRS.begin(),
                                     a_CONSTRAINT_PTRS.end(),
                                     [](const auto *a_CONSTRAINT_PTR) {
                                         return a_CONSTRAINT_PTR->is_enabled();
                                     });
            };

        /// Original
        original.emplace_back(  //
            "singleton",        //
            compute_number_of_constraints(REFERENCE_ORIGINAL.singleton_ptrs));

        original.emplace_back(  //
            "exclusive_or",     //
            compute_number_of_constraints(
                REFERENCE_ORIGINAL.exclusive_or_ptrs));

        original.emplace_back(  //
            "exclusive_nor",    //
            compute_number_of_constraints(
                REFERENCE_ORIGINAL.exclusive_nor_ptrs));

        original.emplace_back(    //
            "inverted_integers",  //
            compute_number_of_constraints(
                REFERENCE_ORIGINAL.inverted_integers_ptrs));

        original.emplace_back(    //
            "balanced_integers",  //
            compute_number_of_constraints(
                REFERENCE_ORIGINAL.balanced_integers_ptrs));

        original.emplace_back(        //
            "constant_sum_integers",  //
            compute_number_of_constraints(
                REFERENCE_ORIGINAL.constant_sum_integers_ptrs));

        original.emplace_back(               //
            "constant_difference_integers",  //
            compute_number_of_constraints(
                REFERENCE_ORIGINAL.constant_difference_integers_ptrs));

        original.emplace_back(          //
            "constant_ratio_integers",  //
            compute_number_of_constraints(
                REFERENCE_ORIGINAL.constant_ratio_integers_ptrs));

        original.emplace_back(  //
            "aggregation",      //
            compute_number_of_constraints(REFERENCE_ORIGINAL.aggregation_ptrs));

        original.emplace_back(  //
            "precedence",       //
            compute_number_of_constraints(REFERENCE_ORIGINAL.precedence_ptrs));

        original.emplace_back(  //
            "variable_bound",   //
            compute_number_of_constraints(
                REFERENCE_ORIGINAL.variable_bound_ptrs));

        original.emplace_back(          //
            "trinomial_exclusive_nor",  //
            compute_number_of_constraints(
                REFERENCE_ORIGINAL.trinomial_exclusive_nor_ptrs));

        original.emplace_back(   //
            "set_partitioning",  //
            compute_number_of_constraints(
                REFERENCE_ORIGINAL.set_partitioning_ptrs));

        original.emplace_back(  //
            "set_packing",      //
            compute_number_of_constraints(REFERENCE_ORIGINAL.set_packing_ptrs));

        original.emplace_back(  //
            "set_covering",     //
            compute_number_of_constraints(
                REFERENCE_ORIGINAL.set_covering_ptrs));

        original.emplace_back(  //
            "cardinality",      //
            compute_number_of_constraints(REFERENCE_ORIGINAL.cardinality_ptrs));

        original.emplace_back(     //
            "invariant_knapsack",  //
            compute_number_of_constraints(
                REFERENCE_ORIGINAL.invariant_knapsack_ptrs));

        original.emplace_back(    //
            "multiple_covering",  //
            compute_number_of_constraints(
                REFERENCE_ORIGINAL.multiple_covering_ptrs));

        original.emplace_back(  //
            "binary_flow",      //
            compute_number_of_constraints(REFERENCE_ORIGINAL.binary_flow_ptrs));

        original.emplace_back(  //
            "integer_flow",     //
            compute_number_of_constraints(
                REFERENCE_ORIGINAL.integer_flow_ptrs));

        original.emplace_back(  //
            "soft_selection",   //
            compute_number_of_constraints(
                REFERENCE_ORIGINAL.soft_selection_ptrs));

        original.emplace_back(  //
            "min_max",          //
            compute_number_of_constraints(REFERENCE_ORIGINAL.min_max_ptrs));

        original.emplace_back(  //
            "max_min",          //
            compute_number_of_constraints(REFERENCE_ORIGINAL.max_min_ptrs));

        original.emplace_back(  //
            "intermediate",     //
            compute_number_of_constraints(
                REFERENCE_ORIGINAL.intermediate_ptrs));

        original.emplace_back(    //
            "equation_knapsack",  //
            compute_number_of_constraints(
                REFERENCE_ORIGINAL.equation_knapsack_ptrs));

        original.emplace_back(  //
            "bin_packing",      //
            compute_number_of_enabled_constraints(
                REFERENCE_ORIGINAL.bin_packing_ptrs));

        original.emplace_back(  //
            "knapsack",         //
            compute_number_of_constraints(REFERENCE_ORIGINAL.knapsack_ptrs));

        original.emplace_back(   //
            "integer_knapsack",  //
            compute_number_of_constraints(
                REFERENCE_ORIGINAL.integer_knapsack_ptrs));

        original.emplace_back(  //
            "gf2",              //
            compute_number_of_constraints(REFERENCE_ORIGINAL.gf2_ptrs));

        original.emplace_back(  //
            "general_linear",   //
            compute_number_of_constraints(
                REFERENCE_ORIGINAL.general_linear_ptrs));

        original.emplace_back(  //
            "nonlinear",        //
            compute_number_of_constraints(REFERENCE_ORIGINAL.nonlinear_ptrs));

        /// Presolved
        presolved.emplace_back(  //
            "singleton",         //
            compute_number_of_enabled_constraints(
                REFERENCE_PRESOLVED.singleton_ptrs));

        presolved.emplace_back(  //
            "exclusive_or",      //
            compute_number_of_enabled_constraints(
                REFERENCE_PRESOLVED.exclusive_or_ptrs));

        presolved.emplace_back(  //
            "exclusive_nor",     //
            compute_number_of_enabled_constraints(
                REFERENCE_PRESOLVED.exclusive_nor_ptrs));

        presolved.emplace_back(   //
            "inverted_integers",  //
            compute_number_of_enabled_constraints(
                REFERENCE_PRESOLVED.inverted_integers_ptrs));

        presolved.emplace_back(   //
            "balanced_integers",  //
            compute_number_of_enabled_constraints(
                REFERENCE_PRESOLVED.balanced_integers_ptrs));

        presolved.emplace_back(       //
            "constant_sum_integers",  //
            compute_number_of_enabled_constraints(
                REFERENCE_PRESOLVED.constant_sum_integers_ptrs));

        presolved.emplace_back(              //
            "constant_difference_integers",  //
            compute_number_of_enabled_constraints(
                REFERENCE_PRESOLVED.constant_difference_integers_ptrs));

        presolved.emplace_back(         //
            "constant_ratio_integers",  //
            compute_number_of_enabled_constraints(
                REFERENCE_PRESOLVED.constant_ratio_integers_ptrs));

        presolved.emplace_back(  //
            "aggregation",       //
            compute_number_of_enabled_constraints(
                REFERENCE_PRESOLVED.aggregation_ptrs));

        presolved.emplace_back(  //
            "precedence",        //
            compute_number_of_enabled_constraints(
                REFERENCE_PRESOLVED.precedence_ptrs));

        presolved.emplace_back(  //
            "variable_bound",    //
            compute_number_of_enabled_constraints(
                REFERENCE_PRESOLVED.variable_bound_ptrs));

        presolved.emplace_back(         //
            "trinomial_exclusive_nor",  //
            compute_number_of_enabled_constraints(
                REFERENCE_PRESOLVED.trinomial_exclusive_nor_ptrs));

        presolved.emplace_back(  //
            "set_partitioning",  //
            compute_number_of_enabled_constraints(
                REFERENCE_PRESOLVED.set_partitioning_ptrs));

        presolved.emplace_back(  //
            "set_packing",       //
            compute_number_of_enabled_constraints(
                REFERENCE_PRESOLVED.set_packing_ptrs));

        presolved.emplace_back(  //
            "set_covering",      //
            compute_number_of_enabled_constraints(
                REFERENCE_PRESOLVED.set_covering_ptrs));

        presolved.emplace_back(  //
            "cardinality",       //
            compute_number_of_enabled_constraints(
                REFERENCE_PRESOLVED.cardinality_ptrs));

        presolved.emplace_back(    //
            "invariant_knapsack",  //
            compute_number_of_enabled_constraints(
                REFERENCE_PRESOLVED.invariant_knapsack_ptrs));

        presolved.emplace_back(   //
            "multiple_covering",  //
            compute_number_of_enabled_constraints(
                REFERENCE_PRESOLVED.multiple_covering_ptrs));

        presolved.emplace_back(  //
            "binary_flow",       //
            compute_number_of_enabled_constraints(
                REFERENCE_PRESOLVED.binary_flow_ptrs));

        presolved.emplace_back(  //
            "integer_flow",      //
            compute_number_of_enabled_constraints(
                REFERENCE_PRESOLVED.integer_flow_ptrs));

        presolved.emplace_back(  //
            "soft_selection",    //
            compute_number_of_enabled_constraints(
                REFERENCE_PRESOLVED.soft_selection_ptrs));

        presolved.emplace_back(  //
            "min_max",           //
            compute_number_of_enabled_constraints(
                REFERENCE_PRESOLVED.min_max_ptrs));

        presolved.emplace_back(  //
            "max_min",           //
            compute_number_of_enabled_constraints(
                REFERENCE_PRESOLVED.max_min_ptrs));

        presolved.emplace_back(  //
            "intermediate",      //
            compute_number_of_enabled_constraints(
                REFERENCE_PRESOLVED.intermediate_ptrs));

        presolved.emplace_back(   //
            "equation_knapsack",  //
            compute_number_of_enabled_constraints(
                REFERENCE_PRESOLVED.equation_knapsack_ptrs));

        presolved.emplace_back(  //
            "bin_packing",       //
            compute_number_of_enabled_constraints(
                REFERENCE_PRESOLVED.bin_packing_ptrs));

        presolved.emplace_back(  //
            "knapsack",          //
            compute_number_of_enabled_constraints(
                REFERENCE_PRESOLVED.knapsack_ptrs));

        presolved.emplace_back(  //
            "integer_knapsack",  //
            compute_number_of_enabled_constraints(
                REFERENCE_PRESOLVED.integer_knapsack_ptrs));

        presolved.emplace_back(  //
            "gf2",               //
            compute_number_of_enabled_constraints(
                REFERENCE_PRESOLVED.gf2_ptrs));

        presolved.emplace_back(  //
            "general_linear",    //
            compute_number_of_enabled_constraints(
                REFERENCE_PRESOLVED.general_linear_ptrs));

        presolved.emplace_back(  //
            "nonlinear",         //
            compute_number_of_enabled_constraints(
                REFERENCE_PRESOLVED.nonlinear_ptrs));

        utility::json::JsonObject constraint_type_detail;
        constraint_type_detail.emplace_back("original", original);
        constraint_type_detail.emplace_back("presolved", presolved);
        a_object->emplace_back("constraint_type_detail",
                               constraint_type_detail);
    }

    /*************************************************************************/
    void write_json_by_name(const std::string &a_FILE_NAME) const {
        utility::json::JsonObject object;
        this->add_summary_json(&object);
        this->add_variable_detail(&object);
        this->add_variable_type_detail(&object);
        this->add_constraint_detail(&object);
        this->add_constraint_type_detail(&object);
        this->add_option_json(&object);

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
    void write_json_by_array(const std::string &a_FILE_NAME) const {
        utility::json::JsonObject object;
        this->add_summary_json(&object);
        this->add_variable_detail(&object);
        this->add_variable_type_detail(&object);
        this->add_constraint_detail(&object);
        this->add_constraint_type_detail(&object);
        this->add_option_json(&object);

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
