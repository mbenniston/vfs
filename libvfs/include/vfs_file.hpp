/**
 * @file vfs_file.hpp
 * @brief Contains definition of a File
 */
#pragma once
#include <filesystem>
#include <memory>

#include "vfs_resource.hpp"

// vfs file search order
// internal (top level), internal (bundles), disk  
// checks every second for modifcation time inequivilance on a seperate thread

namespace vfs
{
    /**
     * @brief Provides access to a files data
     * data() is only valid for the lifetime of the resource access guard object
     */
    struct ResourceAccessGuard final
    {
    protected:
        std::unique_lock<std::mutex> m_dataLock;
        std::shared_ptr<Resource> m_resource;
        const std::span<const byte_t> m_data;

    public:

        /**
         * @brief Data getter
         * 
         * @return const std::span<const byte_t> a reference to the data
         */
        const std::span<const byte_t> data() const
        {
            return m_data;
        } 

        /**
         * @brief Construct a new Resource Access Guard object from a temporary resource access guard
         * 
         * @param contents The temporary access guard to be transferred to this instance
         */
        ResourceAccessGuard(ResourceAccessGuard&& contents)
        {
            m_dataLock.swap(contents.m_dataLock);
            std::swap(contents.m_resource, m_resource);
        }

        /**
         * @brief Construct a new Resource Access Guard object
         * 
         * @param lock A unique_lock that is currently locked so the data can be accessed safely
         * @param data A reference to the data
         * @param resource A pointer to the shared resource that owns the data
         */
        ResourceAccessGuard(
            std::unique_lock<std::mutex>& lock,
            const std::span<const byte_t> data,
            std::shared_ptr<Resource> resource) : m_resource(resource), m_data(data)
        {
            m_dataLock.swap(lock);
        }
    };

    /**
     * @brief Adapter class for a shared resource
     * Allows the user to access a shared resource
     */
    class File final
    {
    private:
        std::shared_ptr<Resource> m_resource;

    public:

        /**
         * @brief Reads the contents of the resource
         * 
         * @return ResourceAccessGuard The accessor class to get the data from the resource
         */
        ResourceAccessGuard read() const;

        /**
         * @brief Currently not implemented
         * 
         * @param data 
         */
        void write(const std::span<const byte_t> data);
        
        /**
         * @brief Re-reads the file from disk or the bundle
         */
        void reload();

        /**
         * @brief Checks if the file is read-only
         * 
         * @return true The file is read only
         * @return false The file can be written to and read from
         */
        bool isReadOnly() const;

        /**
         * @brief Checks if the file has been disowned by the file system
         * This means that its resource is no-longer valid and cannot be used.
         * @return true The file has been disowned
         * @return false The resource is valid
         */
        bool isDisowned() const;

        /**
         * @brief Attaches a new observer to watch for reload events
         * 
         * @param observer A pointer to the observer to be added
         */
        void addObserver(std::shared_ptr<ResourceChangeObserver> observer);

        /**
         * @brief Detaches an observer so it no longer watches for file events
         * 
         * @param observer A pointer to the observer to be removed
         */
        void removeObserver(std::shared_ptr<ResourceChangeObserver> observer);

        /**
         * @brief Construct a new File object from a shared resource
         * 
         * @param res A pointer to a shared resource 
         */
        File(std::shared_ptr<Resource> res) : m_resource(res)
        {
        }
    };

}