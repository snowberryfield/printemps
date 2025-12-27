/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
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
    solver::Solver<T_Variable, T_Expression> *solver_ptr;
    model::Model<T_Variable, T_Expression>   *model_ptr;
    option::Option                            option;

    T_Expression objective;
    T_Expression total_violation;
    bool         is_found_feasible_solution;

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

    double averaged_inner_iteration_speed;
    double averaged_move_evaluation_speed;

    double averaged_number_of_threads_move_update;
    double averaged_number_of_threads_move_evaluation;

    std::unordered_map<std::string, multi_array::ValueProxy<double>>
        penalty_coefficients;

    std::unordered_map<std::string, multi_array::ValueProxy<long>>
        update_counts;
    std::unordered_map<std::string, multi_array::ValueProxy<long>>
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
        this->solver_ptr = nullptr;
        this->model_ptr  = nullptr;
        this->option.initialize();

        this->objective                  = 0;
        this->total_violation            = 0;
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

        this->averaged_inner_iteration_speed             = 0.0;
        this->averaged_move_evaluation_speed             = 0.0;
        this->averaged_number_of_threads_move_update     = 0.0;
        this->averaged_number_of_threads_move_evaluation = 0.0;

        this->penalty_coefficients.clear();
        this->update_counts.clear();
        this->violation_counts.clear();
    }

    /*************************************************************************/
    void setup(Solver<T_Variable, T_Expression> *a_solver_ptr) {
        this->solver_ptr = a_solver_ptr;
        const auto &LAGRANGE_DUAL_RESULT =
            a_solver_ptr->lagrange_dual_controller().result();
        const auto &LOCAL_SEARCH_RESULT =
            a_solver_ptr->local_search_controller().result();
        const auto &TABU_SEARCH_RESULT =
            a_solver_ptr->tabu_search_controller().result();

        const auto &INCUMBENT_SOLUTION =
            this->solver_ptr->global_state()
                .incumbent_holder.global_augmented_incumbent_solution();

        this->model_ptr = a_solver_ptr->model_ptr();
        this->option    = a_solver_ptr->option_original();

        this->objective = INCUMBENT_SOLUTION.objective;

        this->total_violation            = INCUMBENT_SOLUTION.total_violation;
        this->is_found_feasible_solution = INCUMBENT_SOLUTION.is_feasible;

        this->start_date_time  = a_solver_ptr->start_date_time();
        this->finish_date_time = a_solver_ptr->finish_date_time();

        this->name = this->model_ptr->name();
        this->number_of_variables =
            this->model_ptr->reference().number_of_variables();
        this->number_of_constraints =
            this->model_ptr->reference().number_of_constraints();
        this->elapsed_time = a_solver_ptr->time_keeper().elapsed_time();

        this->number_of_local_search_iterations =
            LOCAL_SEARCH_RESULT.core.number_of_iterations;
        this->number_of_lagrange_dual_iterations =
            LAGRANGE_DUAL_RESULT.core.number_of_iterations;
        this->number_of_tabu_search_iterations =
            TABU_SEARCH_RESULT.state.total_number_of_inner_iterations;
        this->number_of_tabu_search_loops = TABU_SEARCH_RESULT.state.iteration;
        this->number_of_evaluated_moves =
            TABU_SEARCH_RESULT.state.total_number_of_evaluated_moves;

        this->averaged_inner_iteration_speed =
            TABU_SEARCH_RESULT.state.averaged_inner_iteration_speed;
        this->averaged_move_evaluation_speed =
            TABU_SEARCH_RESULT.state.averaged_move_evaluation_speed;
        this->averaged_number_of_threads_move_update =
            TABU_SEARCH_RESULT.state.averaged_number_of_threads_move_update;
        this->averaged_number_of_threads_move_evaluation =
            TABU_SEARCH_RESULT.state.averaged_number_of_threads_move_evaluation;

        this->penalty_coefficients = this->model_ptr->state_inspector()
                                         .export_named_penalty_coefficients();
        this->update_counts =
            this->model_ptr->state_inspector().export_named_update_counts();
        this->violation_counts =
            this->model_ptr->state_inspector().export_named_violation_counts();
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
        a_object->emplace_back(  //
            "version", constant::VERSION);

        a_object->emplace_back(  //
            "name", this->name);

        a_object->emplace_back(  //
            "number_of_variables", this->number_of_variables);

        a_object->emplace_back(  //
            "number_of_constraints", this->number_of_constraints);

        a_object->emplace_back(  //
            "objective", this->objective);

        a_object->emplace_back(  //
            "total_violation", this->total_violation);

        a_object->emplace_back(  //
            "is_found_feasible_solution", this->is_found_feasible_solution);

        a_object->emplace_back(  //
            "start_date_time", this->start_date_time);

        a_object->emplace_back(  //
            "finish_date_time", this->finish_date_time);

        a_object->emplace_back(  //
            "elapsed_time", this->elapsed_time);

        a_object->emplace_back(                    //
            "number_of_lagrange_dual_iterations",  //
            this->number_of_lagrange_dual_iterations);

        a_object->emplace_back(                   //
            "number_of_local_search_iterations",  //
            this->number_of_local_search_iterations);

        a_object->emplace_back(                  //
            "number_of_tabu_search_iterations",  //
            this->number_of_tabu_search_iterations);

        a_object->emplace_back(             //
            "number_of_tabu_search_loops",  //
            this->number_of_tabu_search_loops);

        a_object->emplace_back(           //
            "number_of_evaluated_moves",  //
            this->number_of_evaluated_moves);

        a_object->emplace_back(                //
            "averaged_inner_iteration_speed",  //
            this->averaged_inner_iteration_speed);

        a_object->emplace_back(                //
            "averaged_move_evaluation_speed",  //
            this->averaged_move_evaluation_speed);

        a_object->emplace_back(                        //
            "averaged_number_of_threads_move_update",  //
            this->averaged_number_of_threads_move_update);

        a_object->emplace_back(                            //
            "averaged_number_of_threads_move_evaluation",  //
            this->averaged_number_of_threads_move_evaluation);

        const auto &PROBLEM_SIZE_REDUCER =
            this->model_ptr->problem_size_reducer();

        a_object->emplace_back(                           //
            "variable_bound_update_count_in_preprocess",  //
            PROBLEM_SIZE_REDUCER.variable_bound_update_count_in_preprocess());

        a_object->emplace_back(                             //
            "variable_bound_update_count_in_optimization",  //
            PROBLEM_SIZE_REDUCER.variable_bound_update_count_in_optimization());
    }

    /*************************************************************************/
    inline void add_option_json(utility::json::JsonObject *a_object) const {
        a_object->emplace_back("option", this->option.to_json());
    }

    /*************************************************************************/
    inline void add_nonzero_detail(utility::json::JsonObject *a_object) const {
        using namespace model_handler;
        const std::array<std::string, 2> LABELS = {"original", "presolved"};
        const std::array<  //
            const VariableReference<T_Variable, T_Expression> *, 2>
            VARIABLE_REFERENCE_PTRS = {
                &(this->model_ptr->reference_original().variable),
                &(this->model_ptr->reference().variable)};
        const std::array<  //
            const ConstraintReference<T_Variable, T_Expression> *, 2>
            CONSTRAINT_REFERENCE_PTRS = {
                &(this->model_ptr->reference_original().constraint),
                &(this->model_ptr->reference().constraint)};

        utility::json::JsonObject nonzero_detail;

        for (auto i = 0; i < 2; i++) {
            const auto  LABEL = LABELS[i];
            const auto &VARIABLE_PTRS =
                VARIABLE_REFERENCE_PTRS[i]->mutable_variable_ptrs;
            const auto &CONSTRAINT_PTRS =
                CONSTRAINT_REFERENCE_PTRS[i]->enabled_constraint_ptrs;
            const int VARIABLES_SIZE   = VARIABLE_PTRS.size();
            const int CONSTRAINTS_SIZE = CONSTRAINT_PTRS.size();

            long   nonzero                 = 0;
            double density                 = 0.0;
            double variable_nonzero_mean   = 0.0;
            double constraint_nonzero_mean = 0.0;

            utility::Range<int> variable_nonzero_range;
            utility::Range<int> constraint_nonzero_range;

            for (auto &&constraint_ptr : CONSTRAINT_PTRS) {
                const int NUMBER_OF_RELATED_VARIABLES =
                    constraint_ptr->expression().sensitivities().size();
                nonzero += NUMBER_OF_RELATED_VARIABLES;
                constraint_nonzero_range.update(NUMBER_OF_RELATED_VARIABLES);
            }

            for (auto &&variable_ptr : VARIABLE_PTRS) {
                const int NUMBER_OF_RELATED_CONSTRAINTS =
                    variable_ptr->related_constraint_ptrs().size();
                variable_nonzero_range.update(NUMBER_OF_RELATED_CONSTRAINTS);
            }

            if (VARIABLES_SIZE > 0 && CONSTRAINTS_SIZE > 0) {
                density = static_cast<double>(nonzero) /
                          (static_cast<double>(VARIABLES_SIZE) *
                           static_cast<double>(CONSTRAINTS_SIZE));
            }

            if (VARIABLES_SIZE > 0) {
                variable_nonzero_mean = static_cast<double>(nonzero) /
                                        static_cast<double>(VARIABLES_SIZE);
            }

            if (CONSTRAINTS_SIZE > 0) {
                constraint_nonzero_mean = static_cast<double>(nonzero) /
                                          static_cast<double>(CONSTRAINTS_SIZE);
            }

            utility::json::JsonObject obj;

            obj.emplace_back("nonzero", nonzero);
            obj.emplace_back("density", density);

            obj.emplace_back(  //
                "variable_nonzero_mean", variable_nonzero_mean);

            obj.emplace_back(  //
                "variable_nonzero_min", variable_nonzero_range.min());

            obj.emplace_back(  //
                "variable_nonzero_max", variable_nonzero_range.max());

            obj.emplace_back(  //
                "constraint_nonzero_mean", constraint_nonzero_mean);

            obj.emplace_back(  //
                "constraint_nonzero_min", constraint_nonzero_range.min());

            obj.emplace_back(  //
                "constraint_nonzero_max", constraint_nonzero_range.max());

            nonzero_detail.emplace_back(LABEL, obj);
        }
        a_object->emplace_back("nonzero_detail", nonzero_detail);
    }

    /*************************************************************************/
    inline void add_variable_detail(utility::json::JsonObject *a_object) const {
        using namespace model_handler;
        const std::array<std::string, 2> LABELS = {"original", "presolved"};
        const std::array<const VariableReference<T_Variable, T_Expression> *, 2>
            VARIABLE_REFERENCE_PTRS = {
                &(this->model_ptr->reference_original().variable),
                &(this->model_ptr->reference().variable)};

        utility::json::JsonObject variable_detail;

        for (auto i = 0; i < 2; i++) {
            const auto  LABEL     = LABELS[i];
            const auto &REFERENCE = (*VARIABLE_REFERENCE_PTRS[i]);

            utility::json::JsonObject obj;
            obj.emplace_back("all", REFERENCE.variable_ptrs.size());
            obj.emplace_back("fixed", REFERENCE.fixed_variable_ptrs.size());
            obj.emplace_back("mutable", REFERENCE.mutable_variable_ptrs.size());

            variable_detail.emplace_back(LABEL, obj);
        }

        a_object->emplace_back("variable_detail", variable_detail);
    }

    /*************************************************************************/
    inline void add_variable_type_detail(
        utility::json::JsonObject *a_object) const {
        using namespace model_handler;
        const std::array<std::string, 2> LABELS = {"original", "presolved"};
        const std::array<
            const VariableTypeReference<T_Variable, T_Expression> *, 2>
            VARIABLE_TYPE_REFERENCE_PTRS = {
                &(this->model_ptr->reference_original().variable_type),
                &(this->model_ptr->reference().variable_type)};

        utility::json::JsonObject variable_type_detail;

        std::function<std::size_t(const std::vector<model_component::Variable<
                                      T_Variable, T_Expression> *> &)>
            compute_number_of_all_variables = [](const auto &a_VARIABLE_PTRS) {
                return a_VARIABLE_PTRS.size();
            };

        std::function<std::size_t(const std::vector<model_component::Variable<
                                      T_Variable, T_Expression> *> &)>
            compute_number_of_mutable_variables =
                [](const auto &a_VARIABLE_PTRS) {
                    return std::count_if(a_VARIABLE_PTRS.begin(),
                                         a_VARIABLE_PTRS.end(),
                                         [](const auto *a_VARIABLE_PTR) {
                                             return !a_VARIABLE_PTR->is_fixed();
                                         });
                };

        for (auto i = 0; i < 2; i++) {
            const auto  LABEL     = LABELS[i];
            const auto &REFERENCE = (*VARIABLE_TYPE_REFERENCE_PTRS[i]);

            auto compute_number_of_variables =
                (i == 0) ? compute_number_of_all_variables
                         : compute_number_of_mutable_variables;

            utility::json::JsonObject obj;

            obj.emplace_back(  //
                "binary",      //
                compute_number_of_variables(REFERENCE.binary_variable_ptrs));

            obj.emplace_back(  //
                "integer",     //
                compute_number_of_variables(REFERENCE.integer_variable_ptrs));

            obj.emplace_back(  //
                "selection",   //
                compute_number_of_variables(REFERENCE.selection_variable_ptrs));

            obj.emplace_back(  //
                "dependent_binary",
                compute_number_of_variables(
                    REFERENCE.dependent_binary_variable_ptrs));

            obj.emplace_back(         //
                "dependent_integer",  //
                compute_number_of_variables(
                    REFERENCE.dependent_integer_variable_ptrs));

            variable_type_detail.emplace_back(LABEL, obj);
        }
        a_object->emplace_back("variable_type_detail", variable_type_detail);
    }

    /*************************************************************************/
    inline void add_constraint_detail(
        utility::json::JsonObject *a_object) const {
        using namespace model_handler;
        const std::array<std::string, 2> LABELS = {"original", "presolved"};
        const std::array<  //
            const ConstraintReference<T_Variable, T_Expression> *, 2>
            CONSTRAINT_REFERENCE_PTRS = {
                &(this->model_ptr->reference_original().constraint),
                &(this->model_ptr->reference().constraint)};

        utility::json::JsonObject constraint_detail;

        for (auto i = 0; i < 2; i++) {
            const auto  LABEL     = LABELS[i];
            const auto &REFERENCE = (*CONSTRAINT_REFERENCE_PTRS[i]);

            utility::json::JsonObject obj;

            obj.emplace_back(  //
                "all", REFERENCE.constraint_ptrs.size());

            obj.emplace_back(  //
                "enabled", REFERENCE.enabled_constraint_ptrs.size());

            obj.emplace_back(  //
                "disabled", REFERENCE.disabled_constraint_ptrs.size());

            constraint_detail.emplace_back(LABEL, obj);
        }

        a_object->emplace_back("constraint_detail", constraint_detail);
    }

    /*************************************************************************/
    inline void add_constraint_type_detail(
        utility::json::JsonObject *a_object) const {
        using namespace model_handler;
        const std::array<std::string, 2> LABELS = {"original", "presolved"};
        const std::array<  //
            const ConstraintTypeReference<T_Variable, T_Expression> *, 2>
            CONSTRAINT_TYPE_REFERENCE_PTRS = {
                &(this->model_ptr->reference_original().constraint_type),
                &(this->model_ptr->reference().constraint_type)};

        utility::json::JsonObject constraint_type_detail;

        std::function<std::size_t(const std::vector<model_component::Constraint<
                                      T_Variable, T_Expression> *> &)>
            compute_number_of_all_constraints =
                [](const auto &a_CONSTRAINT_PTRS) {
                    return a_CONSTRAINT_PTRS.size();
                };

        std::function<std::size_t(const std::vector<model_component::Constraint<
                                      T_Variable, T_Expression> *> &)>
            compute_number_of_enabled_constraints =
                [](const auto &a_CONSTRAINT_PTRS) {
                    return std::count_if(
                        a_CONSTRAINT_PTRS.begin(), a_CONSTRAINT_PTRS.end(),
                        [](const auto *a_CONSTRAINT_PTR) {
                            return a_CONSTRAINT_PTR->is_enabled();
                        });
                };

        for (auto i = 0; i < 2; i++) {
            const auto  LABEL     = LABELS[i];
            const auto &REFERENCE = (*CONSTRAINT_TYPE_REFERENCE_PTRS[i]);

            auto compute_number_of_constraints =
                (i == 0) ? compute_number_of_all_constraints
                         : compute_number_of_enabled_constraints;

            utility::json::JsonObject obj;

            obj.emplace_back(  //
                "empty",       //
                compute_number_of_constraints(REFERENCE.empty_ptrs));

            obj.emplace_back(  //
                "singleton",   //
                compute_number_of_constraints(REFERENCE.singleton_ptrs));

            obj.emplace_back(    //
                "exclusive_or",  //
                compute_number_of_constraints(REFERENCE.exclusive_or_ptrs));

            obj.emplace_back(     //
                "exclusive_nor",  //
                compute_number_of_constraints(REFERENCE.exclusive_nor_ptrs));

            obj.emplace_back(         //
                "inverted_integers",  //
                compute_number_of_constraints(
                    REFERENCE.inverted_integers_ptrs));

            obj.emplace_back(         //
                "balanced_integers",  //
                compute_number_of_constraints(
                    REFERENCE.balanced_integers_ptrs));

            obj.emplace_back(             //
                "constant_sum_integers",  //
                compute_number_of_constraints(
                    REFERENCE.constant_sum_integers_ptrs));

            obj.emplace_back(                    //
                "constant_difference_integers",  //
                compute_number_of_constraints(
                    REFERENCE.constant_difference_integers_ptrs));

            obj.emplace_back(               //
                "constant_ratio_integers",  //
                compute_number_of_constraints(
                    REFERENCE.constant_ratio_integers_ptrs));

            obj.emplace_back(   //
                "aggregation",  //
                compute_number_of_constraints(REFERENCE.aggregation_ptrs));

            obj.emplace_back(  //
                "precedence",  //
                compute_number_of_constraints(REFERENCE.precedence_ptrs));

            obj.emplace_back(      //
                "variable_bound",  //
                compute_number_of_constraints(REFERENCE.variable_bound_ptrs));

            obj.emplace_back(               //
                "trinomial_exclusive_nor",  //
                compute_number_of_constraints(
                    REFERENCE.trinomial_exclusive_nor_ptrs));

            obj.emplace_back(      //
                "all_or_nothing",  //
                compute_number_of_constraints(REFERENCE.all_or_nothing_ptrs));

            obj.emplace_back(        //
                "set_partitioning",  //
                compute_number_of_constraints(REFERENCE.set_partitioning_ptrs));

            obj.emplace_back(   //
                "set_packing",  //
                compute_number_of_constraints(REFERENCE.set_packing_ptrs));

            obj.emplace_back(    //
                "set_covering",  //
                compute_number_of_constraints(REFERENCE.set_covering_ptrs));

            obj.emplace_back(   //
                "cardinality",  //
                compute_number_of_constraints(REFERENCE.cardinality_ptrs));

            obj.emplace_back(          //
                "invariant_knapsack",  //
                compute_number_of_constraints(
                    REFERENCE.invariant_knapsack_ptrs));

            obj.emplace_back(         //
                "multiple_covering",  //
                compute_number_of_constraints(
                    REFERENCE.multiple_covering_ptrs));

            obj.emplace_back(   //
                "binary_flow",  //
                compute_number_of_constraints(REFERENCE.binary_flow_ptrs));

            obj.emplace_back(    //
                "integer_flow",  //
                compute_number_of_constraints(REFERENCE.integer_flow_ptrs));

            obj.emplace_back(      //
                "soft_selection",  //
                compute_number_of_constraints(REFERENCE.soft_selection_ptrs));

            obj.emplace_back(  //
                "min_max",     //
                compute_number_of_constraints(REFERENCE.min_max_ptrs));

            obj.emplace_back(  //
                "max_min",     //
                compute_number_of_constraints(REFERENCE.max_min_ptrs));

            obj.emplace_back(    //
                "intermediate",  //
                compute_number_of_constraints(REFERENCE.intermediate_ptrs));

            obj.emplace_back(         //
                "equation_knapsack",  //
                compute_number_of_constraints(
                    REFERENCE.equation_knapsack_ptrs));

            obj.emplace_back(   //
                "bin_packing",  //
                compute_number_of_constraints(REFERENCE.bin_packing_ptrs));

            obj.emplace_back(  //
                "knapsack",    //
                compute_number_of_constraints(REFERENCE.knapsack_ptrs));

            obj.emplace_back(        //
                "integer_knapsack",  //
                compute_number_of_constraints(REFERENCE.integer_knapsack_ptrs));

            obj.emplace_back(  //
                "gf2",         //
                compute_number_of_constraints(REFERENCE.gf2_ptrs));

            obj.emplace_back(      //
                "general_linear",  //
                compute_number_of_constraints(REFERENCE.general_linear_ptrs));

            constraint_type_detail.emplace_back(LABEL, obj);
        }

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
        this->add_nonzero_detail(&object);
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
        this->add_nonzero_detail(&object);
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
