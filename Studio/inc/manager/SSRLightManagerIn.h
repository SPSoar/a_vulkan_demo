#pragma once
#ifndef SSR_SSRLIGHTMANAGERIN_H_
#define SSR_SSRLIGHTMANAGERIN_H_


#include "../common/SSRStudioHeadIn.h"

_SSR_STUDIO_BEGIN

class SSRLightManagerIn : public SSRLightManager
{
    _SSR_DELETER_FRIEND_DEF(SSRLightManager)
protected:
    SSRLightManagerIn() noexcept = default;
    ~SSRLightManagerIn()noexcept = default;
public:
    _SSR_CLASS_DELETE_COPY(SSRLightManagerIn);
    _SSR_CLASS_DELETE_MOVE(SSRLightManagerIn);
    _SSR_LIGHT_MANAGER_PUB_MEMBER_FUNC_DEF()

};


_SSR_END

#endif

