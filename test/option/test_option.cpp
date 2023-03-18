/*****************************************************************************/
// Copyright (c) 2020-2023 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
using namespace printemps;
/*****************************************************************************/
class TestOption : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestOption, setup) {
    using namespace printemps::option;

    {
        Option option;
        option.setup("./test/dat/option/option_00.json");

        /// general
        EXPECT_EQ(  //
            1,      //
            option.general.iteration_max);

        EXPECT_FLOAT_EQ(  //
            0.1,          //
            option.general.time_max);

        EXPECT_FLOAT_EQ(  //
            0.1,          //
            option.general.time_offset);

        EXPECT_FLOAT_EQ(  //
            1E100,        //
            option.general.target_objective_value);

        EXPECT_EQ(  //
            2,      //
            option.general.seed);

        /// penalty
        EXPECT_FLOAT_EQ(  //
            0.1,          //
            option.penalty.penalty_coefficient_relaxing_rate);

        EXPECT_FLOAT_EQ(  //
            0.1,          //
            option.penalty.penalty_coefficient_tightening_rate);

        EXPECT_FLOAT_EQ(  //
            0.1,          //
            option.penalty.penalty_coefficient_updating_balance);

        EXPECT_FLOAT_EQ(  //
            0.1,          //
            option.penalty.initial_penalty_coefficient);

        EXPECT_EQ(  //
            true,   //
            option.penalty.is_enabled_group_penalty_coefficient);

        /// parallel
        EXPECT_EQ(  //
            false,  //
            option.parallel.is_enabled_parallel_evaluation);

        EXPECT_EQ(  //
            false,  //
            option.parallel.is_enabled_parallel_neighborhood_update);

        EXPECT_EQ(  //
            false,  //
            option.parallel.is_enabled_automatic_evaluation_parallelization);

        EXPECT_EQ(  //
            false,  //
            option.parallel
                .is_enabled_automatic_neighborhood_update_parallelization);

        EXPECT_FLOAT_EQ(  //
            0.6,          //
            option.parallel.evaluation_parallelization_decay_factor);

        EXPECT_FLOAT_EQ(  //
            0.6,          //
            option.parallel.neighborhood_update_parallelization_decay_factor);

        EXPECT_EQ(  //
            2,      //
            option.parallel.number_of_threads_evaluation);

        EXPECT_EQ(  //
            2,      //
            option.parallel.number_of_threads_neighborhood_update);

        /// preprocess
        EXPECT_EQ(  //
            false,  //
            option.preprocess.is_enabled_presolve);

        EXPECT_EQ(  //
            false,  //
            option.preprocess.is_enabled_remove_duplicated_constraints);

        EXPECT_EQ(  //
            false,  //
            option.preprocess.is_enabled_remove_redundant_set_variables);

        EXPECT_EQ(  //
            false,  //
            option.preprocess.is_enabled_remove_redundant_set_constraints);

        EXPECT_EQ(  //
            false,  //
            option.preprocess.is_enabled_extract_implicit_equality_constraints);

        EXPECT_EQ(  //
            false,  //
            option.preprocess.is_enabled_online_bounding);

        EXPECT_EQ(  //
            false,  //
            option.preprocess.is_enabled_initial_value_correction);

        EXPECT_EQ(  //
            false,  //
            option.preprocess.is_enabled_extract_dependent_exclusive_or);

        EXPECT_EQ(  //
            true,   //
            option.preprocess.is_enabled_extract_dependent_exclusive_nor);

        EXPECT_EQ(  //
            false,  //
            option.preprocess.is_enabled_extract_dependent_inverted_integers);

        EXPECT_EQ(  //
            true,   //
            option.preprocess.is_enabled_extract_dependent_balanced_integers);

        EXPECT_EQ(  //
            false,  //
            option.preprocess
                .is_enabled_extract_dependent_constant_sum_integers);

        EXPECT_EQ(  //
            false,  //
            option.preprocess
                .is_enabled_extract_dependent_constant_difference_integers);

        EXPECT_EQ(  //
            false,  //
            option.preprocess
                .is_enabled_extract_dependent_constant_ratio_integers);

        EXPECT_EQ(  //
            false, option.preprocess.is_enabled_extract_dependent_intermediate);

        /// neighborhood
        EXPECT_EQ(  //
            false,  //
            option.neighborhood.is_enabled_binary_move);

        EXPECT_EQ(  //
            false,  //
            option.neighborhood.is_enabled_integer_move);

        EXPECT_EQ(  //
            false,  //
            option.neighborhood.is_enabled_exclusive_or_move);

        EXPECT_EQ(  //
            false,  //
            option.neighborhood.is_enabled_exclusive_or_move);

        EXPECT_EQ(  //
            false,  //
            option.neighborhood.is_enabled_inverted_integers_move);

        EXPECT_EQ(  //
            false,  //
            option.neighborhood.is_enabled_balanced_integers_move);

        EXPECT_EQ(  //
            false,  //
            option.neighborhood.is_enabled_constant_sum_integers_move);

        EXPECT_EQ(
            false,
            option.neighborhood.is_enabled_constant_difference_integers_move);

        EXPECT_EQ(  //
            false,  //
            option.neighborhood.is_enabled_constant_ratio_integers_move);

        EXPECT_EQ(  //
            false,  //
            option.neighborhood.is_enabled_aggregation_move);

        EXPECT_EQ(  //
            true,   //
            option.neighborhood.is_enabled_precedence_move);

        EXPECT_EQ(  //
            true,   //
            option.neighborhood.is_enabled_variable_bound_move);

        EXPECT_EQ(  //
            true,   //
            option.neighborhood.is_enabled_soft_selection_move);

        EXPECT_EQ(  //
            false,  //
            option.neighborhood.is_enabled_chain_move);

        EXPECT_EQ(  //
            true,   //
            option.neighborhood.is_enabled_two_flip_move);

        EXPECT_EQ(  //
            1,      //
            option.neighborhood.chain_move_capacity);

        EXPECT_EQ(  //
            1,      //
            option.neighborhood.chain_move_reduce_mode);

        EXPECT_FLOAT_EQ(  //
            0.1,          //
            option.neighborhood.chain_move_overlap_rate_threshold);

        EXPECT_EQ(  //
            0,      //
            option.neighborhood.selection_mode);

        EXPECT_EQ(  //
            0,      //
            option.neighborhood.improvability_screening_mode);

        /// output
        EXPECT_EQ(  //
            0,      //
            option.output.verbose);

        EXPECT_EQ(  //
            true,   //
            option.output.is_enabled_write_trend);

        EXPECT_EQ(  //
            true,   //
            option.output.is_enabled_store_feasible_solutions);

        EXPECT_EQ(  //
            1,      //
            option.output.feasible_solutions_capacity);

        /// pdlp
        EXPECT_EQ(  //
            false,  //
            option.pdlp.is_enabled);

        EXPECT_EQ(  //
            1,      //
            option.pdlp.iteration_max);

        EXPECT_FLOAT_EQ(  //
            0.1,          //
            option.pdlp.time_max);

        EXPECT_FLOAT_EQ(  //
            0.1,          //
            option.pdlp.time_offset);

        EXPECT_FLOAT_EQ(  //
            0.1,          //
            option.pdlp.tolerance);

        EXPECT_FLOAT_EQ(  //
            0.1,          //
            option.pdlp.step_size_extend_exponent);

        EXPECT_FLOAT_EQ(  //
            0.1,          //
            option.pdlp.step_size_reduce_exponent);

        EXPECT_FLOAT_EQ(  //
            0.2,          //
            option.pdlp.restart_threshold_sufficient);

        EXPECT_FLOAT_EQ(  //
            0.1,          //
            option.pdlp.restart_threshold_necessary);

        EXPECT_FLOAT_EQ(  //
            0.1,          //
            option.pdlp.restart_threshold_artificial);

        EXPECT_EQ(  //
            1,      //
            option.pdlp.restart_check_interval);

        EXPECT_EQ(  //
            1,      //
            option.pdlp.convergence_check_interval);

        EXPECT_EQ(  //
            1,      //
            option.pdlp.counts_of_ruiz_scaling);

        EXPECT_EQ(  //
            false,  //
            option.pdlp.is_enabled_pock_chambolle_scaling);

        EXPECT_EQ(  //
            1,      //
            option.pdlp.log_interval);

        /// lagrange_dual
        EXPECT_EQ(  //
            true,   //
            option.lagrange_dual.is_enabled);

        EXPECT_EQ(  //
            1,      //
            option.lagrange_dual.iteration_max);

        EXPECT_FLOAT_EQ(  //
            0.1,          //
            option.lagrange_dual.time_max);

        EXPECT_FLOAT_EQ(  //
            0.1,          //
            option.lagrange_dual.time_offset);

        EXPECT_FLOAT_EQ(  //
            0.1,          //
            option.lagrange_dual.step_size_extend_rate);

        EXPECT_FLOAT_EQ(  //
            0.1,          //
            option.lagrange_dual.step_size_reduce_rate);

        EXPECT_FLOAT_EQ(  //
            0.1,          //
            option.lagrange_dual.tolerance);

        EXPECT_EQ(  //
            1,      //
            option.lagrange_dual.log_interval);

        /// local_search
        EXPECT_EQ(  //
            false,  //
            option.local_search.is_enabled);

        EXPECT_EQ(  //
            1,      //
            option.local_search.iteration_max);

        EXPECT_FLOAT_EQ(  //
            0.1,          //
            option.local_search.time_max);

        EXPECT_FLOAT_EQ(  //
            0.1,          //
            option.local_search.time_offset);

        EXPECT_EQ(  //
            1,      //
            option.local_search.log_interval);

        /// tabu_search
        EXPECT_EQ(  //
            1,      //
            option.tabu_search.iteration_max);

        EXPECT_FLOAT_EQ(  //
            0.1,          //
            option.tabu_search.time_max);

        EXPECT_FLOAT_EQ(  //
            0.1,          //
            option.tabu_search.time_offset);

        EXPECT_EQ(  //
            1,      //
            option.tabu_search.log_interval);

        EXPECT_EQ(  //
            1,      //
            option.tabu_search.initial_tabu_tenure);

        EXPECT_FLOAT_EQ(  //
            0.1,          //
            option.tabu_search.tabu_tenure_randomize_rate);

        EXPECT_FLOAT_EQ(  //
            0.1,          //
            option.tabu_search.initial_modification_fixed_rate);

        EXPECT_FLOAT_EQ(  //
            0.1,          //
            option.tabu_search.initial_modification_randomize_rate);

        EXPECT_EQ(  //
            1,      //
            option.tabu_search.tabu_mode);

        EXPECT_FLOAT_EQ(  //
            0.1,          //
            option.tabu_search.move_preserve_rate);

        EXPECT_FLOAT_EQ(  //
            0.1,          //
            option.tabu_search.frequency_penalty_coefficient);

        EXPECT_FLOAT_EQ(  //
            0.1,          //
            option.tabu_search.lagrangian_penalty_coefficient);

        EXPECT_FLOAT_EQ(  //
            0.1,          //
            option.tabu_search.pruning_rate_threshold);

        EXPECT_EQ(  //
            false,  //
            option.tabu_search.is_enabled_shuffle);

        EXPECT_EQ(  //
            true,   //
            option.tabu_search.is_enabled_move_curtail);

        EXPECT_EQ(  //
            false,  //
            option.tabu_search.is_enabled_automatic_break);

        EXPECT_EQ(
            false,
            option.tabu_search.is_enabled_automatic_tabu_tenure_adjustment);

        EXPECT_EQ(  //
            false,  //
            option.tabu_search.is_enabled_automatic_iteration_adjustment);

        EXPECT_EQ(  //
            false,  //
            option.tabu_search.is_enabled_initial_modification);

        EXPECT_EQ(  //
            1,      //
            option.tabu_search.intensity_increase_count_threshold);

        EXPECT_EQ(  //
            1,      //
            option.tabu_search.intensity_decrease_count_threshold);

        EXPECT_FLOAT_EQ(  //
            0.1,          //
            option.tabu_search.iteration_increase_rate);

        EXPECT_FLOAT_EQ(  //
            0.1,          //
            option.tabu_search.iteration_decrease_rate);

        EXPECT_EQ(  //
            false,  //
            option.tabu_search.ignore_tabu_if_global_incumbent);

        EXPECT_EQ(  //
            1,      //
            option.tabu_search.number_of_initial_modification);

        EXPECT_EQ(  //
            2,      //
            option.tabu_search.seed);
    }

    {
        Option option;
        option.setup("./test/dat/option/option_01.json");
        EXPECT_EQ(                                //
            chain_move_reduce_mode::OverlapRate,  //
            option.neighborhood.chain_move_reduce_mode);

        EXPECT_EQ(                //
            selection_mode::Off,  //
            option.neighborhood.selection_mode);

        EXPECT_EQ(                              //
            improvability_screening_mode::Off,  //
            option.neighborhood.improvability_screening_mode);

        EXPECT_EQ(         //
            verbose::Off,  //
            option.output.verbose);

        EXPECT_FLOAT_EQ(     //
            tabu_mode::All,  //
            option.tabu_search.tabu_mode);
    }

    {
        Option option;
        option.setup("./test/dat/option/option_02.json");
        EXPECT_EQ(                            //
            chain_move_reduce_mode::Shuffle,  //
            option.neighborhood.chain_move_reduce_mode);

        EXPECT_EQ(                        //
            selection_mode::Independent,  //
            option.neighborhood.selection_mode);

        EXPECT_EQ(                                    //
            improvability_screening_mode::Automatic,  //
            option.neighborhood.improvability_screening_mode);

        EXPECT_EQ(           //
            verbose::Debug,  //
            option.output.verbose);

        EXPECT_FLOAT_EQ(     //
            tabu_mode::Any,  //
            option.tabu_search.tabu_mode);
    }
}

/*****************************************************************************/
TEST_F(TestOption, to_json) {
    using namespace printemps::option;

    Option option;
    option.setup("./test/dat/option/option_00.json");
    auto obj = option.to_json();

    auto to_int    = [](auto v) { return std::any_cast<int>(v); };
    auto to_double = [](auto v) { return std::any_cast<double>(v); };
    auto to_bool   = [](auto v) { return std::any_cast<bool>(v); };
    auto to_str    = [](auto v) { return std::any_cast<std::string>(v); };

    /// general
    auto general = std::any_cast<utility::json::JsonObject>(obj.at("general"));

    EXPECT_EQ(  //
        1,      //
        to_int(general.at("iteration_max")));

    EXPECT_FLOAT_EQ(  //
        0.1,          //
        to_double(general.at("time_max")));

    EXPECT_FLOAT_EQ(  //
        0.1,          //
        to_double(general.at("time_offset")));

    EXPECT_FLOAT_EQ(  //
        1E100,        //
        to_double(general.at("target_objective_value")));

    EXPECT_EQ(  //
        2,      //
        to_int(general.at("seed")));

    /// penalty
    auto penalty = std::any_cast<utility::json::JsonObject>(obj.at("penalty"));

    EXPECT_FLOAT_EQ(  //
        0.1,          //
        to_double(penalty.at("penalty_coefficient_relaxing_rate")));

    EXPECT_FLOAT_EQ(  //
        0.1,          //
        to_double(penalty.at("penalty_coefficient_tightening_rate")));

    EXPECT_FLOAT_EQ(  //
        0.1,          //
        to_double(penalty.at("penalty_coefficient_updating_balance")));

    EXPECT_FLOAT_EQ(  //
        0.1,          //
        to_double(penalty.at("initial_penalty_coefficient")));

    EXPECT_EQ(  //
        true,   //
        to_bool(penalty.at("is_enabled_group_penalty_coefficient")));

    /// parallel
    auto parallel =
        std::any_cast<utility::json::JsonObject>(obj.at("parallel"));

    EXPECT_EQ(  //
        false,  //
        to_bool(parallel.at("is_enabled_parallel_evaluation")));

    EXPECT_EQ(  //
        false,  //
        to_bool(parallel.at("is_enabled_parallel_neighborhood_update")));

    EXPECT_EQ(  //
        false,  //
        to_bool(
            parallel.at("is_enabled_automatic_evaluation_parallelization")));

    EXPECT_EQ(  //
        false,  //
        to_bool(parallel.at(
            "is_enabled_automatic_neighborhood_update_parallelization")));

    /// preprocess
    auto preprocess =
        std::any_cast<utility::json::JsonObject>(obj.at("preprocess"));

    EXPECT_EQ(  //
        false,  //
        to_bool(preprocess.at("is_enabled_presolve")));

    EXPECT_EQ(  //
        false,  //
        to_bool(preprocess.at("is_enabled_remove_duplicated_constraints")));

    EXPECT_EQ(  //
        false,  //
        to_bool(preprocess.at("is_enabled_remove_redundant_set_variables")));

    EXPECT_EQ(  //
        false,  //
        to_bool(preprocess.at("is_enabled_remove_redundant_set_constraints")));

    EXPECT_EQ(  //
        false,  //
        to_bool(
            preprocess.at("is_enabled_extract_implicit_equality_constraints")));

    EXPECT_EQ(  //
        false,  //
        to_bool(preprocess.at("is_enabled_online_bounding")));

    EXPECT_EQ(  //
        false,  //
        to_bool(preprocess.at("is_enabled_initial_value_correction")));

    EXPECT_EQ(  //
        false,  //
        to_bool(preprocess.at("is_enabled_extract_dependent_exclusive_or")));

    EXPECT_EQ(  //
        true,   //
        to_bool(preprocess.at("is_enabled_extract_dependent_exclusive_nor")));

    EXPECT_EQ(  //
        false,  //
        to_bool(
            preprocess.at("is_enabled_extract_dependent_inverted_integers")));

    EXPECT_EQ(  //
        true,   //
        to_bool(
            preprocess.at("is_enabled_extract_dependent_balanced_integers")));

    EXPECT_EQ(  //
        false,  //
        to_bool(preprocess.at(
            "is_enabled_extract_dependent_constant_sum_integers")));

    EXPECT_EQ(  //
        false,  //
        to_bool(preprocess.at(
            "is_enabled_extract_dependent_constant_difference_integers")));

    EXPECT_EQ(  //
        false,  //
        to_bool(preprocess.at(
            "is_enabled_extract_dependent_constant_ratio_integers")));

    EXPECT_EQ(  //
        false,  //
        to_bool(preprocess.at("is_enabled_extract_dependent_intermediate")));

    /// neighborhood
    auto neighborhood =
        std::any_cast<utility::json::JsonObject>(obj.at("neighborhood"));

    EXPECT_EQ(  //
        false,  //
        to_bool(neighborhood.at("is_enabled_binary_move")));

    EXPECT_EQ(  //
        false,  //
        to_bool(neighborhood.at("is_enabled_integer_move")));

    EXPECT_EQ(  //
        false,  //
        to_bool(neighborhood.at("is_enabled_exclusive_or_move")));

    EXPECT_EQ(  //
        false,  //
        to_bool(neighborhood.at("is_enabled_exclusive_or_move")));

    EXPECT_EQ(  //
        false,  //
        to_bool(neighborhood.at("is_enabled_inverted_integers_move")));

    EXPECT_EQ(  //
        false,  //
        to_bool(neighborhood.at("is_enabled_balanced_integers_move")));

    EXPECT_EQ(  //
        false,  //
        to_bool(neighborhood.at("is_enabled_constant_sum_integers_move")));

    EXPECT_EQ(  //
        false,  //
        to_bool(
            neighborhood.at("is_enabled_constant_difference_integers_move")));

    EXPECT_EQ(  //
        false,  //
        to_bool(neighborhood.at("is_enabled_constant_ratio_integers_move")));

    EXPECT_EQ(  //
        false,  //
        to_bool(neighborhood.at("is_enabled_aggregation_move")));

    EXPECT_EQ(  //
        true,   //
        to_bool(neighborhood.at("is_enabled_precedence_move")));

    EXPECT_EQ(  //
        true,   //
        to_bool(neighborhood.at("is_enabled_variable_bound_move")));

    EXPECT_EQ(  //
        true,   //
        to_bool(neighborhood.at("is_enabled_soft_selection_move")));

    EXPECT_EQ(  //
        false,  //
        to_bool(neighborhood.at("is_enabled_chain_move")));

    EXPECT_EQ(  //
        true,   //
        to_bool(neighborhood.at("is_enabled_two_flip_move")));

    EXPECT_EQ(  //
        1,      //
        to_int(neighborhood.at("chain_move_capacity")));

    EXPECT_EQ(      //
        "Shuffle",  //
        to_str(neighborhood.at("chain_move_reduce_mode")));

    EXPECT_FLOAT_EQ(  //
        0.1,          //
        to_double(neighborhood.at("chain_move_overlap_rate_threshold")));

    EXPECT_EQ(  //
        "Off",  //
        to_str(neighborhood.at("selection_mode")));

    EXPECT_EQ(  //
        "Off",  //
        to_str(neighborhood.at("improvability_screening_mode")));

    /// output
    auto output = std::any_cast<utility::json::JsonObject>(obj.at("output"));

    EXPECT_EQ(  //
        "Off",  //
        to_str(output.at("verbose")));

    EXPECT_EQ(  //
        true,   //
        to_bool(output.at("is_enabled_write_trend")));

    EXPECT_EQ(  //
        true,   //
        to_bool(output.at("is_enabled_store_feasible_solutions")));

    EXPECT_EQ(  //
        1,      //
        to_int(output.at("feasible_solutions_capacity")));

    /// lagrange_dual
    auto lagrange_dual =
        std::any_cast<utility::json::JsonObject>(obj.at("lagrange_dual"));

    EXPECT_EQ(  //
        true,   //
        to_bool(lagrange_dual.at("is_enabled")));

    EXPECT_EQ(  //
        1,      //
        to_int(lagrange_dual.at("iteration_max")));

    EXPECT_FLOAT_EQ(  //
        0.1,          //
        to_double(lagrange_dual.at("time_max")));

    EXPECT_FLOAT_EQ(  //
        0.1,          //
        to_double(lagrange_dual.at("time_offset")));

    EXPECT_FLOAT_EQ(  //
        0.1,          //
        to_double(lagrange_dual.at("step_size_extend_rate")));

    EXPECT_FLOAT_EQ(  //
        0.1,          //
        to_double(lagrange_dual.at("step_size_reduce_rate")));

    EXPECT_FLOAT_EQ(  //
        0.1,          //
        to_double(lagrange_dual.at("tolerance")));

    EXPECT_EQ(  //
        1,      //
        to_int(lagrange_dual.at("log_interval")));

    /// local_search
    auto local_search =
        std::any_cast<utility::json::JsonObject>(obj.at("local_search"));

    EXPECT_EQ(  //
        false,  //
        to_bool(local_search.at("is_enabled")));

    EXPECT_EQ(  //
        1,      //
        to_int(local_search.at("iteration_max")));

    EXPECT_FLOAT_EQ(  //
        0.1,          //
        to_double(local_search.at("time_max")));

    EXPECT_FLOAT_EQ(  //
        0.1,          //
        to_double(local_search.at("time_offset")));

    EXPECT_EQ(  //
        1,      //
        to_int(local_search.at("log_interval")));

    /// tabu_search
    auto tabu_search =
        std::any_cast<utility::json::JsonObject>(obj.at("tabu_search"));

    EXPECT_EQ(  //
        1,      //
        to_int(tabu_search.at("iteration_max")));

    EXPECT_FLOAT_EQ(  //
        0.1,          //
        to_double(tabu_search.at("time_max")));

    EXPECT_FLOAT_EQ(  //
        0.1,          //
        to_double(tabu_search.at("time_offset")));

    EXPECT_EQ(  //
        1,      //
        to_int(tabu_search.at("log_interval")));

    EXPECT_EQ(  //
        1,      //
        to_int(tabu_search.at("initial_tabu_tenure")));

    EXPECT_FLOAT_EQ(  //
        0.1,          //
        to_double(tabu_search.at("tabu_tenure_randomize_rate")));

    EXPECT_FLOAT_EQ(  //
        0.1,          //
        to_double(tabu_search.at("initial_modification_fixed_rate")));

    EXPECT_FLOAT_EQ(  //
        0.1,          //
        to_double(tabu_search.at("initial_modification_randomize_rate")));

    EXPECT_EQ(  //
        "Any",  //
        to_str(tabu_search.at("tabu_mode")));

    EXPECT_FLOAT_EQ(  //
        0.1,          //
        to_double(tabu_search.at("move_preserve_rate")));

    EXPECT_FLOAT_EQ(  //
        0.1,          //
        to_double(tabu_search.at("frequency_penalty_coefficient")));

    EXPECT_FLOAT_EQ(  //
        0.1,          //
        to_double(tabu_search.at("lagrangian_penalty_coefficient")));

    EXPECT_FLOAT_EQ(  //
        0.1,          //
        to_double(tabu_search.at("pruning_rate_threshold")));

    EXPECT_EQ(  //
        false,  //
        to_bool(tabu_search.at("is_enabled_shuffle")));

    EXPECT_EQ(  //
        true,   //
        to_bool(tabu_search.at("is_enabled_move_curtail")));

    EXPECT_EQ(  //
        false,  //
        to_bool(tabu_search.at("is_enabled_automatic_break")));

    EXPECT_EQ(  //
        false,  //
        to_bool(tabu_search.at("is_enabled_automatic_tabu_tenure_adjustment")));

    EXPECT_EQ(  //
        false,  //
        to_bool(tabu_search.at("is_enabled_automatic_iteration_adjustment")));

    EXPECT_EQ(  //
        false,  //
        to_bool(tabu_search.at("is_enabled_initial_modification")));

    EXPECT_EQ(  //
        1,      //
        to_int(tabu_search.at("intensity_increase_count_threshold")));

    EXPECT_EQ(  //
        1,      //
        to_int(tabu_search.at("intensity_decrease_count_threshold")));

    EXPECT_FLOAT_EQ(  //
        0.1,          //
        to_double(tabu_search.at("iteration_increase_rate")));

    EXPECT_FLOAT_EQ(  //
        0.1,          //
        to_double(tabu_search.at("iteration_decrease_rate")));

    EXPECT_EQ(  //
        false,  //
        to_bool(tabu_search.at("ignore_tabu_if_global_incumbent")));

    EXPECT_EQ(  //
        1,      //
        to_int(tabu_search.at("number_of_initial_modification")));

    EXPECT_EQ(  //
        2,      //
        to_int(tabu_search.at("seed")));
}
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/