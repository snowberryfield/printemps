# ##############################################################################
# Common configuration for Printemps projects
# This file sets up compiler options, OpenMP, static linking, and helper functions
# ##############################################################################
cmake_minimum_required(VERSION 3.15)
project(printemps LANGUAGES C CXX)

# ##############################################################################
# C++ language standard
# ##############################################################################
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ##############################################################################
# OpenMP support
# ##############################################################################
find_package(OpenMP)

if(OpenMP_FOUND)
    message(STATUS "OpenMP found, enabling flags")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
else()
    message(STATUS "OpenMP not found, building without OpenMP")
endif()


# ##############################################################################
# CPU architecture setting (Release builds only)
# ##############################################################################

# CPU_ARCH controls architecture-specific compiler flags in Release builds.
#   "native"        (default) — -mcpu=native (arm64/aarch64) or -march=native -mtune=native (x86_64)
#   "none"          — no architecture flags
#   any other value — passed directly as -mcpu=<value> (arm64/aarch64) or -march=<value> -mtune=generic (x86_64)
set(CPU_ARCH "native" CACHE STRING
    "Target CPU architecture: 'native' (default), 'none', or a value such as x86-64-v2 / armv8-a")

# ##############################################################################
# switch compiler options according to CMAKE_BUILD_TYPE
# ##############################################################################
if(MSVC)
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        set(COMMON_COMPILE_OPTIONS /W3 /O2 /permissive- /utf-8 /bigobj /EHsc /D_USE_MATH_DEFINES /DNOMINMAX /D_CRT_SECURE_NO_WARNINGS /wd4244 /wd4267 /wd4305 /wd4996 /wd4056 /wd4756 /wd4129 /wd5051)
    else() # Debug
        set(COMMON_COMPILE_OPTIONS /W3 /Od /Zi /permissive- /utf-8 /bigobj /EHsc /D_USE_MATH_DEFINES /DNOMINMAX /D_CRT_SECURE_NO_WARNINGS /wd4244 /wd4267 /wd4305 /wd4996 /wd4056 /wd4756 /wd4129 /wd5051)
    endif()
else()
    set(COMMON_COMPILE_OPTIONS
        -Wall
        -Wextra
        -Wno-error=missing-field-initializers
        -pthread
    )

    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        # Optimize for speed
        list(APPEND COMMON_COMPILE_OPTIONS -O3)

        # CPU-specific tuning
        if(CPU_ARCH STREQUAL "none")
            # no architecture flags
        elseif(CPU_ARCH STREQUAL "native")
            if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64|aarch64")
                list(APPEND COMMON_COMPILE_OPTIONS -mcpu=native)
            else()
                list(APPEND COMMON_COMPILE_OPTIONS -march=native -mtune=native)
            endif()
        else()
            if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64|aarch64")
                list(APPEND COMMON_COMPILE_OPTIONS -mcpu=${CPU_ARCH})
            else()
                list(APPEND COMMON_COMPILE_OPTIONS -march=${CPU_ARCH} -mtune=generic)
            endif()
        endif()
    endif()
endif()

# ##############################################################################
# executable (Helper function to configure a target for Printemps)
# Sets include directories, compiler options, OpenMP linking, and static linking
# ##############################################################################
function(configure_printemps_target target)
    # Include Printemps headers
    target_include_directories(${target}
        PRIVATE ${TOP_DIR}/printemps
        PRIVATE ${TOP_DIR}/external/include
    )

    # Apply common compiler options
    target_compile_options(${target} PRIVATE ${COMMON_COMPILE_OPTIONS})

    if(OpenMP_FOUND)
        target_link_libraries(${target} PRIVATE OpenMP::OpenMP_CXX)
    endif()

    # Optionally enable full static linking
    if(LINK_STATIC)
        if(MSVC)
            set_property(TARGET ${target} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
        elseif(UNIX AND NOT APPLE)
            target_link_options(${target}
                PRIVATE -static -static-libgcc -static-libstdc++
            )
        endif()
    endif()
endfunction()