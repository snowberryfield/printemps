
file(GLOB TEST_SOURCE ${TOP_DIR}/test/neighborhood/*.cpp)

add_executable(
    test_neighborhood
    ${TEST_SOURCE}
)

target_include_directories(
    test_neighborhood
    PUBLIC ${TOP_DIR}/printemps/
    PUBLIC ${TOP_DIR}/external/include/
)

target_link_libraries(
    test_neighborhood
    PUBLIC gtest
    PUBLIC gtest_main
    PUBLIC pthread
)

add_test(
    NAME test_neighborhood
    COMMAND test_neighborhood --gtest_output=xml
)