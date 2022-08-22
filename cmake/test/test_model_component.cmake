
file(GLOB TEST_SOURCE ${TOP_DIR}/test/model_component/*.cpp)

add_executable(
    test_model_component
    ${TEST_SOURCE}
)

target_include_directories(
    test_model_component
    PUBLIC ${TOP_DIR}/printemps/
    PUBLIC ${TOP_DIR}/external/include/
)

target_link_libraries(
    test_model_component
    PUBLIC gtest
    PUBLIC gtest_main
    PUBLIC pthread
)

add_test(
    NAME test_model_component
    COMMAND test_model_component --gtest_output=xml
)