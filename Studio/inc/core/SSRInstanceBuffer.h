#pragma once
#ifndef SSR_SSRINSTANCEBUFFER_H_
#define SSR_SSRINSTANCEBUFFER_H_

#include "../common/SSRStudioHeadIn.h"

_SSR_STUDIO_BEGIN

class SSRInstanceBuffer
{
protected:
    SSRInstanceBuffer()  noexcept;
    ~SSRInstanceBuffer() noexcept;
public:
    _SSR_CLASS_DELETE_COPY(SSRInstanceBuffer);
    _SSR_CLASS_DELETE_MOVE(SSRInstanceBuffer);

};


_SSR_END

#endif

