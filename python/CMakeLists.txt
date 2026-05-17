cmake_minimum_required(VERSION 3.15)
project(printemps_python LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Locate the umbrella printemps header. When building from an sdist tarball
# the headers are vendored into python/_vendor/printemps/ at release time
# (see tools/vendor.py); for development builds out of the source repo we
# fall back to the sibling printemps/ directory.
if(EXISTS "${PROJECT_SOURCE_DIR}/_vendor/printemps/printemps.h")
    set(PRINTEMPS_HEADER_DIR_DEFAULT "${PROJECT_SOURCE_DIR}/_vendor/printemps")
else()
    set(PRINTEMPS_HEADER_DIR_DEFAULT "${PROJECT_SOURCE_DIR}/../printemps")
endif()

set(PRINTEMPS_HEADER_DIR "${PRINTEMPS_HEADER_DIR_DEFAULT}"
    CACHE PATH "Path to the printemps header-only include directory")

if(NOT EXISTS "${PRINTEMPS_HEADER_DIR}/printemps.h")
    message(FATAL_ERROR
        "Could not find printemps.h at ${PRINTEMPS_HEADER_DIR}. "
        "Set -DPRINTEMPS_HEADER_DIR=<path to printemps/> when configuring.")
endif()

# OpenMP is optional. printemps uses it for parallel solver iterations but
# builds fine without it.
find_package(OpenMP)

find_package(Python COMPONENTS Interpreter Development.Module REQUIRED)
find_package(pybind11 CONFIG REQUIRED)

pybind11_add_module(_printemps MODULE src/_printemps.cpp)

target_include_directories(_printemps PRIVATE "${PRINTEMPS_HEADER_DIR}")

if(MSVC)
    target_compile_options(_printemps PRIVATE
        /O2
        /W3
        /bigobj
        /wd4244
        /wd4267
        /wd4996
    )
else()
    target_compile_options(_printemps PRIVATE
        -O3
        -Wall
        -Wextra
        -Wno-error=missing-field-initializers
        -Wno-unused-parameter
        -Wno-unused-variable
        -Wno-deprecated-declarations
    )
endif()

if(OpenMP_CXX_FOUND)
    target_link_libraries(_printemps PRIVATE OpenMP::OpenMP_CXX)
endif()

# scikit-build-core puts the extension into the package directory.
install(TARGETS _printemps DESTINATION printemps)
