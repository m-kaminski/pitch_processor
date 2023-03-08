cmake_minimum_required(VERSION 3.14)
project(my_project)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
  )

FetchContent_MakeAvailable(googletest)

enable_testing()
include(GoogleTest)


add_subdirectory(io_engine)
add_subdirectory(pitch)

add_executable(pitch_processor 
main.cpp 
worker_thread.cpp
pitch/summary_formatter.cpp 
io_engine/io_engine_ios.cpp 
io_engine/io_engine_aio.cpp 
pitch/event_accumulator.cpp
)
target_link_libraries(pitch_processor rt pthread)



### tests
add_executable(worker_thread_test worker_thread.cpp worker_thread_test.cpp)
target_link_libraries(worker_thread_test GTest::gtest_main)
gtest_discover_tests(worker_thread_test pthread)
