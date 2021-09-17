/**
 * @file vfs_errors.hpp
 * @brief Contains common virtual file system errors
 */
#pragma once
#include <stdexcept>
#include <string>

namespace vfs
{
    class FileDoesNotExistError : public std::runtime_error{
    public:
        FileDoesNotExistError(const std::string& fileName) : 
            std::runtime_error("File: \"" + fileName + "\" does not exist!") {}
    };

    class FileSizeError : public std::runtime_error{
    public:
        FileSizeError(const std::string& fileName) : 
            std::runtime_error("File size of \"" + fileName + "\" could not be determined!") {}
    };

    class BundleDoesNotExistError : public std::runtime_error{
    public:
        BundleDoesNotExistError(const std::string& bundleName) : 
            std::runtime_error("Bundle: \"" + bundleName + "\" does not exist!") {}
    };

    class BundleWriteError : public std::runtime_error{
    public:
        BundleWriteError() : std::runtime_error("Cannot write mounted to bundle file!") {}
    };

    class FileDisowned : public std::runtime_error{
    public:
        FileDisowned() : std::runtime_error("VirtualFS has been destroyed, data is no longer valid") {}
    };
}