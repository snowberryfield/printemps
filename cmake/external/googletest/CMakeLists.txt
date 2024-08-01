cmake_minimum_required(VERSION 3.10)
project(googletest)

include(ExternalProject)

set(BUILD_DIR ${CMAKE_BINARY_DIR}/googletest)
set(INSTALL_DIR ${TOP_DIR}/external)
set(INCLUDE_DIR ${INSTALL_DIR}/include)
set(LIB_DIR ${INSTALL_DIR}/lib)

find_package(Threads REQUIRED)

ExternalProject_Add(
    ${PROJECT_NAME}
    URL https://github.com/google/googletest/releases/download/v1.15.2/googletest-1.15.2.tar.gz
    PREFIX ${BUILD_DIR}
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
    CMAKE_ARGS -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
    CMAKE_ARGS -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
)

