#include "vfs_bundle.hpp"

namespace vfs
{
    static void disownResource(std::weak_ptr<Resource>& resPtr)
    {
        auto res = resPtr.lock();
        if(res)
        {
            res->disown();
        }
    }

    static const std::span<const byte_t> getDataFromBundle(const Bundle& bundle, const std::string& fileName)
    {
        if(bundle.files.count(fileName) == 0)
        {
            throw FileDoesNotExistError(fileName);
        }

        const FileTableEntry& entry = bundle.files.at(fileName);

        auto startByte = bundle.blob.begin() + static_cast<long>(entry.startByte);
        auto endByte = startByte + static_cast<long>(entry.length);

        return std::span<const byte_t>(startByte, endByte);
    }

    void BundleManager::addGlobalBundle(const Bundle& bundle)
    {
        // invalidate old files now shadowed by the bundle?
        for(const auto&[fileName, fileEntry] : bundle.files)
        {
            if(m_globalBundleResources.count(fileName) > 0)
            {
                auto&[bundlePtr, resource] = m_globalBundleResources.at(fileName);
                disownResource(resource);
                m_globalBundleResources.erase(fileName);
            }
        }

        m_globalBundles.insert(m_globalBundles.begin(), bundle);
    }

    void BundleManager::disownMountedBundle(const std::string& bundleName)
    {
        auto& oldBundleResources = m_mountedBundleResources.at(bundleName);  
        for(auto& resourceEntry : oldBundleResources)
        {
            disownResource(resourceEntry.second);
        }

        m_mountedBundleResources.erase(bundleName);
    }

    void BundleManager::addBundle(const std::string& bundleName, const Bundle& bundle)
    {
        // invalidate old bundle if it existed files
        if(m_mountedBundleResources.count(bundleName) > 0)
        {
            disownMountedBundle(bundleName);
        }

        m_mountedBundles[bundleName] = bundle;
    }

    void BundleManager::removeBundle(const std::string& bundleName)
    {
        if(m_mountedBundleResources.count(bundleName) > 0)
        {
            disownMountedBundle(bundleName);
        }

        m_mountedBundles.erase(bundleName);
    }

    static bool operator==(const Bundle& left, const Bundle& right)
    {
        return left.files == right.files && 
               left.blob.data() == right.blob.data() && 
               left.blob.size() == right.blob.size();
    }

    void BundleManager::removeGlobalBundle(const Bundle& bundle)
    {
        auto bundleItr = std::find(m_globalBundles.begin(), m_globalBundles.end(), bundle);
        const Bundle* bundle_ptr = &(*bundleItr);

        for(auto itr = m_globalBundleResources.begin(); itr != m_globalBundleResources.end();)
        {
            auto&[fileName, filePair] = *itr;
            auto&[itrBundlePtr, file] = filePair;

            if(itrBundlePtr == bundle_ptr)
            {
                auto res = file.lock();
                if(res)
                {
                    res->disown();
                }
                itr = m_globalBundleResources.erase(itr);
            }
            else
            {
                itr++;
            }

        }

        m_globalBundles.erase(bundleItr);
    }

    std::shared_ptr<Resource> BundleManager::getResourceFromGlobalBundle(const std::string& fileName)
    {
        // check already loaded bundle resources
        if(m_globalBundleResources.count(fileName) > 0)
        {
            auto res = m_globalBundleResources.at(fileName).second.lock();
            if(res)
            {
                return res;
            }
        }

        // check global bundles
        for(const Bundle& bundle : m_globalBundles)
        {
            try
            {
                auto bundleFile = std::make_shared<Resource>(getDataFromBundle(bundle, fileName));
                m_globalBundleResources[fileName] = std::make_pair(&bundle, bundleFile);
                
                return bundleFile;
            }
            catch(const FileDoesNotExistError&)
            {
            }
        }

        throw FileDoesNotExistError(fileName);
    }

    std::shared_ptr<Resource> BundleManager::getResourceFromMountedBundle(const std::string& bundleName, const std::string& fileName)
    {
        // check already loaded bundle resources
        if(m_mountedBundleResources.count(bundleName) > 0) 
        {
            const auto& bundleResources = m_mountedBundleResources.at(bundleName);

            if(bundleResources.count(fileName) > 0)
            {
                auto res = bundleResources.at(fileName).lock();
                if(res)
                {
                    return res;
                }
            }
        }

        if(m_mountedBundles.count(bundleName) == 0)
        {
            throw BundleDoesNotExistError(bundleName);
        }

        const Bundle& bundle = m_mountedBundles.at(bundleName);

        auto bundleFile = std::make_shared<Resource>(getDataFromBundle(bundle, fileName));
        m_mountedBundleResources[bundleName][fileName] = bundleFile;

        return bundleFile;
    }

    BundleManager::BundleManager()
    {
    }

    BundleManager::~BundleManager()
    {
        for(auto& globalResource : m_globalBundleResources)
        {
            disownResource(globalResource.second.second);
        }

        for(auto& bundle : m_mountedBundleResources)
        {
            for(auto& bundleResource : bundle.second)
            {
                disownResource(bundleResource.second);
            }
        }
    }
}