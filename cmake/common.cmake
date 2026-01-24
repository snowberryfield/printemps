# ==============================================================================
# Common configuration for Printemps projects
# This file sets up compiler options, OpenMP, static linking, and helper functions
# ==============================================================================
cmake_minimum_required(VERSION 3.15)
project(printemps LANGUAGES C CXX)

# ------------------------------------------------------------------------------
# C++ language standard
# ------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ------------------------------------------------------------------------------
# OpenMP support
# ------------------------------------------------------------------------------
find_package(OpenMP)

if(OpenMP_FOUND)
    message(STATUS "OpenMP found, enabling flags")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
else()
    message(STATUS "OpenMP not found, building without OpenMP")
endif()


# ------------------------------------------------------------------------------
# Common compiler options
# -Wall, -Wextra for warnings
# -Wno-error=missing-field-initializers to allow aggregate initialization without error
# Release builds: optimization flags and CPU-specific tuning
# ------------------------------------------------------------------------------
set(COMMON_COMPILE_OPTIONS
    -Wall
    -Wextra
    -Wno-error=missing-field-initializers
)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    # Optimize for speed
    list(APPEND COMMON_COMPILE_OPTIONS -O3)

    # CPU-specific tuning
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64|aarch64")
        list(APPEND COMMON_COMPILE_OPTIONS -mcpu=native)
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64")
        list(APPEND COMMON_COMPILE_OPTIONS -march=native -mtune=native)
    endif()
endif()

# ------------------------------------------------------------------------------
# Helper function to configure a target for Printemps
# Sets include directories, compiler options, OpenMP linking, and optional static linking
# ------------------------------------------------------------------------------
function(configure_printemps_target target)
    # Include Printemps headers
    target_include_directories(${target}
        PRIVATE ${TOP_DIR}/printemps
    )

    # Apply common compiler options
    target_compile_options(${target} PRIVATE ${COMMON_COMPILE_OPTIONS})

    if(OpenMP_FOUND)
        target_link_libraries(${target} PRIVATE OpenMP::OpenMP_CXX)
    endif()

    # On Linux, optionally enable full static linking
    if(LINK_STATIC AND UNIX AND NOT APPLE)
        target_link_options(${target}
            PRIVATE -static -static-libgcc -static-libstdc++
        )
    endif()
endfunction()