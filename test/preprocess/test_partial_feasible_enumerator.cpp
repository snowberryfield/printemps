/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
using namespace printemps;
/*****************************************************************************/
class TestPartialFeasibleEnumerator : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestPartialFeasibleEnumerator, run) {
    {  // basic test
        model::Model<int, double> model;

        auto& x = model.create_variables("x", 4, 0, 1);
        auto& f = model.create_constraints("f", 2);
        f(0)    = x(0) + x(1) - x(2) - x(3) == 1;
        f(1)    = x(0) + x(1) + x(2) + x(3) == 3;
        model.builder().setup_unique_names();
        model.builder().update_derived_components();
        model.partial_feasible_enumerator().run(false);

        const auto small_constraint_groups =
            model.partial_feasible_enumerator().small_constraint_groups();

        EXPECT_EQ(1, static_cast<int>(small_constraint_groups.size()));
        EXPECT_EQ(2, static_cast<int>(
                         small_constraint_groups[0].constraint_ptrs.size()));
        EXPECT_EQ(4, static_cast<int>(
                         small_constraint_groups[0].variable_ptrs.size()));
        EXPECT_EQ(
            2, static_cast<int>(small_constraint_groups[0].solutions.size()));
    }

    { /**
       * No small constraint groups because there are 4 feasible solutions,
       * which is more than the number of variables.
       */
        model::Model<int, double> model;

        auto& x = model.create_variables("x", 4, 0, 1);
        auto& f = model.create_constraints("f", 2);
        f(0)    = x(0) + x(1) - x(2) - x(3) == 0;
        f(1)    = x(0) + x(1) + x(2) + x(3) == 2;
        model.builder().setup_unique_names();
        model.builder().update_derived_components();
        model.partial_feasible_enumerator().run(false);

        const auto small_constraint_groups =
            model.partial_feasible_enumerator().small_constraint_groups();

        EXPECT_EQ(0, static_cast<int>(small_constraint_groups.size()));
    }

    { /**
       * No small constraint groups because the constraints include integer
       * variables.
       */
        model::Model<int, double> model;

        auto& x = model.create_variables("x", 4, 0, 2);
        auto& f = model.create_constraints("f", 2);
        f(0)    = x(0) + x(1) - x(2) - x(3) == 1;
        f(1)    = x(0) + x(1) + x(2) + x(3) == 3;
        model.builder().setup_unique_names();
        model.builder().update_derived_components();
        model.partial_feasible_enumerator().run(false);

        const auto small_constraint_groups =
            model.partial_feasible_enumerator().small_constraint_groups();

        EXPECT_EQ(0, static_cast<int>(small_constraint_groups.size()));
    }

    { /**
       * No small constraint groups because the number of variables is less than
       * lower limit (3).
       */
        model::Model<int, double> model;

        auto& x = model.create_variables("x", 2, 0, 1);
        auto& f = model.create_constraints("f", 2);
        f(0)    = x(0) + x(1) <= 1;
        f(1)    = x(0) + x(1) >= 1;
        model.builder().setup_unique_names();
        model.builder().update_derived_components();
        model.partial_feasible_enumerator().run(false);

        const auto small_constraint_groups =
            model.partial_feasible_enumerator().small_constraint_groups();

        EXPECT_EQ(0, static_cast<int>(small_constraint_groups.size()));
    }

    { /**
       * No small constraint groups because the number of variables is greater
       * than lower limit (8).
       */
        model::Model<int, double> model;

        auto& x = model.create_variables("x", 9, 0, 1);
        auto& f = model.create_constraints("f", 2);
        f(0)    = x.sum() + 9 * (x(0) + x(1)) == 20;
        f(1)    = x.sum() >= 2;
        model.builder().setup_unique_names();
        model.builder().update_derived_components();
        model.partial_feasible_enumerator().run(false);

        const auto small_constraint_groups =
            model.partial_feasible_enumerator().small_constraint_groups();

        EXPECT_EQ(0, static_cast<int>(small_constraint_groups.size()));
    }

    { /**
       * No small constraint groups because the number of constraints is less
       * than lower limit (2).
       */
        model::Model<int, double> model;

        auto& x = model.create_variables("x", 3, 0, 1);
        auto& f = model.create_constraints("f", 1);
        f       = x(0) + 5 * (x(1) + x(2)) == 6;
        model.builder().setup_unique_names();
        model.builder().update_derived_components();
        model.partial_feasible_enumerator().run(false);

        const auto small_constraint_groups =
            model.partial_feasible_enumerator().small_constraint_groups();

        EXPECT_EQ(0, static_cast<int>(small_constraint_groups.size()));
    }
}

/*****************************************************************************/
TEST_F(TestPartialFeasibleEnumerator, small_constraint_groups) {
    /// This test is covered by run().
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/