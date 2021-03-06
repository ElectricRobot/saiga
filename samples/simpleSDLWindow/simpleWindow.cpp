﻿/**
 * Copyright (c) 2017 Darius Rückert
 * Licensed under the MIT License.
 * See LICENSE file for more information.
 */

#include "simpleWindow.h"

#include "saiga/geometry/triangle_mesh_generator.h"
#include "saiga/imgui/imgui.h"
#include "saiga/opengl/shader/shaderLoader.h"


Sample::Sample(OpenGLWindow& window, Renderer& renderer) : Updating(window), DeferredRenderingInterface(renderer)
{
    // create a perspective camera
    float aspect = window.getAspectRatio();
    camera.setProj(60.0f, aspect, 0.1f, 50.0f);
    camera.setView(vec3(0, 5, 10), vec3(0, 0, 0), vec3(0, 1, 0));
    camera.rotationPoint = vec3(0);

    // Set the camera from which view the scene is rendered
    window.setCamera(&camera);


    // This simple AssetLoader can create assets from meshes and generate some generic debug assets
    ObjAssetLoader assetLoader;

    teapot.asset = assetLoader.loadBasicAsset("teapot.obj");
    //    teapot.asset = assetLoader.loadTexturedAsset("cat.obj");
    teapot.translateGlobal(vec3(0, 1, 0));
    teapot.calculateModel();

    groundPlane.asset = assetLoader.loadDebugPlaneAsset2(vec2(20, 20), 1.0f, Colors::firebrick, Colors::gray);

    // create one directional light
    Deferred_Renderer& r = static_cast<Deferred_Renderer&>(parentRenderer);
    sun                  = r.lighting.createDirectionalLight();
    sun->setDirection(vec3(-1, -3, -2));
    sun->setColorDiffuse(LightColorPresets::DirectSunlight);
    sun->setIntensity(1.0);
    sun->setAmbientIntensity(0.3f);
    sun->createShadowMap(2048, 2048);
    sun->enableShadows();

    cout << "Program Initialized!" << endl;
}

void Sample::update(float dt)
{
    // Update the camera position
    camera.update(dt);
    sun->fitShadowToCamera(&camera);
}

void Sample::interpolate(float dt, float interpolation)
{
    // Update the camera rotation. This could also be done in 'update' but
    // doing it in the interpolate step will reduce latency
    camera.interpolate(dt, interpolation);
}

void Sample::render(Camera* cam)
{
    // Render all objects from the viewpoint of 'cam'
    groundPlane.render(cam);
    teapot.render(cam);
}

void Sample::renderDepth(Camera* cam)
{
    // Render the depth of all objects from the viewpoint of 'cam'
    // This will be called automatically for shadow casting light sources to create shadow maps
    groundPlane.renderDepth(cam);
    teapot.renderDepth(cam);
}

void Sample::renderOverlay(Camera* cam)
{
    // The skybox is rendered after lighting and before post processing
    skybox.render(cam);
}

void Sample::renderFinal(Camera* cam)
{
    // The final render path (after post processing).
    // Usually the GUI is rendered here.

    parentWindow.renderImGui();

    {
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiSetCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(400, 200), ImGuiSetCond_FirstUseEver);
        ImGui::Begin("An Imgui Window :D");

        ImGui::End();
    }
}


void Sample::keyPressed(SDL_Keysym key)
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

void Sample::keyReleased(SDL_Keysym key) {}
