cmake_minimum_required(VERSION 3.15)

project(vfs)

# Basic global project settings
include(cmake/StandardProjectSettings.cmake)

# Project meta targets
add_library(vfs_project_options INTERFACE)
add_library(vfs_project_warnings INTERFACE)

# Create Compiler warnings interface
include(cmake/CompilerWarnings.cmake)
set_project_warnings(vfs_project_warnings)

# Create Project options interface
include(cmake/Sanitizers.cmake)
enable_sanitizers(vfs_project_options)

# Add targets
add_subdirectory(deps)
add_subdirectory(libvfs)
add_subdirectory(vfspack)
add_subdirectory(vfsexample)