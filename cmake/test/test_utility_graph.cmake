
file(GLOB TEST_SOURCE ${TOP_DIR}/test/utility/graph/*.cpp)

add_executable(
    test_utility_graph
    ${TEST_SOURCE}
)

target_include_directories(
    test_utility_graph
    PUBLIC ${TOP_DIR}/printemps/
    PUBLIC ${TOP_DIR}/external/include/
)

target_link_libraries(
    test_utility_graph
    PUBLIC gtest
    PUBLIC gtest_main
    PUBLIC pthread
)

add_test(
    NAME test_utility_graph
    COMMAND test_utility_graph --gtest_output=xml
)