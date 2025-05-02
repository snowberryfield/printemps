cmake_minimum_required(VERSION 3.10)

project(printemps)

find_package(OpenMP REQUIRED)

if(OpenMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

option(LINK_STATIC OFF)

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

# simple_1
add_executable(
    simple_1 ${TOP_DIR}/example/simple_1.cpp
)

target_include_directories(
    simple_1
    PRIVATE ${TOP_DIR}/printemps/
)

# simple_2
add_executable(
    simple_2 ${TOP_DIR}/example/simple_2.cpp
)

target_include_directories(
    simple_2
    PRIVATE ${TOP_DIR}/printemps/
)

# knapsack
add_executable(
    knapsack ${TOP_DIR}/example/knapsack.cpp
)

target_include_directories(
    knapsack
    PRIVATE ${TOP_DIR}/printemps/
)

# bin_packing
add_executable(
    bin_packing ${TOP_DIR}/example/bin_packing.cpp
)

target_include_directories(
    bin_packing
    PRIVATE ${TOP_DIR}/printemps/
)

# sudoku
add_executable(
    sudoku ${TOP_DIR}/example/sudoku.cpp
)

target_include_directories(
    sudoku
    PRIVATE ${TOP_DIR}/printemps/
)

if(LINK_STATIC)
    target_link_options(simple_1 PRIVATE -static-libgcc -static-libstdc++ -static)
    target_link_options(simple_2 PRIVATE -static-libgcc -static-libstdc++ -static)
    target_link_options(knapsack PRIVATE -static-libgcc -static-libstdc++ -static)
    target_link_options(bin_packing PRIVATE -static-libgcc -static-libstdc++ -static)
    target_link_options(sudoku PRIVATE -static-libgcc -static-libstdc++ -static)
endif()

# ##############################################################################
# END
# ##############################################################################
