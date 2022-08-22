
file(GLOB TEST_SOURCE ${TOP_DIR}/test/example/*.cpp)

add_executable(
    test_example
    ${TEST_SOURCE}
)

target_include_directories(
    test_example
    PUBLIC ${TOP_DIR}/printemps/
    PUBLIC ${TOP_DIR}/external/include/
)

target_link_libraries(
    test_example
    PUBLIC gtest
    PUBLIC gtest_main
    PUBLIC pthread
)

add_test(
    NAME test_example
    COMMAND test_example --gtest_output=xml
)