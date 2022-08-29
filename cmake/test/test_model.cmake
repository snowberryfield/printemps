
file(GLOB TEST_SOURCE ${TOP_DIR}/test/model/*.cpp)

add_executable(
    test_model
    ${TEST_SOURCE}
)

target_include_directories(
    test_model
    PUBLIC ${TOP_DIR}/printemps/
    PUBLIC ${TOP_DIR}/external/include/
)

target_link_libraries(
    test_model
    PUBLIC gtest
    PUBLIC gtest_main
    PUBLIC pthread
)

add_test(
    NAME test_model
    COMMAND test_model --gtest_output=xml
)