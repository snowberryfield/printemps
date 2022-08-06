
file(GLOB TEST_MODEL_SOURCE ${TOP_DIR}/test/option/*.cpp)

add_executable(
    test_option
    ${TEST_MODEL_SOURCE}
)

target_include_directories(
    test_option
    PUBLIC ${TOP_DIR}/printemps/
    PUBLIC ${TOP_DIR}/external/include/
)

target_link_libraries(
    test_option
    PUBLIC gtest
    PUBLIC gtest_main
    PUBLIC pthread
)

add_test(
    NAME test_option
    COMMAND test_option --gtest_output=xml
)