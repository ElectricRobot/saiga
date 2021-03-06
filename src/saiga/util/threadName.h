
/**
 * Copyright (c) 2017 Darius Rückert
 * Licensed under the MIT License.
 * See LICENSE file for more information.
 */

#pragma once

#include "saiga/config.h"

#include <thread>

namespace Saiga
{
/**
 * Sets a thread name for debugging purposes.
 *
 * Basically copy+paste from here: https://stackoverflow.com/questions/10121560/stdthread-naming-your-thread
 * Slight modifications on the interface.
 */
SAIGA_GLOBAL extern void setThreadName(const std::string& name);
SAIGA_GLOBAL extern void setThreadName(std::thread& thread, const std::string& name);

}  // namespace Saiga
