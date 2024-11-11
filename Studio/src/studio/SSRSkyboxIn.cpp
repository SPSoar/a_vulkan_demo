#include "studio/SSRSkyboxIn.h"

_SSR_STUDIO_BEGIN

_SSR_UNIQUE_IMPL_IN(SSRSkybox)

_SSR_SKYBOX_PUB_MEMBER_FUNC_IMPL(SSRSkybox)

//_SSR_SKYBOX_IN_PUB_MEMBER_FUNC_IMPL(SSRSkyboxIn)

auto SSRSkybox::Create() -> SSRSkyboxUPtr
{
    return _SSR_MAKE_UNIQUE_IN(SSRSkybox);
}

auto SSRSkyboxIn::init() noexcept -> SSRSkybox& 
{
    return *this;
}

auto SSRSkyboxIn::init_enviroment(_SSR_CORE::SSRImagePtr cubeMap) noexcept -> SSRSkybox&
{
    return *this;
}
auto SSRSkyboxIn::init_intensity(SFLOAT32 intensity) noexcept -> SSRSkybox&
{
    return *this;
}
auto SSRSkyboxIn::init_color(const SSRColor& color) noexcept -> SSRSkybox&
{
    mColor = color;
    return *this;
}
auto SSRSkyboxIn::init_show_sun(SBOOL show) noexcept -> SSRSkybox&
{
    return *this;
}

auto SSRSkyboxIn::setup() noexcept -> SSRSkybox&
{
    return *this;
}

auto SSRSkyboxIn::commit(SSRStudioPtr studio) noexcept -> void
{
}


_SSR_END