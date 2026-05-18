# include/CMakeLists.txt
add_library(headers INTERFACE)

target_include_directories(headers INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(headers INTERFACE fmt)
