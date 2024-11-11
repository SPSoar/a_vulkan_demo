#pragma once
#ifndef SSR_SSRTEXTUREMANAGER_H_
#define SSR_SSRTEXTUREMANAGER_H_


#include "../common/SSRStudioHeadIn.h"

_SSR_STUDIO_BEGIN

class SSRTextureManager
{
protected:
    SSRTextureManager() noexcept = default;
    ~SSRTextureManager()noexcept = default;
public:
    _SSR_CLASS_DELETE_COPY(SSRTextureManager);
    _SSR_CLASS_DELETE_MOVE(SSRTextureManager);

};


_SSR_END

#endif

