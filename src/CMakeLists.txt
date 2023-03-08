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
add_subdirectory(execution_policy)

add_executable(pitch_processor 
main.cpp 
execution_policy/execution_policy_single_threaded.cpp
execution_policy/execution_policy_multi_threaded.cpp
execution_policy/worker_thread.cpp
io_engine/io_engine_aio.cpp 
io_engine/io_engine_ios.cpp 
pitch/event_accumulator.cpp
pitch/summary_formatter.cpp 
)
target_link_libraries(pitch_processor rt pthread)


