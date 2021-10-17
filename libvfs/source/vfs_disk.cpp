#include "vfs_disk.hpp"

#include <iostream>
#include <fstream>

namespace vfs
{
    std::shared_ptr<Resource> DiskManager::getDiskResource(const std::string& fileName)
    {
        // check for existing file
        {
            std::scoped_lock<std::mutex> lock{m_diskResourcesLock};

            if(m_diskResources.count(fileName) > 0) 
            {
                auto diskFile = m_diskResources.at(fileName).lock();
                if(diskFile != nullptr)
                {
                    // check for a more updated file
                    auto lastModTime = diskFile->getLastModifiedTime(); 
                    auto newModTime = tryGetLastModTime(fileName);

                    // if the new time is less than or the same as the old time
                    if(!(lastModTime && newModTime && *newModTime > *lastModTime))
                    {
                        return diskFile;
                    }
                }
            }
        }

        // otherwise load from disk
        auto file = std::make_shared<Resource>(fileName);

        {
            std::scoped_lock<std::mutex> lock{m_diskResourcesLock};
            m_diskResources[fileName] = file;
        }

        return file;
    }

    void DiskManager::checkForUpdatedFiles()
    {
        m_diskResourcesLock.lock();
        for(auto diskFile : m_diskResources)
        {
            auto file = diskFile.second.lock();
            if(file != nullptr)
            {
                auto lastModTime = file->getLastModifiedTime();
                if(lastModTime) 
                {
                    auto newModTime = tryGetLastModTime(diskFile.first);
                    if(newModTime && *newModTime > *lastModTime) 
                    {
                        file->reload();
                    }
                }
            } 
        }       
        m_diskResourcesLock.unlock();
    }

    void DiskManager::enableAsyncReload()
    {
        if(!m_changeCheckThread.has_value())
        {
            // 'this' is fine in this case because copying / moving of the class is disabled
            m_changeCheckThread = std::jthread(
                [this](std::stop_token stopToken)
                {
                    while(!stopToken.stop_requested()) {
                        std::this_thread::sleep_for(std::chrono::milliseconds(CHANGE_CHECK_DELAY_MS));

                        checkForUpdatedFiles();
                    }
                }
            );
        }
    }

    void DiskManager::disableAsyncReload()
    {
        m_changeCheckThread = std::nullopt;
    }

    void DiskManager::setReloadMode(ReloadMode newMode)
    {
        if(m_reloadMode == ReloadMode::ASYNC_LIVE_RELOAD && newMode != ReloadMode::ASYNC_LIVE_RELOAD) { 
            disableAsyncReload();
        } else if(m_reloadMode != ReloadMode::ASYNC_LIVE_RELOAD && newMode == ReloadMode::ASYNC_LIVE_RELOAD) { 
            enableAsyncReload();
        }

        m_reloadMode = newMode;
    }

    ReloadMode DiskManager::getReloadMode() const
    {
        return m_reloadMode;
    }

    void DiskManager::pollForUpdatedFiles()
    {
        if(m_reloadMode == ReloadMode::POLL_LIVE_RELOAD)
        {
            checkForUpdatedFiles();
        }
    }

    DiskManager::DiskManager(ReloadMode mode) : m_reloadMode(mode)
    {
        if(mode == ReloadMode::ASYNC_LIVE_RELOAD)
        {
            enableAsyncReload();
        }
    }

    DiskManager::~DiskManager()
    {
        for(auto& file : m_diskResources)
        {
            auto filePtr = file.second.lock();
            if(filePtr != nullptr) {
                filePtr->disown();
            }
        }
    }
}