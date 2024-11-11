#ifndef SSR_STUDIO_INCLUDE
#error "DO NOT include the header directly, use #include \"SSRStudioHead.h\" instead"
#endif

#pragma once
#ifndef SSR_SSRSKYBOX_H_
#define SSR_SSRSKYBOX_H_

_SSR_STUDIO_BEGIN


_SSR_UNIQUE_DEF_BASE(SSRSkybox, _SSR_STUDIO_EXPORT);

#define _SSR_SKYBOX_PUB_MEMBER_FUNC_DEF()\
    auto init() noexcept -> SSRSkybox&;\
    auto init_enviroment(_SSR_CORE::SSRImagePtr cubeMap) noexcept -> SSRSkybox&;\
    auto init_intensity(SFLOAT32 intensity) noexcept -> SSRSkybox&;\
    auto init_color(const SSRColor& color) noexcept -> SSRSkybox&;\
    auto init_show_sun(SBOOL show) noexcept -> SSRSkybox&;\
    auto setup() noexcept -> SSRSkybox&;\
    auto commit(SSRStudioPtr studio) noexcept -> void;\

#define _SSR_SKYBOX_PUB_MEMBER_FUNC_IMPL(Class)\
    _SSR_CLASS_BASE_FUNC_OUT_NOEXCEPT(Class, SSRSkybox&, init,,)\
    _SSR_CLASS_BASE_FUNC_OUT_NOEXCEPT(Class, SSRSkybox&, init_enviroment, _SSR_CORE::SSRImagePtr, cubeMap)\
    _SSR_CLASS_BASE_FUNC_OUT_NOEXCEPT(Class, SSRSkybox&, init_intensity, SFLOAT32, intensity)\
    _SSR_CLASS_BASE_FUNC_OUT_NOEXCEPT(Class, SSRSkybox&, init_color, const SSRColor&, color)\
    _SSR_CLASS_BASE_FUNC_OUT_NOEXCEPT(Class, SSRSkybox&, init_show_sun, SBOOL, show )\
    _SSR_CLASS_BASE_FUNC_OUT_NOEXCEPT(Class, SSRSkybox&, setup,,)\
    _SSR_CLASS_BASE_FUNC_OUT_NOEXCEPT(Class, void, commit, SSRStudioPtr, studio)\

#define _SSR_SKYBOX_IN_PUB_MEMBER_FUNC_IMPL(Class)\
    //_SSR_CLASS_DERIVE_FUNC_OUT_CONST_NOEXCEPT(Class, SSRToken&, get_token, SSRToken::E_TokenType, tokenType)

_SSR_DEF_BASE(SSRStudio);

class _SSR_STUDIO_EXPORT SSRSkybox
{

protected:
    SSRSkybox() noexcept = default;
    ~SSRSkybox()         = default;
public:
    _SSR_CLASS_DELETE_COPY(SSRSkybox);
    _SSR_CLASS_DELETE_MOVE(SSRSkybox);
    _SSR_SKYBOX_PUB_MEMBER_FUNC_DEF()

public:
    static auto Create() -> SSRSkyboxUPtr;
};


_SSR_END

#endif

