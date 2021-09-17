cmake_minimum_required(VERSION 3.15)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/myBundle.hpp ${CMAKE_CURRENT_BINARY_DIR}/myBundle.cpp
    COMMAND vfspack --recursive ${CMAKE_CURRENT_BINARY_DIR}/myBundle.cpp ${CMAKE_CURRENT_BINARY_DIR}/myBundle.hpp res
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    DEPENDS res)

add_executable(vfsexample vfsexample.cpp ${CMAKE_CURRENT_BINARY_DIR}/myBundle.cpp)
target_include_directories(vfsexample PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/)

target_link_libraries(vfsexample PRIVATE vfs_project_options vfs_project_warnings vfs)