add_executable(io_engine_test_test io_engine_test.cpp io_engine_ios.cpp io_engine_aio.cpp)
target_link_libraries(io_engine_test_test GTest::gtest_main rt)
gtest_discover_tests(io_engine_test_test)

