﻿/*
* Vulkan Example - imGui (https://github.com/ocornut/imgui)
*
* Copyright (C) 2017 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#include "compute.h"
#include <saiga/imgui/imgui.h>
#include "saiga/util/color.h"

#if defined(SAIGA_OPENGL_INCLUDED)
#error OpenGL was included somewhere.
#endif

Compute::Compute(Saiga::Vulkan::VulkanWindow &window, Saiga::Vulkan::VulkanForwardRenderer &renderer)
    :  Updating(window), Saiga::Vulkan::VulkanForwardRenderingInterface(renderer), renderer(renderer)
{
    float aspect = window.getAspectRatio();
    camera.setProj(60.0f,aspect,0.1f,50.0f,true);
    camera.setView(vec3(0,5,10),vec3(0,0,0),vec3(0,1,0));
    camera.rotationPoint = vec3(0);

    window.setCamera(&camera);
}

Compute::~Compute()
{
    teapot.destroy();
    plane.destroy();
    assetRenderer.destroy();
    compute.storageBuffer.destroy();
    computePipeline.destroy();
    compute.commandPool.destroy();
}

void Compute::init(Saiga::Vulkan::VulkanBase &base)
{
    vulkanDevice = &renderer.base;
    device = vulkanDevice->device;

    // create storage buffer
    compute.data.resize(10,1);
    compute.storageBuffer.createBuffer(renderer.base,sizeof(int)*compute.data.size(),vk::BufferUsageFlagBits::eStorageBuffer);
    compute.storageBuffer.allocateMemoryBuffer(renderer.base,vk::MemoryPropertyFlagBits::eHostVisible|vk::MemoryPropertyFlagBits::eHostCoherent);
    compute.storageBuffer.mappedUpload(0,sizeof(int)*compute.data.size(),compute.data.data());


    compute.queue = device.getQueue(vulkanDevice->queueFamilyIndices.compute,0);


    computePipeline.init(base,1);
    computePipeline.addDescriptorSetLayout({{ 0,vk::DescriptorType::eStorageBuffer,1,vk::ShaderStageFlagBits::eCompute }});
    computePipeline.shader.load(device,"vulkan/test.comp");
    computePipeline.create();



    descriptorSet = computePipeline.createDescriptorSet();

    vk::DescriptorBufferInfo descriptorInfo = compute.storageBuffer.createInfo();
    device.updateDescriptorSets({
                                    vk::WriteDescriptorSet(descriptorSet,0,0,1,vk::DescriptorType::eStorageBuffer,nullptr,&descriptorInfo,nullptr),
                                },nullptr);


    compute.commandPool.create(device,vulkanDevice->queueFamilyIndices.compute);
    compute.commandBuffer = compute.commandPool.allocateCommandBuffer();


    {
        // Build the command buffer
        compute.commandBuffer.begin( vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit) );
        computePipeline.bind(compute.commandBuffer);
        computePipeline.bindDescriptorSets(compute.commandBuffer,descriptorSet);
        // Dispatch 1 block
        compute.commandBuffer.dispatch(1,1,1);
        compute.commandBuffer.end();
    }


    vulkanDevice->submitAndWait(compute.commandBuffer,compute.queue);

    compute.storageBuffer.mappedDownload(0,sizeof(int)*compute.data.size(),compute.data.data());

    for(int i : compute.data)
        cout << i << endl;
}



void Compute::update(float dt)
{
    camera.update(dt);
    camera.interpolate(dt,0);
}


void Compute::render(VkCommandBuffer cmd)
{
}

void Compute::renderGUI()
{
}

void Compute::keyPressed(SDL_Keysym key)
{
    switch(key.scancode){
    case SDL_SCANCODE_ESCAPE:
        parentWindow.close();
        break;
    default:
        break;
    }
}

void Compute::keyReleased(SDL_Keysym key)
{
}


