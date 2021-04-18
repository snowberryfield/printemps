
file(GLOB TEST_MODEL_SOURCE ${TOP_DIR}/test/verifier/*.cpp)

add_executable(
    test_verifier
    ${TEST_MODEL_SOURCE}
)

target_include_directories(
    test_verifier
    PUBLIC ${TOP_DIR}/printemps/
    PUBLIC ${TOP_DIR}/external/include/
)

target_link_libraries(
    test_verifier
    PUBLIC gtest
    PUBLIC gtest_main
    PUBLIC pthread
)

add_test(
    NAME test_verifier
    COMMAND test_verifier --gtest_output=xml
)