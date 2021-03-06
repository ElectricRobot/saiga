﻿/**
 * Copyright (c) 2017 Darius Rückert
 * Licensed under the MIT License.
 * See LICENSE file for more information.
 */

#pragma once

#include "saiga/image/managedImage.h"


namespace Saiga
{
SAIGA_LOCAL bool saveImageSTB(const std::string& path, Image& img);

SAIGA_LOCAL std::vector<uint8_t> compressImageSTB(Image& img);


}  // namespace Saiga
