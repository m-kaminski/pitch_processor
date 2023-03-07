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

add_executable(base36_test base36_test.cpp)
target_link_libraries(base36_test GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(base36_test)
