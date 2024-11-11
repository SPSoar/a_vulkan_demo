#include "studio/SSRLightIn.h"

_SSR_STUDIO_BEGIN

_SSR_UNIQUE_IMPL_IN(SSRLight)

_SSR_LIGHT_PUB_MEMBER_FUNC_IMPL(SSRLight)

//_SSR_LIGHT_IN_PUB_MEMBER_FUNC_IMPL(SSRLightIn)

auto SSRLight::Create() -> SSRLightUPtr
{
    return _SSR_MAKE_UNIQUE_IN(SSRLight);
}

SSRLightIn::SSRLightIn() noexcept
{

}

SSRLightIn::~SSRLightIn() noexcept
{

}

_SSR_END