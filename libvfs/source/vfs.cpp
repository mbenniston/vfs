#include "vfs.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

namespace vfs
{
    File VirtualFS::getFileFromDisk(const std::string& fileName)
    {
        return File(m_diskManager.getDiskResource(fileName));
    }

    File VirtualFS::getFile(const std::string& fileName)
    {
        try
        {
            return getFileFromGlobalBundle(fileName);
        }
        catch(const FileDoesNotExistError&)
        {
        }
        
        return getFileFromDisk(fileName);
    }

    void VirtualFS::enableLiveReload()
    {
        m_diskManager.enableLiveReloading();
    }

    void VirtualFS::disableLiveReload()
    {
        m_diskManager.disableLiveReloading();
    }

    void VirtualFS::addGlobalBundle(const Bundle& bundle)
    {
        m_bundleManager.addGlobalBundle(bundle);
    }

    void VirtualFS::addBundle(const std::string& bundleName, const Bundle& bundle)
    {
        m_bundleManager.addBundle(bundleName, bundle);
    }

    void VirtualFS::removeGlobalBundle(const Bundle& bundle)
    {
        m_bundleManager.removeGlobalBundle(bundle);
    }

    void VirtualFS::removeBundle(const std::string& bundleName)
    {
        m_bundleManager.removeBundle(bundleName);
    }

    File VirtualFS::getFileFromGlobalBundle(const std::string& fileName)
    {
        return File(m_bundleManager.getResourceFromGlobalBundle(fileName));
    }

    File VirtualFS::getFileFromMountedBundle(const std::string& bundleName, const std::string& fileName)
    {
        return File(m_bundleManager.getResourceFromMountedBundle(bundleName, fileName));
    }

    VirtualFS::VirtualFS(bool fileLiveReloading) : m_diskManager(fileLiveReloading)
    {
    }
}