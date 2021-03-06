﻿/**
 * Copyright (c) 2017 Darius Rückert
 * Licensed under the MIT License.
 * See LICENSE file for more information.
 */

#include "Instance.h"

//#include "saiga/vulkan/VulkanTools.h"

namespace Saiga
{
namespace Vulkan
{
void Instance::destroy()
{
    debug.destroy();
    vkDestroyInstance(instance, nullptr);
}

void Instance::create(std::vector<const char*> instanceExtensions, bool enableValidation)
{
    VkApplicationInfo appInfo = {};
    appInfo.sType             = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName  = "Saiga Application";
    appInfo.pEngineName       = "Saiga";
    appInfo.apiVersion        = VK_API_VERSION_1_1;

    //    std::vector<const char*> instanceExtensions = getRequiredInstanceExtensions();

    //    instanceExtensions.push_back( VK_KHR_SURFACE_EXTENSION_NAME );



    VkInstanceCreateInfo instanceCreateInfo = {};
    instanceCreateInfo.sType                = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pNext                = NULL;
    instanceCreateInfo.pApplicationInfo     = &appInfo;
    if (instanceExtensions.size() > 0)
    {
        if (enableValidation)
        {
            instanceExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
        }
        instanceCreateInfo.enabledExtensionCount   = (uint32_t)instanceExtensions.size();
        instanceCreateInfo.ppEnabledExtensionNames = instanceExtensions.data();
    }
    auto layers = Debug::getDebugValidationLayers();
    if (enableValidation)
    {
        // TODO
        instanceCreateInfo.enabledLayerCount   = layers.size();
        instanceCreateInfo.ppEnabledLayerNames = layers.data();
    }

#if 0
    cout << "Instance extensions:" << endl;
    for (auto ex : instanceExtensions) cout << ex << endl;
#endif


    //   VK_CHECK_RESULT(vkCreateInstance(&instanceCreateInfo, nullptr, &instance));
    instance = vk::createInstance(instanceCreateInfo);
    SAIGA_ASSERT(instance);

#if 0
    auto extprops = vk::enumerateInstanceExtensionProperties();
    for (auto e : extprops)
    {
        cout << e.specVersion << " " << e.extensionName << endl;
    }
#endif


    // If requested, we enable the default validation layers for debugging
    if (enableValidation)
    {
        debug.init(instance);
    }

    cout << "Vulkan instance created." << endl;
}

vk::PhysicalDevice Instance::pickPhysicalDevice()
{
    std::vector<vk::PhysicalDevice> physicalDevices = instance.enumeratePhysicalDevices();
    if (physicalDevices.size() == 0)
    {
        SAIGA_EXIT_ERROR("Could not find a vulkan capable device.");
    }

    uint32_t selectedDevice = 0;
    auto physicalDevice     = physicalDevices[selectedDevice];

    {
        vk::PhysicalDeviceProperties deviceProperties = physicalDevice.getProperties();

        std::cout << "Selected Vulkan Device [" << selectedDevice << "] : " << deviceProperties.deviceName << ", ";
        std::cout << " API: " << (deviceProperties.apiVersion >> 22) << "."
                  << ((deviceProperties.apiVersion >> 12) & 0x3ff) << "." << (deviceProperties.apiVersion & 0xfff)
                  << std::endl;
    }
    return physicalDevice;
}

}  // namespace Vulkan
}  // namespace Saiga
