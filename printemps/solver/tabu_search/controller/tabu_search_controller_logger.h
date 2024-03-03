/*****************************************************************************/
// Copyright (c) 2020-2023 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_TABU_SEARCH_CONTROLLER_TABU_SEARCH_CONTROLLER_LOGGER_H__
#define PRINTEMPS_SOLVER_TABU_SEARCH_CONTROLLER_TABU_SEARCH_CONTROLLER_LOGGER_H__

namespace printemps::solver::tabu_search::controller {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class TabuSearchController;

template <class T_Variable, class T_Expression>
class TabuSearchControllerLogger {
   private:
    std::string   m_file_name;
    std::ofstream m_ofstream;

    TabuSearchController<T_Variable, T_Expression> *     m_controller_ptr;
    TabuSearchControllerState<T_Variable, T_Expression> *m_state_ptr;

    /*************************************************************************/
    TabuSearchControllerLogger(const TabuSearchControllerLogger &);
    TabuSearchControllerLogger &operator=(const TabuSearchControllerLogger &);

   public:
    /*************************************************************************/
    TabuSearchControllerLogger(void) {
        this->initialize();
    }
    /*************************************************************************/
    TabuSearchControllerLogger(
        const std::string &                                  a_FILE_NAME,
        TabuSearchController<T_Variable, T_Expression> *     a_controller_ptr,
        TabuSearchControllerState<T_Variable, T_Expression> *a_state_ptr) {
        this->setup(a_FILE_NAME, a_controller_ptr, a_state_ptr);
    }

    /*************************************************************************/
    void initialize(void) {
        if (m_ofstream.is_open()) {
            m_ofstream.close();
        }

        m_controller_ptr = nullptr;
        m_state_ptr      = nullptr;
    }

    /*************************************************************************/
    void setup(
        const std::string &                                  a_FILE_NAME,
        TabuSearchController<T_Variable, T_Expression> *     a_controller_ptr,
        TabuSearchControllerState<T_Variable, T_Expression> *a_state_ptr) {
        this->initialize();
        m_file_name = a_FILE_NAME;
        m_ofstream.open(a_FILE_NAME);

        m_controller_ptr = a_controller_ptr;
        m_state_ptr      = a_state_ptr;
    }

    /*************************************************************************/
    void write_instance_info(void) {
        auto model_ptr = m_controller_ptr->model_ptr();
        m_ofstream << "#instance_name: " << model_ptr->name() << std::endl;
        m_ofstream << "#number_of_variables: "
                   << model_ptr->number_of_variables() << std::endl;
        m_ofstream << "#number_of_Constraints: "
                   << model_ptr->number_of_constraints() << std::endl;
    }

    /*************************************************************************/
    void write_header(void) {
        m_ofstream                                                //
            << "#iteration "                                      //
            << "elapsed_time "                                    //
            << "averaged_inner_iteration_speed "                  //
            << "averaged_move_evaluation_speed "                  //
            << "local_objective "                                 //
            << "local_violation "                                 //
            << "global_objective "                                //
            << "global_violation "                                //
            << "primal_intensity "                                //
            << "dual_intensity "                                  //
            << "performance "                                     //
            << "update_status "                                   //
            << "employing_local_solution_flag "                   //
            << "employing_global_solution_flag "                  //
            << "employing_previous_solution_flag "                //
            << "is_enabled_penalty_coefficient_relaxing "         //
            << "is_enabled_penalty_coefficient_tightening "       //
            << "penalty_coefficient_reset_flag "                  //
            << "penalty_coefficient_relaxing_rate "               //
            << "penalty_coefficient_tightening_rate "             //
            << "is_enabled_forcibly_initial_modification "        //
            << "number_of_initial_modification "                  //
            << "initial_tabu_tenure "                             //
            << "number_of_threads_neighborhood_update "           //
            << "averaged_number_of_threads_neighborhood_update "  //
            << "number_of_threads_evaluation "                    //
            << "averaged_number_of_threads_evaluation "           //
            << std::endl;                                         //
    }

    /*************************************************************************/
    void write_log(void) {
        if (!m_ofstream.is_open()) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "This log file " + m_file_name + " is not opened."));
        }
        auto &global_incumbent = m_controller_ptr->incumbent_holder_ptr()
                                     ->global_augmented_incumbent_score();
        auto &local_incumbent = m_controller_ptr->incumbent_holder_ptr()
                                    ->local_augmented_incumbent_score();

        auto &s = *m_state_ptr;

        m_ofstream                                                      //
            << s.iteration << " "                                       //
            << s.total_elapsed_time << " "                              //
            << s.averaged_inner_iteration_speed << " "                  //
            << s.averaged_move_evaluation_speed << " "                  //
            << local_incumbent.objective << " "                         //
            << local_incumbent.total_violation << " "                   //
            << global_incumbent.objective << " "                        //
            << global_incumbent.total_violation << " "                  //
            << s.current_primal_intensity << " "                        //
            << s.current_dual_intensity << " "                          //
            << s.tabu_search_result.performance << " "                  //
            << s.tabu_search_result.total_update_status << " "          //
            << s.employing_local_solution_flag << " "                   //
            << s.employing_global_solution_flag << " "                  //
            << s.employing_previous_solution_flag << " "                //
            << s.is_enabled_penalty_coefficient_relaxing << " "         //
            << s.is_enabled_penalty_coefficient_tightening << " "       //
            << s.penalty_coefficient_reset_flag << " "                  //
            << s.penalty_coefficient_relaxing_rate << " "               //
            << s.penalty_coefficient_tightening_rate << " "             //
            << s.is_enabled_forcibly_initial_modification << " "        //
            << s.number_of_initial_modification << " "                  //
            << s.initial_tabu_tenure << " "                             //
            << s.number_of_threads_neighborhood_update << " "           //
            << s.averaged_number_of_threads_neighborhood_update << " "  //
            << s.number_of_threads_evaluation << " "                    //
            << s.averaged_number_of_threads_evaluation                  //
            << std::endl;
    }
};
}  // namespace printemps::solver::tabu_search::controller
#endif
/*****************************************************************************/
// END
/*****************************************************************************/