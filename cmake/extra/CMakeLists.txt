# ==============================================================================
# CMakeLists for building extra solvers
# Target: pb_competition_2025_solver
# ==============================================================================
cmake_minimum_required(VERSION 3.15)
project(extra)

# ------------------------------------------------------------------------------
# Include common Printemps configuration
# Sets compiler options, OpenMP, and optional static linking
# ------------------------------------------------------------------------------
include(${TOP_DIR}/cmake/common.cmake)

# ------------------------------------------------------------------------------
# Add extra solver executable
# ------------------------------------------------------------------------------
add_executable(
    pb_competition_2025_solver
    ${TOP_DIR}/extra/pb_competition_2025/main.cpp
)

# ------------------------------------------------------------------------------
# Apply common configuration:
# - Include directories
# - Compiler options (warnings, C++17 standard, optimizations)
# - OpenMP linking
# - Optional static linking on Linux
# ------------------------------------------------------------------------------
configure_printemps_target(pb_competition_2025_solver)

# ==============================================================================
# END
# ==============================================================================
