/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_H__
#define PRINTEMPS_SOLVER_H__

#include "memory.h"
#include "status.h"
#include "result.h"
#include "state.h"
#include "tabu_search_trend_logger.h"

#include "lagrange_dual/lagrange_dual.h"
#include "local_search/local_search.h"
#include "tabu_search/tabu_search.h"

namespace printemps {
namespace solver {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Solver {
   private:
    model::Model<T_Variable, T_Expression>* m_model_ptr;
    option::Option                          m_master_option;

    std::mt19937 m_get_rand_mt;
    Memory       m_memory;

    solution::SolutionArchive<T_Variable, T_Expression> m_solution_archive;
    solution::IncumbentHolder<T_Variable, T_Expression> m_incumbent_holder;

    utility::TimeKeeper m_time_keeper;
    std::string         m_start_date_time;
    std::string         m_finish_date_time;

    State<T_Variable, T_Expression> m_state;

    /*************************************************************************/
    inline void print_current_incumbent_summary(const bool a_IS_ENABLED_PRINT) {
        double elapsed_time = m_time_keeper.clock();
        utility::print_info(  //
            " -- Total elapsed time: " +
                utility::to_string(elapsed_time, "%.3f") + "sec",
            a_IS_ENABLED_PRINT);

        utility::print_info(
            " -- Global augmented incumbent objective: " +
                utility::to_string(
                    m_incumbent_holder.global_augmented_incumbent_objective() *
                        m_model_ptr->sign(),
                    "%.3f"),
            a_IS_ENABLED_PRINT);

        utility::print_info(
            " -- Feasible incumbent objective: " +
                utility::to_string(
                    m_incumbent_holder.feasible_incumbent_objective() *
                        m_model_ptr->sign(),
                    "%.3f"),
            a_IS_ENABLED_PRINT);
    }

   public:
    /*************************************************************************/
    Solver(void) {
        this->initialize();
    }

    /*************************************************************************/
    Solver(model::Model<T_Variable, T_Expression>* a_model_ptr,  //
           const option::Option&                   a_OPTION) {
        this->initialize();
        this->setup(a_model_ptr, a_OPTION);
    }

    /*************************************************************************/
    ~Solver(void) {
        /// nothing to do
    }

    /*************************************************************************/
    inline void initialize(void) {
        m_model_ptr = nullptr;
        m_master_option.initialize();
        m_memory.initialize();
        m_solution_archive.initialize();
        m_incumbent_holder.initialize();
        m_time_keeper.initialize();
        m_start_date_time.clear();
        m_finish_date_time.clear();
        m_state.initialize();
    }

    /*************************************************************************/
    inline void setup(model::Model<T_Variable, T_Expression>* a_model_ptr,  //
                      const option::Option&                   a_OPTION) {
        m_model_ptr     = a_model_ptr;
        m_master_option = a_OPTION;
    }

    /*************************************************************************/
    inline void preprocess(void) {
        /**
         * Start to measure computational time and get the starting time.
         */
        m_time_keeper.set_start_time();
        m_start_date_time = utility::date_time();

        /**
         * Print the program name, the project url, and the starting time.
         */
        utility::print_single_line(m_master_option.verbose >=
                                   option::verbose::Outer);
        utility::print("PRINTEMPS " + constant::VERSION + " (" +
                           constant::PROJECT_URL + ")",
                       m_master_option.verbose >= option::verbose::Outer);
        utility::print("running from " + m_start_date_time,
                       m_master_option.verbose >= option::verbose::Outer);

        /**
         * The model can be solved only once.
         */
        if (m_model_ptr->is_solved()) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "This model has already been solved."));
        } else {
            m_model_ptr->set_is_solved(true);
        }

        /**
         * Set default target objective value if it is not defined by the user.
         * For minimization problems, the default target value is set as -1E100.
         * For maximization problems, the default target value is set as 1E100.
         * If there is no objective function definition, the default target
         * value is set as 0 and the algorithm will be terminated if a feasible
         * solution is found.
         */
        auto target_objective_changed_rate =
            m_master_option.target_objective_value /
                option::OptionConstant::DEFAULT_TARGET_OBJECTIVE -
            1.0;

        if (fabs(target_objective_changed_rate) > constant::EPSILON) {
            m_master_option.target_objective_value *= m_model_ptr->sign();
        }

        if (fabs(target_objective_changed_rate) < constant::EPSILON) {
            if (!m_model_ptr->is_defined_objective()) {
                m_master_option.target_objective_value = 0.0;
            }
        }

        /**
         * Print the option value.
         */
        if (m_master_option.verbose >= option::verbose::Outer) {
            m_master_option.print();
        }

        /**
         * Setup the model.
         */
        m_model_ptr->setup(m_master_option.is_enabled_presolve,
                           m_master_option.is_enabled_initial_value_correction,
                           m_master_option.is_enabled_aggregation_move,
                           m_master_option.is_enabled_precedence_move,
                           m_master_option.is_enabled_variable_bound_move,
                           m_master_option.is_enabled_chain_move,
                           m_master_option.is_enabled_two_flip_move,
                           m_master_option.is_enabled_user_defined_move,
                           m_master_option.selection_mode,
                           m_master_option.verbose >= option::verbose::Outer);

        /**
         * Print the problem size.
         */
        if (m_master_option.verbose >= option::verbose::Outer) {
            m_model_ptr->print_number_of_variables();
            m_model_ptr->print_number_of_constraints();
        }

        /**
         * Disable the Chain move for the problem with no zero one_coefficient
         * constraints (set partitioning, set packing, set covering,
         * cardinality, and invariant knapsack).
         */
        if (m_master_option.is_enabled_chain_move &&
            !m_model_ptr->has_chain_move_effective_constraints()) {
            m_master_option.is_enabled_chain_move = false;
            utility::print_warning(
                "Chain move was disabled because the problem does not include "
                "any zero-one coefficient constraints (set "
                "partitioning/packing/covering, cardinality, invariant "
                "knapsack, and multiple covering).",
                m_master_option.verbose >= option::verbose::Warning);
        }

        /**
         * Enables the default neighborhood moves. Special neighborhood moves
         * will be enabled when optimization stagnates.
         */
        if (m_master_option.is_enabled_binary_move) {
            m_model_ptr->neighborhood().binary().enable();
        }

        if (m_master_option.is_enabled_integer_move) {
            m_model_ptr->neighborhood().integer().enable();
        }

        if (m_master_option.is_enabled_user_defined_move) {
            m_model_ptr->neighborhood().user_defined().enable();
        }

        if (m_master_option.selection_mode != option::selection_mode::None) {
            m_model_ptr->neighborhood().selection().enable();
        }

        /**
         * Check whether there exist special neighborhood moves or not.
         */
        m_state.has_special_neighborhood_moves =
            (m_model_ptr->neighborhood().aggregation().moves().size() +
             m_model_ptr->neighborhood().precedence().moves().size() +
             m_model_ptr->neighborhood().variable_bound().moves().size() +
             m_model_ptr->neighborhood().two_flip().moves().size()) > 0;

        if (m_master_option.is_enabled_chain_move) {
            m_state.has_special_neighborhood_moves = true;
        }

        /**
         * Prepare a random generator.
         */
        m_get_rand_mt.seed(m_master_option.seed);

        /**
         * Set local and global penalty coefficient for each constraint.
         */
        for (auto&& proxy : m_model_ptr->constraint_proxies()) {
            for (auto&& constraint : proxy.flat_indexed_constraints()) {
                constraint.global_penalty_coefficient() =
                    m_master_option.initial_penalty_coefficient;
                constraint.reset_local_penalty_coefficient();
            }
        }

        /**
         * Create memory which stores updating count for each variable.
         */
        m_memory.setup(m_model_ptr);

        /**
         * Prepare feasible solutions archive.
         */
        m_solution_archive.setup(
            m_master_option.feasible_solutions_capacity,  //
            m_model_ptr->is_minimization(),               //
            m_model_ptr->name(),                          //
            m_model_ptr->number_of_variables(),           //
            m_model_ptr->number_of_constraints());

        /**
         * Compute the values of expressions, constraints, and the objective
         * function according to the initial solution.
         */
        m_model_ptr->update();

        /**
         * Update the state.
         */
        m_state.current_solution       = m_model_ptr->export_solution();
        m_state.current_solution_score = m_model_ptr->evaluate({});

        m_state.update_status = m_incumbent_holder.try_update_incumbent(
            m_state.current_solution, m_state.current_solution_score);

        m_state.number_of_lagrange_dual_iterations = 0;
        m_state.number_of_local_search_iterations  = 0;
        m_state.number_of_tabu_search_iterations   = 0;
        m_state.number_of_tabu_search_loops        = 0;

        m_state.is_terminated = false;
    }

    /*************************************************************************/
    inline Result<T_Variable, T_Expression> postprocess(void) {
        /**
         * Get the finish time.
         */
        m_finish_date_time = utility::date_time();

        /**
         * If a feasible solution is found in optimization, the incumbent
         * solution is defined by the solution with the best objective function
         * value among the feasible solutions. If no feasible solution is found,
         * the incumbent solution is substituted by solution with the best
         * augmented solution which has smallest sum of the objective function
         * value and the penalty value.
         */
        auto incumbent =
            m_incumbent_holder.is_found_feasible_solution()
                ? m_incumbent_holder.feasible_incumbent_solution()
                : m_incumbent_holder.global_augmented_incumbent_solution();

        /**
         * All value of the expressions and the constraints are updated forcibly
         * to take into account the cases they are disabled.
         */
        m_model_ptr->import_variable_values(incumbent.variable_value_proxies);
        m_model_ptr->update();
        incumbent = m_model_ptr->export_solution();

        auto named_solution = m_model_ptr->convert_to_named_solution(incumbent);

        /**
         * Export the final penalty coefficient values.
         */
        std::unordered_map<std::string, multi_array::ValueProxy<double>>
            named_penalty_coefficients;

        auto local_penalty_coefficient_proxies =
            m_model_ptr->export_local_penalty_coefficient_proxies();

        const int CONSTRAINT_PROXIES_SIZE =
            m_model_ptr->constraint_proxies().size();
        const auto& constraint_names = m_model_ptr->constraint_names();
        for (auto i = 0; i < CONSTRAINT_PROXIES_SIZE; i++) {
            named_penalty_coefficients[constraint_names[i]] =
                local_penalty_coefficient_proxies[i];
        }

        /**
         * Export the final variable update counts.
         */
        std::unordered_map<std::string, multi_array::ValueProxy<int>>
                  named_update_counts;
        const int VARIABLE_PROXIES_SIZE =
            m_model_ptr->variable_proxies().size();
        const auto& variable_names = m_model_ptr->variable_names();

        const auto& update_counts = m_memory.update_counts();
        for (auto i = 0; i < VARIABLE_PROXIES_SIZE; i++) {
            named_update_counts[variable_names[i]] = update_counts[i];
        }

        /**
         * Prepare the result object to return.
         */
        Result<T_Variable, T_Expression> result;
        result.solution                          = named_solution;
        result.status.penalty_coefficients       = named_penalty_coefficients;
        result.status.update_counts              = named_update_counts;
        result.status.is_found_feasible_solution = named_solution.is_feasible();
        result.status.start_date_time            = m_start_date_time;
        result.status.finish_date_time           = m_finish_date_time;
        result.status.name                       = m_model_ptr->name();
        result.status.number_of_variables = m_model_ptr->number_of_variables();
        result.status.number_of_constraints =
            m_model_ptr->number_of_constraints();
        result.status.elapsed_time = m_time_keeper.elapsed_time();
        result.status.number_of_lagrange_dual_iterations =
            m_state.number_of_lagrange_dual_iterations;
        result.status.number_of_local_search_iterations =
            m_state.number_of_local_search_iterations;
        result.status.number_of_tabu_search_iterations =
            m_state.number_of_tabu_search_iterations;
        result.status.number_of_tabu_search_loops =
            m_state.number_of_tabu_search_loops;
        result.solution_archive = m_solution_archive;

        return result;
    }

    /*************************************************************************/
    inline void run_lagrange_dual(void) {
        utility::print_single_line(m_master_option.verbose ==
                                   option::verbose::Outer);
        if (!m_model_ptr->is_linear()) {
            utility::print_warning(
                "Solving lagrange dual was skipped because the problem is "
                "nonlinear.",
                m_master_option.verbose >= option::verbose::Warning);
            return;
        } else if (m_model_ptr->number_of_selection_variables() > 0 ||
                   m_model_ptr->number_of_intermediate_variables() > 0) {
            utility::print_warning(
                "Solving lagrange dual was skipped because it not applicable "
                "to problems which include selection variables or intermediate "
                "variables.",
                m_master_option.verbose >= option::verbose::Warning);
            return;
        }

        double elapsed_time = m_time_keeper.clock();

        /**
         *  Check the terminating condition.
         */
        if (elapsed_time > m_master_option.time_max) {
            m_state.is_terminated = true;
            utility::print_message(
                "Outer loop was terminated because of time-over (" +
                    utility::to_string(elapsed_time, "%.3f") + "sec).",
                m_master_option.verbose >= option::verbose::Outer);
        } else if (m_incumbent_holder.feasible_incumbent_objective() <=
                   m_master_option.target_objective_value) {
            m_state.is_terminated = true;
            utility::print_message(
                "Outer loop was terminated because of feasible "
                "objective reaches the target limit.",
                m_master_option.verbose >= option::verbose::Outer);
        }

        if (m_state.is_terminated) {
            return;
        }

        /**
         *  Prepare an option object for lagrange dual search.
         */
        option::Option option            = m_master_option;
        option.lagrange_dual.time_offset = elapsed_time;

        /**
         * Prepare the initial variable values.
         */
        auto initial_variable_value_proxies =
            m_state.current_solution.variable_value_proxies;

        /**
         * Run the lagrange dual search.
         */
        auto result = lagrange_dual::solve(m_model_ptr,                     //
                                           option,                          //
                                           initial_variable_value_proxies,  //
                                           m_incumbent_holder);             //

        /**
         * Update the current solution.
         */
        m_state.previous_solution       = m_state.current_solution;
        m_state.previous_solution_score = m_state.current_solution_score;

        m_state.current_solution =
            result.incumbent_holder.global_augmented_incumbent_solution();
        m_state.current_solution_score =
            result.incumbent_holder.global_augmented_incumbent_score();

        /**
         * Update the feasible solutions archive.
         */
        if (m_master_option.is_enabled_store_feasible_solutions &&
            result.feasible_solutions.size() > 0) {
            m_solution_archive.push(result.feasible_solutions);
        }

        /**
         * Update the global augmented incumbent solution if it was
         * improved by the Lagrange dual search.
         */
        m_state.update_status = m_incumbent_holder.try_update_incumbent(
            result.incumbent_holder.global_augmented_incumbent_solution(),
            result.incumbent_holder.global_augmented_incumbent_score());

        /**
         * Preserve the number of iterations for solving the
         * Lagrange dual problem.
         */
        m_state.number_of_lagrange_dual_iterations =
            result.number_of_iterations;

        /**
         * Print the search summary.
         */
        utility::print_message(
            "Solving Lagrange dual finished. ",
            m_master_option.verbose >= option::verbose::Outer);
        this->print_current_incumbent_summary(m_master_option.verbose >=
                                              option::verbose::Outer);
    }

    /*************************************************************************/
    inline void run_local_search(void) {
        double elapsed_time = m_time_keeper.clock();
        utility::print_single_line(m_master_option.verbose ==
                                   option::verbose::Outer);
        /**
         *  Check the terminating condition.
         */
        if (elapsed_time > m_master_option.time_max) {
            m_state.is_terminated = true;
            utility::print_message(
                "Outer loop was terminated because of time-over (" +
                    utility::to_string(elapsed_time, "%.3f") + "sec).",
                m_master_option.verbose >= option::verbose::Outer);
        } else if (m_incumbent_holder.feasible_incumbent_objective() <=
                   m_master_option.target_objective_value) {
            m_state.is_terminated = true;
            utility::print_message(
                "Outer loop was terminated because of feasible objective "
                "reaches the target limit.",
                m_master_option.verbose >= option::verbose::Outer);
        }

        if (m_state.is_terminated) {
            return;
        }
        /**
         *  Prepare an option object for local search.
         */
        option::Option option           = m_master_option;
        option.local_search.time_offset = elapsed_time;

        /**
         * Prepare the initial variable values.
         */
        auto initial_variable_value_proxies =
            m_state.current_solution.variable_value_proxies;

        /**
         * Run the local search.
         */
        auto result = local_search::solve(m_model_ptr,                     //
                                          option,                          //
                                          initial_variable_value_proxies,  //
                                          m_incumbent_holder,              //
                                          m_memory);                       //

        /**
         * Update the current solution.
         */
        m_state.previous_solution       = m_state.current_solution;
        m_state.previous_solution_score = m_state.current_solution_score;

        m_state.current_solution =
            result.incumbent_holder.global_augmented_incumbent_solution();
        m_state.current_solution_score =
            result.incumbent_holder.global_augmented_incumbent_score();

        /**
         * Update the feasible solutions archive.
         */
        if (m_master_option.is_enabled_store_feasible_solutions &&
            result.feasible_solutions.size() > 0) {
            m_solution_archive.push(result.feasible_solutions);
        }

        /**
         * Update the global augmented incumbent solution if it was
         * improved by the local search.
         */
        m_state.update_status = m_incumbent_holder.try_update_incumbent(
            result.incumbent_holder.global_augmented_incumbent_solution(),
            result.incumbent_holder.global_augmented_incumbent_score());

        /**
         * Update the memory.
         */
        m_memory = result.memory;

        /**
         * Preserve the number of iterations for the local search.
         */
        m_state.number_of_local_search_iterations = result.number_of_iterations;

        /**
         * Print the search summary.
         */
        utility::print_message(
            "Local search finished.",
            m_master_option.verbose >= option::verbose::Outer);
        this->print_current_incumbent_summary(m_master_option.verbose >=
                                              option::verbose::Outer);
    }

    /*************************************************************************/
    inline void run_tabu_search(void) {
        int iteration                                         = 0;
        int iteration_after_global_augmented_incumbent_update = 0;
        int iteration_after_no_update                         = 0;
        int iteration_after_relaxation                        = 0;

        int relaxation_count = 0;

        auto termination_status =
            tabu_search::TabuSearchTerminationStatus::ITERATION_OVER;

        int inital_tabu_tenure =
            m_master_option.tabu_search.initial_tabu_tenure;
        double pruning_rate_threshold =
            m_master_option.tabu_search.pruning_rate_threshold;
        int number_of_initial_modification = 0;
        int iteration_max = m_master_option.tabu_search.iteration_max;

        double                  current_intensity  = m_memory.intensity();
        [[maybe_unused]] double previous_intensity = 0.0;

        double current_intensity_before_relaxation  = 1.0;
        double previous_intensity_before_relaxation = 1.0;

        double penalty_coefficient_relaxing_rate =
            m_master_option.penalty_coefficient_relaxing_rate;
        double penalty_coefficient_tightening_rate =
            m_master_option.penalty_coefficient_tightening_rate;

        bool is_penalty_coefficient_exceed_initial_value = false;

        int employing_local_augmented_solution_count  = 0;
        int employing_global_augmented_solution_count = 0;
        int employing_previous_solution_count         = 0;

        TabuSearchTrendLogger logger;

        if (m_master_option.is_enabled_write_trend) {
            logger.setup("trend.txt");
            logger.write_instance_info(m_model_ptr->name(),
                                       m_model_ptr->number_of_variables(),
                                       m_model_ptr->number_of_constraints());
            logger.write_header();
        }

        auto improvability_screening_mode =
            m_master_option.improvability_screening_mode;
        if (improvability_screening_mode ==
            option::improvability_screening_mode::Automatic) {
            improvability_screening_mode =
                option::improvability_screening_mode::Intensive;
        }

        while (!m_state.is_terminated) {
            utility::print_single_line(m_master_option.verbose ==
                                       option::verbose::Outer);
            /**
             *  Check the terminating condition.
             */
            double elapsed_time = m_time_keeper.clock();
            if (elapsed_time > m_master_option.time_max) {
                utility::print_message(
                    "Outer loop was terminated because of time-over (" +
                        utility::to_string(elapsed_time, "%.3f") + "sec).",
                    m_master_option.verbose >= option::verbose::Outer);
                m_state.is_terminated = true;
            } else if (iteration >= m_master_option.iteration_max) {
                utility::print_message(
                    "Outer loop was terminated because of iteration limit (" +
                        utility::to_string(iteration, "%d") + " iterations).",
                    m_master_option.verbose >= option::verbose::Outer);
                m_state.is_terminated = true;
            } else if (m_incumbent_holder.feasible_incumbent_objective() <=
                       m_master_option.target_objective_value) {
                utility::print_message(
                    "Outer loop was terminated because of feasible objective "
                    "reaches the target limit (" +
                        utility::to_string(iteration, "%d") + " iterations).",
                    m_master_option.verbose >= option::verbose::Outer);
                m_state.is_terminated = true;
            } else if (iteration > 0 &&
                       termination_status ==
                           tabu_search::TabuSearchTerminationStatus::OPTIMAL) {
                utility::print_message(
                    "Outer loop was terminated because an optimal solution was "
                    "found.",
                    m_master_option.verbose >= option::verbose::Outer);
                m_state.is_terminated = true;
            }

            if (m_state.is_terminated) {
                break;
            }

            /**
             * Prepare an option object for tabu search.
             */
            option::Option option = m_master_option;

            option.improvability_screening_mode = improvability_screening_mode;
            option.tabu_search.iteration_max    = iteration_max;
            option.tabu_search.time_offset      = elapsed_time;
            option.tabu_search.seed += iteration;
            option.tabu_search.number_of_initial_modification =
                number_of_initial_modification;
            option.tabu_search.initial_tabu_tenure    = inital_tabu_tenure;
            option.tabu_search.pruning_rate_threshold = pruning_rate_threshold;

            /**
             * Prepare the initial variable values.
             */
            auto initial_variable_value_proxies =
                m_state.current_solution.variable_value_proxies;

            /**
             * Run the tabu search.
             */
            auto result = tabu_search::solve(m_model_ptr,                     //
                                             option,                          //
                                             initial_variable_value_proxies,  //
                                             m_incumbent_holder,              //
                                             m_memory);

            /**
             * Update the global augmented incumbent solution if it was improved
             * by the tabu search.
             */
            m_state.update_status = m_incumbent_holder.try_update_incumbent(
                result.incumbent_holder.global_augmented_incumbent_solution(),
                result.incumbent_holder.global_augmented_incumbent_score());

            /**
             * Update the iteration after global augmented incumbent update.
             */
            if (m_state.update_status &
                solution::IncumbentHolderConstant::
                    STATUS_GLOBAL_AUGMENTED_INCUMBENT_UPDATE) {
                /**
                 * NOTE: This case includes feasible incumbent and (not
                 * feasible) global augmented incumbent update.
                 */
                iteration_after_global_augmented_incumbent_update = 0;
            } else {
                iteration_after_global_augmented_incumbent_update++;
            }

            /**
             * Update variable bounds.
             */
            if (m_master_option.is_enabled_presolve &&
                (m_state.update_status &
                 solution::IncumbentHolderConstant::
                     STATUS_FEASIBLE_INCUMBENT_UPDATE)) {
                auto objective =
                    result.incumbent_holder.feasible_incumbent_solution()
                        .objective;
                auto number_of_newly_fixed_variables =
                    m_model_ptr->update_variable_bounds(
                        objective,
                        m_master_option.verbose >= option::verbose::Outer);

                /**
                 * If there is new fixed variable, setup the variable category
                 * and the binary/integer neighborhood again.
                 */
                if (number_of_newly_fixed_variables > 0) {
                    m_model_ptr->categorize_variables();
                    m_model_ptr->neighborhood().binary().setup(
                        m_model_ptr->variable_reference().binary_variable_ptrs);
                    m_model_ptr->neighborhood().integer().setup(
                        m_model_ptr->variable_reference()
                            .integer_variable_ptrs);
                }
            }

            /**
             * Update the memory.
             */
            m_memory = result.memory;

            /**
             * Update the intensity.
             */
            previous_intensity = current_intensity;
            current_intensity  = m_memory.intensity();

            /**
             * Update the termination status.
             */
            termination_status = result.termination_status;

            /**
             * Update the current solution which is employed as the initial
             * solution of the next loop. Whether the local penalty coefficients
             * should be relaxed or tightened, is also determined.
             */
            m_state.previous_solution       = m_state.current_solution;
            m_state.previous_solution_score = m_state.current_solution_score;

            /**
             * Prepare variables for control initial solution, penalty
             * coefficients, initial modification, etc.
             */
            auto result_local_augmented_incumbent_solution =
                result.incumbent_holder.local_augmented_incumbent_solution();
            auto result_global_augmented_incumbent_solution =
                result.incumbent_holder.global_augmented_incumbent_solution();

            auto result_local_augmented_incumbent_objective =
                result.incumbent_holder.local_augmented_incumbent_objective();
            auto result_global_augmented_incumbent_objective =
                result.incumbent_holder.global_augmented_incumbent_objective();

            auto result_local_augmented_incumbent_score =
                result.incumbent_holder.local_augmented_incumbent_score();
            auto result_global_augmented_incumbent_score =
                result.incumbent_holder.global_augmented_incumbent_score();

            bool employing_local_augmented_solution_flag  = false;
            bool employing_global_augmented_solution_flag = false;
            bool employing_previous_solution_flag         = false;

            bool is_enabled_penalty_coefficient_tightening = false;
            bool is_enabled_penalty_coefficient_relaxing   = false;
            bool is_enabled_forcibly_initial_modification  = false;

            bool penalty_coefficient_reset_flag = false;

            /**
             * "Stagnation" refers to when the iteration after global augmented
             * incumbent update is not less than the constant
             * STAGNATION_THRESHOLD.
             */
            constexpr int STAGNATION_THRESHOLD = 80;

            bool is_infeasible_stagnation =
                !m_incumbent_holder.is_found_feasible_solution() &&
                iteration_after_global_augmented_incumbent_update >=
                    STAGNATION_THRESHOLD;

            /**
             * "Improved" refers to when any of the following conditions are
             * satisfied:
             * - Objective function value is improved from the previous one.
             * - Total penalty is decreased from the previous one.
             */
            bool is_improved =
                (result_local_augmented_incumbent_score.objective <
                 m_state.previous_solution_score.objective) ||
                (result_local_augmented_incumbent_score.global_penalty <
                 m_state.previous_solution_score.global_penalty);

            /**
             * If the improvability_screening_mode was set to "Automatic",
             * determine the mode according to the search status so far.
             */
            if (m_master_option.improvability_screening_mode ==
                option::improvability_screening_mode::Automatic) {
                if (result.termination_status ==
                    tabu_search::TabuSearchTerminationStatus::NO_MOVE) {
                    improvability_screening_mode =
                        option::improvability_screening_mode::Soft;
                } else if (result.total_update_status &
                           solution::IncumbentHolderConstant::
                               STATUS_GLOBAL_AUGMENTED_INCUMBENT_UPDATE) {
                    /**
                     * If the incumbent solution was updated in the last loop,
                     * the improvability screening mode is set to "Intensive" to
                     * search better solutions by intensive search.
                     */
                    improvability_screening_mode =
                        option::improvability_screening_mode::Intensive;
                } else if (result.is_few_permissible_neighborhood) {
                    /**
                     * If the last loop encountered a situation where there is
                     * no permissible solution, the improvability screening mode
                     * is set to "Soft" for search diversity.
                     */
                    improvability_screening_mode =
                        option::improvability_screening_mode::Soft;
                } else if (!result.is_found_new_feasible_solution) {
                    /**
                     * If the last loop failed to find any feasible solution,
                     * the improvability screening mode is set to "Aggressive"
                     * or "Intensive" to prioritize the search for feasible
                     * solutions.
                     */
                    if (is_infeasible_stagnation) {
                        if (relaxation_count % 2 == 0) {
                            improvability_screening_mode =
                                option::improvability_screening_mode::Intensive;
                        } else {
                            improvability_screening_mode = option::
                                improvability_screening_mode::Aggressive;
                        }
                    } else {
                        improvability_screening_mode =
                            option::improvability_screening_mode::Aggressive;
                    }
                } else {
                    /**
                     * Otherwise, the improvability screening mode is set to
                     * "Soft" for search diversity.
                     */
                    improvability_screening_mode =
                        option::improvability_screening_mode::Soft;
                }
            }

            /**
             * Determine the initial solution for the next loop and flags to
             * tighten or relax the penalty coefficients.
             */

            /**
             *  NOTE: The gap can takes both of positive and negative value.
             */
            double gap = result_global_augmented_incumbent_objective -
                         result_local_augmented_incumbent_objective;

            if (result.total_update_status &
                solution::IncumbentHolderConstant::
                    STATUS_GLOBAL_AUGMENTED_INCUMBENT_UPDATE) {
                /**
                 * If the global incumbent solution was updated in the last
                 * loop, the global incumbent is employed as the initial
                 * solution for the next loop. The penalty coefficients are to
                 * be relaxed.
                 */
                employing_global_augmented_solution_flag = true;
                is_enabled_penalty_coefficient_relaxing  = true;

                iteration_after_no_update = 0;
            } else {
                if (result.total_update_status ==
                    solution::IncumbentHolderConstant::STATUS_NO_UPDATED) {
                    /**
                     * If the last loop failed to find any local/global
                     * incumbent solution, the global incumbent solution is
                     * employed as the initial solution for the next loop with
                     * some initial modifications. The penalty coefficients are
                     * to be relaxed after two consecutive search failures.
                     */
                    employing_global_augmented_solution_flag = true;
                    is_enabled_forcibly_initial_modification = true;

                    if (result_local_augmented_incumbent_score.is_feasible) {
                        is_enabled_penalty_coefficient_relaxing = true;
                        iteration_after_no_update               = 0;
                    } else {
                        if (iteration_after_no_update < 1) {
                            iteration_after_no_update++;
                        } else {
                            is_enabled_penalty_coefficient_relaxing = true;
                            iteration_after_no_update               = 0;
                        }
                    }
                } else {
                    /**
                     * If a local incumbent solution was updated the last loop,
                     * the initial solution for the next loop and flags to
                     * tighten or relax the penalty coefficients will be
                     * determined by complexed rules below.
                     */
                    iteration_after_no_update = 0;

                    double gap_tolerance = constant::EPSILON;

                    if (gap < gap_tolerance) {
                        /**
                         * The fact that the gap is negative implies that the
                         * obtained local incumbent solution is worse than the
                         * global incumbent solution. For this case, the initial
                         * solution for the next loop is reset by the global
                         * incumbent solution with some initial modifications.
                         * The penalty coefficients are to be relaxed or
                         * tightened according to the feasibility of the local
                         * incumbent solution.
                         */
                        employing_global_augmented_solution_flag = true;
                        is_enabled_forcibly_initial_modification = true;

                        if (result_local_augmented_incumbent_score
                                .is_feasible) {
                            is_enabled_penalty_coefficient_relaxing = true;
                        } else {
                            is_enabled_penalty_coefficient_tightening = true;
                        }
                    } else {
                        if (result_local_augmented_incumbent_score
                                .is_feasible) {
                            /**
                             * If the gap is positive and the local incumbent
                             * solution is feasible, the local incumbent
                             * solution is employed as the initial solution for
                             * the next loop. The penalty coefficients are to be
                             * relaxed.
                             */
                            employing_local_augmented_solution_flag = true;
                            is_enabled_penalty_coefficient_relaxing = true;

                        } else {
                            /**
                             * For the case that the gap is positive and the
                             * local incumbent solution is not feasible, the
                             * following rules will be applied: (1) If no
                             * feasible solution has been found in the previous
                             * loops:
                             * -> The obtained local incumbent solution is
                             * employed as the initial solution for the next
                             * loop. (2) If a feasible solution has been found
                             * in the previous loops: (2.1) If the obtained
                             * local incumbent solution improves the objective
                             * function value or global penalty than those of
                             * global incumbent solution:
                             * -> The obtained local incumbent solution is
                             * employed as the initial solution for the next
                             * loop. (2.2) Otherwise; if the obtained local
                             * incumbent solution does not improve either
                             * objective function value or global penalty:
                             * -> The previous initial solution is employed as
                             * the initial solution for the next loop.
                             *
                             * For all cases, penalty coefficients are to be
                             * tightened.
                             */
                            if (m_incumbent_holder
                                    .is_found_feasible_solution()) {
                                if (is_improved) {
                                    employing_local_augmented_solution_flag =
                                        true;
                                } else {
                                    employing_previous_solution_flag = true;
                                }
                            } else {
                                employing_local_augmented_solution_flag = true;
                            }
                            is_enabled_penalty_coefficient_tightening = true;
                        }
                    }
                }
            }

            /**
             * Set the current solution and its score according to the result of
             * the above process.
             */
            if (employing_global_augmented_solution_flag) {
                m_state.current_solution =
                    result_global_augmented_incumbent_solution;
                m_state.current_solution_score =
                    result_global_augmented_incumbent_score;
                employing_global_augmented_solution_count++;
            } else if (employing_local_augmented_solution_flag) {
                m_state.current_solution =
                    result_local_augmented_incumbent_solution;
                m_state.current_solution_score =
                    result_local_augmented_incumbent_score;
                employing_local_augmented_solution_count++;
            } else if (employing_previous_solution_flag) {
                m_state.current_solution = m_state.previous_solution;
                m_state.current_solution_score =
                    m_state.previous_solution_score;
                employing_previous_solution_count++;
            } else {
                throw std::logic_error(utility::format_error_location(
                    __FILE__, __LINE__, __func__,
                    "An error ocurred in determining the next initial "
                    "solution."));
            }

            /**
             * Additional processes for cases when the penalty coefficients are
             * relaxed.
             */
            if (is_enabled_penalty_coefficient_relaxing) {
                previous_intensity_before_relaxation =
                    current_intensity_before_relaxation;
                current_intensity_before_relaxation = current_intensity;

                constexpr double PENALTY_COEFFICIENT_RELAXING_RATE_MIN = 0.3;
                constexpr double PENALTY_COEFFICIENT_RELAXING_RATE_MAX =
                    1.0 - 1E-4;

                /**
                 * Adjust the penalty coefficient relaxing rate.
                 */
                if (is_infeasible_stagnation &&
                    current_intensity_before_relaxation >
                        previous_intensity_before_relaxation) {
                    /**
                     * If no feasible solution has been found, descrease penalty
                     * coefficient relaxing rate if lack of diversification is
                     * detected.
                     */
                    penalty_coefficient_relaxing_rate =
                        std::max(PENALTY_COEFFICIENT_RELAXING_RATE_MIN,
                                 penalty_coefficient_relaxing_rate * 0.5);
                } else if (employing_previous_solution_count >
                           std::max(employing_global_augmented_solution_count,
                                    employing_local_augmented_solution_count)) {
                    /**
                     * Increase penalty coefficient relaxing rate if previous
                     * solutions are most frequently employed as initial
                     * solutions, which indicates overrelaxation.
                     */
                    penalty_coefficient_relaxing_rate =
                        std::min(PENALTY_COEFFICIENT_RELAXING_RATE_MAX,
                                 sqrt(penalty_coefficient_relaxing_rate));
                } else {
                    /**
                     * Otherwise, draw back the penalty coefficient relaxing
                     * rate to the original value.
                     */
                    constexpr double LEARNING_RATE = 5E-2;
                    penalty_coefficient_relaxing_rate +=
                        LEARNING_RATE *
                        (m_master_option.penalty_coefficient_relaxing_rate -
                         penalty_coefficient_relaxing_rate);
                }

                iteration_after_relaxation = 0;
                relaxation_count++;
            } else {
                iteration_after_relaxation++;
            }

            /**
             * Additional processes for cases when the penalty coefficients are
             * tightened: Reset penalty coefficients if stagnation is detected.
             */
            if (is_enabled_penalty_coefficient_tightening) {
                constexpr int ITERATION_AFTER_RELAXATION_MAX = 30;

                if (is_infeasible_stagnation &&
                    is_penalty_coefficient_exceed_initial_value &&
                    ((iteration_after_relaxation + 1) %
                         ITERATION_AFTER_RELAXATION_MAX ==
                     0)) {
                    penalty_coefficient_reset_flag           = true;
                    is_enabled_forcibly_initial_modification = true;
                }
            }

            /**
             * This flag will be used to control the initial modification.
             */
            bool is_changed = m_state.current_solution.variable_value_proxies !=
                              m_state.previous_solution.variable_value_proxies;

            /**
             * Update the feasible solutions archive.
             */
            if (m_master_option.is_enabled_store_feasible_solutions &&
                result.feasible_solutions.size() > 0) {
                m_solution_archive.push(result.feasible_solutions);
            }

            /**
             * Update the local penalty coefficients.
             */
            if (penalty_coefficient_reset_flag) {
                /**
                 * If penalty_coefficient_reset_flag is true, the penalty
                 * coefficients will be reset.
                 */
                for (auto&& proxy : m_model_ptr->constraint_proxies()) {
                    for (auto&& constraint : proxy.flat_indexed_constraints()) {
                        constraint.reset_local_penalty_coefficient();
                    }
                }
                /**
                 * In cases the penalty coefficients are reset, they are within
                 * their initial values.
                 */
                is_penalty_coefficient_exceed_initial_value = false;
            } else if (is_enabled_penalty_coefficient_tightening) {
                /**
                 * Tighten the local penalty coefficients.
                 */
                double total_violation         = 0.0;
                double total_squared_violation = 0.0;
                for (const auto& proxy :
                     result_local_augmented_incumbent_solution
                         .violation_value_proxies) {
                    for (const auto& element : proxy.flat_indexed_values()) {
                        total_violation += element;
                        total_squared_violation += element * element;
                    }
                }

                double balance =
                    m_master_option.penalty_coefficient_updating_balance;
                double gap = result_global_augmented_incumbent_objective -
                             result_local_augmented_incumbent_objective;

                for (auto&& proxy : m_model_ptr->constraint_proxies()) {
                    const auto& constraint_values =
                        result_local_augmented_incumbent_solution
                            .constraint_value_proxies[proxy.index()]
                            .flat_indexed_values();

                    const auto& violation_values =
                        result_local_augmented_incumbent_solution
                            .violation_value_proxies[proxy.index()]
                            .flat_indexed_values();

                    for (auto&& constraint : proxy.flat_indexed_constraints()) {
                        double constraint_value =
                            constraint_values[constraint.flat_index()];
                        double violation_value =
                            violation_values[constraint.flat_index()];
                        double delta_penalty_coefficient_constant =
                            std::max(0.0, gap) / total_violation;
                        double delta_penalty_coefficient_proportional =
                            std::max(0.0, gap) / total_squared_violation *
                            violation_value;

                        double positive_part = std::max(constraint_value, 0.0);
                        double negative_part = std::max(-constraint_value, 0.0);
                        double delta_penalty_coefficient =
                            (balance * delta_penalty_coefficient_constant +
                             (1.0 - balance) *
                                 delta_penalty_coefficient_proportional);

                        if (constraint.is_less_or_equal() &&
                            positive_part > constant::EPSILON) {
                            constraint.local_penalty_coefficient_less() +=
                                penalty_coefficient_tightening_rate *
                                delta_penalty_coefficient;
                        } else if (constraint.is_greater_or_equal() &&
                                   negative_part > constant::EPSILON) {
                            constraint.local_penalty_coefficient_greater() +=
                                penalty_coefficient_tightening_rate *
                                delta_penalty_coefficient;
                        }
                    }

                    if (m_master_option
                            .is_enabled_grouping_penalty_coefficient) {
                        double max_local_penalty_coefficient = 0;
                        for (auto&& constraint :
                             proxy.flat_indexed_constraints()) {
                            max_local_penalty_coefficient = std::max(
                                max_local_penalty_coefficient,
                                constraint.local_penalty_coefficient_less());
                            max_local_penalty_coefficient = std::max(
                                max_local_penalty_coefficient,
                                constraint.local_penalty_coefficient_greater());
                        }
                        for (auto&& constraint :
                             proxy.flat_indexed_constraints()) {
                            constraint.local_penalty_coefficient_less() =
                                max_local_penalty_coefficient;
                            constraint.local_penalty_coefficient_greater() =
                                max_local_penalty_coefficient;
                        }
                    }

                    /**
                     * Penalty coefficients are bounded by the initial penalty
                     * coefficient specified in option.
                     */
                    for (auto&& constraint : proxy.flat_indexed_constraints()) {
                        constexpr double THRESHOLD_WITH_MARGIN = 1.0 + 1E-4;
                        /**
                         * NOTE: In cases where a value of the penalty
                         * coefficient is larger than its initial value, the
                         * adjustment of the penalty coefficient in subsequent
                         * iterations will not work properly. If such a
                         * situation is detected, the penalty coefficients will
                         * be reset based on an appropriate rule.
                         */
                        is_penalty_coefficient_exceed_initial_value |=
                            constraint.local_penalty_coefficient_less() >
                            m_master_option.initial_penalty_coefficient *
                                THRESHOLD_WITH_MARGIN;

                        is_penalty_coefficient_exceed_initial_value |=
                            constraint.local_penalty_coefficient_greater() >
                            m_master_option.initial_penalty_coefficient *
                                THRESHOLD_WITH_MARGIN;

                        constraint.local_penalty_coefficient_less() = std::min(
                            constraint.local_penalty_coefficient_less(),
                            m_master_option.initial_penalty_coefficient);
                        constraint.local_penalty_coefficient_greater() =
                            std::min(
                                constraint.local_penalty_coefficient_greater(),
                                m_master_option.initial_penalty_coefficient);
                    }
                }
            } else if (is_enabled_penalty_coefficient_relaxing) {
                /**
                 * Relax the local penalty coefficients of which
                 * corresponding constraints are satisfied.
                 */
                double corrected_penalty_coefficient_relaxing_rate =
                    penalty_coefficient_relaxing_rate;

                if (result.objective_constraint_rate > constant::EPSILON) {
                    if (result_local_augmented_incumbent_score.is_feasible) {
                        constexpr double MARGIN = 1.0;
                        corrected_penalty_coefficient_relaxing_rate =
                            std::min(penalty_coefficient_relaxing_rate,
                                     result.objective_constraint_rate * MARGIN);
                    }
                }

                for (auto&& proxy : m_model_ptr->constraint_proxies()) {
                    const auto& constraint_values =
                        m_state.current_solution
                            .constraint_value_proxies[proxy.index()]
                            .flat_indexed_values();

                    for (auto&& constraint : proxy.flat_indexed_constraints()) {
                        double constraint_value =
                            constraint_values[constraint.flat_index()];
                        double positive_part = std::max(constraint_value, 0.0);
                        double negative_part = std::max(-constraint_value, 0.0);

                        if (constraint.is_less_or_equal() &&
                            positive_part < constant::EPSILON) {
                            constraint.local_penalty_coefficient_less() *=
                                corrected_penalty_coefficient_relaxing_rate;
                        }
                        if (constraint.is_greater_or_equal() &&
                            negative_part < constant::EPSILON) {
                            constraint.local_penalty_coefficient_greater() *=
                                corrected_penalty_coefficient_relaxing_rate;
                        }
                    }
                }
                /**
                 * In cases the penalty coefficients are reset, they are within
                 * their initial values.
                 */
                is_penalty_coefficient_exceed_initial_value = false;
            }

            /**
             * Update the initial tabu tenure for the next loop.
             */
            if (m_master_option.tabu_search
                    .is_enabled_automatic_tabu_tenure_adjustment) {
                if (result.total_update_status &
                    solution::IncumbentHolderConstant::
                        STATUS_GLOBAL_AUGMENTED_INCUMBENT_UPDATE) {
                    inital_tabu_tenure = std::min(
                        m_master_option.tabu_search.initial_tabu_tenure,
                        m_model_ptr->number_of_mutable_variables());
                } else if (result.total_update_status ==
                           solution::IncumbentHolderConstant::
                               STATUS_NO_UPDATED) {
                    inital_tabu_tenure = std::max(
                        option.tabu_search.initial_tabu_tenure - 1,
                        std::min(
                            m_master_option.tabu_search.initial_tabu_tenure,
                            m_model_ptr->number_of_mutable_variables()));
                } else if (result.tabu_tenure >
                           option.tabu_search.initial_tabu_tenure) {
                    inital_tabu_tenure =
                        std::min(option.tabu_search.initial_tabu_tenure + 1,
                                 m_model_ptr->number_of_mutable_variables());

                } else {
                    inital_tabu_tenure = std::max(
                        option.tabu_search.initial_tabu_tenure - 1,
                        std::min(
                            m_master_option.tabu_search.initial_tabu_tenure,
                            m_model_ptr->number_of_mutable_variables()));
                }
            } else {
                inital_tabu_tenure =
                    m_master_option.tabu_search.initial_tabu_tenure;
            }

            /**
             * Update the number of initial modification for the next loop.
             */
            if (result.total_update_status &
                solution::IncumbentHolderConstant::
                    STATUS_FEASIBLE_INCUMBENT_UPDATE) {
                number_of_initial_modification = 0;
            } else if (result.total_update_status &
                       solution::IncumbentHolderConstant::
                           STATUS_GLOBAL_AUGMENTED_INCUMBENT_UPDATE) {
                number_of_initial_modification = 0;
            } else {
                if (m_master_option.tabu_search
                        .is_enabled_initial_modification &&
                    (!is_changed || is_enabled_forcibly_initial_modification)) {
                    int nominal_number_of_initial_modification  //
                        = static_cast<int>(
                            std::floor(m_master_option.tabu_search
                                           .initial_modification_fixed_rate *
                                       inital_tabu_tenure));

                    int initial_modification_random_width = static_cast<int>(
                        option.tabu_search.initial_modification_randomize_rate *
                        nominal_number_of_initial_modification);

                    int number_of_initial_modification_temp =
                        nominal_number_of_initial_modification;
                    if (initial_modification_random_width > 0) {
                        number_of_initial_modification_temp +=
                            m_get_rand_mt() %
                                (2 * initial_modification_random_width) -
                            initial_modification_random_width;
                    }

                    number_of_initial_modification =
                        std::max(1, number_of_initial_modification_temp);
                } else {
                    number_of_initial_modification = 0;
                }
            }

            /**
             * Update the maximum number of iterations and pruning rate
             * threshold for the next loop.
             */
            if (m_master_option.tabu_search
                    .is_enabled_automatic_iteration_adjustment &&
                result.number_of_iterations ==
                    option.tabu_search.iteration_max) {
                int iteration_max_temp = 0;
                if (result.total_update_status &
                    solution::IncumbentHolderConstant::
                        STATUS_GLOBAL_AUGMENTED_INCUMBENT_UPDATE) {
                    iteration_max_temp  //
                        = static_cast<int>(ceil(
                            result
                                .last_local_augmented_incumbent_update_iteration *
                            m_master_option.tabu_search
                                .iteration_increase_rate));

                } else {
                    iteration_max_temp  //
                        = static_cast<int>(
                            ceil(option.tabu_search.iteration_max *
                                 m_master_option.tabu_search
                                     .iteration_increase_rate));
                }
                iteration_max =
                    std::max(m_master_option.tabu_search.initial_tabu_tenure,
                             std::min(m_master_option.tabu_search.iteration_max,
                                      iteration_max_temp));

                if (iteration_max ==
                    m_master_option.tabu_search.iteration_max) {
                    pruning_rate_threshold =
                        m_master_option.tabu_search.pruning_rate_threshold;
                } else {
                    pruning_rate_threshold = 1.0;
                }
            }

            /**
             * Reset chain moves if the global augmented objective was updated.
             */
            if (result.total_update_status &
                solution::IncumbentHolderConstant::
                    STATUS_GLOBAL_AUGMENTED_INCUMBENT_UPDATE) {
                if (option.is_enabled_chain_move) {
                    m_model_ptr->neighborhood().chain().clear_moves();
                }
            }

            /**
             * Update the neighborhood moves to be employed for the next loop.
             */
            bool is_activated_special_neighborhood_move   = false;
            bool is_deactivated_special_neighborhood_move = false;

            if (result.total_update_status &
                solution::IncumbentHolderConstant::
                    STATUS_GLOBAL_AUGMENTED_INCUMBENT_UPDATE) {
                /**
                 * Disable the special neighborhood moves if the incumbent was
                 * updated.
                 */
                /// Aggregation
                if (m_master_option.is_enabled_aggregation_move) {
                    if (m_model_ptr->neighborhood()
                            .aggregation()
                            .is_enabled()) {
                        m_model_ptr->neighborhood().aggregation().disable();
                        is_deactivated_special_neighborhood_move = true;
                    }
                }

                /// Precedence
                if (m_master_option.is_enabled_precedence_move) {
                    if (m_model_ptr->neighborhood().precedence().is_enabled()) {
                        m_model_ptr->neighborhood().precedence().disable();
                        is_deactivated_special_neighborhood_move = true;
                    }
                }

                /// Variable Bound
                if (m_master_option.is_enabled_variable_bound_move) {
                    if (m_model_ptr->neighborhood()
                            .variable_bound()
                            .is_enabled()) {
                        m_model_ptr->neighborhood().variable_bound().disable();
                        is_deactivated_special_neighborhood_move = true;
                    }
                }

                /// Chain
                if (m_master_option.is_enabled_chain_move) {
                    if (m_model_ptr->neighborhood().chain().is_enabled()) {
                        m_model_ptr->neighborhood().chain().disable();
                        is_deactivated_special_neighborhood_move = true;
                    }
                }

                /// Two Flip
                if (m_master_option.is_enabled_two_flip_move &&
                    m_model_ptr->flippable_variable_ptr_pairs().size() > 0) {
                    m_model_ptr->neighborhood().two_flip().disable();
                    is_deactivated_special_neighborhood_move = true;
                }
            } else {
                /**
                 * Enable the special neighborhood moves if the incumbent was
                 * not updated.
                 */
                if (result.number_of_iterations ==
                    m_master_option.tabu_search.iteration_max) {
                    /// Aggregation
                    if (m_master_option.is_enabled_aggregation_move) {
                        if (!m_model_ptr->neighborhood()
                                 .aggregation()
                                 .is_enabled()) {
                            m_model_ptr->neighborhood().aggregation().enable();
                            is_activated_special_neighborhood_move = true;
                        }
                    }

                    /// Precedence
                    if (m_master_option.is_enabled_precedence_move) {
                        if (!m_model_ptr->neighborhood()
                                 .precedence()
                                 .is_enabled()) {
                            m_model_ptr->neighborhood().precedence().enable();
                            is_activated_special_neighborhood_move = true;
                        }
                    }

                    /// Variable Bound
                    if (m_master_option.is_enabled_variable_bound_move) {
                        if (!m_model_ptr->neighborhood()
                                 .variable_bound()
                                 .is_enabled()) {
                            m_model_ptr->neighborhood()
                                .variable_bound()
                                .enable();
                            is_activated_special_neighborhood_move = true;
                        }
                    }

                    /// Chain
                    if (m_master_option.is_enabled_chain_move) {
                        if (!m_model_ptr->neighborhood().chain().is_enabled()) {
                            m_model_ptr->neighborhood().chain().enable();
                            is_activated_special_neighborhood_move = true;
                        }
                    }

                    /// Two Flip
                    if (m_master_option.is_enabled_two_flip_move &&
                        m_model_ptr->flippable_variable_ptr_pairs().size() >
                            0) {
                        if (!m_model_ptr->neighborhood()
                                 .two_flip()
                                 .is_enabled()) {
                            m_model_ptr->neighborhood().two_flip().enable();
                            is_activated_special_neighborhood_move = true;
                        }
                    }
                }
            }

            /**
             * Sort and deduplicate registered chain moves.
             */
            if (m_model_ptr->neighborhood().chain().is_enabled() &&
                m_master_option.chain_move_capacity > 0) {
                m_model_ptr->neighborhood().chain().sort_moves();
                m_model_ptr->neighborhood().chain().deduplicate_moves();
            }

            /**
             * Reduce the registered chain moves.
             */
            if (static_cast<int>(
                    m_model_ptr->neighborhood().chain().moves().size()) >
                m_master_option.chain_move_capacity) {
                switch (m_master_option.chain_move_reduce_mode) {
                    case option::chain_move_reduce_mode::OverlapRate: {
                        m_model_ptr->neighborhood().chain().reduce_moves(
                            m_master_option.chain_move_capacity);
                        break;
                    }
                    case option::chain_move_reduce_mode::Shuffle: {
                        m_model_ptr->neighborhood().chain().shuffle_moves(
                            &m_get_rand_mt);
                        m_model_ptr->neighborhood().chain().reduce_moves(
                            m_master_option.chain_move_capacity);
                        break;
                    }
                    default: {
                        throw std::logic_error(utility::format_error_location(
                            __FILE__, __LINE__, __func__,
                            "The specified Chain move reduce mode is "
                            "invalid."));
                    }
                }
            }

            /**
             * Reset the availability of special neighborhood moves.
             */
            if (m_model_ptr->neighborhood()
                    .is_enabled_special_neighborhood_move()) {
                m_model_ptr->neighborhood()
                    .reset_special_neighborhood_moves_availability();
            }

            /**
             * Preserve the number of iterations of the previous loop.
             */
            m_state.number_of_tabu_search_iterations +=
                result.number_of_iterations;
            m_state.number_of_tabu_search_loops++;

            /**
             * Update the elapsed time.
             */
            elapsed_time = m_time_keeper.clock();

            /**
             * Print the summary.
             */
            utility::print_message(
                "Tabu search loop (" + std::to_string(iteration + 1) + "/" +
                    std::to_string(m_master_option.iteration_max) +
                    ") finished.",
                m_master_option.verbose >= option::verbose::Outer);

            this->print_current_incumbent_summary(m_master_option.verbose >=
                                                  option::verbose::Outer);

            /**
             * Print the optimization status of the previous tabu search loop.
             */
            if (result.total_update_status &
                solution::IncumbentHolderConstant::
                    STATUS_FEASIBLE_INCUMBENT_UPDATE) {
                utility::print_message(
                    "Feasible incumbent objective was updated. ",
                    m_master_option.verbose >= option::verbose::Outer);

            } else if (result.total_update_status &
                       solution::IncumbentHolderConstant::
                           STATUS_GLOBAL_AUGMENTED_INCUMBENT_UPDATE) {
                utility::print_message(
                    "Global incumbent objective was updated. ",
                    m_master_option.verbose >= option::verbose::Outer);
            } else {
                utility::print_message(
                    "Incumbent objective was not updated.",
                    m_master_option.verbose >= option::verbose::Outer);
            }

            /**
             * Print the search intensity.
             */
            utility::print_message(
                "Historical search intensity is " +
                    std::to_string(current_intensity) + ".",
                m_master_option.verbose >= option::verbose::Outer);

            /**
             * Print the number of found feasible solutions.
             */
            if (m_master_option.is_enabled_store_feasible_solutions) {
                utility::print_message(
                    "The number of feasible solutions found so far is " +
                        std::to_string(m_solution_archive.solutions().size()) +
                        ".",
                    m_master_option.verbose >= option::verbose::Outer);
            }

            /**
             * Print the number of violative constraints.
             */
            if (!m_state.current_solution.is_feasible) {
                int number_of_violative_constraints = 0;
                /**
                 * Due to the slow speed of standard output in the Windows DOS,
                 * printing all violations will affect performance. To avoid
                 * this problem, the maximum number of violations to be printed
                 * is 20.
                 */
                constexpr int MAX_NUMBER_OF_PRINT_ITEMS = 20;

                utility::print_message(
                    "The current solution does not satisfy the following "
                    "constraints:",
                    m_master_option.verbose >= option::verbose::Outer);

                for (const auto& proxy :
                     m_state.current_solution.violation_value_proxies) {
                    const auto& values      = proxy.flat_indexed_values();
                    const auto& names       = proxy.flat_indexed_names();
                    const int   VALUES_SIZE = values.size();

                    for (auto i = 0; i < VALUES_SIZE; i++) {
                        if (values[i] > 0) {
                            number_of_violative_constraints++;
                            if (number_of_violative_constraints <=
                                MAX_NUMBER_OF_PRINT_ITEMS) {
                                utility::print_info(
                                    " -- " + names[i] + " (violation: " +
                                        std::to_string(values[i]) + ")",
                                    m_master_option.verbose >=
                                        option::verbose::Outer);
                            }
                        }
                    }
                }
                if (number_of_violative_constraints >
                    MAX_NUMBER_OF_PRINT_ITEMS) {
                    utility::print_info(
                        "and much more...",
                        m_master_option.verbose >= option::verbose::Outer);
                }

                utility::print_message(
                    "There are " +
                        std::to_string(number_of_violative_constraints) +
                        " violative constraints.",
                    m_master_option.verbose >= option::verbose::Outer);
            }

            /**
             * Print message if the penalty coefficients were changed.
             */
            if (penalty_coefficient_reset_flag) {
                utility::print_message(
                    "The penalty coefficients were reset due to search "
                    "stagnation.",
                    m_master_option.verbose >= option::verbose::Outer);
            } else if (is_enabled_penalty_coefficient_relaxing) {
                utility::print_message(  //
                    "The penalty coefficients were relaxed.",
                    m_master_option.verbose >= option::verbose::Outer);
            } else if (is_enabled_penalty_coefficient_tightening) {
                utility::print_message(  //
                    "The penalty coefficients were tightened.",
                    m_master_option.verbose >= option::verbose::Outer);
            }
            utility::print_message(  //
                "The penalty coefficients relaxing rate is " +
                    std::to_string(penalty_coefficient_relaxing_rate) + ".",
                m_master_option.verbose >= option::verbose::Outer);
            utility::print_message(  //
                "The penalty coefficients tightening rate is " +
                    std::to_string(penalty_coefficient_tightening_rate) + ".",
                m_master_option.verbose >= option::verbose::Outer);

            /**
             * Print the initial tabu tenure for the next loop.
             */
            utility::print_message(
                "The tabu tenure for the next loop was set to " +
                    std::to_string(inital_tabu_tenure) + ".",
                m_master_option.verbose >= option::verbose::Outer);

            /**
             * Print the improvability screening_mode
             */
            switch (improvability_screening_mode) {
                case option::improvability_screening_mode::Off: {
                    utility::print_message(
                        "No improvability screening will be applied for the "
                        "next loop.",
                        m_master_option.verbose >= option::verbose::Outer);
                    break;
                }
                case option::improvability_screening_mode::Soft: {
                    utility::print_message(
                        "The soft improvability screening will be applied in "
                        "the next loop.",
                        m_master_option.verbose >= option::verbose::Outer);
                    break;
                }
                case option::improvability_screening_mode::Aggressive: {
                    utility::print_message(
                        "The aggressive improvability screening will be "
                        "applied in the next loop.",
                        m_master_option.verbose >= option::verbose::Outer);
                    break;
                }
                case option::improvability_screening_mode::Intensive: {
                    utility::print_message(
                        "The intensive improvability screening will be applied "
                        "in the next loop.",
                        m_master_option.verbose >= option::verbose::Outer);
                    break;
                }
                default: {
                    throw std::logic_error(utility::format_error_location(
                        __FILE__, __LINE__, __func__,
                        "The specified improvability screening mode is "
                        "invalid."));
                }
            }

            /**
             * Print the initial solution for the next loop.
             */
            if (employing_global_augmented_solution_flag) {
                utility::print_message(
                    "The next loop will start from the global incumbent "
                    "solution.",
                    m_master_option.verbose >= option::verbose::Outer);
            } else if (employing_local_augmented_solution_flag) {
                utility::print_message(
                    "The next loop will start from the local incumbent "
                    "solution found in the previous loop.",
                    m_master_option.verbose >= option::verbose::Outer);
            } else if (employing_previous_solution_flag) {
                utility::print_message(
                    "The next loop will start from the same initial solution "
                    "of the previous loop.",
                    m_master_option.verbose >= option::verbose::Outer);
            }

            /**
             * Print the number of initial modification for the next
             * loop.
             */
            if (number_of_initial_modification > 0) {
                utility::print_message(
                    "For the initial " +
                        std::to_string(number_of_initial_modification) +
                        " iterations in the next loop, the solution will be "
                        "randomly updated to escape from the local optimum.",
                    m_master_option.verbose >= option::verbose::Outer);
            }

            /**
             * Print the number of iterations for the next loop.
             */
            if (m_master_option.tabu_search
                    .is_enabled_automatic_iteration_adjustment) {
                utility::print_message(
                    "The maximum number of iterations for the next loop was "
                    "set to " +
                        std::to_string(iteration_max) + ".",
                    m_master_option.verbose >= option::verbose::Outer);
            }

            /**
             * Print a message of the special neighborhood moves
             * activation/deactivation.
             */
            if (is_deactivated_special_neighborhood_move &&
                m_state.has_special_neighborhood_moves) {
                utility::print_message(
                    "Special neighborhood moves were disabled.",
                    m_master_option.verbose >= option::verbose::Outer);
            }

            if (is_activated_special_neighborhood_move &&
                m_state.has_special_neighborhood_moves) {
                utility::print_message(
                    "Special neighborhood moves were enabled.",
                    m_master_option.verbose >= option::verbose::Outer);
            }

            /**
             * Print the number of the stored chain moves.
             */
            if (m_model_ptr->neighborhood().chain().is_enabled()) {
                utility::print_message(
                    "There are " +
                        std::to_string(m_model_ptr->neighborhood()
                                           .chain()
                                           .moves()
                                           .size()) +
                        " stored chain moves.",
                    m_master_option.verbose >= option::verbose::Outer);
            }

            /**
             * Logging.
             */
            if (m_master_option.is_enabled_write_trend) {
                auto& global_incumbent =
                    m_incumbent_holder.global_augmented_incumbent_score();
                auto& local_incumbent =
                    result.incumbent_holder.local_augmented_incumbent_score();
                logger.write_log(                                       //
                    iteration,                                          // 0
                    elapsed_time,                                       // 1
                    local_incumbent.objective,                          // 2
                    local_incumbent.total_violation,                    // 3
                    global_incumbent.objective,                         // 4
                    global_incumbent.total_violation,                   // 5
                    current_intensity,                                  // 6
                    m_state.update_status,                              // 7
                    employing_local_augmented_solution_flag,            // 8
                    employing_global_augmented_solution_flag,           // 9
                    employing_previous_solution_flag,                   // 10
                    is_enabled_penalty_coefficient_relaxing,            // 11
                    is_enabled_penalty_coefficient_tightening,          // 12
                    penalty_coefficient_reset_flag,                     // 13
                    penalty_coefficient_relaxing_rate,                  // 14
                    penalty_coefficient_tightening_rate,                // 15
                    is_enabled_forcibly_initial_modification,           // 16
                    option.tabu_search.number_of_initial_modification,  // 17
                    option.tabu_search.initial_tabu_tenure);            // 18
            }

            m_model_ptr->callback(&option, &m_incumbent_holder);
            iteration++;
        }
    }

    /*************************************************************************/
    inline Result<T_Variable, T_Expression> solve(void) {
        /**
         * Preprocessing; setup the model and the solver.
         */
        this->preprocess();

        /**
         * Start optimization.
         */
        utility::print_single_line(  //
            m_master_option.verbose >= option::verbose::Outer);

        utility::print_message(  //
            "Optimization starts.",
            m_master_option.verbose >= option::verbose::Outer);

        /**
         * Solve Lagrange dual to obtain a better initial solution (Optional).
         */
        if (m_master_option.is_enabled_lagrange_dual  //
            && !m_state.is_terminated) {
            this->run_lagrange_dual();
        }

        /**
         * Run a local search to improve the initial solution (optional).
         */
        if (m_master_option.is_enabled_local_search  //
            && !m_state.is_terminated) {
            this->run_local_search();
        }

        /**
         * Run tabu searches to find better solutions.
         */
        this->run_tabu_search();

        /**
         * Postprocessing; create the result object.
         */
        auto result = this->postprocess();

        return result;
    }

    /*************************************************************************/
    inline std::vector<
        presolver::FlippableVariablePair<T_Variable, T_Expression>>
    extract_flippable_variable_pairs(const int a_MINIMUM_COMMON_ELEMENT) {
        /**
         * Preprocessing; setup the model and the solver.
         */
        this->preprocess();

        /**
         * Extract flippable variable pairs
         */
        auto flippable_variable_pairs =
            presolver::extract_flippable_variable_pairs(
                m_model_ptr->constraint_reference().enabled_constraint_ptrs,
                a_MINIMUM_COMMON_ELEMENT,
                m_master_option.verbose >= option::verbose::Outer);
        return flippable_variable_pairs;
    }
};

/*****************************************************************************/
template <class T_Variable, class T_Expression>
inline Result<T_Variable, T_Expression> solve(
    model::Model<T_Variable, T_Expression>* a_model_ptr,  //
    const option::Option&                   a_OPTION) {
    Solver<T_Variable, T_Expression> solver(a_model_ptr, a_OPTION);
    auto                             result = solver.solve();

    return result;
}

/*****************************************************************************/
template <class T_Variable, class T_Expression>
inline Result<T_Variable, T_Expression> solve(
    model::Model<T_Variable, T_Expression>* a_model_ptr) {
    option::Option option;
    return solve(a_model_ptr, option);
}

/*****************************************************************************/
template <class T_Variable, class T_Expression>
inline std::vector<presolver::FlippableVariablePair<T_Variable, T_Expression>>
extract_flippable_variable_pairs(
    model::Model<T_Variable, T_Expression>* a_model_ptr,  //
    const option::Option& a_OPTION, const int a_MINIMUM_COMMON_ELEMENT) {
    Solver<T_Variable, T_Expression> solver(a_model_ptr, a_OPTION);

    auto flippable_variable_pairs =
        solver.extract_flippable_variable_pairs(a_MINIMUM_COMMON_ELEMENT);

    return flippable_variable_pairs;
}

}  // namespace solver
}  // namespace printemps

#endif
/*****************************************************************************/
// END
/*****************************************************************************/