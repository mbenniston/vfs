cmake_minimum_required(VERSION 3.15)

add_library(vfs_deps INTERFACE)

target_sources(vfs_deps PUBLIC 
    argparse.hpp
    argparse_nowarn.hpp
)

target_compile_features(vfs_deps INTERFACE cxx_std_20)

target_include_directories(
    vfs_deps
    INTERFACE
    .
)
