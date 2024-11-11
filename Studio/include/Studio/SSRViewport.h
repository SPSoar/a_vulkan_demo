#ifndef SSR_STUDIO_INCLUDE
#error "DO NOT include the header directly, use #include \"SSRStudioHead.h\" instead"
#endif

#pragma once
#ifndef SSR_SSRVIEWPORT_H_
#define SSR_SSRVIEWPORT_H_

_SSR_STUDIO_BEGIN

struct SSRViewport
{
    SUINT32 x{0};
    SUINT32 y{0};
    SUINT32 width{0};
    SUINT32 height{0};

    auto operator==(const SSRViewport& other) noexcept -> bool
    {
        return x == other.x && y == other.y && width == other.width && height == other.height;
    }
    auto operator!=(const SSRViewport& other) noexcept -> bool
    {
        return !(*this == other);
    }
};

_SSR_END

#endif

