﻿/**
 * Copyright (c) 2017 Darius Rückert
 * Licensed under the MIT License.
 * See LICENSE file for more information.
 */


#pragma once

#include "saiga/vulkan/window/Window.h"

#ifndef SAIGA_USE_GLFW
#    error Saiga was compiled without GLFW.
#endif

struct GLFWwindow;
struct GLFWcursor;

namespace Saiga
{
namespace Vulkan
{
class SAIGA_GLOBAL GLFWWindow : public VulkanWindow
{
   public:
    GLFWwindow* window = nullptr;

    GLFWWindow(WindowParameters _windowParameters);
    ~GLFWWindow();

    virtual std::shared_ptr<ImGuiVulkanRenderer> createImGui(size_t frameCount) override;

    std::vector<const char*> getRequiredInstanceExtensions() override;
    void createSurface(VkInstance instance, VkSurfaceKHR* surface) override;
    virtual void update(float dt) override;

   private:
    void create();
};


}  // namespace Vulkan
}  // namespace Saiga
