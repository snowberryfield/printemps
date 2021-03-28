
file(GLOB TEST_MODEL_SOURCE ${TOP_DIR}/test/solution/*.cpp)

add_executable(
    test_solution
    ${TEST_MODEL_SOURCE}
)

target_include_directories(
    test_solution
    PUBLIC ${TOP_DIR}/printemps/
    PUBLIC ${TOP_DIR}/external/include/
)

target_link_libraries(
    test_solution
    PUBLIC gtest
    PUBLIC gtest_main
    PUBLIC pthread
)

add_test(
    NAME test_solution
    COMMAND test_solution --gtest_output=xml
)