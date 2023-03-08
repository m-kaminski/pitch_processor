add_executable(io_engine_test io_engine_test.cpp io_engine_ios.cpp io_engine_aio.cpp)
target_link_libraries(io_engine_test GTest::gtest_main rt)
gtest_discover_tests(io_engine_test)

add_executable(io_engine_ios_test io_engine_ios_test.cpp io_engine_ios.cpp)
target_link_libraries(io_engine_ios_test GTest::gtest_main)
gtest_discover_tests(io_engine_ios_test)

