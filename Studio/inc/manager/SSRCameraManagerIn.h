#pragma once
#ifndef SSR_SSRCAMERAMANAGERIN_H_
#define SSR_SSRCAMERAMANAGERIN_H_


#include "../common/SSRStudioHeadIn.h"

_SSR_STUDIO_BEGIN

class SSRCameraManagerIn : public SSRCameraManager
{
    _SSR_DELETER_FRIEND_DEF(SSRCameraManager)
protected:
    SSRCameraManagerIn() noexcept = default;
    ~SSRCameraManagerIn()noexcept = default;
public:
    _SSR_CLASS_DELETE_COPY(SSRCameraManagerIn);
    _SSR_CLASS_DELETE_MOVE(SSRCameraManagerIn);
    _SSR_CAMERA_MANAGER_PUB_MEMBER_FUNC_DEF()

};


_SSR_END

#endif

