cmake_minimum_required(VERSION 3.15)

add_library(vfs)
target_compile_features(vfs PUBLIC cxx_std_20)

target_sources(vfs PRIVATE
    include/vfs.hpp

    source/vfs.cpp    
    source/vfs_file.cpp    
    source/vfs_disk.cpp    
    source/vfs_resource.cpp    
    source/vfs_bundle.cpp    
)

target_include_directories(vfs PUBLIC include)

target_link_libraries(vfs PRIVATE vfs_project_options vfs_project_warnings)
target_link_libraries(vfs PUBLIC pthread)