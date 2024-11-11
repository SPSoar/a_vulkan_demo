#pragma once
#ifndef SSR_SSRSHADOWUNIFORM_H_
#define SSR_SSRSHADOWUNIFORM_H_

#include "../common/SSRStudioHeadIn.h"

_SSR_STUDIO_BEGIN

_SSR_PR_DEF(SSRShadowUniform);
_SSR_PR_DEF(SSRViewIn);

class SSRShadowUniform
{
    friend class SSRViewIn;
protected:
    SSRShadowUniform() noexcept = default;
    ~SSRShadowUniform()noexcept = default;
public:
    _SSR_CLASS_DELETE_COPY(SSRShadowUniform);
    _SSR_CLASS_DELETE_MOVE(SSRShadowUniform);

};


_SSR_END

#endif

