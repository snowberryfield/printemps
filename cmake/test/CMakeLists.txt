# ==============================================================================
# CMakeLists for building unit tests
# ==============================================================================
cmake_minimum_required(VERSION 3.15)
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
# Directory for GoogleTest XML results
# ------------------------------------------------------------------------------
set(RESULT_DIR ${CMAKE_BINARY_DIR}/result)
file(MAKE_DIRECTORY ${RESULT_DIR})

# ------------------------------------------------------------------------------
# Fetch GoogleTest
# ------------------------------------------------------------------------------
include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

# ------------------------------------------------------------------------------
# Copy test data (tests expect relative paths)
# ------------------------------------------------------------------------------
file(COPY ${TOP_DIR}/test/dat DESTINATION ${CMAKE_BINARY_DIR})

# ------------------------------------------------------------------------------
# Automatically detect test groups and create targets
# ------------------------------------------------------------------------------
file(GLOB TEST_GROUP_DIRS RELATIVE ${TOP_DIR}/test ${TOP_DIR}/test/*)

foreach(group ${TEST_GROUP_DIRS})
    if(NOT IS_DIRECTORY ${TOP_DIR}/test/${group})
        continue()
    endif()

    file(GLOB TEST_SOURCES CONFIGURE_DEPENDS
        ${TOP_DIR}/test/${group}/*.cpp
    )
    if(NOT TEST_SOURCES)
        continue()
    endif()

    set(test_target test_${group})
    add_executable(${test_target} ${TEST_SOURCES})

    # Apply common Printemps configuration (include dirs, compile options, OpenMP, static linking)
    configure_printemps_target(${test_target})

    # Link GoogleTest libraries
    target_link_libraries(${test_target}
        PRIVATE
            GTest::gtest_main
            pthread
    )

    # Register test with CTest
    add_test(
        NAME ${test_target}
        COMMAND ${test_target}
            --gtest_output=xml:${RESULT_DIR}/${test_target}.xml
    )
endforeach()
