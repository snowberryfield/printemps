
file(GLOB TEST_MODEL_SOURCE ${TOP_DIR}/test/solver/*.cpp)

add_executable(
    test_solver
    ${TEST_MODEL_SOURCE}
)

target_include_directories(
    test_solver
    PUBLIC ${TOP_DIR}/cppmh/
    PUBLIC ${TOP_DIR}/external/include/
)

target_link_libraries(
    test_solver
    PUBLIC gtest
    PUBLIC gtest_main
    PUBLIC pthread
)

add_test(
    NAME test_solver
    COMMAND test_solver --gtest_output=xml
)