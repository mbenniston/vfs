/**
 * @file vfs.hpp
 * @brief Central header that contains the VirtualFS which allows access to bundles and disk files in an organsied way.
 */
#pragma once

#include "vfs_disk.hpp"
#include "vfs_bundle.hpp"

namespace vfs
{
    /**
     * @brief Allows the access to files from bundles or disk
     * This is the main class that should be used from vfs
     */
    class VirtualFS final
    {
    private:
        DiskManager m_diskManager;
        BundleManager m_bundleManager;

    public:

        /**
         * @brief Enables callbacks to file observers when the file is changed 
         */
        void enableLiveReload();

        /**
         * @brief Disables callbacks to file observers when the file is changed 
         */
        void disableLiveReload();

        /**
         * @brief Appends a new global bundle to the list of global bundles
         * 
         * @param bundle The bundle to be added
         */
        void addGlobalBundle(const Bundle& bundle);

        /**
         * @brief Removes a global bundle from the list of global bundles
         * 
         * @param bundle The bundle to be removed
         */
        void removeGlobalBundle(const Bundle& bundle);

        /**
         * @brief Adds a bundle to the virtual filesystem that has to be explicitly accessed with the bundles name
         * 
         * @param bundleName The name to access the bundle with 
         * @param bundle The bundle to be added
         */
        void addBundle(const std::string& bundleName, const Bundle& bundle);
        
        /**
         * @brief Removes a named bundle
         * 
         * @param bundleName The name of the budle to be removed
         */
        void removeBundle(const std::string& bundleName);

        /**
         * @brief Get a named file from the list of global bundles
         * 
         * @param fileName The name of the file to be retrieved
         * @return File The file with the given file name
         */
        File getFileFromGlobalBundle(const std::string& fileName);
        
        /**
         * @brief Get a named file from a named bundle
         * 
         * @param bundleName The name of the bundle in which to search for the file
         * @param fileName The name of the file to be retrieved
         * @return File The file with the given file name
         */
        File getFileFromMountedBundle(const std::string& bundleName, const std::string& fileName);

        /**
         * @brief Get a file from disk
         * 
         * @param fileName The path to the file on disk
         * @return File The file with the given file name
         */
        File getFileFromDisk(const std::string& fileName);

        /**
         * @brief General file access function
         * First searchs the within the global bundles and then falls back to disk if not found in the global bundles.
         * Does not search within named bundles at all. 
         * 
         * @param fileName The name of the file to be retrieved
         * @return File The file with the given file name
         */
        File getFile(const std::string& fileName);

        /**
         * @brief Construct a new VirtualFS object
         * 
         * @param fileLiveReloading Enables live-reloading of disk files
         */
        VirtualFS(bool fileLiveReloading = true);
    };
}