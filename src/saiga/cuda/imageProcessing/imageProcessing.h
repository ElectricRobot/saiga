﻿/**
 * Copyright (c) 2017 Darius Rückert
 * Licensed under the MIT License.
 * See LICENSE file for more information.
 */

#pragma once

#include "saiga/cuda/imageProcessing/image.h"
#include "saiga/image/imageView.h"
#include "saiga/util/ArrayView.h"

namespace Saiga
{
namespace CUDA
{
// dst = src1 - src2
SAIGA_GLOBAL void subtract(ImageView<float> src1, ImageView<float> src2, ImageView<float> dst);

// subtract multiple images at the same time
// src.n - 1 == dst.n
// dst[0] = src[0] - src[1]
// dst[1] = src[1] - src[2]
//...
SAIGA_GLOBAL void subtractMulti(ImageArrayView<float> src, ImageArrayView<float> dst);


SAIGA_GLOBAL void fill(ImageView<float> img, float value);
SAIGA_GLOBAL void mult(ImageView<float> img, float value);
SAIGA_GLOBAL void add(ImageView<float> img, float value);
SAIGA_GLOBAL void abs(ImageView<float> img);


//==================== Image Scaling ======================

template <typename T>
SAIGA_GLOBAL void scaleDown2EveryOther(ImageView<T> src, ImageView<T> dst);

SAIGA_GLOBAL void scaleUp2Linear(ImageView<float> src, ImageView<float> dst);

//==================== Image Format Conversions ======================

SAIGA_GLOBAL void convertRGBtoRGBA(ImageView<uchar3> src, ImageView<uchar4> dst, unsigned char alpha = 255);
SAIGA_GLOBAL void convertRGBAtoRGB(ImageView<uchar4> src, ImageView<uchar3> dst);
SAIGA_GLOBAL void convertRGBAtoGrayscale(ImageView<uchar4> src, ImageView<float> dst);
SAIGA_GLOBAL void convertBGRtoRGBA(ImageView<uchar3> src, ImageView<uchar4> dst, unsigned char alpha = 255);
SAIGA_GLOBAL void convertRGBAtoBGR(ImageView<uchar4> src, ImageView<uchar3> dst);

//==================== Convolution ======================

#define SAIGA_MAX_CONVOLUTION_RADIUS 24
#define SAIGA_MAX_KERNEL_SIZE (SAIGA_MAX_CONVOLUTION_RADIUS * 2 + 1)

SAIGA_GLOBAL void convolveSinglePassSeparateOuterLinear(ImageView<float> src, ImageView<float> dst,
                                                        Saiga::ArrayView<float> kernel, int radius);
SAIGA_GLOBAL void convolveSinglePassSeparateOuterHalo(ImageView<float> src, ImageView<float> dst,
                                                      Saiga::ArrayView<float> kernel, int radius);
SAIGA_GLOBAL void convolveSinglePassSeparateInner(ImageView<float> src, ImageView<float> dst,
                                                  Saiga::ArrayView<float> kernel, int radius);
SAIGA_GLOBAL void convolveSinglePassSeparateInner75(ImageView<float> src, ImageView<float> dst,
                                                    Saiga::ArrayView<float> kernel, int radius);
SAIGA_GLOBAL void convolveSinglePassSeparateInnerShuffle(ImageView<float> src, ImageView<float> dst,
                                                         Saiga::ArrayView<float> kernel, int radius);

SAIGA_GLOBAL void convolveRow(ImageView<float> src, ImageView<float> dst, Saiga::ArrayView<float> kernel, int radius);
SAIGA_GLOBAL void convolveCol(ImageView<float> src, ImageView<float> dst, Saiga::ArrayView<float> kernel, int radius);

SAIGA_GLOBAL thrust::device_vector<float> createGaussianBlurKernel(int radius, float sigma);

// uploads kernel and convoles images
SAIGA_GLOBAL void applyFilterSeparate(ImageView<float> src, ImageView<float> dst, ImageView<float> tmp,
                                      ArrayView<float> kernelRow, ArrayView<float> kernelCol);
SAIGA_GLOBAL void applyFilterSeparateSinglePass(ImageView<float> src, ImageView<float> dst, ArrayView<float> kernel);


}  // namespace CUDA
}  // namespace Saiga
