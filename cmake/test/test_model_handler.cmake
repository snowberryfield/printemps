
file(GLOB TEST_SOURCE ${TOP_DIR}/test/model_handler/*.cpp)

add_executable(
    test_model_handler
    ${TEST_SOURCE}
)

target_include_directories(
    test_model_handler
    PUBLIC ${TOP_DIR}/printemps/
    PUBLIC ${TOP_DIR}/external/include/
)

target_link_libraries(
    test_model_handler
    PUBLIC gtest
    PUBLIC gtest_main
    PUBLIC pthread
)

add_test(
    NAME test_model_handler
    COMMAND test_model_handler --gtest_output=xml
)