# ==============================================================================
# CMakeLists for building extra solvers
# Targets: pb_competition_solver, maxsat_evaluation_solver
# ==============================================================================
cmake_minimum_required(VERSION 3.15)
project(extra)

# ------------------------------------------------------------------------------
# Include common Printemps configuration
# ------------------------------------------------------------------------------
include(${TOP_DIR}/cmake/common.cmake)

# ------------------------------------------------------------------------------
# Add extra solver executables
# ------------------------------------------------------------------------------

# pb_solver for Pseudo-Boolean Competition 2025
add_executable(
    pb_competition_solver
    ${TOP_DIR}/extra/pb_competition/main.cpp
)
configure_printemps_target(pb_competition_solver)

# maxsat_evaluation_solver for the MaxSAT Evaluation anytime tracks
add_executable(
    maxsat_evaluation_solver
    ${TOP_DIR}/extra/maxsat_evaluation/main.cpp
)
configure_printemps_target(maxsat_evaluation_solver)
