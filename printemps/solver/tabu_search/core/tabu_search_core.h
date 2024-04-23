/*****************************************************************************/
// Copyright (c) 2020-2024 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_TABU_SEARCH_CORE_TABU_SEARCH_CORE_H__
#define PRINTEMPS_SOLVER_TABU_SEARCH_CORE_TABU_SEARCH_CORE_H__

#include "../../integer_step_size_adjuster.h"
#include "../../memory.h"
#include "tabu_search_core_move_score.h"
#include "tabu_search_core_move_evaluator.h"
#include "tabu_search_core_termination_status.h"
#include "tabu_search_core_state.h"
#include "tabu_search_core_state_manager.h"
#include "tabu_search_core_result.h"

namespace printemps::solver::tabu_search::core {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class TabuSearchCore {
   private:
    model::Model<T_Variable, T_Expression>*              m_model_ptr;
    GlobalState<T_Variable, T_Expression>*               m_global_state_ptr;
    solution::DenseSolution<T_Variable, T_Expression>    m_initial_solution;
    solution::IncumbentHolder<T_Variable, T_Expression>* m_incumbent_holder_ptr;
    option::Option                                       m_option;

    std::vector<solution::SparseSolution<T_Variable, T_Expression>>
        m_feasible_solutions;
    std::vector<solution::SparseSolution<T_Variable, T_Expression>>
        m_incumbent_solutions;

    TabuSearchCoreStateManager<T_Variable, T_Expression> m_state_manager;
    TabuSearchCoreResult<T_Variable, T_Expression>       m_result;
    std::mt19937                                         m_get_rand_mt;

    /*************************************************************************/
    inline void preprocess(void) {
        /**
         * Reset the local augmented incumbent.
         */
        m_global_state_ptr->incumbent_holder.reset_local_augmented_incumbent();

        /**
         * Reset the last update iterations.
         */
        m_global_state_ptr->memory.reset_last_update_iterations();

        /**
         * Reset the feasible solutions storage.
         */
        m_feasible_solutions.clear();

        /**
         * Reset the incumbent solutions storage.
         */
        m_incumbent_solutions.clear();

        /**
         * Prepare a random generator, which is used for shuffling moves.
         */
        m_get_rand_mt.seed(m_option.tabu_search.seed);

        /**
         * Initialize the solution and update the model.
         */
        m_model_ptr->import_solution(m_initial_solution);
        m_model_ptr->update();

        /**
         * Reset the variable improvability.
         */
        m_model_ptr->reset_variable_objective_improvabilities();
        m_model_ptr->reset_variable_feasibility_improvabilities();

        m_state_manager.setup(m_model_ptr, m_global_state_ptr, m_option);
    }

    /*************************************************************************/
    inline void postprocess(void) {
        /**
         * Prepare the result.
         */
        m_result = TabuSearchCoreResult<T_Variable, T_Expression>(
            m_state_manager.state(), m_option);
    }

    /*************************************************************************/
    inline bool satisfy_time_over_terminate_condition(void) {
        const auto& STATE = m_state_manager.state();

        if (STATE.elapsed_time > m_option.tabu_search.time_max) {
            m_state_manager.set_termination_status(
                TabuSearchCoreTerminationStatus::TIME_OVER);
            return true;
        }

        if (STATE.elapsed_time + m_option.tabu_search.time_offset >
            m_option.general.time_max) {
            m_state_manager.set_termination_status(
                TabuSearchCoreTerminationStatus::TIME_OVER);
            return true;
        }
        return false;
    }

    /*************************************************************************/
    inline bool satisfy_iteration_over_terminate_condition(void) {
        const auto& STATE = m_state_manager.state();

        if (STATE.iteration >= m_option.tabu_search.iteration_max) {
            m_state_manager.set_termination_status(
                TabuSearchCoreTerminationStatus::ITERATION_OVER);
            return true;
        }
        return false;
    }

    /*************************************************************************/
    inline bool satisfy_reach_target_terminate_condition(void) {
        if (m_global_state_ptr->incumbent_holder
                .feasible_incumbent_objective() <=
            m_option.general.target_objective_value) {
            m_state_manager.set_termination_status(
                TabuSearchCoreTerminationStatus::REACH_TARGET);
            return true;
        }
        return false;
    }

    /*************************************************************************/
    inline bool satisfy_early_stop_terminate_condition(void) {
        const auto& STATE = m_state_manager.state();

        if (STATE.number_of_ineffective_updates >
            m_option.tabu_search.pruning_rate_threshold *
                m_option.tabu_search.iteration_max) {
            m_state_manager.set_termination_status(
                TabuSearchCoreTerminationStatus::EARLY_STOP);
            return true;
        }
        return false;
    }

    /*************************************************************************/
    inline bool satisfy_optimal_or_no_move_terminate_condition(void) {
        const auto& STATE = m_state_manager.state();

        if (STATE.number_of_moves > 0) {
            if (fabs(m_global_state_ptr->incumbent_holder
                         .feasible_incumbent_objective() -
                     m_global_state_ptr->incumbent_holder.dual_bound()) <
                constant::EPSILON) {
                m_state_manager.set_termination_status(
                    TabuSearchCoreTerminationStatus::OPTIMAL);
                return true;
            } else {
                return false;
            }
        }

        if (m_model_ptr->is_linear() && m_model_ptr->is_feasible()) {
            /**
             * NOTE: If the current solution is feasible and there is no
             * improvable solution, the solution should be an optimum. It can
             * happen for decomp2 instance in MIPLIB 2017.
             */
            m_state_manager.set_termination_status(
                TabuSearchCoreTerminationStatus::OPTIMAL);
            for (const auto& variable_ptr :
                 m_model_ptr->variable_reference().variable_ptrs) {
                if (variable_ptr->is_objective_improvable()) {
                    m_state_manager.set_termination_status(
                        TabuSearchCoreTerminationStatus::NO_MOVE);
                    break;
                }
            }
            return true;
        } else {
            m_state_manager.set_termination_status(
                TabuSearchCoreTerminationStatus::NO_MOVE);
            return true;
        }

        return false;
    }

    /*************************************************************************/
    inline bool satisfy_penalty_coefficient_too_large_terminate_condition(
        const std::vector<solution::SolutionScore>& a_TRIAL_SOLUTION_SCORES) {
        const auto& STATE = m_state_manager.state();

        constexpr int    ITERATION_MIN = 10;
        constexpr double MARGIN        = 100.0;

        if (STATE.iteration <= ITERATION_MIN) {
            return false;
        }

        if (!STATE.current_solution_score.is_feasible) {
            return false;
        }

        double min_infeasible_local_penalty  = HUGE_VALF;
        bool   has_infeasible_trial_solution = false;

        for (const auto& score : a_TRIAL_SOLUTION_SCORES) {
            if (!score.is_feasible) {
                min_infeasible_local_penalty =
                    std::min(min_infeasible_local_penalty, score.local_penalty);
                has_infeasible_trial_solution = true;
            }
        }

        if (!has_infeasible_trial_solution) {
            return false;
        }

        const auto SCORE_PTR_PAIR = std::minmax_element(
            a_TRIAL_SOLUTION_SCORES.begin(), a_TRIAL_SOLUTION_SCORES.end(),
            [](const auto& a_FIRST, const auto& a_SECOND) {
                return a_FIRST.objective_improvement <
                       a_SECOND.objective_improvement;
            });

        const double MAX_OBJECTIVE_SENSITIVITY =
            std::max(SCORE_PTR_PAIR.second->objective_improvement,
                     -SCORE_PTR_PAIR.first->objective_improvement);

        if (MAX_OBJECTIVE_SENSITIVITY * MARGIN < min_infeasible_local_penalty) {
            m_state_manager.set_termination_status(
                TabuSearchCoreTerminationStatus::PENALTY_COEFFICIENT_TOO_LARGE);
            return true;
        }

        return false;
    }

    /*************************************************************************/
    inline void update_moves(utility::TimeKeeper* a_time_keeper_ptr) {
        const auto& STATE = m_state_manager.state();

        bool accept_all                    = true;
        bool accept_objective_improvable   = true;
        bool accept_feasibility_improvable = true;

        if (!m_model_ptr->is_linear() ||
            m_option.neighborhood.improvability_screening_mode ==
                option::improvability_screening_mode::Off) {
            m_model_ptr->neighborhood().update_moves(
                accept_all,                     //
                accept_objective_improvable,    //
                accept_feasibility_improvable,  //
                m_option.parallel.is_enabled_move_update_parallelization,
                m_option.parallel.number_of_threads_move_update);

            m_state_manager.set_number_of_moves(
                m_model_ptr->neighborhood().move_ptrs().size());
            return;
        }

        /**
         * If the option improvability_screening_mode is not Off, only
         * improvable moves will be generated.
         */
        if (STATE.iteration == 0) {
            m_model_ptr->update_variable_objective_improvabilities();
        } else {
            m_model_ptr->update_variable_objective_improvabilities(
                neighborhood::related_variable_ptrs_vector(STATE.current_move));
        }

        switch (m_option.neighborhood.improvability_screening_mode) {
            case option::improvability_screening_mode::Soft: {
                if (m_model_ptr->is_feasible()) {
                    accept_all                    = false;
                    accept_objective_improvable   = true;
                    accept_feasibility_improvable = false;
                } else {
                    m_model_ptr->reset_variable_feasibility_improvabilities();
                    m_model_ptr->update_variable_feasibility_improvabilities(
                        m_model_ptr->violative_constraint_ptrs());

                    accept_all                    = false;
                    accept_objective_improvable   = true;
                    accept_feasibility_improvable = true;
                }
                break;
            }
            case option::improvability_screening_mode::Aggressive: {
                if (m_model_ptr->is_feasible()) {
                    accept_all                    = false;
                    accept_objective_improvable   = true;
                    accept_feasibility_improvable = false;
                } else {
                    m_model_ptr->reset_variable_feasibility_improvabilities();
                    m_model_ptr->update_variable_feasibility_improvabilities(
                        m_model_ptr->violative_constraint_ptrs());

                    accept_all                    = false;
                    accept_objective_improvable   = false;
                    accept_feasibility_improvable = true;
                }
                break;
            }
            case option::improvability_screening_mode::Intensive: {
                if (m_model_ptr->is_feasible()) {
                    accept_all                    = false;
                    accept_objective_improvable   = true;
                    accept_feasibility_improvable = false;
                } else {
                    if (STATE.iteration == 0) {
                        m_model_ptr
                            ->reset_variable_feasibility_improvabilities();
                        m_model_ptr
                            ->update_variable_feasibility_improvabilities();
                    } else {
                        m_model_ptr->reset_variable_feasibility_improvabilities(
                            STATE.current_move.related_constraint_ptrs);
                        m_model_ptr
                            ->update_variable_feasibility_improvabilities(
                                STATE.current_move.related_constraint_ptrs);
                    }

                    accept_all                    = false;
                    accept_objective_improvable   = false;
                    accept_feasibility_improvable = true;
                }
                break;
            }
            default: {
                throw std::logic_error(utility::format_error_location(
                    __FILE__, __LINE__, __func__,
                    "The specified improvability screening mode is "
                    "invalid."));
            }
        }

        const double START_TIME = a_time_keeper_ptr->clock();
        m_model_ptr->neighborhood().update_moves(
            accept_all,                                                //
            accept_objective_improvable,                               //
            accept_feasibility_improvable,                             //
            m_option.parallel.is_enabled_move_update_parallelization,  //
            m_option.parallel.number_of_threads_move_update);
        const double END_TIME = a_time_keeper_ptr->clock();

        m_state_manager.update_move_update_statistics(
            m_model_ptr->neighborhood().number_of_updated_moves(),
            END_TIME - START_TIME);

        m_state_manager.set_number_of_moves(
            m_model_ptr->neighborhood().move_ptrs().size());
    }

    /*************************************************************************/
    inline void curtail_moves(void) {
        m_state_manager.set_number_of_moves(static_cast<int>(
            floor(m_option.tabu_search.move_preserve_rate *
                  m_model_ptr->neighborhood().move_ptrs().size())));
    }

    /*************************************************************************/
    inline std::pair<int, bool> select_move(
        const std::vector<double>&                  a_TOTAL_SCORES,
        const std::vector<TabuSearchCoreMoveScore>& a_TRIAL_MOVE_SCORES,
        const std::vector<solution::SolutionScore>& a_TRIAL_SOLUTION_SCORES) {
        const auto& STATE = m_state_manager.state();

        int  selected_index = 0;
        bool is_aspirated   = false;

        if (STATE.iteration <
            m_option.tabu_search.number_of_initial_modification) {
            /**
             * For diversification, the move for next solution will be
             * randomly selected for initial several iteration.
             */
            selected_index = m_get_rand_mt() % STATE.number_of_moves;
            is_aspirated   = false;
            return std::make_pair(selected_index, is_aspirated);
        }

        /**
         * The move for next solution will be determined by evaluations
         * of solutions and moves after the inital modifications.
         */
        selected_index = utility::argmin(a_TOTAL_SCORES);
        is_aspirated   = false;

        /**
         * A move which improves the augmented incumbent solution can be
         * accepted (optional).
         */
        if (!m_option.tabu_search.ignore_tabu_if_global_incumbent) {
            return std::make_pair(selected_index, is_aspirated);
        }

        const int ARGMIN_GLOBAL_AUGMENTED_OBJECTIVE =
            solution::argmin_index_global_augmented_objective(
                a_TRIAL_SOLUTION_SCORES);

        if (a_TRIAL_SOLUTION_SCORES[ARGMIN_GLOBAL_AUGMENTED_OBJECTIVE]
                    .global_augmented_objective +
                constant::EPSILON <
            m_global_state_ptr->incumbent_holder
                .global_augmented_incumbent_objective()) {
            selected_index = ARGMIN_GLOBAL_AUGMENTED_OBJECTIVE;
            if (!a_TRIAL_MOVE_SCORES[selected_index].is_permissible) {
                is_aspirated = true;
            }
        }
        return std::make_pair(selected_index, is_aspirated);
    }

    /*************************************************************************/
    inline void update_memory(
        const neighborhood::Move<T_Variable, T_Expression>* a_move_ptr) {
        const auto& STATE = m_state_manager.state();

        const int RANDOM_WIDTH =
            static_cast<int>(m_option.tabu_search.tabu_tenure_randomize_rate *
                             STATE.tabu_tenure);
        m_global_state_ptr->memory.update(*a_move_ptr,      //
                                          STATE.iteration,  //
                                          RANDOM_WIDTH,     //
                                          &m_get_rand_mt);
    }

    /*************************************************************************/
    inline void update_chain_moves(void) {
        auto& STATE = m_state_manager.state();

        if ((STATE.previous_move.sense == neighborhood::MoveSense::Binary &&
             STATE.current_move.sense == neighborhood::MoveSense::Binary &&
             STATE.previous_move.alterations.front().second !=
                 STATE.current_move.alterations.front().second) ||
            (STATE.previous_move.sense == neighborhood::MoveSense::Chain &&
             STATE.current_move.sense == neighborhood::MoveSense::Chain) ||
            (STATE.previous_move.sense == neighborhood::MoveSense::TwoFlip &&
             STATE.current_move.sense == neighborhood::MoveSense::TwoFlip)) {
            neighborhood::Move<T_Variable, T_Expression> chain_move;
            if (STATE.previous_move.alterations.front().first <
                STATE.current_move.alterations.front().first)
                chain_move = STATE.previous_move + STATE.current_move;
            else {
                chain_move = STATE.current_move + STATE.previous_move;
            }

            if (chain_move.overlap_rate >
                    m_option.neighborhood.chain_move_overlap_rate_threshold &&
                !neighborhood::has_duplicate_variable(chain_move)) {
                auto back_chain_move = chain_move;
                for (auto&& alteration : back_chain_move.alterations) {
                    alteration.second = 1 - alteration.second;
                }
                m_model_ptr->neighborhood().chain().register_move(chain_move);
                m_model_ptr->neighborhood().chain().register_move(
                    back_chain_move);
            }
        }
    }

    /*****************************************************************************/
    inline void print_table_header(const bool a_IS_ENABLED_PRINT) {
        if (!a_IS_ENABLED_PRINT) {
            return;
        }

        utility::print(
            "---------+------------------------+----------------------+--------"
            "--------------",
            true);
        utility::print(
            "Iteration| Number of Neighborhoods|   Current Solution   |"
            "  Incumbent Solution ",
            true);
        utility::print(
            "         |  All Feas. Perm. Impr. |   Objective (Viol.)  | "
            "  Objective (Viol.)  ",
            true);
        utility::print(
            "---------+------------------------+----------------------+--------"
            "--------------",
            true);
    }
    /*************************************************************************/
    inline void print_table_initial(const bool a_IS_ENABLED_PRINT) {
        if (!a_IS_ENABLED_PRINT) {
            return;
        }

        const auto& STATE = m_state_manager.state();
        const auto  SIGN  = m_model_ptr->sign();

        const auto& CURRENT_SOLUTION_SCORE = STATE.current_solution_score;
        const auto& INCUMBENT_SOLUTION_SCORE =
            m_global_state_ptr->incumbent_holder
                .global_augmented_incumbent_score();

        std::string color_current_feasible_begin   = "";
        std::string color_current_feasible_end     = "";
        std::string color_incumbent_feasible_begin = "";
        std::string color_incumbent_feasible_end   = "";

#ifdef _PRINTEMPS_STYLING
        if (CURRENT_SOLUTION_SCORE.is_feasible) {
            color_current_feasible_begin = constant::CYAN;
            color_current_feasible_end   = constant::NO_COLOR;
        }

        if (INCUMBENT_SOLUTION_SCORE.is_feasible) {
            color_incumbent_feasible_begin = constant::CYAN;
            color_incumbent_feasible_end   = constant::NO_COLOR;
        }
#endif

        std::printf(
            " INITIAL |          -           - | %9.2e %s(%8.2e)%s | %9.2e "
            "%s(%8.2e)%s\n",
            CURRENT_SOLUTION_SCORE.objective * SIGN,
            color_current_feasible_begin.c_str(),
            CURRENT_SOLUTION_SCORE.is_feasible
                ? 0.0
                : CURRENT_SOLUTION_SCORE.total_violation,  //
            color_current_feasible_end.c_str(),
            INCUMBENT_SOLUTION_SCORE.objective * SIGN,
            color_incumbent_feasible_begin.c_str(),
            INCUMBENT_SOLUTION_SCORE.total_violation * SIGN,
            color_incumbent_feasible_end.c_str());
    }

    /*************************************************************************/
    inline void print_table_body(const bool a_IS_ENABLED_PRINT) {
        if (!a_IS_ENABLED_PRINT) {
            return;
        }

        const auto& STATE = m_state_manager.state();
        const auto  SIGN  = m_model_ptr->sign();

        const auto& CURRENT_SOLUTION_SCORE = STATE.current_solution_score;
        const auto& INCUMBENT_SOLUTION_SCORE =
            m_global_state_ptr->incumbent_holder
                .global_augmented_incumbent_score();

        char mark_special_neighborhood_move  = ' ';
        char mark_current                    = ' ';
        char mark_global_augmented_incumbent = ' ';

        if (STATE.current_move.is_special_neighborhood_move) {
            mark_special_neighborhood_move = 's';
        }

        if (STATE.update_status &  //
            solution::IncumbentHolderConstant::
                STATUS_FEASIBLE_INCUMBENT_UPDATE) {
            mark_current                    = '*';
            mark_global_augmented_incumbent = '*';
        } else if (STATE.update_status &  //
                   solution::IncumbentHolderConstant::
                       STATUS_GLOBAL_AUGMENTED_INCUMBENT_UPDATE) {
            mark_current                    = '#';
            mark_global_augmented_incumbent = '#';
            if (STATE.is_aspirated) {
                mark_current                    = '@';
                mark_global_augmented_incumbent = '@';
            }
        } else if (STATE.update_status &  //
                   solution::IncumbentHolderConstant::
                       STATUS_LOCAL_AUGMENTED_INCUMBENT_UPDATE) {
            mark_current = '!';
        }

        std::string color_current_feasible_begin   = "";
        std::string color_current_feasible_end     = "";
        std::string color_incumbent_feasible_begin = "";
        std::string color_incumbent_feasible_end   = "";
        std::string color_incumbent_update_begin   = "";
        std::string color_incumbent_update_end     = "";

#ifdef _PRINTEMPS_STYLING
        if (CURRENT_SOLUTION_SCORE.is_feasible) {
            color_current_feasible_begin = constant::CYAN;
            color_current_feasible_end   = constant::NO_COLOR;
        }

        if (INCUMBENT_SOLUTION_SCORE.is_feasible) {
            color_incumbent_feasible_begin = constant::CYAN;
            color_incumbent_feasible_end   = constant::NO_COLOR;
        }

        if (STATE.update_status &  //
            solution::IncumbentHolderConstant::
                STATUS_GLOBAL_AUGMENTED_INCUMBENT_UPDATE) {
            color_current_feasible_begin = constant::YELLOW;
            color_current_feasible_end   = constant::NO_COLOR;

            color_incumbent_feasible_begin = constant::YELLOW;
            color_incumbent_feasible_end   = constant::NO_COLOR;

            color_incumbent_update_begin = constant::YELLOW;
            color_incumbent_update_end   = constant::NO_COLOR;
        }
#endif

        auto int_format = [](const int a_VALUE) {
            if (a_VALUE >= 100000) {
                return utility::to_string(a_VALUE / 1000, "%4dk");
            } else {
                return utility::to_string(a_VALUE, "%5d");
            }
        };

        std::printf(  //
            "%8d%c|%s %s %s %s |%s%c%9.2e%s %s(%8.2e)%s |%s%c%9.2e%s "
            "%s(%8.2e)%s\n",
            STATE.iteration,
            mark_special_neighborhood_move,                                 //
            int_format(STATE.number_of_all_neighborhoods).c_str(),          //
            int_format(STATE.number_of_feasible_neighborhoods).c_str(),     //
            int_format(STATE.number_of_permissible_neighborhoods).c_str(),  //
            int_format(STATE.number_of_improvable_neighborhoods).c_str(),   //
            color_incumbent_update_begin.c_str(),
            mark_current,  //
            CURRENT_SOLUTION_SCORE.objective * SIGN,
            color_incumbent_update_end.c_str(),
            color_current_feasible_begin.c_str(),
            CURRENT_SOLUTION_SCORE.is_feasible
                ? 0.0
                : CURRENT_SOLUTION_SCORE.total_violation,
            color_current_feasible_end.c_str(),  //
            color_incumbent_update_begin.c_str(),
            mark_global_augmented_incumbent,
            INCUMBENT_SOLUTION_SCORE.objective * SIGN,
            color_incumbent_update_end.c_str(),
            color_incumbent_feasible_begin.c_str(),
            INCUMBENT_SOLUTION_SCORE.total_violation * SIGN,
            color_incumbent_feasible_end.c_str());
    }
    /*************************************************************************/
    inline void print_table_footer(const bool a_IS_ENABLED_PRINT) {
        if (!a_IS_ENABLED_PRINT) {
            return;
        }

        utility::print(
            "---------+------------------------+----------------------+--------"
            "--------------");
        utility::print_info(  //
            " -- s: Special neighborhood move was employed.", true);
        utility::print_info(  //
            " -- *: Feasible incumbent solution was updated.", true);
        utility::print_info(  //
            " -- #: Global incumbent solution was updated.", true);
        utility::print_info(  //
            " -- @: Global incumbent solution was updated by aspiration "
            "criteria.",
            true);
        utility::print_info(  //
            " -- !: Local incumbent solution was updated.", true);
        utility::print_single_line(true);
    }

   public:
    /*************************************************************************/
    TabuSearchCore(void) {
        this->initialize();
    }

    /*************************************************************************/
    TabuSearchCore(
        model::Model<T_Variable, T_Expression>* a_model_ptr,         //
        GlobalState<T_Variable, T_Expression>*  a_global_state_ptr,  //
        const solution::SparseSolution<T_Variable, T_Expression>&    //
                              a_INITIAL_SOLUTION,                    //
        const option::Option& a_OPTION) {
        this->initialize();
        this->setup(a_model_ptr,         //
                    a_global_state_ptr,  //
                    a_INITIAL_SOLUTION,  //
                    a_OPTION);
    }

    /*************************************************************************/
    inline void initialize(void) {
        m_model_ptr        = nullptr;
        m_global_state_ptr = nullptr;

        m_initial_solution.initialize();
        m_option.initialize();

        m_feasible_solutions.clear();
        m_incumbent_solutions.clear();

        m_state_manager.initialize();
        m_result.initialize();
        m_get_rand_mt.seed(0);
    }

    /*************************************************************************/
    inline void setup(                                               //
        model::Model<T_Variable, T_Expression>* a_model_ptr,         //
        GlobalState<T_Variable, T_Expression>*  a_global_state_ptr,  //
        const solution::SparseSolution<T_Variable, T_Expression>&    //
                              a_INITIAL_SOLUTION,                    //
        const option::Option& a_OPTION) {
        m_model_ptr        = a_model_ptr;
        m_global_state_ptr = a_global_state_ptr;
        m_model_ptr->import_solution(a_INITIAL_SOLUTION);
        m_initial_solution = m_model_ptr->export_dense_solution();
        m_option           = a_OPTION;

        m_feasible_solutions.clear();
        m_incumbent_solutions.clear();
    }

    /*************************************************************************/
    inline void run(void) {
        /**
         * Start to measure computational time.
         */
        utility::TimeKeeper time_keeper;
        time_keeper.set_start_time();

        const auto& STATE = m_state_manager.state();

        /**
         * Preprocess.
         */
        this->preprocess();

        /**
         * Prepare a move evaluator.
         */
        TabuSearchCoreMoveEvaluator<T_Variable, T_Expression> move_evaluator(
            m_model_ptr, &(m_global_state_ptr->memory), m_option);

        /**
         * Prepare an step size adjuster for integer moves.
         */
        IntegerStepSizeAdjuster integer_step_size_adjuster(m_model_ptr,
                                                           m_option);

        std::vector<solution::SolutionScore> trial_solution_scores;
        std::vector<TabuSearchCoreMoveScore> trial_move_scores;
        std::vector<double>                  total_scores;

        /**
         * Print the header of optimization progress table and print the initial
         * solution status.
         */
        utility::print_single_line(m_option.output.verbose >=
                                   option::verbose::Outer);
        utility::print_message(
            "Tabu Search starts.",
            m_option.output.verbose >= option::verbose::Outer);

        print_table_header(m_option.output.verbose >= option::verbose::Inner);
        print_table_initial(m_option.output.verbose >= option::verbose::Inner);

        /**
         * Iterations start.
         */
        m_state_manager.reset_iteration();

        while (true) {
            m_state_manager.set_elapsed_time(time_keeper.clock());
            /**
             * Terminate the loop if the time is over.
             */
            if (this->satisfy_time_over_terminate_condition()) {
                break;
            }

            /**
             * Terminate the loop if the iteration is over.
             */
            if (this->satisfy_iteration_over_terminate_condition()) {
                break;
            }

            /**
             * Terminate the loop if the objective value of the feasible
             * incumbent reaches the target value.
             */
            if (this->satisfy_reach_target_terminate_condition()) {
                break;
            }

            /**
             * Terminate the loop if "early stop" condition is satisfied.
             */
            if (this->satisfy_early_stop_terminate_condition()) {
                break;
            }

            /**
             * Update the moves.
             */
            this->update_moves(&time_keeper);

            /**
             * Shuffle the moves.
             */
            if (m_option.tabu_search.is_enabled_shuffle) {
                m_model_ptr->neighborhood().shuffle_moves(&m_get_rand_mt);
            }

            /**
             * Curtail moves (optional).
             */
            if (m_option.tabu_search.is_enabled_move_curtail) {
                this->curtail_moves();
            }

            /**
             * Terminate the loop if the optimal solution is found or there are
             * no improving moves.
             */
            if (this->satisfy_optimal_or_no_move_terminate_condition()) {
                break;
            }

            /**
             * Reserve elements for vectors by the number of the moves.
             */
            const auto& TRIAL_MOVE_PTRS =
                m_model_ptr->neighborhood().move_ptrs();
            trial_solution_scores.resize(STATE.number_of_moves);
            trial_move_scores.resize(STATE.number_of_moves);
            total_scores.resize(STATE.number_of_moves);

            const double START_TIME = time_keeper.clock();

            const auto NUMBER_OF_MOVES        = STATE.number_of_moves;
            const auto CURRENT_SOLUTION_SCORE = STATE.current_solution_score;
            const auto ITERATION              = STATE.iteration;
            const auto TABU_TENURE            = STATE.tabu_tenure;
            const auto DURATION               = ITERATION - TABU_TENURE;
#ifdef _OPENMP
#pragma omp parallel for if (m_option.parallel                                \
                                 .is_enabled_move_evaluation_parallelization) \
    schedule(static)                                                          \
        num_threads(m_option.parallel.number_of_threads_move_evaluation)
#endif
            for (auto i = 0; i < NUMBER_OF_MOVES; i++) {
                /**
                 * The neighborhood solutions will be evaluated in parallel by
                 * fast or ordinary(slow) evaluation methods.
                 */
#ifndef _PRINTEMPS_LINEAR_MINIMIZATION
                if (m_model_ptr->is_enabled_fast_evaluation()) {
#endif
                    if (TRIAL_MOVE_PTRS[i]->is_univariable_move) {
                        m_model_ptr->evaluate_single(
                            &trial_solution_scores[i],  //
                            *TRIAL_MOVE_PTRS[i],        //
                            CURRENT_SOLUTION_SCORE);
                    } else if (TRIAL_MOVE_PTRS[i]->is_selection_move) {
                        m_model_ptr->evaluate_selection(  //
                            &trial_solution_scores[i],    //
                            *TRIAL_MOVE_PTRS[i],          //
                            CURRENT_SOLUTION_SCORE);

                    } else {
                        m_model_ptr->evaluate_multi(    //
                            &trial_solution_scores[i],  //
                            *TRIAL_MOVE_PTRS[i],        //
                            CURRENT_SOLUTION_SCORE);
                    }

#ifndef _PRINTEMPS_LINEAR_MINIMIZATION
                } else {
                    m_model_ptr->evaluate(&trial_solution_scores[i],  //
                                          *TRIAL_MOVE_PTRS[i]);
                }
#endif
                move_evaluator.evaluate(&trial_move_scores[i],  //
                                        *TRIAL_MOVE_PTRS[i],    //
                                        ITERATION,              //
                                        DURATION);

                total_scores[i] =
                    trial_solution_scores[i].local_augmented_objective +
                    trial_move_scores[i].frequency_penalty +
                    trial_move_scores[i].lagrangian_penalty;

                /**
                 * If the move is "tabu", it will be set lower priorities in
                 * selecting a move for the next solution.
                 */
                if (!trial_move_scores[i].is_permissible) {
                    total_scores[i] += constant::LARGE_VALUE_50;
                }

                /**
                 * If the move is special neighborhood moves, it must improves
                 * objective or feasibility.
                 */
                if (TRIAL_MOVE_PTRS[i]->is_special_neighborhood_move &&
                    !(trial_solution_scores[i].is_objective_improvable ||
                      trial_solution_scores[i].is_feasibility_improvable)) {
                    total_scores[i] += constant::LARGE_VALUE_100;
                }
            }
            const double END_TIME = time_keeper.clock();

            m_state_manager.update_move_evaluation_statistics(
                NUMBER_OF_MOVES, END_TIME - START_TIME);

            /**
             * Select moves for the next solution.
             */
            const auto SELECT_RESULT = this->select_move(
                total_scores, trial_move_scores, trial_solution_scores);
            const auto SELECTED_INDEX = SELECT_RESULT.first;
            const auto IS_ASPIRATED   = SELECT_RESULT.second;

            /**
             * Update the model by the selected move.
             */
            auto move_ptr = TRIAL_MOVE_PTRS[SELECTED_INDEX];

            /**
             * If the selected move updates the global incumbent solution and
             * its type is "Integer", adjust the step size to obtain better
             * solution.
             */
            if (m_option.neighborhood.is_enabled_integer_step_size_adjuster &&
                m_model_ptr->is_enabled_fast_evaluation() &&
                move_ptr->sense == neighborhood::MoveSense::Integer &&
                trial_solution_scores[SELECTED_INDEX]
                        .global_augmented_objective <
                    m_global_state_ptr->incumbent_holder
                        .global_augmented_incumbent_objective()) {
                integer_step_size_adjuster.adjust(move_ptr,
                                                  CURRENT_SOLUTION_SCORE);
                m_model_ptr->evaluate_multi(                 //
                    &trial_solution_scores[SELECTED_INDEX],  //
                    *move_ptr,                               //
                    CURRENT_SOLUTION_SCORE);
            }

            m_model_ptr->update(*move_ptr);

            /**
             * Update the memory.
             */
            this->update_memory(move_ptr);

            /**
             * Update the state.
             */
            m_state_manager.update(move_ptr,           //
                                   SELECTED_INDEX,     //
                                   IS_ASPIRATED,       //
                                   trial_move_scores,  //
                                   trial_solution_scores);

            /**
             * To avoid cycling, each special neighborhood can be used only once
             * in one tabu search loop.
             */
            if (move_ptr->is_special_neighborhood_move) {
                move_ptr->is_available = false;
            }

            /**
             * Update the stored chain moves.
             */
            if (STATE.iteration > 0 &&
                m_option.neighborhood.is_enabled_chain_move) {
                this->update_chain_moves();
            }

            /**
             * Store the current feasible solution.
             */
            if (m_option.output.is_enabled_store_feasible_solutions &&
                STATE.current_solution_score.is_feasible) {
                m_feasible_solutions.push_back(
                    m_model_ptr->export_sparse_solution());
            }

            /**
             * Print the optimization progress.
             */
            if ((STATE.iteration %
                 std::max(m_option.tabu_search.log_interval, 1)) == 0 ||
                STATE.update_status > 0) {
                print_table_body(m_option.output.verbose >=
                                 option::verbose::Inner);
            }

            /**
             * If the local penalty us sufficiently larger than objective
             * sensitivity, the current loop will be terminated and the local
             * penalty coefficients will be adjusted.
             */
            if (m_option.tabu_search.is_enabled_automatic_break) {
                if (this->satisfy_penalty_coefficient_too_large_terminate_condition(
                        trial_solution_scores)) {
                    break;
                }
            }

            m_state_manager.next_iteration();
        }

        /**
         * Print the footer of the optimization progress table.
         */
        print_table_footer(m_option.output.verbose >= option::verbose::Inner);

        /**
         * Store the incumbent solution.
         */
        if (STATE.total_update_status &  //
            solution::IncumbentHolderConstant::
                STATUS_GLOBAL_AUGMENTED_INCUMBENT_UPDATE) {
            m_incumbent_solutions.push_back(
                m_global_state_ptr->incumbent_holder
                    .global_augmented_incumbent_solution()
                    .to_sparse());
        }
        /**
         * Postprocess.
         */
        m_state_manager.set_elapsed_time(time_keeper.clock());
        this->postprocess();
    }

    /*************************************************************************/
    inline const std::vector<
        solution::SparseSolution<T_Variable, T_Expression>>&
    feasible_solutions(void) const {
        return m_feasible_solutions;
    }

    /*************************************************************************/
    inline const std::vector<
        solution::SparseSolution<T_Variable, T_Expression>>&
    incumbent_solutions(void) const {
        return m_incumbent_solutions;
    }

    /*************************************************************************/
    inline const TabuSearchCoreResult<T_Variable, T_Expression>& result(
        void) const {
        return m_result;
    }
};
}  // namespace printemps::solver::tabu_search::core
#endif
/*****************************************************************************/
// END
/*****************************************************************************/