# ==============================================================================
# CMakeLists for building unit tests
# ==============================================================================
cmake_minimum_required(VERSION 3.31)
project(test)

# ------------------------------------------------------------------------------
# Include common Printemps configuration
# - Sets compiler options, OpenMP, C++17 standard
# - Provides the helper function configure_printemps_target()
# ------------------------------------------------------------------------------
include(${TOP_DIR}/cmake/common.cmake)

# ------------------------------------------------------------------------------
# Enable CTest for running tests
# ------------------------------------------------------------------------------
enable_testing()

# ------------------------------------------------------------------------------
# Directory to store test result files in XML format
# ------------------------------------------------------------------------------
set(RESULT_DIR ${CMAKE_BINARY_DIR}/result)
file(MAKE_DIRECTORY ${RESULT_DIR})

# ------------------------------------------------------------------------------
# Use ExternalProject to fetch and build GoogleTest automatically
# ------------------------------------------------------------------------------
include(ExternalProject)

set(GTEST_INSTALL_DIR ${CMAKE_BINARY_DIR}/gtest)

ExternalProject_Add(
    googletest
    URL https://github.com/google/googletest/releases/download/v1.17.0/googletest-1.17.0.tar.gz
    PREFIX ${GTEST_INSTALL_DIR}
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${GTEST_INSTALL_DIR}
        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
        -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
)

# Retrieve installation directory for linking and includes
ExternalProject_Get_Property(googletest install_dir)
set(GTEST_INCLUDE_DIR ${install_dir}/include)
set(GTEST_LIBRARY ${install_dir}/lib/libgtest.a)
set(GTEST_MAIN_LIBRARY ${install_dir}/lib/libgtest_main.a)

# ------------------------------------------------------------------------------
# Automatically detect test groups in test/ and create CMake targets
# ------------------------------------------------------------------------------
# Copy test data to build directory
file(COPY ${TOP_DIR}/test/dat DESTINATION ${CMAKE_BINARY_DIR})

# Get subdirectories of test/ (each represents a test group)
file(GLOB TEST_GROUP_DIRS RELATIVE ${TOP_DIR}/test ${TOP_DIR}/test/*)

foreach(group ${TEST_GROUP_DIRS})
    # Skip entries that are not directories
    if(NOT IS_DIRECTORY ${TOP_DIR}/test/${group})
        continue()
    endif()

    # Collect all C++ source files for this test group
    file(GLOB TEST_SOURCES CONFIGURE_DEPENDS ${TOP_DIR}/test/${group}/*.cpp)
    if(NOT TEST_SOURCES)
        continue()
    endif()

    # Define a unique test target for this group
    set(test_target test_${group})
    add_executable(${test_target} ${TEST_SOURCES})

    # Apply common Printemps configuration (include dirs, compile options, OpenMP, static linking)
    configure_printemps_target(${test_target})

    # Include GoogleTest headers
    target_include_directories(${test_target}
        PUBLIC ${GTEST_INCLUDE_DIR}
    )

    # Link GoogleTest libraries and pthread
    target_link_libraries(${test_target}
        PRIVATE ${GTEST_LIBRARY} ${GTEST_MAIN_LIBRARY} pthread
    )

    # Ensure GoogleTest is built before running this test target
    add_dependencies(${test_target} googletest)

    # Register test with CTest, outputting XML results for reporting
    add_test(
        NAME ${test_target}
        COMMAND ${test_target} --gtest_output=xml:${RESULT_DIR}/${test_target}.xml
    )
endforeach()
