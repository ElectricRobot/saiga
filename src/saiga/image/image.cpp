﻿/**
 * Copyright (c) 2017 Darius Rückert 
 * Licensed under the MIT License.
 * See LICENSE file for more information.
 */

#include "saiga/image/image.h"
#include "saiga/util/assert.h"
#include <cstring>
#ifdef SAIGA_USE_FREEIMAGE
#include <FreeImagePlus.h>
#include "saiga/image/freeimage.h"
#endif
#include "saiga/image/png_wrapper.h"
#include "saiga/image/imageConverter.h"
#include "saiga/util/imath.h"
#include "saiga/image/templatedImage.h"

namespace Saiga {

Image::Image(ImageFormat format, int w, int h, void* _data) : width(w), height(h), format(format)
{
//    pitch = width * format.bytesPerPixel();
//    pitch = iAlignUp(pitch,rowAlignment);
//    data.resize(getSize());
//    if(_data){
//        memcpy(getRawData(),_data,getSize());
//    }
    create(format,w,h,_data);
}

Image::Image(ImageFormat format, int w, int h, int p, void* _data) : width(w), height(h), format(format){
//    pitch = p;
//    data.resize(getSize());
//    if(_data){
//        memcpy(getRawData(),_data,getSize());
//    }
    create(format,w,h,p,_data);
}



int Image::getBytesPerRow() const
{
    return pitch;
}

void Image::free()
{
    data.resize(0);
    data.shrink_to_fit();
//    data.swap( std::vector<byte_t>() );
}

Image::byte_t *Image::getRawData()
{
    return data.data();
}

const Image::byte_t *Image::getRawData() const
{
    return data.data();
}

int Image::position(int x, int y){
    return y*pitch+x*format.bytesPerPixel();
}

Image::byte_t *Image::positionPtr(int x, int y){
    //    return &(this->data[position(x,y)]);
    return getRawData() + position(x,y);
}


void Image::setPixel(int x, int y, void* data){
    std::memcpy(positionPtr(x,y),data,format.bytesPerPixel());
}

void Image::setPixel(int x, int y, uint8_t data){
    *(uint8_t*)positionPtr(x,y) = data;
}

void Image::setPixel(int x, int y, uint16_t data){
    *(uint16_t*)positionPtr(x,y) = data;
}

void Image::setPixel(int x, int y, uint32_t data){
    *(uint32_t*)positionPtr(x,y) = data;
}

void Image::setPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b){
    uint8_t* ptr = positionPtr(x,y);
    ptr[0] = r;
    ptr[1] = g;
    ptr[2] = b;
}



void Image::makeZero()
{
    std::fill(data.begin(), data.end(), 0);
}

void Image::create(void *data)
{
    create(format,width,height,data);
}

void Image::create(ImageFormat format, int w, int h, void *data)
{
    auto pitch = w * format.bytesPerPixel();
    pitch = iAlignUp(pitch,rowAlignment);

    create(format,w,h,pitch,data);
}

void Image::create(ImageFormat _format, int w, int h, int p, void *_data)
{
    format = _format;
    pitch = p;
    width = w;
    height = h;

    data.resize(getSize());

    if(_data)
    {
        memcpy(getRawData(),_data,getSize());
    }
}

void Image::resize(int w, int h)
{
    width = w;
    height = h;
    create();
}

void Image::resizeCopy(int w, int h)
{
    Image oldimg = *this;
    width = w;
    height = h;
    create();
    makeZero();
    setSubImage(0,0,oldimg);
}

void Image::setSubImage(int x, int y, Image& src)
{
    SAIGA_ASSERT(src.width<=width && src.height<=height);
    //copy row by row
    for(int i = 0; i < src.height; i++){
        memcpy(this->positionPtr(x,y+i),src.getRawData()+src.pitch*i,src.pitch);
    }
}

void Image::setSubImage(int x, int y, int w, int h, uint8_t *data)
{
    int rowsize = format.bytesPerPixel()*w;
    //copy row by row
    for(int i=0;i<h;i++){
        memcpy(this->positionPtr(x,y+i),data+rowsize*i,rowsize);
    }
}

void Image::getSubImage(int x, int y, int w, int h, Image &out){
    out.width = w;
    out.height = h;
    out.format = format;

    out.create();

    int rowsize = format.bytesPerPixel()*w;

    //copy row by row
    for(int i=0;i<h;i++){//rows
        memcpy(out.getRawData()+rowsize*i,positionPtr(x,y+i),rowsize);
    }


}



void Image::flipRB()
{
    SAIGA_ASSERT(format.getBitDepth()==8 || format.getBitDepth()==16);
    SAIGA_ASSERT(format.getChannels()==3 || format.getChannels()==4);

    if(format.getBitDepth() == 8){
        for(int y = 0 ; y < (int)height ; ++y){
            uint8_t* ptr = getRawData() + (y*pitch);
            for(int x = 0 ; x < (int)width ; ++x){
                uint8_t r = *ptr;
                *ptr = *(ptr+2);
                *(ptr+2) = r;
                ptr += format.getChannels();
            }
        }
    }else if(format.getBitDepth() == 16){
        for(int y = 0 ; y < (int)height ; ++y){
            uint16_t* ptr = (uint16_t*)(getRawData() + (y*pitch));
            for(int x = 0 ; x < (int)width ; ++x){
                uint16_t r = *ptr;
                *ptr = *(ptr+2);
                *(ptr+2) = r;
                ptr += format.getChannels();
            }
        }
    }
}

void Image::flipY()
{
    auto copy = data;
    for(int y = 0 ; y < height ; ++y){
        int offset = pitch * y;
        int invOffset = pitch * (height-y-1);
        std::copy(copy.begin()+offset,
                  copy.begin()+offset+pitch,
                  data.begin()+invOffset
                  );
    }
}

void Image::to8bitImage()
{
    if(format.getBitDepth() == 8)
        return;

    Image old = *this;

    if(format.getElementFormat() == ImageElementFormat::FloatingPoint && format.getChannels() == 1)
    {
        cout << "to8bitImage float" << endl;

        ImageView<float> f = old.getImageView<float>();
        f.normalize();

        format.setElementFormat(ImageElementFormat::UnsignedNormalized);
        format.setBitDepth(8);
        create();

        ImageView<unsigned char> dst = this->getImageView<unsigned char>();

        for(int y = 0 ; y < height; ++y){
            for(int x = 0 ; x < width; ++x){
                dst(y,x) = int(f(y,x) * 255.f);
            }
        }
        return;
    }



    if(format.getBitDepth() != 16)
        return;



    SAIGA_ASSERT(format.getElementFormat() == ImageElementFormat::UnsignedNormalized);
    SAIGA_ASSERT(format.getBitDepth() == 16);



    format.setBitDepth(8);
    create();

    for(int y = 0 ; y < height; ++y){
        uint16_t* srcPtr = reinterpret_cast<uint16_t*>(old.positionPtr(0,y));
        uint8_t* targetPtr = reinterpret_cast<uint8_t*>(positionPtr(0,y));
        for(int x = 0 ; x < width * format.getChannels(); ++x){
            targetPtr[x] = int(double(srcPtr[x]) / (65535.0) * 255.0);
        }

    }



}

void Image::removeAlpha()
{
    if(format.getChannels() != 4)
        return;

    Image old = *this;

    format.setChannels(3);
    create();

    for(int y = 0 ; y < height; ++y){
        uint8_t* srcPtr = reinterpret_cast<uint8_t*>(old.positionPtr(0,y));
        uint8_t* targetPtr = reinterpret_cast<uint8_t*>(positionPtr(0,y));
        for(int x = 0 ; x < width; ++x){
            int srcOffset = x * old.Format().bytesPerPixel();
            int targetOffset = x * format.bytesPerPixel();
            for(int i = 0; i < format.bytesPerPixel(); ++ i){
                targetPtr[targetOffset + i] = srcPtr[srcOffset + i];
            }
        }

    }

}

//======================================================


ImageFormat &Image::Format()
{
    return format;
}

const ImageFormat &Image::Format() const
{
    return format;
}



size_t Image::getSize(){
    return height*pitch;
}

std::ostream& operator<<(std::ostream& os, const Image& f)
{
    os << "Image " << f.width << "x" << f.height << " " << f.Format();
    return os;
}


bool loadImage(const std::string &path, Image &outImage)
{
    bool erg = false;

    //use libfreeimage if available, libpng otherwise
#ifdef SAIGA_USE_FREEIMAGE
    erg = FIP::load(path,outImage,0);
//    fipImage img;
//    erg = img.load(path.c_str());
//    if(erg){
//        ImageConverter::convert(img,outImage);
//    }
#else
#ifdef SAIGA_USE_PNG
    PNG::PngImage pngimg;
    erg = PNG::readPNG( &pngimg,path);
    if(erg)
        ImageConverter::convert(pngimg,outImage);
#endif
#endif

    if(erg){
#ifndef SAIGA_RELEASE
        std::cout<<"Loaded: "<< path << " " << outImage << std::endl;
#endif
    }else{
        std::cout << "Error: Could not load image: " << path << std::endl;
        SAIGA_ASSERT(0);
    }

    return erg;
}

bool saveImage(const std::string &path, const Image &image)
{
    bool erg = false;

    //use libfreeimage if available, libpng otherwise
#ifdef SAIGA_USE_FREEIMAGE
//    fipImage fipimage;
//    ImageConverter::convert(image,fipimage);
//    erg = fipimage.save(path.c_str());
    erg = FIP::save(path,image);
#else
#ifdef SAIGA_USE_PNG
    PNG::PngImage pngimg;
    ImageConverter::convert(image,pngimg);
    erg = PNG::writePNG(&pngimg,path);
#endif
#endif

    if(erg){
        std::cout<<"Saved: "<< path << " " << image << std::endl;
    }else{
        std::cout << "Error: Could not save Image: " << path << std::endl;
        SAIGA_ASSERT(0);
    }


    return erg;
}



bool saveHSV(const std::string& path, ImageView<float> img, float vmin, float vmax)
{
    std::vector<float> cpy(img.width*img.height);
    ImageView<float> vcpy(img.height,img.width,cpy.data());
    img.copyTo(vcpy);

    vcpy.add(-vmin);
    vcpy.multWithScalar(float(1) / (vmax-vmin));

    TemplatedImage<3,8> simg(img.width,img.height);
    for(int i = 0; i < img.height; ++i)
    {
        for(int j = 0; j < img.width; ++j)
        {
            float f = glm::clamp(vcpy(i,j),0.0f,1.0f);

//            vec3 hsv = vec3(f,1,1);
            vec3 hsv(f* (240.0/360.0),1,1);
            Saiga::Color c (Color::hsv2rgb(hsv));
//            unsigned char c = Saiga::iRound(f * 255.0f);
            simg(j,i).r = c.r;
            simg(j,i).g = c.g;
            simg(j,i).b = c.b;
        }
    }
    return saveImage(path,simg);
}


bool save(const std::string& path, ImageView<float> img, float vmin, float vmax)
{
    std::vector<float> cpy(img.width*img.height);
    ImageView<float> vcpy(img.height,img.width,cpy.data());
    img.copyTo(vcpy);

    vcpy.add(-vmin);
    vcpy.multWithScalar(float(1) / (vmax-vmin));

    TemplatedImage<1,8> simg(img.width,img.height);
    for(int i = 0; i < img.height; ++i)
    {
        for(int j = 0; j < img.width; ++j)
        {
            float f = glm::clamp(vcpy(i,j),0.0f,1.0f);
            unsigned char c = Saiga::iRound(f * 255.0f);
            simg(j,i).r = c;
        }
    }
    return saveImage(path,simg);
}

}