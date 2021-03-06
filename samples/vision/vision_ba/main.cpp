﻿#include "saiga/framework/framework.h"
#include "saiga/util/easylogging++.h"
#include "saiga/vulkan/window/SDLWindow.h"

#include "sample.h"

int main(const int argc, const char* argv[])
{
    using namespace Saiga;

    {
        Saiga::WindowParameters windowParameters;
        Saiga::initSample(windowParameters.saigaParameters);
        windowParameters.fromConfigFile("config.ini");

        Saiga::Vulkan::SDLWindow window(windowParameters);
        //        Saiga::Vulkan::GLFWWindow window(windowParameters);

        Saiga::Vulkan::VulkanParameters vulkanParams;
        vulkanParams.enableValidationLayer = true;
        vulkanParams.fromConfigFile("config.ini");
        Saiga::Vulkan::VulkanForwardRenderer renderer(window, vulkanParams);



        VulkanExample example(window, renderer);
        renderer.initChildren();

        MainLoopParameters params;
        params.fromConfigFile("config.ini");
        window.startMainLoop(params);

        renderer.waitIdle();
    }

    return 0;
}
