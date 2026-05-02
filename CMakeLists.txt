# CMakeLists.txt
cmake_minimum_required(VERSION 3.20)

project(
    compiler
    LANGUAGES CXX
    DESCRIPTION "Exodus!"
    VERSION 0.0.1
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# --- Flex & Bison (当前环境缺失，暂时注释) ---
# find_package(FLEX REQUIRED)
# find_package(BISON REQUIRED)

add_compile_definitions(__DEBUG)

link_libraries(stdc++exp)

add_subdirectory(include)
add_subdirectory(src)
