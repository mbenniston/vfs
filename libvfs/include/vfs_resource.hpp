/**
 * @file vfs_resource.hpp
 * @brief Contains Resource class definition responsible for storing (or pointing to) data.
 */
#pragma once
#include <span>
#include <optional>
#include <filesystem>
#include <mutex>
#include <variant>
#include <vector>

#include "vfs_base.hpp"
#include "vfs_errors.hpp"

// vfs file search order
// internal (top level), internal (bundles), disk  
// checks every second for modifcation time inequivilance on a seperate thread

namespace vfs
{
    using TimePoint = std::filesystem::file_time_type;
    
    /**
     * @brief Attempts to return the last time a file was edited on disk
     * Will return std::nullopt if the file doesn't exist or if an error occurs when getting the time.
     * @param filePath The path to the file
     * @return std::optional<TimePoint> The time the file was edited last
     */
    std::optional<TimePoint> tryGetLastModTime(const std::string& filePath);

    /**
     * @brief Helper function to load a file in-full from disk
     * 
     * @param filePath The path to the file to be read
     * @return std::vector<byte_t> The data
     */
    std::vector<byte_t> loadDataFromDisk(const std::string& filePath);

    /**
     * @brief Interface class for an object that wishes to be notified of file reload events
     * @note make persistent?
     */
    struct ResourceChangeObserver
    {
        /**
         * @brief Callback function called whenever a file is changed on disk and has been reloaded.
         */
        virtual void onFileReload() = 0;

        virtual ~ResourceChangeObserver() = default;  
    };

    struct DataReference
    {
        std::span<const byte_t> data;
    };

    /**
     * @brief Contains data and meta-data loaded from disk
     */
    struct DiskData
    {
        std::string dataSourceFileName;
        std::vector<byte_t> loadedData;
        std::optional<TimePoint> timeLastModified;
    };

    /**
     * @brief Represents a source of data from either ownership of the data or a reference to some data in memory
     */
    class Resource final
    {
    private:
        std::variant<DataReference, DiskData> m_data;
        std::vector<std::shared_ptr<ResourceChangeObserver>> m_observers;
        mutable std::mutex m_dataLock;
        mutable std::mutex m_observersLock;

        bool m_disowned = false;

    public:

        /**
         * @brief Gives the caller access to read the resource
         * 
         * @return std::pair<std::unique_lock<std::mutex>, const std::span<const byte_t>> Returns a reference to some data and a lock to use while accessing the data
         */
        std::pair<std::unique_lock<std::mutex>, const std::span<const byte_t>> read() const;

        /**
         * @brief Writes the file back to disk (Not implemented)
         * 
         * @param data 
         */
        void write(const std::span<const byte_t> data);
        
        /**
         * @brief Re-reads the file from disk
         */
        void reload();
        
        /**
         * @brief Disowns the resource meaning it is now invalid and cannot be read / written 
         */
        void disown();

        /**
         * @brief Get the Last Modified Time object if it has one
         * 
         * @return std::optional<TimePoint> The time the resource was modified last
         */
        std::optional<TimePoint> getLastModifiedTime() const;

        /**
         * @brief Checks whether the resource has been disowned 
         * 
         * @return true The resource has been disowned and is no longer valid
         * @return false The resource is still valid
         */
        bool isDisowned() const;

        /**
         * @brief Checks whether the resource originated from disk
         * 
         * @return true The resource is from disk
         * @return false The resource is from memory (Bundle)
         */
        bool isFromDisk() const;
        
        /**
         * @brief Checks whether the resource is from memory
         * 
         * @return true The resource points to data in a Bundle
         * @return false The resource is from disk
         */
        bool isDataReference() const;

        /**
         * @brief Registers an observer with the resource so it can be notified of events
         * 
         * @param observer The observer to be added
         */
        void addObserver(std::shared_ptr<ResourceChangeObserver> observer);
        
        /**
         * @brief Deregisters an observer from the resource
         * 
         * @param observer The observer to be removed
         */
        void removeObserver(std::shared_ptr<ResourceChangeObserver> observer);

        // cannot move or copy a resource
        Resource& operator=(const Resource&) = delete;
        Resource& operator=(Resource&&) = delete;
        Resource(const Resource&) = delete;
        Resource(Resource&&) = delete;

        /**
         * @brief Construct a new Resource object from a reference to some data in memory
         * 
         * @param data Pointer to data in memory
         */
        Resource(const std::span<const byte_t> data);

        /**
         * @brief Construct a new Resource object from loading a file from disk
         * 
         * @param fileName The name of the file to be read
         */
        Resource(const std::string& fileName);
    };
}