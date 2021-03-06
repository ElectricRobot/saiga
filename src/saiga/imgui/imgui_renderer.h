/**
 * Copyright (c) 2017 Darius Rückert
 * Licensed under the MIT License.
 * See LICENSE file for more information.
 */

#pragma once

#include "saiga/config.h"

struct ImDrawData;

namespace Saiga
{
class SAIGA_GLOBAL ImGuiRenderer
{
   public:
    ImGuiRenderer();
    virtual ~ImGuiRenderer();


    virtual void beginFrame() = 0;
    virtual void endFrame();

    void render();

   protected:
    virtual void renderDrawLists(ImDrawData* draw_data) = 0;
};

}  // namespace Saiga
