# ==============================================================================
# CMakeLists for building the standalone Printemps applications
# Targets: printemps
# ==============================================================================
cmake_minimum_required(VERSION 3.15)
project(application)

# ------------------------------------------------------------------------------
# Include common Printemps configuration
# Sets compiler options, OpenMP, and optional static linking
# ------------------------------------------------------------------------------
if(NOT DEFINED TOP_DIR)
    get_filename_component(TOP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)
endif()
include(${TOP_DIR}/cmake/common.cmake)

# ------------------------------------------------------------------------------
# Source files for the standalone executables
# ------------------------------------------------------------------------------
set(MAIN_SRC ${TOP_DIR}/application/standalone/main.cpp)

# ------------------------------------------------------------------------------
# Create executables
# For each target, add the executable and configure it using the common helper
# ------------------------------------------------------------------------------
add_executable(printemps ${MAIN_SRC})

# Apply common Printemps configuration
configure_printemps_target(printemps)
