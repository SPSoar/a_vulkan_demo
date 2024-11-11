#pragma once
#ifndef SSR_SSRTRANSFORMMANAGERIN_H_
#define SSR_SSRTRANSFORMMANAGERIN_H_


#include "../common/SSRStudioHeadIn.h"

_SSR_STUDIO_BEGIN

class SSRTransformManagerIn : public SSRTransformManager
{
    _SSR_DELETER_FRIEND_DEF(SSRTransformManager)
protected:
    SSRTransformManagerIn() noexcept = default;
    ~SSRTransformManagerIn()noexcept = default;
public:
    _SSR_CLASS_DELETE_COPY(SSRTransformManagerIn);
    _SSR_CLASS_DELETE_MOVE(SSRTransformManagerIn);
    _SSR_TRANSFORM_MANAGER_PUB_MEMBER_FUNC_DEF()

};


_SSR_END

#endif

