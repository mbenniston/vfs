#include "vfs_file.hpp"

#include <iostream>

namespace vfs
{
    ResourceAccessGuard File::read() const
    {
        auto[lock, data] = m_resource->read();

        return ResourceAccessGuard(lock, data, m_resource);
    }

    void File::write(const std::span<const byte_t> data)
    {
        m_resource->write(data);
    }

    void File::reload()
    {
        m_resource->reload();
    }

    bool File::isReadOnly() const
    {
        return m_resource->isDataReference();
    }

    bool File::isDisowned() const
    {
        return m_resource->isDisowned();
    }

    void File::addObserver(std::shared_ptr<ResourceChangeObserver> observer)
    {
        m_resource->addObserver(observer);
    }

    void File::removeObserver(std::shared_ptr<ResourceChangeObserver> observer)
    {
        m_resource->removeObserver(observer);
    }
}