/*****************************************************************************/
// Copyright (c) 2020-2026 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>
#include "extra/pb_competition/pb_competition_solver.h"

namespace {
using namespace printemps;
namespace pbc = printemps::extra::pb_competition;

/*****************************************************************************/
class TestPBCompetitionSolver : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
/*****************************************************************************/
TEST_F(TestPBCompetitionSolver, supported_pure_opb_intsize_53) {
    const char              *argv[] = {"pb_competition_solver",
                                       "./dat/pb/test_pbc_supported_pure.opb"};
    pbc::PBCompetitionSolver app;
    app.setup(2, argv);
    EXPECT_NO_THROW(app.solve());
}

/*****************************************************************************/
TEST_F(TestPBCompetitionSolver, supported_wbo_intsize_52) {
    const char              *argv[] = {"pb_competition_solver",
                                       "./dat/pb/test_pbc_supported_wbo.wbo"};
    pbc::PBCompetitionSolver app;
    app.setup(2, argv);
    EXPECT_NO_THROW(app.solve());
}

/*****************************************************************************/
TEST_F(TestPBCompetitionSolver, supported_wbo_border) {
    const char              *argv[] = {"pb_competition_solver",
                                       "./dat/pb/test_pbc_supported_wbo_border.wbo"};
    pbc::PBCompetitionSolver app;
    app.setup(2, argv);
    EXPECT_NO_THROW(app.solve());
}

/*****************************************************************************/
TEST_F(TestPBCompetitionSolver, unsupported_pure_opb_intsize_54) {
    EXPECT_EXIT(([]() {
                    std::cout.rdbuf(std::cerr.rdbuf());
                    const char *argv[] = {
                        "pb_competition_solver",
                        "./dat/pb/test_pbc_unsupported_pure.opb"};
                    pbc::PBCompetitionSolver app;
                    app.setup(2, argv);
                    app.solve();
                }()),
                ::testing::ExitedWithCode(0), "s UNSUPPORTED");
}

/*****************************************************************************/
TEST_F(TestPBCompetitionSolver, unsupported_wbo_intsize_53) {
    EXPECT_EXIT(([]() {
                    std::cout.rdbuf(std::cerr.rdbuf());
                    const char *argv[] = {
                        "pb_competition_solver",
                        "./dat/pb/test_pbc_unsupported_wbo_intsize.wbo"};
                    pbc::PBCompetitionSolver app;
                    app.setup(2, argv);
                    app.solve();
                }()),
                ::testing::ExitedWithCode(0), "s UNSUPPORTED");
}

/*****************************************************************************/
TEST_F(TestPBCompetitionSolver, unsupported_wbo_sumcost) {
    EXPECT_EXIT(([]() {
                    std::cout.rdbuf(std::cerr.rdbuf());
                    const char *argv[] = {
                        "pb_competition_solver",
                        "./dat/pb/test_pbc_unsupported_wbo_sumcost.wbo"};
                    pbc::PBCompetitionSolver app;
                    app.setup(2, argv);
                    app.solve();
                }()),
                ::testing::ExitedWithCode(0), "s UNSUPPORTED");
}

}  // namespace
