/**
 * @file vfs_bundle.hpp
 * @brief Contains the manager class responsible for handling the mounting and access of bundles
 */
#pragma once

#include "vfs_bundle_def.hpp"
#include "vfs_file.hpp"

namespace vfs
{
    // responsible for handling the mounting and access of bundles
    /**
     * @brief Handles mounting and access of bundles
     * Allows for the user to store global and named bundles (mounted bundles)
     * Also allows the user to access files within bundles.
     */
    class BundleManager final
    {
    private:
        std::vector<Bundle> m_globalBundles;
        std::unordered_map<std::string_view, Bundle> m_mountedBundles;

        std::unordered_map<std::string, std::pair<const Bundle*, std::weak_ptr<Resource>>> m_globalBundleResources;
        std::unordered_map<std::string, std::unordered_map<std::string, std::weak_ptr<Resource>>> m_mountedBundleResources;

        void disownMountedBundle(const std::string& bundleName);

    public:

        /**
         * @brief Adds a new bundle to the list of global bundles
         * 
         * @param bundle The bundle to be added to the global list
         */
        void addGlobalBundle(const Bundle& bundle);

        /**
         * @brief Removes a given bundle from the list of global bundles
         * 
         * @param bundle The bundle to be removed
         */
        void removeGlobalBundle(const Bundle& bundle);

        /**
         * @brief Mounts a new bundle at the given bundle name
         * 
         * @param bundleName The identifier to access the bundle
         * @param bundle The bundle to be attached
         */
        void addBundle(const std::string& bundleName, const Bundle& bundle);
        
        /**
         * @brief Removes a bundle from a given mount point
         * 
         * @param bundleName The bundle to be unmounted
         */
        void removeBundle(const std::string& bundleName);

        /**
         * @brief Retrieve a resource from the list of global bundles
         * 
         * @param fileName The name of the file to be retrieved
         * @return std::shared_ptr<Resource> A pointer to a shared resource representing the bundle data
         */
        std::shared_ptr<Resource> getResourceFromGlobalBundle(const std::string& fileName);
        
        /**
         * @brief Retrieve a resource from a specified mounted bundle 
         * 
         * @param bundleName The name of the bundle to be accessed
         * @param fileName The name of the file to be retrieved
         * @return std::shared_ptr<Resource> A pointer to a shared resource representing the bundle data
         */
        std::shared_ptr<Resource> getResourceFromMountedBundle(const std::string& bundleName, const std::string& fileName);

        // explicitly disable copying and moving 
        BundleManager& operator=(BundleManager&&) = delete;
        BundleManager& operator=(const BundleManager&) = delete;
        BundleManager(BundleManager&&) = delete;
        BundleManager(const BundleManager&) = delete;

        BundleManager();
        ~BundleManager();
    };
}