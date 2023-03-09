
### tests
add_executable(worker_thread_test worker_thread.cpp worker_thread_test.cpp)
target_compile_options(worker_thread_test PRIVATE -g -O0 --coverage -fprofile-arcs -ftest-coverage)
target_link_libraries(worker_thread_test GTest::gtest_main gcov)
gtest_discover_tests(worker_thread_test pthread)


add_executable(execution_policy_single_threaded_test 
        execution_policy_single_threaded.cpp 
        execution_policy_single_threaded_test.cpp
        ../io_engine/io_engine_ios.cpp
        ../pitch/event_accumulator.cpp
        ../pitch/summary_formatter.cpp)
target_compile_options(execution_policy_single_threaded_test PRIVATE -g -O0 --coverage -fprofile-arcs -ftest-coverage)
target_link_libraries(execution_policy_single_threaded_test GTest::gtest_main gcov)
gtest_discover_tests(execution_policy_single_threaded_test pthread)



add_executable(execution_policy_multi_threaded_test 
        execution_policy_multi_threaded.cpp 
        execution_policy_multi_threaded_test.cpp
        worker_thread.cpp
        ../io_engine/io_engine_ios.cpp
        ../pitch/event_accumulator.cpp
        ../pitch/summary_formatter.cpp)
target_compile_options(execution_policy_single_threaded_test PRIVATE -g -O0 --coverage -fprofile-arcs -ftest-coverage)
target_link_libraries(execution_policy_multi_threaded_test GTest::gtest_main pthread gcov)
gtest_discover_tests(execution_policy_multi_threaded_test pthread)



add_executable(error_counter_test error_counter_test.cpp)
target_compile_options(error_counter_test PRIVATE -g -O0 --coverage -fprofile-arcs -ftest-coverage)
target_link_libraries(error_counter_test GTest::gtest_main gcov)
gtest_discover_tests(error_counter_test)