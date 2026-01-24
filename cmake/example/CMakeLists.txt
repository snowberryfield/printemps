# ==============================================================================
# CMakeLists for building example programs of Printemps
# ==============================================================================
cmake_minimum_required(VERSION 3.15)
project(example)

# ------------------------------------------------------------------------------
# Include common Printemps configuration (compile options, OpenMP, C++17, etc.)
# ------------------------------------------------------------------------------
include(${TOP_DIR}/cmake/common.cmake)

# ------------------------------------------------------------------------------
# Automatically detect all example source files
# - EXAMPLE_SOURCES: full path to each .cpp file in example/
# - EXAMPLES: base filenames without extension
# ------------------------------------------------------------------------------
file(GLOB EXAMPLE_SOURCES CONFIGURE_DEPENDS ${TOP_DIR}/example/*.cpp)

set(EXAMPLES "")
foreach(src_file ${EXAMPLE_SOURCES})
    get_filename_component(name ${src_file} NAME_WE)
    list(APPEND EXAMPLES ${name})
endforeach()

# ------------------------------------------------------------------------------
# Add executables for each example
# ------------------------------------------------------------------------------
foreach(name ${EXAMPLES})
    add_executable(${name} ${TOP_DIR}/example/${name}.cpp)
    configure_printemps_target(${name})
endforeach()
