cmake_minimum_required(VERSION 3.15)

add_executable(vfspack
    vfspack.cpp
)

target_link_libraries(vfspack PRIVATE vfs_project_options vfs_project_warnings vfs_deps)