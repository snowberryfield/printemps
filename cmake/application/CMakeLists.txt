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
    )

    if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64|aarch64")
        # for Apple Silicon
        add_compile_options(-mcpu=native)
    else()
        # for x86_64
        add_compile_options(-march=native -mtune=native)
    endif()
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
add_executable(printemps ${TOP_DIR}/application/standalone/main.cpp)
add_executable(mps_solver ${TOP_DIR}/application/standalone/main.cpp)
add_executable(opb_solver ${TOP_DIR}/application/standalone/main.cpp)

target_include_directories(
    printemps
    PRIVATE ${TOP_DIR}/printemps/
    PRIVATE ${TOP_DIR}/external/include/
)

target_include_directories(
    mps_solver
    PRIVATE ${TOP_DIR}/printemps/
    PRIVATE ${TOP_DIR}/external/include/
)

target_include_directories(
    opb_solver
    PRIVATE ${TOP_DIR}/printemps/
    PRIVATE ${TOP_DIR}/external/include/
)

if(LINK_STATIC)
    target_link_options(printemps PRIVATE -static-libgcc -static-libstdc++ -static)
    target_link_options(mps_solver PRIVATE -static-libgcc -static-libstdc++ -static)
    target_link_options(opb_solver PRIVATE -static-libgcc -static-libstdc++ -static)
endif()

# ##############################################################################
# END
# ##############################################################################
