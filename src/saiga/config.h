﻿/**
 * Copyright (c) 2017 Darius Rückert
 * Licensed under the MIT License.
 * See LICENSE file for more information.
 */

#pragma once

#include "export.h"


#ifdef _MSC_VER
//#pragma warning( disable : 4267 ) //
#    pragma warning(disable : 4244)  // 'initializing' : conversion from 'float' to 'int', possible loss of data
#    pragma warning(disable : 4005)  // 'M_PI' : macro redefinition
#    pragma warning(disable : 4800)  //'BOOL' : forcing value to bool 'true' or 'false' (performance warning)
#    pragma warning(disable : 4305)  //'argument' : truncation from 'double' to 'const btScalar'
#    pragma warning(disable : 4018)  //'<' : signed/unsigned mismatch
#    pragma warning(disable : 4251)  // needs to have dll-interface to be used by clients of class 'glbinding::Binding'
#    pragma warning(disable : 4201)  // nonstandard extension used : nameless struct/union
#    pragma warning(disable : 4267)
#    pragma warning(disable : 4505)  // Unreferenced local function has been removed


// because windows.h has stupid min/max defines
#    define NOMINMAX
// so we get std::min std::max
#    include <algorithm>
#endif

#if !defined(_MSC_VER) || _MSC_VER > 1800
#    define SAIGA_HAS_CONSTEXPR
#    define SAIGA_HAS_NOEXCEPT
#    define SAIGA_CONSTEXPR constexpr
#    define SAIGA_NOEXCEPT noexcept
#else
#    define SAIGA_CONSTEXPR
#    define SAIGA_NOEXCEPT
#endif

#if defined(_WIN32) && !defined(_WIN64)
#    error 32-bit builds are not supported.
#endif

// Unused result. We require c++17 so we can use this
#define SAIGA_WARN_UNUSED_RESULT [[nodiscard]]


#if defined(_MSC_VER)
#    define SAIGA_ALIGN(x) __declspec(align(x))
#elif defined(__GNUC__)
#    define SAIGA_ALIGN(x) __attribute__((aligned(x)))
#else
#    error Please add the correct align macro for your compiler.
#endif



// includes that are used for everything
#include <iostream>
#include <memory>
#include <string>

using std::cerr;
using std::cout;
using std::endl;



// ============== CUDA Stuff ==============

// remove all CUDA_SYNC_CHECK_ERROR and CUDA_ASSERTS
// for gcc add cppflag: -DCUDA_NDEBUG
#if !defined(CUDA_NDEBUG)
#    if !defined(CUDA_DEBUG)
#        define CUDA_DEBUG
#    endif
#else
#    undef CUDA_DEBUG
#endif


#define SAIGA_ON_HOST

#ifdef __CUDACC__
#    define HD __host__ __device__
#    define IS_CUDA
#    if defined(__CUDA_ARCH__) && __CUDA_ARCH__ > 0
#        define SAIGA_ON_DEVICE
#        undef SAIGA_ON_HOST
#    endif
#else
#    if !defined(HD)
#        define HD
#    endif
#endif

#define WARP_SIZE 32
#define L1_CACHE_LINE_SIZE 128
#define L2_CACHE_LINE_SIZE 32
#define MAX_THREADS_PER_SM 2048
