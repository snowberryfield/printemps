cmake_minimum_required(VERSION 3.10)
project(nlohmann_json)

include(ExternalProject)

set(BUILD_DIR ${CMAKE_BINARY_DIR}/nlohmann_json)
set(INSTALL_DIR ${TOP_DIR}/external)
set(INCLUDE_DIR ${INSTALL_DIR}/include)

ExternalProject_Add(
    ${PROJECT_NAME}
    URL https://github.com/nlohmann/json/releases/download/v3.7.3/include.zip
    PREFIX "${BUILD_DIR}"
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND
      ${CMAKE_COMMAND} -E copy_directory
        ${BUILD_DIR}/src/nlohmann_json/single_include/
        ${INCLUDE_DIR}
)