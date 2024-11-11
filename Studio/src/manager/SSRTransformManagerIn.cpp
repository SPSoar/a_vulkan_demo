#include "manager/SSRTransformManagerIn.h"

_SSR_STUDIO_BEGIN

_SSR_UNIQUE_IMPL_IN(SSRTransformManager)

_SSR_TRANSFORM_MANAGER_PUB_MEMBER_FUNC_IMPL(SSRTransformManager)

//_SSR_TRANSFORM_MANAGER_IN_PUB_MEMBER_FUNC_IMPL(SSRTransformManagerIn)

auto SSRTransformManager::Create() -> SSRTransformManagerUPtr
{
    return _SSR_MAKE_UNIQUE_IN(SSRTransformManager);
}


_SSR_END