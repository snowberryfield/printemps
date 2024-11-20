cmake_minimum_required(VERSION 3.10)

project(printemps)

find_package(OpenMP REQUIRED)

if(OpenMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

# ##############################################################################
# switch compiler options according to CMAKE_BUILD_TYPE
# ##############################################################################
if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    add_compile_options(
        -std=c++17
        -Wall
        -O3
        -Wextra
        -Wno-error=missing-field-initializers
        -pthread
        -march=native
        -mtune=native
    )
else() # Debug
    add_compile_options(
        -std=c++17
        -Wall
        -Wextra
        -Wno-error=missing-field-initializers
        -pthread
    )
endif()

# ##############################################################################
# executable
# ##############################################################################

# mps_solver
add_executable(
    mps_solver ${TOP_DIR}/application/mps_solver/main.cpp
)

target_include_directories(
    mps_solver
    PRIVATE ${TOP_DIR}/printemps/
    PRIVATE ${TOP_DIR}/external/include/
)

# ##############################################################################
# END
# ##############################################################################
