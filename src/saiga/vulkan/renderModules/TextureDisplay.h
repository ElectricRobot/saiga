﻿/**
 * Copyright (c) 2017 Darius Rückert
 * Licensed under the MIT License.
 * See LICENSE file for more information.
 */


#pragma once

#include "saiga/geometry/triangle_mesh.h"
#include "saiga/vulkan/Base.h"
#include "saiga/vulkan/VulkanAsset.h"
#include "saiga/vulkan/VulkanBuffer.hpp"
#include "saiga/vulkan/buffer/UniformBuffer.h"
#include "saiga/vulkan/pipeline/Pipeline.h"
#include "saiga/vulkan/svulkan.h"
#include "saiga/vulkan/texture/Texture.h"

namespace Saiga
{
namespace Vulkan
{
class SAIGA_GLOBAL TextureDisplay : public Pipeline
{
   public:
    using VertexType = VertexNC;


    ~TextureDisplay() { destroy(); }
    void destroy();


    /**
     * Render the texture at the given pixel position and size
     */
    void renderTexture(vk::CommandBuffer cmd, vk::DescriptorSet texture, vec2 position, vec2 size);



    void init(Saiga::Vulkan::VulkanBase& vulkanDevice, VkRenderPass renderPass);


    vk::DescriptorSet createAndUpdateDescriptorSet(Texture& texture);

   private:
    Saiga::Vulkan::VulkanVertexColoredAsset blitMesh;
};



}  // namespace Vulkan
}  // namespace Saiga
