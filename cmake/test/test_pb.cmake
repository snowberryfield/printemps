
file(GLOB TEST_SOURCE ${TOP_DIR}/test/pb/*.cpp)

add_executable(
    test_pb
    ${TEST_SOURCE}
)

target_include_directories(
    test_pb
    PUBLIC ${TOP_DIR}/printemps/
    PUBLIC ${TOP_DIR}/external/include/
)

target_link_libraries(
    test_pb
    PUBLIC gtest
    PUBLIC gtest_main
    PUBLIC pthread
)

add_test(
    NAME test_pb
    COMMAND test_pb --gtest_output=xml
)