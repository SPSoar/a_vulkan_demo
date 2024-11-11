#include "manager/SSRLightManagerIn.h"

_SSR_STUDIO_BEGIN

_SSR_UNIQUE_IMPL_IN(SSRLightManager)

_SSR_LIGHT_MANAGER_PUB_MEMBER_FUNC_IMPL(SSRLightManager)

//_SSR_LIGHT_MANAGER_IN_PUB_MEMBER_FUNC_IMPL(SSRLightManagerIn)

auto SSRLightManager::Create() -> SSRLightManagerUPtr
{
    return _SSR_MAKE_UNIQUE_IN(SSRLightManager);
}


_SSR_END