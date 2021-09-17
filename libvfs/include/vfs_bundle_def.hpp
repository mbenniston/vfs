/**
 * @file vfs_bundle_def.hpp
 * @brief Contains Bundle structure definition 
 */
#pragma once
#include <span>
#include <unordered_map>
#include <string>

#include "vfs_base.hpp"

namespace vfs
{
    /**
     * @brief The location of a file within a bundle data blob
     */
    struct FileTableEntry
    {
        std::size_t startByte;
        std::size_t length;

        bool operator==(const FileTableEntry&) const = default;
    };

    /**
     * @brief A data blob and file table which contains the file data and file locations within the blob respectfully
     */
    struct Bundle
    {
        std::span<const byte_t> blob;
        std::unordered_map<std::string, FileTableEntry> files;
    };
}