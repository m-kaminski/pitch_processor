add_executable(io_engine_test io_engine_test.cpp io_engine_ios.cpp io_engine_aio.cpp)
target_compile_options(io_engine_test PRIVATE -g -O0 --coverage -fprofile-arcs -ftest-coverage)
target_link_libraries(io_engine_test GTest::gtest_main rt gcov)
gtest_discover_tests(io_engine_test)

add_executable(io_engine_ios_test io_engine_ios_test.cpp io_engine_ios.cpp)
target_compile_options(io_engine_ios_test PRIVATE -g -O0 --coverage -fprofile-arcs -ftest-coverage)
target_link_libraries(io_engine_ios_test GTest::gtest_main gcov)
gtest_discover_tests(io_engine_ios_test)


add_executable(io_engine_aio_test io_engine_aio_test.cpp io_engine_aio.cpp)
target_compile_options(io_engine_aio_test PRIVATE -g -O0 --coverage -fprofile-arcs -ftest-coverage)
target_link_libraries(io_engine_aio_test GTest::gtest_main rt gcov)
gtest_discover_tests(io_engine_aio_test)

