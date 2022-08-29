
file(GLOB TEST_SOURCE ${TOP_DIR}/test/multi_array/*.cpp)

add_executable(
    test_multi_array
    ${TEST_SOURCE}
)

target_include_directories(
    test_multi_array
    PUBLIC ${TOP_DIR}/printemps/
    PUBLIC ${TOP_DIR}/external/include/
)

target_link_libraries(
    test_multi_array
    PUBLIC gtest
    PUBLIC gtest_main
    PUBLIC pthread
)

add_test(
    NAME test_multi_array
    COMMAND test_multi_array --gtest_output=xml
)