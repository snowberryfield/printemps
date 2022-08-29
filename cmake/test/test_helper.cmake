
file(GLOB TEST_SOURCE ${TOP_DIR}/test/helper/*.cpp)

add_executable(
    test_helper
    ${TEST_SOURCE}
)

target_include_directories(
    test_helper
    PUBLIC ${TOP_DIR}/printemps/
    PUBLIC ${TOP_DIR}/external/include/
)

target_link_libraries(
    test_helper
    PUBLIC gtest
    PUBLIC gtest_main
    PUBLIC pthread
)

add_test(
    NAME test_helper
    COMMAND test_helper --gtest_output=xml
)