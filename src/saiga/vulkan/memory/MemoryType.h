//
// Created by Peter Eichinger on 2018-11-30.
//

#pragma once

#include <ostream>

namespace Saiga
{
namespace Vulkan
{
namespace Memory
{
template <typename T>
struct MemoryType
{
    T usageFlags;
    vk::MemoryPropertyFlags memoryFlags;

    bool operator==(const MemoryType<T>& rhs) const
    {
        return std::tie(usageFlags, memoryFlags) == std::tie(rhs.usageFlags, rhs.memoryFlags);
    }

    bool operator!=(const MemoryType<T>& rhs) const { return !(rhs == *this); }

    friend std::ostream& operator<<(std::ostream& os, const MemoryType& type)
    {
        auto usage = vk::to_string(type.usageFlags);
        auto flags = vk::to_string(type.memoryFlags);
        os << "{ " << usage.substr(1, usage.size() - 2) << ", " << flags.substr(1, flags.size() - 2) << " }";
        return os;
    }

    bool operator<(const MemoryType& rhs) const
    {
        if (static_cast<unsigned int>(usageFlags) < static_cast<unsigned int>(rhs.usageFlags))
        {
            return true;
        }
        return static_cast<unsigned int>(memoryFlags) < static_cast<unsigned int>(rhs.memoryFlags);
    }

    bool operator>(const MemoryType& rhs) const { return rhs < *this; }

    bool operator<=(const MemoryType& rhs) const { return !(rhs < *this); }

    bool operator>=(const MemoryType& rhs) const { return !(*this < rhs); }
};

struct SAIGA_GLOBAL BufferType : public MemoryType<vk::BufferUsageFlags>
{
};

struct SAIGA_GLOBAL ImageType : public MemoryType<vk::ImageUsageFlags>
{
};

}  // namespace Memory
}  // namespace Vulkan
}  // namespace Saiga

namespace std
{
template <typename T>
struct SAIGA_GLOBAL hash<Saiga::Vulkan::Memory::MemoryType<T>>
{
    typedef Saiga::Vulkan::Memory::MemoryType<T> argument_type;
    typedef std::size_t result_type;
    result_type operator()(argument_type const& s) const noexcept
    {
        result_type const h1(std::hash<unsigned int>{}(static_cast<unsigned int>(s.usageFlags)));
        result_type const h2(std::hash<unsigned int>{}(static_cast<unsigned int>(s.memoryFlags)));
        return h1 ^ (h2 << 1);
    }
};
template <>
struct SAIGA_GLOBAL hash<Saiga::Vulkan::Memory::BufferType>
    : public hash<Saiga::Vulkan::Memory::MemoryType<vk::BufferUsageFlags>>
{
};
template <>
struct SAIGA_GLOBAL hash<Saiga::Vulkan::Memory::ImageType>
    : public hash<Saiga::Vulkan::Memory::MemoryType<vk::ImageUsageFlags>>
{
};
}  // namespace std