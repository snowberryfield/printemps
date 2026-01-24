# ==============================================================================
# CMakeLists for building the standalone Printemps applications
# Targets: printemps, mps_solver, opb_solver
# ==============================================================================
cmake_minimum_required(VERSION 3.15)
project(application)

# ------------------------------------------------------------------------------
# Include common Printemps configuration
# Sets compiler options, OpenMP, and optional static linking
# ------------------------------------------------------------------------------
include(${TOP_DIR}/cmake/common.cmake)

# ------------------------------------------------------------------------------
# Source files for the standalone executables
# ------------------------------------------------------------------------------
set(MAIN_SRC ${TOP_DIR}/application/standalone/main.cpp)

# ------------------------------------------------------------------------------
# Create executables
# For each target, add the executable and configure it using the common helper
# ------------------------------------------------------------------------------
foreach(target printemps mps_solver opb_solver)
    add_executable(${target} ${MAIN_SRC})

    # Apply common Printemps configuration
    # - Include directories
    # - Compiler options (warnings, C++17 standard, optimizations)
    # - OpenMP linking
    # - Optional static linking on Linux
    configure_printemps_target(${target})
endforeach()
