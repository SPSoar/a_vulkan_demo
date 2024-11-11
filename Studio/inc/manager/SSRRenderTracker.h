#pragma once
#ifndef SSR_SSRRENDERTRACKER_H_
#define SSR_SSRRENDERTRACKER_H_

#include "../common/SSRStudioHeadIn.h"

_SSR_STUDIO_BEGIN

class SSRRenderTracker
{
public:
    SSRRenderTracker() noexcept = default;
    ~SSRRenderTracker()noexcept = default;
    _SSR_CLASS_DELETE_COPY(SSRRenderTracker);
    _SSR_CLASS_DELETE_MOVE(SSRRenderTracker);
public:
    auto init(SSRStudioRef& studio) noexcept -> void;
    auto uninit(SSRStudioRef& studio) noexcept -> void;
};


_SSR_END

#endif

