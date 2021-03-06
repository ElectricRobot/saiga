﻿/*
 * Vulkan Example - imGui (https://github.com/ocornut/imgui)
 *
 * Copyright (C) 2017 by Sascha Willems - www.saschawillems.de
 *
 * This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
 */

#include "sample.h"

#include "saiga/image/imageTransformations.h"
#include "saiga/imgui/imgui.h"
#include "saiga/network/RGBDCameraNetwork.h"
#include "saiga/openni2/RGBDCameraInput.h"
#include "saiga/util/color.h"
#include "saiga/util/threadName.h"

#if defined(SAIGA_OPENGL_INCLUDED)
#    error OpenGL was included somewhere.
#endif

VulkanExample::VulkanExample(Saiga::Vulkan::VulkanWindow& window, Saiga::Vulkan::VulkanForwardRenderer& renderer)
    : Updating(window), Saiga::Vulkan::VulkanForwardRenderingInterface(renderer), renderer(renderer)
{
#if 0
    std::string file = "server.ini";
    Saiga::SimpleIni ini;
    ini.LoadFile(file.c_str());
    auto ip         = ini.GetAddString ("server","ip","10.0.0.2");
    auto port        = ini.GetAddLong ("server","port",9000);
    if(ini.changed()) ini.SaveFile(file.c_str());
    auto cam = std::make_shared<Saiga::RGBDCameraNetwork>();
    cam->connect(ip,port);
    rgbdcamera = cam;
#elif 1
    Saiga::RGBDCameraInput::CameraOptions co1, co2;
    co2.h = 240;
    co2.w = 320;

    auto cam   = std::make_shared<Saiga::RGBDCameraInput>(co1, co2);
    rgbdcamera = cam;

    cam->autoexposure     = false;
    cam->autoWhiteBalance = false;
    cam->exposure         = 50;
    cam->gain             = 300;
    cam->updateCameraSettings();
#else
    auto cam = std::make_shared<Saiga::TumRGBDCamera>(
        "/home/dari/Programming/ORB_SLAM2/datasets/rgbd_dataset_freiburg3_long_office_household");
    rgbdcamera = cam;
#endif


    Saiga::setThreadName("main");

    cout << "init done" << endl;
}

VulkanExample::~VulkanExample() {}

void VulkanExample::init(Saiga::Vulkan::VulkanBase& base)
{
    textureDisplay.init(base, renderer.renderPass);



    frameData = rgbdcamera->waitForImage();


    rgbImage.create(frameData->colorImg.h, frameData->colorImg.w);
    //    Saiga::ImageTransformation::addAlphaChannel(frameData->colorImg.getImageView(),rgbImage.getImageView());

    texture = std::make_shared<Saiga::Vulkan::Texture2D>();
    texture->fromImage(renderer.base, rgbImage);


    texture2 = std::make_shared<Saiga::Vulkan::Texture2D>();
    //    Saiga::TemplatedImage<ucvec4> depthmg(frameData->depthImg.height,frameData->depthImg.width);
    depthmg.create(frameData->depthImg.height, frameData->depthImg.width);
    Saiga::ImageTransformation::depthToRGBA(frameData->depthImg.getImageView(), depthmg.getImageView(), 0, 7000);
    texture2->fromImage(renderer.base, depthmg);



    textureDes  = textureDisplay.createAndUpdateDescriptorSet(*texture);
    textureDes2 = textureDisplay.createAndUpdateDescriptorSet(*texture2);

    cout << "init done " << endl;
}



void VulkanExample::update(float dt) {}

void VulkanExample::transfer(vk::CommandBuffer cmd)
{
    std::shared_ptr<Saiga::RGBDCameraInput> cam = std::dynamic_pointer_cast<Saiga::RGBDCameraInput>(rgbdcamera);
    cam->updateCameraSettings();

    //    rgbdcamera->readFrame(*frameData);
    auto newFrameData = rgbdcamera->tryGetImage();



    if (newFrameData)
    {
        frameData = newFrameData;
        //        Saiga::ImageTransformation::addAlphaChannel(frameData->colorImg.getImageView(),rgbImage.getImageView());

        // texture->uploadImage(renderer.base,frameData->colorImg);
        //        texture->uploadImage(renderer.base,rgbImage);


        Saiga::ImageTransformation::depthToRGBA(frameData->depthImg, depthmg, 0, 8);
        // texture2->uploadImage(renderer.base,depthmg);
    }
}


void VulkanExample::render(vk::CommandBuffer cmd)
{
    if (textureDisplay.bind(cmd))
    {
        textureDisplay.renderTexture(cmd, textureDes, vec2(0, 0),
                                     vec2(frameData->colorImg.width, frameData->colorImg.height));
        textureDisplay.renderTexture(cmd, textureDes2, vec2(frameData->colorImg.width, 0),
                                     vec2(frameData->depthImg.width, frameData->depthImg.height));
    }
}

void VulkanExample::renderGUI()
{
    parentWindow.renderImGui();


    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiSetCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiSetCond_FirstUseEver);
    ImGui::Begin("Saiga OpenNI");


    std::shared_ptr<Saiga::RGBDCameraInput> cam = std::dynamic_pointer_cast<Saiga::RGBDCameraInput>(rgbdcamera);
    cam->updateCameraSettings();


    ImGui::Checkbox("autoexposure", &cam->autoexposure);
    ImGui::Checkbox("autoWhiteBalance", &cam->autoWhiteBalance);

    ImGui::InputInt("exposure", &cam->exposure);
    ImGui::InputInt("gain", &cam->gain);

    ImGui::End();
}


void VulkanExample::keyPressed(SDL_Keysym key)
{
    switch (key.scancode)
    {
        case SDL_SCANCODE_ESCAPE:
            parentWindow.close();
            break;
        default:
            break;
    }
}

void VulkanExample::keyReleased(SDL_Keysym key) {}
