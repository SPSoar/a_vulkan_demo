#ifndef S_MANGASTUDIOENUM_H_
#define S_MANGASTUDIOENUM_H_
#include "common.h"
enum class Backend : spuint {
    VULKAN = 0,   
};

enum class Platform : spuint {
    QT_WINDOWS = 0,
    QT_MAC = 1,
    QT_ANDROID = 2,
    QT_IOS = 3
};

enum class ImgFormat : spuint {

    R8G8B8A8_SRGB = 0,
    R8G8B8A8_UINT = 1,
    R8G8B8A8_SINT = 2,
    R8G8B8A8_UNORM = 3,
    R8G8B8A8_SNORM = 4,

    R8_SRGB = 10,
    R8_UINT = 11,
    R8_SINT = 12,
    R8_UNORM = 13,
    R8_SNORM = 14,

};

enum class ColorSpace : spuint {
    SRGB_NONLINEAR = 0,
};

enum class PresentMode : spuint {
    IMMEDIATE = 0,
    DOUBLE_BUFFER = 1,
    TRIPLE_BUFFER = 2
};


struct Canvas 
{
    ImgFormat format = ImgFormat::R8G8B8A8_SRGB;
    ColorSpace colorSpace = ColorSpace::SRGB_NONLINEAR;
    PresentMode presentMode = PresentMode::TRIPLE_BUFFER;

};

enum class WorkFlowType : spuint
{
    None = 0,
    Forward = 1,
    Deferred = 2,
    RTRT = 3
};


#endif
