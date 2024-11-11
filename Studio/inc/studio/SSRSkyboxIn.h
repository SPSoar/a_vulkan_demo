#pragma once
#ifndef SSR_SSRSKYBOXIN_H_
#define SSR_SSRSKYBOXIN_H_


#include "../common/SSRStudioHeadIn.h"

_SSR_STUDIO_BEGIN

class SSRSkyboxIn : public SSRSkybox
{
    _SSR_DELETER_FRIEND_DEF(SSRSkybox)
protected:
    SSRSkyboxIn() noexcept = default;
    ~SSRSkyboxIn()noexcept = default;
public:
    _SSR_CLASS_DELETE_COPY(SSRSkyboxIn);
    _SSR_CLASS_DELETE_MOVE(SSRSkyboxIn);
    _SSR_SKYBOX_PUB_MEMBER_FUNC_DEF()
    _SSR_CLASS_MEMBER_GET_FUNC(SSRColor, Color, color,)
protected:
   SSRColor mColor{};
};


_SSR_END

#endif

