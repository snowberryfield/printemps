
file(GLOB TEST_MODEL_SOURCE ${TOP_DIR}/test/preprocess/*.cpp)

add_executable(
    test_preprocess
    ${TEST_MODEL_SOURCE}
)

target_include_directories(
    test_preprocess
    PUBLIC ${TOP_DIR}/printemps/
    PUBLIC ${TOP_DIR}/external/include/
)

target_link_libraries(
    test_preprocess
    PUBLIC gtest
    PUBLIC gtest_main
    PUBLIC pthread
)

add_test(
    NAME test_presolver
    COMMAND test_preprocess --gtest_output=xml
)