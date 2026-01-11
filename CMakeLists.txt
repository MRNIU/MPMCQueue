# Copyright The MPMCQueue Contributors

cmake_minimum_required(VERSION 3.20)

project(MPMCQueue
    DESCRIPTION "Multi-Producer Multi-Consumer Lock-Free Queue for Freestanding C++26"
    LANGUAGES CXX
)

# Header-only library interface
add_library(mpmc_queue INTERFACE)
target_include_directories(mpmc_queue INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_options(mpmc_queue INTERFACE
    -Wall
    -Wextra
    -Wpedantic
    -Werror
)

# Freestanding environment flags (optional, can be enabled if needed)
option(MPMC_FREESTANDING "Enable freestanding environment mode" OFF)
if(MPMC_FREESTANDING)
    target_compile_options(mpmc_queue INTERFACE
        -ffreestanding
        -fno-exceptions
        -fno-rtti
    )
endif()

# Option to build examples
option(MPMC_BUILD_EXAMPLES "Build example programs" ON)

if(MPMC_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Option to build tests
option(MPMC_BUILD_TESTS "Build test programs" ON)

if(MPMC_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
