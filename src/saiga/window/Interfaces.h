﻿/**
 * Copyright (c) 2017 Darius Rückert
 * Licensed under the MIT License.
 * See LICENSE file for more information.
 */

#pragma once

#include "saiga/config.h"



namespace Saiga
{
class Camera;
class WindowBase;
class RenderingInterfaceBase;
/**
 * Base class of all render engines.
 * This includes the deferred and forward OpenGL engines
 * as well as the Vulkan renderers.
 *
 * Each renderer needs a fitting rendering interface.
 * For example, the deferrred renderer has the DeferredRenderingInterface.
 * An application can now derive from DeferredRenderingInterface to use
 * the deferred rendering engine.
 */
class SAIGA_GLOBAL RendererBase
{
   public:
    virtual ~RendererBase() {}
    RenderingInterfaceBase* rendering = nullptr;


    virtual void printTimings() {}
    void setRenderObject(RenderingInterfaceBase& r) { rendering = &r; }

    virtual void renderImGui(bool* p_open = nullptr) {}
    virtual float getTotalRenderTime() { return 0; }

    virtual void resize(int windowWidth, int windowHeight) {}
    virtual void render(Camera* cam)     = 0;
    virtual void bindCamera(Camera* cam) = 0;
};

/**
 * Base class for all rendering interfaces.
 */
class SAIGA_GLOBAL RenderingInterfaceBase
{
   public:
    RenderingInterfaceBase(RendererBase& parent);
    virtual ~RenderingInterfaceBase() {}


   protected:
    RendererBase& parentRenderer;
};

/**
 * Base class for applications to make them updateable.
 * The mainloop will call the appropriate functions.
 */
class SAIGA_GLOBAL Updating
{
   public:
    Updating(WindowBase& parent);

    virtual ~Updating() {}

    // advances the state of the program by dt. All game logic should happen here
    virtual void update(float dt) {}

    virtual void parallelUpdate(float dt) { (void)dt; }

    // interpolation between two logic steps for high fps rendering.
    // Example:
    // Game loop: constant 60 Hz
    // Render rate: around 120 Hz
    //-> The game is rendered two times per update
    //
    // We don't want to render two times the same image, so the game state should be interpolated either into the future
    // or from the past. Alpha is in the range [0,1] where 1 is equivalent to a timestep of dt
    virtual void interpolate(float dt, float alpha) {}

   protected:
    WindowBase& parentWindow;
};

}  // namespace Saiga
