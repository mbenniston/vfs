/**
 * @file vfs_disk.hpp
 * @brief Contains the manager class responsible for handling the mounting and access of files from the operating systems files sytem
 */
#pragma once

#include <unordered_map>
#include <memory>
#include <variant>
#include <thread>
#include <mutex>

#include "vfs_file.hpp"

namespace vfs
{
    /**
     * @brief Handles the loading and live-reloading of files retrieved from disk 
     */
    class DiskManager final
    {
    private:
        std::unordered_map<std::string, std::weak_ptr<Resource>> m_diskResources;

        std::optional<std::jthread> m_changeCheckThread;
        std::mutex m_diskResourcesLock;

        static constexpr std::int64_t CHANGE_CHECK_DELAY_MS = 100;

    public:
        
        /**
         * @brief Starts up live-reloading if previously disabled
         * 
         */
        void enableLiveReloading();

        /**
         * @brief Shuts down live-reloading if previously enabled
         * 
         */
        void disableLiveReloading();

        /**
         * @brief Get the Disk Resource object
         * 
         * @param fileName 
         * @return std::shared_ptr<Resource> 
         */
        std::shared_ptr<Resource> getDiskResource(const std::string& fileName);

        DiskManager& operator=(DiskManager&&) = delete;
        DiskManager& operator=(const DiskManager&) = delete;
        DiskManager(DiskManager&&) = delete;
        DiskManager(const DiskManager&) = delete;

        DiskManager(bool liveReloadEnabled = true);
        ~DiskManager();
    };
}