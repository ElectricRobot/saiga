#include "saiga/opengl/texture/imageFormat.h"
#include "saiga/opengl/texture/image2.h"
#include "saiga/util/assert.h"
#include <iostream>

void asksdfkg(){
    Texel<2,8,ImageElementFormat::SignedIntegral> t;
    t.r = 0.5f;
    t.g = 1.0f;

    //    TemplatedImage<2,8,ImageFormat::SignedIntegral,false> i(5,5);
    TemplatedImage<3,8,ImageElementFormat::UnsignedNormalized,false> i(5,5);
    i.create();
    auto texel = i.getTexel(3,4);
    i.flipRB();
}

ImageFormat::ImageFormat(int channels, int bitDepth, ImageElementFormat elementFormat, bool srgb)
{
    //use setters instead of initializer list, because the setters test if the format satisfies certain conditions.
    setChannels(channels);
    setBitDepth(bitDepth);
    setElementFormat(elementFormat);
    setSrgb(srgb);
}

int ImageFormat::getChannels() const
{
    return channels;
}

void ImageFormat::setChannels(int value)
{
    assert(value>0 && value<=4);
    channels = value;
}

int ImageFormat::getBitDepth() const
{
    return bitDepth;
}

void ImageFormat::setBitDepth(int value)
{
    assert(value%8==0);
    assert(value>=8 && value<=32);
    bitDepth = value;
}

bool ImageFormat::getSrgb() const
{
    return srgb;
}

void ImageFormat::setSrgb(bool value)
{
    srgb = value;
}


ImageElementFormat ImageFormat::getElementFormat() const
{
    return elementFormat;
}

void ImageFormat::setElementFormat(const ImageElementFormat &value)
{
    elementFormat = value;
}


int ImageFormat::bytesPerChannel(){
    return bitDepth/8;
}

int ImageFormat::bytesPerPixel(){
    return channels*bytesPerChannel();
}

int ImageFormat::bitsPerPixel(){
    return channels*bitDepth;
}


//=====================================================================================



GLenum ImageFormat::getGlInternalFormat() const
{

    static const GLenum UnsignedNormalizedFormats[4][3] {
        {GL_R8,     GL_R16,     GL_INVALID_ENUM},
        {GL_RG8,    GL_RG16,    GL_INVALID_ENUM},
        {GL_RGB8,   GL_RGB16,   GL_INVALID_ENUM},
        {GL_RGBA8,  GL_RGBA16,  GL_INVALID_ENUM}
    };
    static const GLenum SignedNormalizedFormats[4][3] {
        {GL_R8_SNORM,     GL_R16_SNORM,     GL_INVALID_ENUM},
        {GL_RG8_SNORM,    GL_RG16_SNORM,    GL_INVALID_ENUM},
        {GL_RGB8_SNORM,   GL_RGB16_SNORM,   GL_INVALID_ENUM},
        {GL_RGBA8_SNORM,  GL_RGBA16_SNORM,  GL_INVALID_ENUM}
    };
    static const GLenum UnsignedIntegralFormats[4][3] {
        {GL_R8UI,     GL_R16UI,     GL_R32UI},
        {GL_RG8UI,    GL_RG16UI,    GL_RG32UI},
        {GL_RGB8UI,   GL_RGB16UI,   GL_RGB32UI},
        {GL_RGBA8UI,  GL_RGBA16UI,  GL_RGBA32UI}
    };
    static const GLenum SignedIntegralFormats[4][3] {
        {GL_R8I,     GL_R16I,     GL_R32I},
        {GL_RG8I,    GL_RG16I,    GL_RG32I},
        {GL_RGB8I,   GL_RGB16I,   GL_RGB32I},
        {GL_RGBA8I,  GL_RGBA16I,  GL_RGBA32I}
    };
    static const GLenum FloatingPointFormats[4][3] {
        {GL_INVALID_ENUM,  GL_R16F,     GL_R32F},
        {GL_INVALID_ENUM,  GL_RG16F,    GL_RG32F},
        {GL_INVALID_ENUM,  GL_RGB16F,   GL_RGB32F},
        {GL_INVALID_ENUM,  GL_RGBA16F,  GL_RGBA32F}
    };

    if(srgb){
        //currently there are only 2 srgb formats.
        assert(bitDepth == 8);
        assert(channels == 3 || channels == 4);
        assert(elementFormat == ImageElementFormat::UnsignedNormalized);
        return (channels==3)? GL_SRGB8 : GL_SRGB8_ALPHA8;
    }


    int i = channels - 1;
    int j = (bitDepth / 8) - 1;

    GLenum internalFormat = GL_INVALID_ENUM;

    switch (elementFormat){
    case ImageElementFormat::UnsignedNormalized:
        internalFormat = UnsignedNormalizedFormats[i][j];
        break;
    case ImageElementFormat::SignedNormalized:
        internalFormat = SignedNormalizedFormats[i][j];
        break;
    case ImageElementFormat::UnsignedIntegral:
        internalFormat = UnsignedIntegralFormats[i][j];
        break;
    case ImageElementFormat::SignedIntegral:
        internalFormat = SignedIntegralFormats[i][j];
        break;
    case ImageElementFormat::FloatingPoint:
        internalFormat = FloatingPointFormats[i][j];
        break;
    }

    if(internalFormat == GL_INVALID_ENUM){
        std::cout << "Invalid Image Format. " << (*this) << std::endl;
        assert(0);
    }


    return internalFormat;
}

GLenum ImageFormat::getGlFormat() const
{
    static const GLenum formats[] = {GL_RED,GL_RG,GL_RGB,GL_RGBA};
    return formats[channels-1];
}

GLenum ImageFormat::getGlType() const
{
    if(elementFormat == ImageElementFormat::FloatingPoint)
        return GL_FLOAT;

    bool isSigned = (elementFormat==ImageElementFormat::SignedIntegral || elementFormat==ImageElementFormat::SignedNormalized) ? true : false;

    switch(bitDepth){
    case 8:
        return (isSigned) ? GL_BYTE : GL_UNSIGNED_BYTE;
    case 16:
        return (isSigned) ? GL_SHORT : GL_UNSIGNED_SHORT;
    case 32:
        return (isSigned) ? GL_INT : GL_UNSIGNED_INT;
    default:
        std::cout<<"Bit depth not supported: "<<(*this)<<std::endl;
        assert(0);
    }
}

std::ostream& operator<<(std::ostream& os, const ImageFormat& f){
    std::string efstr;
    switch (f.getElementFormat()){
    case ImageElementFormat::UnsignedNormalized:
        efstr = "UnsignedNormalized";
        break;
    case ImageElementFormat::SignedNormalized:
        efstr = "SignedNormalized";
        break;
    case ImageElementFormat::UnsignedIntegral:
        efstr = "UnsignedIntegral";
        break;
    case ImageElementFormat::SignedIntegral:
        efstr = "SignedIntegral";
        break;
    case ImageElementFormat::FloatingPoint:
        efstr = "FloatingPoint";
        break;
    }
    os << "ImageFormat"<<" channels="<<f.getChannels()<<" bitDepth="<<f.getBitDepth()<<" elementFormat="<<efstr<<" srgb="<<f.getSrgb();
          return os;
}

