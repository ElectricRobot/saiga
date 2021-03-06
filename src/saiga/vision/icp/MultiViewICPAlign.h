/**
 * Copyright (c) 2017 Darius Rückert
 * Licensed under the MIT License.
 * See LICENSE file for more information.
 */

#pragma once

#include "saiga/vision/Depthmap.h"
#include "saiga/vision/icp/ICPDepthMap.h"

#include <vector>

namespace Saiga
{
namespace ICP
{
SAIGA_GLOBAL void multiViewICPAlign(size_t N, const std::vector<std::pair<size_t, size_t>>& pairs,
                                    const std::vector<AlignedVector<Correspondence>>& corrs, std::vector<SE3>& guesses,
                                    int iterations);


}  // namespace ICP
}  // namespace Saiga
