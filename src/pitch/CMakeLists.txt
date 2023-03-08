


add_executable(base36_test base36_test.cpp)
target_compile_options(base36_test PRIVATE -g -O0 --coverage -fprofile-arcs -ftest-coverage)
target_link_libraries(base36_test GTest::gtest_main gcov)
gtest_discover_tests(base36_test)

add_executable(pitch_decoder_test pitch_decoder_test.cpp)
target_compile_options(pitch_decoder_test PRIVATE -g -O0 --coverage -fprofile-arcs -ftest-coverage)
target_link_libraries(pitch_decoder_test GTest::gtest_main gcov)
gtest_discover_tests(pitch_decoder_test)

add_executable(event_accumulator_test event_accumulator.cpp event_accumulator_test.cpp)
target_compile_options(event_accumulator_test PRIVATE -g -O0 --coverage -fprofile-arcs -ftest-coverage)
target_link_libraries(event_accumulator_test GTest::gtest_main gcov)
gtest_discover_tests(event_accumulator_test)

add_executable(summary_formatter_test summary_formatter.cpp summary_formatter_test.cpp)
target_compile_options(summary_formatter_test PRIVATE -g -O0 --coverage -fprofile-arcs -ftest-coverage)
target_link_libraries(summary_formatter_test GTest::gtest_main gcov)
gtest_discover_tests(summary_formatter_test)

