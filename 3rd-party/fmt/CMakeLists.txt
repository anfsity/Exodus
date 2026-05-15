# 3rd-party/fmt/CMakeLists.txt
add_library(fmt INTERFACE)

target_include_directories(fmt INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_compile_definitions(fmt INTERFACE FMT_HEADER_ONLY=1)
