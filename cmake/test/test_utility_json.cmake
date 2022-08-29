
file(GLOB TEST_SOURCE ${TOP_DIR}/test/utility/json/*.cpp)

add_executable(
    test_utility_json
    ${TEST_SOURCE}
)

target_include_directories(
    test_utility_json
    PUBLIC ${TOP_DIR}/printemps/
    PUBLIC ${TOP_DIR}/external/include/
)

target_link_libraries(
    test_utility_json
    PUBLIC gtest
    PUBLIC gtest_main
    PUBLIC pthread
)

add_test(
    NAME test_utility_json
    COMMAND test_utility_json --gtest_output=xml
)