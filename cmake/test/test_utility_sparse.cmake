
file(GLOB TEST_SOURCE ${TOP_DIR}/test/utility/sparse/*.cpp)

add_executable(
    test_utility_sparse
    ${TEST_SOURCE}
)

target_include_directories(
    test_utility_sparse
    PUBLIC ${TOP_DIR}/printemps/
    PUBLIC ${TOP_DIR}/external/include/
)

target_link_libraries(
    test_utility_sparse
    PUBLIC gtest
    PUBLIC gtest_main
    PUBLIC pthread
)

add_test(
    NAME test_utility_sparse
    COMMAND test_utility_sparse--gtest_output=xml
)