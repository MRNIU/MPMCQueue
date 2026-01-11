cmake_minimum_required(VERSION 3.20)

# Basic example
add_executable(basic_example basic_example.cpp)
target_link_libraries(basic_example PRIVATE mpmc_queue)

# Multi-threaded example
find_package(Threads)
if(Threads_FOUND)
    add_executable(threaded_example threaded_example.cpp)
    target_link_libraries(threaded_example PRIVATE mpmc_queue Threads::Threads)
endif()
