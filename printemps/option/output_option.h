/*****************************************************************************/
// Copyright (c) 2020-2024 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_OPTION_OUTPUT_OPTION_H__
#define PRINTEMPS_OPTION_OUTPUT_OPTION_H__

namespace printemps::option {
/*****************************************************************************/
struct OutputOptionConstant {
    static constexpr verbose::Verbose DEFAULT_VERBOSE = verbose::Off;
    static constexpr bool             DEFAULT_IS_ENABLED_WRITE_TREND  = false;
    static constexpr bool DEFAULT_IS_ENABLED_STORE_FEASIBLE_SOLUTIONS = false;
    static constexpr int  DEFAULT_FEASIBLE_SOLUTIONS_CAPACITY         = 1000;

    static constexpr bool  //
        DEFAULT_IS_ENABLED_PRINT_SEARCH_BEHAVIOR_SUMMARY = true;

    static constexpr bool  //
        DEFAULT_IS_ENABLED_PRINT_TREE_SUMMARY = true;

    static constexpr bool  //
        DEFAULT_IS_ENABLED_PRINT_THREAD_COUNT_OPTIMIZER_SUMMARY = true;

    static constexpr bool  //
        DEFAULT_IS_ENABLED_PRINT_VARIABLE_UPDATE_SUMMARY = true;

    static constexpr bool  //
        DEFAULT_IS_ENABLED_PRINT_CONSTRAINT_VIOLATION_SUMMARY = true;

    static constexpr bool  //
        DEFAULT_IS_ENABLED_PRINT_VIOLATION_AND_PENALTY_SUMMARY = true;

    static constexpr bool  //
        DEFAULT_IS_ENABLED_PRINT_TABU_SEARCH_PARAMETER = true;
};

/*****************************************************************************/
struct OutputOption {
    verbose::Verbose verbose;
    bool             is_enabled_write_trend;
    bool             is_enabled_store_feasible_solutions;
    int              feasible_solutions_capacity;

    bool is_enabled_print_search_behavior_summary;
    bool is_enabled_print_tree_summary;
    bool is_enabled_print_thread_count_optimizer_summary;
    bool is_enabled_print_variable_update_summary;
    bool is_enabled_print_constraint_violation_summary;
    bool is_enabled_print_violation_and_penalty_summary;
    bool is_enabled_print_tabu_search_parameter;

    /*************************************************************************/
    OutputOption(void) {
        this->initialize();
    }

    /*************************************************************************/
    OutputOption(const utility::json::JsonObject &a_OBJECT) {
        this->setup(a_OBJECT);
    }

    /*************************************************************************/
    void initialize(void) {
        this->verbose = OutputOptionConstant::DEFAULT_VERBOSE;
        this->is_enabled_write_trend =
            OutputOptionConstant::DEFAULT_IS_ENABLED_WRITE_TREND;
        this->is_enabled_store_feasible_solutions =
            OutputOptionConstant::DEFAULT_IS_ENABLED_STORE_FEASIBLE_SOLUTIONS;
        this->feasible_solutions_capacity =
            OutputOptionConstant::DEFAULT_FEASIBLE_SOLUTIONS_CAPACITY;

        this->is_enabled_print_search_behavior_summary =  //
            OutputOptionConstant::
                DEFAULT_IS_ENABLED_PRINT_SEARCH_BEHAVIOR_SUMMARY;

        this->is_enabled_print_tree_summary =  //
            OutputOptionConstant::DEFAULT_IS_ENABLED_PRINT_TREE_SUMMARY;

        this->is_enabled_print_thread_count_optimizer_summary =
            OutputOptionConstant::
                DEFAULT_IS_ENABLED_PRINT_THREAD_COUNT_OPTIMIZER_SUMMARY;

        this->is_enabled_print_variable_update_summary =  //
            OutputOptionConstant::
                DEFAULT_IS_ENABLED_PRINT_VARIABLE_UPDATE_SUMMARY;

        this->is_enabled_print_constraint_violation_summary =
            OutputOptionConstant::
                DEFAULT_IS_ENABLED_PRINT_CONSTRAINT_VIOLATION_SUMMARY;

        this->is_enabled_print_violation_and_penalty_summary =
            OutputOptionConstant::
                DEFAULT_IS_ENABLED_PRINT_VIOLATION_AND_PENALTY_SUMMARY;

        this->is_enabled_print_tabu_search_parameter = OutputOptionConstant::
            DEFAULT_IS_ENABLED_PRINT_TABU_SEARCH_PARAMETER;
    }

    /*************************************************************************/
    void print(void) const {
        utility::print(               //
            " -- output.verbose: " +  //
            verbose::VerboseInverseMap.at(this->verbose));

        utility::print(                              //
            " -- output.is_enabled_write_trend: " +  //
            utility::to_true_or_false(               //
                this->is_enabled_write_trend));

        utility::print(                                           //
            " -- output.is_enabled_store_feasible_solutions: " +  //
            utility::to_true_or_false(                            //
                this->is_enabled_store_feasible_solutions));

        utility::print(                                   //
            " -- output.feasible_solutions_capacity: " +  //
            utility::to_string(                           //
                this->feasible_solutions_capacity, "%d"));

        utility::print(                                                //
            " -- output.is_enabled_print_search_behavior_summary: " +  //
            utility::to_true_or_false(                                 //
                this->is_enabled_print_search_behavior_summary));

        utility::print(                                     //
            " -- output.is_enabled_print_tree_summary: " +  //
            utility::to_true_or_false(                      //
                this->is_enabled_print_tree_summary));

        utility::print(                                                       //
            " -- output.is_enabled_print_thread_count_optimizer_summary: " +  //
            utility::to_true_or_false(                                        //
                this->is_enabled_print_thread_count_optimizer_summary));

        utility::print(                                                //
            " -- output.is_enabled_print_variable_update_summary: " +  //
            utility::to_true_or_false(                                 //
                this->is_enabled_print_variable_update_summary));

        utility::print(                                                     //
            " -- output.is_enabled_print_constraint_violation_summary: " +  //
            utility::to_true_or_false(                                      //
                this->is_enabled_print_constraint_violation_summary));

        utility::print(                                                      //
            " -- output.is_enabled_print_violation_and_penalty_summary: " +  //
            utility::to_true_or_false(                                       //
                this->is_enabled_print_violation_and_penalty_summary));

        utility::print(                                              //
            " -- output.is_enabled_print_tabu_search_parameter: " +  //
            utility::to_true_or_false(                               //
                this->is_enabled_print_tabu_search_parameter));
    }

    /**************************************************************************/
    inline void setup(const utility::json::JsonObject &a_OBJECT) {
        this->initialize();

        read_json(           //
            &this->verbose,  //
            "verbose", a_OBJECT);

        read_json(                          //
            &this->is_enabled_write_trend,  //
            "is_enabled_write_trend", a_OBJECT);

        read_json(                                       //
            &this->is_enabled_store_feasible_solutions,  //
            "is_enabled_store_feasible_solutions", a_OBJECT);

        read_json(                               //
            &this->feasible_solutions_capacity,  //
            "feasible_solutions_capacity", a_OBJECT);

        read_json(                                            //
            &this->is_enabled_print_search_behavior_summary,  //
            "is_enabled_print_search_behavior_summary", a_OBJECT);

        read_json(                                 //
            &this->is_enabled_print_tree_summary,  //
            "is_enabled_print_tree_summary", a_OBJECT);

        read_json(                                                   //
            &this->is_enabled_print_thread_count_optimizer_summary,  //
            "is_enabled_print_thread_count_optimizer_summary", a_OBJECT);

        read_json(                                            //
            &this->is_enabled_print_variable_update_summary,  //
            "is_enabled_print_variable_update_summary", a_OBJECT);

        read_json(                                                 //
            &this->is_enabled_print_constraint_violation_summary,  //
            "is_enabled_print_constraint_violation_summary", a_OBJECT);

        read_json(                                                  //
            &this->is_enabled_print_violation_and_penalty_summary,  //
            "is_enabled_print_violation_and_penalty_summary", a_OBJECT);

        read_json(                                          //
            &this->is_enabled_print_tabu_search_parameter,  //
            "is_enabled_print_tabu_search_parameter", a_OBJECT);
    }

    /**************************************************************************/
    inline utility::json::JsonObject to_json(void) const {
        utility::json::JsonObject obj;
        obj.emplace_back(  //
            "verbose",     //
            verbose::VerboseInverseMap.at(this->verbose));

        obj.emplace_back(              //
            "is_enabled_write_trend",  //
            this->is_enabled_write_trend);

        obj.emplace_back(                           //
            "is_enabled_store_feasible_solutions",  //
            this->is_enabled_store_feasible_solutions);

        obj.emplace_back(                   //
            "feasible_solutions_capacity",  //
            this->feasible_solutions_capacity);

        obj.emplace_back(                                //
            "is_enabled_print_search_behavior_summary",  //
            this->is_enabled_print_search_behavior_summary);

        obj.emplace_back(                     //
            "is_enabled_print_tree_summary",  //
            this->is_enabled_print_tree_summary);

        obj.emplace_back(                                       //
            "is_enabled_print_thread_count_optimizer_summary",  //
            this->is_enabled_print_thread_count_optimizer_summary);

        obj.emplace_back(                                //
            "is_enabled_print_variable_update_summary",  //
            this->is_enabled_print_variable_update_summary);

        obj.emplace_back(                                     //
            "is_enabled_print_constraint_violation_summary",  //
            this->is_enabled_print_constraint_violation_summary);

        obj.emplace_back(                                      //
            "is_enabled_print_violation_and_penalty_summary",  //
            this->is_enabled_print_violation_and_penalty_summary);

        obj.emplace_back(                              //
            "is_enabled_print_tabu_search_parameter",  //
            this->is_enabled_print_tabu_search_parameter);

        return obj;
    }
};
}  // namespace printemps::option

#endif
/*****************************************************************************/
// END
/*****************************************************************************/