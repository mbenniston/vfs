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
     * @brief Represents the different types of reload modes that the disk manager can use
     */
    enum class ReloadMode
    {
        NO_LIVE_RELOAD, // No reload callbacks to file observers
        ASYNC_LIVE_RELOAD, // Asynchronous callbacks
        POLL_LIVE_RELOAD // Polling triggered callbacks which requires the user to call the "pollForUpdatedFiles" method
    };

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

        ReloadMode m_reloadMode;

        void enableAsyncReload();
        void disableAsyncReload();
        void checkForUpdatedFiles();

    public:
        /**
         * @brief Sets the Reload Mode to be used
         * 
         * @param newMode The mode to be used
         */
        void setReloadMode(ReloadMode newMode);
        
        /**
         * @brief Gets the current Reload Mode
         * 
         * @return ReloadMode The reload strategy currently in use
         */
        ReloadMode getReloadMode() const;

        /**
         * @brief Checks for updated files and calls their observers callbacks
         */
        void pollForUpdatedFiles();

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

        DiskManager(ReloadMode reloadMode = ReloadMode::NO_LIVE_RELOAD);
        ~DiskManager();
    };
}