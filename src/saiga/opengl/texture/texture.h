/**
 * Copyright (c) 2017 Darius Rückert
 * Licensed under the MIT License.
 * See LICENSE file for more information.
 */

#pragma once

#include "saiga/opengl/texture/raw_texture.h"

namespace Saiga
{
class SAIGA_GLOBAL basic_Texture_2D : public raw_Texture
{
   public:
    basic_Texture_2D() : raw_Texture(GL_TEXTURE_2D) {}

    virtual ~basic_Texture_2D() {}

    void setDefaultParameters() override;
    bool fromImage(const Image& img, bool srgb, bool flipY);
};

typedef basic_Texture_2D Texture;

class SAIGA_GLOBAL multisampled_Texture_2D : public raw_Texture
{
   public:
    int samples = 4;

    multisampled_Texture_2D(int samples) : raw_Texture(GL_TEXTURE_2D_MULTISAMPLE), samples(samples) {}
    virtual ~multisampled_Texture_2D() {}

    void setDefaultParameters() override;
    void uploadData(const void* data) override;
};

}  // namespace Saiga
