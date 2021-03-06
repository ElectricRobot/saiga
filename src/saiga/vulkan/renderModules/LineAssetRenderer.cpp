﻿/**
 * Copyright (c) 2017 Darius Rückert
 * Licensed under the MIT License.
 * See LICENSE file for more information.
 */

#include "LineAssetRenderer.h"

#include "saiga/model/objModelLoader.h"
#include "saiga/util/easylogging++.h"
#include "saiga/vulkan/Shader/all.h"
#include "saiga/vulkan/Vertex.h"
#if defined(SAIGA_OPENGL_INCLUDED)
#    error OpenGL was included somewhere.
#endif

namespace Saiga
{
namespace Vulkan
{
void LineAssetRenderer::destroy()
{
    Pipeline::destroy();
    uniformBufferVS.destroy();
}
bool LineAssetRenderer::bind(vk::CommandBuffer cmd)
{
    cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, descriptorSet, nullptr);
    return Pipeline::bind(cmd);
}

void LineAssetRenderer::pushModel(VkCommandBuffer cmd, mat4 model)
{
    pushConstant(cmd, vk::ShaderStageFlagBits::eVertex, sizeof(mat4), &model[0][0]);
}

void LineAssetRenderer::updateUniformBuffers(vk::CommandBuffer cmd, glm::mat4 view, glm::mat4 proj)
{
    uboVS.projection = proj;
    uboVS.modelview  = view;
    uboVS.lightPos   = vec4(5, 5, 5, 0);
    uniformBufferVS.update(cmd, sizeof(UBOVS), &uboVS);
    //      cmd.updateBuffer(uniformBufferVS.m_memoryLocation.buffer,uniformBufferVS.m_memoryLocation.offset,sizeof(uboVS),&uboVS);
}

void LineAssetRenderer::init(VulkanBase& vulkanDevice, VkRenderPass renderPass, float lineWidth)
{
    PipelineBase::init(vulkanDevice, 1);
    addDescriptorSetLayout({{7, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex}});
    addPushConstantRange({vk::ShaderStageFlagBits::eVertex, 0, sizeof(mat4)});
    shaderPipeline.load(device, {"vulkan/line.vert", "vulkan/line.frag"});
    PipelineInfo info;
    info.inputAssemblyState.topology = vk::PrimitiveTopology::eLineList;
    if (base->enabledFeatures.wideLines)
    {
        info.rasterizationState.lineWidth = lineWidth;
    }
    else
    {
        if (lineWidth != 1.0f)
        {
            LOG(WARNING) << "Line width " << lineWidth << " requested, wide lines is not enabled or supported";
        }
    }
    info.addVertexInfo<VertexType>();
    create(renderPass, info);



    uniformBufferVS.init(vulkanDevice, &uboVS, sizeof(UBOVS));
    descriptorSet                           = createDescriptorSet();
    vk::DescriptorBufferInfo descriptorInfo = uniformBufferVS.getDescriptorInfo();
    device.updateDescriptorSets(
        {
            vk::WriteDescriptorSet(descriptorSet, 7, 0, 1, vk::DescriptorType::eUniformBuffer, nullptr, &descriptorInfo,
                                   nullptr),
        },
        nullptr);
}



}  // namespace Vulkan
}  // namespace Saiga
