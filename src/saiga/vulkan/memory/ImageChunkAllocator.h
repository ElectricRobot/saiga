//
// Created by Peter Eichinger on 30.10.18.
//

#pragma once
#include "BaseChunkAllocator.h"

namespace Saiga{
namespace Vulkan{
namespace Memory{

class ImageChunkAllocator : public BaseChunkAllocator {

public:
    ImageChunkAllocator() : BaseChunkAllocator() {

    }
    ImageChunkAllocator(const vk::Device &_device, ChunkBuilder *chunkAllocator, const vk::MemoryPropertyFlags &_flags,
                        FitStrategy &strategy, vk::DeviceSize chunkSize, bool _mapped = false);

    void destroy();

protected:
    ChunkIterator createNewChunk() override;
};

}
}
}

