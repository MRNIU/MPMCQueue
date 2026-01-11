cmake_minimum_required(VERSION 3.20)

project(MPMCQueue
    VERSION 1.0.0
    DESCRIPTION "Multi-Producer Multi-Consumer Lock-Free Queue for Freestanding C++26"
    LANGUAGES CXX
)

# Set C++26 standard
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands for IDE support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Header-only library interface
add_library(mpmc_queue INTERFACE)
target_include_directories(mpmc_queue INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Compiler-specific settings for freestanding environment
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
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
elseif(MSVC)
    target_compile_options(mpmc_queue INTERFACE
        /W4
        /WX
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

# Installation rules
include(GNUInstallDirs)

install(TARGETS mpmc_queue
    EXPORT MPMCQueueTargets
)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT MPMCQueueTargets
    FILE MPMCQueueTargets.cmake
    NAMESPACE mpmc::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MPMCQueue
)

# Generate package config file
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/MPMCQueueConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/MPMCQueueConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/MPMCQueueConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MPMCQueue
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/MPMCQueueConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/MPMCQueueConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MPMCQueue
)
