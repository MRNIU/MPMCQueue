cmake_minimum_required(VERSION 3.20)

# Basic functionality test
add_executable(test_basic test_basic.cpp)
target_link_libraries(test_basic PRIVATE mpmc_queue)
add_test(NAME BasicFunctionality COMMAND test_basic)

# Type safety test
add_executable(test_types test_types.cpp)
target_link_libraries(test_types PRIVATE mpmc_queue)
add_test(NAME TypeSafety COMMAND test_types)

# Multi-threaded test (if threads are available)
find_package(Threads)
if(Threads_FOUND)
    add_executable(test_concurrent test_concurrent.cpp)
    target_link_libraries(test_concurrent PRIVATE mpmc_queue Threads::Threads)
    add_test(NAME ConcurrentOperations COMMAND test_concurrent)
endif()
