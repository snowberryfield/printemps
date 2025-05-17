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
class TestPB : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
    std::string common_terms_string(void) {
        return "1 x1 -2 x2 x1 +3 x3 x3 -4 ~x4 x4 +5 x1 ~x2 x3 ~x4";
    }
};

/*****************************************************************************/
TEST_F(TestPB, initialize) {
    pb::PB pb;

    EXPECT_EQ("", pb.name);

    EXPECT_TRUE(pb.objective.is_minimization);
    EXPECT_EQ("", pb.objective.name);
    EXPECT_TRUE(pb.objective.terms.empty());

    EXPECT_TRUE(pb.soft_constraints.empty());
    EXPECT_TRUE(pb.hard_constraints.empty());

    EXPECT_EQ(0, pb.metadata.number_of_variables);
    EXPECT_EQ(0, pb.metadata.number_of_constraints);
    EXPECT_EQ(0, pb.metadata.number_of_equals);
    EXPECT_EQ(0, pb.metadata.number_of_products);
    EXPECT_EQ(0, pb.metadata.intsize);
    EXPECT_EQ(0, pb.metadata.sizeproduct);
    EXPECT_EQ(0, pb.metadata.number_of_soft_constraints);
    EXPECT_EQ(0, pb.metadata.mincost);
    EXPECT_EQ(0, pb.metadata.maxcost);
    EXPECT_EQ(0, pb.metadata.sumcost);

    EXPECT_FALSE(pb.top_cost.is_defined);
    EXPECT_EQ(std::numeric_limits<int>::max(), pb.top_cost.value);

    EXPECT_TRUE(pb.variable_names.empty());
    EXPECT_TRUE(pb.negated_variable_names.empty());
    EXPECT_TRUE(pb.product_variable_names.empty());
}

/*****************************************************************************/
TEST_F(TestPB, parse_metadata) {
    {
        const auto METADATA = pb::PB::parse_metadata(
            "* #variable= 1 #constraint= 2 #equal= 3 #product= 4 intsize= 5 "
            "sizeproduct= 6 #soft= 7 mincost= 8 maxcost= 9 sumcost= 10");
        EXPECT_EQ(1, METADATA.number_of_variables);
        EXPECT_EQ(2, METADATA.number_of_constraints);
        EXPECT_EQ(3, METADATA.number_of_equals);
        EXPECT_EQ(4, METADATA.number_of_products);
        EXPECT_EQ(5, METADATA.intsize);
        EXPECT_EQ(6, METADATA.sizeproduct);
        EXPECT_EQ(7, METADATA.number_of_soft_constraints);
        EXPECT_EQ(8, METADATA.mincost);
        EXPECT_EQ(9, METADATA.maxcost);
        EXPECT_EQ(10, METADATA.sumcost);
    }

    {
        const auto METADATA = pb::PB::parse_metadata(
            "*#variable= 1 #constraint= 2 #equal= 3 #product= 4 intsize= 5 "
            "sizeproduct= 6 #soft= 7 mincost= 8 maxcost= 9 sumcost= 10");
        EXPECT_EQ(1, METADATA.number_of_variables);
        EXPECT_EQ(2, METADATA.number_of_constraints);
        EXPECT_EQ(3, METADATA.number_of_equals);
        EXPECT_EQ(4, METADATA.number_of_products);
        EXPECT_EQ(5, METADATA.intsize);
        EXPECT_EQ(6, METADATA.sizeproduct);
        EXPECT_EQ(7, METADATA.number_of_soft_constraints);
        EXPECT_EQ(8, METADATA.mincost);
        EXPECT_EQ(9, METADATA.maxcost);
        EXPECT_EQ(10, METADATA.sumcost);
    }

    {
        const auto METADATA = pb::PB::parse_metadata("* #variable= 1");
        EXPECT_EQ(1, METADATA.number_of_variables);
        EXPECT_EQ(0, METADATA.number_of_constraints);
        EXPECT_EQ(0, METADATA.number_of_equals);
        EXPECT_EQ(0, METADATA.number_of_products);
        EXPECT_EQ(0, METADATA.intsize);
        EXPECT_EQ(0, METADATA.sizeproduct);
        EXPECT_EQ(0, METADATA.number_of_soft_constraints);
        EXPECT_EQ(0, METADATA.mincost);
        EXPECT_EQ(0, METADATA.maxcost);
        EXPECT_EQ(0, METADATA.sumcost);
    }
}

/*****************************************************************************/
TEST_F(TestPB, parse_top_cost) {
    {
        const auto TOP_COST = pb::PB::parse_top_cost("soft:");
        EXPECT_FALSE(TOP_COST.is_defined);
        EXPECT_EQ(std::numeric_limits<int>::max(), TOP_COST.value);
    }

    {
        const auto TOP_COST = pb::PB::parse_top_cost("Soft:");
        EXPECT_FALSE(TOP_COST.is_defined);
        EXPECT_EQ(std::numeric_limits<int>::max(), TOP_COST.value);
    }

    {
        const auto TOP_COST = pb::PB::parse_top_cost("soft: 10");
        EXPECT_TRUE(TOP_COST.is_defined);
        EXPECT_EQ(10, TOP_COST.value);
    }

    {
        const auto TOP_COST = pb::PB::parse_top_cost("Soft: 10");
        EXPECT_TRUE(TOP_COST.is_defined);
        EXPECT_EQ(10, TOP_COST.value);
    }

    {
        /// Unnecessary space before colon.
        ASSERT_THROW(pb::PB::parse_top_cost("soft : 10"), std::runtime_error);
    }

    {
        /// Extra value definition.
        ASSERT_THROW(pb::PB::parse_top_cost("soft: 10 20"), std::runtime_error);
    }
}

/*****************************************************************************/
TEST_F(TestPB, parse_objective) {
    {
        const auto OBJECTIVE =
            pb::PB::parse_objective("min: " + common_terms_string());
        EXPECT_TRUE(OBJECTIVE.is_minimization);
        EXPECT_EQ(5, static_cast<int>(OBJECTIVE.terms.size()));
    }

    {
        const auto OBJECTIVE =
            pb::PB::parse_objective("Min: " + common_terms_string());
        EXPECT_TRUE(OBJECTIVE.is_minimization);
        EXPECT_EQ(5, static_cast<int>(OBJECTIVE.terms.size()));
    }

    {
        const auto OBJECTIVE =
            pb::PB::parse_objective("max: " + common_terms_string());
        EXPECT_FALSE(OBJECTIVE.is_minimization);
        EXPECT_EQ(5, static_cast<int>(OBJECTIVE.terms.size()));
    }

    {
        const auto OBJECTIVE =
            pb::PB::parse_objective("Max: " + common_terms_string());
        EXPECT_FALSE(OBJECTIVE.is_minimization);
        EXPECT_EQ(5, static_cast<int>(OBJECTIVE.terms.size()));
    }

    {
        ASSERT_THROW(pb::PB::parse_objective("minn: " + common_terms_string()),
                     std::runtime_error);
    }
}

/*****************************************************************************/
TEST_F(TestPB, parse_soft_constraint) {
    {
        const auto SOFT_CONSTRAINT = pb::PB::parse_soft_constraint(
            "[1]" + common_terms_string() + "<=10", 100);

        EXPECT_EQ(1, SOFT_CONSTRAINT.weight);
        EXPECT_EQ(pb::PBConstraintSense::Less, SOFT_CONSTRAINT.sense);
        EXPECT_EQ(10, SOFT_CONSTRAINT.rhs);
        EXPECT_EQ(5, static_cast<int>(SOFT_CONSTRAINT.terms.size()));
        EXPECT_EQ("soft_constraint_100", SOFT_CONSTRAINT.name);
    }
    {
        const auto SOFT_CONSTRAINT = pb::PB::parse_soft_constraint(
            "[2]" + common_terms_string() + "=20", 200);

        EXPECT_EQ(2, SOFT_CONSTRAINT.weight);
        EXPECT_EQ(pb::PBConstraintSense::Equal, SOFT_CONSTRAINT.sense);
        EXPECT_EQ(20, SOFT_CONSTRAINT.rhs);
        EXPECT_EQ(5, static_cast<int>(SOFT_CONSTRAINT.terms.size()));
        EXPECT_EQ("soft_constraint_200", SOFT_CONSTRAINT.name);
    }
    {
        const auto SOFT_CONSTRAINT = pb::PB::parse_soft_constraint(
            "[3]" + common_terms_string() + ">=30", 300);

        EXPECT_EQ(3, SOFT_CONSTRAINT.weight);
        EXPECT_EQ(pb::PBConstraintSense::Greater, SOFT_CONSTRAINT.sense);
        EXPECT_EQ(30, SOFT_CONSTRAINT.rhs);
        EXPECT_EQ(5, static_cast<int>(SOFT_CONSTRAINT.terms.size()));
        EXPECT_EQ("soft_constraint_300", SOFT_CONSTRAINT.name);
    }
    {
        const auto SOFT_CONSTRAINT = pb::PB::parse_soft_constraint(
            "[ 4 ]" + common_terms_string() + " <=40", 400);

        EXPECT_EQ(4, SOFT_CONSTRAINT.weight);
        EXPECT_EQ(pb::PBConstraintSense::Less, SOFT_CONSTRAINT.sense);
        EXPECT_EQ(40, SOFT_CONSTRAINT.rhs);
        EXPECT_EQ(5, static_cast<int>(SOFT_CONSTRAINT.terms.size()));
        EXPECT_EQ("soft_constraint_400", SOFT_CONSTRAINT.name);
    }
    {
        const auto SOFT_CONSTRAINT = pb::PB::parse_soft_constraint(
            "[ 5 ]" + common_terms_string() + " = 50", 500);

        EXPECT_EQ(5, SOFT_CONSTRAINT.weight);
        EXPECT_EQ(pb::PBConstraintSense::Equal, SOFT_CONSTRAINT.sense);
        EXPECT_EQ(50, SOFT_CONSTRAINT.rhs);
        EXPECT_EQ(5, static_cast<int>(SOFT_CONSTRAINT.terms.size()));
        EXPECT_EQ("soft_constraint_500", SOFT_CONSTRAINT.name);
    }
    {
        const auto SOFT_CONSTRAINT = pb::PB::parse_soft_constraint(
            "[ 6 ]" + common_terms_string() + " >= 60", 600);

        EXPECT_EQ(6, SOFT_CONSTRAINT.weight);
        EXPECT_EQ(pb::PBConstraintSense::Greater, SOFT_CONSTRAINT.sense);
        EXPECT_EQ(60, SOFT_CONSTRAINT.rhs);
        EXPECT_EQ(5, static_cast<int>(SOFT_CONSTRAINT.terms.size()));
        EXPECT_EQ("soft_constraint_600", SOFT_CONSTRAINT.name);
    }
    {
        /// Unnecessary space before colon.
        ASSERT_THROW(pb::PB::parse_soft_constraint(
                         "[1 " + common_terms_string() + "<=10", 0),
                     std::runtime_error);
    }
}

/*****************************************************************************/
TEST_F(TestPB, parse_hard_constraint) {
    {
        const auto HARD_CONSTRAINT =
            pb::PB::parse_hard_constraint(common_terms_string() + "<=10", 100);

        EXPECT_EQ(pb::PBConstraintSense::Less, HARD_CONSTRAINT.sense);
        EXPECT_EQ(10, HARD_CONSTRAINT.rhs);
        EXPECT_EQ(5, static_cast<int>(HARD_CONSTRAINT.terms.size()));
        EXPECT_EQ("hard_constraint_100", HARD_CONSTRAINT.name);
    }
    {
        const auto HARD_CONSTRAINT =
            pb::PB::parse_hard_constraint(common_terms_string() + "=20", 200);

        EXPECT_EQ(pb::PBConstraintSense::Equal, HARD_CONSTRAINT.sense);
        EXPECT_EQ(20, HARD_CONSTRAINT.rhs);
        EXPECT_EQ(5, static_cast<int>(HARD_CONSTRAINT.terms.size()));
        EXPECT_EQ("hard_constraint_200", HARD_CONSTRAINT.name);
    }
    {
        const auto HARD_CONSTRAINT =
            pb::PB::parse_hard_constraint(common_terms_string() + ">=30", 300);

        EXPECT_EQ(pb::PBConstraintSense::Greater, HARD_CONSTRAINT.sense);
        EXPECT_EQ(30, HARD_CONSTRAINT.rhs);
        EXPECT_EQ(5, static_cast<int>(HARD_CONSTRAINT.terms.size()));
        EXPECT_EQ("hard_constraint_300", HARD_CONSTRAINT.name);
    }
    {
        const auto HARD_CONSTRAINT =
            pb::PB::parse_hard_constraint(common_terms_string() + " <=40", 400);

        EXPECT_EQ(pb::PBConstraintSense::Less, HARD_CONSTRAINT.sense);
        EXPECT_EQ(40, HARD_CONSTRAINT.rhs);
        EXPECT_EQ(5, static_cast<int>(HARD_CONSTRAINT.terms.size()));
        EXPECT_EQ("hard_constraint_400", HARD_CONSTRAINT.name);
    }
    {
        const auto HARD_CONSTRAINT =
            pb::PB::parse_hard_constraint(common_terms_string() + " = 50", 500);

        EXPECT_EQ(pb::PBConstraintSense::Equal, HARD_CONSTRAINT.sense);
        EXPECT_EQ(50, HARD_CONSTRAINT.rhs);
        EXPECT_EQ(5, static_cast<int>(HARD_CONSTRAINT.terms.size()));
        EXPECT_EQ("hard_constraint_500", HARD_CONSTRAINT.name);
    }
    {
        const auto HARD_CONSTRAINT = pb::PB::parse_hard_constraint(
            common_terms_string() + " >= 60", 600);

        EXPECT_EQ(pb::PBConstraintSense::Greater, HARD_CONSTRAINT.sense);
        EXPECT_EQ(60, HARD_CONSTRAINT.rhs);
        EXPECT_EQ(5, static_cast<int>(HARD_CONSTRAINT.terms.size()));
        EXPECT_EQ("hard_constraint_600", HARD_CONSTRAINT.name);
    }
}

/*****************************************************************************/
TEST_F(TestPB, parse_constraint) {
    /// This method is virtually tested in parse_hard_constraint().
}

/*****************************************************************************/
TEST_F(TestPB, setup_variable_information) {
    /// This method is virtually tested in read_pb().
}

/*****************************************************************************/
TEST_F(TestPB, read_pb_00) {
    {
        pb::PB pb;
        pb.read_pb("./test/dat/pb/test_00a.pb");
        EXPECT_EQ(0, static_cast<int>(pb.soft_constraints.size()));
        EXPECT_EQ(4, static_cast<int>(pb.hard_constraints.size()));
        EXPECT_EQ(5, static_cast<int>(pb.variable_names.size()));
        EXPECT_EQ(0, static_cast<int>(pb.negated_variable_names.size()));
        EXPECT_EQ(0, static_cast<int>(pb.product_variable_names.size()));
    }

    {
        pb::PB pb;
        pb.read_pb("./test/dat/pb/test_00b.pb");
        EXPECT_EQ(0, static_cast<int>(pb.soft_constraints.size()));
        EXPECT_EQ(4, static_cast<int>(pb.hard_constraints.size()));
        EXPECT_EQ(5, static_cast<int>(pb.variable_names.size()));
        EXPECT_EQ(5, static_cast<int>(pb.negated_variable_names.size()));
        EXPECT_EQ(5, static_cast<int>(pb.product_variable_names.size()));
    }

    {
        pb::PB pb;
        pb.read_pb("./test/dat/pb/test_00c.pb");
        EXPECT_EQ(0, static_cast<int>(pb.soft_constraints.size()));
        EXPECT_EQ(3, static_cast<int>(pb.hard_constraints.size()));
        EXPECT_EQ(6, static_cast<int>(pb.variable_names.size()));
        EXPECT_EQ(0, static_cast<int>(pb.negated_variable_names.size()));
        EXPECT_EQ(9, static_cast<int>(pb.product_variable_names.size()));
    }

    {
        pb::PB pb;
        pb.read_pb("./test/dat/pb/test_00d.pb");
        EXPECT_EQ(4, static_cast<int>(pb.soft_constraints.size()));
        EXPECT_EQ(2, static_cast<int>(pb.hard_constraints.size()));
        EXPECT_EQ(4, static_cast<int>(pb.variable_names.size()));
        EXPECT_EQ(0, static_cast<int>(pb.negated_variable_names.size()));
        EXPECT_EQ(0, static_cast<int>(pb.product_variable_names.size()));
    }
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/