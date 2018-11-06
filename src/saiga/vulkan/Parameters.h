﻿/**
 * Copyright (c) 2017 Darius Rückert
 * Licensed under the MIT License.
 * See LICENSE file for more information.
 */


#pragma once

#include "saiga/vulkan/svulkan.h"

namespace Saiga {
namespace Vulkan {




struct SAIGA_GLOBAL VulkanParameters
{

    vk::PhysicalDeviceFeatures physicalDeviceFeatures;
    std::vector<const char*> deviceExtensions;
    bool enableValidationLayer    = true;
    bool enableImgui = true;

    uint32_t maxDescriptorSets = 4096;
    // for {uniformBuffer,texture}
    std::array<uint32_t,4> descriptorCounts = { 1024, 1024, 1024, 1024 };
};


}
}
