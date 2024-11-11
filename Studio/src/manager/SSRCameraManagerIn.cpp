#include "manager/SSRCameraManagerIn.h"

_SSR_STUDIO_BEGIN

_SSR_UNIQUE_IMPL_IN(SSRCameraManager)

_SSR_CAMERA_MANAGER_PUB_MEMBER_FUNC_IMPL(SSRCameraManager)

//_SSR_CAMERA_MANAGER_IN_PUB_MEMBER_FUNC_IMPL(SSRCameraManagerIn)

auto SSRCameraManager::Create() -> SSRCameraManagerUPtr
{
    return _SSR_MAKE_UNIQUE_IN(SSRCameraManager);
}


_SSR_END