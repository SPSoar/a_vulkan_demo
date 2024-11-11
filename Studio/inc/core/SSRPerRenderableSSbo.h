#pragma once
#ifndef SSR_SSRPERRENDERABLEUNIFORM_H_
#define SSR_SSRPERRENDERABLEUNIFORM_H_

#include "../common/SSRStudioHeadIn.h"

_SSR_STUDIO_BEGIN


struct alignas(256) SSRPerRenderableBib
{
    SMAT4X4::Container mModel{};
    float              mDepth{ 1.f };
};

_SSR_SASSERT(sizeof(SSRPerRenderableBib) == 256, _SSR_RDERROR);

_SSR_PR_DEF(SSRPerRenderableSSbo);
_SSR_PR_DEF(SSRRenderBlock);

class SSRPerRenderableSSbo
{
    friend class SSRRenderBlock;
protected:
    SSRPerRenderableSSbo() noexcept = default;
    ~SSRPerRenderableSSbo()noexcept = default;
public:
    _SSR_CLASS_DELETE_COPY(SSRPerRenderableSSbo);
    _SSR_CLASS_DELETE_MOVE(SSRPerRenderableSSbo);

};


_SSR_END

#endif

