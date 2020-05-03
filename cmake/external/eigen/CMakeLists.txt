cmake_minimum_required(VERSION 3.10)
project(eigen)

include(ExternalProject)

set(BUILD_DIR ${CMAKE_BINARY_DIR}/eigen)
set(INSTALL_DIR ${TOP_DIR}/external)
set(INCLUDE_DIR ${INSTALL_DIR}/include)

ExternalProject_Add(
    ${PROJECT_NAME}
    URL https://gitlab.com/libeigen/eigen/-/archive/3.3.7/eigen-3.3.7.tar.gz
    PREFIX "${BUILD_DIR}"
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND
      ${CMAKE_COMMAND} -E copy_directory
        ${BUILD_DIR}/src/eigen/Eigen
        ${INCLUDE_DIR}/Eigen &&
      ${CMAKE_COMMAND} -E copy_directory
        ${BUILD_DIR}/src/eigen/unsupported
        ${INCLUDE_DIR}/unsupported
)