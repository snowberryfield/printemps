/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_LAGRANGE_DUAL_CONTROLLER_LAGRANGE_DUAL_CONTROLLER_H__
#define PRINTEMPS_SOLVER_LAGRANGE_DUAL_CONTROLLER_LAGRANGE_DUAL_CONTROLLER_H__

#include "../../abstract_controller.h"
#include "../core/lagrange_dual.h"
#include "lagrange_dual_controller_result.h"

namespace printemps {
namespace solver {
namespace lagrange_dual {
namespace controller {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class LagrangeDualController
    : public AbstractController<T_Variable, T_Expression> {
   private:
    LagrangeDualControllerResult m_result;

   public:
    /*************************************************************************/
    LagrangeDualController(void) {
        this->initialize();
    }

    /*************************************************************************/
    LagrangeDualController(
        model::Model<T_Variable, T_Expression>* a_model_ptr,  //
        const solution::DenseSolution<T_Variable, T_Expression>&
            a_CURRENT_SOLUTION,  //
        solution::IncumbentHolder<T_Variable, T_Expression>*
                                          a_incumbent_holder_ptr,
        Memory<T_Variable, T_Expression>* a_memory_ptr,  //
        solution::SolutionArchive<T_Variable, T_Expression>*
                                   a_solution_archive_ptr,  //
        const utility::TimeKeeper& a_TIME_KEEPER,           //
        const option::Option&      a_OPTION) {
        this->initialize();
        this->setup(a_model_ptr,             //
                    a_CURRENT_SOLUTION,      //
                    a_incumbent_holder_ptr,  //
                    a_memory_ptr,            //
                    a_solution_archive_ptr,  //
                    a_TIME_KEEPER,           //
                    a_OPTION);
    }

    /*************************************************************************/
    virtual ~LagrangeDualController(void) {
        /// nothing to do
    }

    /*************************************************************************/
    inline void initialize(void) {
        AbstractController<T_Variable, T_Expression>::initialize();
        m_result.initialize();
    }

    /*************************************************************************/
    inline bool satisfy_skip_condition(const bool a_IS_ENABLED_PRINT) {
        if (!this->m_model_ptr->is_linear()) {
            utility::print_warning(
                "Solving lagrange dual was skipped because the problem is "
                "nonlinear.",
                a_IS_ENABLED_PRINT);
            return true;
        }

        if (this->m_model_ptr->number_of_selection_variables() > 0 ||
            this->m_model_ptr->number_of_intermediate_variables() > 0) {
            utility::print_warning(
                "Solving lagrange dual was skipped because it not applicable "
                "to problems which include selection variables or intermediate "
                "variables.",
                a_IS_ENABLED_PRINT);
            return true;
        }

        return false;
    }

    /*************************************************************************/
    inline bool satisfy_terminate_condition(const double a_ELAPSED_TIME,
                                            const bool   a_IS_ENABLED_PRINT) {
        if (a_ELAPSED_TIME > this->m_master_option.time_max) {
            utility::print_message(
                "Outer loop was terminated because of time-over (" +
                    utility::to_string(a_ELAPSED_TIME, "%.3f") + "sec).",
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
        const double ELAPSED_TIME = this->m_time_keeper.clock();

        if (this->satisfy_terminate_condition(
                ELAPSED_TIME,
                this->m_master_option.verbose >= option::verbose::Outer)) {
            m_result.initialize();
            return;
        }

        /**
         * Check the skip condition.
         */
        if (this->satisfy_skip_condition(  //
                this->m_master_option.verbose >= option::verbose::Warning)) {
            m_result.initialize();
            return;
        }

        /**
         * Prepare an option object for lagrange dual search.
         */
        option::Option option            = this->m_master_option;
        option.lagrange_dual.time_offset = ELAPSED_TIME;

        /**
         * Prepare feasible solutions storage.
         */
        std::vector<solution::SparseSolution<T_Variable, T_Expression>>
            feasible_solutions;

        /**
         * Prepare the initial variable values.
         */
        auto initial_variable_value_proxies =
            this->m_current_solution.variable_value_proxies;

        /**
         * Run the lagrange dual search.
         */
        auto lagrange_dual_result =
            lagrange_dual::core::solve(this->m_model_ptr,             //
                                       this->m_incumbent_holder_ptr,  //
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
        m_result = LagrangeDualControllerResult(
            lagrange_dual_result.number_of_iterations,
            lagrange_dual_result.total_update_status);

        /**
         * Print the search summary.
         */
        utility::print_message(
            "Solving Lagrange dual finished. ",
            this->m_master_option.verbose >= option::verbose::Outer);

        this->print_elapsed_time(
            this->m_time_keeper.clock(),
            this->m_master_option.verbose >= option::verbose::Outer);

        this->print_incumbent_summary(  //
            this->m_master_option.verbose >= option::verbose::Outer);
    }

    /*************************************************************************/
    inline constexpr const LagrangeDualControllerResult& result(void) const {
        return m_result;
    }
};
}  // namespace controller
}  // namespace lagrange_dual
}  // namespace solver
}  // namespace printemps

#endif
/*****************************************************************************/
// END
/*****************************************************************************/