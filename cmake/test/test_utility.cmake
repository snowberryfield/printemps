
file(GLOB TEST_MODEL_SOURCE ${TOP_DIR}/test/utility/*.cpp)

add_executable(
    test_utility
    ${TEST_MODEL_SOURCE}
)

target_include_directories(
    test_utility
    PUBLIC ${TOP_DIR}/printemps/
    PUBLIC ${TOP_DIR}/external/include/
)

target_link_libraries(
    test_utility
    PUBLIC gtest
    PUBLIC gtest_main
    PUBLIC pthread
)

add_test(
    NAME test_utility
    COMMAND test_utility --gtest_output=xml
)