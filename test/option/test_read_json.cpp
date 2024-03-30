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
class TestReadJson : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestReadJson, read_json_int_or_double) {
    using namespace printemps;
    {
        utility::json::JsonObject object;
        std::string               key       = "int";
        std::string               wrong_key = "wrong";

        object.emplace_back(key, 1);

        double parameter = 0.0;
        EXPECT_TRUE(option::read_json(&parameter, key, object));
        EXPECT_FLOAT_EQ(1.0, parameter);

        EXPECT_FALSE(option::read_json(&parameter, wrong_key, object));
        EXPECT_FLOAT_EQ(1.0, parameter);
    }
    {
        utility::json::JsonObject object;

        std::string key       = "double";
        std::string wrong_key = "wrong";

        object.emplace_back(key, 2.0);

        double parameter = 0.0;
        option::read_json(&parameter, key, object);
        EXPECT_TRUE(option::read_json(&parameter, key, object));
        EXPECT_FLOAT_EQ(2.0, parameter);

        EXPECT_FALSE(option::read_json(&parameter, wrong_key, object));
        EXPECT_FLOAT_EQ(2.0, parameter);
    }
}

/*****************************************************************************/
TEST_F(TestReadJson, read_json_int_chain_move_reduce_mode) {
    using namespace printemps;
    {
        utility::json::JsonObject object;
        std::string               key       = "chain_move_reduce_mode";
        std::string               wrong_key = "wrong";

        object.emplace_back(key, 0);  // OverlapRate

        option::chain_move_reduce_mode::ChainMoveReduceMode parameter;
        EXPECT_TRUE(option::read_json(&parameter, key, object));
        EXPECT_EQ(option::chain_move_reduce_mode::OverlapRate, parameter);

        EXPECT_FALSE(option::read_json(&parameter, wrong_key, object));
        EXPECT_EQ(option::chain_move_reduce_mode::OverlapRate, parameter);
    }

    {
        utility::json::JsonObject object;
        std::string               key       = "chain_move_reduce_mode";
        std::string               wrong_key = "wrong";

        object.emplace_back(key, 1);  // Shuffle

        option::chain_move_reduce_mode::ChainMoveReduceMode parameter;
        EXPECT_TRUE(option::read_json(&parameter, key, object));
        EXPECT_EQ(option::chain_move_reduce_mode::Shuffle, parameter);

        EXPECT_FALSE(option::read_json(&parameter, wrong_key, object));
        EXPECT_EQ(option::chain_move_reduce_mode::Shuffle, parameter);
    }

    {
        utility::json::JsonObject object;
        std::string               key       = "chain_move_reduce_mode";
        std::string               wrong_key = "wrong";

        object.emplace_back(key, std::string("OverlapRate"));

        option::chain_move_reduce_mode::ChainMoveReduceMode parameter;
        EXPECT_TRUE(option::read_json(&parameter, key, object));
        EXPECT_EQ(option::chain_move_reduce_mode::OverlapRate, parameter);

        EXPECT_FALSE(option::read_json(&parameter, wrong_key, object));
        EXPECT_EQ(option::chain_move_reduce_mode::OverlapRate, parameter);
    }

    {
        utility::json::JsonObject object;
        std::string               key       = "chain_move_reduce_mode";
        std::string               wrong_key = "wrong";

        object.emplace_back(key, std::string("Shuffle"));

        option::chain_move_reduce_mode::ChainMoveReduceMode parameter;
        EXPECT_TRUE(option::read_json(&parameter, key, object));
        EXPECT_EQ(option::chain_move_reduce_mode::Shuffle, parameter);

        EXPECT_FALSE(option::read_json(&parameter, wrong_key, object));
        EXPECT_EQ(option::chain_move_reduce_mode::Shuffle, parameter);
    }
}

/*****************************************************************************/
TEST_F(TestReadJson, read_json_int_selection_mode) {
    using namespace printemps;
    {
        utility::json::JsonObject object;
        std::string               key       = "selection_mode";
        std::string               wrong_key = "wrong";

        object.emplace_back(key, 0);  // Off

        option::selection_mode::SelectionMode parameter;
        EXPECT_TRUE(option::read_json(&parameter, key, object));
        EXPECT_EQ(option::selection_mode::Off, parameter);

        EXPECT_FALSE(option::read_json(&parameter, wrong_key, object));
        EXPECT_EQ(option::selection_mode::Off, parameter);
    }

    {
        utility::json::JsonObject object;
        std::string               key       = "selection_mode";
        std::string               wrong_key = "wrong";

        object.emplace_back(key, 1);  // Defined

        option::selection_mode::SelectionMode parameter;
        EXPECT_TRUE(option::read_json(&parameter, key, object));
        EXPECT_EQ(option::selection_mode::Defined, parameter);

        EXPECT_FALSE(option::read_json(&parameter, wrong_key, object));
        EXPECT_EQ(option::selection_mode::Defined, parameter);
    }

    {
        utility::json::JsonObject object;
        std::string               key       = "selection_mode";
        std::string               wrong_key = "wrong";

        object.emplace_back(key, 2);  // Smaller

        option::selection_mode::SelectionMode parameter;
        EXPECT_TRUE(option::read_json(&parameter, key, object));
        EXPECT_EQ(option::selection_mode::Smaller, parameter);

        EXPECT_FALSE(option::read_json(&parameter, wrong_key, object));
        EXPECT_EQ(option::selection_mode::Smaller, parameter);
    }

    {
        utility::json::JsonObject object;
        std::string               key       = "selection_mode";
        std::string               wrong_key = "wrong";

        object.emplace_back(key, 3);  // Larger

        option::selection_mode::SelectionMode parameter;
        EXPECT_TRUE(option::read_json(&parameter, key, object));
        EXPECT_EQ(option::selection_mode::Larger, parameter);

        EXPECT_FALSE(option::read_json(&parameter, wrong_key, object));
        EXPECT_EQ(option::selection_mode::Larger, parameter);
    }

    {
        utility::json::JsonObject object;
        std::string               key       = "selection_mode";
        std::string               wrong_key = "wrong";

        object.emplace_back(key, 4);  // Independent

        option::selection_mode::SelectionMode parameter;
        EXPECT_TRUE(option::read_json(&parameter, key, object));
        EXPECT_EQ(option::selection_mode::Independent, parameter);

        EXPECT_FALSE(option::read_json(&parameter, wrong_key, object));
        EXPECT_EQ(option::selection_mode::Independent, parameter);
    }

    {
        utility::json::JsonObject object;
        std::string               key       = "selection_mode";
        std::string               wrong_key = "wrong";

        object.emplace_back(key, 5);  // UserDefined

        option::selection_mode::SelectionMode parameter;
        EXPECT_TRUE(option::read_json(&parameter, key, object));
        EXPECT_EQ(option::selection_mode::UserDefined, parameter);

        EXPECT_FALSE(option::read_json(&parameter, wrong_key, object));
        EXPECT_EQ(option::selection_mode::UserDefined, parameter);
    }

    {
        utility::json::JsonObject object;
        std::string               key       = "selection_mode";
        std::string               wrong_key = "wrong";

        object.emplace_back(key, std::string("Off"));

        option::selection_mode::SelectionMode parameter;
        EXPECT_TRUE(option::read_json(&parameter, key, object));
        EXPECT_EQ(option::selection_mode::Off, parameter);

        EXPECT_FALSE(option::read_json(&parameter, wrong_key, object));
        EXPECT_EQ(option::selection_mode::Off, parameter);
    }

    {
        utility::json::JsonObject object;
        std::string               key       = "selection_mode";
        std::string               wrong_key = "wrong";

        object.emplace_back(key, std::string("Defined"));

        option::selection_mode::SelectionMode parameter;
        EXPECT_TRUE(option::read_json(&parameter, key, object));
        EXPECT_EQ(option::selection_mode::Defined, parameter);

        EXPECT_FALSE(option::read_json(&parameter, wrong_key, object));
        EXPECT_EQ(option::selection_mode::Defined, parameter);
    }

    {
        utility::json::JsonObject object;
        std::string               key       = "selection_mode";
        std::string               wrong_key = "wrong";

        object.emplace_back(key, std::string("Smaller"));

        option::selection_mode::SelectionMode parameter;
        EXPECT_TRUE(option::read_json(&parameter, key, object));
        EXPECT_EQ(option::selection_mode::Smaller, parameter);

        EXPECT_FALSE(option::read_json(&parameter, wrong_key, object));
        EXPECT_EQ(option::selection_mode::Smaller, parameter);
    }

    {
        utility::json::JsonObject object;
        std::string               key       = "selection_mode";
        std::string               wrong_key = "wrong";

        object.emplace_back(key, std::string("Larger"));

        option::selection_mode::SelectionMode parameter;
        EXPECT_TRUE(option::read_json(&parameter, key, object));
        EXPECT_EQ(option::selection_mode::Larger, parameter);

        EXPECT_FALSE(option::read_json(&parameter, wrong_key, object));
        EXPECT_EQ(option::selection_mode::Larger, parameter);
    }

    {
        utility::json::JsonObject object;
        std::string               key       = "selection_mode";
        std::string               wrong_key = "wrong";

        object.emplace_back(key, std::string("Independent"));

        option::selection_mode::SelectionMode parameter;
        EXPECT_TRUE(option::read_json(&parameter, key, object));
        EXPECT_EQ(option::selection_mode::Independent, parameter);

        EXPECT_FALSE(option::read_json(&parameter, wrong_key, object));
        EXPECT_EQ(option::selection_mode::Independent, parameter);
    }

    {
        utility::json::JsonObject object;
        std::string               key       = "selection_mode";
        std::string               wrong_key = "wrong";

        object.emplace_back(key, std::string("UserDefined"));

        option::selection_mode::SelectionMode parameter;
        EXPECT_TRUE(option::read_json(&parameter, key, object));
        EXPECT_EQ(option::selection_mode::UserDefined, parameter);

        EXPECT_FALSE(option::read_json(&parameter, wrong_key, object));
        EXPECT_EQ(option::selection_mode::UserDefined, parameter);
    }
}

/*****************************************************************************/
TEST_F(TestReadJson, read_json_improvability_screening_mode) {
    using namespace printemps;
    {
        utility::json::JsonObject object;
        std::string               key       = "improvability_screening_mode";
        std::string               wrong_key = "wrong";

        object.emplace_back(key, 0);  // Off

        option::improvability_screening_mode::ImprovabilityScreeningMode
            parameter;
        EXPECT_TRUE(option::read_json(&parameter, key, object));
        EXPECT_EQ(option::improvability_screening_mode::Off, parameter);

        EXPECT_FALSE(option::read_json(&parameter, wrong_key, object));
        EXPECT_EQ(option::improvability_screening_mode::Off, parameter);
    }

    {
        utility::json::JsonObject object;
        std::string               key       = "improvability_screening_mode";
        std::string               wrong_key = "wrong";

        object.emplace_back(key, 1);  // Soft

        option::improvability_screening_mode::ImprovabilityScreeningMode
            parameter;
        EXPECT_TRUE(option::read_json(&parameter, key, object));
        EXPECT_EQ(option::improvability_screening_mode::Soft, parameter);

        EXPECT_FALSE(option::read_json(&parameter, wrong_key, object));
        EXPECT_EQ(option::improvability_screening_mode::Soft, parameter);
    }

    {
        utility::json::JsonObject object;
        std::string               key       = "improvability_screening_mode";
        std::string               wrong_key = "wrong";

        object.emplace_back(key, 2);  // Aggressive

        option::improvability_screening_mode::ImprovabilityScreeningMode
            parameter;
        EXPECT_TRUE(option::read_json(&parameter, key, object));
        EXPECT_EQ(option::improvability_screening_mode::Aggressive, parameter);

        EXPECT_FALSE(option::read_json(&parameter, wrong_key, object));
        EXPECT_EQ(option::improvability_screening_mode::Aggressive, parameter);
    }

    {
        utility::json::JsonObject object;
        std::string               key       = "improvability_screening_mode";
        std::string               wrong_key = "wrong";

        object.emplace_back(key, 3);  // Intensive

        option::improvability_screening_mode::ImprovabilityScreeningMode
            parameter;
        EXPECT_TRUE(option::read_json(&parameter, key, object));
        EXPECT_EQ(option::improvability_screening_mode::Intensive, parameter);

        EXPECT_FALSE(option::read_json(&parameter, wrong_key, object));
        EXPECT_EQ(option::improvability_screening_mode::Intensive, parameter);
    }

    {
        utility::json::JsonObject object;
        std::string               key       = "improvability_screening_mode";
        std::string               wrong_key = "wrong";

        object.emplace_back(key, 4);  // Automatic

        option::improvability_screening_mode::ImprovabilityScreeningMode
            parameter;
        EXPECT_TRUE(option::read_json(&parameter, key, object));
        EXPECT_EQ(option::improvability_screening_mode::Automatic, parameter);

        EXPECT_FALSE(option::read_json(&parameter, wrong_key, object));
        EXPECT_EQ(option::improvability_screening_mode::Automatic, parameter);
    }

    {
        utility::json::JsonObject object;
        std::string               key       = "improvability_screening_mode";
        std::string               wrong_key = "wrong";

        object.emplace_back(key, std::string("Off"));

        option::improvability_screening_mode::ImprovabilityScreeningMode
            parameter;
        EXPECT_TRUE(option::read_json(&parameter, key, object));
        EXPECT_EQ(option::improvability_screening_mode::Off, parameter);

        EXPECT_FALSE(option::read_json(&parameter, wrong_key, object));
        EXPECT_EQ(option::improvability_screening_mode::Off, parameter);
    }

    {
        utility::json::JsonObject object;
        std::string               key       = "improvability_screening_mode";
        std::string               wrong_key = "wrong";

        object.emplace_back(key, std::string("Soft"));

        option::improvability_screening_mode::ImprovabilityScreeningMode
            parameter;
        EXPECT_TRUE(option::read_json(&parameter, key, object));
        EXPECT_EQ(option::improvability_screening_mode::Soft, parameter);

        EXPECT_FALSE(option::read_json(&parameter, wrong_key, object));
        EXPECT_EQ(option::improvability_screening_mode::Soft, parameter);
    }

    {
        utility::json::JsonObject object;
        std::string               key       = "improvability_screening_mode";
        std::string               wrong_key = "wrong";

        object.emplace_back(key, std::string("Aggressive"));

        option::improvability_screening_mode::ImprovabilityScreeningMode
            parameter;
        EXPECT_TRUE(option::read_json(&parameter, key, object));
        EXPECT_EQ(option::improvability_screening_mode::Aggressive, parameter);

        EXPECT_FALSE(option::read_json(&parameter, wrong_key, object));
        EXPECT_EQ(option::improvability_screening_mode::Aggressive, parameter);
    }

    {
        utility::json::JsonObject object;
        std::string               key       = "improvability_screening_mode";
        std::string               wrong_key = "wrong";

        object.emplace_back(key, std::string("Intensive"));

        option::improvability_screening_mode::ImprovabilityScreeningMode
            parameter;
        EXPECT_TRUE(option::read_json(&parameter, key, object));
        EXPECT_EQ(option::improvability_screening_mode::Intensive, parameter);

        EXPECT_FALSE(option::read_json(&parameter, wrong_key, object));
        EXPECT_EQ(option::improvability_screening_mode::Intensive, parameter);
    }

    {
        utility::json::JsonObject object;
        std::string               key       = "improvability_screening_mode";
        std::string               wrong_key = "wrong";

        object.emplace_back(key, std::string("Automatic"));

        option::improvability_screening_mode::ImprovabilityScreeningMode
            parameter;
        EXPECT_TRUE(option::read_json(&parameter, key, object));
        EXPECT_EQ(option::improvability_screening_mode::Automatic, parameter);

        EXPECT_FALSE(option::read_json(&parameter, wrong_key, object));
        EXPECT_EQ(option::improvability_screening_mode::Automatic, parameter);
    }
}

/*****************************************************************************/
TEST_F(TestReadJson, read_json_verbose) {
    using namespace printemps;
    {
        utility::json::JsonObject object;
        std::string               key       = "verbose";
        std::string               wrong_key = "wrong";

        object.emplace_back(key, 0);  // Off

        option::verbose::Verbose parameter;
        EXPECT_TRUE(option::read_json(&parameter, key, object));
        EXPECT_EQ(option::verbose::Off, parameter);

        EXPECT_FALSE(option::read_json(&parameter, wrong_key, object));
        EXPECT_EQ(option::verbose::Off, parameter);
    }

    {
        utility::json::JsonObject object;
        std::string               key       = "verbose";
        std::string               wrong_key = "wrong";

        object.emplace_back(key, 1);  // Warning

        option::verbose::Verbose parameter;
        EXPECT_TRUE(option::read_json(&parameter, key, object));
        EXPECT_EQ(option::verbose::Warning, parameter);

        EXPECT_FALSE(option::read_json(&parameter, wrong_key, object));
        EXPECT_EQ(option::verbose::Warning, parameter);
    }
    {
        utility::json::JsonObject object;
        std::string               key       = "verbose";
        std::string               wrong_key = "wrong";

        object.emplace_back(key, 2);  // Outer

        option::verbose::Verbose parameter;
        EXPECT_TRUE(option::read_json(&parameter, key, object));
        EXPECT_EQ(option::verbose::Outer, parameter);

        EXPECT_FALSE(option::read_json(&parameter, wrong_key, object));
        EXPECT_EQ(option::verbose::Outer, parameter);
    }

    {
        utility::json::JsonObject object;
        std::string               key       = "verbose";
        std::string               wrong_key = "wrong";

        object.emplace_back(key, 3);  // Inner

        option::verbose::Verbose parameter;
        EXPECT_TRUE(option::read_json(&parameter, key, object));
        EXPECT_EQ(option::verbose::Inner, parameter);

        EXPECT_FALSE(option::read_json(&parameter, wrong_key, object));
        EXPECT_EQ(option::verbose::Inner, parameter);
    }

    {
        utility::json::JsonObject object;
        std::string               key       = "verbose";
        std::string               wrong_key = "wrong";

        object.emplace_back(key, 4);  // Full

        option::verbose::Verbose parameter;
        EXPECT_TRUE(option::read_json(&parameter, key, object));
        EXPECT_EQ(option::verbose::Full, parameter);

        EXPECT_FALSE(option::read_json(&parameter, wrong_key, object));
        EXPECT_EQ(option::verbose::Full, parameter);
    }

    {
        utility::json::JsonObject object;
        std::string               key       = "verbose";
        std::string               wrong_key = "wrong";

        object.emplace_back(key, 5);  // Debug

        option::verbose::Verbose parameter;
        EXPECT_TRUE(option::read_json(&parameter, key, object));
        EXPECT_EQ(option::verbose::Debug, parameter);

        EXPECT_FALSE(option::read_json(&parameter, wrong_key, object));
        EXPECT_EQ(option::verbose::Debug, parameter);
    }

    {
        utility::json::JsonObject object;
        std::string               key       = "verbose";
        std::string               wrong_key = "wrong";

        object.emplace_back(key, std::string("Off"));

        option::verbose::Verbose parameter;
        EXPECT_TRUE(option::read_json(&parameter, key, object));
        EXPECT_EQ(option::verbose::Off, parameter);

        EXPECT_FALSE(option::read_json(&parameter, wrong_key, object));
        EXPECT_EQ(option::verbose::Off, parameter);
    }

    {
        utility::json::JsonObject object;
        std::string               key       = "verbose";
        std::string               wrong_key = "wrong";

        object.emplace_back(key, std::string("Warning"));

        option::verbose::Verbose parameter;
        EXPECT_TRUE(option::read_json(&parameter, key, object));
        EXPECT_EQ(option::verbose::Warning, parameter);

        EXPECT_FALSE(option::read_json(&parameter, wrong_key, object));
        EXPECT_EQ(option::verbose::Warning, parameter);
    }
    {
        utility::json::JsonObject object;
        std::string               key       = "verbose";
        std::string               wrong_key = "wrong";

        object.emplace_back(key, std::string("Outer"));

        option::verbose::Verbose parameter;
        EXPECT_TRUE(option::read_json(&parameter, key, object));
        EXPECT_EQ(option::verbose::Outer, parameter);

        EXPECT_FALSE(option::read_json(&parameter, wrong_key, object));
        EXPECT_EQ(option::verbose::Outer, parameter);
    }

    {
        utility::json::JsonObject object;
        std::string               key       = "verbose";
        std::string               wrong_key = "wrong";

        object.emplace_back(key, std::string("Full"));

        option::verbose::Verbose parameter;
        EXPECT_TRUE(option::read_json(&parameter, key, object));
        EXPECT_EQ(option::verbose::Full, parameter);

        EXPECT_FALSE(option::read_json(&parameter, wrong_key, object));
        EXPECT_EQ(option::verbose::Full, parameter);
    }

    {
        utility::json::JsonObject object;
        std::string               key       = "verbose";
        std::string               wrong_key = "wrong";

        object.emplace_back(key, std::string("Debug"));  // Debug

        option::verbose::Verbose parameter;
        EXPECT_TRUE(option::read_json(&parameter, key, object));
        EXPECT_EQ(option::verbose::Debug, parameter);

        EXPECT_FALSE(option::read_json(&parameter, wrong_key, object));
        EXPECT_EQ(option::verbose::Debug, parameter);
    }
}

/*****************************************************************************/
TEST_F(TestReadJson, read_json_int_tabu_mode) {
    using namespace printemps;
    {
        utility::json::JsonObject object;
        std::string               key       = "tabu_mode";
        std::string               wrong_key = "wrong";

        object.emplace_back(key, 0);  // All

        option::tabu_mode::TabuMode parameter;
        EXPECT_TRUE(option::read_json(&parameter, key, object));
        EXPECT_EQ(option::tabu_mode::All, parameter);

        EXPECT_FALSE(option::read_json(&parameter, wrong_key, object));
        EXPECT_EQ(option::tabu_mode::All, parameter);
    }

    {
        utility::json::JsonObject object;
        std::string               key       = "tabu_mode";
        std::string               wrong_key = "wrong";

        object.emplace_back(key, 1);  // Any

        option::tabu_mode::TabuMode parameter;
        EXPECT_TRUE(option::read_json(&parameter, key, object));
        EXPECT_EQ(option::tabu_mode::Any, parameter);

        EXPECT_FALSE(option::read_json(&parameter, wrong_key, object));
        EXPECT_EQ(option::tabu_mode::Any, parameter);
    }

    {
        utility::json::JsonObject object;
        std::string               key       = "tabu_mode";
        std::string               wrong_key = "wrong";

        object.emplace_back(key, std::string("All"));

        option::tabu_mode::TabuMode parameter;
        EXPECT_TRUE(option::read_json(&parameter, key, object));
        EXPECT_EQ(option::tabu_mode::All, parameter);

        EXPECT_FALSE(option::read_json(&parameter, wrong_key, object));
        EXPECT_EQ(option::tabu_mode::All, parameter);
    }

    {
        utility::json::JsonObject object;
        std::string               key       = "tabu_mode";
        std::string               wrong_key = "wrong";

        object.emplace_back(key, std::string("Any"));

        option::tabu_mode::TabuMode parameter;
        EXPECT_TRUE(option::read_json(&parameter, key, object));
        EXPECT_EQ(option::tabu_mode::Any, parameter);

        EXPECT_FALSE(option::read_json(&parameter, wrong_key, object));
        EXPECT_EQ(option::tabu_mode::Any, parameter);
    }
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/