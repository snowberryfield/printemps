/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_TABU_SEARCH_CONTROLLER_TABU_SEARCH_CONTROLLER_LOGGER_H__
#define PRINTEMPS_SOLVER_TABU_SEARCH_CONTROLLER_TABU_SEARCH_CONTROLLER_LOGGER_H__

namespace printemps {
namespace solver {
namespace tabu_search {
namespace controller {
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
    TabuSearchControllerParameter *                      m_parameter_ptr;

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
        TabuSearchControllerState<T_Variable, T_Expression> *a_state_ptr,
        TabuSearchControllerParameter *                      a_parameter_ptr) {
        this->setup(a_FILE_NAME,       //
                    a_controller_ptr,  //
                    a_state_ptr,       //
                    a_parameter_ptr);
    }

    /*************************************************************************/
    void initialize(void) {
        if (m_ofstream.is_open()) {
            m_ofstream.close();
        }

        m_controller_ptr = nullptr;
        m_state_ptr      = nullptr;
        m_parameter_ptr  = nullptr;
    }

    /*************************************************************************/
    void setup(const std::string &                             a_FILE_NAME,
               TabuSearchController<T_Variable, T_Expression> *a_controller_ptr,
               TabuSearchControllerState<T_Variable, T_Expression> *a_state_ptr,
               TabuSearchControllerParameter *a_parameter_ptr) {
        this->initialize();
        m_file_name = a_FILE_NAME;
        m_ofstream.open(a_FILE_NAME);

        m_controller_ptr = a_controller_ptr;
        m_state_ptr      = a_state_ptr;
        m_parameter_ptr  = a_parameter_ptr;
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
        m_ofstream                                           //
            << "#iteration "                                 // 0
            << "elapsed_time "                               // 1
            << "local_objective "                            // 2
            << "local_violation "                            // 3
            << "global_objective "                           // 4
            << "global_violation "                           // 5
            << "primal_intensity "                           // 6
            << "dual_intensity "                             // 7
            << "update_status "                              // 8
            << "employing_local_augmented_solution_flag "    // 9
            << "employing_global_augmented_solution_flag "   // 10
            << "employing_previous_solution_flag "           // 11
            << "is_enabled_penalty_coefficient_relaxing "    // 12
            << "is_enabled_penalty_coefficient_tightening "  // 13
            << "penalty_coefficient_reset_flag"              // 14
            << "penalty_coefficient_relaxing_rate "          // 15
            << "penalty_coefficient_tightening_rate "        // 16
            << "is_enabled_forcibly_initial_modification "   // 17
            << "number_of_initial_modification "             // 18
            << "initial_tabu_tenure" << std::endl;           // 19
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
        auto &p = *m_parameter_ptr;

        m_ofstream                                                 //
            << s.iteration << " "                                  // 0
            << m_controller_ptr->time_keeper().clock() << " "      // 1
            << local_incumbent.objective << " "                    // 2
            << local_incumbent.total_violation << " "              // 3
            << global_incumbent.objective << " "                   // 4
            << global_incumbent.total_violation << " "             // 5
            << s.current_primal_intensity << " "                   // 6
            << s.current_dual_intensity << " "                     // 7
            << s.tabu_search_result.total_update_status << " "     // 8
            << p.employing_local_augmented_solution_flag << " "    // 9
            << p.employing_global_augmented_solution_flag << " "   // 10
            << p.employing_previous_solution_flag << " "           // 11
            << p.is_enabled_penalty_coefficient_relaxing << " "    // 12
            << p.is_enabled_penalty_coefficient_tightening << " "  // 13
            << p.penalty_coefficient_reset_flag << " "             // 14
            << p.penalty_coefficient_relaxing_rate << " "          // 15
            << p.penalty_coefficient_tightening_rate << " "        // 16
            << p.is_enabled_forcibly_initial_modification << " "   // 17
            << p.number_of_initial_modification << " "             // 18
            << p.initial_tabu_tenure                               // 19
            << std::endl;
    }
};
}  // namespace controller
}  // namespace tabu_search
}  // namespace solver
}  // namespace printemps
#endif
/*****************************************************************************/
// END
/*****************************************************************************/