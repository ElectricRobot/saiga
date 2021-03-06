//
// Created by Peter Eichinger on 08.10.18.
//

#pragma once
#include <iostream>
#include <saiga/util/assert.h>
#include <vulkan/vulkan.hpp>

namespace Saiga
{
namespace Vulkan
{
namespace Memory
{
struct Chunk
{
    vk::DeviceMemory memory;
    vk::DeviceSize size;
    vk::MemoryPropertyFlags flags;

    Chunk(vk::DeviceMemory _memory, vk::DeviceSize _size, vk::MemoryPropertyFlags _flags)
        : memory(_memory), size(_size), flags(_flags)
    {
    }
};

}  // namespace Memory
}  // namespace Vulkan
}  // namespace Saiga
