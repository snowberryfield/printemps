
file(GLOB TEST_SOURCE ${TOP_DIR}/test/mps/*.cpp)

add_executable(
    test_mps
    ${TEST_SOURCE}
)

target_include_directories(
    test_mps
    PUBLIC ${TOP_DIR}/printemps/
    PUBLIC ${TOP_DIR}/external/include/
)

target_link_libraries(
    test_mps
    PUBLIC gtest
    PUBLIC gtest_main
    PUBLIC pthread
)

add_test(
    NAME test_mps
    COMMAND test_mps --gtest_output=xml
)