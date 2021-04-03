
file(GLOB TEST_MODEL_SOURCE ${TOP_DIR}/test/presolver/*.cpp)

add_executable(
    test_presolver
    ${TEST_MODEL_SOURCE}
)

target_include_directories(
    test_presolver
    PUBLIC ${TOP_DIR}/printemps/
    PUBLIC ${TOP_DIR}/external/include/
)

target_link_libraries(
    test_presolver
    PUBLIC gtest
    PUBLIC gtest_main
    PUBLIC pthread
)

add_test(
    NAME test_presolver
    COMMAND test_presolver --gtest_output=xml
)