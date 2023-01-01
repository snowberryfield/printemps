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
class TestImprovabilityScreeningMode : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestImprovabilityScreeningMode, ImprovabilityScreeningModeMap) {
    using namespace printemps::option::improvability_screening_mode;
    EXPECT_EQ(Off, ImprovabilityScreeningModeMap.at("Off"));
    EXPECT_EQ(Soft, ImprovabilityScreeningModeMap.at("Soft"));
    EXPECT_EQ(Aggressive, ImprovabilityScreeningModeMap.at("Aggressive"));
    EXPECT_EQ(Intensive, ImprovabilityScreeningModeMap.at("Intensive"));
    EXPECT_EQ(Automatic, ImprovabilityScreeningModeMap.at("Automatic"));
}

/*****************************************************************************/
TEST_F(TestImprovabilityScreeningMode, ImprovabilityScreeningModeInverseMap) {
    using namespace printemps::option::improvability_screening_mode;
    EXPECT_EQ("Off", ImprovabilityScreeningModeInverseMap.at(Off));
    EXPECT_EQ("Soft", ImprovabilityScreeningModeInverseMap.at(Soft));
    EXPECT_EQ("Aggressive",
              ImprovabilityScreeningModeInverseMap.at(Aggressive));
    EXPECT_EQ("Intensive", ImprovabilityScreeningModeInverseMap.at(Intensive));
    EXPECT_EQ("Automatic", ImprovabilityScreeningModeInverseMap.at(Automatic));
}
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/