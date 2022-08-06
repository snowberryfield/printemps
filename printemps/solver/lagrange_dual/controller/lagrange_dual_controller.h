/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_LAGRANGE_DUAL_CONTROLLER_LAGRANGE_DUAL_CONTROLLER_H__
#define PRINTEMPS_SOLVER_LAGRANGE_DUAL_CONTROLLER_LAGRANGE_DUAL_CONTROLLER_H__

#include "../../abstract_solver_controller.h"
#include "../core/lagrange_dual_core.h"
#include "lagrange_dual_controller_result.h"

namespace printemps::solver::lagrange_dual::controller {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class LagrangeDualController
    : public AbstractSolverController<T_Variable, T_Expression> {
   private:
    LagrangeDualControllerResult<T_Variable, T_Expression> m_result;

   public:
    /*************************************************************************/
    LagrangeDualController(void) {
        this->initialize();
    }

    /*************************************************************************/
    LagrangeDualController(
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
    virtual ~LagrangeDualController(void) {
        /// nothing to do
    }

    /*************************************************************************/
    inline void initialize(void) {
        AbstractSolverController<T_Variable, T_Expression>::initialize();
        m_result.initialize();
    }

    /*************************************************************************/
    inline bool satisfy_not_linear_skip_condition(
        const bool a_IS_ENABLED_PRINT) {
        if (!this->m_model_ptr->is_linear()) {
            utility::print_warning(
                "Solving Lagrange dual was skipped because the problem is not "
                "linear.",
                a_IS_ENABLED_PRINT);
            return true;
        }
        return false;
    }

    /*************************************************************************/
    inline bool satisfy_not_applicable_skip_condition(
        const bool a_IS_ENABLED_PRINT) {
        if (this->m_model_ptr->number_of_selection_variables() > 0 ||
            this->m_model_ptr->number_of_dependent_binary_variables() > 0 ||
            this->m_model_ptr->number_of_dependent_integer_variables() > 0) {
            utility::print_warning(
                "Solving lagrange dual was skipped because it is not "
                "applicable to problems which include selection variables or "
                "dependent variables.",
                a_IS_ENABLED_PRINT);
            return true;
        }
        return false;
    }

    /*************************************************************************/
    inline bool satisfy_time_over_skip_condition(
        const double a_TOTAL_ELAPSED_TIME, const bool a_IS_ENABLED_PRINT) {
        if (a_TOTAL_ELAPSED_TIME > this->m_option.general.time_max) {
            utility::print_message(
                "Solving Lagrange dual was skipped because of time-over (" +
                    utility::to_string(a_TOTAL_ELAPSED_TIME, "%.3f") + "sec).",
                a_IS_ENABLED_PRINT);
            return true;
        }
        return false;
    }

    /*************************************************************************/
    inline bool satisfy_reach_target_skip_condition(
        const bool a_IS_ENABLED_PRINT) {
        if (this->m_incumbent_holder_ptr->feasible_incumbent_objective() <=
            this->m_option.general.target_objective_value) {
            utility::print_message(
                "Solving Lagrange dual was skipped because of feasible "
                "objective reaches the target limit.",
                a_IS_ENABLED_PRINT);
            return true;
        }
        return false;
    }

    /*************************************************************************/
    inline void run(void) {
        const double TOTAL_ELAPSED_TIME = this->m_time_keeper.clock();
        /**
         * Skip Lagrange dual if the problem is not linear.
         */
        if (this->satisfy_not_linear_skip_condition(  //
                this->m_option.output.verbose >= option::verbose::Outer)) {
            m_result.initialize();
            return;
        }

        /**
         * Skip Lagrange dual if it is not applicable.
         */
        if (this->satisfy_not_applicable_skip_condition(  //
                this->m_option.output.verbose >= option::verbose::Outer)) {
            m_result.initialize();
            return;
        }

        /**
         * Skip Lagrange dual if the time is over.
         */
        if (this->satisfy_time_over_skip_condition(
                TOTAL_ELAPSED_TIME,
                this->m_option.output.verbose >= option::verbose::Outer)) {
            m_result.initialize();
            return;
        }

        /**
         * Skip Lagrange dual if the objective value of the feasible
         * incumbent reaches the target value.
         */
        if (this->satisfy_reach_target_skip_condition(  //
                this->m_option.output.verbose >= option::verbose::Outer)) {
            m_result.initialize();
            return;
        }

        /**
         * Prepare an option object for lagrange dual search.
         */
        option::Option option            = this->m_option;
        option.lagrange_dual.time_offset = TOTAL_ELAPSED_TIME;

        /**
         * Run the lagrange dual search.
         */
        core::LagrangeDualCore<T_Variable, T_Expression> lagrange_dual(
            this->m_model_ptr,                                //
            this->m_initial_solution.variable_value_proxies,  //
            this->m_incumbent_holder_ptr,                     //
            this->m_memory_ptr,                               //
            option);

        lagrange_dual.run();

        auto lagrange_dual_result = lagrange_dual.result();

        /**
         * Update the feasible solutions archive.
         */
        if (this->m_option.output.is_enabled_store_feasible_solutions) {
            this->update_archive(lagrange_dual.feasible_solutions());
        }

        /**
         * Store the result.
         */
        m_result = LagrangeDualControllerResult<T_Variable, T_Expression>(
            lagrange_dual_result);

        /**
         * Print the search summary.
         */
        utility::print_message(
            "Solving Lagrange dual finished. ",
            this->m_option.output.verbose >= option::verbose::Outer);

        this->print_total_elapsed_time(
            this->m_time_keeper.clock(),
            this->m_option.output.verbose >= option::verbose::Outer);

        this->print_incumbent_summary(  //
            this->m_option.output.verbose >= option::verbose::Outer);
    }

    /*************************************************************************/
    inline constexpr const LagrangeDualControllerResult<T_Variable,
                                                        T_Expression>&
    result(void) const {
        return m_result;
    }
};
}  // namespace printemps::solver::lagrange_dual::controller

#endif
/*****************************************************************************/
// END
/*****************************************************************************/