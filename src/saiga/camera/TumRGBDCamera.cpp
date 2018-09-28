/**
 * Copyright (c) 2017 Darius Rückert
 * Licensed under the MIT License.
 * See LICENSE file for more information.
 */

#include "TumRGBDCamera.h"
#include "saiga/util/tostring.h"
#include <fstream>

namespace Saiga {




static std::vector<TumRGBDCamera::CameraData> readCameraData(std::string file)
{
    std::vector<TumRGBDCamera::CameraData> data;
    {
        std::ifstream strm(file);
        SAIGA_ASSERT(strm.is_open());
        std::string line;
        while (std::getline(strm, line))
        {
            if(line.empty() || line[0] == '#')
                continue;
            std::vector<std::string> elements = Saiga::split(line,' ');
            SAIGA_ASSERT(elements.size() == 2);
            TumRGBDCamera::CameraData d;
            d.timestamp = Saiga::to_double(elements[0]);
            d.img = elements[1];
            data.push_back(d);
        }
    }
    return data;
}

static std::vector<TumRGBDCamera::GroundTruth> readGT(std::string file)
{
    std::vector<TumRGBDCamera::GroundTruth> data;
    {
        std::ifstream strm(file);
        SAIGA_ASSERT(strm.is_open());
        std::string line;
        while (std::getline(strm, line))
        {
            if(line.empty() || line[0] == '#')
                continue;
            std::vector<std::string> elements = Saiga::split(line,' ');
            SAIGA_ASSERT(elements.size() == 8);
            TumRGBDCamera::GroundTruth d;
            d.timeStamp = Saiga::to_double(elements[0]);

            d.position.x = Saiga::to_float(elements[1]);
            d.position.y = Saiga::to_float(elements[2]);
            d.position.z = Saiga::to_float(elements[3]);

            d.rotation.x = Saiga::to_float(elements[4]);
            d.rotation.y = Saiga::to_float(elements[5]);
            d.rotation.z = Saiga::to_float(elements[6]);
            d.rotation.w = Saiga::to_float(elements[7]);

            data.push_back(d);
        }
    }
    return data;
}



TumRGBDCamera::TumRGBDCamera(const std::string &datasetDir, double depthFactor)
    : depthFactor(depthFactor)
{
    cout << "Loading TUM RGBD Dataset: " << datasetDir << endl;
    associate(datasetDir);

    load(datasetDir);
}

std::shared_ptr<RGBDCamera::FrameData> TumRGBDCamera::waitForImage()
{
    if(currentId == (int)frames.size())
    {
        return nullptr;
    }
    auto img = frames[currentId];
    setNextFrame(*img);
    return img;
}

mat4 TumRGBDCamera::getGroundTruth(int frame)
{
    SAIGA_ASSERT(frame >= 0 && frame < (int)tumframes.size());
    GroundTruth gt = tumframes[frame].gt;
    return createTRSmatrix(vec4(gt.position,1),gt.rotation,vec4(1));
}

void TumRGBDCamera::associate(const std::string& datasetDir)
{

    std::vector<CameraData> rgbData = readCameraData(datasetDir + "/rgb.txt");
    std::vector<CameraData> depthData = readCameraData(datasetDir + "/depth.txt");
    std::vector<GroundTruth> gt = readGT(datasetDir + "/groundtruth.txt");

    // Find for each rgb image the best depth and gt
    int cdepth = 0;
    int cgt = 0;

    int lastBest = -1;
    for(auto r : rgbData)
    {
        TumFrame tf;

        auto t = r.timestamp;
        int ibest;

        {
            int ismaller = cdepth;
            int ibigger = cdepth;

            auto smaller = depthData[ismaller].timestamp;
            auto bigger = smaller;

            while(bigger < t && cdepth + 1 < (int)depthData.size())
            {
                smaller = bigger;
                ismaller = ibigger;

                cdepth++;
                ibigger = cdepth;
                bigger = depthData[ibigger].timestamp;
            }

            ibest = t - smaller < bigger - t ? ismaller : ibigger;

            tf.rgb = r;
            tf.depth = depthData[ibest];
        }

        int igtbest;

        {
            int ismaller = cgt;
            int ibigger = cgt;

            auto smaller = gt[ismaller].timeStamp;
            auto bigger = smaller;

            while(bigger < t && cgt + 1 < (int)gt.size())
            {
                smaller = bigger;
                ismaller = ibigger;

                cgt++;
                ibigger = cgt;
                bigger = gt[ibigger].timeStamp;
            }

            igtbest = t - smaller < bigger - t ? ismaller : ibigger;
        }


        tf.rgb = r;
        tf.depth = depthData[ibest];
        tf.gt = gt[igtbest];


#if 0
        {
            // Debug check
            int ibest2 = 0;
            double diffbest = 2135388888787;
            for(int i = 0; i <depthData.size(); ++i)
            {
                double diff = glm::abs(t - depthData[i].timestamp);
                if(diff < diffbest)
                {
                    ibest2 = i;
                    diffbest = diff;
                }
            }
            SAIGA_ASSERT(ibest == ibest2);
        }
#endif



        if(ibest != lastBest)
        {
            tumframes.push_back(tf);
        }
        lastBest = ibest;

    }
}

void TumRGBDCamera::load(const std::string &datasetDir)
{
    frames.resize(tumframes.size());

#pragma omp parallel for
    for(unsigned int i = 0; i < tumframes.size(); ++i)
    {
        TumFrame d = tumframes[i];
        cout << "loading " << d.rgb.img << endl;


        Image cimg(datasetDir + "/" + d.rgb.img);
        rgbo.h = cimg.h;
        rgbo.w = cimg.w;

        Image dimg(datasetDir + "/" + d.depth.img);
        deptho.h = dimg.h;
        deptho.w = dimg.w;

        auto f = makeFrameData();


        if(cimg.type == UC3)
        {
            // convert to rgba
            ImageTransformation::addAlphaChannel(cimg.getImageView<ucvec3>(),f->colorImg);
        }else if(cimg.type == UC4)
        {
            cimg.getImageView<ucvec4>().copyTo(f->colorImg.getImageView());
        }else{
            SAIGA_ASSERT(0);
        }

        if(dimg.type == US1)
        {
            dimg.getImageView<unsigned short>().copyTo(f->depthImg.getImageView(), depthFactor);
        }else{
            SAIGA_ASSERT(0);
        }
        frames[i] = f;
    }
}



}