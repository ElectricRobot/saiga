/**
 * Copyright (c) 2017 Darius Rückert
 * Licensed under the MIT License.
 * See LICENSE file for more information.
 */

#pragma once

#include "saiga/assets/all.h"
#include "saiga/assets/objAssetLoader.h"
#include "saiga/rendering/forwardRendering/forwardRendering.h"
#include "saiga/rendering/overlay/deferredDebugOverlay.h"
#include "saiga/rendering/overlay/textDebugOverlay.h"
#include "saiga/rendering/renderer.h"
#include "saiga/sdl/sdl_camera.h"
#include "saiga/sdl/sdl_eventhandler.h"
#include "saiga/sdl/sdl_window.h"
#include "saiga/text/all.h"
#include "saiga/world/proceduralSkybox.h"

using namespace Saiga;

class Sample : public Updating, public ForwardRenderingInterface, public SDL_KeyListener
{
   public:
    SDLCamera<PerspectiveCamera> camera;

    Timer timer;
    TextDebugOverlay tdo;
    TextureAtlas textAtlas;

    bool running = true;
    Sample(OpenGLWindow& window, Renderer& renderer);
    ~Sample();

    void update(float dt) override;
    void interpolate(float dt, float interpolation) override;

    void renderOverlay(Camera* cam) override;
    void renderFinal(Camera* cam) override;

    void keyPressed(SDL_Keysym key) override;
    void keyReleased(SDL_Keysym key) override;
};
