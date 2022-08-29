
file(GLOB TEST_SOURCE ${TOP_DIR}/test/option/enumerate/*.cpp)

add_executable(
    test_option_enumerate
    ${TEST_SOURCE}
)

target_include_directories(
    test_option_enumerate
    PUBLIC ${TOP_DIR}/printemps/
    PUBLIC ${TOP_DIR}/external/include/
)

target_link_libraries(
    test_option_enumerate
    PUBLIC gtest
    PUBLIC gtest_main
    PUBLIC pthread
)

add_test(
    NAME test_option_enumerate
    COMMAND test_option_enumerate --gtest_output=xml
)