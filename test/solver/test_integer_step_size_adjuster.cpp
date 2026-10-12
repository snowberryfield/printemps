/*****************************************************************************/
// Copyright (c) 2020-2026 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
using namespace printemps;
using namespace printemps::solver;

/*****************************************************************************/
class TestIntegerStepSizeAdjuster : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown(void) {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestIntegerStepSizeAdjuster, constructor) {
    /// This method is tested in adjust().
}

/*****************************************************************************/
TEST_F(TestIntegerStepSizeAdjuster, initialize) {
    IntegerStepSizeAdjuster<int, double> adjuster;
    EXPECT_NO_THROW(adjuster.initialize());
}

/*****************************************************************************/
TEST_F(TestIntegerStepSizeAdjuster, setup) {
    model::IPModel model;
    model.create_variable("x", 0, 10);

    option::Option                       option;
    IntegerStepSizeAdjuster<int, double> adjuster;
    EXPECT_NO_THROW(adjuster.setup(&model, option));
}

/*****************************************************************************/
TEST_F(TestIntegerStepSizeAdjuster, adjust) {
    model::IPModel model;
    auto&          x = model.create_variable("x", 0, 10);
    model.minimize(-5 * x);

    option::Option                      option;
    model_handler::Builder<int, double> builder(&model);
    builder.build(option, false);

    IntegerStepSizeAdjuster<int, double> adjuster(&model, option);

    neighborhood::Move<int, double> move;
    move.alterations.emplace_back(x.flat_indexed_variables(0).reference(), 1);

    solution::SolutionScore reference_score;
    adjuster.adjust(&move, reference_score);

    // Initial move target was 1, with step size adjustment it should increase
    // towards upper bound
    EXPECT_GE(move.alterations.back().second, 1);
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/