
file(GLOB TEST_SOURCE ${TOP_DIR}/test/standalone/*.cpp)

add_executable(
    test_standalone
    ${TEST_SOURCE}
)

target_include_directories(
    test_standalone
    PUBLIC ${TOP_DIR}/printemps/
    PUBLIC ${TOP_DIR}/external/include/
)

target_link_libraries(
    test_standalone
    PUBLIC gtest
    PUBLIC gtest_main
    PUBLIC pthread
)

add_test(
    NAME test_standalone
    COMMAND test_standalone --gtest_output=xml
)