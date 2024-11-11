#pragma once
#ifndef SSR_SSRLIGHTIN_H_
#define SSR_SSRLIGHTIN_H_

#include "../common/SSRStudioHeadIn.h"

_SSR_STUDIO_BEGIN

class SSRLightIn : public SSRLight
{
    _SSR_DELETER_FRIEND_DEF(SSRLight)
protected:
    SSRLightIn()  noexcept;
    ~SSRLightIn() noexcept;
public:
    _SSR_CLASS_DELETE_COPY(SSRLightIn);
    _SSR_CLASS_DELETE_MOVE(SSRLightIn);
    _SSR_LIGHT_PUB_MEMBER_FUNC_DEF()

};


_SSR_END

#endif

