/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_LOCAL_SEARCH_CORE_LOCAL_SEARCH_CORE_CONTROLLER_H__
#define PRINTEMPS_SOLVER_LOCAL_SEARCH_CORE_LOCAL_SEARCH_CORE_CONTROLLER_H__

#include "../../abstract_solver_controller.h"
#include "../core/local_search_core.h"
#include "local_search_controller_result.h"

namespace printemps {
namespace solver {
namespace local_search {
namespace controller {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class LocalSearchController
    : public AbstractSolverController<T_Variable, T_Expression> {
   private:
    LocalSearchControllerResult m_result;

   public:
    /*************************************************************************/
    LocalSearchController(void) {
        this->initialize();
    }

    /*************************************************************************/
    LocalSearchController(
        model::Model<T_Variable, T_Expression>* a_model_ptr,  //
        const solution::DenseSolution<T_Variable, T_Expression>&
            a_INITIAL_SOLUTION,  //
        solution::IncumbentHolder<T_Variable, T_Expression>*
                                          a_incumbent_holder_ptr,
        Memory<T_Variable, T_Expression>* a_memory_ptr,  //
        solution::SolutionArchive<T_Variable, T_Expression>*
                                   a_solution_archive_ptr,  //
        const utility::TimeKeeper& a_TIME_KEEPER,           //
        const option::Option&      a_OPTION) {
        this->initialize();
        this->setup(a_model_ptr,             //
                    a_INITIAL_SOLUTION,      //
                    a_incumbent_holder_ptr,  //
                    a_memory_ptr,            //
                    a_solution_archive_ptr,  //
                    a_TIME_KEEPER,           //
                    a_OPTION);
    }

    /*************************************************************************/
    virtual ~LocalSearchController(void) {
        /// nothing to do
    }

    /*************************************************************************/
    inline void initialize(void) {
        AbstractSolverController<T_Variable, T_Expression>::initialize();
        m_result.initialize();
    }

    /*************************************************************************/
    inline bool satisfy_terminate_condition(const double a_TOTAL_ELAPSED_TIME,
                                            const bool   a_IS_ENABLED_PRINT) {
        if (a_TOTAL_ELAPSED_TIME > this->m_master_option.time_max) {
            utility::print_message(
                "Outer loop was terminated because of time-over (" +
                    utility::to_string(a_TOTAL_ELAPSED_TIME, "%.3f") + "sec).",
                a_IS_ENABLED_PRINT);
            return true;
        }

        if (this->m_incumbent_holder_ptr->feasible_incumbent_objective() <=
            this->m_master_option.target_objective_value) {
            utility::print_message(
                "Outer loop was terminated because of feasible objective "
                "reaches the target limit.",
                a_IS_ENABLED_PRINT);
            return true;
        }
        return false;
    }

    /*************************************************************************/
    inline void run(void) {
        /**
         * Check the terminate condition.
         */
        const double TOTAL_ELAPSED_TIME = this->m_time_keeper.clock();

        if (this->satisfy_terminate_condition(
                TOTAL_ELAPSED_TIME,
                this->m_master_option.verbose >= option::verbose::Outer)) {
            m_result.initialize();
            return;
        }

        /**
         * Prepare an option object for local search.
         */
        option::Option option           = this->m_master_option;
        option.local_search.time_offset = TOTAL_ELAPSED_TIME;

        /**
         * Prepare feasible solutions storage.
         */
        std::vector<solution::SparseSolution<T_Variable, T_Expression>>
            feasible_solutions;

        /**
         * Prepare the initial variable values.
         */
        auto initial_variable_value_proxies =
            this->m_initial_solution.variable_value_proxies;

        /**
         * Run the local search.
         */
        auto local_search_result =
            local_search::core::solve(this->m_model_ptr,             //
                                      this->m_incumbent_holder_ptr,  //
                                      this->m_memory_ptr,            //
                                      &feasible_solutions,           //
                                      option,                        //
                                      initial_variable_value_proxies);

        /**
         * Update the feasible solutions archive.
         */
        if (this->m_master_option.is_enabled_store_feasible_solutions) {
            this->update_archive(feasible_solutions);
        }

        /**
         * Store the result.
         */
        m_result = LocalSearchControllerResult(
            local_search_result.number_of_iterations,
            local_search_result.total_update_status);

        /**
         * Print the search summary.
         */
        utility::print_message(
            "Local search finished.",
            this->m_master_option.verbose >= option::verbose::Outer);

        this->print_total_elapsed_time(  //
            this->m_time_keeper.clock(),
            this->m_master_option.verbose >= option::verbose::Outer);

        this->print_incumbent_summary(  //
            this->m_master_option.verbose >= option::verbose::Outer);
    }

    /*************************************************************************/
    inline constexpr const LocalSearchControllerResult& result(void) const {
        return m_result;
    }
};
}  // namespace controller
}  // namespace local_search
}  // namespace solver
}  // namespace printemps

#endif
/*****************************************************************************/
// END
/*****************************************************************************/