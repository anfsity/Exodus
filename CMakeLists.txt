cmake_minimum_required(VERSION 3.20)

project(
    compiler
    LANGUAGES CXX
    DESCRIPTION "Exodus!"
    VERSION 0.0.1
)

if(NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 17)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(__DEBUG)
endif()

find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

add_subdirectory(include)
add_subdirectory(src)

enable_testing()
add_subdirectory(tests)
