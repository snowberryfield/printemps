
file(GLOB TEST_SOURCE ${TOP_DIR}/test/opb/*.cpp)

add_executable(
    test_opb
    ${TEST_SOURCE}
)

target_include_directories(
    test_opb
    PUBLIC ${TOP_DIR}/printemps/
    PUBLIC ${TOP_DIR}/external/include/
)

target_link_libraries(
    test_opb
    PUBLIC gtest
    PUBLIC gtest_main
    PUBLIC pthread
)

add_test(
    NAME test_opb
    COMMAND test_opb --gtest_output=xml
)