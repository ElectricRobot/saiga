﻿/**
 * Copyright (c) 2017 Darius Rückert
 * Licensed under the MIT License.
 * See LICENSE file for more information.
 */

#include "saiga/framework/framework.h"

#include "saiga/image/image.h"
#include "saiga/model/ModelLoader.h"
#include "saiga/util/assert.h"
#include "saiga/util/directory.h"
#include "saiga/util/easylogging++.h"
#include "saiga/util/floatingPoint.h"
#include "saiga/util/ini/ini.h"
#include "saiga/util/tostring.h"

#ifdef SAIGA_USE_OPENGL
#    include "saiga/opengl/opengl.h"
#endif

#ifdef SAIGA_USE_VULKAN
#    include "saiga/vulkan/Shader/GLSL.h"
#endif

#ifdef SAIGA_USE_FREETYPE
#    include "saiga/text/fontLoader.h"
#endif

#include "saiga/util/threadName.h"

namespace Saiga
{
bool initialized = false;



bool isShaderDirectory(const std::string& dir)
{
    Directory dirbase(dir);
    Directory dirgeo(dir + "/geometry");
    return dirbase.existsFile("colored_points.glsl") && dirgeo.existsFile("deferred_mvp_texture.glsl");
}


void SaigaParameters::fromConfigFile(const std::string& file)
{
    Saiga::SimpleIni ini;
    ini.LoadFile(file.c_str());


    std::string comment =
        "# Multiple search pathes must be separated by '!'.\n"
        "# Example:\n"
        "# shaderDirectory = shader!/usr/local/share/saiga/shader!somepath/asdf/shader";

    char sep = '!';
    shaderDirectory =
        split(ini.GetAddString("saiga", "shaderDirectory", concat(shaderDirectory, sep).c_str(), comment.c_str()), sep);
    textureDirectory = split(ini.GetAddString("saiga", "textureDirectory", concat(textureDirectory, sep).c_str()), sep);
    modelDirectory   = split(ini.GetAddString("saiga", "modelDirectory", concat(modelDirectory, sep).c_str()), sep);
    fontDirectory    = split(ini.GetAddString("saiga", "fontDirectory", concat(fontDirectory, sep).c_str()), sep);
    mainThreadName   = ini.GetAddString("saiga", "mainThreadName", mainThreadName.c_str());
    logging_enabled  = ini.GetAddBool("saiga", "logging", logging_enabled);
    if (ini.changed()) ini.SaveFile(file.c_str());
}



static void printSaigaInfo()
{
    cout << "Saiga Version " << SAIGA_VERSION_MAJOR << "." << SAIGA_VERSION_MINOR << endl;
    std::string libs;
#ifdef SAIGA_USE_SDL
    libs += "SDL,";
#endif
#ifdef SAIGA_USE_GLFW
    libs += "GLFW,";
#endif
#ifdef SAIGA_USE_OPENAL
    libs += "OPENAL,";
#endif
#ifdef SAIGA_USE_ALUT
    libs += "ALUT,";
#endif
#ifdef SAIGA_USE_OPUS
    libs += "OPUS,";
#endif
#ifdef SAIGA_USE_ASSIMP
    libs += "ASSIMP,";
#endif
#ifdef SAIGA_USE_PNG
    libs += "PNG,";
#endif
#ifdef SAIGA_USE_FREEIMAGE
    libs += "FREEIMAGE,";
#endif
#ifdef SAIGA_USE_FFMPEG
    libs += "FFMPEG,";
#endif
#ifdef SAIGA_USE_CUDA
    libs += "CUDA,";
#endif
#ifdef SAIGA_USE_EIGEN
    libs += "EIGEN,";
#endif
    cout << "Libs: " << libs << endl;

    std::string options;
#ifdef SAIGA_BUILD_SHARED
    options += "BUILD_SHARED,";
#endif
#ifdef SAIGA_DEBUG
    options += "DEBUG,";
#endif
#ifdef SAIGA_ASSERTS
    options += "ASSERTS,";
#endif
#ifdef SAIGA_BUILD_SAMPLES
    options += "BUILD_SAMPLES,";
#endif
#ifdef SAIGA_WITH_CUDA
    options += "WITH_CUDA,";
#endif
#ifdef SAIGA_STRICT_FP
    options += "STRICT_FP,";
#endif
#ifdef SAIGA_FULL_OPTIMIZE
    options += "FULL_OPTIMIZE,";
#endif
#ifdef SAIGA_CUDA_DEBUG
    options += "CUDA_DEBUG,";
#endif
    cout << "Build Options: " << options << endl;
}


void initSample(SaigaParameters& saigaParameters)
{
    saigaParameters.shaderDirectory  = {SAIGA_PROJECT_SOURCE_DIR "/shader"};
    saigaParameters.textureDirectory = {SAIGA_PROJECT_SOURCE_DIR "/data/textures"};
    saigaParameters.modelDirectory   = {SAIGA_PROJECT_SOURCE_DIR "/data/models"};
    saigaParameters.fontDirectory    = {SAIGA_PROJECT_SOURCE_DIR "/data/fonts"};
    saigaParameters.logging_enabled  = false;
}

void initSaiga(const SaigaParameters& params)
{
    if (initialized)
    {
        return;
    }

    FP::resetSSECSR();


    std::vector<std::string> searchPathes = {
        // First check in the local working directory
        "shader",
    };

    searchPathes.insert(searchPathes.end(), params.shaderDirectory.begin(), params.shaderDirectory.end());
    // Then the given paramter from the config file
    //        params.shaderDirectory,
    // And last the install prefix from cmake
    searchPathes.push_back(SAIGA_INSTALL_PREFIX "/share/saiga/shader");
    searchPathes.push_back(SAIGA_SHADER_PREFIX);
    //    };


    std::string shaderDir;

    for (auto str : searchPathes)
    {
        if (isShaderDirectory(str))
        {
            shaderDir = str;
            cout << "Found the Saiga shaders at " << shaderDir << endl;
            break;
        }
    }

    if (shaderDir.empty())
    {
        cout << "Could not find the Saiga shaders." << endl;
        cout << "Set the 'shaderDirectory' variable of 'SaigaParameters' accordingly." << endl;
        exit(1);
    }

#ifdef SAIGA_USE_OPENGL
    initSaigaGL(shaderDir, params.textureDirectory);
#endif

#ifdef SAIGA_USE_VULKAN
    Vulkan::GLSLANG::init();
    Vulkan::GLSLANG::shaderPathes.addSearchPath(shaderDir);
    Vulkan::GLSLANG::shaderPathes.addSearchPath(shaderDir + "/include");
#endif

    for (auto t : params.textureDirectory) Image::searchPathes.addSearchPath(t);

    for (auto t : params.modelDirectory) modelPathes.addSearchPath(t);

#if defined(SAIGA_USE_FREETYPE) && defined(SAIGA_USE_OPENGL)
    for (auto t : params.fontDirectory) fontPathes.addSearchPath(t);
#endif


    setThreadName(params.mainThreadName);

    el::Configurations defaultConf;
    defaultConf.setToDefault();
    // Values are always std::string
    defaultConf.set(el::Level::Global, el::ConfigurationType::ToFile, "false");
    defaultConf.set(el::Level::Global, el::ConfigurationType::ToStandardOutput, std::to_string(params.logging_enabled));
    el::Loggers::reconfigureLogger("default", defaultConf);

    if (params.logging_enabled)
    {
        cout << "Logging is enabled" << endl;
    }

    printSaigaInfo();
    cout << "========================== Saiga initialization done!  ==========================" << endl;
    initialized = true;
}

void cleanupSaiga()
{
#ifdef SAIGA_USE_VULKAN
    Saiga::Vulkan::GLSLANG::quit();
#endif

#ifdef SAIGA_USE_OPENGL
    cleanupSaigaGL();
#endif

    cout << "========================== Saiga cleanup done! ==========================" << endl;
    initialized = false;
}



}  // namespace Saiga
