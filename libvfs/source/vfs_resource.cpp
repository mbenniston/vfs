#include "vfs_resource.hpp"

#include <fstream>
#include <iostream>
#include <chrono>
#include <array>

namespace vfs
{
    std::optional<TimePoint> tryGetLastModTime(const std::string& filePath)
    {
        std::error_code timeGetError;
        auto lastModTime = std::filesystem::last_write_time(filePath, timeGetError);

        if(timeGetError)
        {
            return std::nullopt;
        }

        return lastModTime;
    }

    std::vector<byte_t> loadDataFromDisk(const std::string& filePath)
    {
        std::ifstream fileStream{filePath, std::ios::binary};
        if(!fileStream)
        {
            throw FileDoesNotExistError(filePath);
        }

        // create a new file

        // read total contents of the file
        std::vector<byte_t> data;

        // first read size of file
        fileStream.seekg(0, std::ios::end);
        std::streamoff fileSize = fileStream.tellg();
        fileStream.seekg(0);

        if(fileSize < 0)
        {
            throw FileSizeError(filePath);
        }

        data.resize(
            static_cast<std::vector<unsigned char>::size_type>(fileSize));

        // then read the file in full
        fileStream.read(reinterpret_cast<char*>(data.data()), fileSize);

        return data;
    }

    std::pair<std::unique_lock<std::mutex>, const std::span<const byte_t>> Resource::read() const
    {
        if(m_disowned)
        {
            throw FileDisowned();
        }

        std::unique_lock<std::mutex> lock{m_dataLock};

        if(isFromDisk())
        {
            const std::vector<byte_t>& ownedData = std::get<DiskData>(m_data).loadedData;

            auto pair = std::make_pair(
                std::unique_lock<std::mutex>(), 
                std::span<const byte_t>(ownedData.begin(), ownedData.end()));

            pair.first.swap(lock);
            return pair;
        }

        auto pair = std::make_pair(
            std::unique_lock<std::mutex>(), 
            std::get<DataReference>(m_data).data);

        pair.first.swap(lock);

        return pair;
    }

    void Resource::write(const std::span<const byte_t> data)
    {
        (void)data;

        if(isFromDisk())
        {
            // write to file
            throw std::logic_error("Feature not implemented!");
        }
        else
        {
            throw std::runtime_error("Writing to non writeable resource!");
        }
    }

    void Resource::reload()
    {
        if(isFromDisk())
        {
            DiskData& dd = std::get<DiskData>(m_data);

            std::scoped_lock lock(m_dataLock);
            
            dd.timeLastModified = tryGetLastModTime(dd.dataSourceFileName);
            dd.loadedData = loadDataFromDisk(dd.dataSourceFileName);
        }
        
        // copy a list of the observers
        m_observersLock.lock();
        auto observers = m_observers;
        m_observersLock.unlock();

        // and call their callbacks
        std::for_each(
            observers.begin(), 
            observers.end(), 
            [](auto& ob){ ob->onFileReload(); });
    }

    void Resource::disown()
    {
        m_disowned = true;
    }

    std::optional<TimePoint> Resource::getLastModifiedTime() const
    {
        if(isFromDisk())
        {
            return std::get<DiskData>(m_data).timeLastModified;
        }

        return std::nullopt;
    }

    bool Resource::isDisowned() const
    {
        return m_disowned;
    }

    bool Resource::isFromDisk() const
    {
        return m_data.index() == 1;
    }

    bool Resource::isDataReference() const
    {
        return m_data.index() == 0;
    }

    void Resource::addObserver(std::shared_ptr<ResourceChangeObserver> observer)
    {
        std::scoped_lock lock(m_observersLock);
        m_observers.push_back(observer);
    }
    
    void Resource::removeObserver(std::shared_ptr<ResourceChangeObserver> observer)
    {
        std::scoped_lock lock(m_observersLock);
        m_observers.erase(std::find(m_observers.begin(), m_observers.end(), observer));
    }

    Resource::Resource(const std::span<const byte_t> data) : m_data(DataReference{data})
    {
    }

    Resource::Resource(const std::string& fileName) : m_data(
        DiskData{ 
            fileName,
            loadDataFromDisk(fileName),
            tryGetLastModTime(fileName)
        }
    )
    {
    }
}