
### tests
add_executable(worker_thread_test worker_thread.cpp worker_thread_test.cpp)
target_link_libraries(worker_thread_test GTest::gtest_main)
gtest_discover_tests(worker_thread_test pthread)
