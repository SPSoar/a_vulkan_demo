#pragma once
#ifndef SSR_SSRLIGHTUNIFORM_H_
#define SSR_SSRLIGHTUNIFORM_H_

#include "../common/SSRStudioHeadIn.h"

_SSR_STUDIO_BEGIN

_SSR_PR_DEF(SSRLightUniform);
_SSR_PR_DEF(SSRSceneIn);

class SSRLightUniform
{
    friend class SSRSceneIn;
protected:
    SSRLightUniform() noexcept = default;
    ~SSRLightUniform()noexcept = default;
public:
    _SSR_CLASS_DELETE_COPY(SSRLightUniform);
    _SSR_CLASS_DELETE_MOVE(SSRLightUniform);

};

_SSR_END

#endif

