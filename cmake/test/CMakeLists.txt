cmake_minimum_required(VERSION 3.10)
project(test)

# ##############################################################################
# switch compiler options according to CMAKE_BUILD_TYPE
# ##############################################################################
message(${CMAKE_BUILD_TYPE})

if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    add_compile_options(
        -std=c++17
        -Wall
        -O3
        -Wextra
        -Wno-error=missing-field-initializers
        -pthread
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
# # tests
# ##############################################################################
enable_testing()

link_directories(
    ${TOP_DIR}/external/lib
)

include(test_mps.cmake)
include(test_example.cmake)
include(test_helper.cmake)
include(test_model.cmake)
include(test_model_component.cmake)
include(test_multi_array.cmake)
include(test_neighborhood.cmake)
include(test_option.cmake)
include(test_option_enumerate.cmake)
include(test_opb.cmake)
include(test_preprocess.cmake)
include(test_solution.cmake)
include(test_standalone.cmake)
include(test_utility.cmake)
include(test_utility_graph.cmake)
include(test_utility_json.cmake)
include(test_utility_ucb1.cmake)
include(test_utility_sparse.cmake)

# ##############################################################################
# # END
# ##############################################################################