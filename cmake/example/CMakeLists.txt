cmake_minimum_required(VERSION 3.10)

project(printemps)

find_package(OpenMP REQUIRED)
if(OpenMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

###############################################################################
# switch compiler options according to CMAKE_BUILD_TYPE
###############################################################################
if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    add_compile_options(
        -std=c++14
        -Wall
        -O3
        -Wextra
        -Wno-error=missing-field-initializers
        -Wno-deprecated-copy
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

###############################################################################
# executable
###############################################################################

#simple_1
add_executable(
    simple_1.exe ${TOP_DIR}/example/simple_1.cpp
)

target_include_directories(
    simple_1.exe
    PRIVATE ${TOP_DIR}/printemps/
)

#simple_2
add_executable(
    simple_2.exe ${TOP_DIR}/example/simple_2.cpp
)

target_include_directories(
    simple_2.exe
    PRIVATE ${TOP_DIR}/printemps/
)

#knapsack
add_executable(
    knapsack.exe ${TOP_DIR}/example/knapsack.cpp
)

target_include_directories(
    knapsack.exe 
    PRIVATE ${TOP_DIR}/printemps/
)

#bin_packing
add_executable(
    bin_packing.exe ${TOP_DIR}/example/bin_packing.cpp
)

target_include_directories(
    bin_packing.exe
    PRIVATE ${TOP_DIR}/printemps/
)

#quadratic_assignment
add_executable(
    quadratic_assignment.exe ${TOP_DIR}/example/quadratic_assignment.cpp
)

target_include_directories(
    quadratic_assignment.exe
    PRIVATE ${TOP_DIR}/printemps/
)

#sudoku
add_executable(
    sudoku.exe ${TOP_DIR}/example/sudoku.cpp
)

target_include_directories(
    sudoku.exe
    PRIVATE ${TOP_DIR}/printemps/
)

###############################################################################
# END
###############################################################################
