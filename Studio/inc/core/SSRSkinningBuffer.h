#pragma once
#ifndef SSR_SSRSKINNINGBUFFER_H_
#define SSR_SSRSKINNINGBUFFER_H_

#include "../common/SSRStudioHeadIn.h"

_SSR_STUDIO_BEGIN

class SSRSkinningBuffer 
{
protected:
    SSRSkinningBuffer()  noexcept;
    ~SSRSkinningBuffer() noexcept;
public:
    _SSR_CLASS_DELETE_COPY(SSRSkinningBuffer);
    _SSR_CLASS_DELETE_MOVE(SSRSkinningBuffer);

};


_SSR_END

#endif

